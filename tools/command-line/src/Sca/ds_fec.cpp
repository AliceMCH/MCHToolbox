#include <unistd.h>
#include "ds_fec.hpp"

//------------------------------------------------------------------------------
const DsFecAdc::port_map_t
DsFecAdc::ports_ = {
    {18, std::make_tuple(convCalibrationResistor, true, "uA", "ADC Current source")}, // 18
    //
    {19, std::make_tuple([](uint16_t adc_raw, float)->float{return float(adc_raw)/4096*3;}, false, "V", "Voltage 2r25 (conn)")},
    {20, std::make_tuple([](uint16_t adc_raw, float)->float{return float(adc_raw)/4096*3;}, false, "V", "Voltage 2r25 (in)")},
    //
    {21, std::make_tuple([](uint16_t adc_raw, float)->float{return float(adc_raw)/4096*6;}, false, "V", "Voltage 3r25 (conn)")},
    {22, std::make_tuple([](uint16_t adc_raw, float)->float{return float(adc_raw)/4096*6;}, false, "V", "Voltage 3r25 (in)")},
    //
    {23, std::make_tuple([](uint16_t adc_raw, float)->float{return float(adc_raw)/4096*3;}, false, "V", "Voltage 2v5")},
    {24, std::make_tuple([](uint16_t adc_raw, float)->float{return float(adc_raw)/4096*2;}, false, "V", "Voltage 1v5")},
    //
    {26, std::make_tuple(convRtd, true, "ºC", "RTD0 temperature")},
    {27, std::make_tuple(convRtd, true, "ºC", "RTD3 temperature")},
    {28, std::make_tuple(convRtd, true, "ºC", "RTD2 temperature")},
    {29, std::make_tuple(convRtd, true, "ºC", "RTD4 temperature")},
    //
    {15, std::make_tuple(convVtrx, false, "mA",  "VTRx current")},
    {16, std::make_tuple(convPT100, false, "ºC", "PT100 temperature")},
    {31, std::make_tuple(convScaTemp, false, "ºC", "SCA temperature")}
};

/*
std::ostream&
operator<<(std::ostream& os, const DsFecAdc::result_t& r)
{
  os << std::left << std::setw(20) << std::get<2>(r) << ": "
     << std::setw(7) << std::setprecision(4) << std::get<0>(r) << " "
     << std::setw(2) << std::get<1>(r)
     << "    [raw 0x" << std::right << std::hex << std::setw(4) << std::setfill('0')
     << std::get<3>(r) << "]" << std::left << std::dec << std::setfill(' ')
     << (((std::get<3>(r) >> 12) & 0x1) ? "  *** OVERFLOW ***" : "");
  return os;
}
 */

//------------------------------------------------------------------------------
const uint8_t DsFec::cfg_i2c_addr_gbtx_[DsFec::n_gbtx_] = {0xf, 0xe};                  /**< I2C addresses of GBTx0 and GBTx1 */
const uint8_t DsFec::cfg_i2c_addr_sampa_[DsFec::n_sampa_] = { /**< I2C addresses of the SAMPAs */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};

void
DsFec::init()
{
  Sampa& sampa2 = getSampa(0);
  Sampa* psampa2 = sampa_[0].get();
  //std::cout<<"DsFec::init: &sampa2="<<&sampa2<<std::endl;
  //std::cout<<"DsFec::init: psampa2="<<psampa2<<std::endl;

  // Set basic GBT SCA registers (CRB, CRC, CRD)
  if (verbosity_ >= 1)
    std::cout << "Configuring GBT SCA basic registers\n";
  //std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sca_basic_->transceive(sca_basic_->CMD_W_CRB, defaults_.scacfg_basic_crb_);
  //std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sca_basic_->transceive(sca_basic_->CMD_W_CRC, defaults_.scacfg_basic_crc_);
  //std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sca_basic_->transceive(sca_basic_->CMD_W_CRD, defaults_.scacfg_basic_crd_);

  /*if (verbosity_ >= 1)
    std::cout << "\n\nGetting SCA ID\n";
  sca_id_->setVersion(sca_id_->read());

  // Check if SCA version info (now that ADC channel should be enabled) was correct,
  //   update ADC instance if we guessed incorrectly before
  if (verbosity_ >= 1)
    std::cout<<"GBT-SCA unique ID: "<<std::hex<<sca_id_->read()<<std::dec<<std::endl;
  auto sca_version = sca_id_->getVersion();
  if (verbosity_ >= 1)
    std::cout << "\n\nUpdating SCA ID and version information\n";
  sca_id_->setVersion(sca_id_->read());
  if (sca_version != sca_id_->getVersion()) {
    common::HdlcCore& hdlc(sca_adc_->getHdlcCore());
    sca_adc_.reset((sca_id_->getVersion() == gbt::ScaId::SCA_V1) ?
        static_cast<gbt::ScaAdc*>(new gbt::ScaAdcVersion1(hdlc)) :
        static_cast<gbt::ScaAdc*>(new gbt::ScaAdcVersion2(hdlc)));
  }
  */

  // Basic configuration for GPIO
  if (verbosity_ >= 1)
    std::cout << "Configuring GBT SCA GPIO\n";
  //sca_gpio_->setDirection(defaults_.scacfg_gpio_direction_);
  //sca_gpio_->setDataOut(defaults_.scacfg_gpio_dataout_);

  // Configuration for ADC
  if (verbosity_ >= 1)
    std::cout << "Configuring GBT SCA ADC\n";
  sca_adc_->init();

  if (verbosity_ >= 1)
    std::cout << "Configuring GBT SCA I2C interfaces\n";
  //for (uint32_t i = 0; i < 2; i++)
  //  i2c_gbtx_[i]->setFreq();
  for (uint32_t i = 0; i < n_sampa_; i+=10) {
    if (verbosity_ >= 1)
      std::cout << "Configuring frequency of i2c_sampa "<<i<<"\n";
    i2c_sampa_[i]->setFreq();
  }
  i2c_sampa_[n_sampa_]->setFreq();
}


