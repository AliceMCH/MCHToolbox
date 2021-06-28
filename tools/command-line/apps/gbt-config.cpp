
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

#include "Alf/Alf.h"
#include "Alf/Lla.h"

using namespace o2::alf;


int main(int argc, char** argv)
{
  o2::alf::roc::PciAddress pci_addr1(argv[1]);
  o2::alf::roc::PciAddress pci_addr2(argv[2]);
  o2::alf::roc::Parameters::CardIdType cardId1 = pci_addr1;
  o2::alf::roc::Parameters::CardIdType cardId2 = pci_addr2;
  auto card1 = roc::findCard(cardId1);
  auto card2 = roc::findCard(cardId2);
  std::unique_ptr<LlaSession> llaSession = std::make_unique<LlaSession>("MCH", card1.serialId);

  llaSession->start();

  char command = argv[3][0];

  int linkid = atoi(argv[4]);
  
  o2::alf::roc::Parameters::CardIdType cardId = (linkid >= 12) ? cardId1 : cardId2;
  linkid = linkid % 12;
  auto ic = o2::alf::Ic(cardId, linkid);

  if(command == 'r') {
    //int linkid = atoi(argv[3]);
    int reg = atoi(argv[5]);
    //auto ic = Ic(card, linkid);
    try {
      uint32_t rval = ic.read(reg);
      printf("r reg %d: %X\n", reg, rval);
    } catch (const IcException& e) {
      std::cerr << e.what() << std::endl;
      llaSession->stop();
      return 1;
    }
  }

  if(command == 'w') {
    //int linkid = atoi(argv[3]);
    int reg = atoi(argv[5]);
    uint32_t val = atoi(argv[6]);
    //auto ic = Ic(card, linkid);
    printf("Writing 0x%X into GBT register %d\n", val, reg);
    try {
      uint32_t rval = ic.read(reg);
      //printf("reg %d [1]: %X\n", reg, rval);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ic.write(reg, val);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      rval = ic.read(reg);
      printf("w reg %d: %X\n", reg, rval);
      //sleep(1);
    } catch (const IcException& e) {
      std::cerr << e.what() << std::endl;
      llaSession->stop();
      return 1;
    }
  }

  if(command == 'f') {
    //int linkid = atoi(argv[3]);
    FILE* f  = fopen(argv[5],"r");
    //auto ic = Ic(card, linkid);
    uint32_t reg=0, val;
    try {
      while(fscanf(f,"%u %u", &reg, &val) == 2) {
	//printf("Writing 0x%X into GBT register %d of link %d", val, reg, linkid);
	ic.write(reg, val);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//uint32_t rval = ic.read(reg);
	//printf("  ->  0x%X\n", rval);
	//if( val != rval ) {
	//  printf("Failed writing 0x%X into GBT register %d of link %d  ->  0x%X\n", val, reg, linkid, rval);
	//  return 1;
	//}
      }
    } catch (const IcException& e) {
      std::cerr << e.what() << std::endl;
      llaSession->stop();
      return 1;
    }
  }

  if(command == 'd') {
    //int linkid = atoi(argv[3]);
    //auto ic = Ic(card, linkid);
    for(int reg = 0; reg <= 368; reg++) {
      try {
	uint32_t rval = ic.read(reg);
	printf("%02X\n", rval);
      } catch (const IcException& e) {
	std::cerr << e.what() << std::endl;
      }
    }
  }

  llaSession->stop();

  return 0;
}
