//#define SCA_I2C_DEBUG
#ifdef SCA_I2C_DEBUG
  #define debug(s)  std::cout << __func__ << " : " << s << std::endl;
#else
  #define debug(...)
#endif

#include "gbt_sca_i2c.hpp"

namespace gbt {

//------------------------------------------------------------------------------
const std::array<std::string, 8> ScaI2cCore::statusBitNames_ = {
  "Reserved(0)", "Reserved(1)", "Success", "LevelError",
  "Reserved(4)", "InvalidComm", "NoAck", "Reserved(7)"
};

const std::array<std::string, 4> ScaI2cCore::freqNames_ = {
  "100k", "200k", "400k", "1M"
};

//------------------------------------------------------------------------------
uint8_t
ScaI2cGbtx::readByte(uint32_t daddr, uint32_t raddr) const
{
  setNbyte(1);
  uint32_t sd = transceive(I2C_S_7B_W, (raddr << 8) | (daddr & 0xffff)).data;

  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C multi byte write 7b :" + getStatusBitNames(sd & 0xff), channel_);
  sd = transceive(I2C_S_7B_R, (daddr & 0x7f)).data;
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C single byte read 7b :" + getStatusBitNames(sd & 0xff), channel_);

/*
  setNbyte(2);
  transceive(I2C_W_DATA0, (raddr & 0xffff));

  uint32_t sd = transceive(I2C_M_7B_W, (daddr & 0x7f)).data;
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C multi byte write 7b :" + getStatusBitNames(sd & 0xff), channel_);

  sd = transceive(I2C_S_7B_R, (uint32_t(1) << 8) | (daddr & 0x7f)).data;
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C single byte read 7b :" + getStatusBitNames(sd & 0xff), channel_);
*/
  return uint8_t((sd >> 8) & 0xff);
}

uint64_t
ScaI2cGbtx::readMByte(uint32_t daddr, uint32_t raddr, uint32_t size) const
{
setNbyte(1);
  //printf("COMMAND 1:\n");
  uint32_t sd = transceive(I2C_S_7B_W, (raddr << 8) | (daddr & 0xffff)).data;
//return 0;
  //setNbyte(size);
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C multi byte write 7b :" + getStatusBitNames(sd & 0xff), channel_);
  //printf("COMMAND 2:\n");
  sd = transceive(I2C_M_7B_R, (daddr & 0x7f)).data;
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C single byte read 7b :" + getStatusBitNames(sd & 0xff), channel_);

  uint32_t result = 0;
  for(uint32_t idx = 0; idx < size; idx += 4) {
    //printf("  idx=%d  size=%d\n",idx,size);
    switch(idx) {
    case 0:
  //printf("COMMAND 3:\n");
      sd = transceive(I2C_R_DATA3, (daddr & 0xffff)).data;
      //printf("DATA3: sd = 0x%X\n",sd);
      result += sd & 0xffffffff;
      //printf("DATA3: result = 0x%X\n",result);
      break;
    case 4:
  //printf("COMMAND 4:\n");
      sd = transceive(I2C_R_DATA2, (daddr & 0xffff)).data;
      //printf("DATA2: sd = 0x%X\n",sd);
      result += (sd<<32) & 0xffffffff00000000;
      //printf("DATA2: result = 0x%X\n",result);
      break;
    default: break;
    }
  }
  return result;

/*
  setNbyte(2);
  transceive(I2C_W_DATA0, (raddr & 0xffff));

  uint32_t sd = transceive(I2C_M_7B_W, (daddr & 0x7f)).data;
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C multi byte write 7b :" + getStatusBitNames(sd & 0xff), channel_);

  sd = transceive(I2C_S_7B_R, (uint32_t(1) << 8) | (daddr & 0x7f)).data;
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C single byte read 7b :" + getStatusBitNames(sd & 0xff), channel_);
*/
  return uint8_t((sd >> 8) & 0xff);
}

uint8_t
ScaI2cGbtx::writeByte(uint32_t daddr, uint32_t raddr, uint8_t value) const
{
  setNbyte(1);
  uint32_t sd = transceive(I2C_S_7B_W, (value << 16) | (raddr & 0xffff)).data;
/*
  setNbyte(3);
  transceive(I2C_W_DATA0, (value << 16) | (raddr & 0xffff));

  uint32_t sd = transceive(I2C_M_7B_W, (daddr & 0x7f)).data;
*/
  if (!isSuccess(sd & 0xff))
    throw ScaException("I2C multi byte write 7b :" + getStatusBitNames(sd & 0xff), channel_);

  return readByte(daddr, raddr);
}

void
ScaI2cGbtx::pushByte(uint32_t daddr, uint32_t raddr, uint8_t value) const
{
  // Set up I2C to transmit 3 bytes
  //   Push default values for i2c_freq_ and scl_mode, as we can't read
  //   back anything
  uint8_t scl_mode(0);
  transmit(I2C_W_CTRL, (scl_mode << 7) | (3 << 2) | (i2c_freq_ & 0x3));
  transmit(I2C_W_DATA0, (value << 16) | (raddr & 0xffff));
  transmit(I2C_M_7B_W, (daddr & 0x7f));
}


//------------------------------------------------------------------------------
uint8_t
ScaI2cSampa::readByte(uint32_t daddr, uint32_t raddr) const
{
  debug("SCA CH " << static_cast<uint32_t>(getChannel())
         << ", I2C dev addr " << daddr << ", reg " << raddr << ", 10b addr 0x"
         << std::setw(4) << std::setfill('0') << std::hex
         << addressToD1D0(daddr, raddr)
         << std::dec << std::setfill(' '))

  uint32_t sd = transceive(I2C_S_10B_R, addressToD1D0(daddr, raddr)).data;
  if (!isSuccess(sd & 0xf7))
    throw ScaException("I2C single byte read 10b :" + getStatusBitNames(sd & 0xff), channel_);

  return (sd >> 8) & 0xff;
}

uint8_t
ScaI2cSampa::writeByte(uint32_t daddr, uint32_t raddr, uint8_t value) const
{
  debug("SCA CH " << static_cast<uint32_t>(getChannel())
         << ", I2C dev addr " << daddr << ", reg " << raddr << ", val 0x"
         << std::hex << static_cast<uint32_t>(value) << ", 10b addr 0x"
         << std::setw(4) << std::setfill('0')
         << addressToD1D0(daddr, raddr)
         << std::dec << std::setfill(' '))

    //setNbyte(1);
  uint32_t sd = transceive(I2C_S_10B_W, (uint32_t(value) << 16) |
                                        addressToD1D0(daddr, raddr)).data;
  //printf("  sd = 0x%X\n",sd);

  if (!isSuccess(sd & 0xf7))
    throw ScaException("I2C single byte write 10b :" + getStatusBitNames(sd & 0xff), channel_);

  return(sd & 0xff);
  //return readByte(daddr, raddr);
}

void
ScaI2cSampa::pushByte(uint32_t daddr, uint32_t raddr, uint8_t value) const
{
  debug("SCA CH " << static_cast<uint32_t>(getChannel())
         << ", I2C dev addr " << daddr << ", reg " << raddr << ", val 0x"
         << std::hex << static_cast<uint32_t>(value) << ", 10b addr 0x"
         << std::setw(4) << std::setfill('0')
         << addressToD1D0(daddr, raddr)
         << std::dec << std::setfill(' '))

  transmit(I2C_S_10B_W, (uint32_t(value) << 16) | addressToD1D0(daddr, raddr));
}

} // namespace gbt

#undef debug
