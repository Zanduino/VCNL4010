/*******************************************************************************************************************
** Example program for using the VCNL4010 library which allows the Arduino to use the VCNL4010 Fully Integrated   **
** Proximity and Ambient Light Sensor with Infrared Emitter using the built-in interrupt functionality.           **
**                                                                                                                **
** The example will use the proximity sensor and put the Arduino to sleep in low-energy mode until the VCN4010    **
** detects a movement in the sensor. The movement is detected using the low and high threshold value settings,    **
** are set to 10% below and 10% above the startup values to avoid false triggering.                               **
**                                                                                                                **
** The interrupt makes the Arduino wake up and display a message, then it goes back to sleep.                     **
**                                                                                                                **
** The WAKE_UP_PIN is connected to the INT pinout on the VCNL4010 and is pulled low when the interrupt is raised  **
** on the VCNL4010. This pin needs to be attached to one of digital pins to interrupt, which are just a few pins  **
** depending upon which processor is used. Check https://www.arduino.cc/en/Reference/attachInterrupt to see the   **
** pins which may be used.                                                                                        **
**                                                                                                                **
** This program was originally developed on an Arduino Micro, which poses difficulties because of the way that    **
** the USB serial port is implemented. This needs to be turned off before sleeping and back on after sleeping and **
** the specific code for this is put into a conditional compile block.                                            **
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
** 1.0.3  2018-09-29 https://github.com/SV-Zanshin Cleaned up comments                                            **
** 1.0.2  2017-09-02 https://github.com/SV-Zanshin Removed F() flash macro - uneeded here and confusing to some   **
** 1.0.1  2017-08-30 https://github.com/SV-Zanshin Added version information                                      **
**                                                                                                                **
*******************************************************************************************************************/
#include "VCNL4010.h"                                                         // Include the library              //
#include <avr/sleep.h>                                                        // Sleep mode Library               //

/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED   = 115200;                                       // Set the baud rate for Serial I/O //
const uint8_t  GREEN_LED_PIN  =     13;                                       // Define the default Arduino pin   //
const uint8_t  WAKE_UP_PIN    =      7;                                       // Pin used to wake up processor,   //
const float    PERCENT_CHANGE =   0.10;                                       // Trigger percentage change        //

/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
uint16_t Proximity_Value      =      0;                                       // Last displayed Proximity reading //
uint16_t Proximity_Delta      =      0;                                       // Difference between current & last//
VCNL4010 Sensor;                                                              // Instantiate the class            //

/*******************************************************************************************************************
** Declare interrupt vector to jump to when the VCNL4010 triggers a pin change interrupt. Nothing is done here    **
** apart from disabling the interrupt to prevent a race condition where the program jumps here constantly         **
*******************************************************************************************************************/
void sleepVector(void)                                                        // Interrupt handler on pin change  //
{                                                                             //                                 //
  detachInterrupt(digitalPinToInterrupt(WAKE_UP_PIN));                        // Detach interrupt immediately     //
  digitalWrite(GREEN_LED_PIN,true);                                           // Turn on the green LED            //
} // of method sleepVector()                                                  //                                  //

/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup()                                                                  // Arduino standard setup method    //
{                                                                             //                                  //
  pinMode(GREEN_LED_PIN, OUTPUT);                                             // Define the green LED as an output//
  digitalWrite(GREEN_LED_PIN,HIGH);                                           // Show that we've booted           //
  pinMode(WAKE_UP_PIN, INPUT);	                                             // Pin is attached to VCNL4010 INT  //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial comms at set Baud   //
  #ifdef __AVR_ATmega32U4__                                                   // If we are a 32U4 processor, then //
    while(!Serial);                                                           // loop until serial port is ready  //
    delay(1000);                                                              // and wait a little bit more       //
  #endif                                                                      // and then continue                //
  Serial.println("Starting VCNL4010 WakeOnInterrupt program");                //                                  //
  while (!Sensor.begin())                                                     // Loop until sensor found          //
  {                                                                           //                                  //
    Serial.println("Error, unable to find or identify VCNL4010.");            // Show error message               //
    delay(5000);                                                              // Wait 5 seconds before retrying   //
  } // of if-then we can't initialize or find the device                      //                                  //
  Sensor.setLEDmA(200);                                                       // Boost power to Proximity sensor  //
  Sensor.setProximityFreq(32);                                                // Sample 32 times per second       //
  uint16_t ProximityVal  = Sensor.getProximity();                             // Get the proximity reading        //
  uint16_t lowThreshold  = ProximityVal-(ProximityVal * PERCENT_CHANGE);      // low threshold value              //
  uint16_t highThreshold = ProximityVal+(ProximityVal * PERCENT_CHANGE);      // high threshold value             //
  Serial.print("Proximity sensor is ");Serial.print(ProximityVal);            // Display reading                  //
  Serial.print(", low = ");Serial.print(lowThreshold);                        // Display low threshold value      //
  Serial.print(", high = ");Serial.print(highThreshold);                      // Display high threshold value     //
  Serial.println();                                                           //                                  //
  Sensor.setInterrupt( 1,                                                     // Trigger after 1 event            //
                       false,                                                 // Nothing on Proximity reading     //
                       false,                                                 // Nothing on Ambient light reading //
                       true,                                                  // Interrupt on Proximity threshold //
                       false,                                                 // Nothing on Ambient threshold     //
                       lowThreshold,                                          // Low value is 90% of first reading//
                       highThreshold);                                        // High value is 110% of first value//
  Sensor.setProximityContinuous(true);                                        // Turn on continuous Proximity     //
  Serial.println("VCNL4010 initialized.\n\n");                                //                                  //
} // of method setup()                                                        //----------------------------------//

/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop()                                                                   //                                  //
{                                                                             //                                  //
  Serial.println("Going to sleep until sensor movement detected.");           // Go to sleep until wake-up        //
  delay(2000);                                                                // Give USB time to display message //
  #if defined(__AVR_ATmega32U4__)                                             //----------------------------------//
    USBCON |= _BV(FRZCLK);                                                    // Stop the USB clock               //
    PLLCSR &= ~_BV(PLLE);                                                     // Turn off the USB PLL             //
    USBCON &= ~_BV(USBE);                                                     // Disable the USB                  //
  #endif                                                                      //----------------------------------//
  Sensor.clearInterrupt(0);                                                   // Reset status on VCNL4010         //
  attachInterrupt(digitalPinToInterrupt(WAKE_UP_PIN),sleepVector,LOW);        // jump to vector when pin goes LOW //
  digitalWrite(GREEN_LED_PIN,false);                                          // Turn off LED while we sleep      //
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);                                        // Maximum power savings            //
  sleep_enable();                                                             // Enable sleep mode                //
  sleep_mode();                                                               // Go to Sleep here until woken     //
  // S L E E P   H E R E   U N T I L   W O K E N   B Y   I N T E R R U P T   T R I G G E R E D   B Y  VCNL1040    //
  sleep_disable();                                                            // Turn off sleep mode after waking //
  #if defined(__AVR_ATmega32U4__)                                             //----------------------------------//
    USBDevice.attach();                                                       // Re-attach the USB port           //
    Serial.begin(SERIAL_SPEED);                                               // Start serial comms at set Baud   //
    while (!Serial);                                                          // wait for Serial to initialize    //
    delay(3000);                                                              // Delay 3 seconds for Serial       //
  #endif                                                                      //----------------------------------//
  Serial.println("Woke up, sensor movement detected");                        // Tell the world that we're awake  //
} // of method loop()                                                         //----------------------------------//