import processing.serial.*;

Serial serialPort;
String serialInputString;

float roll = 0, minRoll = 0, maxRoll = 0, avgRoll = 0;
float pitch = 0, minPitch = 0, maxPitch = 0, avgPitch = 0;
float yaw = 0, minYaw = 0, maxYaw = 0, avgYaw = 0;

PFont f, f2;

void setup () {
  size(1200, 800, P3D);
  
  // Print serial ports
  println(Serial.list());
  
  // Connect to the corresponding serial port
  serialPort = new Serial(this, Serial.list()[10], 9600);
  
  // Defer callback until new line
  serialPort.bufferUntil('\n');
  
  // Set up font
  f = createFont("", 64, true);
  f2 = createFont("Arial", 12, true);
}

void draw () {
  
  // Set background
  background(0.5);
  
  pushMatrix(); 
  
  translate(width/2, height/2, -30); 
  
  // Rotate
  rotateX(pitch); // rotateX(((float)pitch)*-PI/180.0); 
  rotateY(yaw); // rotateY(((float)yaw)*-PI/180.0); 
  rotateZ(roll); // rotateZ(((float)roll)*-PI/180.0);  
  
  // Draw X, Y, and Z axes
  strokeWeight(1);
  stroke(255, 0, 0); line(-400, 0, 0, 400, 0, 0); // X axis
  stroke(255, 0, 0); line(0, -400, 0, 0, 400, 0); // Y axis 
  stroke(255, 0, 0); line(0, 0, -400, 0, 0, 400); // Z axis
  
  // Draw X, Y, and Z acceleration vectors
  strokeWeight(4);
  stroke(255, 0, 0); line(-200, 0, 0, 200, 0, 0); // X axis
  stroke(255, 0, 0); line(0, -200, 0, 0, 200, 0); // Y axis 
  stroke(255, 0, 0); line(0, 0, -200, 0, 0, 200); // Z axis
  
  scale(90);
  
  beginShape(QUADS);
  
  stroke(255, 255, 255);
  
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
  
  // Render text

  textAlign(LEFT);
  
  // Render X
  fill(255);
  textFont(f);
  text((int) degrees(roll) + "°", (width / 16), 70); // ㎭
  textFont(f2);
  text("Min: " + minRoll + ", Max: " + maxRoll + ", Avg: " + avgRoll + "", width / 16, 90);
  
  // Render Y
  fill(255);
  textFont(f);
  text((int) degrees(pitch) + "°", width / 2 - 100, 70);
  textFont(f2);
  text("Min: " + minPitch + ", Max: " + maxPitch + ", Avg: " + avgPitch + "", width / 2 - 100, 90);
  
  // Render Z
  fill(255);
  textFont(f);
  text((int) degrees(yaw) + "°", (width / 2) + 325, 70);
  textFont(f2);
  text("Min: " + minYaw + ", Max: " + maxYaw + ", Avg: " + avgYaw + "", (width / 2) + 325, 90);
  
  
  text("" + roll + ", " + pitch + ", " + yaw, width / 2, 60);
}

void serialEvent (Serial serialPort) {
  
  // Read serial data
  if ( serialPort.available() > 0) {  // If data is available,
    serialInputString = serialPort.readString();         // read it and store it in val
    print(serialInputString);
    
    if (serialInputString != null && serialInputString.length() > 0) {
    
    // Check if the string begins with a '!' (i.e., check if it's a data string)
    if (serialInputString.charAt(0) == '!') {
      
      serialInputString = serialInputString.substring (1);
      
      String[] serialInputArray = split(serialInputString, '\t');
      
      // Check if array is correct size
      if (serialInputArray.length >= 3) {
        roll = (float(serialInputArray[0]));
        pitch = (float(serialInputArray[1]));
        yaw = (float(serialInputArray[2]));
        
        // Update minimum and maximum values
        if (roll > maxRoll) maxRoll = roll;
        if (roll < minRoll) minRoll = roll;
        if (pitch > maxPitch) maxPitch = pitch;
        if (pitch < minPitch) minPitch = pitch;
        if (yaw > maxYaw) maxYaw = yaw;
        if (yaw < minYaw) minYaw = yaw;
        
        // Print data
//        print(serialInputArray[0]);
//        print("\t");
//        print(serialInputArray[1]);
//        print("\t");
//        println(serialInputArray[2]);
      }
      
    }
  }
  }
}

