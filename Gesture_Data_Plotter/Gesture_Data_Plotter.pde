
int dataTimestamp = 0;
float roll = 0, minRoll = 0, maxRoll = 0, avgRoll = 0;
float pitch = 0, minPitch = 0, maxPitch = 0, avgPitch = 0;
float yaw = 0, minYaw = 0, maxYaw = 0, avgYaw = 0;
int gyroX, gyroY, gyroZ;
int accelerometerX, accelerometerY, accelerometerZ;
int magnetometerX, magnetometerY, magnetometerZ;
float pressure, altitude, temperature;

PFont f, f2, f3;

PrintWriter sensorDataFile;
PrintWriter gestureDataFile;

JSONArray gestureSampleSet;
JSONArray gestureDataSample;

boolean showGesturePrompt = true;
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

boolean showAxisX = true, showAxisY = true, showAxisZ = true;

void setup () {
  size(1200, 800, P3D);
  
  // Set up font
  f = createFont("Quicksand-Regular.ttf", 64, true);
  f2 = createFont("Arial", 12, true);
  f3 = createFont("Arial", 16, true);
  
  gestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  
  print("Opening gesture data... ");
  openGestureData();
  println("Done.");
}

void draw () {
  
  // Set background
  background(backgroundColor[0], backgroundColor[1], backgroundColor[2]);

  // Draw data
  drawGestureTitle();
  //drawGesturePlot();
  drawGesturePlotBoundaries();
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
    
  } else if (key == 'x') {
    showAxisX = !showAxisX;
  } else if (key == 'y') {
    showAxisY = !showAxisY;
  } else if (key == 'z') {
    showAxisZ = !showAxisZ;
  } else if (key == TAB) {
    
    gestureIndex = (gestureIndex + 1) % gestureName.length;
    
    gestureSelectionTime = millis();
    
    updateCurrentGesture();
  }
}

void drawGestureTitle() {
  if (showGesturePrompt) {
    fill(0); textFont(f); textAlign(CENTER);
    text("\"" + gestureName[gestureIndex] + "\"", (width / 2), (height / 4));
  }
}
int maximumSampleSize = 0;
int maximumSampleDuration = 0;
void updateCurrentGesture() {
  
  gestureSamples.clear();
  maximumSampleSize = 0;
  maximumSampleDuration = Integer.MIN_VALUE;
  
  // Populate gesture data arrays
  for (int i = 0; i < gestureSampleSet.size(); i++) {
    JSONObject gestureSample = gestureSampleSet.getJSONObject(i);
    JSONArray gestureSamplePoints = gestureSample.getJSONArray("sample");
    
    int sampleTimeStart = 0, sampleTimeEnd = 0, sampleDuration = 0;
    
    ArrayList<ArrayList<Integer>> singleGestureSample = new ArrayList<ArrayList<Integer>>();
    
    // Populate data arrays
    if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {
      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      
      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
      if (sampleDuration > maximumSampleDuration) {
        maximumSampleDuration = sampleDuration;
      }
      
      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
      }
      
      singleGestureSample.add(gestureSamplePointX);
      singleGestureSample.add(gestureSamplePointY);
      singleGestureSample.add(gestureSamplePointZ);
      
      // Update maximum sample count
      if (gestureSamplePoints.size() > maximumSampleSize) {
        maximumSampleSize = gestureSamplePoints.size();
      }
    }

    gestureSamples.add(singleGestureSample);
  }
}

void drawGesturePlot() {
//  fill(0);
//  textFont(f3);
//  text("Accelerometer", (width / 16), height - 150); // ㎭
//  textFont(f2);
//  fill(255, 0, 0); text("X: " + accelerometerX, width / 16, height - 130);
//  fill(0, 255, 0); text("Y: " + accelerometerY, width / 16, height - 110);
//  fill(0, 0, 255); text("Z: " + accelerometerZ, width / 16, height - 90);
  
  // Draw lines connecting all points
  smooth();
  strokeWeight(1);
  for (int i = 0; i < gestureSamples.size(); i++) {
    ArrayList<ArrayList<Integer>> gestureSample = gestureSamples.get(i);
    if (gestureSample.size() > 0) {
      stroke(255,0,0); drawPlot(gestureSample.get(0), 0, height / 2, width, height, 0, 1000);
      stroke(0,255,0); drawPlot(gestureSample.get(1), 0, height / 2, width, height, 0, 1000);
      stroke(0,0,255); drawPlot(gestureSample.get(2), 0, height / 2, width, height, 0, 1000);
    }
  }
}

