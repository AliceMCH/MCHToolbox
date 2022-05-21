#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "../src/decode/sampa_header.hpp"
#include "../src/decode/decode.hpp"

//#define UL_FORMAT_VERSION 0
#define UL_FORMAT_VERSION 1

int gSkip;
int gCSum = false;

uint64_t out_word[40] = {0};
int n10bitwords[40] = {0};
uint32_t hhvalue,hlvalue,lhvalue,llvalue;
int nFrames = 0;

bool checkBXCNT = false;

struct ULDataWord_v0
{
  union {
    uint64_t word;
    struct {
      uint64_t data: 50;
      uint64_t error: 2;
      uint64_t incomplete: 1;
      uint64_t dsID: 6;
      uint64_t linkID: 5;
    };
  };
};

struct ULDataWord_v1
{
  union {
    uint64_t word;
    struct {
      uint64_t data: 50;
      uint64_t error: 3;
      uint64_t incomplete: 1;
      uint64_t dsID: 6;
      uint64_t linkID: 4;
    };
  };
};


struct CRUheader
{
  uint8_t header_version;
  uint8_t header_size;
  uint16_t block_length;
  uint16_t fee_id;
  uint8_t priority_bit;
  uint8_t reserved_1;

  uint16_t next_packet_offset;
  uint16_t memory_size;
  uint8_t link_id;
  uint8_t packet_counter;
  uint16_t cru_id : 12;
  uint8_t dpw_id : 4;

  uint32_t bx : 12;
  uint32_t reserved_2 : 20;
  uint32_t hb_orbit;

  uint32_t triggerType;
  uint32_t pageCnt : 16;
  uint32_t stop : 8;
  uint32_t reserved4 : 8;
};


typedef struct _RAWDataHeaderV4
{
  // 32-bits words

  union {
    uint32_t word3 = 0x00004004;
    //                     | | version 4
    //                     | header size 16x32 bit = 64 bytes
    struct {
      uint32_t version:8;        /// bit 0 to 7: header version
      uint32_t headerSize:8;     /// bit 8 to 15: header size
      uint32_t blockLength:16;   /// bit 16 to 31: block length
    };
  };

  union {
    uint32_t word2 = 0x00ffffff;
    struct {
      uint32_t feeId:16;          /// bit 0 to 15: FEE id
      uint32_t priorityBit:8;     /// bit 16 to 23: priority bit
      uint32_t zero2:8;           /// bit 24 to 31: reserved
    };
  };

  union {
    uint32_t word1 = 0x0;
    struct {
      uint32_t offsetNextPacket:16;  /// bit 0 to 15: offset of next block
      uint32_t memorySize:16;        /// bit 16 to 31: size of block (in bytes) in memory
    };
  };

  union {
    uint32_t word0 = 0xffffffff;
    struct {
      uint32_t linkId:8;         /// bit 0 to 7: link id (GBT channel number)
      uint32_t packetCounter:8;  /// bit 8 to 15: packet counter (increased at every packet received in the link)
      uint32_t cruId:12;         /// bit 16 to 27: CRU id
      uint32_t dpwId:4;          /// bit 28 to 31: data path wrapper id, used to identify one of the 2 CRU End Points 
    };
  };

  union {
    uint32_t word7 = 0xffffffff;
    struct {
      uint32_t triggerOrbit;     /// bit 0 to 31: TRG orbit
    };
  };

  union {
    uint32_t word6 = 0xffffffff;
    struct {
      uint32_t heartbeatOrbit;     /// bit 0 to 31: HB orbit
    };
  };

  union {
    uint32_t word5 = 0x0;
    struct {
      uint32_t zero5;     /// bit 0 to 31: reserved
    };
  };

  union {
    uint32_t word4 = 0x0;
    struct {
      uint32_t zero4;     /// bit 0 to 31: reserved
    };
  };

  union {
    uint32_t word11 = 0x0;
    struct {
      uint32_t triggerBC:12;     /// bit 0 to 11: TRG BC ID
      uint32_t zero11_0:4;       /// bit 12 to 15: reserved
      uint32_t heartbeatBC:12;   /// bit 16 to 27: HB BC ID
      uint32_t zero11_1:4;       /// bit 28 to 31: reserved
    };
  };

  union {
    uint32_t word10 = 0x0;
    struct {
      uint32_t triggerType:32;   /// bit 0 to 31: trigger types
    };
  };

  union {
    uint32_t word9 = 0x0;
    struct {
      uint32_t zero9;     /// bit 0 to 31: reserved
    };
  };

  union {
    uint32_t word8 = 0x0;
    struct {
      uint32_t zero8;     /// bit 0 to 31: reserved
    };
  };

  union {
    uint32_t word15 = 0x0;
    struct {
      uint32_t detectorField:16;     /// bit 0 to 15: detector field
      uint32_t par:16;               /// bit 16 to 31: PAR
    };
  };
  
  union {
    uint32_t word14 = 0x0;
    struct {
      uint32_t stopBit:8;          /// bit 0 to 7: stop bit
      uint32_t pagesCoutner:16;    /// bit 8 to 23: pages counter
      uint32_t zero14:8;           /// bit 24 to 31: reserved
    };
  };
  
  union {
    uint32_t word13 = 0x0;
    struct {
      uint32_t zero13;     /// bit 0 to 31: reserved
    };
  };
  
  union {
    uint32_t word12 = 0x0;
    struct {
      uint32_t zero12;     /// bit 0 to 31: reserved
    };
  };

} RAWDataHeaderV4;


