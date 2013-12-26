import processing.serial.*;

Serial serialPort;
String serialInputString;

int dataTimestamp = 0;
float roll = 0, minRoll = 0, maxRoll = 0, avgRoll = 0;
float pitch = 0, minPitch = 0, maxPitch = 0, avgPitch = 0;
float yaw = 0, minYaw = 0, maxYaw = 0, avgYaw = 0;
int gyroX, gyroY, gyroZ;
int accelerometerX, accelerometerY, accelerometerZ;
int magnetometerX, magnetometerY, magnetometerZ;
float pressure, altitude, temperature;

float[] rollData, pitchData, yawData;
int[] accelerometerHistoryX, accelerometerHistoryY, accelerometerHistoryZ;
int[] gyroHistoryX, gyroHistoryY, gyroHistoryZ;

PFont f, f2, f3;

PrintWriter sensorDataFile;
PrintWriter gestureDataFile;

JSONArray gestureDataSample;

boolean isRecordingGesture = false;

int backgroundColor[] = { 255, 255, 255 };

int gestureSampleCount = 0;
int gestureSensorSampleCount = 0;

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
  f3 = createFont("Arial", 16, true);
  
  // Set up data file
  sensorDataFile = createWriter("sensorData.txt");
  
  // An array of recent roll values
  rollData = new float[250];
  for (int i = 0; i < rollData.length; i++) { rollData[i] = 0; }
  
  // An array of recent pitch values
  pitchData = new float[250];
  for (int i = 0; i < pitchData.length; i++) { pitchData[i] = 0; }
  
  // An array of recent yaw values
  yawData = new float[250];
  for (int i = 0; i < yawData.length; i++) { yawData[i] = 0; }
  
  // An array of recent accelerometer values
  accelerometerHistoryX = new int[250]; accelerometerHistoryY = new int[250]; accelerometerHistoryZ = new int[250];
  for (int i = 0; i < accelerometerHistoryX.length; i++) { accelerometerHistoryX[i] = 0; }
  for (int i = 0; i < accelerometerHistoryY.length; i++) { accelerometerHistoryY[i] = 0; }
  for (int i = 0; i < accelerometerHistoryZ.length; i++) { accelerometerHistoryZ[i] = 0; }
  
  // An array of recent accelerometer values
  gyroHistoryX = new int[250]; gyroHistoryY = new int[250]; gyroHistoryZ = new int[250];
  for (int i = 0; i < gyroHistoryX.length; i++) { gyroHistoryX[i] = 0; }
  for (int i = 0; i < gyroHistoryY.length; i++) { gyroHistoryY[i] = 0; }
  for (int i = 0; i < gyroHistoryZ.length; i++) { gyroHistoryZ[i] = 0; }
}

