#include <MovingAvarageFilter.h>

#define RELAY_ENABLE_PIN 12

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
  
  
  
  
//  digitalWrite(RELAY_ENABLE_PIN, LOW);
//  delay(500);
//  digitalWrite(RELAY_ENABLE_PIN, HIGH);
//  delay(500);
}
