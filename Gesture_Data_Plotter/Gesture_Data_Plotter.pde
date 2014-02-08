
boolean shiftPressed = false;

int dataTimestamp = 0;
float roll = 0, minRoll = 0, maxRoll = 0, avgRoll = 0;
float pitch = 0, minPitch = 0, maxPitch = 0, avgPitch = 0;
float yaw = 0, minYaw = 0, maxYaw = 0, avgYaw = 0;
int gyroX, gyroY, gyroZ;
int accelerometerX, accelerometerY, accelerometerZ;
int magnetometerX, magnetometerY, magnetometerZ;
float pressure, altitude, temperature;

PFont f, f2, f3;

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
ArrayList<String> gestureSampleNames;
ArrayList<ArrayList<ArrayList<Integer>>> completeGestureSamples;
ArrayList<String> completeGestureSampleNames;

ArrayList<ArrayList<ArrayList<Integer>>> gestureSampleAverage;

boolean showAxisX = true, showAxisY = true, showAxisZ = true;

void setup () {
  size(1200, 800, P3D);
  
  // Set up font
  f = createFont("Quicksand-Regular.ttf", 64, true);
  f2 = createFont("Arial", 12, true);
  f3 = createFont("Arial", 16, true);
  
  // Sample:
  // [sequence][axis][time]
  gestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  completeGestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  gestureSampleNames = new ArrayList<String>();
  completeGestureSampleNames = new ArrayList<String>();
  
  gestureSampleAverage = new ArrayList<ArrayList<ArrayList<Integer>>>();
  
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
  // Compute averages
  
  ArrayList<Float> gestureClassificationScore = new ArrayList<Float>();
  
  // Get most-recent live gesture data
  for (int i = 0; i < getGestureCount(); i++) {
    ArrayList<ArrayList<ArrayList<Integer>>> liveSampleSet = getGestureSamples(i);
    
    // Add counter to compute classification score
    int correctClassificationCount = 0;
    
    for (int j = 0; j < liveSampleSet.size(); j++) {
      ArrayList<ArrayList<Integer>> liveSample = liveSampleSet.get(j);
      
      // Classify the gesture
      int classifiedGestureIndex = classifyGesture(liveSample, 20);
        
//      println("Gesture classification: " + gestureName[classifiedGestureIndex] + "? it's " + gestureName[i]);
      
      // Compute classification score
      if (classifiedGestureIndex == i) {
        correctClassificationCount = correctClassificationCount + 1;
      }
//      println("Deviation: " + gestureIndex);
    }
    
    // Add gesture classification percentage correct
    gestureClassificationScore.add(((float) correctClassificationCount) / (float) liveSampleSet.size());
    
    //print(gestureClassificationScore.get(gestureClassificationScore.size() - 1) + "\t");
    print(correctClassificationCount + "/" + liveSampleSet.size() + "/" + (correctClassificationCount / (float) liveSampleSet.size()) + "\t");
  }
  println();
    
        // Plot sample data
//    for (int axis = 0; axis < 3; axis++) {
//  //    if (axisVisible[axis]) {
//  //      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 180);
//        stroke(255, 0, 0, 180);
//        drawPlot(averageSample.get(axis), 0, height / 2, width, height, 0, 1000);
//        drawPlotNodes(12, averageSample.get(axis), 0, height / 2, width, height, 0, 1000);
//  //    }
//    }
//  }
}

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGesture(ArrayList<ArrayList<Integer>> liveSample, int comparisonFrequency) {
  int minimumDeviationIndex = -1;
  int minimumDeviation = Integer.MAX_VALUE;
    
  for (int gestureSignatureIndex = 0; gestureSignatureIndex < getGestureCount(); gestureSignatureIndex++) {

      ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
      ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
      
      // Calculate the gesture's deviation from the gesture signature
      int gestureDeviation = getGestureDeviation(gestureSignatureSample, liveSample, comparisonFrequency);
//      print(gestureDeviation);
//      println();
      
      // Check if the sample's deviation
      if (minimumDeviationIndex == -1 || gestureDeviation < minimumDeviation) {
        minimumDeviationIndex = gestureSignatureIndex;
        minimumDeviation = gestureDeviation;
      }
  }
  
  return minimumDeviationIndex;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureDeviation(ArrayList<ArrayList<Integer>> averageSample, ArrayList<ArrayList<Integer>> liveSample, int comparisonFrequency) {
  int deltaTotal = 0;
//  int minimumDeviationSampleIndex = -1;
//  float minimumDeltaTotal = 0.0;
  
  if (averageSample.size() > 0) {
    
    // Compare the difference between the average sample for each axis and the live sample
    for (int axis = 0; axis < 3; axis++) {
      ArrayList<Integer> liveSampleAxis = liveSample.get(axis);
      
      int delta = getGestureAxisDeviation2(averageSample.get(axis), liveSample.get(axis), comparisonFrequency);
      deltaTotal = deltaTotal + delta;
        
//      print(delta);
//      print("\t");
    }
  }
  
  return deltaTotal;
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation(ArrayList<Integer> gestureSample, ArrayList<Integer> liveSample, int comparisonFrequency) {
  
  int currentDivision = 0;
  
  int delta = 0; // sum of difference between average x curve and most-recent x data
    
  for (int i = 0; i < gestureSample.size(); i++) {
    if (liveSample.size() > i) {
      if (i == floor((currentDivision + 1) * (gestureSample.size() / (comparisonFrequency - 1)))) {
      
        int difference = abs(gestureSample.get(i) - liveSample.get(i));
        delta = delta + difference;
        
        currentDivision++;
      }
    }
//    else {
//      println("foo");
//    }
  }
  
  return delta;
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation2(ArrayList<Integer> gestureSample, ArrayList<Integer> liveSample, int comparisonFrequency) {
  
  int delta = 0; // sum of difference between average x curve and most-recent x data
    
  for (int i = 0; i < gestureSample.size(); i++) {
    if (i < liveSample.size() && i < gestureSample.size()) {
//      if (i == floor((currentDivision + 1) * (gestureSample.size() / (comparisonFrequency - 1)))) {
      
        int difference = abs(gestureSample.get(i) - liveSample.get(i));
        delta = delta + difference;
//      }
    }
  }
  
  return delta;
}

void openGestureData() {
  
  // Load existing gesture data file
  gestureSampleSet = loadJSONArray("data/gestureSampleSet.json");
  
  updateCurrentGesture();
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == SHIFT) {
      shiftPressed = true;
    }
  }
  
  if (key == ESC) {
    exit(); // Stops the program
  } else if (key == 'x') {
    showAxisX = !showAxisX;
  } else if (key == 'y') {
    showAxisY = !showAxisY;
  } else if (key == 'z') {
    showAxisZ = !showAxisZ;
  } else if (key == TAB) {
    
    if (shiftPressed) {
      if (gestureIndex == 0) {
        gestureIndex = (gestureName.length - 1);
      } else {
        gestureIndex = (gestureIndex - 1) % gestureName.length;
      }
    } else {
      gestureIndex = (gestureIndex + 1) % gestureName.length;
    }
    
    gestureSelectionTime = millis();
    
    updateCurrentGesture();
  }
}

void keyReleased() {
  if (key == CODED) {
    if (keyCode == SHIFT) {
      shiftPressed = false;
    }
  }
}

/**
 * Returns the number of gestures.
 */
int getGestureCount() {
  return gestureName.length;
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
  
  completeGestureSamples.clear();
  
  gestureSamples.clear();
  maximumSampleSize = 0;
  maximumSampleDuration = Integer.MIN_VALUE;
  
  gestureSampleNames.clear();
  completeGestureSampleNames.clear();
  
  // Populate gesture data arrays
  for (int i = 0; i < gestureSampleSet.size(); i++) {
    JSONObject gestureSample = gestureSampleSet.getJSONObject(i);
    JSONArray gestureSamplePoints = gestureSample.getJSONArray("sample");
    
    int sampleTimeStart = 0, sampleTimeEnd = 0, sampleDuration = 0, sampleTimeOffset = 0;
    
    ArrayList<ArrayList<Integer>> singleGestureSample = new ArrayList<ArrayList<Integer>>();
    
    // Populate data arrays
    if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {
      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>();
      
      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
      if (sampleDuration > maximumSampleDuration) {
        maximumSampleDuration = sampleDuration;
      }
      sampleTimeOffset = int(gestureSamplePoints.getJSONObject(0).getString("timestamp")); // The offset for the current sample (reset offset so time starts at zero)
      
      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
        gestureSamplePointT.add(int(gestureSamplePoint.getString("timestamp")) - sampleTimeOffset);
      }
      
      singleGestureSample.add(gestureSamplePointX);
      singleGestureSample.add(gestureSamplePointY);
      singleGestureSample.add(gestureSamplePointZ);
      singleGestureSample.add(gestureSamplePointT);
      
      // Update maximum sample count
      if (gestureSamplePoints.size() > maximumSampleSize) {
        maximumSampleSize = gestureSamplePoints.size();
      }
      
      gestureSamples.add(singleGestureSample);
      
      String gestureName = gestureSample.getString("gesture");
      gestureSampleNames.add(gestureName);
      
      completeGestureSamples.add(singleGestureSample);
      completeGestureSampleNames.add(gestureName);
      
    } else {
      
      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>();
      
      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
//      if (sampleDuration > maximumSampleDuration) {
//        maximumSampleDuration = sampleDuration;
//      }
      sampleTimeOffset = int(gestureSamplePoints.getJSONObject(0).getString("timestamp")); // The offset for the current sample (reset offset so time starts at zero)
      
      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
        gestureSamplePointT.add(int(gestureSamplePoint.getString("timestamp")) - sampleTimeOffset);
      }
      
      singleGestureSample.add(gestureSamplePointX);
      singleGestureSample.add(gestureSamplePointY);
      singleGestureSample.add(gestureSamplePointZ);
      singleGestureSample.add(gestureSamplePointT);
      
      // Update maximum sample count
//      if (gestureSamplePoints.size() > maximumSampleSize) {
//        maximumSampleSize = gestureSamplePoints.size();
//      }
      
//      gestureSamples.add(singleGestureSample);
      
//      String gestureName = gestureSample.getString("gesture");
//      gestureSampleNames.add(gestureName);
      
      completeGestureSamples.add(singleGestureSample);
      
      String gestureName = gestureSample.getString("gesture");
      completeGestureSampleNames.add(gestureName);
    } 
    
    // gestureSamples.add(singleGestureSample);
  }
  
  // Resize sample sequences to be equal to the length of the maximum sample
