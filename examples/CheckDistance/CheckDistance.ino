/*******************************************************************************************************************
** Example program for using the VCNL4010 library which allows the Arduino to use the VCNL4010 Fully Integrated   **
** Proximity and Ambient Light Sensor with Infrared Emitter. The most recent version of this program can be found **
** at https://github.com/SV-Zanshin/VCNL4010/blob/master/Examples/CheckDistance/CheckDistance.ino                 **
**                                                                                                                **
** The VCNL4010 library uses the standard I2C library and assumes that the default hardware I2C ports for SDA and **
** SCL are being utilized. The address of the VCNL4010 is hardcoded at 0x13 and cannot be changed. The library is **
** available on GitHub at https://github.com/SV-Zanshin/VCNL4010/archive/master.zip                               **
**                                                                                                                **
** This program sets up the VCNL4010 with the IR emitter powered at 200mA versus the default of 20mA to increase  **
** the measurement range. The proximity sampling rate is also increased from the default of 2 to 125 in order     **
** to get more accurate readings.                                                                                 **
**                                                                                                                **
** If the movement sensor values change by more than 15% compared to the previous reading then the new value is   **
** printed out to the serial port, otherwise nothing is displayed and the program loops to the next measurement   **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.0.0  2017-09-01 https://github.com/SV-Zanshin Created program from close of DisplayMeasurements.ino          **
**                                                                                                                **
*******************************************************************************************************************/
#include "VCNL4010.h"                                                         // Include the library              //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 115200;                                         // Set the baud rate for Serial I/O //
const float    PERCENTAGE   =   0.15;                                         // Percentage delta trigger         //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
VCNL4010 Sensor;                                                              // Instantiate the class            //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef __AVR_ATmega32U4__                                                   // If we are a 32U4 processor, then //
    while(!Serial){}                                                          // loop until serial port is ready  //
  #endif                                                                      // and then continue                //
  Serial.println("\n\n\nStarting VCNL4010 CheckDistance program");            //                                  //
  while (!Sensor.begin()) {                                                   // Loop until sensor found          //
    Serial.println("Error, unable to find or identify VCNL4010.");            // Show error message               //
    delay(5000);                                                              // Wait 5 seconds before retrying   //
  } // of if-then we can't initialize or find the device                      //                                  //
  Serial.println("- Setting VCNL4010 attributes:");                           //                                  //
  Sensor.setLEDmA(200);                                                       // Boost power to Proximity sensor  //
  Serial.println("  - Proximity sensor power set to 200mA");                  //                                  //
  Sensor.setProximityHz(128);                                                 // Sample 128x per second           //
  Serial.println("  - Proximity sampling set to 128x per second");            //                                  //
  Serial.println("- VCNL4010 initialized.\n\n");                              //                                  //
} // of method setup()                                                        //----------------------------------//
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint16_t proximityLast    = 0;                                       // Last displayed Proximity reading //
         uint16_t proximityCurrent = Sensor.getProximity();                   // Get the Proximity sensor value   //
         uint16_t proximityDelta   = proximityCurrent * PERCENTAGE;           // Set delta to percentage of value //
         int16_t  proximityChange  = proximityLast - proximityCurrent;        // Compute the delta Proximity      //
                                                                              //                                  //
  if (abs(proximityChange) > proximityDelta) {                                // If the change is above the preset//
    Serial.print("Time = ");                                                  // threshold, display the values    //
    Serial.print(millis()/1000);                                              //                                  //
    Serial.print(", Proximity = ");                                           //                                  //
    Serial.println(proximityCurrent);                                         //                                  //
    proximityLast = proximityCurrent;                                         //                                  //
  } // of if-then we have a % change more than PERCENTAGE                     //                                  //
} // of method loop()                                                         //----------------------------------//