void drawGesturePlotBoundaries() {
  
  ArrayList<ArrayList<Integer>> gestureSampleUpperBounds = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleLowerBounds = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleAverageSum = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleAverageCount = new ArrayList<ArrayList<Integer>>();
  
  if (gestureSampleUpperBounds.size() < 3) {
    gestureSampleUpperBounds.add(new ArrayList<Integer>());
    gestureSampleUpperBounds.add(new ArrayList<Integer>());
    gestureSampleUpperBounds.add(new ArrayList<Integer>());
  }
  
  if (gestureSampleLowerBounds.size() < 3) {
    gestureSampleLowerBounds.add(new ArrayList<Integer>());
    gestureSampleLowerBounds.add(new ArrayList<Integer>());
    gestureSampleLowerBounds.add(new ArrayList<Integer>());
  }
  
  if (gestureSampleAverageSum.size() < 3) {
    gestureSampleAverageSum.add(new ArrayList<Integer>());
    gestureSampleAverageSum.add(new ArrayList<Integer>());
    gestureSampleAverageSum.add(new ArrayList<Integer>());
  }
  
  if (gestureSampleAverageCount.size() < 3) {
    gestureSampleAverageCount.add(new ArrayList<Integer>());
    gestureSampleAverageCount.add(new ArrayList<Integer>());
    gestureSampleAverageCount.add(new ArrayList<Integer>());
  }
  
  // Draw lines connecting all points
  smooth();
  strokeWeight(1);
  int sampleCount = 0;
  for (int i = 0; i < gestureSamples.size(); i++) {
    
    ArrayList<ArrayList<Integer>> singleGestureSample = gestureSamples.get(i);
    
    if (singleGestureSample.size() > 0) {
      sampleCount++;
      
      /**
       * Draw gesture accelerometer x-axis data
       */
      
      //stroke(255,0,0, 20); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
      if (showAxisX) {
        stroke(255,0,0, 20); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
      }
      
      // Update list size
      while (gestureSampleUpperBounds.get(0).size() < singleGestureSample.get(0).size()) {
        gestureSampleUpperBounds.get(0).add(Integer.MIN_VALUE);
        gestureSampleLowerBounds.get(0).add(Integer.MAX_VALUE);
        gestureSampleAverageSum.get(0).add(0);
        gestureSampleAverageCount.get(0).add(0);
      }
    
      // Update all upper bounds
      for (int j = 0; j < singleGestureSample.get(0).size(); j++) {
        if (singleGestureSample.get(0).get(j) > gestureSampleUpperBounds.get(0).get(j)) { // Check if the current value is greater than the currently-stored upper bound
          gestureSampleUpperBounds.get(0).set(j, singleGestureSample.get(0).get(j)); // Update value
        }
        
        // Update lower bound
        if (singleGestureSample.get(0).get(j) < gestureSampleLowerBounds.get(0).get(j)) { // Check if the current value is greater than the currently-stored upper bound
          gestureSampleLowerBounds.get(0).set(j, singleGestureSample.get(0).get(j)); // Update value
        }
        
        // Update average
        int cumulativeGestureSample = singleGestureSample.get(0).get(j) + gestureSampleAverageSum.get(0).get(j);
        gestureSampleAverageSum.get(0).set(j, cumulativeGestureSample); // Update value
        // Update average count
        int gestureSampleCount = gestureSampleAverageCount.get(0).get(j) + 1;
        gestureSampleAverageCount.get(0).set(j, gestureSampleCount); // Update value
      }
      
      /**
       * Draw gesture accelerometer x-axis data
       */
      
      if (showAxisY) {
        stroke(0,255,0, 20); drawPlot(singleGestureSample.get(1), 0, height / 2, width, height, 0, 1000);
      }
      
      // Update list size
      while (gestureSampleUpperBounds.get(1).size() < singleGestureSample.get(1).size()) {
        gestureSampleUpperBounds.get(1).add(Integer.MIN_VALUE);
        gestureSampleLowerBounds.get(1).add(Integer.MAX_VALUE);
        gestureSampleAverageSum.get(1).add(0);
        gestureSampleAverageCount.get(1).add(0);
      }
    
      // Update all upper bounds
      for (int j = 0; j < singleGestureSample.get(1).size(); j++) {
        if (singleGestureSample.get(1).get(j) > gestureSampleUpperBounds.get(1).get(j)) { // Check if the current value is greater than the currently-stored upper bound
          gestureSampleUpperBounds.get(1).set(j, singleGestureSample.get(1).get(j)); // Update value
        }
        
        // Update lower bound
        if (singleGestureSample.get(1).get(j) < gestureSampleLowerBounds.get(1).get(j)) { // Check if the current value is greater than the currently-stored upper bound
          gestureSampleLowerBounds.get(1).set(j, singleGestureSample.get(1).get(j)); // Update value
        }
        
        // Update average
        int cumulativeGestureSample = singleGestureSample.get(1).get(j) + gestureSampleAverageSum.get(1).get(j);
        gestureSampleAverageSum.get(1).set(j, cumulativeGestureSample); // Update value
        // Update average count
        int gestureSampleCount = gestureSampleAverageCount.get(1).get(j) + 1;
        gestureSampleAverageCount.get(1).set(j, gestureSampleCount); // Update value
      }
      
      /**
       * Draw gesture accelerometer x-axis data
       */
      
      if (showAxisZ) {
        stroke(0,0,255, 20); drawPlot(singleGestureSample.get(2), 0, height / 2, width, height, 0, 1000);
      }
      
      // Update list size
      while (gestureSampleUpperBounds.get(2).size() < singleGestureSample.get(2).size()) {
        gestureSampleUpperBounds.get(2).add(Integer.MIN_VALUE);
        gestureSampleLowerBounds.get(2).add(Integer.MAX_VALUE);
        gestureSampleAverageSum.get(2).add(0);
        gestureSampleAverageCount.get(2).add(0);
      }
    
      // Update all upper bounds
      for (int j = 0; j < singleGestureSample.get(2).size(); j++) {
        if (singleGestureSample.get(2).get(j) > gestureSampleUpperBounds.get(2).get(j)) { // Check if the current value is greater than the currently-stored upper bound
          gestureSampleUpperBounds.get(2).set(j, singleGestureSample.get(2).get(j)); // Update value
        }
        
        // Update lower bound
        if (singleGestureSample.get(2).get(j) < gestureSampleLowerBounds.get(2).get(j)) { // Check if the current value is greater than the currently-stored upper bound
          gestureSampleLowerBounds.get(2).set(j, singleGestureSample.get(2).get(j)); // Update value
        }
        
        // Update average
        int cumulativeGestureSample = singleGestureSample.get(2).get(j) + gestureSampleAverageSum.get(2).get(j);
        gestureSampleAverageSum.get(2).set(j, cumulativeGestureSample); // Update value
        // Update average count
        int gestureSampleCount = gestureSampleAverageCount.get(2).get(j) + 1;
        gestureSampleAverageCount.get(2).set(j, gestureSampleCount); // Update value
      }
      
//      stroke(0,255,0); drawPlot(singleGestureSample.get(1), 0, height / 2, width, height, 0, 1000);
//      stroke(0,0,255); drawPlot(singleGestureSample.get(2), 0, height / 2, width, height, 0, 1000);
    }
  }
  
  // Compute average of accelerometer x-axis data
  for (int j = 0; j < gestureSampleAverageSum.get(0).size(); j++) {
    //int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(0).get(j)) / float(sampleCount));
    int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(0).get(j)) / float(gestureSampleAverageCount.get(0).get(j)));
    gestureSampleAverageSum.get(0).set(j, cumulativeGestureSample); // Update value
  }
  
  for (int j = 0; j < gestureSampleAverageSum.get(1).size(); j++) {
    //int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(1).get(j)) / float(sampleCount));
    int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(1).get(j)) / float(gestureSampleAverageCount.get(1).get(j)));
    gestureSampleAverageSum.get(1).set(j, cumulativeGestureSample); // Update value
  }
  
  for (int j = 0; j < gestureSampleAverageSum.get(2).size(); j++) {
    //int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(2).get(j)) / float(sampleCount));
    int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(2).get(j)) / float(gestureSampleAverageCount.get(2).get(j)));
    gestureSampleAverageSum.get(2).set(j, cumulativeGestureSample); // Update value
  }
  
  // maximumSampleSize
