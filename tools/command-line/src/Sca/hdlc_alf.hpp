#pragma once

#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>

#include "hdlc_core.hpp"

#include "../../ALF/include/Alf/Alf.h"

using namespace o2::alf;

namespace cru {

/** Interface the HDLC core provided by CERN Microelectronics Group
 *    (also controls the SCA core multiplexer implemented by the CRU core team)
 */
class HdlcAlf : public common::HdlcCore
{
  public:
    /** Constructor
     *  @warn Do not instantiate 2 of these objects, as the constructor sets the
     *        SCA core mux to reflect the SCA index. Instead, only use one object
     *        and potentially use setScaCoreMux to adjust which link/SCA you're talking to.
     *        (For compatibility reasons, it is not forbidden to construct 2 instances of this
     *        object, neither is the structure of the HdlcCore classes changed to properly
     *        reflect these changes)
     *  @see  setScaCoreMux
     */
    HdlcAlf(const roc::Parameters::CardIdType& cardId, uint8_t sca_idx = 0x0, bool request_trid_auto = false) :
        HdlcCore(request_trid_auto, false), sca(cardId, sca_idx)
      {
      //sca.txReady();
      //std::cout<<"sca_idx: "<<(int)sca_idx<<std::endl;
        // Adjust valid request trid stored in HDLC core with last know valid value
        request_trid_ = 123;
      }

    //~HdlcAlf() { std::cout<<"HdlcAlf destructor called"<<std::endl; }


    void     transmitCommand(HdlcEcFramePayload& request) final override;
    uint32_t executeCommand(HdlcEcFramePayload& request, HdlcEcFramePayload& reply) final override;

    /** Send supervisory level command "CONNECT" to SCA (see SCA manual p.11)
     */
    void sendSvlConnect() final override { sca.svlConnect(); }

    /** Send supervisory level command "TEST" to SCA (see SCA manual p.11)
     */
    void sendSvlTest() final override { std::cout << __func__ << " not implemented for HdlcAlf core\n"; }

    /** Send supervisory level command "RESET" to SCA (see SCA manual p.11)
     */
    void sendSvlReset() final override { sca.svlReset(); }

    /** Reset SCA core
     */
    void rst() final override
    {
      sendSvlReset();
      sca.scReset();
      //std::this_thread::sleep_for(std::chrono::milliseconds(100));
      //sendSvlConnect();
      //std::this_thread::sleep_for(std::chrono::milliseconds(100));
      //sca.waitOnBusyClear();
    }


    /** Print status information
     */
    std::ostream& printStatus(std::ostream& os) const final override
      {
        return os;
      }

  protected:
    void     sendFrame_(HdlcEcFramePayload& r) const final override;

  private:
    static const uint32_t REG_WR_DATA    {0x00/4}; // {0x20/4};    // HDLC payload write data part

    static const uint32_t REG_WR_CMD     {0x04/4}; // {0x24/4};    // HDLC payload write cmd part
      static const uint8_t REQ_TRID_LSB {16};
      static const uint8_t REQ_TRID_WDT {8};

    static const uint32_t REG_CTRL        {0x08/4}; // {0x28/4};    // Ctrl, write-related
      static const uint8_t B_SEND_RST     {0};
      static const uint8_t B_SEND_CONNECT {1};
      static const uint8_t B_SEND_CMD     {2};
      static const uint8_t F_SEND_CTRL_BITS_LSB {0};         // Field for 3 bits controlling the sending of
      static const uint8_t F_SEND_CTRL_BITS_WDT {3};        //   frames
      static const uint8_t B_CORE_RST     {23};

    static const uint32_t REG_RD_DATA     {0x10/4}; // {0x30/4};    // HDLC payload read data part

    static const uint32_t REG_RD_CMD      {0x14/4}; // {0x34/4};    // HDLC payload read cmd part

    static const uint32_t REG_CTRL2       {0x18/4}; // {0x38/4};    // Ctrl, read-related
      static const uint8_t B_CORE_TX_BUSY {30};             // TX busy    FIXME: Add support for this
      static const uint8_t B_CORE_BUSY    {31};             // RX busy

    static const uint32_t REG_MON         {0x1c/4}; // {0x3c/4};    // Monitoring
      static const uint8_t F_TX_TO_RX_CYCLES_LSB {0};       // Time from sending of a command (raise busy) to
      static const uint8_t F_TX_TO_RX_CYCLES_WDT {28};      //   reception of the reply (release busy) FIXME: careful, might change in hardware!

    static const uint32_t REG_SC_RST {0x64/4};        // This is access in the CRU team python tool in the init() of the SCA object only
      static const uint8_t B_SC_RST  {0};

      o2::alf::Sca sca;

    /**  Get HDLC frame payload content from SCA core registers and disassemble it
     *   @note Just pulls register content and disassembles it, but does not check whether
     *         received frame is avaible
     *   @param r Reference to store disabled frame data
     *   @return SCA error code (0x0 means no error)
     */
    inline uint32_t getFrame(HdlcEcFramePayload& r);

    /** Read previous request transaction ID from request register
     *    (and recover from potential illegal values)
     *
     *  @return Request TRID in the range [1..254] and different from the current register
     *            value, typically reg_value+1
     */
    /*inline uint8_t getLastValidRequestTrid()
      {
        uint8_t req_trid = bar_.readField(REG_WR_CMD, REQ_TRID_LSB, REQ_TRID_WDT);
        if ((req_trid == 0xfe) || (req_trid == 0xff))
          req_trid = 0x0;
        return req_trid++;
      }*/

    /** Transfer payload struct content to write register
     *    and adjust to correct format for this core
     */
    inline void
    assembleRegisters(const HdlcEcFramePayload& r, uint32_t& w2, uint32_t& w3) const {
      //std::cout<<"channel "<<(int)r.channel<<"  trid "<<(int)r.trid<<"  length "<<(int)r.length<<"  command "<<(int)r.command<<std::endl;
        w2 = ((r.channel << 24) |
              (r.trid    << 16) |
              (r.length  << 8)  |
              (r.command << 0));
        w3 = ((r.data & 0xff)       << 24) |
             ((r.data & 0xff00)     << 8) |
             ((r.data & 0xff0000)   >> 8) |
             ((r.data & 0xff000000) >> 24);
      }

    /** Pack register read values into payload struct
     *    and adjust to correct format for this core
     */
    inline void
    disassembleRegisters(HdlcEcFramePayload& r, uint32_t w2, uint32_t w3) const {
        r.channel = (w2 >> 24) & 0xff;
        r.trid    = (w2 >> 16) & 0xff;
        r.length  = (w2 >> 8) & 0xff;
        r.error   = (w2 >> 0) & 0xff;
        r.data = ((w3 & 0xff)       << 24) |
                 ((w3 & 0xff00)     << 8) |
                 ((w3 & 0xff0000)   >> 8) |
                 ((w3 & 0xff000000) >> 24);
      }

};

} // namespace cru
