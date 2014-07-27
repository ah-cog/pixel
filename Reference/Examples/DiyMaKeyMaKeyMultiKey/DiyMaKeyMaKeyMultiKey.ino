//************************************************************************************
//URL to download the Moving Average Library - :

// https://github.com/sebnil/Moving-Avarage-Filter--Arduino-Library-

//************************************************************************************
// Code for multiple keys
// ************************************************************************************

#include <MovingAvarageFilter.h>

MovingAvarageFilter movingAvarageFilter(20);
MovingAvarageFilter movingAvarageFilter1(20);
MovingAvarageFilter movingAvarageFilter2(20);
MovingAvarageFilter movingAvarageFilter3(20);

boolean check = false;
boolean check1 = false;
boolean check2 = false;
boolean check3 = false;

void setup() {
  Serial.begin(115200);     
}


void loop() {        
  // declare input and output variables

  float input =  analogRead(0);
  float input1 =  analogRead(1); 
  float input2 =  analogRead(2);
  float input3 =  analogRead(3);



  float output = 0;
  float output1 = 0;
  float output2 = 0;
  float output3 = 0;

  output = movingAvarageFilter.process(input);
  output1 = movingAvarageFilter1.process(input1);
  output2 = movingAvarageFilter2.process(input2);
  output3 = movingAvarageFilter3.process(input3);


  ////  key "d"///////////////////////////////////////////////////

  if (output < 200 ) {   // you can change this parameter to fine tune the sensitivity
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

  ////  key "F"///////////////////////////////////////////////////

  if (output1 < 200 ) {   // you can change this parameter to fine tune the sensitivity
    if (!check1){         
      Keyboard.print("f");         
      Serial.println(output1);           
      check1 = !check1;   
    }         
  }


  if (output1 >600) {     
    if (check1){               
      check1 = !check1;   
    }     
  }

  ////  key "G"///////////////////////////////////////////////////

  if (output2 < 200 ) {   // you can change this parameter to fine tune the sensitivity
    if (!check2){         
      Keyboard.print("g");         
      Serial.println(output2);           
      check2 = !check2;   
    }         
  }


  if (output2 >600) {     
    if (check2){               
      check2 = !check2;   
    }     
  }

  ////  key "H"///////////////////////////////////////////////////

  if (output3 < 200 ) {   // you can change this parameter to fine tune the sensitivity
    if (!check3){         
      Keyboard.print("h");         
      Serial.println(output3);           
      check3 = !check3;   
    }         
  }


  if (output3 >600) {     
    if (check3){               
      check3 = !check3;   
    }     
  }
}

