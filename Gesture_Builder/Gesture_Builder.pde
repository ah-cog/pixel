import processing.serial.*;

Serial serialPort;
String serialInputString;

boolean shiftPressed = false;

int dataTimestamp = 0;
float roll = 0, minRoll = 0, maxRoll = 0, avgRoll = 0;
float pitch = 0, minPitch = 0, maxPitch = 0, avgPitch = 0;
float yaw = 0, minYaw = 0, maxYaw = 0, avgYaw = 0;
int gyroX, gyroY, gyroZ;
int accelerometerX, accelerometerY, accelerometerZ;
int magnetometerX, magnetometerY, magnetometerZ;
float pressure, altitude, temperature;

PFont gestureFont, classifiedGestureFont;
PFont boundaryLabelFont;

JSONArray gestureSampleSet;
JSONArray gestureDataSample;

boolean showGesturePrompt = true;
boolean isRecordingGesture = false;
int gestureSelectionTime = 0;

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
int gestureSampleCount = 0;
int gestureSensorSampleCount = 0;

int liveGestureSize = 50;

int maximumSampleSize[] = {
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0
};

int maximumSampleDuration = 0;

// The center of the sampling window for each gesture
int gestureSignatureOffset[] = {
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0
};

// The number of data points that make of each gesture signature (starting at the offset) 
int defaultLiveGestureSize = 50;
int gestureSignatureSize[] = {
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize, 
  defaultLiveGestureSize
};

boolean isFullScreen = true;

// Gesture state machine
ArrayList<ArrayList<Integer>> gestureTransitions = new ArrayList<ArrayList<Integer>>();

ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples;
ArrayList<String> gestureSampleNames;
ArrayList<Integer> gestureSampleWindow;
ArrayList<ArrayList<ArrayList<Integer>>> completeGestureSamples;
ArrayList<String> completeGestureSampleNames;

ArrayList<ArrayList<Integer>> liveGestureSample; // The latest accelerometer data

boolean showAxisX = true, showAxisY = true, showAxisZ = true;

int classifiedGestureIndex = -1;
color backgroundColor = #F0F1F0;

ArrayList<ArrayList<ArrayList<ArrayList<Integer>>>> cachedGestureSamples;
ArrayList<Boolean> hasCachedGestureSamples;

void setup () {
  // size(1200, 800, P3D);
  size(displayWidth, displayHeight, P3D);

  // Set up font
  gestureFont = createFont("DidactGothic.ttf", 64, true);
  classifiedGestureFont = createFont("DidactGothic.ttf", 40, true);
  boundaryLabelFont = createFont("DidactGothic.ttf", 14, true);

  cachedGestureSamples = new ArrayList<ArrayList<ArrayList<ArrayList<Integer>>>>();
  hasCachedGestureSamples = new ArrayList<Boolean>();

  // Sample:
  // [sequence][axis][time]
  gestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  completeGestureSamples = new ArrayList<ArrayList<ArrayList<Integer>>>();
  gestureSampleNames = new ArrayList<String>();
  completeGestureSampleNames = new ArrayList<String>();

  //gestureSampleAverage = new ArrayList<ArrayList<ArrayList<Integer>>>();
  //  gestureSampleAverage = new ArrayList<ArrayList<ArrayList<Integer>>>();

  liveGestureSample = new ArrayList<ArrayList<Integer>>();
  liveGestureSample.add(new ArrayList<Integer>()); // X axis data
  liveGestureSample.add(new ArrayList<Integer>()); // Y
  liveGestureSample.add(new ArrayList<Integer>()); // Z

  print("Opening gesture data... ");
  openGestureData();
  println("Done.");

  updateCurrentGesture();

  // Setup gesture state machine
  setupGestureTransitions();

  // Set up serial ports
  println(Serial.list().length);
  println(Serial.list());

  // Draw out gesture data
  //printGestureSignatures();

  // Connect to the corresponding serial port
  serialPort = new Serial(this, Serial.list()[7], 9600);

  // Defer callback until new line
  serialPort.bufferUntil('\n');
}

void draw() {

  // Check if classified gesture is correct
  if (gestureIndex == classifiedGestureIndex) {
    backgroundColor = #cc0000;
  } else {
    backgroundColor = #F0F1F0;
  }

  // Set background
  //background(backgroundColor[0], backgroundColor[1], backgroundColor[2]);
  background(backgroundColor);

  // Draw data plots
  drawGesturePlotBoundaries();
  drawSignatureData();
  drawLiveGesturePlot();

  //  drawGestureSignatureBoundaries();

  //  // Compute averages
  //  ArrayList<Float> gestureClassificationScore = new ArrayList<Float>();
  //  
  //  // Get most-recent live gesture data
  //  getGestureSamples();
  //  for (int i = 0; i < getGestureCount(); i++) {
  //    ArrayList<ArrayList<ArrayList<Integer>>> liveSampleSet = getGestureSamples(i);
  //    
  //    // Add counter to compute classification score
  //    int correctClassificationCount = 0;
  //    
  //    for (int j = 0; j < liveSampleSet.size(); j++) {
  //      ArrayList<ArrayList<Integer>> liveSample = liveSampleSet.get(j);
  //      
  //      // Compute classification score
  //      if (classifiedGestureIndex == i) {
  //        correctClassificationCount = correctClassificationCount + 1;
  //      }
  //    }
  //    
  //    // Add gesture classification percentage correct
  //    gestureClassificationScore.add(((float) correctClassificationCount) / (float) liveSampleSet.size());
  //  }

  // Draw gesture label and classified gesture lable
  drawGestureTitle();
  //drawGesturePlot();
  drawClassifiedGestureTitle();
}