//------------------------------------------------------------------------------
void
DsFec::gbtxConfigure(uint8_t gbt_idx, const std::string& cfg_file) const
{
  std::ifstream gcfg(cfg_file, std::ios::in);
  if (!gcfg.is_open())
    throw std::runtime_error("Unable to open file " + cfg_file);

  if (verbosity_ >= 1)
    std::cout << "Configuring GBTx" << static_cast<uint32_t>(gbt_idx)
    << " with file " << cfg_file << "\n";

  const gbt::ScaI2c& i2c(*i2c_gbtx_[gbt_idx]);

  std::string line;
  uint32_t addr(0);
  uint32_t val;
  while (gcfg.good()) {
    if (gcfg.peek() == '#') {
      getline(gcfg, line);
      continue;
    }
    gcfg >> std::hex >> val;
    if (!gcfg.good())
      break;
    i2c.writeByte(addr, val & 0xff);
    addr++;
  }

  gcfg.close();

  // Force 'config ready'
  uint32_t cfg_done = i2c.writeByte(365, 0xaa);
  if (verbosity_ >= 2)
    std::cout << "Forcing GBTx config done to 0x" << std::hex
    << cfg_done << std::dec << "\n";

  if (verbosity_ >= 2)
    std::cout << "Forcing GBTx reset\n";
  gbtxReset(gbt_idx);
}


void
DsFec::gbtxDumpConfiguration(uint8_t gbt_idx, std::ostream& os) const
{
  const gbt::ScaI2c& i2c(*i2c_gbtx_[gbt_idx]);
  os << "#GBTx " << static_cast<uint32_t>(gbt_idx) << " configuration\n";
  for (uint32_t addr = 0; addr < 366; addr++)
    os << std::right << std::dec << std::setfill(' ') << std::setw(3)
  << addr << " : 0x"
  << std::hex << std::setfill('0') << std::setw(2)
  << static_cast<uint32_t>(i2c.readByte(addr)) << "\n";
}


void
DsFec::gbtxReset(uint8_t gbt_idx) const
{
  const gbt::ScaI2c& i2c(*i2c_gbtx_[gbt_idx]);
  uint8_t r = i2c.readByte(50) & 0xc7;
  i2c.writeByte(50, r | 0x38);
  i2c.writeByte(50, r);
}


int32_t
DsFec::gbtxI2cScan(uint8_t gbt_idx, std::ostream* const os, bool verbose) const
{
  int32_t addr(-1);
  for (uint8_t i = 1; i < 16; i++) {    // 0 is broadcast address
    try {
      if (os && verbose)
        *os << ".a" << static_cast<uint32_t>(i);
      const gbt::ScaI2c& gbt(*i2c_gbtx_[gbt_idx]);
      gbt.readByte(i, 62); // Phase aligner track mode register, but anything should do here
      if (os) {
        *os << (verbose ? " ->" : "")
                << "  Found GBTx" << static_cast<uint32_t>(gbt_idx)
                << " [I2C addr " << static_cast<uint32_t>(i)
                << ", SCA CH" << static_cast<uint32_t>(gbt.getChannel()) << "]\n";
      }
      addr = i;
      break;
    }
    catch (gbt::ScaException& e) { /* Ignore these here */ }
  }

  if ((addr == -1) && os)
    *os << (verbose ? " ->" : "")
    << "  Unable to detect GBTx" << static_cast<uint32_t>(gbt_idx) << " I2C"
    << " [SCA CH" << static_cast<uint32_t>(i2c_gbtx_[gbt_idx]->getChannel()) << "]\n";

  return addr;
}

