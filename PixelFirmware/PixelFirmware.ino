/*
"Pixel" Firmware
Rendition 2
Revised February 20, 2013 at Collider.

This code is based on or extends open source code. Some notes from these open source
projects is shown below.

 MinIMU-9-Arduino-AHRS
 Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)
 
 Copyright (c) 2011 Pololu Corporation.
 http://www.pololu.com/
 
 MinIMU-9-Arduino-AHRS is based on sf9domahrs by Doug Weibel and Jose Julio:
 http://code.google.com/p/sf9domahrs/
 
 sf9domahrs is based on ArduIMU v1.5 by Jordi Munoz and William Premerlani, Jose
 Julio and Doug Weibel:
 http://code.google.com/p/ardu-imu/
 
 MinIMU-9-Arduino-AHRS is free software: you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by the
 Free Software Foundation, either version 3 of the License, or (at your option)
 any later version.
 
 MinIMU-9-Arduino-AHRS is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser rGeneral Public License for
 more details.
 
 You should have received a copy of the GNU Lesser General Public License along
 with MinIMU-9-Arduino-AHRS. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>
#include <SPI.h>

#include "Light.h"
#include "Gestures.h"

#define DEVICE_ADDRESS   0x0001
#define NEIGHBOR_ADDRESS 0x0002

// These #define's are copied from the RadioBlock.cpp file
#define TYPE_UINT8 	1
#define TYPE_INT8	2
#define	TYPE_UINT16	3
#define TYPE_INT16	4
#define TYPE_UINT32	5
#define TYPE_INT32	6
#define TYPE_UINT64	7
#define TYPE_INT64	8
#define TYPE_FLOAT	9
#define TYPE_FIXED8_8	10
#define TYPE_FIXED16_8	11
#define TYPE_8BYTES	12
#define TYPE_16BYTES	13
#define TYPE_ASCII	14

// Mesh message queue
#define MESH_QUEUE_CAPACITY 20
unsigned short int meshMessageQueue[MESH_QUEUE_CAPACITY] = { 0 };
int meshMessageQueueSize = 0;

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
//int SENSOR_SIGN[9] = {1,-1,-1,-1,1,1,1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer
int SENSOR_SIGN[9] = { 1, -1, -1, 1, -1, 1, 1, -1, -1 }; // Correct directions x,y,z - gyro, accelerometer, magnetometer

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

/**
 * RadioBlocks Setup
 */

#define RADIOBLOCK_PACKET_READ_TIMEOUT 40 // 100
#define PAYLOAD_START_INDEX 5 // Index of the first byte in the payload
#define RADIOBLOCK_PACKET_WRITE_TIMEOUT 120 // 200

// The module's pins 1, 2, 3, and 4 are connected to Arduino's pins 5, 4, 3, and 2.
//RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 7, 8);
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 8, 7);

/**
 * Device Setup
 */

boolean hasCounter = false;
unsigned long lastCount = 0;
#define NEIGHBOR_COUNT 2
unsigned short int neighbors[NEIGHBOR_COUNT];
unsigned short int next[1];

//            _               
//           | |              
//   ___  ___| |_ _   _ _ __  
//  / __|/ _ \ __| | | | '_ \ 
//  \__ \  __/ |_| |_| | |_) |
//  |___/\___|\__|\__,_| .__/ 
//                     | |    
//                     |_|    

void setup() {

  //
  // Setup RadioBlocks
  //
  
  delay(1000);
  
  interface.begin();  
  
  //Give RadioBlock time to init
  delay(500);
  
  //Tell the world we are alive  
  interface.setLED(true);
  delay(1000);
  interface.setLED(false);
  
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  interface.setAddress(DEVICE_ADDRESS);
  
  //
  // Setup serial communication (for debugging)
  //
  
    Serial.begin(9600);

    Serial.println(F("IMU Sensor Data Transducer (Version 2014.03.10.18.03.30)"));

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
    
    // Flash RGB LEDs
    ledOn();
    delay(100);
    ledOff();
    delay(100);
    ledOn();
    delay(100);
    ledOff();
    delay(100);
    ledOn();
    delay(100);
    ledOff();
}

