#include <MovingAvarageFilter.h>

// Add the Moving Average Library, use the link below to download the zip file of library

MovingAvarageFilter movingAvarageFilter(20);

boolean check = false;

void setup() {
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
      Keyboard.print("d");         
      Serial.println(output);           
      check = !check;   
    }         
  }

  if (output >600) {     
    if (check){               
      check = !check;   
    }     
  }
}
