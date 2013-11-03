/**
 * Simple Read
 * 
 * Read data from the serial port and change the color of a rectangle
 * when a switch connected to a Wiring or Arduino board is pressed and released.
 * This example works with the Wiring / Arduino program that follows below.
 */


import processing.serial.*;

Serial myPort;  // Create object from Serial class
int val;      // Data received from the serial port

void setup() 
{
  size(640, 360, P3D);
  noStroke();
  fill(204);
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[14];
  myPort = new Serial(this, portName, 115200);
}

String serialInputString;
void draw()
{
  if ( myPort.available() > 0) {  // If data is available,
    serialInputString = myPort.readString();         // read it and store it in val
  }
  println(serialInputString);
  
  if (serialInputString.length() > 0) {
    
    // Check if the string begins with a '!' (i.e., check if it's a data string)
    if (serialInputString.charAt(0) == '!') {
      
      serialInputString = serialInputString.substring(1);
    
      String[] serialInputArray = split(serialInputString, ',');
    
      float rollDegrees = 0F;
      float pitchDegrees = 0F;
      float yawDegrees = 0F;
      // Check if array is correct size
      if (serialInputArray.length == 3) {
        
        try {
          rollDegrees = Float.parseFloat(serialInputArray[0]);
        } catch (Exception e) { }
        try {
          pitchDegrees = Float.parseFloat(serialInputArray[1]);
        } catch (Exception e) { }
        try {
          yawDegrees = Float.parseFloat(serialInputArray[2]);
        } catch (Exception e) { }
      
        background(0);
        lights();
        
        pushMatrix(); // need this
      
        if(mousePressed) {
          float fov = PI/3.0; 
          float cameraZ = (height/2.0) / tan(fov/2.0); 
          perspective(fov, float(width)/float(height), cameraZ/2.0, cameraZ*2.0); 
        } else {
          ortho(0, width, 0, height); 
        }
        translate(width/2, height/2, 0);
      //  rotateX(-PI/6); 
      //  rotateY(PI/3); 
        try {
          rotateX(radians(rollDegrees));
          rotateY(radians(pitchDegrees));
          rotateZ(radians(yawDegrees));
        } catch (Exception e) {
          //
        }
        box(160);
       popMatrix(); 
      
      }
    }
  }
  
//  background(255);             // Set background to white
//  if (val == 0) {              // If the serial value is 0,
//    fill(0);                   // set fill to black
//  } 
//  else {                       // If the serial value is not 0,
//    fill(204);                 // set fill to light gray
//  }
//  rect(50, 50, 100, 100);
}



/*

// Wiring / Arduino Code
// Code for sensing a switch status and writing the value to the serial port.

int switchPin = 4;                       // Switch connected to pin 4

void setup() {
  pinMode(switchPin, INPUT);             // Set pin 0 as an input
  Serial.begin(9600);                    // Start serial communication at 9600 bps
}

void loop() {
  if (digitalRead(switchPin) == HIGH) {  // If switch is ON,
    Serial.print(1, BYTE);               // send 1 to Processing
  } else {                               // If the switch is not ON,
    Serial.print(0, BYTE);               // send 0 to Processing
  }
  delay(100);                            // Wait 100 milliseconds
}

*/