//------------------------------------------------------------------------------
uint32_t
DsFec::sampaPwrCtrl(uint32_t value, uint32_t mask, uint32_t wait) const
{
  // Apply power SAMPA-by-SAMPA, wait inbetween
  for (uint8_t i = 0; i < n_sampa_; i++) {
    if ((mask >> i) & 0x1) {
      sampaPwr(i, value);
      std::this_thread::sleep_for(std::chrono::milliseconds(wait));
    }
  }

  return sampaPwrStatus();
}


bool
DsFec::sampaEnableElinks(uint8_t n, uint32_t sampa_mask) const
{
  uint8_t nchk(n);
  for (uint8_t i = 0; i < n_sampa_; i++) {
    if ((sampa_mask >> i) & 0x1)
      nchk &= i2c_sampa_[i]->writeByte(REG_SAMPA_SOCFG, n);
  }
  return (nchk == n);
}


uint32_t
DsFec::sampaI2cScan(std::ostream* const os) const
{
  uint32_t active_mask(0x0);
  int n1 = 0, n2 = 80;
  for (uint8_t i = n1; i < n2/*n_sampa_*/; i++) {
    try {
      const gbt::ScaI2c& sampa(*i2c_sampa_[i]);
      if (os) {
        *os << "  Checking SAMPA" << static_cast<uint32_t>(i)
                << " [I2C addr " << sampa.getSlaveAddress()
                << ", SCA CH" << static_cast<uint32_t>(sampa.getChannel()) << "]\n";
      }
      //sampa.readByte(0x7);
      sampa.writeByte(0xe, 0x5);
      if (os) {
        *os << "  Found SAMPA" << static_cast<uint32_t>(i)
                << " [I2C addr " << sampa.getSlaveAddress()
                << ", SCA CH" << static_cast<uint32_t>(sampa.getChannel()) << "]\n";
      }
      active_mask |= (1 << i);
    }
    catch(gbt::ScaException& e) { 
      /* Ignore these here */ 
      if(os) {
        *os << e.what() << std::endl;
      }
    }
    usleep(100000);
  }

  if (active_mask == 0x0)
    *os << "  Found no active SAMPAs\n";

  return active_mask;
}


uint32_t
DsFec::readUniqueID(std::ostream* const os) const
{
  //i2c_sampa_[n_sampa_]->writeByte(0x50, 0x0);
  //uint8_t chip_code = i2c_gbtx_[0]->readByte(0x50, 0x0);
  //if(os) *os << std::hex << std::setfill('0') << std::setw(2) << "Chip code: 0x"<<(int)chip_code<<std::endl;
  //return 0;
  uint64_t chip_ID = i2c_gbtx_[0]->readMByte(0x50, 0x1, 6);
  if(os) *os << std::hex << std::setfill('0') << std::setw(2) << "Unique ID: 0x"<<(int)chip_ID<<std::endl;
  return chip_ID;
}


uint32_t
DsFec::readTemperature(std::ostream* const os) const
{
  uint16_t temp = sca_adc_->sample(31) & 0xFFF;
  if(os) *os << std::dec << "Temperature: "<<temp<<" -> "<<(4300.0f - (float)temp*6.0f)/11.0f<<std::endl;
  uint16_t temp2 = sca_adc_->sample(10) & 0xFFF;
  if(os) *os << std::dec << "Temperature: "<<temp2<<" -> "<<(0.1782*(float)temp2 - 545.1)<<std::endl;
  uint16_t v1 = sca_adc_->sample(0) & 0xFFF;
  if(os) *os << std::dec << "Voltage: "<<v1<<" -> "<<((1.0f+27.4/3.32)*v1)/4096<<std::endl;
  uint16_t v2 = sca_adc_->sample(1) & 0xFFF;
  if(os) *os << std::dec << "Voltage: "<<v2<<" -> "<<((1.0f+12.7/6.81)*v2)/4096<<std::endl;
  uint16_t v3 = sca_adc_->sample(2) & 0xFFF;
  if(os) *os << std::dec << "Voltage: "<<v3<<" -> "<<((1.0f+3.32/4.22)*v3)/4096<<std::endl;
  return temp2;
}