//   _                   
//  | |                  
//  | | ___   ___  _ __  
//  | |/ _ \ / _ \| '_ \ 
//  | | (_) | (_) | |_) |
//  |_|\___/ \___/| .__/ 
//                | |    
//                |_|    

boolean hasGestureProcessed = false;

void loop() {
  
  // Get data from mesh network
  boolean hasReceivedMeshData = false;
  hasReceivedMeshData = receiveMeshData();
  
//  for(int gesture = 0; gesture < GESTURE_COUNT; gesture++) {  
//    for(int axis = 0; axis < AXIS_COUNT; axis++) {
//      for(int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
//        Serial.print(gestureSignature[gesture][axis][point]);
//        Serial.print(" ");
//      }
//      Serial.println();
//    }
//    Serial.println();
//  }
//  Serial.println();
  
  // TODO: Write code to allow Processing sketch (or other software) to automatically connect to this serial port. Send a "waiting for connection" signal.
  
  // Sense phsyical orientation data
  boolean hasGestureChanged = false;
  if (sensePhysicalData()) {
    // printData(); // Note: Print data if using in conjunction with Processing
    storeData();
    
    // Classify live gesture sample
    unsigned long currentTime = millis();
    if (currentTime - lastGestureClassificationTime >= gestureSustainDuration[classifiedGestureIndex]) { // Check if gesture duration has expired
      classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
      
      lastGestureClassificationTime = millis(); // Update time of most recent gesture classification
    }
    
    // Update current gesture (if it has changed)
    if (classifiedGestureIndex != previousClassifiedGestureIndex) {
      Serial.print("Classified gesture: ");
      Serial.print(gestureName[classifiedGestureIndex]);
      Serial.println();
      
      // Update the previous gesture to the current gesture
      previousClassifiedGestureIndex = classifiedGestureIndex;
      
      // Indicate that the gesture has changed
      hasGestureChanged = true;
      hasGestureProcessed = false;
      
      // TODO: Process newly classified gesture
      // TODO: Make sure the transition can happen (with respect to timing, "transition cooldown")
    }
  }
  
//  Serial.print("hasGestureChanged = ");
//  Serial.print(hasGestureChanged);
//  Serial.println();
  
  // Process current gesture (if it hasn't been processed yet)
  if (hasGestureChanged) { // Only executed when the gesture has changed
    if (!hasGestureProcessed) { // Only executed when the gesture hasn't yet been processed
      // TODO: Write code to process the gesture!
      
      ledOff();
      
      // Handle gesture
      // TODO: Consider moving this... is this the right place? Should I care what the gesture is at this point or just send messages? Processing should be done by this point, right?
      if (classifiedGestureIndex == 0) { // Check if gesture is "at rest, on table"
        handleGestureAtRestOnTable();
      } else if (classifiedGestureIndex == 1) { // Check if gesture is "at rest, in hand"
        handleGestureAtRestInHand();
      } else if (classifiedGestureIndex == 2) { // Check if gesture is "pick up"
        handleGesturePickUp();
      } else if (classifiedGestureIndex == 3) { // Check if gesture is "place down"
        handleGesturePlaceDown();
      } else if (classifiedGestureIndex == 4) { // Check if gesture is "tilt left"
        handleGestureTiltLeft();
      } else if (classifiedGestureIndex == 5) { // Check if gesture is "tilt right"
        handleGestureTiltRight();
      } else if (classifiedGestureIndex == 6) { // Check if gesture is "shake"
        handleGestureShake();
      } else if (classifiedGestureIndex == 7) { // Check if gesture is "tap to another, as left"
        handleGestureTapToAnotherAsLeft();    
      } else if (classifiedGestureIndex == 8) { // Check if gesture is "tap to another, as right"
        handleGestureTapToAnotherAsRight();
      }
      
      hasGestureProcessed = true; // Set flag indicating gesture has been processed
    }
    // Serial.println("Gesture has changed");
    
    // TODO: Process newly classified gesture
  }
  
  // TODO: Handle "ongoing" gesture (i.e., do the stuff that should be done more than once, or as long as the gesture is active)
  
  //
  // Send message with updated gesture
  //
  
  // Process mesh message queue  
  if (meshMessageQueueSize > 0) {
    sendMeshMessage();
  }
    
  unsigned long currentTime = millis();
  //if (currentTime - lastCount > random(RADIOBLOCK_PACKET_WRITE_TIMEOUT, 1000)) {
  if (currentTime - lastCount > RADIOBLOCK_PACKET_WRITE_TIMEOUT) {
  
    // Process mesh message queue  
//    if (meshMessageQueueSize > 0) {
//      sendMeshMessage();
//    }
    
    // Process outgoing mesh network messages
    
    // TODO: Check if there are any queued messages and if so, send them, bit by bit!
    
    // Serial.println("Tick.");
//    ledOff();
//    
//    // Handle gesture
//    // TODO: Consider moving this... is this the right place? Should I care what the gesture is at this point or just send messages? Processing should be done by this point, right?
//    if (classifiedGestureIndex == 0) { // Check if gesture is "at rest, on table"
//      handleGestureAtRestOnTable();
//    } else if (classifiedGestureIndex == 1) { // Check if gesture is "at rest, in hand"
//      handleGestureAtRestInHand();
//    } else if (classifiedGestureIndex == 2) { // Check if gesture is "pick up"
//      handleGesturePickUp();
//    } else if (classifiedGestureIndex == 3) { // Check if gesture is "place down"
//      handleGesturePlaceDown();
//    } else if (classifiedGestureIndex == 4) { // Check if gesture is "tilt left"
//      handleGestureTiltLeft();
//    } else if (classifiedGestureIndex == 5) { // Check if gesture is "tilt right"
//      handleGestureTiltRight();
//    } else if (classifiedGestureIndex == 6) { // Check if gesture is "shake"
//      handleGestureShake();
//    } else if (classifiedGestureIndex == 7) { // Check if gesture is "tap to another, as left"
//      handleGestureTapToAnotherAsLeft();    
//    } else if (classifiedGestureIndex == 8) { // Check if gesture is "tap to another, as right"
//      handleGestureTapToAnotherAsRight();
//    }
    
    // Update the time that a message was most-recently dispatched
    lastCount = millis();
  }
}

