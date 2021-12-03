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

#define PARALLEL 1

#include "Alf/Alf.h"
#include "Alf/Lla.h"

using namespace o2::alf;

static std::mutex gCardIdMutex;


void gbtConfig(int linkid, o2::alf::roc::Parameters::CardIdType cardId1, o2::alf::roc::Parameters::CardIdType cardId2, std::vector<std::pair<uint32_t, uint32_t>>* registers)
{
  if(registers->empty()) return;
  gCardIdMutex.lock();
  o2::alf::roc::Parameters::CardIdType cardId = (linkid < 12) ? cardId1 : cardId2;
  auto linkid_ = linkid % 12;
  //std::cout << "Creating GBT IC interface for card " << cardId << "/" << linkid_ << std::endl;
  o2::alf::Ic ic(cardId, linkid_);
  gCardIdMutex.unlock();

  bool success;

  for(auto& reg : *registers) {

    int retry;
    for(retry = 0; retry < 1; retry++) {
      success = true;
      try {
	//printf("Writing 0x%X into GBT register %d of link %d\n", reg.second, reg.first, linkid);
	ic.write(reg.first, reg.second);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	// GBT register readback seems to induce IC WRITE errors in some cases, so it is disabled
	if (reg.first == 255) {
	  auto rval = ic.read(reg.first);
	  if(rval != reg.second) {
	    printf("SOLAR %d/%d ERROR reading reg %d: %X != %X\n", cardId, linkid_, reg.first, rval, reg.second);
	    success = false;
	  }
	  std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
      } catch (const IcException& e) {
	std::cout << "SOLAR " << cardId << "/" << linkid_ << ": ";
	std::cerr << e.what() << std::endl;
	success = false;
      }
      if(success) break;
    }
    if(success == false) {
      gCardIdMutex.lock();
      std::cout << "SOLAR " << cardId << "/" << linkid_ << " failed" << std::endl;
      gCardIdMutex.unlock();
      return;
    }
  }

  gCardIdMutex.lock();
  std::cout << "SOLAR " << cardId << "/" << linkid_ << " succesfully configured" << std::endl;
  gCardIdMutex.unlock();
}

void readFile(std::string regfile, o2::alf::roc::Parameters::CardIdType& cardId1, o2::alf::roc::Parameters::CardIdType& cardId2)
{
  std::vector<std::pair<uint32_t, uint32_t>> registers[24];
  FILE* f  = fopen(regfile.c_str(),"r");
  //auto ic = Ic(card, linkid);
  uint32_t linkid, reg=0, val;
  while(fscanf(f,"%u %u %u", &linkid, &reg, &val) == 3) {
    registers[linkid].push_back(std::make_pair(reg, val));
  }

#ifdef PARALLEL
  std::vector<std::thread> workers;
  for(int linkid = 0; linkid < 24; linkid++) {
    workers.push_back(std::thread(gbtConfig, linkid, cardId1, cardId2, &(registers[linkid])));
  }
  for(auto& t : workers) {
    t.join();
  }
#else
  for(int linkid = 0; linkid < 24; linkid++) {
    gbtConfig(linkid, cardId1, cardId2, &(registers[linkid]));
  }
#endif
}


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

  //int linkid = atoi(argv[4]);

  if(command == 'r') {
    int linkid = atoi(argv[4]);
    int reg = atoi(argv[5]);

  o2::alf::roc::Parameters::CardIdType cardId = (linkid < 12) ? cardId1 : cardId2;
  linkid = linkid % 12;

  //std::cout << "Creating GBT IC interface for card " << cardId << "/" << linkid << std::endl;
  auto ic = o2::alf::Ic(cardId, linkid);
    //auto ic = Ic(card, linkid);
    try {
      uint32_t rval = ic.read(reg);
      printf("r reg %d: 0x%X (%d)\n", reg, rval, rval);
    } catch (const IcException& e) {
      std::cerr << e.what() << std::endl;
      llaSession->stop();
      return 1;
    }
  }

  if(command == 'w') {
    int linkid = atoi(argv[4]);
    int reg = atoi(argv[5]);
    uint32_t val = atoi(argv[6]);

    o2::alf::roc::Parameters::CardIdType cardId = (linkid < 12) ? cardId1 : cardId2;
    linkid = linkid % 12;

    //std::cout << "Creating GBT IC interface for card " << cardId << "/" << linkid << std::endl;
    auto ic = o2::alf::Ic(cardId, linkid);
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
    readFile(argv[4], cardId1, cardId2);
  }

  if(command == 'd') {
    int linkid = atoi(argv[4]);

    o2::alf::roc::Parameters::CardIdType cardId = (linkid < 12) ? cardId1 : cardId2;
    linkid = linkid % 12;

    //std::cout << "Creating GBT IC interface for card " << cardId << "/" << linkid << std::endl;
    auto ic = o2::alf::Ic(cardId, linkid);
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
