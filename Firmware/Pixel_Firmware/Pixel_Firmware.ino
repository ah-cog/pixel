/*
  "Pixel" Firmware (for Gestural Language), Rendition 2
  
  Creator: Michael Gubbels

  "The reasonable man adapts himself to the world;
   the unreasonable one persists in trying to adapt the world to himself.
   Therefore all progress depends on the unreasonable man."
   
   - George Bernard Shaw
     Man and Superman (1903) "Maxims for Revolutionists"

*/

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>
#include <SPI.h>

#include <Adafruit_NeoPixel.h>
#include "Light.h"

#include "Gesture.h"
#include "Movement.h"
#include "Communication.h"
#include "Looper.h"
#include "I2C.h"
#include "Ports.h"

/**
 * Module configuration
 */

//boolean hasCounter = false;

//            _               
//           | |              
//   ___  ___| |_ _   _ _ __  
//  / __|/ _ \ __| | | | '_ \ 
//  \__ \  __/ |_| |_| | |_) |
//  |___/\___|\__|\__,_| .__/ 
//                     | |    
//                     |_|    

void setup() {
  
  setupLooper(); // Setup the Looper engine.
  
//  setupPlatform(); // Setup Pixel's reflection (i.e., it's virtual machine)
  setupPorts(); // Setup pin mode for I/O
  setupLight(); // Setup the Pixel's color
  
  // Initialize pseudorandom number generator
  randomSeed(analogRead(0));

  // Initialize module's color
  //setModuleColor(random(256), random(256), random(256)); // Set the module's default color
//#if MESH_DEVICE_ADDRESS == 0x0000
//  setModuleColor(255, 0, 0);
//#elif MESH_DEVICE_ADDRESS == 0x0001
//  setModuleColor(0, 0, 255);
//#elif MESH_DEVICE_ADDRESS == 0x0002
//  setModuleColor(255, 255, 0);
//#endif
  setModuleColor(255, 255, 255);
  // setModuleColor(205, 205, 205); // Set the module's default color
  
  // Assign the module a unique color
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);

  // Fade on the module to let people know it's alive!
//  fadeOn();
//  fadeOff();
  
  // Setup mesh networking peripherals (i.e., RadioBlocks)
  setupCommunication();
  
  //
  // Setup serial communication (for debugging)
  //
  
  Serial.begin(9600);
  Serial.println(F("Pixel Firmware"));
  
  setupPlatform(); // Setup Pixel's reflection (i.e., it's virtual machine)
  
  // Setup physical orientation sensing peripherals (i.e., IMU)
  setupOrientationSensing();
  
  setupGestureSensing();
  
  // Send message to slave to reboot
