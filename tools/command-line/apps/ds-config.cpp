// Copyright CERN and copyright holders of ALICE O2. This software is
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
#include <mutex>
#include <vector>

#define BUILD_FOR_CRU 1
#define BUILD_FOR_ALF 1
#define PARALLEL 1

#include "../src/Sca/cern/hdlc_cern_me.hpp"
#include "../src/Sca/hdlc_alf.hpp"
#include "../src/Sca/ds_fec.hpp"
#include "../src/Sca/factory.hpp"

using namespace o2::alf;

static bool success = {true};

static std::mutex gCardIdMutex;

static int sRetries = 2;

struct DsConfig
{
  DsConfig(int id, std::string c0, std::string c1): dsId(id), cf0(c0), cf1(c1) { }
  int dsId;
  std::string cf0;
  std::string cf1;
};


void fecConfig(int fec_idx, std::string pci_addr_str1, std::string pci_addr_str2, std::vector<DsConfig>* dsConfigVec)
{
  o2::alf::roc::PciAddress pci_addr1(pci_addr_str1);
  o2::alf::roc::PciAddress pci_addr2(pci_addr_str2);
  o2::alf::roc::Parameters::CardIdType cardId1 = pci_addr1;
  o2::alf::roc::Parameters::CardIdType cardId2 = pci_addr2;
  //return;
  if (dsConfigVec->empty()) { return; }
  try {
#ifdef BUILD_FOR_ALF
    gCardIdMutex.lock();
    o2::alf::roc::Parameters::CardIdType& cardId = (fec_idx < 12) ? cardId1 : cardId2;
    //return;
    int linkId = fec_idx % 12;
    std::unique_ptr<cru::HdlcAlf> hdlc_core(new cru::HdlcAlf(cardId, linkId, true));
    //cru::HdlcAlf* hdlc_core = new cru::HdlcAlf(cardId, fec_idx, true);
    gCardIdMutex.unlock();
#else
    std::unique_ptr<common::Bar> bar;
    try {
      bar.reset(common::BarFactory::makeBar(pci_addr_str1, 2));
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      exit(1);
    }
    std::unique_ptr<common::HdlcCore> hdlc_core(common::HdlcFactory::makeHdlcCore(*(bar.get()), fec_idx, true));
#endif
    //return;
    //std::cout<<"HDLC core created\n";
    hdlc_core->rst();
    //std::cout<<"HDLC core resetted\n";
    //return;

    DsFec fec(*(hdlc_core.get()), 0);
    //DsFec fec(*(hdlc_core), 0);
    //std::cout<<"FEC board created\n";
    //return;
    fec.init();
    //exit(0);

    //std::cout << "Reading the FEC unique ID" << std::endl;
    //fec.readUniqueID(&std::cout);
    //continue;

    bool readback = true;
    int retries = sRetries; //10;

    for(auto cfg : *dsConfigVec) {

      auto ds_idx = cfg.dsId;
      auto cf0 = cfg.cf0;
      auto cf1 = cfg.cf1;

      for(int iter = 0; iter < 1; iter++) {
        //std::cout << "Loading SAMPA registers" << std::endl;
        if( !fec.sampaConfigure(cf0, ds_idx*2, retries, readback, &std::cout) ) {
          std::cout << "ERROR: Configuration of SAMPA chip " << fec_idx << ":" << ds_idx*2
		    << " (LINK " << fec_idx << " S" << (fec_idx%6) + 1 << " J" << ds_idx/5 + 1 << " DS" << (ds_idx%5) << ") FAILED" << std::endl << std::endl;
          success = false;
          break;
        }
        //std::cout<<"SAMPA "<< fec_idx << ":" << ds_idx*2<<" succesfully configured\n";
        if( !fec.sampaConfigure(cf1, ds_idx*2+1, retries, readback, &std::cout) ) {
          std::cout << "ERROR: Configuration of SAMPA chip " << fec_idx << ":" << ds_idx*2+1
		    << " (LINK " << fec_idx << " S" << (fec_idx%6) + 1 << " J" << ds_idx/5 + 1 << " DS" << (ds_idx%5) << ") FAILED" << std::endl << std::endl;
          success = false;
          break;
        }
        //std::cout<<"SAMPA "<< fec_idx << ":" << ds_idx*2+1<<" succesfully configured\n";
      }
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


void fecDumpConfiguration(int fec_idx, std::string pci_addr_str1, std::string pci_addr_str2, std::vector<DsConfig>* dsConfigVec)
{
  o2::alf::roc::PciAddress pci_addr1(pci_addr_str1);
  o2::alf::roc::PciAddress pci_addr2(pci_addr_str2);
  o2::alf::roc::Parameters::CardIdType cardId1 = pci_addr1;
  o2::alf::roc::Parameters::CardIdType cardId2 = pci_addr2;
  //return;
  if (dsConfigVec->empty()) { return; }
  try {
#ifdef BUILD_FOR_ALF
    gCardIdMutex.lock();
    o2::alf::roc::Parameters::CardIdType& cardId = (fec_idx < 12) ? cardId1 : cardId2;
    //return;
    int linkId = fec_idx % 12;
    std::unique_ptr<cru::HdlcAlf> hdlc_core(new cru::HdlcAlf(cardId, linkId, true));
    //cru::HdlcAlf* hdlc_core = new cru::HdlcAlf(cardId, fec_idx, true);
    gCardIdMutex.unlock();
#else
    std::unique_ptr<common::Bar> bar;
    try {
      bar.reset(common::BarFactory::makeBar(pci_addr_str1, 2));
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      exit(1);
    }
    std::unique_ptr<common::HdlcCore> hdlc_core(common::HdlcFactory::makeHdlcCore(*(bar.get()), fec_idx, true));
#endif
    //return;
    //std::cout<<"HDLC core created\n";
    hdlc_core->rst();
    //std::cout<<"HDLC core resetted\n";
    //return;

    DsFec fec(*(hdlc_core.get()), 0);
    //DsFec fec(*(hdlc_core), 0);
    //std::cout<<"FEC board created\n";
    //return;
    fec.init();
    //exit(0);

    //std::cout << "Reading the FEC unique ID" << std::endl;
    //fec.readUniqueID(&std::cout);
    //continue;

    bool readback = true;
    int retries = sRetries; //10;

    for(auto cfg : *dsConfigVec) {

      auto ds_idx = cfg.dsId;
      auto cf0 = cfg.cf0;
      auto cf1 = cfg.cf1;

      for(int iter = 0; iter < 1; iter++) {
	char dumpFileName[501];
	snprintf(dumpFileName, 500, "sampa_dump_%d_%d_0.txt", fec_idx, ds_idx);
        //std::cout << "Loading SAMPA registers" << std::endl;
        if( !fec.sampaDumpConfiguration(cf0, dumpFileName, ds_idx*2, retries, &std::cout) ) {
          std::cout << "ERROR: Configuration of SAMPA chip " << fec_idx << ":" << ds_idx*2
		    << " (LINK " << fec_idx << " S" << (fec_idx%6) + 1 << " J" << ds_idx/5 + 1 << " DS" << (ds_idx%5) << ") FAILED" << std::endl << std::endl;
          success = false;
          break;
        }
	snprintf(dumpFileName, 500, "sampa_dump_%d_%d_1.txt", fec_idx, ds_idx);
        //std::cout<<"SAMPA "<< fec_idx << ":" << ds_idx*2<<" succesfully configured\n";
        if( !fec.sampaDumpConfiguration(cf1, dumpFileName, ds_idx*2+1, retries, &std::cout) ) {
          std::cout << "ERROR: Configuration of SAMPA chip " << fec_idx << ":" << ds_idx*2+1
		    << " (LINK " << fec_idx << " S" << (fec_idx%6) + 1 << " J" << ds_idx/5 + 1 << " DS" << (ds_idx%5) << ") FAILED" << std::endl << std::endl;
          success = false;
          break;
        }
        //std::cout<<"SAMPA "<< fec_idx << ":" << ds_idx*2+1<<" succesfully configured\n";
      }
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


int main(int argc, char** argv)
{
  int argvId = 1;
  bool dump = false;

  if (std::string(argv[1]) == "-d") {
    dump = true;
    argvId = 2;
  }
  
  std::string pci_addr_str1(argv[argvId]);
  std::string pci_addr_str2(argv[argvId + 1]);
  o2::alf::roc::PciAddress pci_addr1(pci_addr_str1);
  o2::alf::roc::PciAddress pci_addr2(pci_addr_str2);
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

  std::vector<DsConfig> dsConfigVec[24];

  std::ifstream istr(argv[argvId + 2]);
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

    dsConfigVec[fec_idx].emplace_back(ds_idx, cf0, cf1);
  }

  sRetries = atoi(argv[argvId + 3]);

  if (dump) {
    for(int fec_idx = 0; fec_idx < 24; fec_idx++) {
      fecDumpConfiguration(fec_idx, pci_addr_str1, pci_addr_str2, &(dsConfigVec[fec_idx]));
      printf("Link %d dumped\n",fec_idx);
      //getchar();
    }
  } else {

#ifdef PARALLEL
  std::vector<std::thread> workers;
  for(int fec_idx = 0; fec_idx < 24; fec_idx++) {
    workers.push_back(std::thread(fecConfig,
        fec_idx,
        pci_addr_str1,
        pci_addr_str2,
        &(dsConfigVec[fec_idx])));
  }
  for(auto& t : workers) {
    t.join();
  }
#else
  for(int fec_idx = 0; fec_idx < 24; fec_idx++) {
    fecConfig(fec_idx, pci_addr_str1, pci_addr_str2, &(dsConfigVec[fec_idx]));
    printf("Link %d configured\n",fec_idx);
    //getchar();
  }
#endif
  }

  printf("success: %d\n", success);

  if( !success ) return 1;
  return 0;
}
