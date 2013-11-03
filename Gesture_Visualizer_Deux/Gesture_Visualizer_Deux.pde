import processing.serial.*;

Serial serialPort;
String serialInputString;

int roll = 0;
int pitch = 0;
int yaw = 0;

void setup () {
  size(640, 360, P3D);
  
  // Connect to the corresponding serial port
  serialPort = new Serial(this, Serial.list()[14], 115200);
  
  // Defer callback until new line
  serialPort.bufferUntil('\n');
}

void draw () {
  
  // Set background
  background(0.5);
  
  pushMatrix(); 
  
  translate(width/2, height/2, -30); 
  
  // Rotate
  rotateX(((float)pitch)*PI/180.0); 
  rotateY(((float)yaw)*PI/180.0); 
  rotateZ(((float)roll)*PI/180.0); 
  
  // Print data
  print("Pitch: ");
  print(pitch);
  print(", Roll: ");
  println(roll);
  
  
  scale(90);
  beginShape(QUADS);
  
  fill(0, 255, 0); vertex(-1,  1,  1);
  fill(0, 255, 0); vertex( 1,  1,  1);
  fill(0, 255, 0); vertex( 1, -1,  1);
  fill(0, 255, 0); vertex(-1, -1,  1);
  
  fill(0, 255, 255); vertex( 1,  1,  1);
  fill(0, 255, 255); vertex( 1,  1, -1);
  fill(0, 255, 255); vertex( 1, -1, -1);
  fill(0, 255, 255); vertex( 1, -1,  1);
  
  
  fill(255, 0, 255); vertex( 1,  1, -1);
  fill(255, 0, 255); vertex(-1,  1, -1);
  fill(255, 0, 255); vertex(-1, -1, -1);
  fill(255, 0, 255); vertex( 1, -1, -1);
  
  fill(255, 255, 0); vertex(-1,  1, -1);
  fill(255, 255, 0); vertex(-1,  1,  1);
  fill(255, 255, 0); vertex(-1, -1,  1);
  fill(255, 255, 0); vertex(-1, -1, -1);
  
  
  fill(255, 0, 0); vertex(-1,  1, -1);
  fill(255, 0, 0); vertex( 1,  1, -1);
  fill(255, 0, 0); vertex( 1,  1,  1);
  fill(255, 0, 0); vertex(-1,  1,  1);
  
  
  fill(0, 0, 255); vertex(-1, -1, -1);
  fill(0, 0, 255); vertex( 1, -1, -1);
  fill(0, 0, 255); vertex( 1, -1,  1);
  fill(0, 0, 255); vertex(-1, -1,  1);
  
  
  endShape();
  
  popMatrix(); 
}

void serialEvent (Serial serialPort) {
  // Read serial data
  if ( serialPort.available() > 0) {  // If data is available,
    serialInputString = fd.readString();         // read it and store it in val
  }
  // println(serialInputString);
  
  if (serialInputString != null && serialInputString.length() > 0) {
    
    // Check if the string begins with a '!' (i.e., check if it's a data string)
    if (serialInputString.charAt(0) == '!') {
      
      serialInputString = serialInputString.substring (1);
      
      String[] serialInputArray = split(serialInputString, '\t');
      
      // Check if array is correct size
      if (serialInputArray.length >= 3) {
        roll = ((int)float(serialInputArray[0]));
        pitch = ((int)float(serialInputArray[1]));
        yaw = ((int)float(serialInputArray[2]));
      }
      
    }
  }
}