//  for (int sequence = 0; sequence < gestureSamples.size(); sequence++) {
//    for (int axis = 0; axis < gestureSamples.get(sequence).size(); axis++) {
//      while (gestureSamples.get(sequence).get(axis).size() < maximumSampleSize) {
//        gestureSamples.get(sequence).get(axis).add(int(1));
//      }
//    }
//  }
}

void drawGesturePlot() {
  
  // Draw lines connecting all points
  smooth();
  strokeWeight(1);
  for (int i = 0; i < gestureSamples.size(); i++) {
    ArrayList<ArrayList<Integer>> singleGestureSample = gestureSamples.get(i);
    if (singleGestureSample.size() > 0) {
      stroke(255,0,0); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
      stroke(0,255,0); drawPlot(singleGestureSample.get(1), 0, height / 2, width, height, 0, 1000);
      stroke(0,0,255); drawPlot(singleGestureSample.get(2), 0, height / 2, width, height, 0, 1000);
    }
  }
}

/**
 * Returns a list of all gesture samples for the gesture with the specified index.
 * The returned list is in the following format:
 *    samples[sampleIndex][axis][pointIndex], where
 *      sampleIndex : [0, sampleCount]
 *      axis : [0, 1, 2] -> [x, y, z]
 *      pointIndex : [0, pointIndexCount]
 */
