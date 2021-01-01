#pragma once

#include "Arduino.h"

#define V_ADS115_ADDR 0x49
#define V_EEPROM_ADDR 0x53

#define VOLTMETER_MEASURING_DIR -1

#define VOLTMETER_PRESSURE_COEFFICIENT 0.015918958F

#define VOLTMETER_PAG_6144_CAL_ADDR   208
#define VOLTMETER_PAG_4096_CAL_ADDR   216
#define VOLTMETER_PAG_2048_CAL_ADDR   224
#define VOLTMETER_PAG_1024_CAL_ADDR   232
#define VOLTMETER_PAG_512_CAL_ADDR    240
#define VOLTMETER_PAG_256_CAL_ADDR    248

#define VOLTMETER_FILTER_NUMBER 10

class Voltmeter {
private:
  void i2cBegin();
  bool i2cReadBytes(uint8_t addr, uint8_t reg_addr, uint8_t* buff, uint16_t len);
  bool i2cWriteBytes(uint8_t addr, uint8_t reg_addr, uint8_t* buff, uint16_t len);
  bool i2cReadU16(uint8_t addr, uint8_t reg_addr, uint16_t* value);
  bool i2cWriteU16(uint8_t addr, uint8_t reg_addr, uint16_t value);

  float getResolution(voltmeterGain_t gain);
  uint16_t getCoverTime(voltmeterRate_t rate);
  uint8_t getPGAEEEPROMAddr(voltmeterGain_t gain);
  
  uint8_t _ads1115_addr;
  uint8_t _eeprom_addr;

public:
  voltmeterGain_t _gain;
  voltmeterRate_t _rate;
  voltmeterMode_t _mode;

  float resolution;
  uint16_t cover_time;
  int16_t adc_raw;
  float calibration_factor;


public:
  Voltmeter(uint8_t ads1115_addr=V_ADS115_ADDR, uint8_t eeprom_addr=V_EEPROM_ADDR);

  void setGain(voltmeterGain_t gain);
  void setRate(voltmeterRate_t rate);
  void setMode(voltmeterMode_t mode);

  float getVoltage(bool calibration = true);
  int16_t getConversion(uint16_t timeout = 125);
  int16_t getAdcRaw();

  bool isInConversion();
  void startSingleConversion();

  bool EEPORMWrite(uint8_t address, uint8_t* buff, uint8_t len);
  bool EEPORMRead(uint8_t address, uint8_t* buff, uint8_t len);

  void setCalibration(int8_t voltage, uint16_t actual);
  bool saveCalibration2EEPROM(voltmeterGain_t gain, int16_t hope, int16_t actual);
  bool readCalibrationFromEEPROM(voltmeterGain_t gain, int16_t* hope, int16_t* actual);
};
