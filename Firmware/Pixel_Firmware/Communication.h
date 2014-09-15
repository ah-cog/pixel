#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define MESH_DEVICE_ADDRESS 0x0002 // The device of the mesh networking radio

#if defined(MESH_DEVICE_ADDRESS)
  #if MESH_DEVICE_ADDRESS == 0x0000
    #define NEIGHBOR_ADDRESS 0x0001
  #elif MESH_DEVICE_ADDRESS == 0x0001
    #define NEIGHBOR_ADDRESS 0x0002
  #elif MESH_DEVICE_ADDRESS == 0x0002
    #define NEIGHBOR_ADDRESS 0x0001
  #elif MESH_DEVICE_ADDRESS == 0x0003
    #define NEIGHBOR_ADDRESS 0x0001
  #endif
#endif

#define BROADCAST_ADDRESS NEIGHBOR_ADDRESS // 0xFFFF
 
#define RADIOBLOCK_POWER_PIN -1
#define RADIOBLOCK_GROUND_PIN -1
#define RADIOBLOCK_RX_PIN 8
#define RADIOBLOCK_TX_PIN 7

/**
 * RadioBlocks Setup
 */

#define RADIOBLOCK_PACKET_READ_TIMEOUT 100 // 40 // 100
#define PAYLOAD_START_INDEX 5 // Index of the first byte in the payload
#define RADIOBLOCK_PACKET_WRITE_TIMEOUT 0 // 120 // 200

#define SEQUENCE_REQUEST_TIMEOUT 1000

// The module's pins 1, 2, 3, and 4 are connected to pins 5V, GND, 8, and 7.
RadioBlockSerialInterface interface = RadioBlockSerialInterface(RADIOBLOCK_POWER_PIN, RADIOBLOCK_GROUND_PIN, RADIOBLOCK_RX_PIN, RADIOBLOCK_TX_PIN);

/**
 * Initialize mesh networking peripheral.
 */
boolean setupCommunication() {
  
//  delay(1000);
  
  interface.begin();
  
  // Give RadioBlock time to init
  delay(500);
  
  // Flash the LED a few times to tell that the module is live
  interface.setLED(true);  delay(200);
  interface.setLED(false); delay(200);
  interface.setLED(true);  delay(200);
  interface.setLED(false); delay(200);
  interface.setLED(true);  delay(200);
  interface.setLED(false);
  
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  interface.setAddress(MESH_DEVICE_ADDRESS);
}

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

struct Message {
  int source;
  int message;
};

// Mesh incoming message queue
#define MESH_INCOMING_QUEUE_CAPACITY 20
//unsigned short int meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY] = { 0 };
Message meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY];
int incomingMessageQueueSize = 0;

// Mesh outgoing message queue
#define MESSAGE_QUEUE_CAPACITY 20
Message messageQueue[MESSAGE_QUEUE_CAPACITY];
int messageQueueSize = 0;
unsigned long lastMessageSendTime = 0;

// Previous modules in sequence
#define PREVIOUS_MODULE_CAPACITY 20
unsigned short int previousModules[PREVIOUS_MODULE_CAPACITY] = { 0 };
int previousModuleCount = 0;

// Next modules in sequence
#define NEXT_MODULE_CAPACITY 20
unsigned short int nextModules[NEXT_MODULE_CAPACITY] = { 0 };
int nextModuleCount = 0;

/**
 * Adds the specified module as a previous module if it hasn't already been added.
 */
boolean addPreviousModule(int module) {
  
  if (previousModuleCount < PREVIOUS_MODULE_CAPACITY) {
    
    // Check if the module has already been added to the set
    for (int i = 0; i < previousModuleCount; i++) {
      if (module == previousModules[i]) {
        return false;
      }
    }
    
    // Add the module to the set of previous modules
    previousModules[previousModuleCount] = module; // Add module to the set
    previousModuleCount++; // Increment the previous module count
    
    return true;
  }
  
  return false;
}

/**
 * Removes the specified module from the set of previous modules if it's in the set.
 */
boolean removePreviousModule(int module) {
  
  if (previousModuleCount > 0) {
    
    // Check if the module has already been added to the set
    for (int i = 0; i < previousModuleCount; i++) {
      if (module == previousModules[i]) {
        
        // Remove the module from the set
        for (int j = i; j < previousModuleCount - 1; j++) {
          previousModules[j] = previousModules[j + 1];
        }
        previousModuleCount--;
        
        return true;
      }
    }
  }

  return false;
}

