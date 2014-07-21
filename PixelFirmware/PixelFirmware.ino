/*
"Pixel" Firmware (for Gestural Language), Rendition 2
Authors: Michael Gubbels
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
#include "Loop.h"
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
  
  // Setup Pixel's reflection (i.e., it's virtual machine)
  setupPixel();
  
  // Set up pin mode for I/O
  setupPorts();
  
  setupLight();
  
  // Initialize pseudorandom number generator
  randomSeed(analogRead(0));

  // Initialize module's color
  //setModuleColor(random(256), random(256), random(256)); // Set the module's default color
#if MESH_DEVICE_ADDRESS == 0x0000
  setModuleColor(255, 0, 0);
#elif MESH_DEVICE_ADDRESS == 0x0001
  setModuleColor(0, 0, 255);
#elif MESH_DEVICE_ADDRESS == 0x0002
  setModuleColor(255, 255, 0);
#endif
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
  Serial.println(F("Pixel, Firmware South Korea"));
  
  // Setup physical orientation sensing peripherals (i.e., IMU)
  setupOrientationSensing();
  
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

boolean awaitingNextModule = false;
boolean awaitingPreviousModule = false;
boolean awaitingNextModuleConfirm = false;
boolean awaitingPreviousModuleConfirm = false;
unsigned long awaitingNextModuleStartTime = 0;
unsigned long awaitingPreviousModuleStartTime = 0;

boolean hasGestureProcessed = false;

// Sequence:
boolean isSequenced = false; // Does the module belong to a sequence?
boolean isActive = false; // Is the module the currently active module in the sequence

void loop() {
  
  doLightBehavior();
  
  lastInputValue = touchInputMean;
  
  // Get module's input
  getInputPortContinous(); // getInputPort(); // syncInputPort()
  
  //Serial.println(touchInputMean); // Output value for debugging (or manual calibration)
  
  if (touchInputMean > 3000 && lastInputValue <= 3000) { // Check if state changed to "pressed" from "not pressed"
    if (outputPinRemote == false) {
      // Output port is on this module!
      setPinValue2 (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
    } else {
      // Output port is on a different module than this one!
      addMessage(NEIGHBOR_ADDRESS, ACTIVATE_MODULE_OUTPUT);
    }
//    delay(500);
  } else if (touchInputMean <= 3000 && lastInputValue > 3000) { // Check if state changed to "not pressed" from "pressed"
    if (outputPinRemote == false) {
      setPinValue2 (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
    } else {
      addMessage(NEIGHBOR_ADDRESS, DEACTIVATE_MODULE_OUTPUT);
    }
//    delay(500);
  }
  
  // Set state of module's output based on its input
  //if (output to local)
//  if (outputPinRemote == false) {
//    
//    // Send output if input surpasses "touch" or "connected" thresholds
////    if (pinReflection[MODULE_OUTPUT_PIN].value == 0) {
//      if (touchInputMean > 3000) {
//        // digitalWrite(MODULE_OUTPUT_PIN, HIGH);
//        setPinValue2 (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
//        // TODO: Send message to "output" module(s).
//      } else {
//        // digitalWrite(MODULE_OUTPUT_PIN, LOW);
//        setPinValue2 (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
//      }
////    }
//    
//  } else {
//    
//    // TODO: Relay "turn on output" message to the pixel's output module
//    
//  }
  
  
  
  ////// LOOPER/VPL STUFF (SLAVE)
  
  //  for (int i = 0; i < 24; i++) {
//    char buf[4]; // "-2147483648\0"
//    Wire.beginTransmission(slaveDevice); // transmit to device #4
//    Wire.write("pin ");
//    Wire.write(itoa(i, buf, 10));
//    Wire.write(" ");
//    Wire.write(digitalRead(i) == HIGH ? "1" : "0");
//    //  Wire.write(";");        // sends five bytes
//    Wire.endTransmission();    // stop transmitting 
//  }

  // TODO: Send updated state of THIS board (master) to the OTHER board (slave) for caching.
  
  // Get behavior updates from slave (Apply)
  syncBehavior(); // TODO: Update "mental model" of device
  
  // Perform behavior step in the interpreter (Evaluate)
  behaviorLoopStep();
  
  
  
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

  // Change color/light if needed
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
  if (senseOrientation()) {
    storeData();
    
    // Classify live gesture sample
    unsigned long currentTime = millis();
    if (currentTime - lastGestureClassificationTime >= gestureSustainDuration[classifiedGestureIndex]) { // Check if gesture duration has expired
      classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
      
      // HACK: Doesn't allow tilt left and tilt right (reclassifies them as "at rest, in hand"
//      if (classifiedGestureIndex == 4 || classifiedGestureIndex == 5) {
//        classifiedGestureIndex = 1;
//      }
      
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
        handleGestureTapToAnotherAsLeft();
      } else if (classifiedGestureIndex == 3) { // Check if gesture is "tap to another, as right"
        handleGestureTapToAnotherAsRight();
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
      setPinValue2 (MODULE_OUTPUT_PIN, PIN_VALUE_HIGH);
    } else if (message.message == DEACTIVATE_MODULE_OUTPUT) {
      setPinValue2 (MODULE_OUTPUT_PIN, PIN_VALUE_LOW);
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





/**
 * "Right" module handle "tap to another, as left" message.
 */
