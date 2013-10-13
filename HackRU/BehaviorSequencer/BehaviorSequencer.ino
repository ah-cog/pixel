/*
ADXL3xx Accelerometer Notes:
 
Reads an Analog Devices ADXL3xx accelerometer and communicates the
acceleration to the computer.  The pins used are designed to be easily
compatible with the breakout boards from Sparkfun, available from:

 http://www.sparkfun.com/commerce/categories.php?c=80
 http://www.arduino.cc/en/Tutorial/ADXL3xx
 http://learn.adafruit.com/adafruit-analog-accelerometer-breakouts/programming

The circuit:
 analog 0: accelerometer self test
 analog 1: z-axis
 analog 2: y-axis
 analog 3: x-axis
 analog 4: ground
 analog 5: vcc

Thanks to Michael Smith-Welch, the folks in The Tinkering Studio at the 
Exploratorium in California. Thanks to the IDC community for their inspiration.

Thanks to attendees and sponsors of HackMIT 2013 and HackRU 2013 for 
valuable inspirational conversations. Likewise, thanks to fellow 
members of Terrapin Hackers and Startup Shell.

This example is based on code in the public domain by 
David A. Mellis and Tom Igoe, Adafruit, SparkFun.
*/

#include <MovingAvarageFilter.h>

#define MAKEY_INPUT_PIN A0
#define ACCELEROMETER_X_PIN A3
#define ACCELEROMETER_Y_PIN A2
#define ACCELEROMETER_Z_PIN A1
#define RELAY_ENABLE_PIN 12

MovingAvarageFilter movingAvarageFilter(20);

boolean check = false;

void setup() {
  pinMode(RELAY_ENABLE_PIN, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  
  //
  // Check for node input
  //
  
  // Declare input and output variables
  float input =  analogRead(MAKEY_INPUT_PIN); // without a real input, looking at the step respons (input at unity, 1)
  float averageInputValue = 0;

  averageInputValue = movingAvarageFilter.process(input);

  // Call the fir routine with the input. The value 'fir' spits out is stored in the output variable.
  
  if (averageInputValue < 400) { // Change this parameter to fine tune the sensitivity
    if (!check){         
      // Keyboard.print("d");
      digitalWrite(RELAY_ENABLE_PIN, HIGH);
      Serial.println(averageInputValue);           
      check = !check;   
    }         
  }
  
  if (averageInputValue > 600) {     
    if (check) {
      check = !check;
      digitalWrite(RELAY_ENABLE_PIN, LOW);  
    }     
  }
  
  
  //
  // Print accelerometer data over serial
  //
  
  // Print the accelerometer sensor values:
  Serial.print(analogRead(ACCELEROMETER_X_PIN));
  Serial.print("\t");
  Serial.print(analogRead(ACCELEROMETER_Y_PIN));
  Serial.print("\t");
  Serial.print(analogRead(ACCELEROMETER_Z_PIN));
  Serial.println();
}
