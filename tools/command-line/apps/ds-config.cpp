
#// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file example.cxx
/// \brief An example on the use of ALF as a library
///
/// \author Kostas Alexopoulos (kostas.alexopoulos@cern.ch)

#include <chrono>
#include <thread>
#include <vector>

#define BUILD_FOR_CRU 1
#define BUILD_FOR_ALF 1

#include "../src/Sca/cern/hdlc_cern_me.hpp"
#include "../src/Sca/hdlc_alf.hpp"
#include "../src/Sca/ds_fec.hpp"
#include "../src/Sca/factory.hpp"

using namespace o2::alf;


int main(int argc, char** argv)
{
  bool success = true;

  o2::alf::roc::PciAddress pci_addr1(argv[1]);
  o2::alf::roc::PciAddress pci_addr2(argv[2]);
  o2::alf::roc::Parameters::CardIdType cardId1 = pci_addr1;
  o2::alf::roc::Parameters::CardIdType cardId2 = pci_addr2;
  auto card1 = roc::findCard(cardId1);
  auto card2 = roc::findCard(cardId2);
  std::unique_ptr<LlaSession> llaSession = std::make_unique<LlaSession>("MCH", card1.serialId);

  llaSession->start();

/*#if !defined(BUILD_FOR_ALF)
  std::unique_ptr<common::Bar> bar;
  try {
    bar.reset(common::BarFactory::makeBar(argv[1], 2));
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
#endif*/

  std::ifstream istr(argv[3]);
  char tstr[501];
  //while( istr.getline(tstr,500) ) {
  while( !istr.eof() ) {
    //std::cout<<"line: "<<tstr<<std::endl;
    int fec_idx, ds_idx;
    std::string cf0, cf1;
    //char cf0[501], cf1[501];
    //sscanf(tstr,"%d %d %s %s",&fec_idx, &ds_idx, cf0, cf1);
    istr >> fec_idx >> ds_idx >> cf0 >> cf1;
    if( !istr ) break;
    std::cout<<"fec_idx="<<fec_idx<<"  ds_idx="<<ds_idx<<"  \""<<cf0<<"\"  \""<<cf1<<"\""<<std::endl;
    if( !istr ) break;

    //auto sca = o2::alf::Sca(cardId, fec_idx);
    //sca.svlReset();
    //sca.svlConnect();

    try {
#ifdef BUILD_FOR_ALF
      o2::alf::roc::Parameters::CardIdType cardId = (fec_idx < 12) ? cardId1 : cardId2;
      fec_idx = fec_idx % 12;
      std::unique_ptr<cru::HdlcAlf> hdlc_core(new cru::HdlcAlf(cardId, fec_idx, true));
      //cru::HdlcAlf* hdlc_core = new cru::HdlcAlf(cardId, fec_idx, true);
#else
      std::unique_ptr<common::Bar> bar;
      try {
        bar.reset(common::BarFactory::makeBar(argv[1], 2));
      } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
      }
      std::unique_ptr<common::HdlcCore> hdlc_core(common::HdlcFactory::makeHdlcCore(*(bar.get()), fec_idx, true));
#endif
      //std::cout<<"HDLC core created\n";
      hdlc_core->rst();
      //std::cout<<"HDLC core resetted\n";
      //continue;
      
      DsFec fec(*(hdlc_core.get()), 0);
      //DsFec fec(*(hdlc_core), 0);
      //std::cout<<"FEC board created\n";
      fec.init();
      //exit(0);
      
      //std::cout << "Reading the FEC unique ID" << std::endl;
      //fec.readUniqueID(&std::cout);
      //continue;

      bool readback = true;
      int retries = 3; //10;

      for(int iter = 0; iter < 1; iter++) {
        //std::cout << "Loading SAMPA registers" << std::endl;
        if( !fec.sampaConfigure(cf0, ds_idx*2, retries, readback, &std::cout) ) {
          std::cout << "ERROR: Configuration of SAMPA chip " << fec_idx << ":" << ds_idx*2
              << " (LINK " << fec_idx << " J" << ds_idx/5 + 1 << " DS" << (ds_idx%5) << ") failed" << std::endl << std::endl;
          success = false;
          break;
        }
        //std::cout<<"SAMPA "<< fec_idx << ":" << ds_idx*2<<" succesfully configured\n";
        if( !fec.sampaConfigure(cf1, ds_idx*2+1, retries, readback, &std::cout) ) {
          std::cout << "ERROR: Configuration of SAMPA chip " << fec_idx << ":" << ds_idx*2+1
              << " (LINK " << fec_idx << " J" << ds_idx/5 + 1 << " DS" << (ds_idx%5) << ") failed" << std::endl << std::endl;
          success = false;
          break;
        }
        //std::cout<<"SAMPA "<< fec_idx << ":" << ds_idx*2+1<<" succesfully configured\n";
      }
      //delete hdlc_core;
      //if( !success ) break;
    }
    catch (std::runtime_error& e) {
      std::cerr << e.what() << std::endl;
      success = false;
      //exit(100);
    }
  }
  printf("success: %d\n", success);

  if( !success ) return 1;
  return 0;
}