static int32_t
convertSampaRegister(std::string a, std::ostream* const os=nullptr)
{
  int32_t ia = -1;
  if(a.find("'h") == 0) {
    std::string a2(a.begin()+2, a.end());
    std::istringstream issa(a2);
    issa >> std::hex >> ia;
  } else if (a.find("'b") == 0) {
    if(os) *os <<"WARNING: convertSampaRegister: binary format not supported!!!\n";
    else std::cerr<<"WARNING: convertSampaRegister: binary format not supported!!!\n";
    //std::string a2(a.begin()+2, a.end());
    //std::istringstream issa(a2);
    //issa >> std::bin >> ia;
    //if(os) *os<<"  a="<<a<<"  ia="<<ia<<std::endl;
  } else {
    std::istringstream issa(a);
    issa >> std::dec >> ia;
  }
  return ia;
}


bool
DsFec::sampaConfigure(std::string filename, int16_t sampa_id, uint8_t nRetry, bool readBack, std::ostream* const os) const
{
  if(false && os) *os <<"sampaConfigure("<<filename<<", "<<sampa_id<<") called"<<std::endl;
  const gbt::ScaI2c& sampa(*i2c_sampa_[sampa_id]);
  gbt::ScaI2c* psampa = i2c_sampa_[sampa_id].get();
  Sampa& sampa2 = getSampa(sampa_id);
  Sampa* psampa2 = sampa_[sampa_id].get();

  std::ifstream infile(filename);
  if( !infile ) {
    if(os) *os <<"Input file "<<filename<<" not correctly opened\n";
    return false;
  }
  std::string line;
  bool failed = false;
  while (std::getline(infile, line)) {
    //if(os) *os <<"Line: \""<<line<<"\"\n";
    if(line.empty()) continue;
    if(line[0] == '#') continue;
    std::istringstream iss(line);
    std::string a, b, c="-1";
    //if (!(iss >> a >> b)) { continue; } // error
    //if(os) *os <<"a: \""<<a<<"\"    b:\""<<b<<"\""<<std::endl;
    int32_t ia; // = convertSampaRegister(a,os);
    int32_t ib; // = convertSampaRegister(b,os);
    int32_t ic; // = convertSampaRegister(c,os);
    //if(os) *os<<"  ia: 0x"<<std::hex<<ia<<" ("<<std::dec<<ia<<")  ib: 0x"<<std::hex<<ib<<" ("<<std::dec<<ib<<")\n";
    sscanf(line.c_str(), "%i %i %i", &ic, &ia, &ib);
    if(ia < 0 || ib < 0) return false;

    sampa2.set_readback(readBack);
    sampa2.set_nretries(nRetry);

    // write register to SAMPA
    bool write_failed = true;
    try {
      if (false && os) {
        *os << "  Writing SAMPA register: SAMPA " << static_cast<uint32_t>(sampa_id)
	        << " [I2C addr " << sampa.getSlaveAddress()
	        << ", SCA CH" << static_cast<uint32_t>(sampa.getChannel()) <<"], [reg 0x"
	        << std::hex << ia << ", value 0x" << ib << std::dec << "]\n";
      }
      //uint8_t val = sampa.writeByte(ia, ib); //usleep(100000);
      //sampa.readByte(ia);
      bool val2;
      if( ic < 0 )
        val2 = sampa_[sampa_id]->writeRegister(ia, ib);
      else if( ic == 32 )
        val2 = sampa_[sampa_id]->writeChannelRegisterBroadcast(ia, ib);
      else
        val2 = sampa_[sampa_id]->writeChannelRegister(ic, ia, ib);
      if(false && os) *os << "  result: " << val2 << std::endl;
      if( val2 ) { write_failed = false; continue; }
      //if(true && os) *os << "  readback value: 0x" << std::hex << (int)val << std::dec << std::endl;
      //if( !readBack || ia == 0xe || ia == 0x1a ) { write_failed = false; continue; }
      //if( val == ib ) { write_failed = false; continue; }
      //if(os) *os << "ERROR: register 0x" << std::hex << ia
      //<< " -> readback value: 0x" << std::hex << (int)val
      //<< " - expected: 0x" << ib << std::dec << std::endl;
    }
    catch(gbt::ScaException& e) {
      /* Ignore these here */ 
      if(os) {
        *os << e.what();
      }
    }

    if( write_failed ) { failed = true; break; }
    //break;
  }
  //if( os ) sampa2.dumpRegisters(*os);
  return( !failed );
}
