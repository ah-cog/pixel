import processing.serial.*;
import controlP5.*;

// ControlP5:
ControlP5 controlP5;
boolean showGUI = false;
Slider[] sliders;
Range[] ranges;
Toggle[] toggles;

// Gesture Builder:

// Serial port
int serialPortIndex = 7; // NOTE: This must be set correctly manually, here in the code!
Serial serialPort;
String serialInputString; // Used to buffer a received string

// Keyboard state
boolean shiftPressed = false;

// Intertial measurement unit (IMU) data
int dataTimestamp = 0;
float roll = 0, minRoll = 0, maxRoll = 0, avgRoll = 0;
float pitch = 0, minPitch = 0, maxPitch = 0, avgPitch = 0;
float yaw = 0, minYaw = 0, maxYaw = 0, avgYaw = 0;
int gyroX, gyroY, gyroZ;
int accelerometerX, accelerometerY, accelerometerZ;
int magnetometerX, magnetometerY, magnetometerZ;
float pressure, altitude, temperature;

int dimensionCount = 3; // The number of dimensions to model and classify along.

PFont gestureFont, classifiedGestureFont;
PFont boundaryLabelFont;

boolean showGesturePrompt = true;

// Used for building data sets
JSONArray gestureDataSample;
JSONArray gestureSampleSet;
boolean isRecordingGesture = false;
int gestureSelectionTime = 0;
int gestureSensorSampleCount = 0;

int gestureIndex = 0;
String gestureName[] = { 
  "at rest",
 // "at rest, upside down", 
  "swing", 
  "tap to another, as left", 
  "tap to another, as right", 
  "shake",
  "tilt left", 
  "tilt right",
  "tilt forward",
  "tilt backward"
};
int gestureSampleCount = 0;

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
int defaultLiveGestureSize = 35;
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
boolean showAxisGyroX = true, showAxisGyroY = true, showAxisGyroZ = true;
boolean showGestureCandidate = true;
boolean showOrientationVisualization = true;
boolean showValidationStatistics = false;

int classifiedGestureIndex = -1;
color backgroundColor = #F0F1F0;

ArrayList<ArrayList<ArrayList<ArrayList<Integer>>>> cachedGestureSamples;
ArrayList<Boolean> hasCachedGestureSamples;

void setup () {
  size(displayWidth, displayHeight, P3D); // size(1200, 800, P3D);
  frame.setTitle("Gesture Builder");
  
  // Setup menu
  setupMenu();

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
  
  liveGestureSample.add(new ArrayList<Integer>()); // X gyro data
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
  serialPort = new Serial(this, Serial.list()[serialPortIndex], 9600);

  // Defer callback until new line
  serialPort.bufferUntil('\n');
}

void draw() {
  
  // Check if we're recording data (store it, if so)
  if (isRecordingGesture) {
    storeGestureData();
  }

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
  
  if (showGestureCandidate) {
    drawLiveGesturePlot();
    
    // Visualize the module's orientation
    if (showOrientationVisualization) {
      drawOrientationVisualization();
    }
    
    // Show validation statistics
    if (showValidationStatistics) {
      drawValidationStatistics();
    }
  }

  //  drawGestureSignatureBoundaries();

  // Draw gesture label and classified gesture lable
  drawGestureTitle();
  //drawGesturePlot();
  //drawExampleGestureTitle();
}

boolean sketchFullScreen() {
  return isFullScreen;
}

/**
 * Receives and processes incoming serial port data.   
 */
void serialEvent (Serial serialPort) {

  // Read serial data
  if (serialPort.available() > 0) { // Check if data is available on the serial port
    serialInputString = serialPort.readString(); // Read the data (a string) and buffer it
    // print(serialInputString); // Print the buffered data received over the serial port

    if (serialInputString != null && serialInputString.length() > 0) {

      // Check if the string begins with a '!' (i.e., check if it's a data string)
      if (serialInputString.charAt(0) == '!') {

        serialInputString = serialInputString.substring (1);

        String[] serialInputArray = split(serialInputString, '\t');

        // Check if array is correct size
        if (serialInputArray.length >= 3) {
          dataTimestamp = millis();

          // Parse the data received over the serial port
          roll           = float(serialInputArray[0]);
          pitch          = float(serialInputArray[1]);
          yaw            = float(serialInputArray[2]);
          gyroX          = int(serialInputArray[3]);
          gyroY          = int(serialInputArray[4]);
          gyroZ          = int(serialInputArray[5]);
          accelerometerX = int(serialInputArray[6]);
          accelerometerY = int(serialInputArray[7]);
          accelerometerZ = int(serialInputArray[8]);
          magnetometerX  = int(serialInputArray[9]);
          magnetometerY  = int(serialInputArray[10]);
          magnetometerZ  = int(serialInputArray[11]);
          pressure       = float(serialInputArray[12]);
          altitude       = float(serialInputArray[13]);
          temperature    = float(serialInputArray[14]);

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
          
          if (dimensionCount > 3) {
            liveGestureSample.get(3).add(gyroX);
            liveGestureSample.get(4).add(gyroY);
            liveGestureSample.get(5).add(gyroZ);
          }

          // Remove oldest element from live gesture queue if greater than threshold
          if (liveGestureSample.get(0).size() > liveGestureSize) { // TODO: In classifier function, compare only the "latest" values...
            liveGestureSample.get(0).remove(0);
            liveGestureSample.get(1).remove(0);
            liveGestureSample.get(2).remove(0);
            
            if (dimensionCount > 3) {
              liveGestureSample.get(3).remove(0);
              liveGestureSample.get(4).remove(0);
              liveGestureSample.get(5).remove(0);
            }
          }

          // Classify live gesture sample
          if (liveGestureSample.get(0).size() >= liveGestureSize) {
            classifiedGestureIndex = classifyGestureFromTransitions(liveGestureSample);
          }
        }
      }
    }
  }
}