boolean sketchFullScreen() {
  return isFullScreen;
}

void serialEvent (Serial serialPort) {

  // Read serial data
  if (serialPort.available() > 0) {  // If data is available,
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

          // Push live gesture sample onto queue  
          liveGestureSample.get(0).add(accelerometerX);
          liveGestureSample.get(1).add(accelerometerY);
          liveGestureSample.get(2).add(accelerometerZ);

          // Remove oldest element from live gesture queue if greater than threshold
          if (liveGestureSample.get(0).size() > liveGestureSize) { // TODO: In classifier function, compare only the "latest" values... 
            liveGestureSample.get(0).remove(0);
            liveGestureSample.get(1).remove(0);
            liveGestureSample.get(2).remove(0);
          }
          //          println(liveGestureSample.get(0).size());

          // Classify live gesture sample
          if (liveGestureSample.get(0).size() >= liveGestureSize) {
            classifiedGestureIndex = classifyGestureFromTransitions(liveGestureSample);
          }
        }
      }
    }
  }
}

/**
 * Set up the gesture transitions that can occur from each gesture.
 */
void setupGestureTransitions() {

  // Set the gesture transitions
  for (int i = 0; i < gestureName.length; i++) {
    ArrayList<Integer> currentTransitions = new ArrayList<Integer>(); // Create list for current gesture's transitions

    if (gestureName[i] == "at rest, on table") {
      currentTransitions.add(getGestureIndex("at rest, on table")); // note, becasue it's a continous gesture
      currentTransitions.add(getGestureIndex("pick up"));
      //      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 
    else if (gestureName[i] == "at rest, in hand") {
      currentTransitions.add(getGestureIndex("at rest, in hand")); // note, becasue it's a continous gesture
      currentTransitions.add(getGestureIndex("place down"));
      //      currentTransitions.add(getGestureIndex("at rest, on table"));

      currentTransitions.add(getGestureIndex("shake"));
      currentTransitions.add(getGestureIndex("tilt left"));
      currentTransitions.add(getGestureIndex("tilt right"));
      currentTransitions.add(getGestureIndex("tap to another, as left"));
      currentTransitions.add(getGestureIndex("tap to another, as right"));
    } 
    else if (gestureName[i] == "pick up") {
      currentTransitions.add(getGestureIndex("pick up")); // note, becasue it's a continous gesture
      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 
    else if (gestureName[i] == "place down") {
      currentTransitions.add(getGestureIndex("at rest, on table"));
    } 
    else if (gestureName[i] == "tilt left") {
      currentTransitions.add(getGestureIndex("tilt left"));
      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 
    else if (gestureName[i] == "tilt right") {
      currentTransitions.add(getGestureIndex("tilt right"));
      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 
    else if (gestureName[i] == "shake") {
      currentTransitions.add(getGestureIndex("shake"));
      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 
    else if (gestureName[i] == "tap to another, as left") {
      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 
    else if (gestureName[i] == "tap to another, as right") {
      currentTransitions.add(getGestureIndex("at rest, in hand"));
    } 

    gestureTransitions.add(currentTransitions);
  }

  // Set the initial state
  classifiedGestureIndex = 0;
}

/**
 * Gets the index associated with the specified gesture.
 */
int getGestureIndex(String findGestureName) {
  for (int i = 0; i < gestureName.length; i++) {
    if (gestureName[i].equals(findGestureName)) {
      return i;
    }
  }
  return -1;
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
    int gestureDeviation = getGestureDeviation(gestureSignatureSample, liveSample);
    int gestureInstability = getGestureInstability(gestureSignatureSample, liveSample);

    // Check if the sample's deviation
    if (minimumDeviationIndex == -1 || (gestureDeviation + gestureInstability) < minimumDeviation) {
      minimumDeviationIndex = gestureSignatureIndex;
      minimumDeviation = gestureDeviation + gestureInstability;
    }
  }

  return minimumDeviationIndex;
}

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGestureFromTransitions(ArrayList<ArrayList<Integer>> liveSample) {
  int minimumDeviationIndex = -1;
  int minimumDeviation = Integer.MAX_VALUE;

  ArrayList<Integer> possibleGestures = gestureTransitions.get(classifiedGestureIndex); // Get list of possible gestures based on current state

  // Loop through possible gestures, calculate the deviation betwen the gesture's signature 
  // and the live sensor data sample.
  for (int i = 0; i < possibleGestures.size(); i++) {

    int gestureSignatureIndex = possibleGestures.get(i); // Get index of possible gesture

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //      ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex]);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureSignatureIndex], gestureSignatureSize[gestureSignatureIndex]);

    // Calculate the gesture's deviation from the gesture signature
    int gestureDeviation = getGestureDeviation(gestureSignatureSample, liveSample);
    //int gestureInstability = 0;
    int gestureInstability = getGestureInstability(gestureSignatureSample, liveSample);
    //      println("gestureDeviation = " + gestureDeviation + ", gestureInstability = " + gestureInstability);
    //      println();

    // Check if the sample's deviation
    if (minimumDeviationIndex == -1 || (gestureDeviation + gestureInstability) < minimumDeviation) {
      minimumDeviationIndex = gestureSignatureIndex;
      minimumDeviation = gestureDeviation + gestureInstability;
    }
  }

  return minimumDeviationIndex;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureInstability(ArrayList<ArrayList<Integer>> averageSample, ArrayList<ArrayList<Integer>> liveSample) {
  int instabilityTotal = 0;

  //  println("averageSample = " + averageSample.get(0).size() + ", liveSample = " + liveSample.get(0).size());
  //  if (averageSample.get(0).size() > 0 && liveSample.get(0).size() > 0) {

  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    ArrayList<Integer> liveSampleAxis = liveSample.get(axis);

    int instability = getGestureAxisInstability(averageSample.get(axis), liveSample.get(axis));
    instabilityTotal = instabilityTotal + instability;
    //      print(instability);
    //      print("\t");
  }

  //    print(instabilityTotal);
  //    println();
  //  }

  return instabilityTotal;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureDeviation(ArrayList<ArrayList<Integer>> averageSample, ArrayList<ArrayList<Integer>> liveSample) {
  int deltaTotal = 0;

  //  if (averageSample.size() > 0 && liveSample.size() > 0 && averageSample.size() < liveSample.size()) {

  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    ArrayList<Integer> liveSampleAxis = liveSample.get(axis);

    int delta = getGestureAxisDeviation(averageSample.get(axis), liveSample.get(axis));
    deltaTotal = deltaTotal + delta;

    //      print(delta);
    //      print("\t");
    //    }
    //    println();
  }

  return deltaTotal;
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation(ArrayList<Integer> gestureSample, ArrayList<Integer> liveSample) {

  int delta = 0; // sum of difference between average x curve and most-recent x data

  //  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
  //    if (i < liveSample.size() && i < gestureSample.size()) {
  //        int difference = abs(gestureSample.get(i) - liveSample.get(i));
  //        delta = delta + difference;
  //    }
  //  }

  for (int i = 0; i < gestureSample.size(); i++) {
    if (i < liveSample.size() && i < gestureSample.size()) {
      int difference = abs(gestureSample.get(i) - liveSample.get(i));
      delta = delta + difference;
    }
  }

  return delta;
}

/**
 * Relative instability. How relative is the live sample in comparison to a gesture's signature sample?
 */
int getGestureAxisInstability(ArrayList<Integer> gestureSample, ArrayList<Integer> liveSample) {

  int relativeInstability = 0; // sum of difference between average x curve and most-recent x data

  //  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size() - 1; i++) {
  //    if (i < liveSample.size() && i < gestureSample.size()) {
  //        int signatureDifference = abs(gestureSample.get(i + 1) - gestureSample.get(i));
  //        int liveDifference = abs(liveSample.get(i + 1) - liveSample.get(i));
  //        int instabilityDifference = abs(signatureDifference - liveDifference);
  //        
  //        relativeInstability = relativeInstability + instabilityDifference;
  //    }
  //  }

  for (int i = 0; i < gestureSample.size() - 1; i++) {
    //  for (int i = 0; i < gestureSample.size() - 1; i++) {
    if (i < liveSample.size() && i < gestureSample.size() && gestureSample.size() <= liveSample.size()) {
      int signatureDifference = abs(gestureSample.get(i + 1) - gestureSample.get(i));
      int liveDifference = abs(liveSample.get(i + 1) - liveSample.get(i));
      int instabilityDifference = abs(signatureDifference - liveDifference);

      relativeInstability = relativeInstability + instabilityDifference;
    }
  }

  return relativeInstability;
}

void openGestureData() {

  // Load existing gesture data file
  gestureSampleSet = loadJSONArray("data/gestureSampleSet.json");

  // updateCurrentGesture();
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == SHIFT) {
      shiftPressed = true;
    }

    // Change position of left boundary
    if (keyCode == LEFT) {
      if (shiftPressed) {
        // TODO: ?
      } 
      else {
        // Increase the offset
        if (gestureSignatureOffset[gestureIndex] > 0) {
          gestureSignatureOffset[gestureIndex] = gestureSignatureOffset[gestureIndex] - 1;
          printGestureSignatures();
        }
      }
    }

    // Change position of right boundary
    if (keyCode == RIGHT) {
      if (shiftPressed) {
        // TODO: ?
      } 
      else {
        // Increase the offset
        if ((gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) < maximumSampleSize[gestureIndex]) {
          gestureSignatureOffset[gestureIndex] = gestureSignatureOffset[gestureIndex] + 1;
          printGestureSignatures();
        }
      }
    }

    if (keyCode == UP) {
      if (shiftPressed) {
        // TODO: ?
      } 
      else {
        // Increase the offset
        //        if ((gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) < liveGestureSize) {
        if (gestureSignatureSize[gestureIndex] < liveGestureSize) {
          gestureSignatureSize[gestureIndex] = gestureSignatureSize[gestureIndex] + 1;
        }
        printGestureSignatures();
        //        }
      }
    }

    if (keyCode == DOWN) {
      if (shiftPressed) {
        // TODO: ?
      } 
      else {
        gestureSignatureSize[gestureIndex] = gestureSignatureSize[gestureIndex] - 1;
        printGestureSignatures();
      }
    }
  }

  if (key == ESC) {
    exit(); // Stops the program
  } 
  else if (key == 'x') {
    showAxisX = !showAxisX;
  } 
  else if (key == 'y') {
    showAxisY = !showAxisY;
  } 
  else if (key == 'z') {
    showAxisZ = !showAxisZ;
  } 
  else if (key == 's') {
    saveGestureModelFile();
  } 
  else if (key == TAB) {

    if (shiftPressed) {
      if (gestureIndex == 0) {
        gestureIndex = (gestureName.length - 1);
      } 
      else {
        gestureIndex = (gestureIndex - 1) % gestureName.length;
      }
    } 
    else {
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
    fill(0); 
    textFont(gestureFont); 
    textAlign(CENTER);
    text("" + gestureName[classifiedGestureIndex] + "", (width / 2), (height / 4) - 50);
  }
}

void drawClassifiedGestureTitle() {
  if (showGesturePrompt) {
    if (classifiedGestureIndex != -1) {
      fill(0); 
      textFont(classifiedGestureFont); 
      textAlign(CENTER);
      text("showing " + gestureName[gestureIndex] + "  ", (width / 2), (height / 4) + 20);
    }
  }
}

//void drawGestureSignatureBoundaries() {
//  // gestureSignatureBoundingIndices
//  // gestureIndex
//  
//  // Draw lines connecting all points
//  smooth();
//  strokeWeight(1);
//  
//  // int sampleSize = averageSample.get(0).size();
//  int scaledBoundLeft = int((float(gestureSignatureBoundingIndices[gestureIndex][0]) / float(maximumSampleSize)) * width);
//  int scaledBoundRight = int((float(gestureSignatureBoundingIndices[gestureIndex][1]) / float(maximumSampleSize)) * width);
//  //int scaledWidth = int((float(liveGestureSize) / float(averageSample.get(0).size())) * width);
////  if (liveGestureSample.size() > 0) {
////    stroke(255,0,0); drawPlot(liveGestureSample.get(0), 0, height / 2, scaledWidth, height, 0, 1000);
////    stroke(0,255,0); drawPlot(liveGestureSample.get(1), 0, height / 2, scaledWidth, height, 0, 1000);
////    stroke(0,0,255); drawPlot(liveGestureSample.get(2), 0, height / 2, scaledWidth, height, 0, 1000);
////  }
//  
//  
//  // Draw starting boundary of gesture signature
//  stroke(128, 128, 128);
//  line(scaledBoundLeft, 0, scaledBoundLeft, height);
//  //line(gestureSignatureBoundingIndices[gestureIndex][0], 0, gestureSignatureBoundingIndices[gestureIndex][0], height);
//  
//  // Draw stopping boundary of gesture signature 
//  stroke(128, 128, 128);
//  line(scaledBoundRight, 0, scaledBoundRight, height);
//  //line(scaledWidth, 0, scaledWidth, height);
//  //line(gestureSignatureBoundingIndices[gestureIndex][1], 0, gestureSignatureBoundingIndices[gestureIndex][1], height);
//}

void updateCurrentGesture() {

  completeGestureSamples.clear();

  gestureSamples.clear();
  maximumSampleSize[gestureIndex] = 0;
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
      if (gestureSamplePoints.size() > maximumSampleSize[gestureIndex]) {
        maximumSampleSize[gestureIndex] = gestureSamplePoints.size();
      }

      // println(maximumSampleSize);

      gestureSamples.add(singleGestureSample);

      String gestureName = gestureSample.getString("gesture");
      gestureSampleNames.add(gestureName);

      completeGestureSamples.add(singleGestureSample);
      completeGestureSampleNames.add(gestureName);
    } 
    else {

      ArrayList<Integer> gestureSamplePointX = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointY = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointZ = new ArrayList<Integer>();
      ArrayList<Integer> gestureSamplePointT = new ArrayList<Integer>();

      // Get gesture duration and set to maximum duration (if that's the case)
      sampleTimeStart = int(gestureSamplePoints.getJSONObject(0).getString("timestamp"));
      sampleTimeEnd = int(gestureSamplePoints.getJSONObject(gestureSamplePoints.size() - 1).getString("timestamp"));
      sampleDuration = sampleTimeEnd - sampleTimeStart;
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
      completeGestureSamples.add(singleGestureSample);

      String gestureName = gestureSample.getString("gesture");
      completeGestureSampleNames.add(gestureName);
    }
  }
}

/**
 * Draw the plot for the currently selected gesture signature.
 */
//void drawGesturePlot() {
//  
//  // Draw lines connecting all points
//  smooth();
//  strokeWeight(1);
//  for (int i = 0; i < gestureSamples.size(); i++) {
//    ArrayList<ArrayList<Integer>> singleGestureSample = gestureSamples.get(i);
//    if (singleGestureSample.size() > 0) {
//      stroke(255,0,0); drawPlot(singleGestureSample.get(0), 0, height / 2, width, height, 0, 1000);
//      stroke(0,255,0); drawPlot(singleGestureSample.get(1), 0, height / 2, width, height, 0, 1000);
//      stroke(0,0,255); drawPlot(singleGestureSample.get(2), 0, height / 2, width, height, 0, 1000);
//    }
//  }
//}

void drawSignatureData() {
  // Draw ending bound of gesture signature
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(RIGHT);
  text("" + gestureName[gestureIndex] + "\nsize " + maximumSampleSize[gestureIndex] + "\nexamples " + gestureSamples.size(), width - 8, 30);
} 

/**
 * Draw the plot for the latest accelerometer data.
 */
void drawLiveGesturePlot() {

  // Draw lines connecting all points
  smooth();
  strokeWeight(1);

  int scaledBoundLeft = int((float(gestureSignatureOffset[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  int scaledBoundRight = int((float(gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) / float(maximumSampleSize[gestureIndex])) * width);
  //int signatureSize = (gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) - gestureSignatureOffset[gestureIndex];

  // int sampleSize = averageSample.get(0).size();
  //  int scaledOffsetStart = int((float(gestureSIgnatureOffset[gestureIndex]) / float(maximumSampleSize)) * width);
  //int scaledWidth = int((float(liveGestureSize + gestureSignatureOffset[gestureIndex]) / float(maximumSampleSize)) * width);
  //int scaledWidth = int((float(liveGestureSize) / float(averageSample.get(0).size())) * width);
  if (liveGestureSample.size() > 0) {
    // TODO: Check here with an "if" whether the index should be drawn (if it's in the boundary range)
    stroke(255, 0, 0); 
    drawPlot(liveGestureSample.get(0), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
    stroke(0, 255, 0); 
    drawPlot(liveGestureSample.get(1), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
    stroke(0, 0, 255); 
    drawPlot(liveGestureSample.get(2), gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex], scaledBoundLeft, height / 2, scaledBoundRight - scaledBoundLeft, height, 0, 1000);
  }

  // Draw beginning bound of gesture signature
  stroke(128, 128, 128);
  line(scaledBoundLeft, 0, scaledBoundLeft, height);
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(LEFT);
  text("least recent\noffset " + gestureSignatureOffset[gestureIndex], scaledBoundLeft + 8, 30);

  // Draw ending bound of gesture signature
  stroke(128, 128, 128);
  line(scaledBoundRight, 0, scaledBoundRight, height);
  fill(0); 
  textFont(boundaryLabelFont); 
  textAlign(RIGHT);
  text("now\nsize " + gestureSignatureSize[gestureIndex], scaledBoundRight - 8, 30);
}

/**
 * Get the accelerometer data samples for all gestures. Cache the gesture samples in memory. 
 */
ArrayList<ArrayList<ArrayList<ArrayList<Integer>>>> getGestureSamples() {

  // Initialize caching flags 
  while (hasCachedGestureSamples.size () < getGestureCount()) {
    hasCachedGestureSamples.add(false);
  }

  // Initialize cache
  while (cachedGestureSamples.size () < getGestureCount()) {
    cachedGestureSamples.add(new ArrayList<ArrayList<ArrayList<Integer>>>());
  }

  // Cache gesture samples for each gesture and flag them as cached for future reference
  for (int i = 0; i < getGestureCount(); i++) {
    if (!hasCachedGestureSamples.get(i)) {
      ArrayList<ArrayList<ArrayList<Integer>>> cachedGestureSample = getGestureSamples(i);
      cachedGestureSamples.set(i, cachedGestureSample); // Cache the samples
      hasCachedGestureSamples.set(i, true); // Flag the samples as cached
    }
  }

  return cachedGestureSamples;
}

void printGestureSignatures() {

  for (int gestureSignatureIndex = 0; gestureSignatureIndex < getGestureCount(); gestureSignatureIndex++) {

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureSignatureIndex], gestureSignatureSize[gestureSignatureIndex]);
    println("Size: " + gestureSignatureSample.get(0).size());

    for (int axis = 0; axis < gestureSignatureSample.size(); axis++) {
      print("gestureSignatures[" + gestureSignatureIndex + "][" + axis + "] = { ");
      int gestureSignatureSize = gestureSignatureSample.get(0).size(); // 50;
      for (int pointIndex = 0; pointIndex < gestureSignatureSize; pointIndex++) {
        print("" + gestureSignatureSample.get(axis).get(pointIndex));
        if (pointIndex < gestureSignatureSize - 1) {
          print(", ");
        } 
        else {
          print(" };");
        }
      }
      println();
    }
    println();
  }
}

/**
 * Writes the current gesture model to a .h header file for use with Arduino sketches.
 */
void saveGestureModelFile() {

  PrintWriter gestureFile = createWriter("Gestures.h");

  // GESTURE_COUNT 9
  // AXIS_COUNT 3
  // GESTURE_SIGNATURE_SIZE 50

  gestureFile.println("#define GESTURE_COUNT 9");
  gestureFile.println("#define AXIS_COUNT 3");
  gestureFile.println("#define GESTURE_SIGNATURE_SIZE 50");
  gestureFile.println("#define DEFAULT_GESTURE_SIGNATURE_SIZE 50");
  gestureFile.println("#define GESTURE_CANDIDATE_SIZE 50 // The number of most recent live data points to store");
  gestureFile.println();

  gestureFile.println("int gestureIndex = 0;");
  gestureFile.println();

  gestureFile.println("char* gestureName[GESTURE_COUNT] = {");
  for (int i = 0; i < getGestureCount(); i++) {
    if (i < getGestureCount() - 1) {
      gestureFile.println("\t" + gestureName[i] + "\",");
    } 
    else {
      gestureFile.println("\t\"" + gestureName[i] + "\"");
    }
  }
  gestureFile.println("};");
  gestureFile.println();

  gestureFile.println("int gestureSignatureOffset[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };");
  gestureFile.println();

  gestureFile.println("int gestureSignatureSize[] = {");
  for (int i = 0; i < getGestureCount(); i++) {
    if (i < getGestureCount() - 1) {
      gestureFile.println("\tDEFAULT_GESTURE_SIGNATURE_SIZE,");
    } 
    else {
      gestureFile.println("\tDEFAULT_GESTURE_SIGNATURE_SIZE");
    }
  }
  gestureFile.println("};");
  gestureFile.println();

  gestureFile.println("int gestureSustainDuration[GESTURE_COUNT] = {");
  gestureFile.println("\t0, // \"at rest, on table\"");
  gestureFile.println("\t0, // \"at rest, in hand\"");
  gestureFile.println("\t0, // \"pick up\"");
  gestureFile.println("\t0, // \"place down\"");
  gestureFile.println("\t0, // \"tilt left\"");
  gestureFile.println("\t0, // \"tilt right\"");
  gestureFile.println("\t0, // \"shake\"");
  gestureFile.println("\t200, // \"tap to another, as left\"");
  gestureFile.println("\t200  // \"tap to another, as right\"");
  gestureFile.println("};");
  gestureFile.println();

  gestureFile.println("int gestureTransitions[GESTURE_COUNT][GESTURE_COUNT] = {");
  for (int i = 0; i < gestureTransitions.size(); i++) {
    gestureFile.print("\t{");

    for (int j = 0; j < gestureTransitions.size(); j++) {
      if (j < gestureTransitions.get(i).size()) {
        gestureFile.print(" " + gestureTransitions.get(i).get(j));
      } 
      else {
        gestureFile.print(" -1");
      }

      if (j < gestureTransitions.size() - 1) {
        gestureFile.print(",");
      }
    }

    // Write either "}," or "}" to the file, depending on whether the data for the last axis are being written or not
    if (i < gestureTransitions.size() - 1) {
      gestureFile.println(" },");
    } 
    else {
      gestureFile.println(" }");
    }
  }
  gestureFile.println("};");

  gestureFile.println("int gestureCandidateSize = 0; // Current size of the live gesture data");

  gestureFile.println("int gestureCandidate[AXIS_COUNT][GESTURE_CANDIDATE_SIZE] = {");
  for (int axis = 0; axis < 3; axis++) {
    gestureFile.print("\t{");
    for (int i = 0; i < liveGestureSize; i++) {
      gestureFile.print(" 0");
      if (i < liveGestureSize - 1) {
        gestureFile.print(",");
      } 
      else {
        // Write either "}," or "}" to the file, depending on whether the data for the last axis are being written or not
        if (axis < 3 - 1) {
          gestureFile.println(" },");
        } 
        else {
          gestureFile.println(" }");
        }
      }
    }
  }
  gestureFile.println("};");

  gestureFile.println("\n");

  gestureFile.println("int gestureSampleCount = 0;");
  gestureFile.println("int gestureSensorSampleCount = 0;");

  gestureFile.println("int gestureSignature[GESTURE_COUNT][AXIS_COUNT][GESTURE_SIGNATURE_SIZE] = {");

  for (int gestureSignatureIndex = 0; gestureSignatureIndex < getGestureCount(); gestureSignatureIndex++) {

    ArrayList<ArrayList<ArrayList<Integer>>> gestureSamples = getGestureSamples(gestureSignatureIndex);
    //ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage(gestureSamples);
    ArrayList<ArrayList<Integer>> gestureSignatureSample = getGestureSampleAverage2(gestureSamples, gestureSignatureOffset[gestureIndex], gestureSignatureSize[gestureIndex]);
    //println("Size: " + gestureSignatureSample.get(0).size());

    gestureFile.println("\t{");

    for (int axis = 0; axis < gestureSignatureSample.size(); axis++) {
      //print("gestureSignature[" + gestureSignatureIndex + "][" + axis + "] = { ");

      gestureFile.print("\t\t{ ");

      int gestureSignatureSize = gestureSignatureSample.get(0).size(); // 50;

      for (int pointIndex = 0; pointIndex < gestureSignatureSize; pointIndex++) {
        gestureFile.print("" + gestureSignatureSample.get(axis).get(pointIndex));
        if (pointIndex < gestureSignatureSize - 1) {
          gestureFile.print(", ");
        } 
        else {
          // Write either "}," or "}" to the file, depending on whether the data for the last axis are being written or not
          if (axis < gestureSignatureSample.size() - 1) {
            gestureFile.println(" },");
          } 
          else {
            gestureFile.println(" }");
          }
        }
      }
    }

    // Write "}," or "}" to the end of the line, depending on whether it's the last gesture or not
    if (gestureSignatureIndex < getGestureCount() - 1) {
      gestureFile.println("\t},");
    } 
    else {
      gestureFile.println("\t}");
    }
  }
  gestureFile.println("};");

  gestureFile.println("int classifiedGestureIndex = 0;");
  gestureFile.println("int previousClassifiedGestureIndex = -1;");
  gestureFile.println("unsigned long lastGestureClassificationTime = 0L; // Time of last gesture classification");

  gestureFile.flush(); // Write remaining data to file
  gestureFile.close(); // Finish writing to the file and close it
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

  if (hasCachedGestureSamples.size() > gestureIndex && hasCachedGestureSamples.get(gestureIndex)) {

    return cachedGestureSamples.get(gestureIndex);
  } 
  else {

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
      int sampleTimeOffset = 0; // Gesture time offset // TODO: Rather than sliding everything to the left, find the best fit for each to each other

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
        if (gestureSamplePoints.size() > maximumSampleSize[gestureIndex]) {
          maximumSampleSize[gestureIndex] = gestureSamplePoints.size();
        }

        // Add gesture process to list that will be returned
        selectedGestureSamples.add(singleGestureSample);
      }
    }
    return selectedGestureSamples;
  }
}

/**
 * Returns a subsample of the gesture signature of the specified size starting from the specified offset.
 */
ArrayList<ArrayList<Integer>> getGestureSampleAverage2(ArrayList<ArrayList<ArrayList<Integer>>> currentGestureSamples, int offset, int size) {

  // Get a list containing the average of gesture examples (i.e., the average sample)
  ArrayList<ArrayList<Integer>> averageSample = getGestureSampleAverage(currentGestureSamples);

  // Initialize lists for the subsample from the gesture signature (i.e., the average of all sample)
  ArrayList<ArrayList<Integer>> averageSubsample = new ArrayList<ArrayList<Integer>>();
  averageSubsample.add(new ArrayList<Integer>()); // initialize x-value list
  averageSubsample.add(new ArrayList<Integer>()); // y
  averageSubsample.add(new ArrayList<Integer>()); // z

  // Calculate the starting index of the sublist
  //  int sublistStartIndex = 0;
  //  if (size < averageSample.get(0).size()) {
  //    sublistStartIndex = (averageSample.get(0).size() - size); // Set new start index for list
  //  }

  int sublistStartIndex = 0;
  if (size < averageSample.get(0).size()) {
    sublistStartIndex = offset; // Set new start index for list
  }

  //  println();
  //  println("gestureIndex = " + gestureIndex);
  //  println("offset = " + offset);
  //  println("size = " + size);
  //  println("averageSample.get(" + gestureIndex + ").size() = " + averageSample.get(0).size());
  //  println("sublistStartIndex = " + sublistStartIndex);

  // Create sublist. Copy elements from gesture signature to a new list structure (the sublist).
  // if (offset >= 0 && (offset + size) < averageSample.get(0).size()) {
  //  if (offset >= 0 && (offset + size) < averageSample.get(0).size()) {
  //    for (int i = offset; i < offset + size; i++) {
  //for (int i = sublistStartIndex; i < averageSample.get(0).size(); i++) {
  for (int i = sublistStartIndex; i < offset + size; i++) {
    averageSubsample.get(0).add(averageSample.get(0).get(i));
    averageSubsample.get(1).add(averageSample.get(1).get(i));
    averageSubsample.get(2).add(averageSample.get(2).get(i));
  }

  //    println("offset = " + offset + ", size = " + size + ", size(sample) = " + averageSample.get(0).size() + ", size(slice) = " + averageSubsample.get(0).size());

  return averageSubsample;
}

// TODO: Get gesture subsample using offset (the offset doesn't affect the live subsample the data is compared)

//  println("offset = " + offset + ", size = " + size + ", size(sample) = " + averageSample.get(0).size() + ", size(slice) = " + averageSubsample.get(0).size());
//  return averageSample;  
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

  // Create array to store average of accelerometer sample data
  if (gestureSampleAverageSum.size() < 3) {
    gestureSampleAverageSum.add(new ArrayList<Integer>());
    gestureSampleAverageSum.add(new ArrayList<Integer>());
    gestureSampleAverageSum.add(new ArrayList<Integer>());
  }

  // Create array to store the number of accelerometer sample data points (for the division in the computation of the accelerometer point averages)
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
        while (gestureSampleAverageSum.get (axis).size() < singleGestureSample.get(axis).size()) {
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
  boolean axisVisible[] = { 
    showAxisX, showAxisY, showAxisZ
  };
  int axisColor[][] = {
    { 
      255, 0, 0, 20
    }
    , 
    { 
      0, 255, 0, 20
    }
    , 
    { 
      0, 0, 255, 20
    }
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
          stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], axisColor[axis][3]); 
          drawPlot(singleGestureSample.get(axis), 0, height / 2, width, height, 0, 1000);
        }

        // Update list sizes
        while (gestureSampleUpperBounds.get (axis).size() < singleGestureSample.get(axis).size()) {
          gestureSampleUpperBounds.get(axis).add(Integer.MIN_VALUE);
          gestureSampleLowerBounds.get(axis).add(Integer.MAX_VALUE);
          gestureSampleAverageSum.get(axis).add(0);
          gestureSampleAverageCount.get(axis).add(0);
        }

        // println(singleGestureSample.get(axis).size());

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
      stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], 85); 
      drawPlot(gestureSampleUpperBounds.get(axis), 0, height / 2, width, height, 0, 1000);
      stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], 85); 
      drawPlot(gestureSampleLowerBounds.get(axis), 0, height / 2, width, height, 0, 1000);
      stroke(axisColor[axis][0], axisColor[axis][1], axisColor[axis][2], 180); 
      drawPlot(gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
      //drawPlotNodes(12, gestureSampleAverageSum.get(axis), 0, height / 2, width, height, 0, 1000);
    }
  }
}