//  int SLAVE_DEVICE_ADDRESS = 2;
  char buf[8];
  Wire.beginTransmission(SLAVE_DEVICE_ADDRESS); // transmit to device #4
  Wire.write("reboot  ");
  Wire.endTransmission();    // stop transmitting
  
  // Flash RGB LEDs
  blinkLight(3);
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
  
  Perform_Light_Behavior ();
  
  lastInputValue = touchInputMean;
  
  // Get module's input
  Get_Input_Port_Continuous (); // getInputPort(); // syncInputPort()
  
  //Serial.println(touchInputMean); // Output value for debugging (or manual calibration)
  
  if (touchInputMean > 3000 && lastInputValue <= 3000) { // Check if state changed to "pressed" from "not pressed"
    if (outputPinRemote == false) {
      // Output port is on this module!
      //Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
      Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
      Update_Channel_Value (moduleOutputChannel, PIN_VALUE_HIGH);
      Propagate_Channel_Value (moduleOutputChannel);
    } else {
      // Output port is on a different module than this one!
      addMessage (NEIGHBOR_ADDRESS, ACTIVATE_MODULE_OUTPUT);
    }
//    delay(500);
  } else if (touchInputMean <= 3000 && lastInputValue > 3000) { // Check if state changed to "not pressed" from "pressed"
    if (outputPinRemote == false) {
//      Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
//      syncPinValue(MODULE_OUTPUT_PIN);
      Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
      Update_Channel_Value (moduleOutputChannel, PIN_VALUE_LOW);
      Propagate_Channel_Value (moduleOutputChannel);
    } else {
      addMessage (NEIGHBOR_ADDRESS, DEACTIVATE_MODULE_OUTPUT);
    }
//    delay(500);
  }

  // TODO: Send updated state of THIS board (master) to the OTHER board (slave) for caching.
  
  // Get behavior updates from slave
  Get_Behavior_Transformations ();
  
  // Perform behavior step in the interpreter (Evaluate)
  // TODO: Transform_Behavior (i.e., the Behavior Transformer does this, akin to interpreting an instruction/command)
  boolean performanceResult = Perform_Behavior (performer);
  
  
  
  ////// GESTURE/MODULE STUFF (MASTER)
  
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

  // Change color/light if neededse
  if (crossfadeStep < 256) {
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
  if (senseOrientation ()) {
    storeData();
    
    // Classify live gesture sample
    unsigned long currentTime = millis();
    if (currentTime - lastGestureClassificationTime >= gestureSustainDuration[classifiedGestureIndex]) { // Check if gesture duration has expired
      classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
      
      if (classifyMostFrequentGesture) {
        // Add the most recent gesture classification to the end of the classification history list
        if (previousClassifiedGestureCount < PREVIOUS_CLASSIFIED_GESTURE_COUNT) {
          previousClassifiedGestures[previousClassifiedGestureCount] = classifiedGestureIndex;
          previousClassifiedGestureCount++;
        } else {
          for (int i = 0; i < (PREVIOUS_CLASSIFIED_GESTURE_COUNT - 1); i++) {
            previousClassifiedGestures[i] = previousClassifiedGestures[i + 1];
          }
          previousClassifiedGestures[PREVIOUS_CLASSIFIED_GESTURE_COUNT - 1] = classifiedGestureIndex;
        }
        
        // Get the most frequently classified gesture in the history
        for (int i = 0; i < GESTURE_COUNT; i++) { // Reset the previous classified gesture frequency (the number of times each occured in the short history)
          previousClassifiedGestureFrequency[i] = 0;
        }
        
        // Count the number of times each previous gesture occurred i.e., compute frequency of gesture classifications in short history)
        for (int i = 0; i < previousClassifiedGestureCount; i++) {
          int previouslyClassifiedGestureIndex = previousClassifiedGestures[i]; // Get a previous gesture's index
          previousClassifiedGestureFrequency[previouslyClassifiedGestureIndex]++; // Increment the gesture's frequency by one
        }
        
        // Determine the gesture most frequently classified
        int mostFrequentGestureIndex = 0;
        for (int i = 0; i < GESTURE_COUNT; i++) {
  //        Serial.print(previousClassifiedGestureFrequency[i]); Serial.print(" ");
          if (previousClassifiedGestureFrequency[i] > previousClassifiedGestureFrequency[mostFrequentGestureIndex]) {
            mostFrequentGestureIndex = i;
          }
        }
  //      Serial.println();
        
        // Update the classified gesture to the most frequent one
        classifiedGestureIndex = mostFrequentGestureIndex;
      }
      
      // HACK: Doesn't allow tilt left and tilt right (reclassifies them as "at rest, in hand"
//      if (classifiedGestureIndex == 4 || classifiedGestureIndex == 5) {
//        classifiedGestureIndex = 1;
//      }

      if (classifiedGestureIndex == 2 || classifiedGestureIndex == 3) { // If hasn't yet swung, then ignore tap gestures
        if (!hasSwung) {
          classifiedGestureIndex = 0;
        }
      } else if (classifiedGestureIndex == 1) { // If has swung, don't allow another swing if already swung (no effect)
        if (hasSwung) {
          classifiedGestureIndex = previousClassifiedGestureIndex;
        }
      }
      
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
      if (classifiedGestureIndex == 0) { // Check if gesture is "at rest"
        handleGestureAtRest();
      } else if (classifiedGestureIndex == 1) { // Check if gesture is "swing"
        handleGestureSwing();
      } else if (classifiedGestureIndex == 2) { // Check if gesture is "tap to another, as left"
        handleGestureTap(); // handleGestureTapToAnotherAsLeft();
      } else if (classifiedGestureIndex == 3) { // Check if gesture is "tap to another, as right"
        handleGestureTap(); // handleGestureTapToAnotherAsRight();
      } else if (classifiedGestureIndex == 4) { // Check if gesture is "shake"
        handleGestureShake();
      } else if (classifiedGestureIndex == 5) { // Check if gesture is "tilt left"
        handleGestureTiltLeft();
      } else if (classifiedGestureIndex == 6) { // Check if gesture is "tilt right"
        handleGestureTiltRight();
      } else if (classifiedGestureIndex == 7) { // Check if gesture is "tilt forward"
        handleGestureTiltForward();
      } else if (classifiedGestureIndex == 8) { // Check if gesture is "tilt backward"
        handleGestureTiltBackward();
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
    // Sent by "left" module:
    if (message.message == ANNOUNCE_GESTURE_TAP_AS_LEFT) {
      Serial.print("ANNOUNCE_GESTURE_TAP_AS_LEFT");
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT) {
      Serial.print("REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT");
    } else if (message.message == CONFIRM_GESTURE_TAP_AS_LEFT) {
      Serial.print("CONFIRM_GESTURE_TAP_AS_LEFT");
    }
    
    // Sent by "right" module:
    else if (message.message == ANNOUNCE_GESTURE_TAP_AS_RIGHT) {
      Serial.print("ANNOUNCE_GESTURE_TAP_AS_RIGHT");
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT) {
      Serial.print("REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT");
    } else if (message.message == CONFIRM_GESTURE_TAP_AS_RIGHT) {
      Serial.print("CONFIRM_GESTURE_TAP_AS_RIGHT");
    }
    
    else {
      Serial.print(message.message);
    }
    
    Serial.print(" from module ");
    Serial.print(message.source);
    Serial.print(" (of ");
    Serial.print(incomingMessageQueueSize);
    Serial.print(")\n");
    
    //
    // Process received messages
    //

    if (message.message == ANNOUNCE_GESTURE_TAP_AS_LEFT) { // From "left" module. "Left" module announces that it was tapped to another module as the left module [Sequence: Sequencing request (i.e., linking) confirmation, from "right" module]
      handleMessageTapToAnotherAsLeft(message);
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT) { // From "left" module. "Left" module requests "right" module to confirm that it received "ANNOUNCE_GESTURE_TAP_AS_LEFT"
       handleMessageRequestConfirmTapToAnotherAsLeft(message);
    } else if (message.message == CONFIRM_GESTURE_TAP_AS_LEFT) { // From "right" module (if it received the message from the "left" module). // Sequence: Sequencing (i.e., linking) confirmation, from "right" module
      handleMessageConfirmTapToAnotherAsLeft(message);
    } else if (message.message == ANNOUNCE_GESTURE_TAP_AS_RIGHT) { // From "right" module. // Sequence: Sequencing request (i.e., linking) confirmation, from "left" module
      handleMessageTapToAnotherAsRight(message);
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT) { // From "right" module. // Sequence: Sequencing (i.e., linking) confirmation, from "left" module
      handleMessageRequestConfirmTapToAnotherAsRight(message);
    } else if (message.message == CONFIRM_GESTURE_TAP_AS_RIGHT) { // From "left" module (if it received the messsage from the "right" module). // Sequence: Sequencing (i.e., linking) confirmation, from "right" module
      handleMessageConfirmTapToAnotherAsRight(message);
      // Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_LEFT");
    
    } else if (message.message == ACTIVATE_MODULE_OUTPUT) {
      // ACTIVATE_MODULE_OUTPUT
//      Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
//      syncPinValue(MODULE_OUTPUT_PIN);
      Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
      Update_Channel_Value (moduleOutputChannel, PIN_VALUE_HIGH);
      Propagate_Channel_Value (moduleOutputChannel);
    } else if (message.message == DEACTIVATE_MODULE_OUTPUT) {
//      Update_Channel_Value (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
//      syncPinValue(MODULE_OUTPUT_PIN);
      Channel* moduleOutputChannel = Get_Channel (platform, MODULE_OUTPUT_PIN);
      Update_Channel_Value (moduleOutputChannel, PIN_VALUE_LOW);
      Propagate_Channel_Value (moduleOutputChannel);
    }
    
    // TODO: ANNOUNCE_GESTURE_SHAKE
    
    // TODO: Deactivate module (because it's passing a sequence iterator forward)
    // TODO: Module announces removal from sequence (previous and next)
  }
  
  //
  // Update state of current module
  //
  
  //if (awaitingNextModuleConfirm) {
  if (awaitingNextModule) {
    unsigned long currentTime = millis();
    if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
      Serial.println("awaitingNextModule reset");
      awaitingNextModule = false;
      awaitingNextModuleConfirm = false;
    }
  }
  
  //if (awaitingPreviousModuleConfirm) {
  if (awaitingPreviousModule) {
    unsigned long currentTime = millis();
    if (currentTime - awaitingPreviousModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
      Serial.println("awaitingPreviousModule reset");
      awaitingPreviousModule = false;
      awaitingPreviousModuleConfirm = false;
    }
  }
  
  //
  // Send outgoing messages (e.g., this module's updated gesture)
  //
  
  unsigned long currentTime = millis();
  if (currentTime - lastMessageSendTime > RADIOBLOCK_PACKET_WRITE_TIMEOUT) {
  
    // Process mesh message queue  
    if (messageQueueSize > 0) {
      sendMessage();
    }
    
    // Update the time that a message was most-recently dispatched
    lastMessageSendTime = millis();
  }
}