void draw () {
  
  // Set background
  background(backgroundColor[0], backgroundColor[1], backgroundColor[2]);
  
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
  
  stroke(0, 0, 0);
  
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
  
  //
  // Render X
  //
  
  fill(0);
  textFont(f);
  text((int) degrees(roll) + "°", (width / 16), 70); // ㎭
  textFont(f2);
  text("Min: " + minRoll + ", Max: " + maxRoll + ", Avg: " + avgRoll + "", width / 16, 90);
  
  // Draw lines connecting all points
  for (int i = 0; i < rollData.length-1; i++) {
    stroke(0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 16), (width / 16) + 80),
      map(rollData[i], 0, 360, 130, 90+75),
      map(i+1, 0, 100, (width / 16), (width / 16) + 80),
      map(rollData[i+1], 0, 360, 130, 90+75)
    );
  }

  // Slide everything down in the array
  for (int i = 0; i < rollData.length-1; i++) {
    rollData[i] = rollData[i+1];
  }
  // Add a new random value
  rollData[rollData.length-1] = degrees(roll);
  
  //
  // Render Y
  //
  
  fill(0);
  textFont(f);
  text((int) degrees(pitch) + "°", width / 2 - 100, 70);
  textFont(f2);
  text("Min: " + minPitch + ", Max: " + maxPitch + ", Avg: " + avgPitch + "", width / 2 - 100, 90);
  
  // Draw lines connecting all points
  for (int i = 0; i < pitchData.length-1; i++) {
    stroke(0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 2) - 100, (width / 2) - 20),
      map(pitchData[i], 0, 360, 130, 90+75),
      map(i+1, 0, 100, (width / 2) - 100, (width / 2) - 20),
      map(pitchData[i+1], 0, 360, 130, 90+75)
    );
  }

  // Slide everything down in the array
  for (int i = 0; i < pitchData.length-1; i++) {
    pitchData[i] = pitchData[i+1];
  }
  // Add a new random value
  pitchData[pitchData.length-1] = degrees(pitch);
  
  //
  // Render Z
  //
  
  fill(0);
  textFont(f);
  text((int) degrees(yaw) + "°", (width / 2) + 325, 70);
  textFont(f2);
  text("Min: " + minYaw + ", Max: " + maxYaw + ", Avg: " + avgYaw + "", (width / 2) + 325, 90);
  
  // Draw lines connecting all points
  for (int i = 0; i < yawData.length-1; i++) {
    stroke(0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 2) + 325, (width / 2) + 325 + 80),
      map(yawData[i], 0, 360, 130, 90+75),
      map(i+1, 0, 100, (width / 2) + 325, (width / 2) + 325 + 80),
      map(yawData[i+1], 0, 360, 130, 90+75)
    );
  }

  // Slide everything down in the array
  for (int i = 0; i < yawData.length-1; i++) {
    yawData[i] = yawData[i+1];
  }
  // Add a new random value
  yawData[yawData.length-1] = degrees(yaw);
  
  text("N", (width / 2) + 325 - 20, 90 + 25);
  text("E", (width / 2) + 325 - 20, 90 + 35);
  text("S", (width / 2) + 325 - 20, 90 + 45);
  text("W", (width / 2) + 325 - 20, 90 + 55);
  
  
  
  
  
  
  //
  // Render Accelerometer Data
  //
  
  fill(0);
  textFont(f3);
  text("Accelerometer", (width / 16), height - 150); // ㎭
  textFont(f2);
  fill(255, 0, 0); text("X: " + accelerometerX, width / 16, height - 130);
  fill(0, 255, 0); text("Y: " + accelerometerY, width / 16, height - 110);
  fill(0, 0, 255); text("Z: " + accelerometerZ, width / 16, height - 90);
  
  // Draw lines connecting all points
  for (int i = 0; i < accelerometerHistoryX.length-1; i++) {
    stroke(255,0,0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 16) + 50, (width / 16) + 80),
      map(accelerometerHistoryX[i], 0, 360, height - 115, height - 115+100),
      map(i+1, 0, 100, (width / 16) + 50, (width / 16) + 80),
      map(accelerometerHistoryX[i+1], 0, 360, height - 115, height - 115+100)
    );
  }
  
  for (int i = 0; i < accelerometerHistoryX.length-1; i++) {
    stroke(0,255,0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 16) + 50, (width / 16) + 80),
      map(accelerometerHistoryY[i], 0, 360, height - 115, height - 115+100),
      map(i+1, 0, 100, (width / 16) + 50, (width / 16) + 80),
      map(accelerometerHistoryY[i+1], 0, 360, height - 115, height - 115+100)
    );
  }
  
  for (int i = 0; i < accelerometerHistoryX.length-1; i++) {
    stroke(0,0,255);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 16) + 50, (width / 16) + 80),
      map(accelerometerHistoryZ[i], 0, 360, height - 115, height - 115+100),
      map(i+1, 0, 100, (width / 16) + 50, (width / 16) + 80),
      map(accelerometerHistoryZ[i+1], 0, 360, height - 115, height - 115+100)
    );
  }

  // Slide everything down in the array
  for (int i = 0; i < accelerometerHistoryX.length-1; i++) {
    accelerometerHistoryX[i] = accelerometerHistoryX[i+1];
    accelerometerHistoryY[i] = accelerometerHistoryY[i+1];
    accelerometerHistoryZ[i] = accelerometerHistoryZ[i+1];
  }
  // Add a new random value
  accelerometerHistoryX[accelerometerHistoryX.length-1] = accelerometerX;
  accelerometerHistoryY[accelerometerHistoryX.length-1] = accelerometerY;
  accelerometerHistoryZ[accelerometerHistoryX.length-1] = accelerometerZ;
  
  
  
  
  
  
  //
  // Render Gyroscope Data
  //
  
  fill(0);
  textFont(f3);
  text("Gyroscope", (width / 2) - 100, height - 150); // ㎭
  textFont(f2);
  fill(255, 0, 0); text("X: " + gyroX, (width / 2) - 100, height - 130);
  fill(0, 255, 0); text("Y: " + gyroY, (width / 2) - 100, height - 110);
  fill(0, 0, 255); text("Z: " + gyroZ, (width / 2) - 100, height - 90);
  
  // Draw lines connecting all points
  for (int i = 0; i < gyroHistoryX.length-1; i++) {
    stroke(255,0,0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 2) - 100 + 50, (width / 2) - 100 + 80),
      map(gyroHistoryX[i], 0, 360, height - 115, height - 115+100),
      map(i+1, 0, 100, (width / 2) - 100 + 50, (width / 2) - 100 + 80),
      map(gyroHistoryX[i+1], 0, 360, height - 115, height - 115+100)
    );
  }
  
  for (int i = 0; i < gyroHistoryY.length-1; i++) {
    stroke(0,255,0);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 2) - 100 + 50, (width / 2) - 100 + 80),
      map(gyroHistoryY[i], 0, 360, height - 115, height - 115+100),
      map(i+1, 0, 100, (width / 2) - 100 + 50, (width / 2) - 100 + 80),
      map(gyroHistoryY[i+1], 0, 360, height - 115, height - 115+100)
    );
  }
  
  for (int i = 0; i < gyroHistoryZ.length-1; i++) {
    stroke(0,0,255);
    strokeWeight(1);
    line(
      map(i, 0, 100, (width / 2) - 100 + 50, (width / 2) - 100 + 80),
      map(gyroHistoryZ[i], 0, 360, height - 115, height - 115+100),
      map(i+1, 0, 100, (width / 2) - 100 + 50, (width / 2) - 100 + 80),
      map(gyroHistoryZ[i+1], 0, 360, height - 115, height - 115+100)
    );
  }

  // Slide everything down in the array
  for (int i = 0; i < accelerometerHistoryX.length-1; i++) {
    gyroHistoryX[i] = gyroHistoryX[i+1];
    gyroHistoryY[i] = gyroHistoryY[i+1];
    gyroHistoryZ[i] = gyroHistoryZ[i+1];
  }
  // Add a new random value
  gyroHistoryX[gyroHistoryX.length-1] = gyroX;
  gyroHistoryY[gyroHistoryY.length-1] = gyroY;
  gyroHistoryZ[gyroHistoryZ.length-1] = gyroZ;
  
  //
  // Print data to file
  //
  sensorDataFile.print(serialInputString);
  
  if (isRecordingGesture) {
    // gestureDataFile.print(serialInputString);
    
    JSONObject gestureDataPoint = new JSONObject();
    
    gestureDataPoint.setString("timestamp", str(dataTimestamp));
    gestureDataPoint.setString("roll", str(roll));
    gestureDataPoint.setString("pitch", str(pitch));
    gestureDataPoint.setString("yaw", str(yaw));
    gestureDataPoint.setString("gyroX", str(gyroX));
    gestureDataPoint.setString("gyroY", str(gyroY));
    gestureDataPoint.setString("gyroZ", str(gyroZ));
    gestureDataPoint.setString("accelerometerX", str(accelerometerX));
    gestureDataPoint.setString("accelerometerY", str(accelerometerY));
    gestureDataPoint.setString("accelerometerZ", str(accelerometerZ));
    gestureDataPoint.setString("magnetometerX", str(magnetometerX));
    gestureDataPoint.setString("magnetometerY", str(magnetometerY));
    gestureDataPoint.setString("magnetometerZ", str(magnetometerZ));
    gestureDataPoint.setString("pressure", str(pressure));
    gestureDataPoint.setString("altitude", str(altitude));
    gestureDataPoint.setString("temperature", str(temperature));

    // gestureDataSample.setJSONObject(gestureSensorSampleCount, gestureData);
    gestureDataSample.append(gestureDataPoint);
    
    gestureSensorSampleCount++;
  }
  
  
  //text("" + roll + ", " + pitch + ", " + yaw, width / 2, 60);
}