int GESTURE_MODE = 0;
int GESTURE_ENVELOPE_MODE = 1;
int keyMode = GESTURE_MODE;
int keyModeCount = 2;

/**
 * Handles key press (down) events.
 */
void keyPressed() {
  if (key == CODED) {
    if (keyCode == SHIFT) {
      shiftPressed = true;
    }

    // Change position of left boundary
    if (keyCode == LEFT) {
      
      if (keyMode == GESTURE_MODE) {
        
        if (gestureIndex == 0) {
          gestureIndex = (gestureName.length - 1);
        } else {
          gestureIndex = (gestureIndex - 1) % gestureName.length;
        }
    
        gestureSelectionTime = millis();
    
        updateCurrentGesture();
        
      } else if (keyMode == GESTURE_ENVELOPE_MODE) {
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
    }

    // Change position of right boundary
    if (keyCode == RIGHT) {
      
      if (keyMode == GESTURE_MODE) {
        gestureIndex = (gestureIndex + 1) % gestureName.length;
    
        gestureSelectionTime = millis();
    
        updateCurrentGesture();
      } else if (keyMode == GESTURE_ENVELOPE_MODE) {
        if (shiftPressed) {
          // TODO: ?
        } else {
          // Increase the offset
          if ((gestureSignatureOffset[gestureIndex] + gestureSignatureSize[gestureIndex]) < maximumSampleSize[gestureIndex]) {
            gestureSignatureOffset[gestureIndex] = gestureSignatureOffset[gestureIndex] + 1;
            printGestureSignatures();
          }
        }
      }
    }

    if (keyCode == UP) {
      if (keyMode == GESTURE_MODE) {
      } else if (keyMode == GESTURE_ENVELOPE_MODE) {
        if (shiftPressed) {
          // TODO: ?
        } else {
          // Increase the offset
          if (gestureSignatureSize[gestureIndex] < liveGestureSize) {
            gestureSignatureSize[gestureIndex] = gestureSignatureSize[gestureIndex] + 1;
          }
          printGestureSignatures();
        }
      }
    }

    if (keyCode == DOWN) {
      if (keyMode == GESTURE_MODE) {
      } else if (keyMode == GESTURE_ENVELOPE_MODE) {
        if (shiftPressed) {
          // TODO: ?
        } else {
          gestureSignatureSize[gestureIndex] = gestureSignatureSize[gestureIndex] - 1;
          printGestureSignatures();
        }
      }
    }
  }

  if (key == ' ') {
    recordGestureExample();
  } else if (key == ESC) {
    exit(); // Stops the program
  } else if (key == 'x' || key == 'X') {
    showAxisX = !showAxisX;
  } else if (key == 'y' || key == 'Y') {
    showAxisY = !showAxisY;
  } else if (key == 'z' || key == 'Z') {
    showAxisZ = !showAxisZ;
  } else if (key == 'o' || key == 'O') {
    showOrientationVisualization = !showOrientationVisualization;
  } else if (key == 'c' || key == 'C') {
    showGestureCandidate = !showGestureCandidate;
  } else if (key == 'v' || key == 'V') {
    showValidationStatistics = !showValidationStatistics;
  } else if (key == 's' || key == 'S') {
    saveGestureModelFile();
  } else if (key == TAB) {
    keyMode = (keyMode + 1) % keyModeCount;
  }
}

/**
 * Handle key release (up) events.
 */
void keyReleased() {
  if (key == CODED) {
    if (keyCode == SHIFT) {
      shiftPressed = false;
    }
  }
  
  if (key == 'm' || key == 'M') {
    showGUI = controlP5.group("menu").isOpen();
    showGUI = !showGUI;
  }
  if (showGUI) { controlP5.group("menu").open(); }
  else { controlP5.group("menu").close(); }
}
