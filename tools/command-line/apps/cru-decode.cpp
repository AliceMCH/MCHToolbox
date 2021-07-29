#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "../src/decode/decode.hpp"

int gSkip;


int cruId = 0;
int linkId = -1;
std::vector<int> board_vec;


uint64_t out_word[40] = {0};
int n10bitwords[40] = {0};
uint32_t hhvalue,hlvalue,lhvalue,llvalue;
int nFrames = 0;

DualSampa ds[24][40];
DualSampaGroup dsg[24][8];


FILE* fout = NULL;


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
  //uint16_t cru_id;
  //uint8_t dummy1;
  //uint64_t dummy2;
};



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



void ProcessGBTWord(uint32_t* GBTw, int cru_lid)
{
  uint32_t* ptr = GBTw;
  hhvalue = ptr[3]; hlvalue = ptr[2]; lhvalue = ptr[1]; llvalue = ptr[0];
  //printf("%d (%d): %.8X %.8X %.8X %.8X\n",n,sizeof(CRUbuf)*8,
  //    hhvalue, hlvalue, lhvalue, llvalue);
  //fprintf(stdout,"%.8X %.8X %.8X %.8X\n",
  //        hhvalue, hlvalue, lhvalue, llvalue);

  uint32_t bufpt[4] = {hhvalue, hlvalue, lhvalue, llvalue};
  uint32_t data2bits[40];
  DecodeGBTWord(bufpt, data2bits);

  decode_state_t state;
  for(unsigned int j = 0; j < board_vec.size(); j++) {
    int lid = board_vec[j]/40;
    if( lid != cru_lid ) continue;
    //printf("lid=%d  cru_lid=%d\n", lid, cru_lid);
    int i = board_vec[j] % 40;
    int link = ds[cru_lid][i].id / 5;
    int bits[2] = {data2bits[i]&0x1, (data2bits[i]>>1)&0x1};
    for(unsigned int k = 0; k < 2; k++) {
      int n10bitwords2write = 0;
      uint64_t cur_word = 0;
      bool send_partial = false;
      //printf("ds[%d]: Add1BitOfData() called\n", i);
      state = Add1BitOfData( bits[k], (ds[cru_lid][i]), &(dsg[cru_lid][link]) );
      switch(state) {
      case DECODE_STATE_SYNC_FOUND: send_partial = true; break;
      case DECODE_STATE_HEADER_FOUND:
        //printf("HEADER: %05lX\n",*(unsigned long*)(&ds[cru_lid][i].header));
        n10bitwords2write = 5;
        cur_word = *(uint64_t*)(&ds[cru_lid][i].header);
        break;
      case DECODE_STATE_CSIZE_FOUND:
        //printf("CLUSTER SIZE: %d\n",ds[cru_lid][i].csize);
        n10bitwords2write = 1;
        cur_word = ds[cru_lid][i].csize;
        break;
      case DECODE_STATE_CTIME_FOUND:
        //printf("CLUSTER TIME: %d\n",ds[cru_lid][i].ctime);
        n10bitwords2write = 1;
        cur_word = ds[cru_lid][i].ctime;
        break;
      case DECODE_STATE_SAMPLE_FOUND:
        //printf("SAMPLE: %lX\n",ds[cru_lid][i].sample);
        n10bitwords2write = 1;
        cur_word = ds[cru_lid][i].sample;
        break;
      default: break;
      }

      if( n10bitwords[i] > 0 && send_partial) {
        int cru_link = 0;
        uint64_t nboard = i;
        out_word[nboard] += (nboard << 50) & 0x3F000000000000;
        out_word[nboard] += 0x8000000000000000;
        //printf("Writing %016lX\n", out_word[nboard]);
        //fprintf(fout,"%016lX\n", out_word[nboard]);
        out_word[nboard] = 0; n10bitwords[nboard] = 0;
      }
      if( n10bitwords2write > 0 ) {
        uint32_t v10bitwords[5] = {
            (uint32_t)(cur_word & 0x3FF),
            (uint32_t)((cur_word >> 10) & 0x3FF),
            (uint32_t)((cur_word >> 20) & 0x3FF),
            (uint32_t)((cur_word >> 30) & 0x3FF),
            (uint32_t)((cur_word >> 40) & 0x3FF),
        };
        for( int iw = 0; iw < n10bitwords2write; iw++ ) {
          uint64_t nboard = i;
          uint64_t w = v10bitwords[iw];
          w = (w << n10bitwords[nboard]*10);
          uint64_t mask = 0x3FF;
          for( int jw = 0; jw < n10bitwords[nboard]; jw++ ) {
            mask = (mask << 10) & 0xFFFFFFFFFFFFFC00;
          }
          //printf("10bit word in=%d out=%d: w=%016lX  mask=%016lX\n", iw, n10bitwords[nboard], w, mask);
          out_word[nboard] += w & mask;
          n10bitwords[nboard] += 1;
          if( n10bitwords[nboard] == 5) {
            int cru_link = 0;
            out_word[nboard] += (nboard << 50) & 0x3F000000000000;
            if(send_partial) out_word[nboard] += 0x8000000000000000;
            //printf("Writing %016lX\n", out_word[nboard]);
            if(fout) fprintf(fout,"%016lX\n", out_word[nboard]);
            out_word[nboard] = 0; n10bitwords[nboard] = 0;
          }
        }
      }
    }
  }
}