ArrayList<ArrayList<ArrayList<Integer>>> getGestureSamples(int gestureIndex) {
  
  // Declare the list that will store the gesture samples for the gesture with the specified index
  ArrayList<ArrayList<ArrayList<Integer>>> selectedGestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  
  // Populate gesture data arrays
  for (int i = 0; i < gestureSampleSet.size(); i++) {
    JSONObject gestureSample = gestureSampleSet.getJSONObject(i);
    JSONArray gestureSamplePoints = gestureSample.getJSONArray("sample");
    
    // Variables to store time characteristics of the current, single gesture sample
    int sampleTimeStart  = 0; // Gesture sample start time
    int sampleTimeEnd    = 0; // Gesture sample end time
    int sampleDuration   = 0; // Gesture sample duration
    int sampleTimeOffset = 0; // Gesture time offset
    
    // List to store the time process (multiple series)
    ArrayList<ArrayList<Integer>> singleGestureSample = new ArrayList<ArrayList<Integer>>();
    
    // Populate lists of gesture sample data
    //if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {
    if (gestureSample.getString("gesture").equals(gestureName[gestureIndex])) {
      
      // Time series data for gesture
      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>();
      
      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
      if (sampleDuration > maximumSampleDuration) {
        maximumSampleDuration = sampleDuration;
      }
      sampleTimeOffset = int(gestureSamplePoints.getJSONObject(0).getString("timestamp")); // The offset for the current sample (reset offset so time starts at zero)
      
      for (int j = 0; j < gestureSamplePoints.size(); j++) {
        JSONObject gestureSamplePoint = gestureSamplePoints.getJSONObject(j);
        gestureSamplePointX.add(int(gestureSamplePoint.getString("accelerometerX")));
        gestureSamplePointY.add(int(gestureSamplePoint.getString("accelerometerY")));
        gestureSamplePointZ.add(int(gestureSamplePoint.getString("accelerometerZ")));
        gestureSamplePointT.add(int(gestureSamplePoint.getString("timestamp")) - sampleTimeOffset);
      }
      
      // Add data point to time the series
      singleGestureSample.add(gestureSamplePointX);
      singleGestureSample.add(gestureSamplePointY);
      singleGestureSample.add(gestureSamplePointZ);
      singleGestureSample.add(gestureSamplePointT);
      
      // Update maximum sample count
      if (gestureSamplePoints.size() > maximumSampleSize) {
        maximumSampleSize = gestureSamplePoints.size();
      }
      
      // Add gesture process to list that will be returned
      selectedGestureSamples.add(singleGestureSample);
      
//      String gestureName = gestureSample.getString("gesture");
//      gestureSampleNames.add(gestureName);
      
//      completeGestureSamples.add(singleGestureSample);
//      completeGestureSampleNames.add(gestureName);
      
    }
  }
  
  return selectedGestureSamples;
}

