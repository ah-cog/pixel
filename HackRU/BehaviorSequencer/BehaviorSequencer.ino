#include <MovingAvarageFilter.h>

#define ACCELEROMETER_X_PIN A3
#define ACCELEROMETER_Y_PIN A2
#define ACCELEROMETER_Z_PIN A1
#define RELAY_ENABLE_PIN 12

// these constants describe the pins. They won't change:
//const int groundpin = 18;             // analog input pin 4 -- ground
//const int powerpin = 19;              // analog input pin 5 -- voltage
//const int xpin = A3;                  // x-axis of the accelerometer
//const int ypin = A2;                  // y-axis
//const int zpin = A1;                  // z-axis (only on 3-axis models)

MovingAvarageFilter movingAvarageFilter(20);

boolean check = false;

void setup() {
  pinMode(RELAY_ENABLE_PIN, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  
  // declare input and output variables
  float input =  analogRead(0); // without a real input, looking at the step respons (input at unity, 1)
  float output = 0;

  output = movingAvarageFilter.process(input);

  // here we call the fir routine with the input. The value 'fir' spits out is stored in the output variable.
  
  if (output < 400 ) {   // you can change this parameter to fine tune the sensitivity
    if (!check){         
//      Keyboard.print("d");         
      digitalWrite(RELAY_ENABLE_PIN, HIGH);
      Serial.println(output);           
      check = !check;   
    }         
  }

  if (output >600) {     
    if (check){               
      check = !check;
      digitalWrite(RELAY_ENABLE_PIN, LOW);  
    }     
  }
  
  
  //
  // Print accelerometer data over serial
  //
  
  // print the sensor values:
  Serial.print(analogRead(ACCELEROMETER_X_PIN));
  // print a tab between values:
  Serial.print("\t");
  Serial.print(analogRead(ACCELEROMETER_Y_PIN));
  // print a tab between values:
  Serial.print("\t");
  Serial.print(analogRead(ACCELEROMETER_Z_PIN));
  Serial.println();
}
