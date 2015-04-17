#ifndef GESTURE_H
#define GESTURE_H

#include "Communication.h"
#include "Gestures.h"

#define MAX_INTEGER_VALUE 32767

//#define ANNOUNCE_GESTURE_AT_REST 1
////#define ANNOUNCE_GESTURE_AT_REST_ON_TABLE 1
////#define ANNOUNCE_GESTURE_AT_REST_IN_HAND 2
////#define ANNOUNCE_GESTURE_PICK_UP 3
////#define ANNOUNCE_GESTURE_PLACE_DOWN 4
//#define ANNOUNCE_GESTURE_SWING 2
//
//#define ANNOUNCE_GESTURE_TAP_AS_LEFT 3 // 8
//#define ANNOUNCE_GESTURE_TAP_AS_RIGHT 4 // 9
//
//#define ANNOUNCE_GESTURE_SHAKE 5 // 7
//
//#define ANNOUNCE_GESTURE_TILT_LEFT 6 // 5
//#define ANNOUNCE_GESTURE_TILT_RIGHT 7 // 6
//#define ANNOUNCE_GESTURE_TILT_FORWARD 8
//#define ANNOUNCE_GESTURE_TILT_BACKWARD 9
//
//#define REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT 13
//#define REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT 14
//
//#define CONFIRM_GESTURE_TAP_AS_LEFT 15
//#define CONFIRM_GESTURE_TAP_AS_RIGHT 16

//#define ANNOUNCE_UNSEQUENCE 17 // Same as "ANNOUNCE_GESTURE_SHAKE" ?

#define ACTIVATE_MODULE_OUTPUT 20
#define DEACTIVATE_MODULE_OUTPUT 21

boolean classifyMostFrequentGesture = false;
#define PREVIOUS_CLASSIFIED_GESTURE_COUNT 10
int previousClassifiedGestures[PREVIOUS_CLASSIFIED_GESTURE_COUNT];
int previousClassifiedGestureCount = 0;

int previousClassifiedGestureFrequency[GESTURE_COUNT];

/**
 * Sets up gesture sensing.
 */
void Setup_Gesture_Sensing () {
  for (int i = 0; i < PREVIOUS_CLASSIFIED_GESTURE_COUNT; i++) {
    previousClassifiedGestures[i] = -1;
  }
  previousClassifiedGestureCount = 0;
  
  // Reset the previous classified gesture frequency (the number of times each occured in the short history)
  for (int i = 0; i < GESTURE_COUNT; i++) {
    previousClassifiedGestureFrequency[i] = 0;
  }
}

/**
 * Calculate the deviation of the live gesture sample and the signature gesture sample along only one axis (x, y, or z).
 */
int getGestureAxisDeviation (int gestureSignatureIndex, int axis) { // (int gestureSample[GESTURE_SIGNATURE_SIZE], int gestureCandidate[GESTURE_SIGNATURE_SIZE]) {
  
  int delta = 0; // sum of difference between average x curve and most-recent x data
  
//  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
//    if (i < liveSample.size() && i < gestureSample.size()) {
//        int difference = abs(gestureSample.get(i) - liveSample.get(i));
//        delta = delta + difference;
//    }
//  }

  // Compare the first 50 points of the gesture signature to the most recent 50 accelerometer data points
  //for (int point = 0; point < GESTURE_SIGNATURE_SIZE; point++) {
  for (int point = 0; point < gestureSignatureSize[gestureSignatureIndex]; point++) {
      int difference = abs(gestureSignature[gestureSignatureIndex][axis][point] - gestureCandidate[axis][point]);
      delta = delta + difference;
  }
  
  return delta;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureDeviation (int classifiedGestureIndex) {
  int deltaTotal = 0;
    
  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    int delta = getGestureAxisDeviation(classifiedGestureIndex, axis); // gestureSignature[classifiedGestureIndex][axis], gestureCandidate[axis]);
    deltaTotal = deltaTotal + delta;
  }
  
  return deltaTotal;
}

/**
 * Relative instability. How relative is the live sample in comparison to a gesture's signature sample?
 */
int getGestureAxisInstability (int gestureSignatureIndex, int axis) { // (int gestureSample[GESTURE_SIGNATURE_SIZE], int gestureCandidate[GESTURE_SIGNATURE_SIZE]) {
  
  int relativeInstability = 0; // sum of difference between average x curve and most-recent x data
  
//  for (int i = liveSample.size() - comparisonWindowSize; i < liveSample.size(); i++) {
//    if (i < liveSample.size() && i < gestureSample.size()) {
//        int difference = abs(gestureSample.get(i) - liveSample.get(i));
//        delta = delta + difference;
//    }
//  }

  //for (int point = 0; point < GESTURE_SIGNATURE_SIZE - 1; point++) {
  for (int point = 0; point < gestureSignatureSize[gestureSignatureIndex] - 1; point++) {
      int signatureDifference = abs(gestureSignature[gestureSignatureIndex][axis][point + 1] - gestureCandidate[axis][point]);
      int liveDifference = abs(gestureCandidate[axis][point + 1] - gestureCandidate[axis][point]);
      int instabilityDifference = abs(signatureDifference - liveDifference);
      relativeInstability = relativeInstability + instabilityDifference;
  }
  
  return relativeInstability;
}

