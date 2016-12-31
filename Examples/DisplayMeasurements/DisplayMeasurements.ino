/*******************************************************************************************************************
** Example program for using the VCNL4010 library which allows the Arduino to use the VCNL4010 Fully Integrated   **
** Proximity and Ambient Light Sensor with Infrared Emitter.                                                      **
**                                                                                                                **
** The VCNL4010 library uses the standard I2C library and assumes that the default hardware I2C ports for SDA and **
** SCL are being utilized.                                                                                        **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
*******************************************************************************************************************/
#include "VCNL4010.h"                                                         // Include the library              //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 9600;                                           // Set the baud rate for Serial I/O //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
uint16_t ALS_Value       = 0;                                                 // Last displayed ALS reading       //
uint16_t Proximity_Value = 0;                                                 // Last displayed Proximity reading //
uint16_t ALS_Delta       = 0;                                                 // Difference between current & last//
uint16_t Proximity_Delta = 0;                                                 // Difference between current & last//
VCNL4010 Sensor;                                                              // Instantiate the class            //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial comms at set Baud   //
  delay(2000);                                                                // Some Atmel Serials need time     //
  Serial.println(F("Starting VCNL4010 display measurements program"));        //                                  //
  while (!Sensor.begin()) {                                                   // Loop until sensor found          //
    Serial.println(F("Error, unable to find or identify VCNL4010."));         // Show error message               //
    delay(5000);                                                              // Wait 5 seconds before retrying   //
  } // of if-then we can't initialize or find the device                      //                                  //
  Sensor.setLEDmA(200);                                                       // Boost power to Proximity sensor  //
  Sensor.setAmbientLight(2,128);                                              // Sample 2x per second, 128 avgs.  //
  Sensor.setProximityRate(64);                                                // Sample 64x per second            //
  Serial.println(F("VCNL4010 initialized.\n\n"));                             //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  uint16_t ALS                    = Sensor.getAmbientLight();                 // Get the ambient light value      //
  uint16_t Proximity              = Sensor.getProximity();                    // Get the Proximity sensor value   //
  if (ALS_Delta==0 && Proximity_Delta==0) {                                   // Set delta values in first loop   //
    ALS_Delta       = ALS/10;                                                 // Set to 10% of value              //
    Proximity_Delta = Proximity/10;                                           // Set to 10% of value              //
  } // of if-then first loop                                                  //                                  //
  int16_t ALS_Change       = ALS_Value - ALS;                                 // Compute the delta ALS            //
  int16_t Proximity_Change = Proximity_Value - Proximity;                     // Compute the delta Proximity      //
  if (abs(ALS_Change)>ALS_Delta || abs(Proximity_Change)>Proximity_Delta) {   // If either is above threshold,    //
    Serial.print(F("Time = "));                                               // then display the new values      //
    Serial.print(millis()/1000);                                              // marking the ALS or proximity that//
    Serial.print(F(", Ambient Light = "));                                    // triggered display with a "*"     //
    if (abs(ALS_Change)>ALS_Delta) Serial.print(F("*"));                      //                                  //
    Serial.print(ALS);                                                        //                                  //
    Serial.print(F(", Proximity = "));                                        //                                  //
    if (abs(Proximity_Change)>Proximity_Delta) Serial.print(F("*"));          //                                  //
    Serial.print(Proximity);                                                  //                                  //
    Serial.println();                                                         //                                  //
    ALS_Value       = ALS;                                                    //                                  //
    Proximity_Value = Proximity;                                              //                                  //
  } // of if-then we have a 10% change or more in either reading              //                                  //
} // of method loop()                                                         //----------------------------------//


//  Sensor.setInterrupt(1,false,false,false,true,30,15000);
//  Serial.print(Sensor.getInterrupt(),BIN);
//  Sensor.clearInterrupt(0);