/**
 * Removes all previous modules
 */
boolean removePreviousModules() {
  previousModuleCount = 0;
}

/**
 * Checks if the specified module is in the set of previous modules.
 */
boolean hasPreviousModule(int module) {
  
  if (previousModuleCount > 0) {
    
    // Check if the module has been added to the set
    for (int i = 0; i < previousModuleCount; i++) {
      if (module == previousModules[i]) {
        // The module has been found, so return true
        return true;
      }
    }
  }

  // The module was not found, so return false
  return false;
}

int getPreviousModuleCount() {
  return previousModuleCount;
}

/**
 * Adds the specified module as a next module if it hasn't already been added.
 */
boolean addNextModule (int module) {
  
  if (nextModuleCount < NEXT_MODULE_CAPACITY) {
    
    // Check if the module has already been added to the set
    for (int i = 0; i < nextModuleCount; i++) {
      if (module == nextModules[i]) {
        return false;
      }
    }
    
    // Add the module to the set of next modules
    nextModules[nextModuleCount] = module; // Add module to the set
    nextModuleCount++; // Increment the next module count
    
    return true;
  }
  
  return false;
}

/**
 * Removes the specified module from the set of next modules if it's in the set.
 */
boolean removeNextModule(int module) {
  
  if (nextModuleCount > 0) {
    
    // Check if the module has already been added to the set
    for (int i = 0; i < nextModuleCount; i++) {
      if (module == nextModules[i]) {
        
        // Remove the module from the set
        for (int j = i; j < nextModuleCount - 1; j++) {
          nextModules[j] = nextModules[j + 1];
        }
        nextModuleCount--;
        
        return true;
      }
    }
  }

  return false;
}

/**
 * Removes all next modules
 */
boolean removeNextModules() {
  nextModuleCount = 0;
}

/**
 * Checks if the specified module is in the set of next modules.
 */
boolean hasNextModule(int module) {
  
  if (nextModuleCount > 0) {
    
    // Check if the module has been added to the set
    for (int i = 0; i < nextModuleCount; i++) {
      if (module == nextModules[i]) {
        // The module has been found, so return true
        return true;
      }
    }
  }

  // The module was not found, so return false
  return false;
}

int getNextModuleCount() {
  return nextModuleCount;
}



//----------




/**
 * Push a message onto the queue of messages to be processed and sent via the mesh network.
 */
boolean queueIncomingMeshMessage(int source, int message) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (incomingMessageQueueSize < MESH_INCOMING_QUEUE_CAPACITY) {
    // Add message to queue
    meshIncomingMessages[incomingMessageQueueSize].source = source;
    meshIncomingMessages[incomingMessageQueueSize].message = message; // Add message to the back of the queue
    incomingMessageQueueSize++; // Increment the message count
  }
  
//  Serial.print("queueing message (size: ");
//  Serial.print(messageQueueSize);
//  Serial.print(")\n");
}

/**
 * Sends the top message on the mesh's message queue.
 */
Message dequeueIncomingMeshMessage() {
  
  if (incomingMessageQueueSize > 0) {
    
    // Get the next message from the front of the queue
    //unsigned short int message = meshIncomingMessages[0].message; // Get message on front of queue
    Message message = { meshIncomingMessages[0].source, meshIncomingMessages[0].message }; // Get message on front of queue
    incomingMessageQueueSize--;
    
    // Shift the remaining messages forward one position in the queue
    for (int i = 0; i < MESH_INCOMING_QUEUE_CAPACITY - 1; i++) {
      meshIncomingMessages[i].source = meshIncomingMessages[i + 1].source;
      meshIncomingMessages[i].message = meshIncomingMessages[i + 1].message;
    }
    meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY - 1].source = -1; // Set last message to "noop"
    meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY - 1].message = -1; // Set last message to "noop"
    
    return message;
  }
}

/**
 * Push a message onto the queue of messages to be processed and sent via the mesh network.
 */