void drawPlot(ArrayList<Integer> data, int dataOffset, int dataSize, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {

  //  int dataRange = data.size() - 1;
  //  int min = min(data.size() - 1, dataOffset); // Move -1 to the end of the expression?
  //  for (int i = data.size() - 1; i > min; i--) {
  //    line(
  //      map(i - 1, min, dataRange, originX, originX + plotWidth), // x1
  //      map(data.get(i - 1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), // y1
  //      map(i, min, dataRange, originX, originX + plotWidth), // x2
  //      map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight) // y2
  //    );
  //  }

  // Plot data
  int dataRange = min(data.size(), dataSize) - 1;
  for (int i = 0; i < dataRange; i++) {
    line(
    map(i, 0, dataRange, originX, originX + plotWidth), // x1
    map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), // y1
    map(i+1, 0, dataRange, originX, originX + plotWidth), // x2
    map(data.get(i+1), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight) // y2
    );
  }
}

void drawPlot(ArrayList<Integer> data, int originX, int originY, int plotWidth, int plotHeight, int plotRangeFloor, int plotRangeCeiling) {

  // Plot data
  for (int i = 0; i < data.size() - 1; i++) {
    line(
    map(i, 0, data.size() - 1, originX, originX + plotWidth), 
    map(data.get(i), plotRangeFloor, plotRangeCeiling, originY, originY + plotHeight), 
    map(i+1, 0, data.size() - 1, originX, originX + plotWidth), 
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
      fill(255, 255, 255, 0);
      ellipse(
      map(i, 0, maximumSampleSize[gestureIndex], originX, originX + plotWidth), 
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

