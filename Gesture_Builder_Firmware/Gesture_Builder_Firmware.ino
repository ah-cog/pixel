/*
"Gesture Builder" Firmware

This is for use with the assocated "Gesture Builder" Processing sketch.
 */

#include <Wire.h>
#include <SoftwareSerial.h>
#include <SPI.h>

//#include "Gestures.h"

//---

#define GESTURE_COUNT 9
#define AXIS_COUNT 3
#define GESTURE_SIGNATURE_SIZE 50

int gestureIndex = 0;
char* gestureName[GESTURE_COUNT] = { 
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

int gestureCandidate[AXIS_COUNT][GESTURE_SIGNATURE_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
int gestureCandidateSize = 0;

//int gestureSignature1[GESTURE_COUNT][GESTURE_SIGNATURE_SIZE] = {
int gestureSignature[GESTURE_COUNT][AXIS_COUNT][GESTURE_SIGNATURE_SIZE] = {
  //as many vals as dimenstion 1
  {
    // as many as dimension 2
    { -8, -8, -8, -9, -9, -8, -7, -7, -6, -6, -6, -8, -9, -9, -9, -7, -6, -6, -6, -7, -9, -9, -9, -8, -7, -7, -7, -7, -7, -8, -8, -9, -9, -9, -8, -7, -6, -7, -7, -7, -8, -7, -8, -8, -9, -8, -8, -7, -7, -7 },
    { -23, -21, -20, -21, -21, -23, -23, -24, -23, -23, -22, -21, -21, -21, -20, -21, -22, -24, -25, -25, -24, -22, -21, -21, -21, -21, -22, -22, -23, -23, -23, -23, -23, -22, -22, -21, -21, -21, -22, -21, -22, -22, -23, -23, -22, -22, -23, -22, -22, -22 }, //as many rows as dim0
    { 229, 232, 231, 231, 228, 227, 228, 230, 231, 230, 228, 228, 229, 229, 229, 229, 229, 229, 229, 229, 228, 229, 229, 229, 229, 230, 229, 229, 229, 229, 229, 228, 229, 229, 229, 229, 229, 229, 228, 229, 229, 229, 229, 228, 228, 228, 228, 229, 230, 229 }
  }, 
  {
    // as many as dimension 2
    { -4, -5, -5, -5, -5, -5, -6, -7, -7, -8, -7, -7, -7, -7, -7, -7, -5, -6, -5, -6, -5, -5, -5, -6, -5, -5, -6, -6, -6, -6, -7, -7, -8, -8, -9, -8, -8, -8, -7, -6, -5, -5, -5, -4, -5, -5, -4, -5, -5, -6 },
    { -7, -8, -8, -8, -9, -10, -10, -10, -9, -10, -10, -9, -9, -8, -8, -8, -6, -6, -5, -7, -7, -7, -7, -8, -8, -8, -9, -9, -9, -9, -9, -9, -8, -8, -8, -8, -8, -8, -8, -9, -9, -10, -9, -9, -10, -11, -10, -11, -12, -11 }, //as many rows as dim0
    { 228, 228, 229, 229, 229, 230, 229, 229, 229, 230, 230, 229, 229, 229, 229, 229, 229, 228, 228, 228, 227, 228, 228, 228, 228, 229, 229, 228, 229, 229, 229, 229, 229, 229, 229, 228, 228, 229, 229, 230, 230, 230, 229, 229, 229, 229, 229, 229, 229, 229 }
  }, 
  {
    // as many as dimension 2
    { -6, -5, -5, -5, -5, -5, -3, -1, -27, -24, -14, -13, -6, -10, -1, -18, -18, -10, -9, -9, -5, -4, -5, -6, -7, -6, -5, -4, -4, -5, -7, -11, -13, -16, -17, -18, -20, -23, -24, -26, -28, -29, -31, -31, -30, -27, -28, -26, -26, -31 },
    { -24, -24, -24, -24, -24, -23, -21, -20, -43, -35, -37, -39, -34, -39, -37, -35, -28, -25, -20, -17, -17, -16, -17, -15, -12, -9, -3, 0, 4, 7, 9, 10, 11, 13, 14, 15, 16, 17, 18, 17, 17, 16, 16, 16, 17, 19, 19, 21, 22, 17 }, //as many rows as dim0
    { 227, 229, 231, 228, 227, 227, 229, 231, 232, 249, 269, 270, 271, 261, 257, 264, 281, 296, 306, 309, 312, 303, 289, 276, 268, 258, 244, 234, 223, 212, 202, 193, 183, 171, 166, 162, 161, 162, 162, 164, 168, 172, 181, 188, 194, 200, 205, 207, 208, 218 }
  }, 
  {
    // as many as dimension 2
    { -38, -37, -38, -37, -36, -34, -31, -31, -27, -27, -25, -23, -22, -22, -20, -19, -17, -14, -12, -10, -9, -19, -19, -6, -17, -15, -14, -14, -11, -9, -5, -6, -5, -6, -7, -7, -6, -5, -5, -6, -6, -6, -6, -5, -6, -6, -7, -6, -7, -7 },
    { 10, 10, 11, 13, 13, 13, 14, 14, 13, 13, 13, 12, 6, 2, -5, -9, -16, -20, -25, -27, -28, -17, -26, -34, -58, -49, -45, -44, -34, -32, -32, -29, -29, -27, -20, -21, -22, -21, -21, -22, -22, -23, -23, -23, -24, -23, -22, -22, -23, -23 }, //as many rows as dim0
    { 209, 209, 206, 202, 195, 185, 175, 162, 149, 142, 137, 144, 152, 164, 181, 198, 225, 243, 260, 266, 282, 255, 266, 263, 314, 277, 292, 268, 253, 245, 239, 233, 245, 234, 232, 230, 230, 229, 227, 228, 228, 229, 229, 228, 228, 228, 229, 228, 228, 228 }
  }, 
  {
    // as many as dimension 2
    { -29, -29, -29, -29, -29, -29, -29, -28, -28, -28, -28, -28, -28, -27, -27, -27, -27, -28, -28, -28, -29, -30, -30, -30, -30, -31, -31, -32, -33, -34, -35, -36, -37, -37, -38, -39, -39, -39, -39, -39, -39, -40, -40, -39, -39, -39, -39, -39, -39, -39 },
    { -25, -24, -24, -24, -24, -23, -24, -25, -24, -24, -22, -20, -16, -11, -4, 5, 15, 27, 42, 56, 69, 86, 102, 117, 131, 144, 156, 165, 171, 178, 186, 190, 194, 198, 200, 202, 204, 207, 208, 209, 208, 209, 210, 210, 210, 210, 211, 211, 211, 211 }, //as many rows as dim0
    { 230, 229, 229, 228, 228, 228, 227, 227, 226, 225, 225, 223, 223, 223, 222, 222, 221, 220, 217, 211, 206, 198, 193, 187, 180, 172, 164, 157, 152, 146, 140, 136, 132, 128, 125, 122, 120, 118, 116, 114, 113, 111, 111, 113, 113, 113, 112, 111, 110, 110 }
  }, 
  {
    // as many as dimension 2
    { -7, -6, -5, -5, -6, -5, -5, -5, -4, -4, -4, -4, -5, -5, -5, -5, -6, -6, -7, -9, -11, -14, -18, -23, -27, -30, -32, -37, -40, -42, -43, -45, -45, -46, -46, -46, -46, -46, -45, -44, -44, -43, -43, -42, -42, -42, -43, -43, -44, -44 },
    { 21, 19, 19, 19, 19, 18, 19, 19, 19, 20, 20, 22, 21, 21, 22, 17, 11, 1, -11, -28, -43, -55, -72, -96, -116, -135, -147, -168, -187, -198, -205, -212, -221, -227, -230, -229, -231, -233, -235, -237, -239, -239, -237, -238, -238, -237, -235, -234, -233, -233 }, //as many rows as dim0
    { 230, 229, 230, 229, 230, 231, 232, 233, 234, 235, 235, 236, 236, 239, 243, 246, 245, 244, 241, 236, 231, 227, 216, 197, 178, 159, 145, 126, 109, 98, 90, 80, 72, 68, 65, 63, 61, 61, 61, 61, 62, 64, 67, 68, 69, 70, 71, 72, 73, 74 }
  }, 
  {
    // as many as dimension 2
    { 0, 0, 0, 3, 8, 18, 35, 46, 59, 70, 75, 71, 58, 47, 38, 18, 15, 42, 72, 116, 181, 196, 212, 214, 203, 171, 136, 97, 67, 35, 20, 12, 2, 11, 57, 77, 101, 126, 125, 106, 87, 62, 36, 23, 15, 7, 5, 2, 0, -3 },
    { -15, -16, -15, -16, -18, -22, -35, -49, -60, -75, -77, -63, -37, -9, 23, 65, 77, 63, 42, 2, -35, -75, -110, -128, -121, -104, -74, -53, -18, 8, 25, 42, 37, 32, 9, -3, -25, -51, -60, -49, -51, -36, -28, -35, -37, -33, -32, -33, -30, -28 }, //as many rows as dim0
    { 227, 229, 231, 240, 256, 279, 329, 368, 383, 405, 392, 328, 241, 115, -15, -138, -180, -161, -112, 18, 216, 342, 458, 549, 565, 492, 383, 275, 139, 2, -72, -124, -130, -147, -33, 62, 162, 303, 362, 342, 363, 328, 292, 275, 267, 253, 241, 231, 228, 216 }
  }, 
  {
    // as many as dimension 2
    { -6, -5, -5, -5, -5, -6, -7, -8, -8, -9, -9, -9, -10, -10, -10, -11, -9, -7, -4, 1, 4, 10, 10, 6, 12, 18, 27, 32, -2, 16, -1, -57, -47, -27, -8, -29, -45, -17, -15, -24, -15, -11, -7, -22, -21, -10, -8, -5, -5, -6 },
    { -27, -28, -29, -30, -29, -28, -26, -25, -25, -26, -30, -35, -42, -39, -43, -50, -49, -46, -44, -26, -17, -4, 2, 12, 24, 47, 59, 73, 25, 48, 67, 28, 11, 20, 16, 33, 30, 15, -1, -8, 0, 4, 5, 2, 4, 7, 12, 10, 6, 4 }, //as many rows as dim0
    { 229, 230, 231, 230, 229, 227, 226, 225, 225, 227, 230, 232, 234, 234, 235, 241, 240, 238, 236, 234, 233, 230, 226, 226, 222, 225, 221, 223, 195, 211, 207, 174, 210, 214, 216, 214, 215, 235, 230, 226, 231, 236, 227, 196, 194, 209, 218, 222, 227, 227 }
  }, 
  {
    // as many as dimension 2
    { 5, 6, 7, 8, 7, 6, 4, 3, 2, 2, 2, 4, 6, 6, 7, 8, 6, 7, 8, 9, 9, 9, 9, 8, 12, 15, 17, 21, 25, 29, 18, -18, -7, 24, 30, 30, 29, -21, -52, -34, -22, -8, -15, -7, -2, -3, -2, -3, -2, -2 },
    { -38, -37, -35, -34, -32, -31, -31, -33, -36, -37, -41, -46, -49, -51, -43, -36, -24, -18, -16, -15, -15, -22, -31, -32, -43, -58, -88, -120, -141, -175, -155, -136, -164, -154, -133, -114, -108, -53, -27, -24, 10, -7, -1, -4, -26, -39, -58, -58, -60, -61 }, //as many rows as dim0
    { 224, 226, 228, 227, 226, 223, 221, 218, 216, 216, 216, 216, 219, 222, 230, 237, 244, 246, 248, 246, 244, 239, 231, 228, 223, 216, 207, 197, 189, 186, 190, 175, 175, 224, 218, 215, 230, 193, 198, 210, 199, 204, 205, 213, 209, 205, 205, 206, 208, 212 }
  }, 
};

// Gestures (index, title):
//  0   "at rest, on table"
//  1   "at rest, in hand"
//  2   "pick up"
//  3   "place down"
//  4   "tilt left"
//  5   "tilt right"
//  6   "shake"
//  7   "tap to another, as left"
//  8   "tap to another, as right"
int gestureTransitions[GESTURE_COUNT][GESTURE_COUNT] = {
  { 0,  2, -1, -1, -1, -1, -1, -1, -1 },
  { 1,  3,  4,  5,  6,  7,  8, -1, -1 },
  { 2,  1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, -1, -1, -1, -1, -1, -1, -1, -1 },
  { 4,  1, -1, -1, -1, -1, -1, -1, -1 },
  { 5,  1, -1, -1, -1, -1, -1, -1, -1 },
  { 6,  1, -1, -1, -1, -1, -1, -1, -1 },
  { 1, -1, -1, -1, -1, -1, -1, -1, -1 },
  { 1, -1, -1, -1, -1, -1, -1, -1, -1 }
};
int classifiedGestureIndex = 0;

#define MAX_INTEGER_VALUE 32767

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGestureFromTransitions() {
  int minimumDeviationIndex = -1;
  int minimumDeviation = MAX_INTEGER_VALUE;
  
  for (int i = 0; i < GESTURE_COUNT; i++) {
    
      int gestureSignatureIndex = gestureTransitions[classifiedGestureIndex][i]; // Get index of possible gesture
      
      if (gestureSignatureIndex != -1) { // Make sure the transition is valid before continuing
        
        // Calculate the gesture's deviation from the gesture signature
        int gestureDeviation = getGestureDeviation(gestureSignatureIndex);
        int gestureInstability = getGestureInstability(gestureSignatureIndex);

        // Check if the sample's deviation
        if (minimumDeviationIndex == -1 || (gestureDeviation + gestureInstability) < minimumDeviation) {
          minimumDeviationIndex = gestureSignatureIndex;
          minimumDeviation = gestureDeviation + gestureInstability;
        }
    }
  }

  return minimumDeviationIndex;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureDeviation(int classifiedGestureIndex) {
  int deltaTotal = 0;
    
  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    int delta = getGestureAxisDeviation(classifiedGestureIndex, axis); // gestureSignature[classifiedGestureIndex][axis], gestureCandidate[axis]);
    deltaTotal = deltaTotal + delta;
  }
  
  return deltaTotal;
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation(int gestureSignatureIndex, int axis) { // (int gestureSample[GESTURE_SIGNATURE_SIZE], int gestureCandidate[GESTURE_SIGNATURE_SIZE]) {
  
  int delta = 0; // sum of difference between average x curve and most-recent x data
  
//  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
//    if (i < liveSample.size() && i < gestureSample.size()) {
//        int difference = abs(gestureSample.get(i) - liveSample.get(i));
//        delta = delta + difference;
//    }
//  }

  // Compare the first 50 points of the gesture signature to the most recent 50 accelerometer data points
  for (int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
      int difference = abs(gestureSignature[gestureSignatureIndex][axis][point] - gestureCandidate[axis][point]);
      delta = delta + difference;
  }
  
  return delta;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureInstability(int classifiedGestureIndex) {
  int instabilityTotal = 0;
    
  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    int instability = getGestureAxisInstability(classifiedGestureIndex, axis); // gestureSignature[classifiedGestureIndex][axis], gestureCandidate[axis]);
    instabilityTotal = instabilityTotal + instability;
  }
  
  return instabilityTotal;
}

/**
 * Relative instability. How relative is the live sample in comparison to a gesture's signature sample?
 */
int getGestureAxisInstability(int gestureSignatureIndex, int axis) { // (int gestureSample[GESTURE_SIGNATURE_SIZE], int gestureCandidate[GESTURE_SIGNATURE_SIZE]) {
  
  int relativeInstability = 0; // sum of difference between average x curve and most-recent x data
  
//  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
//    if (i < liveSample.size() && i < gestureSample.size()) {
//        int difference = abs(gestureSample.get(i) - liveSample.get(i));
//        delta = delta + difference;
//    }
//  }

  for (int point = 0; point < GESTURE_SIGNATURE_SIZE - 1; point++) {
      int signatureDifference = abs(gestureSignature[gestureSignatureIndex][axis][point + 1] - gestureCandidate[axis][point]);
      int liveDifference = abs(gestureCandidate[axis][point + 1] - gestureCandidate[axis][point]);
      int instabilityDifference = abs(signatureDifference - liveDifference);
      relativeInstability = relativeInstability + instabilityDifference;
  }
  
  return relativeInstability;
}

// Print signature
//for(int axis = 0; axis < AXIS_COUNT; axis++) {
//  for(int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
//    Serial.print(gestureSignature1[axis][point]);
//    Serial.print(" ");
//  }
//  Serial.println();
//}

//---

/**
 * Accelerometer Setup
 */

// Uncomment the below line to use this axis definition: 
// X axis pointing forward
// Y axis pointing to the right 
// and Z axis pointing down.
// Positive pitch : nose up
// Positive roll : right wing down
// Positive yaw : clockwise
//int SENSOR_SIGN[9] = { 1, 1, 1, -1, -1, -1, 1, 1, 1 }; //Correct directions x,y,z - gyro, accelerometer, magnetometer

// Uncomment the below line to use this axis definition: 
// X axis pointing forward
// Y axis pointing to the left 
// and Z axis pointing up.
// Positive pitch : nose down
// Positive roll : right wing down
// Positive yaw : counterclockwise
int SENSOR_SIGN[9] = {1,-1,-1,-1,1,1,1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer
//int SENSOR_SIGN[9] = { 1, -1, -1, 1, -1, 1, 1, -1, -1 }; // Correct directions x,y,z - gyro, accelerometer, magnetometer

// LSM303 accelerometer (8g sensitivity)
// 3.8 mg/digit; 1 g = 256
#define GRAVITY 256  // this equivalent to 1G in the raw data coming from the accelerometer 

#define ToRad(x) ((x) * 0.01745329252)  // *pi/180
#define ToDeg(x) ((x) * 57.2957795131)  // *180/pi

// L3G4200D gyro (2000 dps full scale)
// 70 mdps/digit; 1 dps = 0.07
#define Gyro_Gain_X 0.07 // X axis Gyro gain
#define Gyro_Gain_Y 0.07 // Y axis Gyro gain
#define Gyro_Gain_Z 0.07 // Z axis Gyro gain
#define Gyro_Scaled_X(x) ((x) * ToRad(Gyro_Gain_X)) // Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) ((x) * ToRad(Gyro_Gain_Y)) // Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) ((x) * ToRad(Gyro_Gain_Z)) // Return the scaled ADC raw data of the gyro in radians for second

// LSM303 magnetometer calibration constants; use the Calibrate example from
// the Pololu LSM303 library to find the right values for your board
// TODO: Automate this calibration, make it on-going (?). Store the most-recent values in EEPROM.
#define M_X_MIN -447 // -421
#define M_Y_MIN -619 // -639
#define M_Z_MIN -515 // -238
#define M_X_MAX 595 // 424
#define M_Y_MAX 564 // 295
#define M_Z_MAX 487 // 472
//M min X: -540 Y: -655 Z: -570 M max X: 675 Y: 565 Z: 559

#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

/*For debugging purposes*/
//IMU_OUTPUT_MODE=1 will print the corrected data, 
//IMU_OUTPUT_MODE=0 will print uncorrected data of the gyros (with drift)
#define IMU_OUTPUT_MODE 1

//#define PRINT_DCM 0     // Will print the whole direction cosine matrix
#define PRINT_ANALOGS 1 // Will print the analog raw data
#define PRINT_EULER 1   // Will print the Euler angles Roll, Pitch and Yaw

float G_Dt = 0.02;    // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

long timer = 0;   // General purpuse timer
long timer_old;
long timer24 = 0; // Second timer used to print values 
int AN[6]; // Array that stores the gyro and accelerometer data
int AN_OFFSET[6] = { 
    0, 0, 0, 0, 0, 0 }; // Array that stores the Offset of the sensors

int gyro_x, gyro_y, gyro_z;
int accel_x, accel_y, accel_z;
int magnetom_x, magnetom_y, magnetom_z;
float c_magnetom_x, c_magnetom_y, c_magnetom_z;
float MAG_Heading;

float Accel_Vector[3] = { 0, 0, 0 }; // Store the acceleration in a vector
float Gyro_Vector[3]  = { 0, 0, 0 }; // Store the gyros turn rate in a vector
float Omega_Vector[3] = { 0, 0, 0 }; // Corrected Gyro_Vector data
float Omega_P[3]      = { 0, 0, 0 }; // Omega Proportional correction
float Omega_I[3]      = { 0, 0, 0 }; // Omega Integrator
float Omega[3]        = { 0, 0, 0 };

// Euler angles (http://en.wikipedia.org/wiki/Euler_angles)
float roll, pitch, yaw;

float errorRollPitch[3] = { 0, 0, 0 }; 
float errorYaw[3]       = { 0, 0, 0 };

unsigned int counter = 0;
byte gyro_sat = 0;

float pressure = 0;
float altitude = 0;
float temperature = 0;

// The "Direction Cosine Matrix" (DCM), also known as the "Rotation Matrix"
float DCM_Matrix[3][3] = {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 }
};
float Update_Matrix[3][3] = { // Gyros here
    { 0, 1, 2 },
    { 3, 4, 5 },
    { 6, 7, 8 }
};
float Temporary_Matrix[3][3] = {
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 }
};

char serialDataString[512];

void setup() {

    Serial.begin(9600);

    Serial.println(F("IMU Sensor Data Transducer (Version 2013.12.26.01.01.15)"));

    //
    // Initialize IMU
    //

    Serial.println("Initializing IMU...");
    I2C_Init();

    delay(1500);

    Accel_Init();
    Compass_Init();
    Gyro_Init();
    Alt_Init();

    delay(20); // Wait for a small duration for the IMU sensors to initialize (?)

    for (int i = 0;i < 32; i++) { // We take some initial readings... (to warm the IMU up?)
        Read_Gyro();
        Read_Accel();
        for (int y = 0; y < 6; y++) { // Cumulate values
            AN_OFFSET[y] += AN[y];
        }
        delay(20);
    }

    for (int y = 0; y < 6; y++) {
        AN_OFFSET[y] = AN_OFFSET[y]/32;
    }

    AN_OFFSET[5] -= GRAVITY * SENSOR_SIGN[5];

    Serial.println("Offset:");
    for (int y = 0; y < 6; y++) {
        Serial.println(AN_OFFSET[y]);
    }

    delay(1000);

    timer = millis();
    delay(20);
    counter = 0;
}

unsigned long dataPrintTime = 0UL;

void loop() {
  
//    for(int gesture = 0; gesture < GESTURE_COUNT; gesture++) {  
//      for(int axis = 0; axis < AXIS_COUNT; axis++) {
//        for(int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
//          Serial.print(gestureSignature[gesture][axis][point]);
//          Serial.print(" ");
//        }
//        Serial.println();
//      }
//      Serial.println();
//    }
//    Serial.println();
    
    // TODO: Write code to allow Processing sketch (or other software) to automatically connect to this serial port. Send a "waiting for connection" signal.
    
    // Sense phsyical orientation data
    if (sensePhysicalData()) {
        printData();
        //storeData();
        
//        for (int axis = 0; axis < AXIS_COUNT; axis++) {
//          for (int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
//            Serial.print(gestureCandidate[axis][point]); // Shift point left by one
//            Serial.print(' ');
//          }
//          Serial.println();
//        }
//        Serial.println();
    }
    
    
    // Classify live gesture sample
//    if (gestureCandidate.get(0).size() >= liveGestureSize) {
    // classifiedGestureIndex = classifyGesture(liveGestureSample, liveGestureSize);
//    classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
//    Serial.print("Classified gesture: ");
//    Serial.print(gestureName[classifiedGestureIndex]);
//    Serial.println();
//    }
}

boolean sensePhysicalData() {

    if ((millis() - timer) >= 20) { // Main loop runs at 50Hz

        counter++;
        timer_old = timer;
        timer = millis();
        if (timer > timer_old) {
            G_Dt = (timer-timer_old) / 1000.0; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
        } 
        else {
            G_Dt = 0;
        }

        // > start of DCM algorithm

        // Data adquisition
        Read_Gyro(); // This read gyro data
        Read_Accel(); // Read I2C accelerometer

        if (counter > 5) { // Read compass data at 10 Hz... (5 loop runs)
            counter = 0;
            Read_Compass(); // Read I2C magnetometer
            Compass_Heading(); // Calculate magnetic heading
        }

        // Read pressure/altimeter
        Read_Altimeter();

        // Calculations...
        Matrix_update(); 
        Normalize();
        Drift_correction();
        Euler_angles();
        // ^ end of DCM algorithm
        
        return true;
    } else {
        return false;
    }
}

void storeData() {
  
  // Shift points left by one
  if (gestureCandidateSize >= (50 - 1)) {
    for (int axis = 0; axis < AXIS_COUNT; axis++) {
      for (int point = 0; point < GESTURE_SIGNATURE_SIZE - 1; point++) {
        gestureCandidate[axis][point] = gestureCandidate[axis][point + 1]; // Shift point left by one
      }
    }
  }
  
  // Push latest accelerometer data point onto the end of the array
  gestureCandidate[0][gestureCandidateSize] = AN[3]; // accelerometer x
  gestureCandidate[1][gestureCandidateSize] = AN[4]; // accelerometer y
  gestureCandidate[2][gestureCandidateSize] = AN[5]; // accelerometer z
  
  // Increment gesture candidate size (if less than 50)
  if (gestureCandidateSize < (50 - 1)) {
    // Increment gesture candidate size
    gestureCandidateSize = gestureCandidateSize + 1;
  }
}

void printData (void) {
    
    // NOTE: There's sprintf documentation at http://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm along with some examples showing its usage.
    sprintf(serialDataString, "! %f\t%f\t%f\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\n", roll, pitch, yaw, AN[0], AN[1], AN[2], AN[3], AN[4], AN[5], c_magnetom_x, c_magnetom_y, c_magnetom_z, pressure, altitude, temperature);
    Serial.print(serialDataString);
}