struct RAWDataHeaderV6 {
  union {
    // default value
    uint64_t word0 = 0x00000000ffff4006;
    //                       | |     | version 6
    //                       | |   | 8x64 bit words = 64 (0x40) byte
    //                       | | invalid FEE id
    //                       | priority bit 0
    struct {
      uint64_t version : 8;        /// bit  0 to  7: header version
      uint64_t headerSize : 8;     /// bit  8 to 15: header size
      uint64_t feeId : 16;         /// bit 16 to 31: FEE identifier
      uint64_t priority : 8;       /// bit 32 to 39: priority bit
      uint64_t sourceID : 8;       /// bit 40 to 47: source ID
      uint64_t zero0 : 16;         /// bit 48 to 63: zeroed
    };                             ///
  };                               ///
  union {                          ///
    uint64_t word1 = 0x0;          /// data written by the CRU
    struct {                       ///
      uint32_t offsetToNext : 16;  /// bit 64 to 79:  offset to next packet in memory
      uint32_t memorySize : 16;    /// bit 80 to 95:  memory size
      uint32_t linkID : 8;         /// bit 96 to 103: link id
      uint32_t packetCounter : 8;  /// bit 104 to 111: packet counter
      uint16_t cruID : 12;         /// bit 112 to 123: CRU ID
      uint32_t endPointID : 4;     /// bit 124 to 127: DATAPATH WRAPPER ID: number used to
    };                             ///                 identify one of the 2 End Points [0/1]
  };                               ///
  union {                          ///
    uint64_t word2 = 0x0;          ///
    struct {                       ///
      uint32_t bunchCrossing : 12; /// bit 0 to 11: bunch crossing counter
      uint32_t reserved2 : 20;     /// bit 12 to 31: reserved
      uint32_t orbit;              /// bit 32 to 63: orbit
    };                             ///
  };                               ///
  union {                          ///
    uint64_t word3 = 0x0;          /// bit  0 to 63: zeroed
  };                               ///
  union {                          ///
    uint64_t word4 = 0x0;          ///
    struct {                       ///
      uint64_t triggerType : 32;   /// bit  0 to 31: trigger type
      uint64_t pageCnt : 16;       /// bit 32 to 47: pages counter
      uint64_t stopBit : 8;           /// bit 48 to 53: stop code
      uint64_t zero4 : 8;          /// bit 54 to 63: zeroed
    };                             ///
  };                               ///
  union {                          ///
    uint64_t word5 = 0x0;          /// bit  0 to 63: zeroed
  };                               ///
  union {                          ///
    uint64_t word6 = 0x0;          ///
    struct {                       ///
      uint64_t detectorField : 32; /// bit  0 to 31: detector field
      uint64_t detectorPAR : 16;   /// bit 32 to 47: detector PAR (Pause and Reset)
      uint64_t zero6 : 16;         /// bit 48 to 63: zeroed
    };                             ///
  };                               ///
  union {                          ///
    uint64_t word7 = 0x0;          /// bit  0 to 63: zeroed
  };
};




static bool BXCNT_compare(int64_t c1, int64_t c2)
{
  //return true;
  const int64_t MAX = 0xFFFFF;
  int64_t diff = c1 - c2;
  if(diff >= MAX) diff -= MAX;
  if(diff <= -MAX) diff += MAX;
  int64_t c1p = (c1+1) & MAX;
  int64_t c2p = (c2+1) & MAX;
  if( c1 == c2 ) return true;
  //if( c1p == c2 ) return true;
  //if( c2p == c1 ) return true;
  return false;
}


