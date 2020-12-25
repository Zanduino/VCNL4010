/*!
@file DisplayMeasurements.ino

@section DisplayMeasurements_intro_section Description

Example program for using the VCNL4010 library which allows the Arduino to use the VCNL4010 Fully
Integrated Proximity and Ambient Light Sensor with Infrared Emitter.

The VCNL4010 library uses the standard I2C calls and assumes that the default hardware I2C ports
for SDA and SCL are being utilized. The address of the VCNL4010 is hardcoded at 0x13 and cannot be
changed. The library is available on GitHub at https://github.com/Zanduino/VCNL4010

This program sets up the VCNL4010 to sample the ambient light at 2x per second (which is the
default) and to average 128 measurements into each reading (which is more than the default 32
averages) to get more constant measurements. The IR emitter is to the maximum value of 200mA (up
from the default of 20mA) to extend the range at which it detects movement and the sampling rate for
proximity is raised to 125 from the default 2

If either the ambient light or the movement sensor values change by more than 15% then the new
values are displayed to the serial port. Changes are detected using the abs() function, which turns
out to be a macro in the Arduino development environment and doesn't support embedded functions,
which is why the checks are done in separate commands rather than in one function call.

@section DisplayMeasurements_license GNU General Public License v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

 @section DisplayMeasurements_author Author

 Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

 @section DisplayMeasurements_versions Changelog

 Version| Date       | Developer  | Comments
 ------ | ---------- | ---------- | --------
1.0.3   | 2017-09-02 | SV-Zanshin | Added serial handling of Arduino micro initialisation
1.0.2   | 2017-09-01 | SV-Zanshin | Removed F() flash macro - uneeded here and confusing to some
1.0.1   | 2017-07-11 | SV-Zanshin | Refactored variables to standard c++ nomenclature
1.0.0   | 2017-01-02 | SV-Zanshin | Added PERCENTAGE constant and program description

*/
#include "VCNL4010.h"  // Include the library
/***************************************************************************************************
** Declare all program constants                                                                  **
***************************************************************************************************/
const uint32_t SERIAL_SPEED{115200};  ///< Set the baud rate for Serial I/O
const float    PERCENTAGE{0.15f};     ///< Percentage delta trigger

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
#ifdef __AVR_ATmega32U4__      // If we are a 32U4 processor, then
  delay(2000);  // wait 2 seconds for initialization
#endif          // and then continue
  Serial.println("Starting VCNL4010 display measurements program");
  while (!Sensor.begin()) {  // Loop until sensor found
    Serial.println("Error, unable to find or identify VCNL4010.\nChecking again in 5 seconds...");
    delay(5000);
  }  // of if-then we can't initialize or find the device
  Serial.println("Setting VCNL4010 attributes.");
  Sensor.setLEDmA(200);             // Boost power to Proximity sensor
  Sensor.setAmbientLight(2, 128);   // Sample 2x per second, 128 avg.
  Sensor.setProximityHz(128);       // Sample 128x per second
  Sensor.setAmbientContinuous();    // Use continuous readings
  Sensor.setProximityContinuous();  // Use continuous readings
  Serial.println("VCNL4010 initialized.\n\n");
}  // of method setup()

void loop() {
  /*!
    @brief    Arduino method for the main program loop
    @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
    repeating.
    @return   void
  */
  static uint16_t ambientSensorLast{0};  ///< Last displayed ALS reading
  static uint16_t proximityLast{0};      ///< Last displayed Proximity reading
  uint16_t        ambientSensor         = Sensor.getAmbientLight();  // Get the ambient light value
  uint16_t        proximitySensor       = Sensor.getProximity();  // Get the Proximity sensor value
  uint16_t        ambientSensorDelta    = ambientSensor * PERCENTAGE;    // Set delta to percentage
  uint16_t        proximitySensorDelta  = proximitySensor * PERCENTAGE;  // Set delta to percentage
  uint16_t        ambientSensorChange   = abs((int16_t)ambientSensorLast - (int16_t)ambientSensor);
  uint16_t        proximitySensorChange = abs((int16_t)proximityLast - (int16_t)proximitySensor);

  if (ambientSensorChange > ambientSensorDelta ||      // If either ambient or proximity
      proximitySensorChange > proximitySensorDelta) {  // is above the preset threshold,
    Serial.print("T = ");                              // then display the new values
    Serial.print(millis() / 1000);                     // marking the ALS or proximity that
    Serial.print(", Ambient = ");                      // triggered the display with a "*"
    if (ambientSensorChange > ambientSensorDelta) Serial.print("*");
    Serial.print(ambientSensor);
    Serial.print(", Proximity = ");
    if (proximitySensorChange > proximitySensorDelta) Serial.print("*");
    Serial.println(proximitySensor);
    ambientSensorLast = ambientSensor;
    proximityLast     = proximitySensor;
  }  // of if-then we have a 20% change or more in either reading
}  // of method loop()