//ArrayList<ArrayList<ArrayList<Integer>>> getGestureSamples(int gestureIndex) {
//  
//}

/**
 * Returns the average time series curve (i.e., the average gesture signature)
 * used for gesture classification.
 *
 * Note: This function was made to be called after
 *       ArrayList<ArrayList<ArrayList<Integer>>> getGestureSamples(int gestureIndex).
 */
ArrayList<ArrayList<Integer>> getGestureSampleAverage(ArrayList<ArrayList<ArrayList<Integer>>> currentGestureSamples) {
  
  ArrayList<ArrayList<Integer>> gestureSampleAverageSum = new ArrayList<ArrayList<Integer>>();
  ArrayList<ArrayList<Integer>> gestureSampleAverageCount = new ArrayList<ArrayList<Integer>>();
  
  // Create lists
//  for (int i = 0; i < 3; i++) {
//    gestureSampleAverageSum.add(new ArrayList<Integer>());
//    gestureSampleAverageCount.add(new ArrayList<Integer>());
//  }
  
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
  
  // Compute averages
  int sampleCount = 0;
  for (int i = 0; i < currentGestureSamples.size(); i++) {
    
    ArrayList<ArrayList<Integer>> singleGestureSample = currentGestureSamples.get(i);
    
    if (singleGestureSample.size() > 0) {
      sampleCount++;
      
      for (int axis = 0; axis < 3; axis++) {
        
        // Update list sizes
        while (gestureSampleAverageSum.get(axis).size() < singleGestureSample.get(axis).size()) {
//          gestureSampleUpperBounds.get(axis).add(Integer.MIN_VALUE);
//          gestureSampleLowerBounds.get(axis).add(Integer.MAX_VALUE);
          gestureSampleAverageSum.get(axis).add(0);
          gestureSampleAverageCount.get(axis).add(0);
        }
      
        // Update averages
        for (int j = 0; j < singleGestureSample.get(axis).size(); j++) {
          
          // Update average
          int cumulativeGestureSample = singleGestureSample.get(axis).get(j) + gestureSampleAverageSum.get(axis).get(j);
          gestureSampleAverageSum.get(axis).set(j, cumulativeGestureSample); // Update value
          
          // Update average count
          int gestureSampleCount = gestureSampleAverageCount.get(axis).get(j) + 1;
          gestureSampleAverageCount.get(axis).set(j, gestureSampleCount); // Update value
        }
      }
    }
  }
  
  // Compute average of accelerometer x-axis, y-axis, and z-axis data
  for (int axis = 0; axis < 3; axis++) {
    // Compute average of accelerometer data for current axis
    for (int j = 0; j < gestureSampleAverageSum.get(axis).size(); j++) {
      int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(axis).get(j)) / float(gestureSampleAverageCount.get(axis).get(j)));
      gestureSampleAverageSum.get(axis).set(j, cumulativeGestureSample); // Update value
    }
  }

  return gestureSampleAverageSum;
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
  
  // TODO: Move axisVisible and axisColor elsewhere... for efficiency's sake!
  boolean axisVisible[] = { showAxisX, showAxisY, showAxisZ };
  int axisColor[][] = {
    { 255, 0, 0, 20 },
    { 0, 255, 0, 20 },
    { 0, 0, 255, 20 }
  };
  
  // Draw lines connecting all points
  smooth();
  strokeWeight(1);
  int sampleCount = 0;
  for (int i = 0; i < gestureSamples.size(); i++) {
    
    ArrayList<ArrayList<Integer>> singleGestureSample = gestureSamples.get(i);
    
    if (singleGestureSample.size() > 0) {
      sampleCount++;
      
      for (int axis = 0; axis < 3; axis++) {
        
        // Draw gesture accelerometer data for current axis
        
        //stroke(255,0,0, 20); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
        if (axisVisible[axis]) {
          stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], axisColor[axis][3]); drawPlot(singleGestureSample.get(axis), 0, height / 2, width, height, 0, 1000);
        }
        
        // Update list sizes
        while (gestureSampleUpperBounds.get(axis).size() < singleGestureSample.get(axis).size()) {
          gestureSampleUpperBounds.get(axis).add(Integer.MIN_VALUE);
          gestureSampleLowerBounds.get(axis).add(Integer.MAX_VALUE);
          gestureSampleAverageSum.get(axis).add(0);
          gestureSampleAverageCount.get(axis).add(0);
        }
      
        // Update all upper bounds
        for (int j = 0; j < singleGestureSample.get(axis).size(); j++) {
          
          if (singleGestureSample.get(axis).get(j) > gestureSampleUpperBounds.get(axis).get(j)) { // Check if the current value is greater than the currently-stored upper bound
            gestureSampleUpperBounds.get(axis).set(j, singleGestureSample.get(axis).get(j)); // Update value
          }
          
          // Update lower bound
          if (singleGestureSample.get(axis).get(j) < gestureSampleLowerBounds.get(axis).get(j)) { // Check if the current value is greater than the currently-stored upper bound
            gestureSampleLowerBounds.get(axis).set(j, singleGestureSample.get(axis).get(j)); // Update value
          }
          
          // Update average
          int cumulativeGestureSample = singleGestureSample.get(axis).get(j) + gestureSampleAverageSum.get(axis).get(j);
          gestureSampleAverageSum.get(axis).set(j, cumulativeGestureSample); // Update value
          // Update average count
          int gestureSampleCount = gestureSampleAverageCount.get(axis).get(j) + 1;
          gestureSampleAverageCount.get(axis).set(j, gestureSampleCount); // Update value
        }
      }
    }
  }
  
  // Compute average of accelerometer x-axis, y-axis, and z-axis data
  for (int axis = 0; axis < 3; axis++) {
    // Compute average of accelerometer data for current axis
    for (int j = 0; j < gestureSampleAverageSum.get(axis).size(); j++) {
      //int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(0).get(j)) / float(sampleCount));
      int cumulativeGestureSample = int(float(gestureSampleAverageSum.get(axis).get(j)) / float(gestureSampleAverageCount.get(axis).get(j)));
      gestureSampleAverageSum.get(axis).set(j, cumulativeGestureSample); // Update value
    }
  }
  
  // Plot sample data
  for (int axis = 0; axis < 3; axis++) {
    if (axisVisible[axis]) {
      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 85); drawPlot(gestureSampleUpperBounds.get(axis), 0, height / 2, width, height, 0, 1000);
      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 85); drawPlot(gestureSampleLowerBounds.get(axis), 0, height / 2, width, height, 0, 1000);
      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 180); drawPlot(gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
      drawPlotNodes(12, gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
    }
  }
  
