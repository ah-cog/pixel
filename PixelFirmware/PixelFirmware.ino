/*
"Pixel" Firmware, Rendition 2
Authors: Michael Gubbels
*/

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>
#include <SPI.h>

#include "Light.h"
#include "Gesture.h"
#include "Movement.h"
#include "Communication.h"

/**
 * Module configuration
 */

boolean hasCounter = false;
unsigned long lastCount = 0;
#define NEIGHBOR_COUNT 2
unsigned short int neighbors[NEIGHBOR_COUNT]; // TODO: Remove this! Use the "nextModules" and "previousModules" data structures and methods.
//unsigned short int next[1]; // TODO: Remove this! Use the "nextModules" and "previousModules" data structures and methods.

//            _               
//           | |              
//   ___  ___| |_ _   _ _ __  
//  / __|/ _ \ __| | | | '_ \ 
//  \__ \  __/ |_| |_| | |_) |
//  |___/\___|\__|\__,_| .__/ 
//                     | |    
//                     |_|    

void setup() {
  
  // Initialize pseudorandom number generator
  randomSeed(analogRead(0));

  // Initialize module's color
  setModuleColor(random(256), random(256), random(256)); // Set the module's default color
  // setModuleColor(205, 205, 205); // Set the module's default color
  
  // Assign the module a unique color
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);

  // Fade on the module to let people know it's alive!
  fadeOn();
  fadeOff();
  
  // Setup mesh networking peripherals (i.e., RadioBlocks)
  setupCommunication();
  
  //
  // Setup serial communication (for debugging)
  //
  
  Serial.begin(9600);
  Serial.println(F("Pixel 2014.04.01.23.54.14"));
  
  // Setup physical orientation sensing peripherals (i.e., IMU)
  setupOrientationSensing();
  
  // Flash RGB LEDs
  ledOn();  delay(100);
  ledOff(); delay(100);
  ledOn();  delay(100);
  ledOff(); delay(100);
  ledOn();  delay(100);
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

boolean awaitingNextModule = false;
boolean awaitingPreviousModule = false;
boolean awaitingNextModuleConfirm = false;
unsigned long awaitingNextModuleStartTime = 0;
unsigned long awaitingPreviousModuleStartTime = 0;

boolean hasGestureProcessed = false;