//  int divisions = 10;
//  for (int i = 0; i < (divisions - 1); i++) {
//    stroke(0, 0, 0, 75);
//    line(0 + (i + 1) * (width / divisions), 0, 0 + (i + 1) * (width / divisions), height);
//  }
  
  if (showAxisX) {
    stroke(255,0,0,85); drawPlot(gestureSampleUpperBounds.get(0), 0, height / 2, width, height, 0, 1000);
    stroke(255,0,0,85); drawPlot(gestureSampleLowerBounds.get(0), 0, height / 2, width, height, 0, 1000);
    stroke(255,0,0,180); drawPlot(gestureSampleAverageSum.get(0), 0, height / 2, width, height, 0, 1000);
    drawPlotNodes(12, gestureSampleAverageSum.get(0), 0, height / 2, width, height, 0, 1000);
  }
  
  if (showAxisY) {
    stroke(0,255,0,85); drawPlot(gestureSampleUpperBounds.get(1), 0, height / 2, width, height, 0, 1000);
    stroke(0,255,0,85); drawPlot(gestureSampleLowerBounds.get(1), 0, height / 2, width, height, 0, 1000);
    stroke(0,255,0,180); drawPlot(gestureSampleAverageSum.get(1), 0, height / 2, width, height, 0, 1000);
    drawPlotNodes(12, gestureSampleAverageSum.get(1), 0, height / 2, width, height, 0, 1000);
  }
  
  if (showAxisZ) {
    stroke(0,0,255,85); drawPlot(gestureSampleUpperBounds.get(2), 0, height / 2, width, height, 0, 1000);
    stroke(0,0,255,85); drawPlot(gestureSampleLowerBounds.get(2), 0, height / 2, width, height, 0, 1000);
    stroke(0,0,255,180); drawPlot(gestureSampleAverageSum.get(2), 0, height / 2, width, height, 0, 1000);
    drawPlotNodes(12, gestureSampleAverageSum.get(2), 0, height / 2, width, height, 0, 1000);
  }
}

void drawPlot(ArrayList<Integer> data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {
    
  // Plot data
  for (int i = 0; i < data.size() - 1; i++) {
    line(
      map(i, 0, maximumSampleSize, originX, originX + plotWidth),
      map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight),
      map(i+1, 0, maximumSampleSize, originX, originX + plotWidth),
      map(data.get(i+1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight)
    );
  }
}

void drawPlotNodes(int divisions, ArrayList<Integer> data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {
  
  // maximumSampleSize
  int currentDivision = 0;
  int lastDivisionValue = 0;
    
  for (int i = 0; i < data.size(); i++) {
    
    //if (i == ((i + 1) * (data.size() / divisions))) {
    if (i == floor((currentDivision + 1) * (data.size() / (divisions - 1)))) {
    
      // Draw circle over key moment used for gesture classification (recognition)
      fill(255,255,255,0);
      ellipse(
        map(i, 0, maximumSampleSize, originX, originX + plotWidth),
        map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight),
        20,
        20
      );
      
      currentDivision++;
    }
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