//  // Classify gesture
//  float deltaTotal = 0;
//  for (int axis = 0; axis < 3; axis++) {
//    if (axisVisible[axis]) {
////      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 85); drawPlot(gestureSampleUpperBounds.get(axis), 0, height / 2, width, height, 0, 1000);
////      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 85); drawPlot(gestureSampleLowerBounds.get(axis), 0, height / 2, width, height, 0, 1000);
////      stroke(axisColor[axis][0], axisColor[axis][1],axisColor[axis][2], 180); drawPlot(gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
////      drawPlotNodes(12, gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
//      
//      ArrayList<Integer> liveData = gestureSampleAverageSum.get(axis);
//      float delta = classifyGesture(12, gestureSampleAverageSum.get(axis), liveData, 0, height / 2, width, height, 0, 1000);
//      deltaTotal = deltaTotal + delta;
//      
//      print(delta);
//      print("\t");
//    }
//  }
//  println();
  
  
        
        

        
        
        
        // Classify gesture
//        float deltaTotal = 0;
//        if (axisVisible[axis]) {
//          
//          ArrayList<Integer> liveData = singleGestureSample.get(axis);
//          float delta = classifyGesture(12, gestureSampleAverageSum.get(axis), liveData, 0, height / 2, width, height, 0, 1000);
//          deltaTotal = deltaTotal + delta;
//          
//          print(delta);
//          print("\t");
//        }
//        println();
        
