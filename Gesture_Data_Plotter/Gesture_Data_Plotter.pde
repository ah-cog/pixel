
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

JSONArray gestureSampleSet;
JSONArray gestureDataSample;

boolean showGesturePrompt = false;
boolean isRecordingGesture = false;
int gestureSelectionTime = 0;

int backgroundColor[] = { 255, 255, 255 };

int gestureIndex = 0;
String gestureName[] = { 
  "at rest, on table",
  "at rest, in hand",
  "pick up",
  "place down",
  "tilt left",
  "tilt right",
  "shake",
  "tap to another, as left",
  "tap to another, as right"
};
final int CONTINUOUS = 0;
final int DISCRETE = 1;
int gestureTemporalBounds[] = {
  CONTINUOUS,
  CONTINUOUS,
  DISCRETE,
  DISCRETE,
  DISCRETE,
  DISCRETE,
  CONTINUOUS,
  DISCRETE,
  DISCRETE
};
int gestureSampleCount = 0;
int gestureSensorSampleCount = 0;

ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples;

void setup () {
  size(1200, 800, P3D);
  
  // Set up font
  f = createFont("Arial", 64, true);
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
  
  gestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  
  print("Opening gesture data... ");
  openGestureData();
  println("Done.");
}

void draw () {
  
  // Set background
  background(backgroundColor[0], backgroundColor[1], backgroundColor[2]);

  // Draw data
  drawGesturePrompt();
  drawGesturePlot();
}

void openGestureData() {
  
  // Load existing gesture data file
  gestureSampleSet = loadJSONArray("data/gestureSampleSet.json");
  
  updateCurrentGesture();
}

void keyPressed() {
//  if (key == ' ') {
//    
//   // TODO: Start gesture
//   if (isRecordingGesture == false) {
//     backgroundColor[0] = 232; backgroundColor[1] = 94; backgroundColor[2] = 83;
//     isRecordingGesture = true;
//     gestureSensorSampleCount = 0;
//     //gestureDataFile = createWriter("gestureData.txt");
//     gestureDataSample = new JSONArray();
//   } else {
//     backgroundColor[0] = 255; backgroundColor[1] = 255; backgroundColor[2] = 255;
//     isRecordingGesture = false;
//     // gestureDataFile.flush(); // Writes the remaining data to the file
//     // gestureDataFile.close(); // Finishes the file
//     
//     JSONObject gestureSample = new JSONObject();
//     gestureSample.setString("gesture", gestureName[gestureIndex]);
//     gestureSample.setJSONArray("sample", gestureDataSample);
//     
//     JSONArray gestureSampleSet;
//     gestureSampleSet = loadJSONArray("data/gestureSampleSet.json"); // Load existing file
//     // TODO: Add "tried gesture" to gesture sample
//     //gestureSampleSet.append(gestureDataSample);
//     gestureSampleSet.append(gestureSample);
//     //gestureSampleSet.setJSONArray(gestureSampleCount, gestureDataSample);
//     
//     saveJSONArray(gestureSampleSet, "data/gestureSampleSet.json");
//     
//     gestureSampleCount++;
//   } 
//   
//  } else
  if (key == ESC) {
    
//    sensorDataFile.flush(); // Writes the remaining data to the file
//    sensorDataFile.close(); // Finishes the file
    exit(); // Stops the program
    
  } else if (key == TAB) {
    
    gestureIndex = (gestureIndex + 1) % gestureName.length;
    
    showGesturePrompt = true;
    gestureSelectionTime = millis();
    
    updateCurrentGesture();
  }
}

/**
 * Store gesture data sample, consisting of a sequence of data points 
 * collected over time, during a specific duration, specified manually 
 * by a human.
 */