void ReadPIPE(int fin)
{
  uint32_t CRUbuf[4*4];
  CRUheader CRUh;
  RAWDataHeaderV6 rdh;
  bool read_header = true;
  int payload_size = 0;
  uint32_t* payload_buf = NULL;
  void* out_buf = NULL;
  uint32_t out_buf_size = 0;
  int BLOCK_SIZE = 8128;
  int hb_orbits[24];
  int packet_counter = -1;
  int n = 0;
  int nskip = gSkip;
  int iskip = 0;
  bool skip_time_frame = true;

  for(int l = 0; l < 24; l++) hb_orbits[l] = -1;

  while( iskip < nskip ) {
    iskip += 1;
    uint64_t temp;
    read( fin, &temp, sizeof(temp) );
    //printf("skipped: %llX ", temp);
    read( fin, &temp, sizeof(temp) );
    //printf("skipped: %llX\n", temp);
  }

  while(true) {

    if( read_header ) {
      int nread = read( fin, CRUbuf, sizeof(CRUbuf) );
      if( nread != sizeof(CRUbuf) ) break;
      memcpy(&CRUh,CRUbuf,sizeof(CRUheader));
      memcpy(&rdh,CRUbuf,sizeof(rdh));
      hhvalue = CRUbuf[3]; hlvalue = CRUbuf[2]; lhvalue = CRUbuf[1]; llvalue = CRUbuf[0];
      //printf("%d (%d): %.8X %.8X %.8X %.8X\n",nFrames,sizeof(CRUbuf),
      //    hhvalue, hlvalue, lhvalue, llvalue);
      CRUh.block_length = CRUh.memory_size - CRUh.header_size;
      BLOCK_SIZE = CRUh.next_packet_offset - CRUh.header_size;

      bool doPrint = true;
      if((nFrames%100000 != 0) && (gPrintLevel == 0)) doPrint = false;
      if(CRUh.cru_id != cruId) doPrint = false;
      int cru_lid = CRUh.link_id + CRUh.dpw_id*12;
      if((linkId >= 0) && (cru_lid != linkId)) doPrint = false;
      //printf("linkId %d  cru_lid %d  doPrint %d\n", linkId, cru_lid, (int)doPrint);
      if(doPrint) {
	//if( nFrames%1 == 0 )
	printf("%6d:  version %X  memsz %4d  offset %4d  packet %3d  srcID %d  cruID %2d  dp %d  link %2d  orbit %u  bc %4d  trig 0x%08X  page %d  stop %d",
	       nFrames, (int)rdh.version, (int)rdh.memorySize, (int)rdh.offsetToNext,
	       (int)rdh.packetCounter, (int)rdh.sourceID, (int)rdh.cruID, (int)rdh.endPointID, (int)rdh.linkID,
	       rdh.orbit, (int)rdh.bunchCrossing, rdh.triggerType, (int)rdh.pageCnt,
	       (int)rdh.stopBit);
	if( (rdh.triggerType & 0x800) != 0 ) printf(" <===");
	printf("\n");
      }
      if( CRUh.header_version < 4 || CRUh.header_version > 6 || CRUh.header_size != 64 ) break;
      if( BLOCK_SIZE > 0 ) read_header = false;
      nFrames++; continue;
    }
    //continue;

    payload_buf = (uint32_t*)realloc(payload_buf,BLOCK_SIZE);
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

    if(CRUh.cru_id != cruId) continue;

    int cru_lid = CRUh.link_id + CRUh.dpw_id*12;
    if((linkId >= 0) && (cru_lid != linkId)) continue;

    bool orbit_jump = true;
    int Dorbit1 = CRUh.hb_orbit - hb_orbits[cru_lid];
    int Dorbit2 = (uint32_t)( ((uint64_t)CRUh.hb_orbit) + 0x100000000 - hb_orbits[cru_lid] );
    if( Dorbit1 >=0 && Dorbit1 <= 1 ) orbit_jump = false;
    if( Dorbit2 >=0 && Dorbit2 <= 1 ) orbit_jump = false;

    bool packet_jump = true;
    int Dpacket1 = CRUh.packet_counter - packet_counter;
    int Dpacket2 = (uint32_t)( ((uint64_t)CRUh.packet_counter) + 0x100 - packet_counter );
    if( Dpacket1 >=0 && Dpacket1 <= 1 ) packet_jump = false;
    if( Dpacket2 >=0 && Dpacket2 <= 1 ) packet_jump = false;
    packet_jump = false;
    if( /*static_cast<int>(CRUh.packet_counter) == 0 ||*/ orbit_jump|| packet_jump ) {
      //printf("Resetting decoding FSM for link %d\n", cru_lid);
      for(int i = 0; i < 40; i++) {
	DualSampaReset( &(ds[cru_lid][i]) );
	ds[cru_lid][i].id = i;
	ds[cru_lid][i].nbHit = -1;
	for(int j=0;j<64;j++) {
	  ds[cru_lid][i].nbHitChan[j]=0;
	}
      }
      for(int i = 0; i < 8; i++) {
	DualSampaGroupReset( &(dsg[cru_lid][i]) );
      }
    }
    hb_orbits[cru_lid] = CRUh.hb_orbit;
    packet_counter = CRUh.packet_counter;

    if( orbit_jump ) {
      skip_time_frame = !skip_time_frame;
    }
    if( skip_time_frame ) {
      //printf("Frame skipped\n");
      //continue;
    }



    for(int i = 0; i < 40; i++) {
      out_word[i] = 0;
      n10bitwords[i] = 0;
    }

    int nGBTwords = CRUh.block_length / 16;
    for( int wi = 0; wi < nGBTwords; wi++) {

      uint32_t* ptr = payload_buf + wi*4;
      uint32_t hhvalue, hlvalue, lhvalue, llvalue;
      hhvalue = ptr[3];
      hlvalue = ptr[2];
      lhvalue = ptr[1];
      llvalue = ptr[0];
      //fprintf(stdout,"wi=%d  %.8X %.8X %.8X %.8X\n", wi,
      //      hhvalue, hlvalue, lhvalue, llvalue);
      ProcessGBTWord(ptr, cru_lid);
    }
    //printf("=========\n");
    n++;
  }
}