//  classifyGestures(gestureSampleAverageSum);

}

//void classifyGestures(ArrayList<ArrayList<Integer>> gestureSampleAverageSum) {
//  int sampleCount = 0;
//  
//  int minimumDeviationSampleIndex = -1;
//  float minimumDeltaTotal = 0;
//  
//  // Classify the current gesture data (TODO: Replace with live data)
////  for (int i = 0; i < gestureSamples.size(); i++) {
//  for (int i = 0; i < completeGestureSamples.size(); i++) {
//    
//    //text("\"" + gestureName[gestureIndex] + "\"", (width / 2), (height / 4));
//    
//    ArrayList<ArrayList<Integer>> singleGestureSample = completeGestureSamples.get(i);
//    
//    if (singleGestureSample.size() > 0) {
//      sampleCount++;
//      
//      float deltaTotal = 0;
//      
//      for (int axis = 0; axis < 3; axis++) {
//
//        // Get live data from accelerometer
//        ArrayList<Integer> liveGestureSample = singleGestureSample.get(axis);
//        
////        float delta = classifyGesture(12, gestureSampleAverageSum.get(axis), liveGestureSample, 0, height / 2, width, height, 0, 1000);
////        deltaTotal = deltaTotal + delta;
//          
//        print(delta);
//        print("\t");
//      }
//      
//      if (minimumDeviationSampleIndex == -1 || deltaTotal < minimumDeltaTotal) {
//        minimumDeviationSampleIndex = i;
//        minimumDeltaTotal = deltaTotal;
//      }
//      
//      print(deltaTotal);
//      print("\t");
//      
//      print(completeGestureSampleNames.get(i)); // Print sample label
//      
//      println();
//    }
//  }
//}

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