void storeGestureData() {
  
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

void drawGesturePrompt() {
  if (showGesturePrompt && (isRecordingGesture || (millis() - gestureSelectionTime < 2000))) {
    fill(0); textFont(f); textAlign(CENTER);
    text(gestureName[gestureIndex], (width / 2), (height / 4));
  }
}

void updateCurrentGesture() {
  
  gestureSamples.clear();
  
  // Populate gesture data arrays
  for (int i = 0; i < gestureSampleSet.size(); i++) {
    JSONObject gestureSample = gestureSampleSet.getJSONObject(i);
    JSONArray gestureSamplePoints = gestureSample.getJSONArray("sample");
    
    ArrayList<ArrayList<Integer>> gestureSamples2 = new ArrayList<ArrayList<Integer>>();
    
    // Populate data arrays
    if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {
      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      
      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
      }
      
      gestureSamples2.add(gestureSamplePointX);
      gestureSamples2.add(gestureSamplePointY);
      gestureSamples2.add(gestureSamplePointZ);
    }
    
//    strokeWeight(1);
//    stroke(255,0,0); drawPlot(gestureSamplePointX, 0, height / 2, width, height, 0, 1000);
//    stroke(0,255,0); drawPlot(gestureSamplePointY, 0, height / 2, width, height, 0, 1000);
//    stroke(0,0,255); drawPlot(gestureSamplePointZ, 0, height / 2, width, height, 0, 1000);

    gestureSamples.add(gestureSamples2);
  }
}

void drawGesturePlot() {
  fill(0);
  textFont(f3);
  text("Accelerometer", (width / 16), height - 150); // ㎭
  textFont(f2);
//  fill(255, 0, 0); text("X: " + accelerometerX, width / 16, height - 130);
//  fill(0, 255, 0); text("Y: " + accelerometerY, width / 16, height - 110);
//  fill(0, 0, 255); text("Z: " + accelerometerZ, width / 16, height - 90);
  
  // Draw lines connecting all points
  strokeWeight(1);
  for (int i = 0; i < gestureSamples.size(); i++) {
    ArrayList<ArrayList<Integer>> gestureSample = gestureSamples.get(i);
    if (gestureSample.size() > 0) {
    //println(gestureSample.size());
      stroke(255,0,0); drawPlot(gestureSample.get(0), 0, height / 2, width, height, 0, 1000);
      stroke(0,255,0); drawPlot(gestureSample.get(1), 0, height / 2, width, height, 0, 1000);
      stroke(0,0,255); drawPlot(gestureSample.get(2), 0, height / 2, width, height, 0, 1000);
    }
  //  stroke(0,255,0); drawPlot(accelerometerHistoryY, 0, 0, width, height, 0, 360);
  //  stroke(0,0,255); drawPlot(accelerometerHistoryZ, 0, 0, width, height, 0, 360);
  }
}

void drawPlot(ArrayList<Integer> data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {
  // Draw lines connecting all points
  //for (int i = 0; i < data.size(); i++) {
  for (int i = 0; i < data.size() - 1; i++) {
    // stroke(255,0,0);
    // strokeWeight(1);
    line(
      map(i, 0, data.size(), originX, originX + plotWidth),
      map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight),
      map(i+1, 0, data.size(), originX, originX + plotWidth),
      map(data.get(i+1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
    );
    //println("" + (int) data.get(i));
  }
}

void drawPlot(int[] data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {
  // Draw lines connecting all points
  for (int i = 0; i < data.length-1; i++) {
    // stroke(255,0,0);
    // strokeWeight(1);
    line(
      map(i, 0, data.length, originX, originX + plotWidth),
      map(data[i], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight),
      map(i+1, 0, data.length, originX, originX + plotWidth),
      map(data[i+1], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
    );
  }
}

void drawPlot(float[] data, int originX, int originY, int plotWidth, int plotHeight, float plotRangeFloor, float plotRangeCeiling) {
  // Draw lines connecting all points
  for (int i = 0; i < data.length-1; i++) {
    // stroke(255,0,0);
    // strokeWeight(1);
    line(
      map(i, 0, data.length, originX, originX + plotWidth),
      map(data[i], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight),
      map(i+1, 0, data.length, originX, originX + plotWidth),
      map(data[i+1], plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
    );
  }
}
