#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "sampa_header.hpp"

extern int gPrintLevel;
extern int gPattern;
extern int gNbErrors;
extern int gNbWarnings;



enum DualSampaStatus {
    notSynchronized              = 1,
    synchronized                 = 2,
    headerToRead                 = 3,
    sizeToRead                   = 4,
    timeToRead                   = 5,
    dataToRead                   = 6,
    chargeToRead                 = 7,
    OK                           = 8   // Data block filled (over a time window)
};


struct DualSampa
{
  int id;
  int lid;
  DualSampaStatus status;            // Status during the data filling
  uint64_t data;                // curent data
  int bit;                           // current position
  uint64_t powerMultiplier;     // power to convert to move bits
  int nsyn2Bits;                     // Nb of words waiting synchronization
  Sampa::SampaHeaderStruct header;   // current channel header
  int nHeaders;             // number of headers encoutered since the last SYNC  
  int64_t bxc[2];
  uint32_t csize, ctime, cid, sample, csum;
  int chan_addr[2];
  uint64_t packetsize;
  int nbHit; // incremented each time a header packet is received for this card
  int nbHB[2]; // incremented each time a HertaBeat packet is received for this card
  int nbHitChan[64]; // incremented each time a header packet for a given packet is received for this card
  int ndata[2][32];
  int nclus[2][32];
  double pedestal[2][32], noise[2][32];
};


struct DualSampaGroup
{
  int64_t bxc;
};


enum decode_state_t
{
  DECODE_STATE_UNKNOWN,
  DECODE_STATE_SYNC_SEARCH,
  DECODE_STATE_SYNC_FOUND,
  DECODE_STATE_HEADER_FOUND,
  DECODE_STATE_HEARTBEAT_FOUND,
  DECODE_STATE_CSIZE_FOUND,
  DECODE_STATE_CTIME_FOUND,
  DECODE_STATE_SAMPLE_FOUND,
  DECODE_STATE_PACKET_END
};


void DualSampaInit(DualSampa* ds);
void DualSampaReset(DualSampa* ds);
void DualSampaGroupInit(DualSampaGroup* dsg);
void DualSampaGroupReset(DualSampaGroup* dsg);
int CheckDataParity(uint64_t data);
void HammingDecode(unsigned int buffer[2], bool& error, bool& uncorrectable, bool fix_data);
void DecodeGBTWord(uint32_t* bufpt, uint32_t* data);
//void DecodeCRUWord(uint32_t* bufpt, uint32_t* data);
decode_state_t Add1BitOfData(uint64_t gbtdata, DualSampa& ds, DualSampaGroup* dsg=NULL);

/*
inline void DecodeGBTWord(uint32_t* bufpt, uint32_t* data)
{
  uint32_t word = *(bufpt+3);
  //printf("*(bufpt+3): %.8X\n", word);
  data[0] =  (((word)>>0)&0x1)<<1;
  data[0] += (((word)>>1)&0x1);

  //return;

  data[1] =  (((word)>>2)&0x1)<<1;
  data[1] += (((word)>>3)&0x1);
  data[2] =  (((word)>>4)&0x1)<<1;
  data[2] += (((word)>>5)&0x1);
  data[3] =  (((word)>>6)&0x1)<<1;
  data[3] += (((word)>>7)&0x1);

  return;

  data[4] =  (((word)>>8)&0x1)<<1;
  data[4] += (((word)>>9)&0x1);

  data[5] =  (((word)>>10)&0x1)<<1;
  data[5] += (((word)>>11)&0x1);
  data[6] =  (((word)>>12)&0x1)<<1;
  data[6] += (((word)>>13)&0x1);
  data[7] =  (((word)>>14)&0x1)<<1;
  data[7] += (((word)>>15)&0x1);
  data[8] =  (((word)>>16)&0x1)<<1;
  data[8] += (((word)>>17)&0x1);
  data[9] =  (((word)>>18)&0x1)<<1;
  data[9] += (((word)>>19)&0x1);

  data[10] =  (((word)>>20)&0x1)<<1;
  data[10] += (((word)>>21)&0x1);
  data[11] =  (((word)>>22)&0x1)<<1;
  data[11] += (((word)>>23)&0x1);
  data[12] =  (((word)>>24)&0x1)<<1;
  data[12] += (((word)>>25)&0x1);
  data[13] =  (((word)>>26)&0x1)<<1;
  data[13] += (((word)>>27)&0x1);
  data[14] =  (((word)>>28)&0x1)<<1;
  data[14] += (((word)>>29)&0x1);

  data[15] =  (((word)>>30)&0x1)<<1;
  data[15] += (((word)>>31)&0x1);

  word = *(bufpt+2);
  data[16] =  (((word)>>0)&0x1)<<1;
  data[16] += (((word)>>1)&0x1);
  data[17] =  (((word)>>2)&0x1)<<1;
  data[17] += (((word)>>3)&0x1);
  data[18] =  (((word)>>4)&0x1)<<1;
  data[18] += (((word)>>5)&0x1);
  data[19] =  (((word)>>6)&0x1)<<1;
  data[19] += (((word)>>7)&0x1);

  data[20] =  (((word)>>8)&0x1)<<1;
  data[20] += (((word)>>9)&0x1);
  data[21] =  (((word)>>10)&0x1)<<1;
  data[21] += (((word)>>11)&0x1);
  data[22] =  (((word)>>12)&0x1)<<1;
  data[22] += (((word)>>13)&0x1);
  data[23] =  (((word)>>14)&0x1)<<1;
  data[23] += (((word)>>15)&0x1);
  data[24] =  (((word)>>16)&0x1)<<1;
  data[24] += (((word)>>17)&0x1);

  data[25] =  (((word)>>18)&0x1)<<1;
  data[25] += (((word)>>19)&0x1);
  data[26] =  (((word)>>20)&0x1)<<1;
  data[26] += (((word)>>21)&0x1);
  data[27] =  (((word)>>22)&0x1)<<1;
  data[27] += (((word)>>23)&0x1);
  data[28] =  (((word)>>24)&0x1)<<1;
  data[28] += (((word)>>25)&0x1);
  data[29] =  (((word)>>26)&0x1)<<1;
  data[29] += (((word)>>27)&0x1);

  data[30] =  (((word)>>28)&0x1)<<1;
  data[30] += (((word)>>29)&0x1);
  data[31] =  (((word)>>30)&0x1)<<1;
  data[31] += (((word)>>31)&0x1);

  word = *(bufpt+1);
  data[32] =  (((word)>>0)&0x1)<<1;
  data[32] += (((word)>>1)&0x1);
  data[33] =  (((word)>>2)&0x1)<<1;
  data[33] += (((word)>>3)&0x1);
  data[34] =  (((word)>>4)&0x1)<<1;
  data[34] += (((word)>>5)&0x1);

  data[35] =  (((word)>>6)&0x1)<<1;
  data[35] += (((word)>>7)&0x1);
  data[36] =  (((word)>>8)&0x1)<<1;
  data[36] += (((word)>>9)&0x1);
  data[37] =  (((word)>>10)&0x1)<<1;
  data[37] += (((word)>>11)&0x1);
  data[38] =  (((word)>>12)&0x1)<<1;
  data[38] += (((word)>>13)&0x1);
  data[39] =  (((word)>>14)&0x1)<<1;
  data[39] += (((word)>>15)&0x1);
}

*/