void ReadFiltered(int fin)
{
  int cru_lid = 0;
  for(int i = 0; i < 40; i++) {
    out_word[i] = 0;
    n10bitwords[i] = 0;
  }

  uint32_t GBTword[4];
  while(true) {
    int nread = read( fin, GBTword, sizeof(GBTword) );
    if( nread != sizeof(GBTword) ) break;

    ProcessGBTWord(GBTword, cru_lid);
  }
}




int main(int argc, char** argv)
{
  int fin = open(argv[argc-1], O_RDONLY);
  printf("reading file \"%s\"\n", argv[argc-1]);
  int level = 0;
  gPrintLevel = level;  // Global variable defined as extern in the others .cxx files
  gPattern = 0;
  gNbErrors = 0;
  gNbWarnings = 0;
  int boardnum = -1;
  bool print_noise = false;


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
    case 'p' :
      i++;
      //	  InputFile = argv[i];
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
      cruId=atoi(argv[i]);
      break;
    case 'l' :
      i++;
      linkId=atoi(argv[i]);
      break;
    }
  }

  if( board_vec.empty() ) {
    for(int i = 0; i < 40*24 ; i++) {
      board_vec.push_back(i);
    }
  }

  int NbCardStart = 0;
  int NbCardEnd = 39;
  if(boardnum >= 0) {
    NbCardStart = boardnum;
    NbCardEnd = NbCardStart;
  }

  char tstr[500];

  for(int c = 0; c < 24; c++) {
    for(int i = 0; i < 40; i++) {
      DualSampaInit( &(ds[c][i]) );
      ds[c][i].id = i;
      ds[c][i].lid = c;
      ds[c][i].nbHit = -1;
      for(int j=0;j<64;j++) {
        ds[c][i].nbHitChan[j]=0;
      }
    }
    for(int i = 0; i < 8; i++) {
      DualSampaGroupInit( &(dsg[c][i]) );
    }
  }

  fout = fopen("/home/data/data_CRU.txt","w");

  bool GBTxfound = false;
  int nframes = 0;

  ReadPIPE(fin);
  //ReadFiltered(fin);

  if(fout) fclose(fout);

  //int ffilter = open("/home/data/data-filtered.raw", O_CREAT|O_WRONLY|O_TRUNC);
  //write(ffilter,out_buf,out_buf_size);
  //close(ffilter);

  if( print_noise ) {
    FILE* fnoiseout = fopen("/tmp/ds_ch_exclude.txt", "w");

    for(int c = 0; c < 24; c++) {
      for(int i = 0; i < 40; i++) {
        float noise_tot[2] = {0}; int Nnoise[2] = {0};
        for(int j = 0; j < 2; j++) {
          for(int k = 0; k < 32; k++) {
            if( ds[c][i].ndata[j][k] > 0 ) {
              float ped = (float)(ds[c][i].pedestal[j][k]/ds[c][i].ndata[j][k]);
              float noise = (float)sqrt( ds[c][i].noise[j][k]/ds[c][i].ndata[j][k] - ped*ped );
              noise_tot[j] += noise; Nnoise[j] += 1;
              if(ped>150 || noise > 1.5) {
                printf("C %2d L %2d DS %2d CHIP %d CH %2d  %0.3f  %0.3f  %d %d\n",
		       cruId, c, i, j, k, ped, noise, ds[c][i].ndata[j][k], ds[c][i].nclus[j][k]);
		if(fnoiseout)
		  fprintf(fnoiseout, "%d %d %d %d %d  %0.3f  %0.3f  %d %d\n",
			  cruId, c, i, j, k, ped, noise, ds[c][i].ndata[j][k], ds[c][i].nclus[j][k]);
	      }
            }
          }
        }
        if( Nnoise[0] > 0 && Nnoise[1] > 0 )
          printf("L%d J%d DS%d average noise: %0.3f, %0.3f\n",c, ((i/5)+1), (i%5), noise_tot[0]/Nnoise[0],noise_tot[1]/Nnoise[1]);
      }
    }

    if(fnoiseout) fclose(fnoiseout);
  }

  for(int i = 0; i< 50; i++) 
    printf("-");
  printf("\n");
  printf("ERRORS: %.5d\t WARNINGS: %.5d\n",gNbErrors,gNbWarnings);
  for(int i = 0; i< 50; i++)
    printf("-");
  printf("\n\n");

  //for(unsigned int i = 0; i < board_vec.size(); i++) {
  //  int l = board_vec[i] / 40;
  //  int b = board_vec[i] % 40;
  //  printf("L%d B%d J%d DS%d: HB packets %d %d\n", l, board_vec[i], ((b/5)+1), (b%5), ds[l][b].nbHB[0], ds[l][b].nbHB[1]);
  //}

  for(unsigned int i = 0; i < board_vec.size(); i++) {
    int l = board_vec[i] / 40;
    int b = board_vec[i] % 40;
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 32; k++) {
        if( ds[l][b].ndata[j][k] == 0 ) {
          printf("L%d B%d J%d DS%d: missing data from chip %d channel %d\n", l, board_vec[i], ((b/5)+1), (b%5), j, k);
          break;
        }
      }
    }
  }

  /*
  for(int c = 0; c < 6; c++) {
    for(int i = 0; i< 8; i++) {
      for(int j = 0; j< 5; j++) {
        int k=5*i+j;
        for(int l=1;l>=0;l--) {
          uint64_t hits=0;
          for(int m = 31; m>=0; m--) {
            hits=(hits<<1)+(ds[c][k].nbHitChan[m+32*l]>0);
            //printf("%.2d - %.1d %.16X\n",jj,ds[i].nbHitChan[j+32*k],hits);
          }
          printf("%.8X",hits);
        }
        printf(" ");
      }
      printf("\n");
    }
    for(int i = 0; i< 50; i++)
      printf("-");
    printf("\n");
    for(int i = 0; i< 8; i++) {
      printf("{J%d}:: ",i+1);
      for(int j = 0; j< 5; j++) {
        int k=5*i+j;
        if(ds[c][k].nbHit>=0)
          printf("[%.2d]%.5d\t",k,ds[c][k].nbHit);
        else
          printf("[%.2d]-----\t",k);
      }
      printf("\n");
    }
    printf("\n===================\n");
  }
  */
}
