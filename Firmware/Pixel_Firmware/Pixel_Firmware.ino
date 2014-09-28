/*
  "Pixel" Firmware (for Gestural Language), Rendition 2
  
  Creator: Michael Gubbels

  "The reasonable man adapts himself to the world;
   the unreasonable one persists in trying to adapt the world to himself.
   Therefore all progress depends on the unreasonable man."
   
   - George Bernard Shaw
     Man and Superman (1903) "Maxims for Revolutionists"

*/

#include <EEPROM.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>
#include <SPI.h>

#include <Adafruit_NeoPixel.h>
#include "Platform.h"
#include "Light.h"
#include "Sound.h"
#include "Motion.h"

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

#define EEPROM_SIZE 512
boolean clearEeprom () {
  // write a 0 to all bytes of the EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
}


// Foundation State:
boolean hasFoundationUuid = false;

#define UUID_SIZE 36
int foundationUuidSignatureMemoryAddress = 0;
int foundationUuidMemoryAddress = foundationUuidSignatureMemoryAddress + 1;
char foundationUuid[UUID_SIZE];
boolean setupFoundation () {
  // Check if UUID has been written to EEPROM, and if so, read it into RAM (and load it into the Looper engine).
  // If not, generate a UUID into memory
  
  // Read the UUID signature byte. If it is equal to '!', then assume a UUID has been written.
  byte foundationUuidSignatureByte = EEPROM.read (foundationUuidSignatureMemoryAddress);

  // Check if the UUID signature byte (i.e., '!') has been written to memory.
  if (foundationUuidSignatureByte != '!') {
    // The UUID is not present in EEPROM, so generate one and write it to EEPROM before proceeding.
  
    // Version 4 UUID: https://en.wikipedia.org/wiki/Universally_unique_identifier
//    char generatedFoundationUuid[] = "c6ade405-3b5d-4783-8d2e-ac53d429a857"; // Module 1
//    char generatedFoundationUuid[] = "d9c95b97-fbcc-484f-bc61-2572c4a00d9c"; // Module 2
    char generatedFoundationUuid[] = "cad165c7-2238-4455-9f85-7f025a9ddb6f"; // Module 3
//    char generatedFoundationUuid[] = "5f29d296-d444-49e5-8988-5b0bd71b3dcc"; // Module 4
//    char generatedFoundationUuid[] = "118b8b18-a851-49fa-aef9-b8f5b18da90d"; // Module 5
    // TODO: char* generateFoundationUuid ()
    
    // Write UUID signature to EEPROM
    EEPROM.write (foundationUuidSignatureMemoryAddress, '!'); // Write foundation UUID signature (i.e., the '!' symbol)
  
    // Write UUID to EEPROM
    int i = 0;
    for (int address = foundationUuidMemoryAddress; address < (foundationUuidMemoryAddress + UUID_SIZE); address++) {
      EEPROM.write (address, generatedFoundationUuid[i]);
      i++;
    }
    
  }
  
  // Read UUID from EEPROM
  int i = 0;
  for (int address = foundationUuidMemoryAddress; address < (foundationUuidMemoryAddress + UUID_SIZE); address++) {
    foundationUuid[i] = EEPROM.read (address);
    i++;
  }
  
  Serial.print ("Foundation UUID: "); for (int i = 0; i < UUID_SIZE; i++) { Serial.print ((char) foundationUuid[i]); } Serial.print ("\n");
  
}

void setup () {
  
  if (hasFoundationUuid == false) {
    setupFoundation ();
    hasFoundationUuid = true;
  } // The layer on which the "platform" depends
  
  setupLooper (); // Setup the Looper engine.
  
  setupPlatformUuid ();
//  setupPlatform(); // Setup Pixel's reflection (i.e., it's virtual machine)
  setupPorts (); // Setup pin mode for I/O
  setupLight (); // Setup the Pixel's color
  
  // Initialize pseudorandom number generator
  randomSeed (analogRead (0));
  
  // TODO: Read device GUID from EEPROM if it exists, otherwise generate one, store it in EEPROM, and read it.

  // Initialize module's color
  //setModuleColor(random(256), random(256), random(256)); // Set the module's default color
//#if MESH_DEVICE_ADDRESS == 0x0000
//  setModuleColor(255, 0, 0);
//#elif MESH_DEVICE_ADDRESS == 0x0001
//  setModuleColor(0, 0, 255);
//#elif MESH_DEVICE_ADDRESS == 0x0002
//  setModuleColor(255, 255, 0);
//#endif
  setModuleColor (255, 255, 255);
  // setModuleColor(205, 205, 205); // Set the module's default color
  
  // Assign the module a unique color
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);

  // Fade on the module to let people know it's alive!
