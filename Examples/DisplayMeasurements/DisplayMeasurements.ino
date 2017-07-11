/*******************************************************************************************************************
** Example program for using the VCNL4010 library which allows the Arduino to use the VCNL4010 Fully Integrated   **
** Proximity and Ambient Light Sensor with Infrared Emitter. The most recent version of this program can be found **
** at https://github.com/SV-Zanshin/VCNL4010/blob/master/Examples/DisplayMeasurements/DisplayMeasurements.ino     **
**                                                                                                                **
** The VCNL4010 library uses the standard I2C library and assumes that the default hardware I2C ports for SDA and **
** SCL are being utilized. The address of the VCNL4010 is hardcoded at 0x13 and cannot be changed. The library is **
** available on GitHub at https://github.com/SV-Zanshin/VCNL4010                                                  **
**                                                                                                                **
** This program sets up the VCNL4010 to sample the ambient light at 2x per second (which is the default) and to   **
** average 128 measurements into each reading (which is more than the default 32 averages) to get more constant   **
** measurements. The IR emitter is to the maximum value of 200mA (up from the default of 20mA) to extend the      **
** range at which it detects movement and the sampling rate for proximity is raised to 125 from the default 2     **
**                                                                                                                **
** If either the ambient light or the movement sensor values change by more than 15% then the new values are      **
** displayed to the serial port. Changes are detected using the abs() function, which turns out to be a macro in  **
** the Arduino development environment and doesn't support embedded functions, which is why the checks are done   **
** in separate commands rather than in one function call.                                                         **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer           Comments                                                                 **
** ====== ========== =================== ======================================================================== **
** 1.0.1  2017-07-11 Arnd@SV-Zanshin.Com Refactored variables to standard c++ nomenclature                        **
** 1.0.0  2017-01-02 Arnd@SV-Zanshin.Com Added PERCENTAGE constant and program description                        **
**                                                                                                                **
*******************************************************************************************************************/
#include "VCNL4010.h"                                                         // Include the library              //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 9600;                                           // Set the baud rate for Serial I/O //
const uint8_t  PERCENTAGE   = 15;                                             // Percentage delta trigger         //
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
  delay(2000);                                                                // Some Atmel Serials need time     //
  Serial.println(F("Starting VCNL4010 display measurements program"));        //                                  //
  while (!Sensor.begin()) {                                                   // Loop until sensor found          //
    Serial.println(F("Error, unable to find or identify VCNL4010."));         // Show error message               //
    delay(5000);                                                              // Wait 5 seconds before retrying   //
  } // of if-then we can't initialize or find the device                      //                                  //
  Sensor.setLEDmA(200);                                                       // Boost power to Proximity sensor  //
  Sensor.setAmbientLight(2,128);                                              // Sample 2x per second, 128 avgs.  //
  Sensor.setProximityHz(128);                                                 // Sample 128x per second           //
  Serial.println(F("VCNL4010 initialized.\n\n"));                             //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static uint16_t ambientSensorLast = 0;                                      // Last displayed ALS reading       //
  static uint16_t proximityLast     = 0;                                      // Last displayed Proximity reading //
                                                                              //----------------------------------//
  uint16_t ambientSensor         = Sensor.getAmbientLight();                  // Get the ambient light value      //
  uint16_t proximitySensor       = Sensor.getProximity();                     // Get the Proximity sensor value   //
  uint16_t ambientSensorDelta    = Sensor.getAmbientLight()/PERCENTAGE;       // Set delta to percentage of value //
  uint16_t proximitySensorDelta  = Sensor.getProximity()/PERCENTAGE;          // Set delta to percentage of value //
  int16_t  ambientSensorChange   = ambientSensorLast - ambientSensor;         // Compute the delta ALS            //
  int16_t  proximitySensorChange = proximityLast - proximitySensor;           // Compute the delta Proximity      //
  if (abs(ambientSensorChange)>ambientSensorDelta ||                          // If either ambient or proximity   //
      abs(proximitySensorChange)>proximitySensorDelta) {                      // is above the preset threshold,   //
    Serial.print(F("Time = "));                                               // then display the new values      //
    Serial.print(millis()/1000);                                              // marking the ALS or proximity that//
    Serial.print(F(", Ambient Light = "));                                    // triggered the display with a "*" //
    if (abs(ambientSensorChange)>ambientSensorDelta) Serial.print(F("*"));    //                                  //
    Serial.print(ambientSensor);                                              //                                  //
    Serial.print(F(", Proximity = "));                                        //                                  //
    if (abs(proximitySensorChange)>proximitySensorDelta) Serial.print(F("*"));//                                  //
    Serial.println(proximitySensor);                                          //                                  //
    ambientSensorLast = ambientSensor;                                        //                                  //
    proximityLast     = proximitySensor;                                      //                                  //
  } // of if-then we have a 10% change or more in either reading              //                                  //
} // of method loop()                                                         //----------------------------------//
