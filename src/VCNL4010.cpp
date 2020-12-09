/*! @file VCNL4010.cpp
 @section VCNL4010cpp_intro_section Description

Arduino Library for the Vishay VCNL4010\n\n
See main library header file for details
*/
#include "VCNL4010.h"  // Include the header definition
VCNL4010::VCNL4010() {
  /*!
   * @brief   Class constructor
   * @details Class Constructor for VCNL4010 instantiates the class. Unused
   */
}
VCNL4010::~VCNL4010() { /*!
                         * @brief   Class destructor
                         * @details Class Destructor for VCNL4010 destroys the class. Unused
                         */
}
bool VCNL4010::begin(const uint8_t deviceAddress, const uint32_t i2CSpeed) {
  /*!
    @brief     Starts the I2C communications with the VCNL4010 (overloaded)
    @details   Uses the default address specified in VCNL4010_ADDRESS if one is not given
    @param[in] deviceAddress I2C device address
    @param[in] i2CSpeed Speed of the I2C bus in Herz
    @return    "true" when device has been detected, otherwise false
  */
  _I2Caddress = deviceAddress;  // Set the private device address variable
  Wire.begin();                 // Start I2C as master device
  if (readByte(VCNL4010_PRODUCT_REG) != VCNL4010_PRODUCT_VERSION) return false;
  Wire.setClock(i2CSpeed);                                 // Set the I2C speed
  setProximityHz(2);                                       // Default 2Hz proximity rate
  setLEDmA(20);                                            // Default 20mA IR LED power
  setAmbientLight(2, 32);                                  // Default 2/sec and 32 averaged
  setProximityFreq();                                      // Default to 390kHz frequency
  setInterrupt();                                          // Default to no interrupts
  uint8_t commandBuffer = readByte(VCNL4010_COMMAND_REG);  // get the command register values
  commandBuffer |= B00011000;                              // Single ALS and Proximity reading
  writeByte(VCNL4010_COMMAND_REG, commandBuffer);          // Start the measurement cycle
  return true;
}  // of method begin()
bool VCNL4010::begin(void) {
  /*!
    @brief     Starts the I2C communications with the VCNL4010 (overloaded)
    @details   Uses the default address specified in VCNL4010_ADDRESS if one is not given and
    standard I2C Speed
  */
  return begin(VCNL4010_ADDRESS, I2C_STANDARD_MODE);
}  // of overloaded method begin()
bool VCNL4010::begin(const uint8_t deviceAddress) {
  /*!
    @brief     Starts the I2C communications with the VCNL4010 (overloaded)
    @details   Uses the address specified defaults to the standard I2C Speed
    @param[in] deviceAddress I2C device address
    @return    "true" when device has been detected, otherwise false
  */
  return begin(deviceAddress, I2C_STANDARD_MODE);
}  // of overloaded method begin()
bool VCNL4010::begin(const uint32_t i2CSpeed) {
  /*!
    @brief     Starts the I2C communications with the VCNL4010 (overloaded)
    @details   Uses the default VCNL4010_ADDRESS address specified
    @param[in] i2CSpeed Speed of the I2C bus in Herz
    @return    "true" when device has been detected, otherwise false
  */
  return begin(VCNL4010_ADDRESS, i2CSpeed);
}  // of overloaded method begin()
uint8_t VCNL4010::readByte(const uint8_t addr) const{
  /*!
    @brief     reads 1 byte from the specified address
    @details   Delays VCNL4010_I2C_DELAY_MICROSECONDS after reading
    @param[in] addr Address of the I2C device
    @return    single unsigned integer value with the byte value read from I2C
  */
  Wire.beginTransmission(_I2Caddress);                 // Address the I2C device
  Wire.write(addr);                                    // Send the register address to read
  delayMicroseconds(VCNL4010_I2C_DELAY_MICROSECONDS);  // Introduce slight delay
  Wire.requestFrom(_I2Caddress, (uint8_t)1);           // Request 1 byte of data
  return Wire.read();                                  // read it and return it
}  // of method readByte()
uint16_t VCNL4010::readWord(const uint8_t addr) const {
  /*!
    @brief     reads 2 bytes from the specified address
    @details   Delays VCNL4010_I2C_DELAY_MICROSECONDS after reading
    @param[in] addr Address of the I2C device
    @return    Unsigned integer 16 value with the 2 bytes read from I2C
  */
  uint16_t returnData;                                 // Store return value
  Wire.beginTransmission(_I2Caddress);                 // Address the I2C device
  Wire.write(addr);                                    // Send the register address to read
  delayMicroseconds(VCNL4010_I2C_DELAY_MICROSECONDS);  // Introduce slight delay
  Wire.requestFrom(_I2Caddress, (uint8_t)2);           // Request 2 consecutive bytes
  returnData = Wire.read();                            // Read the msb
  returnData = returnData << 8;                        // shift the data over
  returnData |= Wire.read();                           // Read the lsb
  return returnData;
}  // of method readWord()
void VCNL4010::writeByte(const uint8_t addr, const uint8_t data) const {
  /*!
    @brief     Write 1 byte to the specified address
    @param[in] addr Address of the I2C device
    @param[in] data Single byte to write
  */
  Wire.beginTransmission(_I2Caddress);           // Address the I2C device
  Wire.write(addr);                              // Send the register address to read
  Wire.write(data);                              // Send the register address to read
}  // of method writeByte()
void VCNL4010::setProximityHz(const uint8_t Hz) const {
  /*!
    @brief     set the frequency with which the proximity sensor pulses are sent/read
    @details   The only values that the VCNL4010 can be set to are:\n
    Bits  | Measurements/S
    ----- | --------------
    000   | 1.95 (DEFAULT)
    001   | 3.90625
    010   | 7.8125
    011   | 16.625
    100   | 31.25
    101   | 62.5
    110   | 125
    111   | 250
    These roughly equate to Hz (2,4,8,16,32,64,128 and 256)
    @param[in] Hz Herz code value, described in details
  */
  uint8_t setValue;  // temp variable for sampling rate
  if (Hz > 250)
    setValue = 7;
  else if (Hz >= 128)
    setValue = 6;
  else if (Hz >= 64)
    setValue = 5;
  else if (Hz >= 32)
    setValue = 4;
  else if (Hz >= 16)
    setValue = 3;
  else if (Hz >= 8)
    setValue = 2;
  else if (Hz >= 4)
    setValue = 1;
  else
    setValue = 0;
  writeByte(VCNL4010_PROXIMITY_RATE_REG, setValue);  // Write result to register
}  // of method setProximityHz()
void VCNL4010::setLEDmA(const uint8_t mA) const {
  /*!
    @brief     sets the IR LED current output in milliamps
    @details   Range is between 0mA and 200mA, internally set in steps of 10mA with input values
    being truncated down to the next lower value. No range checking is performed
    @param[in] mA Milliamps
  */
  writeByte(VCNL4010_LED_CURRENT_REG, (uint8_t)(mA / 10));  // Divide by 10 and write register
}  // of method setLEDmA()
void VCNL4010::setProximityFreq(const uint8_t value) const {
  /*!
    @brief     sets the proximity modulator timing to one of 4 possible values
    @details   The following 4 values can be set:\n
    00 = 390.625  kHz (DEFAULT)\n
    01 = 781.25   kHz\n
    10 =   1.5625 MHz\n
    11 =   3.125  MHz
    @param[in] value 0-4 see details for encoded values
  */
  uint8_t registerSetting = readByte(VCNL4010_PROXIMITY_TIMING_REG);  // Get the register settings
  registerSetting &= B11100111;                                       // Mask the 2 timing bits
  registerSetting |= (value & B00000011) << 3;                        // Add in 2 bits from value
  writeByte(VCNL4010_PROXIMITY_TIMING_REG, registerSetting);  // Write new buffer back to register
}  // of method setProximityFreq()
void VCNL4010::setAmbientLight(const uint8_t sample, const uint8_t avg) const {
  /*!
    @brief     Sets the number of samples taken per second and the number of samples averaged to
    make a reading
    @details   each reading takes only 300microseconds and the default period for a measurement is
    100ms.
    @param[in] sample Samples to take per second
    @param[in] avg Averages to take per reading (0-128, rounded internally to nearest correct
    value)
  */
  uint8_t workAvg;                  // work variable
  uint8_t workSample = sample - 1;  // subtract one for offset
  if (workSample == 6)
    workSample = 5;
  else if (workSample == 8)
    workSample = 6;
  else if (workSample > 7)
    workSample = 7;

  if (avg >= 128)
    workAvg = B111;
  else if (avg >= 64)
    workAvg = B110;
  else if (avg >= 32)
    workAvg = B101;
  else if (avg >= 16)
    workAvg = B100;
  else if (avg >= 8)
    workAvg = B011;
  else if (avg >= 4)
    workAvg = B010;
  else if (avg >= 2)
    workAvg = B001;
  else
    workAvg = B000;

  uint8_t registerValue = readByte(VCNL4010_AMBIENT_PARAMETER_REG);  // retrieve current settings
  registerValue &= B10001000;                                        // Mask current settings
  registerValue |= workSample << 4;                                  // Set bits 4,5,6
  registerValue |= workAvg;                                          // Set bits 0,1,2
  writeByte(VCNL4010_AMBIENT_PARAMETER_REG, registerValue);          // Write new values to buffer
}  // of method setAmbientLight()
uint16_t VCNL4010::getAmbientLight() const {
  /*!
    @brief     retrieves the 16 bit ambient light value.
    @details   Since we always send a request for another reading after retrieving the previous
    results we just need to wait for a result to come back.
    @return unsigned integer 16 measurement value
  */
  uint8_t commandBuffer = readByte(VCNL4010_COMMAND_REG);       // get the register contents
  while ((commandBuffer & B01000000) == 0)                      // Loop until we have a result
    commandBuffer = readByte(VCNL4010_COMMAND_REG);             // get the register contents again
  uint16_t returnValue = readWord(VCNL4010_AMBIENT_LIGHT_REG);  // retrieve the reading
  if (!_ContinuousAmbient)                                      // Only trigger if not continuous
  {
    commandBuffer |= B00010000;                      // Trigger ambient measurement
    writeByte(VCNL4010_COMMAND_REG, commandBuffer);  // Set trigger for next reading
  }                                                  // of if-then Continuous mode turned on
  return returnValue;
}  // of method getAmbientLight()
uint16_t VCNL4010::getProximity() const {
  /*!
    @brief     retrieves the 16 bit proximity value.
    @details   Since we always send a request for another reading after retrieving the previous
    results we just need to wait for a result to come back.
    @return unsigned integer 16 measurement value
  */
  uint8_t commandBuffer = readByte(VCNL4010_COMMAND_REG);   // get the register contents
  while ((commandBuffer & B00100000) == 0)                  // Loop until we have a result
    commandBuffer = readByte(VCNL4010_COMMAND_REG);         // get the register contents again
  uint16_t returnValue = readWord(VCNL4010_PROXIMITY_REG);  // retrieve the reading
  if (!_ContinuousProximity)                                // Only trigger if not continuous
  {
    commandBuffer |= B00001000;                      // Trigger proximity measurement
    writeByte(VCNL4010_COMMAND_REG, commandBuffer);  // Set trigger for next reading
  }                                                  // of if-then Continuous mode turned on
  return returnValue;
}  // of method getProximity()
uint8_t VCNL4010::getInterrupt() const {
  /*!
    @brief     retrieves the 4 bits denoting which, if any, interrupts have been triggered
    @details   Bit 3 - proximity interrupt\n
               Bit 2 - ambient light interrupt\n
               Bit 1 - low threshold interrupt\n
               Bit 0 - high threshold interrupt\n
    @return unsigned integer 8, only 4 LSB bits are set
  */
  return readByte(VCNL4010_INTERRUPT_STATUS_REG) &
         B0001111;  // get the register contents and mask unused bits
}  // of method getInterrupt()
void VCNL4010::clearInterrupt(const uint8_t intVal) const {
  /*!
    @brief     clearInterrupt() overwrites the 4 bits denoting which, if any, interrupts have been
               triggered
    @details   Bit 3 - proximity interrupt\n
               Bit 2 - ambient light interrupt\n
               Bit 1 - low threshold interrupt\n
               Bit 0 - high threshold interrupt\n
               The bit needs to be written as "1" in order to clear it, so send the bitwise "not"
    value
   @param[in] intVal Use the 4 lsb bit to set the interrupt values
  */
  writeByte(VCNL4010_INTERRUPT_STATUS_REG,
            (readByte(VCNL4010_INTERRUPT_STATUS_REG) & 0xF0) | (~intVal & 0xF));
}  // of method clearInterrupt()
void VCNL4010::setInterrupt(const uint8_t count, const bool ProxReady, const bool ALSReady,
                            const bool ProxThreshold, const bool ALSThreshold,
                            const uint16_t lowThreshold, const uint16_t highThreshold) const {
  /*!
    @brief     sets the interrupts used
    @details   The count is the number of consecutive readings needed in order to trigger an
    interrupt for threshold low or high being exceeded. Then a boolean to indicate an interrupt on
    Proximity reading ready, then the same for an ALS reading to be ready. The next two values
    indicate whether to trigger an interrupt on threshold low or high value being exceeded for
    Proximity and ALS. Since only one can be used the ALS is chosen when both are set. Finally the
    low and high threshold values themselves
    @param[in] count
    @param[in] ProxReady
    @param[in] ALSReady
    @param[in] ProxThreshold
    @param[in] ALSThreshold
    @param[in] lowThreshold
    @param[in] highThreshold
  */
  uint8_t registerValue = 0;  // Default to 1 count
  if (count >= 128)
    registerValue = B111;
  else if (count >= 64)
    registerValue = B110;
  else if (count >= 32)
    registerValue = B101;
  else if (count >= 16)
    registerValue = B100;
  else if (count >= 8)
    registerValue = B011;
  else if (count >= 4)
    registerValue = B010;
  else if (count >= 2)
    registerValue = B001;
  registerValue = registerValue << 5;         // Move value to position
  if (ProxReady) registerValue |= B00001000;  // Set Proximity Ready flag
  if (ALSReady) registerValue |= B00000100;   // Set ALS Ready flag
  if (ProxThreshold || ALSThreshold)          // If we are setting a threshold
  {
    registerValue |= B00000010;            // Set the flag for threshold
    if (ALSThreshold) registerValue += 1;  // Set the flag for ALS
    writeByte(VCNL4010_LOW_THRESHOLD_MSB_REG, (uint8_t)(lowThreshold >> 8));    // Write the MSB
    writeByte(VCNL4010_LOW_THRESHOLD_LSB_REG, (uint8_t)lowThreshold);           // Write the LSB
    writeByte(VCNL4010_HIGH_THRESHOLD_MSB_REG, (uint8_t)(highThreshold >> 8));  // Write the MSB
    writeByte(VCNL4010_HIGH_THRESHOLD_LSB_REG, (uint8_t)highThreshold);         // Write the LSB
  }  // of if-then we have threshold interrupts to set
  writeByte(VCNL4010_INTERRUPT_REG, registerValue);
}  // of method setLEDmA()
void VCNL4010::setAmbientContinuous(const bool ContinuousMode) {
  /*!
    @brief     sets or unsets the continuous measurement mode for the ambient light sensor
    @param[in] ContinuousMode "true" for continuous mode, "false" for triggered measurements
  */
  uint8_t commandBuffer = readByte(VCNL4010_COMMAND_REG);  // get the register contents
  commandBuffer &= B11111010;                              // Mask the 2 relevant bits
  if (ContinuousMode == true)                              // If we are turning on
  {
    commandBuffer |= B00000101;  // then write the 2 relevant bits and
    _ContinuousAmbient = true;   // set the flag
  } else {                       // otherwise if proximity is still
    if (_ContinuousProximity)
      commandBuffer |= B00000001;  // turned on the keep flag or
    else
      commandBuffer |= B00000000;                  // turn off both bits
    _ContinuousAmbient = false;                    // set the flag
  }                                                // of if-then-else we are turning on
  writeByte(VCNL4010_COMMAND_REG, commandBuffer);  // Write value back to register
}  // of method setAmbientContinuous()
void VCNL4010::setProximityContinuous(const bool ContinuousMode) {
  /*!
    @brief     sets or unsets the continuous measurement mode for the proximity sensor
    @param[in] ContinuousMode "true" for continuous mode, "false" for triggered measurements
  */
  uint8_t commandBuffer = readByte(VCNL4010_COMMAND_REG);  // get the register contents
  commandBuffer &= B11111100;                              // Mask the 2 relevant bits
  if (ContinuousMode == true)                              // If we are turning on
  {
    commandBuffer |= B00000011;   // then write the 2 relevant bits and
    _ContinuousProximity = true;  // set flag
  } else {                        // otherwise if proximity is still
    if (_ContinuousAmbient)
      commandBuffer |= B00000001;  // turned on the keep flag or
    else
      commandBuffer |= B00000000;                  // turn off both bits
    _ContinuousProximity = false;                  // set flag
  }                                                // of if-then-else we are turning on
  writeByte(VCNL4010_COMMAND_REG, commandBuffer);  // Write value back to register
}  // of method setProximityContinuous() 
