//
// Struct that defines SAMPA Header
//
// A. Baldisseri (Feb. 2017)
//


#ifndef SAMPA_HEADERSTRUCT_H
#define SAMPA_HEADERSTRUCT_H

namespace Sampa
{
struct SampaHeaderStruct {
    unsigned long
    fHammingCode          : 6,
    fHeaderParity         : 1,
    fPkgType              : 3,
    fNbOf10BitWords       : 10,
    fChipAddress          : 4,
    fChannelAddress       : 5,
    fBunchCrossingCounter : 20,
    fPayloadParity        : 1,
    fUnused               : 14;
};
    
//    struct SampaHeaderStruct {
//        unsigned int fHammingCode          : 6;
//        unsigned int fHeaderParity         : 1;
//        unsigned int fPkgType              : 3;
//        unsigned int fNbOf10BitWords       : 10;
//        unsigned int fChipAddress          : 4;
//        unsigned int fChannelAddress       : 5;
//        unsigned int fBunchCrossingCounter : 20;
//        unsigned int fPayloadParity        : 1;
//    };
    

//    struct SampaHeaderStruct {
//        
//        unsigned int fPayloadParity        : 1;
//        unsigned int fBunchCrossingCounter : 20;
//        unsigned int fChannelAddress       : 5;
//        unsigned int fChipAddress          : 4;
//        unsigned int fNbOf10BitWords       : 10;
//        unsigned int fPkgType              : 3;
//        unsigned int fHeaderParity         : 1;
//        unsigned int fHammingCode          : 6;
//
//        
//    };

static void SampaHeaderDump(struct SampaHeaderStruct* sampaHeader)
{
    printf("--> SAMPA Header: HCode %d HPar %d PkgType %d 10BitWords %d ChipAdd %d ChAdd %d BX %d PPar %d",
           sampaHeader->fHammingCode,sampaHeader->fHeaderParity,sampaHeader->fPkgType,
           sampaHeader->fNbOf10BitWords,sampaHeader->fChipAddress,sampaHeader->fChannelAddress,
           sampaHeader->fBunchCrossingCounter,sampaHeader->fPayloadParity);
    printf("\n");

}
    
    
}

#endif // SAMPA_HEADERSTRUCT_H