void serialEvent (Serial serialPort) {
  
  // Read serial data
  if ( serialPort.available() > 0) {  // If data is available,
    serialInputString = serialPort.readString();         // read it and store it in val
    // print(serialInputString);
    
    if (serialInputString != null && serialInputString.length() > 0) {
    
      // Check if the string begins with a '!' (i.e., check if it's a data string)
      if (serialInputString.charAt(0) == '!') {
        
        serialInputString = serialInputString.substring (1);
        
        String[] serialInputArray = split(serialInputString, '\t');
        
        // Check if array is correct size
        if (serialInputArray.length >= 3) {
          dataTimestamp = millis();
          
          roll = (float(serialInputArray[0]));
          pitch = (float(serialInputArray[1]));
          yaw = (float(serialInputArray[2]));
          gyroX = int(serialInputArray[3]);
          gyroY = int(serialInputArray[4]);
          gyroZ = int(serialInputArray[5]);
          accelerometerX = int(serialInputArray[6]);
          accelerometerY = int(serialInputArray[7]);
          accelerometerZ = int(serialInputArray[8]);
          magnetometerX = int(serialInputArray[9]);
          magnetometerY = int(serialInputArray[10]);
          magnetometerZ = int(serialInputArray[11]);
          pressure = float(serialInputArray[12]);
          altitude = float(serialInputArray[13]);
          temperature = float(serialInputArray[14]);
          
          // Update minimum and maximum values
          if (roll > maxRoll) maxRoll = roll;
          if (roll < minRoll) minRoll = roll;
          if (pitch > maxPitch) maxPitch = pitch;
          if (pitch < minPitch) minPitch = pitch;
          if (yaw > maxYaw) maxYaw = yaw;
          if (yaw < minYaw) minYaw = yaw;
        }
        
      }
    }
  }
}