boolean addBroadcast(int message) {
  if (messageQueueSize < MESSAGE_QUEUE_CAPACITY) { // Check if message queue is full (if so, don't add the message)
    //messageQueue[messageQueueSize] = message; // Add message to the back of the queue
    messageQueue[messageQueueSize].source = BROADCAST_ADDRESS; // Set "broadcast address"
    messageQueue[messageQueueSize].message = message; // Add message to the back of the queue
    messageQueueSize++; // Increment the message count
  }
}

/**
 * Push a message onto the queue of messages to be processed and sent via the mesh network.
 */
boolean addMessage(int source, int message) {
  if (messageQueueSize < MESSAGE_QUEUE_CAPACITY) { // Check if message queue is full (if so, don't add the message)  
    //messageQueue[messageQueueSize] = message; // Add message to the back of the queue
    messageQueue[messageQueueSize].source = source;
    messageQueue[messageQueueSize].message = message; // Add message to the back of the queue
    messageQueueSize++; // Increment the message count
  }
}

Message dequeueOutgoingMessage() {
  
  if (messageQueueSize > 0) {
    
    // Get the next message from the front of the queue
    //unsigned short int message = meshIncomingMessages[0].message; // Get message on front of queue
    Message message = { messageQueue[0].source, messageQueue[0].message }; // Get message on front of queue
    messageQueueSize--;
    
    // Shift the remaining messages forward one position in the queue
    for (int i = 0; i < MESSAGE_QUEUE_CAPACITY - 1; i++) {
      messageQueue[i].source = messageQueue[i + 1].source;
      messageQueue[i].message = messageQueue[i + 1].message;
    }
    messageQueue[MESSAGE_QUEUE_CAPACITY - 1].source = -1; // Set last message to "noop"
    messageQueue[MESSAGE_QUEUE_CAPACITY - 1].message = -1; // Set last message to "noop"
    
    return message;
  }
}

/**
 * Sends the top message on the mesh's message queue.
 */
boolean sendMessage() {
  if (messageQueueSize > 0) {
    
    // Get the next message from the front of the queue
    Message message = dequeueOutgoingMessage();
    
    //
    // Actually send the message
    //
    
    // Set up the destination address of the message
    if (message.source == BROADCAST_ADDRESS) {
      // TODO: Broadcast here, not just send to "neighbor"... might need to iterat through the addresses manually!
      interface.setupMessage(NEIGHBOR_ADDRESS);
    } else {
      interface.setupMessage(message.source); // Set the address of the recipient
    }
    
    Serial.print("Sending message: ");
    Serial.print(message.message);
    Serial.print(" to ");
    Serial.print(message.source);
    Serial.print("\n");
            
    // Package the data payload for transmission
    interface.addData(1, (unsigned char) message.message); // TYPE_UINT8

    // e.g., How to assemble data packets for sending with RadioBlocks
    // interface.addData(1, (unsigned char) 0x13); // TYPE_UINT8
    // interface.addData(1, (char) 0x14); // TYPE_INT8
    // interface.addData(3, (unsigned short int) 0xFFFD); // TYPE_UINT16
    // interface.addData(1, (short) 0xABCD); // TYPE_INT16
    // interface.addData(14, (unsigned long) 0xDDDDCCAA); // TYPE_UINT32
    // interface.addData(9, (long) 0xFF03CCAA); // TYPE_INT32
     
    // Send data OTA (over the air)
    interface.sendMessage();
  }
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
      // Serial.println("\n\n\n\nReceived packet.");
      
      int commandId = interface.getResponse().getCommandId();
      
      if (commandId == APP_COMMAND_ACK) {
        // "Every command is confirmed with an acknowledgment command even if it is impossible
        //   to immediately execute the command. There is no particular order in which responses 
        //   are sent, so for example Data Indication Command might be sent before 
        //   Acknowledgment Command."
  
        // Acknowledgment command format: Page 5 of SimpleMesh Serial Protocol document.
  
        // Serial.println("Received Ack");
        // Serial.print("  Status: ");
        // Serial.println(interface.getResponse().getFrameData()[0], HEX); // Source address

      } else if (commandId == APP_COMMAND_DATA_IND) { // (i.e., 0x22) [Page 15]
      
//        // Queue incoming message!
//        queueIncomingMeshMessage(0, 8);
  
//        Serial.print(interface.getResponse().getFrameData()[0], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[1], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[2], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[3], HEX); // Frame options
//        Serial.print(" | ");
//        Serial.print(interface.getResponse().getFrameData()[4], HEX); // Frame options

        int frameDataLength = interface.getResponse().getFrameDataLength();
        //        Serial.print("  Payload length: ");
        //        Serial.println(frameDataLength, DEC); // "Payload" length (minus "Command Id" (1 byte))

        //        Serial.print("  Frame Command Id: ");
        //        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options

        // Compute source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
        short sourceAddress = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
//        Serial.print("  Source address: ");
//        Serial.println(sourceAddress, HEX); // Source address

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

//                        Serial.println("   Data type is TYPE_UINT8. High and low bytes:");
//                        Serial.print("    High part: ");
//                        Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);

                        // Append message to message queue
//                        messageQueue = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1];
                        int newMessage = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1];

                        // Queue incoming message!
                        queueIncomingMeshMessage(sourceAddress, newMessage);

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
        
//          Serial.println("APP_COMMAND_DATA_CONF");
          
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
    
    int errorCode = interface.getResponse().getErrorCode();
    
    Serial.print("ERROR! (Code ");
    //  APP_STATUS_SUCESS               = 0x00, // Success
    //  // Unknown error
    //  // Out Of Memory
    //  // No Acknowledgment Was Received
    //  // Channel Access Failure
    //  // No Physical Acknowledgment Was Received
    //  APP_STATUS_INVALID_SIZE         = 0x80, // Invalid Command Size
    //  APP_STATUS_INVALID_CRC          = 0x81, // Invalid CRC
    //  APP_STATUS_TIMEOUT              = 0x82, // Timeout
    //  APP_STATUS_UNKNOWN_COMMAND      = 0x83, // Unknown Command
    //  APP_STATUS_MALFORMED_COMMAND    = 0x84, // Malformed Command
    //  APP_STATUS_FLASH_ERROR          = 0x85, // Internal Flash Error
    //  APP_STATUS_INVALID_PAYLOAD_SIZE = 0x86, // Invalid Data Request Payload Size
    Serial.print(errorCode);
    Serial.print(")\n");
    return false;
    
  } else { // Timeout or error occurred
  
    //Serial.println("UNKNOWN ERROR!");
//    return true; // TODO: Add a third state other than true or false
    return false;

  }
}