boolean handleMessageTapToAnotherAsLeft(Message message) {
    if (awaitingPreviousModule) {
      // Update message state
      awaitingPreviousModule = false;
      awaitingPreviousModuleConfirm = true;
      
//      Serial.println(">> Received ANNOUNCE_GESTURE_TAP_AS_LEFT");
      
      // Send ACK message to message.source to confirm linking operation
//      addMessage(message.source, REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
      addBroadcast(REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
      
//      Serial.println("<< Sending REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT");
    }
}

/**
 * "Left" module handle request for confirmation of "tap to another, as left" message.
 */
boolean handleMessageRequestConfirmTapToAnotherAsLeft(Message message) {
        
//  unsigned long currentTime = millis();
//  if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
//    awaitingNextModuleConfirm = false;
//  }
        
  // Send ACK message to message.source to confirm linking operation (if not yet done)
  if (awaitingNextModuleConfirm) {
    awaitingNextModule = false;
    awaitingNextModuleConfirm = false; // awaitingNextModuleConfirm = true;
    
    Serial.println(">> Received REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT");
  
//    addBroadcast(CONFIRM_GESTURE_TAP_AS_LEFT);
    addMessage(message.source, CONFIRM_GESTURE_TAP_AS_LEFT);
    
    // HACK: Move this! This should be more robust, likely!
    // TODO: Make this map to the other module only when it is already sequenced!
    outputPinRemote = true;
    
    
  
//    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_LEFT");
    
    addNextModule(message.source);
    // TODO: addNextModule(message.source, SEQUENCE_ID);
    
    // Add module to sequence
    isSequenced = true;
    setSequenceColor(0, 255, 0); // Set the color of the sequence
    
    // Update the module's color
    if (isSequenced) {
      setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    } else {
      setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
    }
    //setModuleColor(255, 255, 255);
    //setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    
//    addBroadcast(CONFIRM_GESTURE_TAP_AS_LEFT);
    addMessage(message.source, CONFIRM_GESTURE_TAP_AS_LEFT);

    Serial.println("<< Sending ");
  }
}

/**
 * "Right" module handle confirmation of "tap to another, as left" message.
 */
boolean handleMessageConfirmTapToAnotherAsLeft (Message message) {
  Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_LEFT");
}

/**
 * "Left" module handle "tap to another, as right" message.
 */
boolean handleMessageTapToAnotherAsRight(Message message) {
  // NOTE: Received by the "left" module from the "right" module
  
  // If receive "tap to another, as left", then check if this module performed "tap to another, as right" recently. If so, link the modules in a sequence, starting with the other module first.
  
  if (awaitingNextModule) {
    
//    Serial.println(">> Received ANNOUNCE_GESTURE_TAP_AS_RIGHT");
    
    // Update message state
    awaitingNextModule = false;
    awaitingNextModuleConfirm = true;
    
    // Send ACK message to message.source to confirm linking operation
    addBroadcast(REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
//    addMessage(message.source, REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
    
//    Serial.println("<< Sending REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT");
  }
}

/**
 * "Right" module handle request for confirmation of "tap to another, as right" message.
 */
boolean handleMessageRequestConfirmTapToAnotherAsRight (Message message) {
  // NOTE: Received by the "right" module from the "left" module
  
//      unsigned long currentTime = millis();
//      if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
//        awaitingNextModuleConfirm = false;
//      }
    
  // Send ACK message to message.source to confirm linking operation (if not yet done)
  if (awaitingPreviousModuleConfirm) {
  
    Serial.println(">> Received REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT");

    awaitingPreviousModule = false;
    awaitingPreviousModuleConfirm = false;
    
    addPreviousModule (message.source);
    // TODO: addPreviousModule(message.source, SEQUENCE_ID);
    
    // Add module to sequence
    isSequenced = true;
    setSequenceColor(0, 255, 0); // Set the color of the sequence
    
    // Update the module's color
    if (isSequenced) {
      setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    } else {
      setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
    }
    //setModuleColor(255, 255, 255);
    //setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    
    
//    addBroadcast(CONFIRM_GESTURE_TAP_AS_RIGHT);
    addMessage(message.source, CONFIRM_GESTURE_TAP_AS_RIGHT);

    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_RIGHT");
  }
}

/**
 * "Left" module handle confirmation of "tap to another, as left" message.
 */
boolean handleMessageConfirmTapToAnotherAsRight (Message message) {
  Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_RIGHT");
  
  // TODO: Sequence the modules!
}

/**
 * Handle "at rest, on table" gesture.
 */
boolean handleGestureAtRest() {
  setColor(0.3 * defaultModuleColor[0], 0.3 * defaultModuleColor[1], 0.3 * defaultModuleColor[2]);
  
  addBroadcast(ANNOUNCE_GESTURE_AT_REST);
}

/**
 * Handle "at rest, in hand" gesture.
 */
//#define COMPOSITION_MODE_SEQUENCE 0
//#define COMPOSITION_MODE_MAP 1
//int compositionMode = false;
//unsigned long compositionModeStartTime = 0;

boolean handleGestureSwing() {
//  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);

  // Blink the lights
  blinkLight(3);

  // Update the module's color
  if (isSequenced) {
    setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
  } else {
    setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
  }
  
  addBroadcast(ANNOUNCE_GESTURE_SWING);
}

///**
// * Handle "at rest, in hand" gesture.
// */
//boolean handleGestureAtRestInHand() {
////  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
//
//  // Update the module's color
//  if (isSequenced) {
//    setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
//  } else {
//    setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
//  }
//  
//  addBroadcast(ANNOUNCE_GESTURE_AT_REST_IN_HAND);
//}

/**
 * Current (i.e., "left") module handle "tap to another, as left" gesture.
 */
boolean handleGestureTapToAnotherAsLeft() {
  //setColor(255, 0, 0);
  // Blink the lights five times
  blinkLight(5);
  
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
  
  addBroadcast(ANNOUNCE_GESTURE_TAP_AS_LEFT);
  Serial.println("^ Broadcasting ANNOUNCE_GESTURE_TAP_AS_LEFT");
}

/**
 * Current (i.e., "right") module handle "tap to another, as right" gesture.
 */
boolean handleGestureTapToAnotherAsRight() {
//  setColor(255, 0, 0);
  // Blink the lights five times
  blinkLight(5);
  
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

  addBroadcast(ANNOUNCE_GESTURE_TAP_AS_RIGHT);
  Serial.println("^ Broadcasting ANNOUNCE_GESTURE_TAP_AS_RIGHT");
}

/**
 * Handle "shake" gesture.
 */
boolean handleGestureShake() {
//  setColor(255, 0, 0);
  
  // TODO: Message next modules, say this module is leaving the sequence
  // TODO: Message previous module, say this module is leaving the sequence
  
  // HACK: Move this! This should be more robust, likely!
  // TODO: Make this map to the other module only when it is already sequenced!
  if (outputPinRemote == true) {
    
    // Revert output port to local module
    outputPinRemote = false;
    
  } else {
  
    // Unsequence modules
    isSequenced = false;
    if (isSequenced) {
      setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    } else {
      setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
    }
    
    // TODO: Send messages to adjacent modules so they can adapt to the change!
//    addBroadcast(ANNOUNCE_GESTURE_SHAKE);
    
    removePreviousModules();
    removeNextModules();
    
  }
  
  addBroadcast(ANNOUNCE_GESTURE_SHAKE);
}

/**
 * Handle "tilt left" gesture.
 */
boolean handleGestureTiltLeft() {
  setColor(0, 0, 255);
  
  addBroadcast(ANNOUNCE_GESTURE_TILT_LEFT);
}

/**
 * Handle "tilt right" gesture.
 */
boolean handleGestureTiltRight() {
  setColor(0, 255, 0);
  
  addBroadcast(ANNOUNCE_GESTURE_TILT_RIGHT);
}

/**
 * Handle "tilt forward" gesture.
 */
boolean handleGestureTiltForward() {
  setColor(0, 255, 0);
  
  addBroadcast(ANNOUNCE_GESTURE_TILT_FORWARD);
}

/**
 * Handle "tilt backward" gesture.
 */
boolean handleGestureTiltBackward() {
  setColor(0, 255, 0);
  
  addBroadcast(ANNOUNCE_GESTURE_TILT_BACKWARD);
}
