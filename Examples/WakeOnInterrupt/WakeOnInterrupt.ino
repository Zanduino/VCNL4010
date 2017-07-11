

/////////////////////////////////////////////////////////////////////////////////////////////
//   U N F I N I S H E D ,   D O E S   N O T   W O R K   O N   A r d u i n o   M i c r o   //
/////////////////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************************************************
** Example program for using the VCNL4010 library which allows the Arduino to use the VCNL4010 Fully Integrated   **
** Proximity and Ambient Light Sensor with Infrared Emitter using the built-in interrupt functionality.           **
**                                                                                                                **
** The example will use the proximity sensor and put the Arduino to sleep in low-energy mode until the VCN4010    **
** detects a movement in the sensor. The movement is detected using the low and high threshold value settings,    **
** are set to 10% below and 10% above the startup values to avoid false triggering.                               **
** The interrupt makes the Arduino wake up and display a message, then it goes back to sleep.                     **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
*******************************************************************************************************************/
#include "VCNL4010.h"                                                         // Include the library              //
#include <avr/sleep.h>                                                        // Sleep mode Library               //

/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED = 9600;                                           // Set the baud rate for Serial I/O //
const uint8_t  GREEN_LED_PIN = 13;                                            // Define the default Arduino pin   //
const uint8_t  INTERRUPT_PIN = 9;                                             // VCNL4010 attached to this pin    //

/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
uint16_t Proximity_Value = 0;                                                 // Last displayed Proximity reading //
uint16_t Proximity_Delta = 0;                                                 // Difference between current & last//
VCNL4010 Sensor;                                                              // Instantiate the class            //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  pinMode(GREEN_LED_PIN, OUTPUT);                                             // Define the green LED as an output//
  digitalWrite(GREEN_LED_PIN,HIGH);                                           // Show that we've booted           //
  pinMode(INTERRUPT_PIN,INPUT_PULLUP);                                        // Default state HIGH with pull-up  //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial comms at set Baud   //
  delay(2000);                                                                // Some Atmel Serials need time     //
  Serial.println(F("Starting VCNL4010 WakeOnInterrupt program"));             //                                  //
  while (!Sensor.begin()) {                                                   // Loop until sensor found          //
    Serial.println(F("Error, unable to find or identify VCNL4010."));         // Show error message               //
    delay(5000);                                                              // Wait 5 seconds before retrying   //
  } // of if-then we can't initialize or find the device                      //                                  //
  Sensor.setLEDmA(200);                                                       // Boost power to Proximity sensor  //
  Sensor.setProximityFreq(32);                                                // Sample 32 times per second       //
  uint16_t ProximityReading = Sensor.getProximity();                          // Get the proximity reading        //
  Serial.print("Proximity sensor is ");Serial.print(ProximityReading);        // Display reading                  //
  Serial.print(", low = ");Serial.print(ProximityReading*9/10);               // Display low threshold value      //
  Serial.print(", high = ");Serial.print(ProximityReading*11/10);             // Display high threshold value     //
  Serial.println();                                                           //                                  //
  Sensor.setInterrupt( 1,                                                     // Trigger after 4 events           //
                       false,                                                 // Nothing on Proximity reading     //
                       false,                                                 // Nothing on Ambient light reading //
                       true,                                                  // Interrupt on Proximity threshold //
                       false,                                                 // Nothing on Ambient threshold     //
                       ProximityReading *  9 / 10,                            // Low value is 90% of first reading//
                       ProximityReading * 11 / 10);                           // High value is 110% of first value//
  Sensor.setProximityContinuous(true);                                        // Turn on continuous Proximity     //
  Serial.println(F("VCNL4010 initialized.\n\n"));                             //                                  //

  pinMode(7, INPUT);	   // Pin 7 is input to which a switch is connected
  digitalWrite(7, HIGH);   // Configure internal pull-up resistor

/*
  cli();                                                                      // Disable interrupts for a bit     //
  DDRB = B00000000;        // set pins 8 to 13 as inputs
  PORTB |= B11111111;      // enable pullups on pins 8 to 13

  PCICR =0x02;          // Enable PCINT1 interrupt
  PCMSK0 = 0b00000111;

  DDRB &= ~(1 << DDB0);                                                       // Clear the PB0 pin
  PORTB |= (1 << PORTB0);                                                     // turn On the Pull-up
  PCICR |= (1 << PCIE0);                                                      // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT0);                                                    // set PCINT0 to trigger
  sei();                                                                      // turn on interrupts

  pinMode(A1, INPUT);	   // Pin A1 is input to which a switch is connected
  digitalWrite(A1, HIGH);   // Configure internal pull-up resistor
  pinMode(A2, INPUT);	   // Pin A2 is input to which a switch is connected
  digitalWrite(A2, HIGH);   // Configure internal pull-up resistor5
  */


} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  Serial.print("Sleeping until sensor movement detected.\n");                 // Go to sleep until wake-up        //
  digitalWrite(GREEN_LED_PIN,false);                                          // Turn off LED while we sleep      //
  delay(100);
/*
  cli();                                                                      // disable interrupts               //
  ADCSRA = 0;                                                                 // turn off the ADC module          //
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);                                        // Set the maximum power savings    //
  sleep_enable();                                                             // Enable sleep, but don't sleep yet//
  sei();                                                                      // Re-enable interrupts             //
  sleep_cpu ();                                                               // Sleep within 3 clock cycles      //
  ADCSRA = B10000000;                                                         // turn the ADC module back on      //
*/
  digitalWrite(GREEN_LED_PIN,HIGH);                                           // Turning on LED                   //

uint8_t x = digitalRead(7);
Serial.print("-->");Serial.print(x);Serial.println("<--");
  delay(3000);
  Serial.print(millis());Serial.print(" ");Serial.print(Sensor.getProximity());
  Serial.print(": Sensor Movement detected!\n");
  Sensor.clearInterrupt(0);
} // of method loop()                                                         //----------------------------------//


// ISR(PCINT1_vect) {}