//
// Handle messages received over mesh networks
//

#define ANNOUNCE_GESTURE_AT_REST 1
//#define ANNOUNCE_GESTURE_AT_REST_ON_TABLE 1
//#define ANNOUNCE_GESTURE_AT_REST_IN_HAND 2
//#define ANNOUNCE_GESTURE_PICK_UP 3
//#define ANNOUNCE_GESTURE_PLACE_DOWN 4
#define ANNOUNCE_GESTURE_SWING 2

#define ANNOUNCE_GESTURE_TAP_AS_LEFT 3 // 8
#define ANNOUNCE_GESTURE_TAP_AS_RIGHT 4 // 9

#define ANNOUNCE_GESTURE_SHAKE 5 // 7

#define ANNOUNCE_GESTURE_TILT_LEFT 6 // 5
#define ANNOUNCE_GESTURE_TILT_RIGHT 7 // 6
#define ANNOUNCE_GESTURE_TILT_FORWARD 8
#define ANNOUNCE_GESTURE_TILT_BACKWARD 9

#define REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT 13
#define REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT 14

#define CONFIRM_GESTURE_TAP_AS_LEFT 15
#define CONFIRM_GESTURE_TAP_AS_RIGHT 16

boolean awaitingNextModule = false;
boolean awaitingPreviousModule = false;
boolean awaitingNextModuleConfirm = false;
boolean awaitingPreviousModuleConfirm = false;
unsigned long awaitingNextModuleStartTime = 0;
unsigned long awaitingPreviousModuleStartTime = 0;

// Sequence:
boolean isSequenced = false; // Does the module belong to a sequence?
boolean isActive = false; // Is the module the currently active module in the sequence

boolean outputPinRemote = false; // Flag indicating whether the output port is on this module or another module

/**
 * "Right" module handle "tap to another, as left" message.
 */
boolean handleMessageTapToAnotherAsLeft (Message message) {
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

#endif