/**
 * Handle "at rest, on table" gesture.
 */
boolean handleGestureAtRestOnTable() {
  ledOff();
  queueMeshMessage(1);
}

/**
 * Handle "at rest, in hand" gesture.
 */
boolean handleGestureAtRestInHand() {
  ledOn();
  queueMeshMessage(2);
}

/**
 * Handle "pick up" gesture.
 */
boolean handleGesturePickUp() {
  // TODO:
  queueMeshMessage(3);
}

/**
 * Handle "place down" gesture.
 */
boolean handleGesturePlaceDown() {
  // TODO:
  queueMeshMessage(4);
}

/**
 * Handle "tilt left" gesture.
 */
boolean handleGestureTiltLeft() {
  queueMeshMessage(5);
  
  delay(5);
  ledOn();
  delay(5);
  ledOff();
}

/**
 * Handle "tilt right" gesture.
 */
boolean handleGestureTiltRight() {
  queueMeshMessage(6);
  
  delay(20);
  ledOn();
  delay(20);
  ledOff();
}

/**
 * Handle "shake" gesture.
 */
boolean handleGestureShake() {
  // TODO:
  queueMeshMessage(7);
}

/**
 * Handle "tap to another, as left" gesture.
 */
boolean handleGestureTapToAnotherAsLeft() {
  // TODO:
  queueMeshMessage(8);
}

/**
 * Handle "tap to another, as right" gesture.
 */
boolean handleGestureTapToAnotherAsRight() {
  // Send to all linked devices
//      for (int i = 0; i < 1; i++) {
//          // Set the destination address
//          interface.setupMessage(next[i]);
//  
//          // Package the data payload for transmission
//          interface.addData(1, (byte) 0x1F); // TYPE_INT8
//          interface.sendMessage(); // Send data OTA
//  
//          // Wait for confirmation
//          // delayUntilConfirmation();
//      }

  queueMeshMessage(9);
//  sendMeshMessage(8);
}