/**
 * Calculates the deviation between the sampled live gesture and the gesture signature sample.
 */
int getGestureInstability (int classifiedGestureIndex) {
  int instabilityTotal = 0;
    
  // Compare the difference between the average sample for each axis and the live sample
  for (int axis = 0; axis < 3; axis++) {
    int instability = getGestureAxisInstability(classifiedGestureIndex, axis); // gestureSignature[classifiedGestureIndex][axis], gestureCandidate[axis]);
    instabilityTotal = instabilityTotal + instability;
  }
  
  return instabilityTotal;
}

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGestureFromTransitions () {
  int minimumDeviationIndex = -1;
  int minimumDeviation = MAX_INTEGER_VALUE;
  
  for (int i = 0; i < GESTURE_COUNT; i++) {
    
      int gestureSignatureIndex = gestureTransitions[classifiedGestureIndex][i]; // Get index of possible gesture
      
      if (gestureSignatureIndex != -1) { // Make sure the transition is valid before continuing
        
        // Calculate the gesture's deviation from the gesture signature
        int gestureDeviation = getGestureDeviation(gestureSignatureIndex);
//        int gestureInstability = getGestureInstability(gestureSignatureIndex);

        // Check if the sample's deviation
        if (minimumDeviationIndex == -1 || (gestureDeviation /*+ gestureInstability*/) < minimumDeviation) {
          minimumDeviationIndex = gestureSignatureIndex;
          minimumDeviation = gestureDeviation /*+ gestureInstability*/;
        }
    }
  }

  return minimumDeviationIndex;
}

/**
 * Classify the gesture. Choose the gesture that has a "signature" time series that best  
 * matches the recent window of live data.
 */