void loop() {
  
  // TODO: Add "getPins" function to read the state of pins, store the state of the pins, and handle interfacing with the pins (reading, writing), based on the program running (in both Looper and Mover).
  // TODO: Write code to allow Processing sketch (or other software) to automatically connect to this serial port. Send a "waiting for connection" signal one per second (or thereabout).
  
//  if (getPreviousModuleCount()) {
//    Serial.print("Previous modules: ");
//    Serial.print(getPreviousModuleCount());
//    Serial.println();
//  }
//  
//  if (getNextModuleCount()) {
//    Serial.print("Next modules: ");
//    Serial.print(getNextModuleCount());
//    Serial.println();
//  }

  // Change color/light if needed
  if (crossfadeStep < 256) {
    // crossfadeColorStep();
    applyColor();
  }
  
  //
  // Get data from mesh network
  //
  
  boolean hasReceivedMeshData = false;
  hasReceivedMeshData = receiveMeshData();
  
  //
  // Sense gesture (and phsyical orientation, generally)
  //
  
  boolean hasGestureChanged = false;
  if (senseOrientation()) {
    storeData();
    
    // Classify live gesture sample
    unsigned long currentTime = millis();
    if (currentTime - lastGestureClassificationTime >= gestureSustainDuration[classifiedGestureIndex]) { // Check if gesture duration has expired
      classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
      
      lastGestureClassificationTime = millis(); // Update time of most recent gesture classification
    }
    
    // Update current gesture (if it has changed)
    if (classifiedGestureIndex != previousClassifiedGestureIndex) {
      Serial.print("Detected gesture: ");
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
  
  //
  // Gesture Interpreter:
  // Process gestures. Perform the action associated with the gesture.
  //
  
  // Process current gesture (if it hasn't been processed yet)
  if (hasGestureChanged) { // Only executed when the gesture has changed
    if (!hasGestureProcessed) { // Only executed when the gesture hasn't yet been processed
      
      // Handle gesture
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
  }
  
  //
  // Process incoming messages in queue (if any)
  //
  
  if (incomingMessageQueueSize > 0) {
    Message message = dequeueIncomingMeshMessage();
    
    Serial.print("Received ");
//    Serial.print(gestureName[message.message]);
    Serial.print(message.message);
    Serial.print(" from module ");
    Serial.print(message.source);
    Serial.print(" (of ");
    Serial.print(incomingMessageQueueSize);
    Serial.print(")\n");
    
    //
    // Process received messages
    //
    
    if (message.message == 8) {
      // If receive "tap to another, as left", then check if this module performed "tap to another, as right" recently. If so, link the modules in a sequence, starting with the other module first.
      
      if (awaitingPreviousModule) {
        // TODO: previous += [ message.source ]
        
        awaitingPreviousModule = false;
        awaitingNextModuleConfirm = true;
        
        // Send ACK message to message.source to confirm linking operation
        //addMessage(message.source, 13);
        addBroadcast(13);
        
        // TODO: Wait for ACK response before the following... move it in another ACK message event handler.
        
        addPreviousModule(message.source);
        
//        setColor(255, 255, 255);
        setModuleColor(205, 205, 205);
        ledOn();
        delay(60);
        ledOff();
        delay(60);
        ledOn();
        delay(60);
        ledOff();
        delay(60);
        ledOn();
        delay(1500);
      }
      
    } else if (message.message == 9) {
      // If receive "tap to another, as right", then check if this module performed "tap to another, as left" recently. If so, link the modules in a sequence, starting with this module first.
      
      if (awaitingNextModule) {
        // TODO: next += [ message.source ]
        // Send "linked" to module (handshake)
        // TODO: In other module (and this one), when awaitingNextModule is true, look through the received messages for message "9"
        
        awaitingNextModule = false;
        awaitingNextModuleConfirm = true;
        
        // Send ACK message to message.source to confirm linking operation
//        addMessage(message.source, 13);
        addBroadcast(13);
        
        // TODO: Wait for ACK response before the following... move it in another ACK message event handler.
        
        addNextModule(message.source);
        
//        setColor(255, 255, 255);
        setModuleColor(205, 205, 205);
        
        ledOn();
        delay(60);
        ledOff();
        delay(60);
        ledOn();
        delay(60);
        ledOff();
        delay(60);
        ledOn();
        delay(1500);
      }
      
    } else if (message.message == 13) { // Sequencing (i.e., linking) confirmation
    
      Serial.println(">>>> PROCESSING MESSAGE 13");
      
//      if (awaitingNextModule) {
        
        // awaitingNextModule = false;
        // awaitingNextModuleConfirm = true;
        
        // Send ACK message to message.source to confirm linking operation (if not yet done)
        if (awaitingNextModuleConfirm) {
//          addMessage(message.source, 13);
          addBroadcast(13);
//        addMessage(message.source, 14);

          awaitingNextModuleConfirm = false;
        }
        
        setColor(255, 255, 255);
        ledOn();
        delay(200);
        ledOff();
        delay(200);
        ledOn();
        delay(200);
        ledOff();
        delay(200);
        ledOn();
        
        // TODO: Wait for ACK response before the following... move it in another ACK message event handler.
        
        //addNextModule(message.source);
//      }
    } else if (message.message == 14) { // The ACK message from message.source confirming linking operation (if not yet done)
    }
  }
  
  //
  // Send outgoing messages (e.g., this module's updated gesture)
  //
  
  // TODO: Handle "ongoing" gesture (i.e., do the stuff that should be done more than once, or as long as the gesture is active)
  
  // Process mesh message queue  
  // TODO: Put this in the following bit of logic (with RADIOBLOCK_PACKET_WRITE_TIMEOUT)
  if (messageQueueSize > 0) {
    sendMessage();
  }
    
  unsigned long currentTime = millis();
  if (currentTime - lastCount > RADIOBLOCK_PACKET_WRITE_TIMEOUT) {
  
    // Process mesh message queue  
//    if (messageQueueSize > 0) {
//      sendMessage();
//    }
    
    // Update the time that a message was most-recently dispatched
    lastCount = millis();
  }
}

/**
 * Handle "at rest, on table" gesture.
 */
boolean handleGestureAtRestOnTable() {
//  crossfadeColor(100, 100, 100);
  setColor(0.3 * defaultModuleColor[0], 0.3 * defaultModuleColor[1], 0.3 * defaultModuleColor[2]);
  //ledOff();
  
  addBroadcast(1);
}

/**
 * Handle "at rest, in hand" gesture.
 */
boolean handleGestureAtRestInHand() {
//  crossfadeColor(color[0], color[1], color[2]);
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
//  ledOn();
  
  addBroadcast(2);
  
//  crossfadeColor(255, 0, 0);
//  crossfadeColor(0, 255, 0);
//  crossfadeColor(0, 0, 255);
//  crossfadeColor(255, 255, 0);
//  crossfadeColor(0, 255, 255);
//  crossfadeColor(255, 255, 255);
}

/**
 * Handle "pick up" gesture.
 */
boolean handleGesturePickUp() {
  addBroadcast(3);
}

/**
 * Handle "place down" gesture.
 */
boolean handleGesturePlaceDown() {
  addBroadcast(4);
}

/**
 * Handle "tilt left" gesture.
 */
boolean handleGestureTiltLeft() {
  setColor(0, 0, 255);
  
  addBroadcast(5);
}

/**
 * Handle "tilt right" gesture.
 */
boolean handleGestureTiltRight() {
  setColor(0, 255, 0);
  
  addBroadcast(6);
}

/**
 * Handle "shake" gesture.
 */
boolean handleGestureShake() {
  setColor(255, 0, 0);
  ledOn();
  
  addBroadcast(7);.
}

/**
 * Handle "tap to another, as left" gesture.
 */
boolean handleGestureTapToAnotherAsLeft() {
  
//  moduleColor[0] = 255;
//  moduleColor[1] = 0;
//  moduleColor[2] = 0;
  setColor(255, 0, 0);
  
  addBroadcast(8);
  awaitingNextModule = true;
  awaitingNextModuleStartTime = millis();
}

/**
 * Handle "tap to another, as right" gesture.
 */
boolean handleGestureTapToAnotherAsRight() {
//  moduleColor[0] = 0;
//  moduleColor[1] = 0;
//  moduleColor[2] = 255;
  setColor(255, 0, 0);
  
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

  addBroadcast(9);
  awaitingPreviousModule = true;
  awaitingPreviousModuleStartTime = millis();
}
