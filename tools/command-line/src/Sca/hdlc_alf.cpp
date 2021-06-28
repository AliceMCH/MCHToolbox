#include "hdlc_alf.hpp"

namespace cru {

  static int counter = 0;
  static int tid = 1;

void
HdlcAlf::sendFrame_(HdlcEcFramePayload& r) const
{
  //printf("sendFrame_()\n");
  //uint32_t wdata, wcmd;
  //assembleRegisters(r, wcmd, wdata);
  //sca.write(wcmd, wdata);
  //printf("%08X,%08X\n",wcmd,wdata); counter++;
}

uint32_t
HdlcAlf::getFrame(HdlcEcFramePayload& r)
{
  //printf("getFrame()\n");

  //o2::alf::Sca::CommandData cmdData = sca.read();


  //uint32_t w2 = cmdData.command, , w3cmdData.data;
  //disassembleRegisters(r, w2, w3);
  //printf("%08X,%08X  ch=%d  error=%02X  data=%08X\n",w2,w3,(int)r.channel,(int)r.error,(int)r.data);
  //disassembleRegisters(r, bar_.read(base_addr_ + REG_RD_CMD), bar_.read(base_addr_ + REG_RD_DATA));
  return r.error;
}

void
HdlcAlf::transmitCommand(HdlcEcFramePayload& request)
{
  //printf("transmitCommand()\n");
  //sca.waitOnBusyClear();
  //sendFrame(request);
}

uint32_t
HdlcAlf::executeCommand(HdlcEcFramePayload& request, HdlcEcFramePayload& reply)
{
  adjustRequestTrid(request);
  uint32_t wdata, wcmd;
  assembleRegisters(request, wcmd, wdata);
  //printf("%08X,%08X\n",wcmd,wdata); counter++;
  try {
    o2::alf::Sca::CommandData cmdData = sca.executeCommand(wcmd, wdata, false);
    uint32_t w2 = cmdData.command, w3 = cmdData.data;
    disassembleRegisters(reply, w2, w3);
  }
  catch(...) {
    std::cerr <<
        "Unexpected exception, diagnostic information follows:\n" <<
        boost::current_exception_diagnostic_information();
    return 1;
  }

  return 0;  // retry not implemented, number of retries -> 0
}

} // namespace cru