void keyPressed() {
  if (key == ' ') {
   // TODO: Start gesture
   if (isRecordingGesture == false) {
     backgroundColor[0] = 232; backgroundColor[1] = 94; backgroundColor[2] = 83;
     isRecordingGesture = true;
     gestureSensorSampleCount = 0;
     //gestureDataFile = createWriter("gestureData.txt");
     gestureDataSample = new JSONArray();
   } else {
     backgroundColor[0] = 255; backgroundColor[1] = 255; backgroundColor[2] = 255;
     isRecordingGesture = false;
     // gestureDataFile.flush(); // Writes the remaining data to the file
     // gestureDataFile.close(); // Finishes the file
     
     JSONArray gestureSampleSet;
     gestureSampleSet = loadJSONArray("data/gestureSampleSet.json"); // Load existing file
     // TODO: Add "tried gesture" to gesture sample
     gestureSampleSet.append(gestureDataSample);
     //gestureSampleSet.setJSONArray(gestureSampleCount, gestureDataSample);
     
     saveJSONArray(gestureSampleSet, "data/gestureSampleSet.json");
     
     gestureSampleCount++;
   } 
  } else if (key == ESC) {
    sensorDataFile.flush(); // Writes the remaining data to the file
    sensorDataFile.close(); // Finishes the file
    exit(); // Stops the program
  } else if (key == TAB) {
    fill(0);
    textFont(f);
    textAlign(CENTER);
    text("SHAKE", (width / 2), (height / 2));
  }
}