int classifyGestureFromTransitionsWithInstability () {
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

// Temporary/Need to refractor
//boolean hasSwung = false;

/**
 * Handle "at rest, on table" gesture.
 */
boolean Handle_Gesture_At_Rest () {
  // TODO: Replace the following line, but keep the idea. Reduce brightness (to save power). But keep the same color!
  // Update_Color (0.3 * defaultModuleColor[0], 0.3 * defaultModuleColor[1], 0.3 * defaultModuleColor[2]);
  
//  Queue_Broadcast (ANNOUNCE_GESTURE_AT_REST);
//  Queue_Broadcast ("fyi gesture at rest");
    Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture at rest");
}

unsigned long lastSwingTime = 0L;
unsigned long lastSwingTimeout = 5000; // i.e., the time for which a response to a swing can be received

/**
 * Handle "at rest, in hand" gesture.
 */
//#define COMPOSITION_MODE_SEQUENCE 0
//#define COMPOSITION_MODE_MAP 1
//int compositionMode = false;
//unsigned long compositionModeStartTime = 0;

boolean Handle_Gesture_Swing () {
//  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);

  // TODO: Swing multiple times to change color.

  if (hasSwung == false) {
    
    if (lastSwingAddress != -1) { // i.e., another module has been swung
    
      Stop_Blink_Light ();
    
    } else {
    
      // Blink the lights
    //  blinkLight(3);
      Start_Blink_Light ();
      hasSwung = true;
      
      lastSwingTime = millis ();
    
  //    // Update the module's color
  //    if (isSequenced) {
  //      Update_Color (sequenceColor[0], sequenceColor[1], sequenceColor[2]);
  //    } else {
  //      Update_Color (defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
  //    }
      
    //  Queue_Broadcast (ANNOUNCE_GESTURE_SWING);
      
    }
    
    Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture swing");
    
//    Start_Focus ();
    
  }
  
  return true;
}

boolean Handle_Gesture_Tap () {
  
  Serial.println ("handleGestureTap");
  
  if (hasSwung) {
  
//    stopBlinkLight();
//    hasSwung = false;
//  
////  awaitingNextModule = true;
////  awaitingNextModuleConfirm = true;
////  awaitingNextModuleStartTime = millis();
//  
////    addBroadcast(ANNOUNCE_GESTURE_HOT_TAP);
////    Serial.println("^ Broadcasting ANNOUNCE_GESTURE_TAP");
    
  } else {
    
    // When the module is tapped, and if it has received an ANNOUNCE_GESTURE_SWING from a neighbor very recently, respond to the neighbor (that sent the "swing" message) and notify all other neighbors that this module is responding to the swing and to remove it from their memory of "recently swung" neighbors.
    if (lastSwingAddress != -1) {
      
//      Queue_Broadcast (ANNOUNCE_GESTURE_TAP);
      Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tap");
      Serial.println ("^ Broadcasting ANNOUNCE_GESTURE_TAP");
      
    }
    
  }
}

/**
 * Current (i.e., "left") module handle "tap to another, as left" gesture.
 */
boolean Handle_Gesture_Tap_As_Left () {
  //setColor(255, 0, 0);
  // Blink the lights five times
  Blink_Light (5);
  Start_Blink_Light ();
  
//  if (!awaitingPreviousModule) {
//    awaitingNextModule = true;
//    awaitingNextModuleConfirm = true;
//    awaitingNextModuleStartTime = millis();
//  }

//  if (!awaitingNextModule) {
//    awaitingPreviousModule = true;
//    awaitingPreviousModuleConfirm = true;
//    awaitingPreviousModuleStartTime = millis();
//  }

  awaitingNextModule = true;
  awaitingNextModuleConfirm = true;
  awaitingNextModuleStartTime = millis();
  
  // Queue_Broadcast (ANNOUNCE_GESTURE_TAP_AS_LEFT);
  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tap as left");
  Serial.println("^ Broadcasting ANNOUNCE_GESTURE_TAP_AS_LEFT");
}

/**
 * Current (i.e., "right") module handle "tap to another, as right" gesture.
 */
boolean Handle_Gesture_Tap_As_Right () {
//  setColor(255, 0, 0);
  // Blink the lights five times
//  blinkLight(5);
//  startBlinkLight();
  
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
 
//  if (!awaitingNextModule) {
//    awaitingPreviousModule = true;
//    awaitingPreviousModuleConfirm = true;
//    awaitingPreviousModuleStartTime = millis();
//  }

//  if (!awaitingPreviousModule) {
//    awaitingNextModule = true;
//    awaitingNextModuleConfirm = true;
//    awaitingNextModuleStartTime = millis();
//  }

  awaitingPreviousModule = true;
  awaitingPreviousModuleConfirm = true;
  awaitingPreviousModuleStartTime = millis();

  // Queue_Broadcast (ANNOUNCE_GESTURE_TAP_AS_RIGHT);
  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tap as right");
  Serial.println("^ Broadcasting ANNOUNCE_GESTURE_TAP_AS_RIGHT");
}

/**
 * Handle "shake" gesture.
 */
boolean Handle_Gesture_Shake () {
//  setColor(255, 0, 0);
  
  // TODO: Message next modules, say this module is leaving the sequence
  // TODO: Message previous module, say this module is leaving the sequence
  
  // HACK: Move this! This should be more robust, likely!
  // TODO: Make this map to the other module only when it is already sequenced!
//  if (outputPinRemote == true) {
//    
//    // Revert output port to local module
//    outputPinRemote = false;
//    
//  } else {
    
    if (hasSwung) {
      // Stop blinking to cancel behavior shaping (i.e., cool the hot potato)
      // TODO: Stop blinking when successfully linked, too!
      Stop_Blink_Light ();
      hasSwung = false;
      // TODO: Cancel "pairing request"
    
//      // Unsequence modules
//      isSequenced = false;
//      if (isSequenced) {
//        setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
//      } else {
//        setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
//      }
      
      // TODO: Send messages to adjacent modules so they can adapt to the change!
  //    addBroadcast(ANNOUNCE_GESTURE_SHAKE);
      
//      removePreviousModules();
//      removeNextModules();
    }
    
//  }
  
//  Queue_Broadcast (ANNOUNCE_GESTURE_SHAKE);
  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture shake");
}

///**
// * Handle "tilt left" gesture.
// */
//boolean Handle_Gesture_Tilt_Left () {
////  Update_Color (0, 0, 255);
//  
////  Queue_Broadcast (ANNOUNCE_GESTURE_TILT_LEFT);
//  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tilt left");
//}

///**
// * Handle "tilt right" gesture.
// */
//boolean Handle_Gesture_Tilt_Right () {
////  Update_Color (0, 255, 0);
//  
////  Queue_Broadcast (ANNOUNCE_GESTURE_TILT_RIGHT);
//  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tilt right");
//}

///**
// * Handle "tilt forward" gesture.
// */
//boolean Handle_Gesture_Tilt_Forward () {
////  Update_Color (0, 255, 0);
//  
////  Queue_Broadcast (ANNOUNCE_GESTURE_TILT_FORWARD);
//  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tilt forward");
//}

///**
// * Handle "tilt backward" gesture.
// */
//boolean Handle_Gesture_Tilt_Backward () {
////  Update_Color (0, 255, 0);
//  
////  Queue_Broadcast (ANNOUNCE_GESTURE_TILT_BACKWARD);
//  Queue_Message (platformUuid, BROADCAST_ADDRESS, "notice gesture tilt backward");
//}

#endif
