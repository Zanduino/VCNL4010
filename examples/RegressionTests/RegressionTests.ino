/*!
@file RegressionTests.ino

@section RegressionTests_intro_section Description

Example program to test the VCNL4010 library calls for regression errors

The VCNL4010 library uses the standard I2C calls and assumes that the default hardware I2C ports
for SDA and SCL are being utilized. The address of the VCNL4010 is hardcoded at 0x13 and cannot be
changed. The library is available on GitHub at https://github.com/Zanduino/VCNL4010

@section RegressionTests_license GNU General Public License v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

@section RegressionTests_author Author

Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

@section RegressionTests_versions Changelog

| Version | Date       | Developer  | Comments                                                     |
| ------- | ---------- | ---------- | ------------------------------------------------------------ |
| 1.0.0   | 2020-12-25 | SV-Zanshin | Issue #15 - Initial coding                                   |

*/
#include "VCNL4010.h"  // Include the library
/***************************************************************************************************
** Declare all program constants                                                                  **
***************************************************************************************************/
const uint32_t SERIAL_SPEED{115200};  ///< Set the baud rate for Serial I/O

/***************************************************************************************************
** Declare global variables and instantiate classes                                               **
***************************************************************************************************/
VCNL4010 Sensor;  ///< Instantiate the class

void setup() {
  /*!
    @brief    Arduino method called once at startup to initialize the system
    @details  This is an Arduino IDE method which is called first upon boot or restart. It is only
              called one time and then control goes to the main "loop()" method, from which control
              never returns
    @return   void
  */
  Serial.begin(SERIAL_SPEED);  // Start serial port at Baud rate
#ifdef __AVR_ATmega32U4__      // If we are a 32U4 processor, then wait a bit
  delay(2000);
#endif
  Serial.println("Starting VCNL4010 regression test program");
  while (!Sensor.begin()) {  // Loop until sensor found
    Serial.println("Error, unable to find or identify VCNL4010.\nChecking again in 5 seconds...");
    delay(5000);
  }  // of if-then we can't initialize or find the device
  Serial.println("Setting and checking VCNL4010 attributes.");

  Sensor.setProximityHz(128);
  uint8_t value = Sensor.readByte(REGISTER_PROXIMITY_RATE);
  if (value != 6) {
    Serial.print("Error in Proximity Hertz, should be \"6\", value = \"");
    Serial.print(value);
    Serial.println("\".");
  }  // if-then bad Proximity change
  Sensor.setProximityHz(32);
  value = Sensor.readByte(REGISTER_PROXIMITY_RATE);
  if (value != 4) {
    Serial.print("Error in Proximity Hertz, should be \"4\", value = \"");
    Serial.print(value);
    Serial.println("\".");
  }  // if-then bad Proximity change

  Sensor.setProximityFreq(1);
  value = Sensor.readByte(REGISTER_PROXIMITY_TIMING) & B00011000;
  if (value >> 3 != 1) {
    Serial.print("Error in Proximity Freq, should be \"1\", value = \"");
    Serial.print(value >> 3);
    Serial.println("\".");
  }  // if-then bad Proximity change
  Sensor.setProximityFreq(2);
  value = Sensor.readByte(REGISTER_PROXIMITY_TIMING) & B00011000;
  if (value >> 3 != 2) {
    Serial.print("Error in Proximity Freq, should be \"2\", value = \"");
    Serial.print(value >> 3);
    Serial.println("\".");
  }  // if-then bad Proximity change

  Sensor.setAmbientLight(5, 5);
  value = Sensor.readByte(REGISTER_AMBIENT_PARAM) & B01110111;
  if (value != B1000010) {
    Serial.print("Error in Ambient, should be B1000010, value = \"");
    Serial.print(value, BIN);
    Serial.println("\".");
  }  // if-then bad Proximity change
  Sensor.setAmbientLight(3, 2);
  value = Sensor.readByte(REGISTER_AMBIENT_PARAM) & B01110111;
  if (value != B100001) {
    Serial.print("Error in Ambient Freq, should be B100001, value = \"");
    Serial.print(value, BIN);
    Serial.println("\".");
  }  // if-then bad Proximity change

  Sensor.setLEDmA(200);
  value = Sensor.readByte(REGISTER_LED_CURRENT);
  Sensor.setLEDmA(100);
  if (value == Sensor.readByte(REGISTER_LED_CURRENT) * 10) {
    Serial.print("Error in setting/reading LED current value \"");
    Serial.print(value);
    Serial.println("\".");
  }  // if-then bad LED change

  Serial.println("Finished Testing.");

}  // of method setup()

void loop() {
  /*!
    @brief    Arduino method for the main program loop
    @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
    repeating.
    @return   void
  */
}  // of method loop()