decode_state_t Add10BitsOfData(uint64_t data, DualSampa& dsr, DualSampaGroup* dsg)
{
  decode_state_t result = DECODE_STATE_UNKNOWN;
  switch( dsr.status ) {
  case notSynchronized:
    dsr.data += data << dsr.bit;

    if(gPrintLevel >= 2)
      printf("notSynchronized: bit=%02d  data=%013lX  %03X %03X %03X %03X %03X\n",
        dsr.bit, dsr.data,
        (int)((dsr.data>>40) & 0x3FF),
        (int)((dsr.data>>30) & 0x3FF),
        (int)((dsr.data>>20) & 0x3FF),
        (int)((dsr.data>>10) & 0x3FF),
        (int)(dsr.data & 0x3FF)
    );

    dsr.bit += 10;
    result = DECODE_STATE_SYNC_SEARCH;
    if( dsr.bit == 50 ) {
      if(dsr.data == 0x1555540f00113) {
        result = DECODE_STATE_SYNC_FOUND;
        dsr.status = headerToRead;
        dsr.bit = 0;
        dsr.data = 0;
        dsr.packetsize = 0;
        if(gPrintLevel >= 1) {
	  printf("===========================================================\n");
	  printf("[L %d DS %d]  notSynchronized: SYNC word found\n", 
				    (int)dsr.lid, (int)dsr.id);
	}
      } else {
        dsr.data = dsr.data >> 10;
        dsr.bit -= 10;
      }
    }
    break;

  case headerToRead:
    dsr.data += data << dsr.bit;

    if(gPrintLevel >= 2)
      printf("headerToRead: bit=%02d  data=%013lX  %03X %03X %03X %03X %03X\n",
        dsr.bit, dsr.data,
        (int)((dsr.data>>40) & 0x3FF),
        (int)((dsr.data>>30) & 0x3FF),
        (int)((dsr.data>>20) & 0x3FF),
        (int)((dsr.data>>10) & 0x3FF),
        (int)(dsr.data & 0x3FF)
    );

    dsr.bit += 10;

    if( dsr.bit == 50 ) {
      if(dsr.data == 0x1555540f00113) {
        result = DECODE_STATE_SYNC_FOUND;
        dsr.status = headerToRead;
        dsr.bit = 0;
        dsr.data = 0;
        dsr.packetsize = 0;
	dsr.nHeaders = 0;
        if(gPrintLevel >= 1) {
	  printf("-----------------------------------------------------------\n");
	  printf("[L %d DS %d]  headerToRead: SYNC word found\n", 
				(int)dsr.lid, (int)dsr.id);
	}
      } else {
	dsr.nHeaders += 1;
        memcpy(&(dsr.header),&(dsr.data),sizeof(Sampa::SampaHeaderStruct));
        Sampa::SampaHeaderStruct* header = (Sampa::SampaHeaderStruct*)&(dsr.header);
        if(gPrintLevel >= 1) {
          printf("[L %d B %d DS %d CH %d]  Header[%d](%03X %03X %03X %03X %03X): HCode %2d HPar %d PkgType %d 10BitWords %d ChipAdd %d ChAdd %2d BX %d PPar %d\n",
		 (int)dsr.lid, dsr.lid*40+dsr.id, (int)dsr.id, (int)dsr.header.fChannelAddress,
		 dsr.nHeaders, 
		 ((dsr.data >> 40) & 0x3FF),
		 ((dsr.data >> 30) & 0x3FF),
		 ((dsr.data >> 20) & 0x3FF),
		 ((dsr.data >> 10) & 0x3FF),
		 ((dsr.data >>  0) & 0x3FF),
		 header->fHammingCode,header->fHeaderParity,header->fPkgType,
            header->fNbOf10BitWords,header->fChipAddress,header->fChannelAddress,
            header->fBunchCrossingCounter,header->fPayloadParity);
	}

        dsr.bit = 0;
        dsr.data = 0;
        dsr.packetsize = 0;
        if (header->fPkgType == 0) {
	  if ((header->fNbOf10BitWords != 0) || (header->fChannelAddress != 0x15) ||
	      (header->fPayloadParity != 0)) {
	    printf("ERROR: [L %d B %d DS %d CHIP %d] badly formatted heartbeat packet\n", dsr.lid, dsr.lid*40+dsr.id, dsr.id, (int)dsr.header.fChipAddress);
	    dsr.status = notSynchronized;
	    dsr.bit = 0;
	    dsr.data = 0;
	    dsr.packetsize = 0;
	  } else {
	    dsr.status = headerToRead;
	    result = DECODE_STATE_HEARTBEAT_FOUND;
	    dsr.nbHB[dsr.header.fChipAddress%2] += 1;
	  }
	} else if (header->fPkgType == 4) {
	  dsr.status = sizeToRead;
	  result = DECODE_STATE_HEADER_FOUND;
	  
	  int link = dsr.id / 5;
	  if( gPrintLevel >= 1 ) printf("SAMPA [%2d]: BX counter for link %d is %d\n", dsr.id, link, dsg->bxc);
	  if( true && dsg && dsg->bxc >= 0 ) {
	    if( checkBXCNT && !BXCNT_compare(dsg->bxc, dsr.header.fBunchCrossingCounter) ) {
	      printf("===> ERROR SAMPA [L %02d, DS %02d (J%1d,%1d), B %3d]: ChipAdd %d ChAdd %2d BX %d, expected %d, diff %d\n",
		     dsr.lid,dsr.id,dsr.id/5+1,dsr.id%5,(dsr.lid*40+dsr.id), dsr.header.fChipAddress,dsr.header.fChannelAddress,
		     dsr.header.fBunchCrossingCounter, dsg->bxc,
		     dsr.header.fBunchCrossingCounter-dsg->bxc);
	    }
	  } else {
	    dsg->bxc = dsr.header.fBunchCrossingCounter;
	    if( gPrintLevel >= 1 ) printf("SAMPA [%2d]: BX counter for link %d set to %d\n", dsr.id, link, dsg->bxc);
	  }
	} else {
	    printf("ERROR: [L %d B %d DS %d CHIP %d CH %d] unrecognized packet type\n", dsr.lid, dsr.lid*40+dsr.id, dsr.id, (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
	    dsr.status = notSynchronized;
	    dsr.bit = 0;
	    dsr.data = 0;
	    dsr.packetsize = 0;
	}
      }
    }
    break;

  case sizeToRead:
    dsr.csize = data;
    dsr.csum = 0;
    dsr.cid = 0;
    dsr.packetsize += 1;
    if(gPrintLevel >= 1) printf("DS %d CH %d cluster size: %d\n", 
				(int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress, dsr.csize);
    if( dsr.csize < 1 ) {
      printf("ERROR: [L %d B %d DS %d CHIP %d CH %d] too small cluster size\n", dsr.lid, dsr.lid*40+dsr.id, dsr.id, (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
      dsr.status = notSynchronized;
      dsr.bit = 0;
      dsr.data = 0;
      dsr.packetsize = 0;
      break;
    }
    if( !gCSum && ((dsr.csize+2) > dsr.header.fNbOf10BitWords) ) {
      printf("ERROR: [L %d B %d DS %d CHIP %d CH %d] too big cluster size\n", dsr.lid, dsr.lid*40+dsr.id, dsr.id, (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
      dsr.status = notSynchronized;
      dsr.bit = 0;
      dsr.data = 0;
      dsr.packetsize = 0;
      break;
    }
    if( dsr.packetsize == dsr.header.fNbOf10BitWords ) {
      printf("ERROR: [L %d B %d DS %d CHIP %d CH %d] end-of-packet found while reading cluster size\n",
	     dsr.lid, dsr.lid*40+dsr.id, dsr.id, (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
      dsr.status = notSynchronized;
      dsr.bit = 0;
      dsr.data = 0;
      dsr.packetsize = 0;
    } else {
      dsr.status = timeToRead;
      result = DECODE_STATE_CSIZE_FOUND;
    }
    break;

  case timeToRead:
    dsr.ctime = data;
    dsr.packetsize += 1;
    if( dsr.packetsize == dsr.header.fNbOf10BitWords ) {
      printf("ERROR: [L %d B %d DS %d CHIP %d CH %d] end-of-packet found while reading cluster time\n",
	     dsr.lid, dsr.lid*40+dsr.id, dsr.id, (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
      dsr.status = notSynchronized;
      dsr.bit = 0;
      dsr.data = 0;
      dsr.packetsize = 0;
    } else {
      dsr.status = dataToRead;
      result = DECODE_STATE_CTIME_FOUND;
      if(gPrintLevel >= 1) printf("DS %d CH %d cluster time: %d\n", 
				(int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress, dsr.ctime);
    }
    break;

  case dataToRead:
    dsr.sample = data;
    dsr.csum += data;
    dsr.cid += 1;
    dsr.packetsize += 1;

    if (dsr.header.fPkgType == 4) { // Good data
      dsr.pedestal[dsr.header.fChipAddress%2][dsr.header.fChannelAddress] += data;
      dsr.noise[dsr.header.fChipAddress%2][dsr.header.fChannelAddress] += data*data;
      dsr.ndata[dsr.header.fChipAddress%2][dsr.header.fChannelAddress] += 1;
    }

    bool end_of_packet = (dsr.header.fNbOf10BitWords == dsr.packetsize);
    bool end_of_cluster = gCSum ? (dsr.cid == 2) : (dsr.cid == dsr.csize);
    //printf("dataToRead: cid=%d  packetsize=%d  end_of_packet=%d  end_of_cluster=%d\n",
    //    dsr.cid, dsr.packetsize, (int)end_of_packet, (int)end_of_cluster);
    if (end_of_packet && !end_of_cluster) {
      printf("ERROR: [L %d DS %d CHIP %d CH %d] end-of-packet found while reading cluster data\n",
	     dsr.lid, dsr.id, (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
      dsr.bit = 0;
      dsr.data = 0;
      dsr.packetsize = 0;
      dsr.status = notSynchronized;
    } else {
      result = DECODE_STATE_SAMPLE_FOUND;
      dsr.status = dataToRead;
      if(gPrintLevel >= 1) 
	printf("DS %d CH %d  sample[%d]: %d (0x%X)\n",
	       (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress, 
	       dsr.cid, dsr.sample, dsr.sample);
      if(end_of_cluster) {
        //if(dsr.csum > 500) printf("Big signal found!\n");
        if (end_of_packet) {
	  if(gPrintLevel >= 1)
	    printf("DS %d CH %d  end-of-packet reached\n",
		   (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
          dsr.bit = 0;
          dsr.data = 0;
          dsr.packetsize = 0;
          dsr.status = headerToRead;
          result = DECODE_STATE_PACKET_END;
        } else {
	  if(gPrintLevel >= 1)
	    printf("DS %d CH %d  end-of-cluster reached\n",
		   (int)dsr.header.fChipAddress, (int)dsr.header.fChannelAddress);
          dsr.status = sizeToRead;
        }
      }
    }
    break;
  }

  return result;
}


#define NCRU 3

int main(int argc, char** argv)
{
  //FILE* fin = fopen(argv[argc-1],"r");
  printf("reading file \"%s\"\n", argv[argc-1]);
  int fin = open(argv[argc-1], O_RDONLY);
  int level = 0;
  gPrintLevel = level;  // Global variable defined as extern in the others .cxx files
  gPattern = 0;
  gNbErrors = 0;
  gNbWarnings = 0;
  int boardnum = -1;
  bool print_noise = false;
  int end = 0;
  int target_cru_id = 0;

  std::vector<int> board_vec;


  // decode the input line
  for (int i = 1; i < argc; i++) { // argument 0 is the executable name
    char *arg;

    arg =  argv[i];
    if (arg[0] != '-') continue;
    switch (arg[1]) {
    case 's' :
      i++;
      //    InputFile = argv[i];
      gSkip=strtol(argv[i], NULL, 0);
      break;
    case 'S' :
      //    InputFile = argv[i];
      gCSum = true;
      break;
    case 'e' :
      i++;
      end=strtol(argv[i], NULL, 0);
      break;
    case 'p' :
      i++;
      //    InputFile = argv[i];
      gPattern=strtol(argv[i], NULL, 0);
      break;
    case 'd' :
      i++;
      gPrintLevel=atoi(argv[i]);
      break;
    case 'n' :
      print_noise=true;
      break;
    case 'b' :
      i++;
      boardnum=atoi(argv[i]);
      board_vec.push_back(boardnum);
      break;
    case 'c' :
      i++;
      target_cru_id=atoi(argv[i]);
      break;
    case 'x' :
      checkBXCNT = true;
      break;
    }
  }

  int NbCardStart = 0;
  int NbCardEnd = 39;
  if(boardnum >= 0) {
    NbCardStart = boardnum;
    NbCardEnd = NbCardStart;
  }

  char tstr[500];
  DualSampa ds[NCRU][32][40];
  DualSampaGroup dsg[NCRU][32][8];

  for(int c = 0; c < NCRU; c++) {
    for(int l = 0; l < 24; l++) {
      for(int i = 0; i < 40; i++) {
        DualSampaInit( &(ds[c][l][i]) );
        //ds[i].status = headerToRead;
        ds[c][l][i].id = i;
        ds[c][l][i].nbHit = -1;
        for(int j=0;j<64;j++) {
          ds[c][l][i].nbHitChan[j]=0;
        }
      }
      for(int i = 0; i < 8; i++) {
        DualSampaGroupInit( &(dsg[c][l][i]) );
      }
    }
  }

  /*
  DualSampaStatus status = headerToRead;

  int n10bitwords = 0;

  uint64_t data;
  int i10bitword_in = 0;
  int i10bitword_out = 0;
   */

  uint32_t CRUbuf[4*4];
  CRUheader CRUh;
  bool read_header = true;
  int payload_size = 0;
  uint64_t* payload_buf = NULL;
  void* out_buf = NULL;
  uint32_t out_buf_size = 0;
  int BLOCK_SIZE = 8128;
  int hb_orbit[2] = {-1, -1};
  int n = 0;
  int nskip = gSkip; //207;
  int iskip = 0;


  /*while( iskip < nskip ) {
    iskip += 1;
    uint64_t temp;
    read( fin, &temp, sizeof(temp) );
    //printf("skipped: %llX ", temp);
    read( fin, &temp, sizeof(temp) );
    //printf("skipped: %llX\n", temp);
    }*/

  RAWDataHeaderV6 rdh;
  while(true) {

    if (end > 0 && nFrames > end) break;

    int n64bitWords = 0;
    if( read_header ) {
      n64bitWords = 0;
      int nread = read( fin, CRUbuf, sizeof(CRUbuf) );
      if( nread != sizeof(CRUbuf) ) break;
      memcpy(&CRUh,CRUbuf,sizeof(CRUheader));
      memcpy(&rdh,CRUbuf,sizeof(rdh));
      hhvalue = CRUbuf[3]; hlvalue = CRUbuf[2]; lhvalue = CRUbuf[1]; llvalue = CRUbuf[0];
      //printf("%d (%d): %.8X %.8X %.8X %.8X\n",n*4*4*4,sizeof(CRUbuf),
      //    hhvalue, hlvalue, lhvalue, llvalue);
      CRUh.block_length = CRUh.memory_size - CRUh.header_size;
      BLOCK_SIZE = CRUh.block_length;
      int cru_id = ((int)CRUh.cru_id) & 0xFF;
      if( (nFrames%1000000 == 0) || false ||
          (nFrames > nskip && gPrintLevel >= 1 /*&& cru_id == target_cru_id*/ /*&& CRUh.block_length > 0*/) ) {
	//printf("nFrames=%d  skip=%d  end=%d\n", nFrames, nskip, end);
	printf("%6d:  version %X  memsz %4d  offset %4d  packet %3d  srcID %d  cruID %2d  dp %d  feeID %04X  link %2d  orbit %u  bc %4d  trig 0x%08X  page %d  stop %d  UL version %X\n",
	       nFrames, (int)rdh.version, (int)rdh.memorySize, (int)rdh.offsetToNext,
	       (int)rdh.packetCounter, (int)rdh.sourceID, (int)rdh.cruID, (int)rdh.endPointID, (int)rdh.feeId, (int)rdh.linkID,
	       rdh.orbit, (int)rdh.bunchCrossing, rdh.triggerType, (int)rdh.pageCnt,
	       (int)rdh.stopBit, (int)(rdh.word6 & 0xFFFF));
	if(false && CRUh.block_length > 0) 
	  printf("HBF size: %d\nPayload size: %d\nN. of 256-bit words: %f\n",
		 (int)CRUh.memory_size, (int)CRUh.block_length, ((float)CRUh.block_length) / 256);
      }
      if( CRUh.header_version < 4 || CRUh.header_version > 6 || CRUh.header_size != 64 ) break;
      read_header = false;

      if ((int)rdh.cruID & 0x100) {
	gCSum = true;
      }

      if(gPrintLevel >= 9) {
	printf("xxx 0x%016lX\nxxx 0x%016lX\nxxx 0x%016lX\nxxx 0x%016lX\n",
	       CRUbuf[0], CRUbuf[1], CRUbuf[2], CRUbuf[3]);
      }

      nFrames++; continue;
    }
    //continue;

    if (nFrames <= nskip) {
      lseek(fin, BLOCK_SIZE, SEEK_CUR);
      read_header = true;
      continue;
    }

    if(BLOCK_SIZE == 0) {
      read_header = true;
      continue;
    }

    payload_buf = (uint64_t*)realloc(payload_buf,BLOCK_SIZE);
    if( !payload_buf ) break;
    //printf("reading CRU payload (%d)\n", CRUh.block_length);
    int bytesRead = 0;
    while( bytesRead < BLOCK_SIZE ) {
      int nread = read( fin, ((void*)payload_buf)+bytesRead, BLOCK_SIZE-bytesRead );
      if( nread < 0 ) {
        printf("ERROR reading input buffer\n");
        break;
      }
      bytesRead += nread;
      //printf("bytesRead: %d\n", bytesRead);
      //if( nread != BLOCK_SIZE ) {
      //printf("read %d bytes, expected %d\n", nread, CRUh.block_length);
      //printf("read %d bytes, expected %d\n", nread, BLOCK_SIZE);
      //break;
      //}
    }
    read_header = true;


    /*
    out_buf = realloc(out_buf,out_buf_size + CRUh.block_length);
    if( !out_buf ) break;
    memcpy(out_buf+out_buf_size, payload_buf, CRUh.block_length);
    out_buf_size += CRUh.block_length;
     */
    //continue;

    int cru_id = ((int)CRUh.cru_id) & 0xFF;
    int endPointID = (int)rdh.endPointID;
    if(cru_id != target_cru_id) continue;
    cru_id = 0;

    int cru_lid = CRUh.link_id;
    bool orbit_jump = true;
    int Dorbit1 = CRUh.hb_orbit - hb_orbit[endPointID];
    int Dorbit2 = (uint32_t)( ((uint64_t)CRUh.hb_orbit) + 0x100000000 - hb_orbit[endPointID] );
    if( Dorbit1 >=0 && Dorbit1 <= 1 ) orbit_jump = false;
    if( Dorbit2 >=0 && Dorbit2 <= 1 ) orbit_jump = false;
    if( orbit_jump ) {
      if (gPrintLevel >= 1) printf("Resetting decoding FSM for end-point %d\n", endPointID);
      for(int c = 0; c < NCRU; c++) {
        for(int l = endPointID*12; l < (endPointID*12 + 12); l++) {
          for(int i = 0; i < 40; i++) {
            DualSampaReset( &(ds[c][l][i]) );
            ds[c][l][i].id = i;
            ds[c][l][i].nbHit = -1;
            for(int j=0;j<64;j++) {
              ds[c][l][i].nbHitChan[j]=0;
            }
          }
          for(int i = 0; i < 8; i++) {
            DualSampaGroupReset( &(dsg[c][l][i]) );
          }
        }
      }
    }
    hb_orbit[endPointID] = CRUh.hb_orbit;

    for(int i = 0; i < 40; i++) {
      out_word[i] = 0;
      n10bitwords[i] = 0;
    }

    int nWords = CRUh.block_length / 8; 
    if( gPrintLevel >= 1) printf("Number of 64 bit words: %d\n", nWords);
    for( int wi = 0; wi < nWords; wi+=1) {

      //for(int k = 0; k < 8; k++) {
        uint64_t* ptr = payload_buf + wi;
        uint64_t value = *ptr;
        //ProcessGBTWord(ptr, cru_lid);

        if( gPrintLevel >= 2) printf("64 bits[%d]: %016lX, DS %d-%d\n", n64bitWords, value, 
				     (int)((value >> 59) & 0x1F), (int)((value >> 53) & 0x3F));
        n64bitWords += 1;
	//if( value == 0xFFFFFFFFFFFFFFFF ) continue;
	//if( value == 0xFEEDDEEDFEEDDEED || value == 0x0000000000000000 ) continue;
	if( value == 0xFEEDDEEDFEEDDEED ) continue;

	if(false && wi >= nWords-8) {
	  if(gPrintLevel >= 0) {
	    CRUheader CRUh2;
	    memcpy(&CRUh2,ptr,sizeof(CRUheader));
	    memcpy(&rdh,ptr,sizeof(rdh));
	    printf("%d:  header_version: %X, header_size: %d, memory_size: %d, block_length: %d, packet: %d, link_id: %d, orbit: %d, stop_bit: %d\n",
		   nFrames, (int)CRUh2.header_version, (int)CRUh2.header_size, 
		   (int)CRUh2.memory_size, (int)CRUh2.block_length,
		   (int)CRUh2.packet_counter, (int)CRUh2.link_id, (int)CRUh2.hb_orbit,
		   (int)rdh.stopBit);
	  }
	  break;
	}

        //int nch = (value >> 54) & 0x7FF;
	int bit14 = (value >> 50) & 0x3FFF;
#if(UL_FORMAT_VERSION == 0)
	ULDataWord_v0 word;
	word.word = value;
        int err_code = word.error; //bit14 & 0x3;
        int is_incomplete = word.incomplete; //(bit14 >> 2) & 0x1;
        int ds_id = word.dsID; //(bit14 >> 3) & 0x3F;
        int link_id = word.linkID + 12 * (int)CRUh.dpw_id; //((bit14 >> 9) & 0x1F) + 12 * (int)CRUh.dpw_id;
#endif
#if(UL_FORMAT_VERSION == 1)
	ULDataWord_v1 word;
	word.word = value;
        int err_code = word.error; //bit14 & 0x7;
        int is_incomplete = word.incomplete; //(bit14 >> 4) & 0x1;
        int ds_id = word.dsID; //(bit14 >> 5) & 0x3F;
        int link_id = word.linkID + 12 * (int)CRUh.dpw_id; //((bit14 >> 11) & 0xF) + 12 * (int)CRUh.dpw_id;
#endif
        //printf("nch: %d\n", nch);
        //printf("link_id: %d\n", link_id);
        //printf("ds_id: %d\n", ds_id);
	//if(link_id != 0 || (ds_id != 0)) continue;

	if(word.linkID == 15) {
	  if( gPrintLevel >= 1) {
	    printf("status word: %016lX\n", value);
	    printf("14 bits: %016lX\n", (value >> 50)&0x3FFF);
	  }
	  continue;
	}
	if(word.linkID > 11) continue;

	ds[cru_id][link_id][ds_id].lid = link_id;

	if( !board_vec.empty() ) {
	  bool found = false;
	  for(unsigned int j = 0; j < board_vec.size(); j++) {
	    if((ds_id+(link_id*40)) == board_vec[j]) found = true;
	  }
	  if( !found ) continue;
	}

        if( gPrintLevel >= 1) 
	  printf("\nlink_id: %d  ds_id: %d  incomplete: %d  err: %d\n", 
		 link_id, ds_id, is_incomplete, err_code);
        //if( iskip < nskip ) {
        //  iskip += 1;
        //  printf( "SKIPPED\n" );
        //  continue;
        //}
        if( gPrintLevel >= 1) {
	  printf("word: %016lX\n", value);
	  printf("14 bits: %016lX\n", (value >> 50)&0x3FFF);
	  for(int bi = 13; bi >= 0; bi--) {
	    if( (bit14 >> bi) & 0x1 ) printf("1");
	    else printf("0");
#if(UL_FORMAT_VERSION == 0)
	    if(bi == 9 || bi == 3 || bi == 2) printf(" ");
#endif
#if(UL_FORMAT_VERSION == 1)
	    if(bi == 10 || bi == 4 || bi == 3) printf(" ");
#endif
	  }
	  printf("\n"); 
	}
        if( gPrintLevel >= 1) 
	  printf("50 bits: %016lX (%03X %03X %03X %03X %03X)\n", 
		 value & 0x3FFFFFFFFFFFF,
		 ((value >> 40) & 0x3FF),
		 ((value >> 30) & 0x3FF),
		 ((value >> 20) & 0x3FF),
		 ((value >> 10) & 0x3FF),
		 ((value >> 0) & 0x3FF));
        if( gPrintLevel >= 9) printf("xxx 0x%04lX<<50 + 0x%013lX\n", (value >> 50)&0x3FFF, value & 0x3FFFFFFFFFFFF);

        //printf("sizeof SampaHeaderStruct: %d\n", sizeof(Sampa::SampaHeaderStruct));
        //Sampa::SampaHeaderStruct* header = (Sampa::SampaHeaderStruct*)&value;
        //printf("SAMPA Header: HCode %2d HPar %d PkgType %d 10BitWords %d ChipAdd %d ChAdd %2d BX %d PPar %d\n",
        //            header->fHammingCode,header->fHeaderParity,header->fPkgType,
        //            header->fNbOf10BitWords,header->fChipAddress,header->fChannelAddress,
        //            header->fBunchCrossingCounter,header->fPayloadParity);
        if( gPrintLevel >= 2) {
          printf("10 bits:\n");
          printf("    %ld\n", value&0x3FF);
          printf("    %ld\n", (value>>10)&0x3FF);
          printf("    %ld\n", (value>>20)&0x3FF);
          printf("    %ld\n", (value>>30)&0x3FF);
          printf("    %ld\n", (value>>40)&0x3FF);
        }

	/* Fix decoding in the case of mis-placed incomplete bits
      	if( is_incomplete ) {
	  printf("Resetting decoding FSM\n");
	  for(int c = 0; c < 3; c++) {
	    for(int l = 0; l < 24; l++) {
	      for(int i = 0; i < 40; i++) {
		DualSampaReset( &(ds[c][l][i]) );
		ds[c][l][i].id = i;
		ds[c][l][i].nbHit = -1;
		for(int j=0;j<64;j++) {
		  ds[c][l][i].nbHitChan[j]=0;
		}
	      }
	      for(int i = 0; i < 8; i++) {
		DualSampaGroupReset( &(dsg[c][l][i]) );
	      }
	    }
	  }
	}
	*/
	if (err_code & 0x1) {
	  // the packet end was not found in the first 40 bits of the incomplete word, thatś strange...
	  if( gPrintLevel >= 1) {
	  printf("WARNING: [L %d B %d DS %d] too long data sequence!!!\nresetting FSM for DS board[%d][%d][%d]\n",
		 ds[cru_id][link_id][ds_id].lid, ds[cru_id][link_id][ds_id].lid*40+ds[cru_id][link_id][ds_id].id, ds[cru_id][link_id][ds_id].id, cru_id, link_id, ds_id);
	  }
	  DualSampaReset( &(ds[cru_id][link_id][ds_id]) );
	  continue;
	}

	decode_state_t state;
	state = Add10BitsOfData(value&0x3FF, ds[cru_id][link_id][ds_id], 
				&dsg[cru_id][link_id][ds_id/8]);
        if( is_incomplete && (state == DECODE_STATE_SYNC_SEARCH ||
			      state == DECODE_STATE_PACKET_END || 
			      state == DECODE_STATE_HEARTBEAT_FOUND) ) continue;

	state = Add10BitsOfData((value>>10)&0x3FF, ds[cru_id][link_id][ds_id], 
				&dsg[cru_id][link_id][ds_id/8]);
        if( is_incomplete && (state == DECODE_STATE_SYNC_SEARCH ||
			      state == DECODE_STATE_PACKET_END || 
			      state == DECODE_STATE_HEARTBEAT_FOUND) ) continue;

        state = Add10BitsOfData((value>>20)&0x3FF, ds[cru_id][link_id][ds_id], 
				&dsg[cru_id][link_id][ds_id/8]);
        if( is_incomplete && (state == DECODE_STATE_SYNC_SEARCH ||
			      state == DECODE_STATE_PACKET_END || 
			      state == DECODE_STATE_HEARTBEAT_FOUND) ) continue;

        state = Add10BitsOfData((value>>30)&0x3FF, ds[cru_id][link_id][ds_id], 
				&dsg[cru_id][link_id][ds_id/8]);
        if( is_incomplete && (state == DECODE_STATE_SYNC_SEARCH ||
			      state == DECODE_STATE_PACKET_END || 
			      state == DECODE_STATE_HEARTBEAT_FOUND) ) continue;

	state = Add10BitsOfData((value>>40)&0x3FF, ds[cru_id][link_id][ds_id], 
				&dsg[cru_id][link_id][ds_id/8]);
        if( is_incomplete && (state == DECODE_STATE_SYNC_SEARCH ||
			      state == DECODE_STATE_PACKET_END || 
			      state == DECODE_STATE_HEARTBEAT_FOUND) ) continue;

	if (is_incomplete) {
	  // the packet end was not found in the first 40 bits of the incomplete word, thatś strange...
	  printf("ERROR: [L %d B %d DS %d CHIP %d CH %d] packet end not found while processing incomplete word!!!\nresetting FSM for DS board\n",
		 ds[cru_id][link_id][ds_id].lid, ds[cru_id][link_id][ds_id].lid*40+ds[cru_id][link_id][ds_id].id, ds[cru_id][link_id][ds_id].id, 
		 (int)(ds[cru_id][link_id][ds_id].header.fChipAddress), 
		 (int)(ds[cru_id][link_id][ds_id].header.fChannelAddress));
	  DualSampaReset( &(ds[cru_id][link_id][ds_id]) );
	  //ds[cru_id][link_id][ds_id].id = ds_id;
	  //ds[cru_id][link_id][ds_id].nbHit = -1;
	  //for(int j=0;j<64;j++) {
	  //  ds[cru_id][link_id][ds_id].nbHitChan[j]=0;
	  //}
	}
    }
    //printf("=========\n");
    n++;
  }


  FILE* fnoiseout = fopen("/tmp/ds_ch_exclude.txt", "w");

  if( print_noise ) {
    for(int c = 0; c < 1/*NCRU*/; c++) {
      for(int l = 0; l < 24; l++) {
	for(int i = 0; i < 40; i++) {
	  float noise_tot[2] = {0}; int Nnoise[2] = {0};
	  for(int j = 0; j < 2; j++) {
	    for(int k = 0; k < 32; k++) {
	      DualSampa& dsr = ds[c][l][i];
	      if( dsr.ndata[j][k] > 0 ) {
		float ped = (float)(dsr.pedestal[j][k]/dsr.ndata[j][k]);
		float noise = (float)sqrt( dsr.noise[j][k]/dsr.ndata[j][k] - ped*ped );
		noise_tot[j] += noise; Nnoise[j] += 1;
		if(ped>150 || noise > 1.5) {
		  printf("%2d %2d %2d %d %2d  %0.3f  %0.3f  %d %d\n",
			 target_cru_id, l, i, j, k, ped, noise, dsr.ndata[j][k], dsr.nclus[j][k]);
		  if(fnoiseout)
		    fprintf(fnoiseout, "%d %d %d %d %d  %0.3f  %0.3f  %d %d\n",
			 target_cru_id, l, i, j, k, ped, noise, dsr.ndata[j][k], dsr.nclus[j][k]);
		}
	      }
	    }
	  }
	  
	  if( Nnoise[0] > 0 && Nnoise[1] > 0 )
	    printf("average noise: %0.3f, %0.3f\n",noise_tot[0]/Nnoise[0],noise_tot[1]/Nnoise[1]);
	}
      }
    }
  }

  if(fnoiseout) fclose(fnoiseout);

  for(unsigned int i = 0; i < board_vec.size(); i++) {
    int l = board_vec[i] / 40;
    int b = board_vec[i] % 40;
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 32; k++) {
	DualSampa& dsr = ds[0][l][b];
        if( dsr.ndata[j][k] == 0 ) {
          printf("L%d B%d J%d DS%d: missing data from chip %d channel %d\n", l, board_vec[i], ((b/5)+1), (b%5), j, k);
          break;
        }
      }
    }
  }

  for(unsigned int i = 0; i < board_vec.size(); i++) {
    int l = board_vec[i] / 40;
    int b = board_vec[i] % 40;
    //printf("L%d B%d J%d DS%d: HB packets %d %d\n", l, board_vec[i], ((b/5)+1), (b%5), ds[0][l][b].nbHB[0], ds[0][l][b].nbHB[1]);
  }

  return 0;
}