// Push a message onto the queue of messages to be processed and sent via the mesh network.
boolean queueMeshMessage(int message) {
  // TODO: Add message to queue... and use sendMeshMessage to send the messages...
  
  if (meshMessageQueueSize < MESH_QUEUE_CAPACITY) {
    // Add message to queue
    meshMessageQueue[meshMessageQueueSize] = message; // Add message to the back of the queue
    meshMessageQueueSize++; // Increment the message count
  }
  
  Serial.print("queueing message (size: ");
  Serial.print(meshMessageQueueSize);
  Serial.print(")\n");
}

boolean sendMeshMessage() {
  if (meshMessageQueueSize > 0) {
    
    // Get the next message
    unsigned short int message = meshMessageQueue[0]; // Get message on front of queue
    meshMessageQueueSize--;
    
    // Shift messages in queue
    for (int i = 0; i < MESH_QUEUE_CAPACITY - 1; i++) {
      meshMessageQueue[i] = meshMessageQueue[i + 1];
    }
    meshMessageQueue[MESH_QUEUE_CAPACITY - 1] = 0; // Set last message to "noop"
    
//    Serial.print("dequeueing message: ");
//    Serial.print(message);
//    Serial.print (" (size: ");
//    Serial.print(meshMessageQueueSize);
//    Serial.print(")\n");


//  interface.setLED(true);
//  delay(200);
//  interface.setLED(false);
//  delay(200);
//  interface.setLED(true);
//  delay(200);
//  interface.setLED(false);
//  delay(200);
//  interface.setLED(true);
//  delay(200);
//  interface.setLED(false);
//  delay(200);
    
    // Actually send the message
    interface.setupMessage(NEIGHBOR_ADDRESS);
            
    // Package the data payload for transmission
    //interface.addData(1, (unsigned char) 0x13); // TYPE_UINT8
    
//    Serial.println("sending message: " + message);
    
//    interface.addData(1, (unsigned char) classifiedGestureIndex); // TYPE_UINT8
    interface.addData(1, (unsigned char) message); // TYPE_UINT8
    
    //  interface.addData(1, (char) 0x14); // TYPE_INT8
    
    //      interface.addData(3, (unsigned short int) 0xFFFD); // TYPE_UINT16
    //      interface.addData(1, (short) 0xABCD); // TYPE_INT16
    //      interface.addData(14, (unsigned long) 0xDDDDCCAA); // TYPE_UINT32
    //      interface.addData(9, (long) 0xFF03CCAA); // TYPE_INT32
      
      //  //Send state of pot (potentimeter, not drug manufacturing)
      //  interface.addData(CODE_VALVE, analogRead(1));
      //  
      //  //Toggle other other guys LED on RadioBlock
      //  interface.addData(CODE_LED, 1);
     
    //Send data OTA
    interface.sendMessage();
  }
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



/**
 * Read received (and buffered) data from the RadioBlock.
 */
boolean receiveMeshData() {

  // if (interface.readPacket(RADIOBLOCK_PACKET_READ_TIMEOUT)) { // Waits a maximum of <i>timeout</i> milliseconds for a response packet before timing out; returns true if packet is read. Returns false if timeout or error occurs.
  interface.readPacket(); // Read the packet (NOTE: Seemingly must do this for isAvailable() to work properly.)
  if (interface.getResponse().isAvailable()) {
    // TODO: Change this to interface.isAvailable() so it doesn't block and wait for any time, so it just reads "when available" (in quotes because it's doing polling)
    
    // General command format (sizes are in bytes), Page 4:
    //
    // [ Start Byte (1) | Size (1) | Payload (Variable) | CRC (2) ]
    //
    // - Start Byte (1 byte)
    // - Size (1 byte): Size of the command id + options + payload field
    // - Payload (Variable): command payload
    // - CRC (2 bytes): 16 bit CRC calculated over payload with initial value 0x1234 (i.e., it is 
    //                  calculated over the command id, options, and payload if any.)
    
    // Data request command format (this is the Payload field in the command format):
    // 
    // [ Command ID (1) | Destination (2) | Options (1) | Handle (1) ]
    //
    // NOTE: I believe this constitutes a "frame".
    
    // Data response command format (this is the Payload field in the command format):
    // 
    // [ Command ID (1) | Source Address (2) | Options (1) | LQI (1) | RSSI (1) | Payload (Variable) ]
    //
    // NOTE: I believe this constitutes a "frame".
  
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      Serial.println("\n\n\n\nReceived packet.");
      
      int commandId = interface.getResponse().getCommandId();
      
      if (commandId == APP_COMMAND_ACK) {
        // "Every command is confirmed with an acknowledgment command even if it is impossible
        //   to immediately execute the command. There is no particular order in which responses 
        //   are sent, so for example Data Indication Command might be sent before 
        //   Acknowledgment Command."
  
        // Acknowledgment command format: Page 5 of SimpleMesh Serial Protocol document.
  
        Serial.println("Received Ack");
  
        Serial.print("  Status: ");
        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Source address

      } else if (commandId == APP_COMMAND_DATA_IND) { // (i.e., 0x22) [Page 15]
      
        ledToggle();
  
        Serial.print(interface.getResponse().getFrameData()[0], HEX); // Frame options
        Serial.print(" | ");
        Serial.print(interface.getResponse().getFrameData()[1], HEX); // Frame options
        Serial.print(" | ");
        Serial.print(interface.getResponse().getFrameData()[2], HEX); // Frame options
        Serial.print(" | ");
        Serial.print(interface.getResponse().getFrameData()[3], HEX); // Frame options
        Serial.print(" | ");
        Serial.print(interface.getResponse().getFrameData()[4], HEX); // Frame options

        int frameDataLength = interface.getResponse().getFrameDataLength();
        //        Serial.print("  Payload length: ");
        //        Serial.println(frameDataLength, DEC); // "Payload" length (minus "Command Id" (1 byte))

        //        Serial.print("  Frame Command Id: ");
        //        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options

        // Compute source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
        short sourceAddress = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
        Serial.print("  Source address: ");
        Serial.println(sourceAddress, HEX); // Source address

        // Frame Options [Page 15]
        // i.e.,
        // 0x00 None
        // 0x01 Acknowledgment was requested
        // 0x02 Security was used
        //        Serial.print("  Frame options: ");
        //        Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
        //  
        //        Serial.print("  Link Quality Indicator: ");
        //        Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
        //  
        //        Serial.print("  Received Signal Strength Indicator: ");
        //        Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator

        // NOTE: Payload starts at index position 5

        //
        // Parse payload data
        //

        //        int PAYLOAD_START_INDEX = 5; // Index of first byte of "Payload"

        //        Serial.print("Raw Payload Data: [ ");
        //        for (int i = 0; i < (frameDataLength - PAYLOAD_START_INDEX - 1); i++) {
        //          Serial.print(interface.getResponse().getFrameData()[5 + i] , HEX);
        //          Serial.print(" : ");
        //        }
        //        Serial.print(interface.getResponse().getFrameData()[5 + (frameDataLength - PAYLOAD_START_INDEX - 1)], HEX);
        //        Serial.println(" ]");
        //        
        //        Serial.println("Parsed Payload Data:");

        // Get the method the sending unit used to construct the packet
        int sendMethod = -1;
        if (frameDataLength == 6) {
            sendMethod = 0; // The sender used sendData(). More data is expected to come along...
        } else if (frameDataLength > 6) {
            sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
        }

        // Parse payload data based on whether sendData() or the three functions setupMessage(), 
        // addData(), and sendMessage() were used.

        if (sendMethod != -1) { // Check if sendMethod is valid... if < 6, no data was attached...

            // "sendData()" was used, so only one byte of data was sent (since this function sends only one byte).
            // Therefore, extract the one byte of data from the first byte of the "Payload". [Page 15]
            if (sendMethod == 0) {

              Serial.print("  Sent Data: ");
              Serial.println(interface.getResponse().getFrameData()[5], HEX);

            } 
            else if (sendMethod == 1) {

                // The "Payload" field is packed in pairs, each pair consisting of a 1 byte code followed by a 
                // variable number of bytes of data, determinable by the 1 byte code.

                unsigned int codeAndType = 0;
                unsigned int payloadCode = 0;
                unsigned int payloadDataType = 0;

                int payloadOffset = 0;

                int loopCount = 0;
                int maxLoopCount = 20;

                while(payloadOffset < (frameDataLength - PAYLOAD_START_INDEX)) {

                    // Protect against infinite loop with this conditional
//                              if (loopCount > maxLoopCount) {
//                                  Serial.println("WARNING: loopCount > maxLoopCount");
//                                  interface.getResponse().reset(); // TODO: Possibly remove this. This might be a bad idea.
//                                  clearCounter();
//                                  return false;
//                                  break;
//                              }

                    codeAndType = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset];
                    //              Serial.print(" Encoded send code and original data type: ");
                    //              Serial.println(codeAndType, HEX); // The actual data

                    payloadDataType = codeAndType & 0xF;
                    payloadCode = (codeAndType >> 4) & 0xF;
                    //              Serial.print("  The sent code was (in hex): ");
                    //              Serial.println(payloadCode, HEX);
                    //              Serial.print("  The original data type was: ");
                    //              Serial.println(payloadDataType);

                    //
                    // Extract and type cast the data based on data type
                    //

                    if (payloadDataType == TYPE_UINT8) {

                        Serial.println("   Data type is TYPE_UINT8. High and low bytes:");
                        Serial.print("    High part: ");
                        Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);

                        // Append message to message queue
//                        messageQueue = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1];

                        payloadOffset = payloadOffset + sizeof(unsigned char) + 1;

                    } else if (payloadDataType == TYPE_INT8) {

                        Serial.println("   Data type is TYPE_INT8. High and low bytes:");
                        Serial.print("    High part: ");
                        Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);

                        payloadOffset = payloadOffset + sizeof(char) + 1;

                    }

                    /* 
                     else if (payloadDataType == TYPE_UINT16) {
                     
                     Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
                     Serial.print("    High part: ");
                     Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]); 
                     Serial.print("    Low part: ");
                     Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 2]);
                     
                     short unsigned int data = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1] << 8 | ((unsigned short int) interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 2]);
                     Serial.print("    Value: ");
                     Serial.println(data);
                     }
                     */

                    loopCount++;

                } // while()

                //
                // Process received data, update state
                //

                // Turn on if received counter
                //            setCounter();

            }
        } 
        
        // Return true if a packet was read (i.e., received) successfully
        // TODO: Add "return true" and "return false" cases to different conditions above, in this block.
        // return true;
        
        } else if (commandId == APP_COMMAND_DATA_CONF) {
        
          Serial.println("APP_COMMAND_DATA_CONF");
          
          // return false; // Return true if a packet was read (i.e., received) successfully
        
        } else if (commandId == APP_COMMAND_GET_ADDR_RESP) { // (i.e., 0x25) [Page 15]
        
          Serial.print("  Frame Command Id: ");
          Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
  
          // Computer source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
//          address = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
//          Serial.print("  Device address: ");
//          Serial.println(address, HEX); // Source address
//
//          hasValidAddress = true;
  
          return false; // Return true if a packet was read (i.e., received) successfully
        }
  
      } else {
  
        Serial.println("Error: Failed to receive packet.");
        // TODO: Clear the buffer, check if this is causing messages to stop sending back and forth.
        // TODO: Reset!!
        
        // interface.getResponse().reset(); // TODO: Possibly remove this. This might be a bad idea.
        // clearCounter();
        return false; // Return true if a packet was read (i.e., received) successfully
      }
  
      // return true; // Return true if a packet was read (i.e., received) successfully
  
  } else if (interface.getResponse().isError()) { 
    
    Serial.println("ERROR!");
    return false;
    
  } else { // Timeout or error occurred
  
    // Serial.println("UNKNOWN ERROR!");
    return true; // TODO: Add a third state other than true or false

  }
}
