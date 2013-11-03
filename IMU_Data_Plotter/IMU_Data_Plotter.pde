// Learning Processing
// Daniel Shiffman
// http://www.learningprocessing.com

// Example: a graph of random numbers

import processing.serial.*;

Serial myPort;  // Create object from Serial class
String serialInputString;

float[] rollData;
float[] pitchData;
float[] yawData;

int windowWidth = 1280;
int windowHeight = 768;

void setup() {
  size(windowWidth, windowHeight);
  if (frame != null) {
    frame.setResizable(true);
  }
  smooth();
  // An array of random values
  rollData = new float[width];
  pitchData = new float[width];
  yawData = new float[width];
  for (int i = 0; i < rollData.length; i++) {
    rollData[i] = 0; //random(100);
    pitchData[i] = 0; //random(100);
    yawData[i] = 0; //random(100);
  }
  
  
  println(Serial.list());
  String portName = Serial.list()[14];
  myPort = new Serial(this, portName, 115200);
}

// Documentation:
// http://wiki.processing.org/w/Window_Size_and_Full_Screen
//boolean sketchFullScreen() {
//  return true;
//}

//public int sketchWidth() {
//  return displayWidth;
//}
//
//public int sketchHeight() {
//  return displayHeight;
//}
//
//public String sketchRenderer() {
//  return P3D;
//}

void draw() {

  background(255);
  
  if ( myPort.available() > 0) {  // If data is available,
    serialInputString = myPort.readString();         // read it and store it in val
  }
  println(serialInputString);
  
  float rollDegrees = 0F;
  float pitchDegrees = 0F;
  float yawDegrees = 0F;
  
  
  
  
  
  
  // Draw lines connecting all points
//  int xOrigin = 0;
//  int yOrigin = 100;
//  int xRange = 200;
//  float xStep = displayWidth / (float) xRange;
//  int x = 0;
//  //for (int i = floor(vals.length / arrayDivideFactor); i < vals.length - 1; i++) {
//  for (int i = (rollData.length - xRange); i < rollData.length - 1; i++) {
//    stroke(0);
//    strokeWeight(1);
//    //line(i*arrayDivideFactor, vals[i], (i+1)*arrayDivideFactor, vals[i+1]);
//    //line(i, vals[i], (i+1), vals[i+1]);
//    line(xOrigin + (x * xStep), yOrigin + rollData[i], xOrigin + ((x + 1) * xStep), yOrigin + rollData[i+1]);
//    
//    x++;
//  }
  
  // Plot sensor data 
  // i.e., void drawPlot(float[] data, int xOrigin, int yOrigin, int xRange, int yRange)
  drawPlot(rollData, 0, 100, 200, 180);
  drawPlot(pitchData, 0, 400, 200, 180);
  drawPlot(yawData, 0, 700, 200, 180);
  
//  // Slide everything down in the array
//  for (int i = 0; i < vals.length-1; i++) {
//    vals[i] = vals[i+1]; 
//  }
  
  
//  int arrayDivideFactor = 4;
//  for (int i = 0; i < (vals.length-1)/arrayDivideFactor; i++) {
//    stroke(0);
//    strokeWeight(1);
//    line(i*arrayDivideFactor, 150+vals[i], (i+1)*arrayDivideFactor, 150+vals[i+1]);
//  }
//  
////  // Slide everything down in the array
////  for (int i = 0; i < vals.length-1; i++) {
////    vals[i] = vals[i+1]; 
////  }
//  
//  
//  
//   for (int i = 0; i < (vals.length-1)/arrayDivideFactor; i++) {
//    stroke(0);
//    strokeWeight(1);
//    line(i*arrayDivideFactor, 300+vals[i], (i+1)*arrayDivideFactor, 300+vals[i+1]);
//  }
  
  
  // TODO: Plot bad points as RED, good points as BLACK. Track point good/bad-ness.
  
  if (!isPaused) {
    // Slide everything down in the array
    for (int i = 0; i < rollData.length-1; i++) {
      rollData[i] = rollData[i+1];
    }
    
    // Slide everything down in the array
    for (int i = 0; i < pitchData.length-1; i++) {
      pitchData[i] = pitchData[i+1];
    }
    
    // Slide everything down in the array
    for (int i = 0; i < yawData.length-1; i++) {
      yawData[i] = yawData[i+1];
    }
  
    // Add a new random value
    //vals[vals.length-1] = random(100);
    
    // Read serial data
    if (serialInputString != null && serialInputString.length() > 0) {
      
      // Check if the string begins with a '!' (i.e., check if it's a data string)
      if (serialInputString.charAt(0) == '!') {
        
        serialInputString = serialInputString.substring(1);
      
        String[] serialInputArray = split(serialInputString, '\t');
      
        // Check if array is correct size
        if (serialInputArray.length >= 3) {
          
          // Parse roll (rotation about X axis) data point
          try {
            rollDegrees = Float.parseFloat(serialInputArray[0]);
            rollData[rollData.length - 1] = rollDegrees;
          } catch (Exception e) {
            // TODO: Handle exception
          }
          
          // Parse pitch (rotation about Y axis) data point
          try {
            pitchDegrees = Float.parseFloat(serialInputArray[1]);
            pitchData[pitchData.length - 1] = pitchDegrees;
          } catch (Exception e) {
            // TODO: Handle exception
          }
          
          // Parse yaw (rotation about Z axis) data point
          try {
            yawDegrees = Float.parseFloat(serialInputArray[2]);
            yawData[yawData.length - 1] = yawDegrees;
          } catch (Exception e) {
            // TODO: Handle exception
          }
          
        }
      }
    }
  }

}

void drawPlot(float[] data, int xOrigin, int yOrigin, int xRange, int yRange) {
  
  // Draw lines connecting all points
//  int xOrigin = 0;
//  int yOrigin = 100;
//  int xRange = 200;
  float xStep = displayWidth / (float) xRange;
  int x = 0;
  //for (int i = floor(vals.length / arrayDivideFactor); i < vals.length - 1; i++) {
  for (int i = (data.length - xRange); i < data.length - 1; i++) {
    stroke(0);
    strokeWeight(1);
    //line(i*arrayDivideFactor, vals[i], (i+1)*arrayDivideFactor, vals[i+1]);
    //line(i, vals[i], (i+1), vals[i+1]);
    line(xOrigin + (x * xStep), yOrigin + data[i], xOrigin + ((x + 1) * xStep), yOrigin + data[i+1]);
    
    x++;
  }
  
//  // Slide everything down in the array
//  for (int i = 0; i < vals.length-1; i++) {
//    vals[i] = vals[i+1]; 
//  }
}

boolean isPaused = false;

void keyPressed() {
  int keyIndex = -1;
  if (key == ' ') {
    if (!isPaused) {
      isPaused = true;
    } else {
      isPaused = false;
    }
//    keyIndex = key - 'A';
  } else if (key == 'q' || key == 'Q') {
    exit();
  }
  
//  else if (key >= 'a' && key <= 'z') {
//    keyIndex = key - 'a';
//  }
//  if (keyIndex == -1) {
//    // If it's not a letter key, clear the screen
//    background(0);
//  } else { 
//    // It's a letter key, fill a rectangle
//    fill(millis() % 255);
//    float x = map(keyIndex, 0, 25, 0, width - rectWidth);
//    rect(x, 0, rectWidth, height);
//  }
}
