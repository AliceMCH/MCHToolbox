#include "hdlc_cern_me.hpp"

namespace cru {

  static int counter = 0;
  static int tid = 1;

void
HdlcCernMe::sendFrame_(HdlcEcFramePayload& r) const
{
  //printf("sendFrame_()\n");
  uint32_t wdata, wcmd;
  assembleRegisters(r, wcmd, wdata);
  bar_.write(base_addr_ + REG_WR_DATA, wdata);
  bar_.write(base_addr_ + REG_WR_CMD, wcmd);
  //printf("HdlcCernMe::sendFrame_: 0x%X + 0x%X, 0x%X\n",base_addr_,REG_WR_DATA,wdata);
  //printf("HdlcCernMe::sendFrame_: 0x%X + 0x%X, 0x%X\n",base_addr_,REG_WR_CMD,wcmd);
  //printf("%08X,%08X@v%d\n",wdata,REG_WR_DATA+((tid<<16)&0xFF0000),counter); counter++;
  //tid++; if( (tid&0xFF) == 0xFF ) tid = 1;
  //printf("%08X,%08X@v%d\n",wcmd,REG_WR_CMD+((tid<<16)&0xFF0000),counter); counter++;
  //tid++; if( (tid&0xFF) == 0xFF ) tid = 1;
  //printf("%08X,%08X@v%d\n",wcmd,wdata,counter); counter++;
  printf("%08X,%08X\n",wcmd,wdata); counter++;
  startFrameTx();
  //printf("sendFrame_() end\n");
}

uint32_t
HdlcCernMe::getFrame(HdlcEcFramePayload& r) const
{
  //printf("getFrame()\n");
  uint32_t w2, w3;
  w2 = bar_.read(base_addr_ + REG_RD_CMD);
  w3 = bar_.read(base_addr_ + REG_RD_DATA);
  disassembleRegisters(r, w2, w3);
  printf("%08X,%08X  ch=%d  error=%02X  data=%08X\n",w2,w3,(int)r.channel,(int)r.error,(int)r.data);
  //disassembleRegisters(r, bar_.read(base_addr_ + REG_RD_CMD), bar_.read(base_addr_ + REG_RD_DATA));
  return r.error;
}

void
HdlcCernMe::transmitCommand(HdlcEcFramePayload& request)
{
  //printf("transmitCommand()\n");
  std::chrono::high_resolution_clock::time_point ts = std::chrono::high_resolution_clock::now();
  while (!txReady()) {
    if ((std::chrono::high_resolution_clock::now() - ts) > transceive_timeout_) {
      throw HdlcException("Rx timeout - SCA core busy flag stuck?");
    }
  }

  sendFrame(request);
}

uint32_t
HdlcCernMe::executeCommand(HdlcEcFramePayload& request, HdlcEcFramePayload& reply)
{
  //printf("executeCommand()\n");
  //std::cout<<"bar_.read(3C0006)="<<bar_.read(0x3C0006)<<std::endl;
  transmitCommand(request);
  //printf("executeCommand() after transmitCommand()\n");

  std::chrono::high_resolution_clock::time_point ts = std::chrono::high_resolution_clock::now();
  while (!rxReady()) {
    if ((std::chrono::high_resolution_clock::now() - ts) > transceive_timeout_) {
      throw HdlcException("Rx timeout");
    }
  }

  //printf("executeCommand() before getFrame()\n");
  getFrame(reply);
  //std::cout<< "       -> Request frame: {" << request << "}\n"
  //    << "       -> Reply frame  : {" << reply << "}"<<std::endl;
  //printf("executeCommand() after getFrame()\n");

  return 0;  // retry not implemented, number of retries -> 0
}

} // namespace cru