//  fadeOn();
//  fadeOff();
  
  // Setup mesh networking peripherals (i.e., RadioBlocks)
  setupCommunication ();
  setupCommunication2 ();
  
  //
  // Setup serial communication (for debugging)
  //
  
  Serial.begin (9600);
  Serial.println (F("Pixel Firmware"));
  
  setupPlatform (); // Setup Pixel's reflection (i.e., it's virtual machine)
  
  // Setup physical orientation sensing peripherals (i.e., IMU)
  setupOrientationSensing ();
  
  setupGestureSensing ();
  
  // TODO: Send message to slave to reboot (on boot, to keep them in sync)
////  int SLAVE_DEVICE_ADDRESS = 2;
//  char buf[8];
//  Wire.beginTransmission (SLAVE_DEVICE_ADDRESS); // transmit to device #4
//  Wire.write("reboot  ");
//  Wire.endTransmission ();    // stop transmitting

  setupSound ();
  
  // Flash RGB LEDs
  blinkLight (3);
  
  
  Move_Motion (0.5 * 1000);
  
//  Serial.print ("Foundation UUID: "); for (int i = 0; i < UUID_SIZE; i++) { Serial.print ((char) foundationUuid[i]); } Serial.print ("\n");
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

void loop () {
  
  if (hasPlatformUuid) {

    // Broadcast device's address (UUID)
    unsigned long currentTime = millis ();
    if (currentTime - lastBroadcastTime > broadcastTimeout) {
      
      if (isReading == false) {
        isWriting = true;
      
        // MESH_SERIAL.write ('!');
//        String data = String ("{ uuid: ") + String (platformUuid) + String (" , type: 'keep-alive' }");
//        const int serialBufferSize = 64;
//        char charData[serialBufferSize];
//        data.toCharArray (charData, serialBufferSize);
        
        int bytesSent = 0;
        String data = "";
        if (classifiedGestureIndex != previousClassifiedGestureIndex) {
          data = String ("{ uuid: ") + String (platformUuid) + String (" , gesture: ") + String (gestureName[classifiedGestureIndex]) + String (" }");
          const int serialBufferSize = 64;
          char charData[serialBufferSize];
          data.toCharArray (charData, serialBufferSize);
          bytesSent = MESH_SERIAL.write (charData);
        }
        
//        Serial.println (charData);
        
//        int bytesSent = MESH_SERIAL.write (charData);
//        Serial.print ("sent "); Serial.print (bytesSent); Serial.print (" bytes\n\n");
        
        lastBroadcastTime = millis ();
        
        if (bytesSent >= data.length ()) {
          isWriting = false;
        }
      }
    }
    
//    // Relay data received via the serial console over mesh
//    // TODO: (?) Remove this, eventually!
//    if (Serial.available() > 0) {
//      incomingByte = Serial.read ();
//      // Serial.print("USB received: ");
//      // Serial.println(incomingByte, DEC);
//      // HWSERIAL.print("USB received:");
//      MESH_SERIAL.print ((char) incomingByte);
//      MESH_SERIAL.flush ();
//      /* Serial2.print ((char) incomingByte); */
//    }
    
    // Receive any data received over the mesh network.
    if (isWriting == false) {
      if (MESH_SERIAL.available () > 0) {
        isReading = true;
        
        int incomingByte = MESH_SERIAL.read ();
        // Serial.print("UART received: ");
//        Serial.print ((char) incomingByte);
        
        if (incomingByte == '}') {
          
          // Terminate the buffer
          serialBuffer[serialBufferSize] = incomingByte;
          serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
          serialBuffer[serialBufferSize] = '\0';
          
          // TODO: Terminate the buffer and return it for parsing!
          String uuidParameter = String (serialBuffer);
          int neighborUuid = getValue(uuidParameter, ' ', 2).toInt ();
          
          boolean hasNeighbor = false;
          for (int i = 0; i < neighborCount; i++) {
            if (neighbors[i] == neighborUuid) {
              hasNeighbor = true;
              break;
            }
          }
          if (hasNeighbor == false) {
            neighbors[neighborCount] = neighborUuid;
            neighborCount++;
            Serial.print ("Added neighbor "); Serial.print (neighborCount); Serial.print (": "); Serial.print (neighborUuid); Serial.print ("\n");
          }
          
          // TODO: Check timestamps when last received a broadcast, and ping those not reached for a long time, and remove them if needed.
          
          serialBufferSize = 0;
          
          Serial.println (neighborUuid);
          
        } else {
          
          serialBuffer[serialBufferSize] = incomingByte;
          serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
          
        }
        // Serial2.print ((char) incomingByte);
        // HWSERIAL.print("UART received:");
        // HWSERIAL.println(incomingByte, DEC);
        
        // TODO: Buffer the data received over mesh until the message is completely received.
        
        isReading = false;
      }
    }
    
    /*
    if (Serial2.available () > 0) {
      incomingByte = Serial2.read();
      // Serial.print("UART received: ");
      Serial.print ((char) incomingByte);
      // HWSERIAL.print ((char) incomingByte);
      // HWSERIAL.print("UART received:");
      // HWSERIAL.println(incomingByte, DEC);
    }
    */
  }
  
  
  
  
  
  // TODO: Broadcast the foundation's default device address (upon boot)
  
  // TODO: Negotiate a unique dynamic mesh address
 
  // TODO: Broadcast the platform's dynamic mesh address (once per 30 seconds or so)
  
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
      queueMessage (NEIGHBOR_ADDRESS, ACTIVATE_MODULE_OUTPUT);
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
      queueMessage (NEIGHBOR_ADDRESS, DEACTIVATE_MODULE_OUTPUT);
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

  // Change color/light if needed
  if (crossfadeStep < 256) {
    applyColor();
  }
  
  //
  // Get data from mesh network
  //
  
  unsigned long currentTime = millis ();
  
  // Update gesture/mesh communication timers
  // TODO: Add this to Looper's timer!
  if (lastSwingAddress != -1) {
    if (currentTime - lastReceivedSwingTime > lastReceivedSwingTimeout) {
      lastSwingAddress = -1;
    }
  }
  
  // Update gesture/mesh communication timers
  // TODO: Add this to Looper's timer!
  if (hasSwung == true) {
    currentTime = millis ();
    if (currentTime - lastSwingTime > lastSwingTimeout) {
      
      // Cancel swing gesture
      stopBlinkLight();
      hasSwung = false;
      
      // Reset the timer
      lastSwingTime = 0L;
    }
  }
  
  // Broadcast message notifying other modules that this module is still active (i.e., broadcast this module's "heartbeat")
  currentTime = millis ();
  if (currentTime - lastSentActive >= lastSentActiveTimeout) {
    addBroadcast (ANNOUNCE_ACTIVE);
    lastSentActive = millis ();
  }
  
  
  // Check for mesh data and receive it if present
  boolean hasReceivedMeshData = false;
//  hasReceivedMeshData = receiveMeshData();
  
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

      if (classifiedGestureIndex == 4) { // Ignore shake gesture if not yet swung
        // TODO: Check if has a "remote output", and if so, then DO NOT ignore the swing gesture.
        if (hasSwung == false) {
          if (outputPinRemote == true) {
//            classifiedGestureIndex = 4; // Set "shake" to "at rest"
          } else {
            classifiedGestureIndex = 1; // Change "shake" to "swing" (since they're similar enough for this to make sense)
          }
        }
      } else if (classifiedGestureIndex == 2 || classifiedGestureIndex == 3) { // If hasn't yet swung, then ignore tap gestures
        if (lastSwingAddress == -1) {
          classifiedGestureIndex = 0; // Set to "at rest"
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
        handleGestureTap();
        // handleGestureTapToAnotherAsLeft();
      } else if (classifiedGestureIndex == 3) { // Check if gesture is "tap to another, as right"
        handleGestureTap();
        // handleGestureTapToAnotherAsRight();
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
    
    Serial.print ("Received ");
    
    // Sent by "left" module:
    if (message.message == ANNOUNCE_ACTIVE) {
      Serial.print ("ANNOUNCE_ACTIVE");
    }
    
    else if (message.message == ANNOUNCE_GESTURE_SWING) {
      Serial.print("ANNOUNCE_GESTURE_SWING");
    } else if (message.message == ANNOUNCE_GESTURE_TAP) {
      Serial.print("ANNOUNCE_GESTURE_TAP");
    }
    
    else if (message.message == ANNOUNCE_GESTURE_TAP_AS_LEFT) {
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
    
    else if (message.message == ACTIVATE_MODULE_OUTPUT) {
      Serial.print("ACTIVATE_MODULE_OUTPUT");
    } else if (message.message == DEACTIVATE_MODULE_OUTPUT) {
      Serial.print("DEACTIVATE_MODULE_OUTPUT");
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

    if (message.message == ANNOUNCE_ACTIVE) {
      handleMessageActive (message);
    }
    
    else if (message.message == ANNOUNCE_GESTURE_SWING) {
      handleMessageSwing (message);
    } else if (message.message == ANNOUNCE_GESTURE_SHAKE) {
      handleMessageShake (message);
    } else if (message.message == ANNOUNCE_GESTURE_TAP) { // From a module that just recognized a "tap" gesture.
      handleMessageTap (message);
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP) { // From a module that just recognized a "tap" gesture.
      handleMessageRequestConfirmTap (message);
    } else if (message.message == CONFIRM_GESTURE_TAP) { // From a module that just recognized a "tap" gesture.
      handleMessageConfirmTap (message);
    }
    
    else if (message.message == ANNOUNCE_GESTURE_TAP_AS_LEFT) { // From "left" module. "Left" module announces that it was tapped to another module as the left module [Sequence: Sequencing request (i.e., linking) confirmation, from "right" module]
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
  
  currentTime = millis();
  if (currentTime - lastMessageSendTime > RADIOBLOCK_PACKET_WRITE_TIMEOUT) {
  
    // Process mesh message queue  
    if (messageQueueSize > 0) {
      sendMessage();
    }
    
    // Update the time that a message was most-recently dispatched
    lastMessageSendTime = millis();
  }
}
