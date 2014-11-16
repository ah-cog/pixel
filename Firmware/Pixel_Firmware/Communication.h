#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define MESH_SERIAL Serial1

boolean setupCommunication () {
  MESH_SERIAL.begin (9600);
}

#define SERIAL_BUFFER_LIMIT 64
int serialBufferSize = 0;
char serialBuffer[SERIAL_BUFFER_LIMIT];

unsigned long broadcastTimeout = 1000L;
unsigned long lastBroadcastTime = -1000L - 1; // 0L;

#define NEIGHBOR_LIMIT 10
int neighborCount = 0;
int neighbors[NEIGHBOR_LIMIT];

#define BROADCAST_ADDRESS 0xFFFF

//#define RADIOBLOCK_PACKET_READ_TIMEOUT 100 // 40 // 100
//#define PAYLOAD_START_INDEX 5 // Index of the first byte in the payload
#define PACKET_WRITE_TIMEOUT 0 // 120 // 200

#define SEQUENCE_REQUEST_TIMEOUT 1000

//! Mesh message structure
//!
struct Message {
  int source; // The address of the message's sender
  int message; // The received message's data payload
};

// Mesh incoming message queue
#define MESH_INCOMING_QUEUE_CAPACITY 20
//unsigned short int meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY] = { 0 };
Message meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY];
int incomingMessageQueueSize = 0;

// Mesh outgoing message queue
#define MESSAGE_QUEUE_CAPACITY 80
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



//
// Handle messages received over mesh networks
//

#define ANNOUNCE_POOF 30 // i.e., "Poof" into existence.
//#define SAY_POOF 30 // i.e., "Poof" into existence.
#define ANNOUNCE_FOUNDATION 31 // Introduce self (i.e., the "foundation's" communication hardware to neighbors in mesh)
// ANNOUNCE_CONNECTING
#define ANNOUNCE_CONNECTED 32 // 
#define ANNOUNCE_ACTIVE 33 // Message to periodically notify neighbors of the device's foundation (i.e., it is physically present on a device with a physical address and can be communited to)
// ANNOUNCE_DISCONNECTING
#define ANNOUNCE_DISCONNECTED 34
//#define ANNOUNCE_OFF 34

// TODO: Write code to periodically notify neighbors of device's existence. This is used by neighbors to update their tables of neighbors (and their connections).
// TODO: When priodically announcing existence, if encounter an address collision, check UUIDs and re-negotiate an IP address.

#define ANNOUNCE_GESTURE_AT_REST 1
//#define ANNOUNCE_GESTURE_AT_REST_ON_TABLE 1
//#define ANNOUNCE_GESTURE_AT_REST_IN_HAND 2
//#define ANNOUNCE_GESTURE_PICK_UP 3
//#define ANNOUNCE_GESTURE_PLACE_DOWN 4
#define ANNOUNCE_GESTURE_SWING 2 // TODO: When received, place module in "respond to swung module" mode

#define ANNOUNCE_GESTURE_TAP 23 // 9
#define REQUEST_CONFIRM_GESTURE_TAP 24 // 9
#define CONFIRM_GESTURE_TAP 22 // 9

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

// Module state:
boolean awaitingNextModule = false;
boolean awaitingPreviousModule = false;
boolean awaitingNextModuleConfirm = false;
boolean awaitingPreviousModuleConfirm = false;
unsigned long awaitingNextModuleStartTime = 0;
unsigned long awaitingPreviousModuleStartTime = 0;

// Sequence state:
boolean isSequenced = false; // Does the module belong to a sequence?
boolean isActive = false; // Is the module the currently active module in the sequence

boolean outputPinRemote = false; // Flag indicating whether the output port is on this module or another module

boolean hasSwung = false;
int lastSwingAddress = -1; // TODO: Rename to lastReceivedSwingSource
unsigned long lastReceivedSwingTime = 0L;
unsigned long lastReceivedSwingTimeout = 5000;

// TODO: Move this into Looper engine so can edit in web browser. Possibly store it in EEPROM (as well as in cloud).
unsigned long lastSentActive = 0L;
unsigned long lastSentActiveTimeout = 30000L; // Broadcast heartbeat every 30 seconds

/**
 * Adds the specified module as a previous module if it hasn't already been added.
 */
boolean Add_Input_Module (int module) {
  
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
boolean Remove_Input_Module (int module) {
  
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
boolean Remove_Input_Modules () {
  previousModuleCount = 0;
}

/**
 * Checks if the specified module is in the set of previous modules.
 */
boolean Has_Previous_Module (int module) {
  
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

int Get_Input_Module_Count () {
  return previousModuleCount;
}

/**
 * Adds the specified module as a next module if it hasn't already been added.
 */
boolean Add_Output_Module (int module) {
  
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
boolean Remove_Output_Module (int module) {
  
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
boolean Remove_Output_Modules () {
  nextModuleCount = 0;
}

/**
 * Checks if the specified module is in the set of next modules.
 */
boolean Has_Output_Module (int module) {
  
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

int Get_Output_Module_Count () {
  return nextModuleCount;
}



//----------

//! Push a message onto the queue of messages to be processed and sent via the mesh network.
//!
boolean Queue_Incoming_Mesh_Message (int source, int message) {
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
Message Dequeue_Incoming_Mesh_Message () {
  
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

//! Push a message onto the queue of messages to be processed and sent via the mesh network.
//!
boolean Queue_Broadcast (int message) {
  
  if (messageQueueSize < MESSAGE_QUEUE_CAPACITY) { // Check if message queue is full (if so, don't add the message)
    //messageQueue[messageQueueSize] = message; // Add message to the back of the queue
    messageQueue[messageQueueSize].source = BROADCAST_ADDRESS; // Set "broadcast address"
    messageQueue[messageQueueSize].message = message; // Add message to the back of the queue
    messageQueueSize++; // Increment the message count
    
    return true;
  }
  
  return false;
}

//! Push a message onto the queue of messages to be processed and sent via the mesh network.
//!
boolean Queue_Message (int source, int message) {
  if (messageQueueSize < MESSAGE_QUEUE_CAPACITY) { // Check if message queue is full (if so, don't add the message)  
    //messageQueue[messageQueueSize] = message; // Add message to the back of the queue
    messageQueue[messageQueueSize].source = source;
    messageQueue[messageQueueSize].message = message; // Add message to the back of the queue
    messageQueueSize++; // Increment the message count
  }
}

//! Dequeues outgoing message.
//!
Message Dequeue_Outgoing_Message () {
  
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

//! Sends the top message on the mesh's message queue.
//!
boolean Send_Message () {
  if (messageQueueSize > 0) {
    
//    // Get the next message from the front of the queue
//    Message message = dequeueOutgoingMessage ();
    
    //
    // Actually send the message
    //
    
//    if (isReading == false) {
//        isWriting = true;
        
//        Serial.println ("sendMessage");
        
        // Get the next message from the front of the queue
        Message message = Dequeue_Outgoing_Message ();
      
        // MESH_SERIAL.write ('!');
//        String data = String ("{ uuid: ") + String (platformUuid) + String (" , type: 'keep-alive' }");
//        const int serialBufferSize = 64;
//        char charData[serialBufferSize];
//        data.toCharArray (charData, serialBufferSize);
        
        int bytesSent = 0;
//        String data = String ("{ to: ") + String (message.source) + String (", from: ") + String (platformUuid) + String (" , data: ") + String (message.message) + String (" }");
        String data = String ("(") + String (message.source) + String (", ") + String (platformUuid) + String (", ") + String (message.message) + String (")");
        const int serialBufferSize = 64;
        char charData[serialBufferSize];
        data.toCharArray (charData, serialBufferSize);
        bytesSent = MESH_SERIAL.write (charData);
        
//        Serial.println (charData);
        
//        int bytesSent = MESH_SERIAL.write (charData);
//        Serial.print ("sent "); Serial.print (bytesSent); Serial.print (" bytes\n\n");
        
        lastBroadcastTime = millis ();
        
//        if (bytesSent >= data.length ()) {
//          isWriting = false;
//        }
//      }
  }
}

// TODO: Collect, Distribute, Scatter, Saturate

//! Collects any available messages on the mesh.
//!
boolean Collect_Mesh_Messages () {
  
//  Serial.println ("Collect_Mesh_Messages");
  
  // Receive any data received over the mesh network.
//  if (isWriting == false) {
    if (MESH_SERIAL.available () > 0) {
//      isReading = true;
      
      int incomingByte = MESH_SERIAL.read ();
      // Serial.print("UART received: ");
//    Serial.print ((char) incomingByte);
      
//      if (incomingByte == '}') {
      if (incomingByte == ')') {
        
        // Terminate the buffer
        serialBuffer[serialBufferSize] = incomingByte;
        serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
        serialBuffer[serialBufferSize] = '\0';
        
        // TODO: Terminate the buffer and return it for parsing!
        String jsonMessage = String (serialBuffer).substring (1, serialBufferSize - 1);
//        Serial.println (jsonMessage);
//        int count = getValueCount (jsonMessage, ' ');
//        int fromParameter = getValue (jsonMessage, ' ', 2).toInt ();
//        int toParameter = getValue (jsonMessage, ' ', 4).toInt ();
//        String dataParameter = getValue (jsonMessage, ' ', 7);

        // Parse tuple encoded as follows "(<to>, <from>, <message>)".
        int toParameter = getValue (jsonMessage, ' ', 0).toInt ();
        int fromParameter = getValue (jsonMessage, ' ', 1).toInt ();
        String dataParameter = getValue (jsonMessage, ' ', 2);
        
        // Add the module from which the message was received to the list of neighbors if it's not there.
        boolean hasNeighbor = false;
        for (int i = 0; i < neighborCount; i++) {
          if (neighbors[i] == fromParameter) {
            hasNeighbor = true;
            break;
          }
        }
        if (hasNeighbor == false) {
          neighbors[neighborCount] = fromParameter;
          neighborCount++;
          Serial.print ("Added neighbor "); Serial.print (neighborCount); Serial.print (": "); Serial.print (fromParameter); Serial.print ("\n");
        }
        
        // TODO: Check timestamps when last received a broadcast, and ping those not reached for a long time, and remove them if needed.
        
        // Add incoming data to the incoming data queue (i.e., messages to be processed)
        int dataParam = dataParameter.toInt ();
        Serial.print (">> (from: "); Serial.print (fromParameter); Serial.print (", ");
        Serial.print ("to: "); Serial.print (toParameter); Serial.print (", ");
        Serial.print ("data: "); Serial.print (dataParam); Serial.print (")\n\n");
        Queue_Incoming_Mesh_Message (fromParameter, dataParam);
        
        serialBufferSize = 0;
        
        return true;
        
//        Serial.println (neighborUuid);
        
      } else {
        
        serialBuffer[serialBufferSize] = incomingByte;
        serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
        
        return false;
        
      }
      // Serial2.print ((char) incomingByte);
      // HWSERIAL.print("UART received:");
      // HWSERIAL.println(incomingByte, DEC);
      
      // TODO: Buffer the data received over mesh until the message is completely received.
      
//      isReading = false;
//    }
  }
  
  return false;
  
}

boolean Handle_Message_Active (Message message) {
  Serial.println (">> Received ANNOUNCE_ACTIVE");
  
  // TODO: Make sure that a "neighbor" with the source address of the message is in this module's table of neighbors!
}

boolean Handle_Message_Swing (Message message) {
  
  // TODO: Place module in "respond to swung module" mode.
  
  Serial.println (">> Received ANNOUNCE_GESTURE_SWING");
  
//  if (message.source != MESH_DEVICE_ADDRESS) {
  if (message.source != platformUuid) { // TODO: Restore this! Removed for FutureMakers demo
    
    lastSwingAddress = message.source;
    
    // Check if the module is a previous module
//    boolean hasPrevious = hasPreviousModule (message.source);
//    if (hasPrevious) {
//      setColor(255, 0, 0);
//    } else {
//      setColor(0, 255, 0);
//    }
    
    // TODO: Set lastSwingAddressStartTime and check for a timeout (after which a received "tap" message will no longer be allowed to pair)
    lastReceivedSwingTime = millis ();
    
  }
}

boolean Handle_Message_Shake (Message message) {
  
  // TODO: Place module in "respond to swung module" mode.
  
  Serial.println (">> Received ANNOUNCE_GESTURE_SHAKE");
  
  Update_Color (255, 255, 255);
  
//  if (message.source != MESH_DEVICE_ADDRESS) {
//    
//    lastSwingAddress = message.source;
//    
//    // Check if the module is a previous module
//    boolean hasPrevious = hasPreviousModule (message.source);
//    if (hasPrevious) {
//      setColor(255, 0, 0);
//    } else {
//      setColor(0, 255, 0);
//    }
//    
//    // TODO: Set lastSwingAddressStartTime and check for a timeout (after which a received "tap" message will no longer be allowed to pair)
//    lastReceivedSwingTime = millis ();
//    
//  }
}

//! Event handler for the "tap" announcement (i.e., broadcast) message.
//!
boolean Handle_Message_Tap (Message message) {
  
  // Handler code for the module that initiated the "swing"
  if (hasSwung) {
    
    // HACK: Move this! This should be more robust, likely!
    // TODO: Make this map to the other module only when it is already sequenced!
    if (outputPinRemote == true) {
      outputPinRemote = false;
//      removeNextModule(message.source);
      Remove_Input_Modules ();
      Remove_Output_Modules ();
    } else {
      outputPinRemote = true;
      Add_Output_Module (message.source);
    }
    
    Update_Color (0, 0, 0);
    
    
  
//    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_LEFT");
    
    // TODO: REMOVE THIS!!
//    addNextModule(message.source);
    
    
    
    
    // Check of the received message was within the time limit
    
    Serial.println (">> Received ANNOUNCE_GESTURE_TAP");
    
    // Send ACK message to message.source to confirm linking operation
    Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP);
  }
  
}

//! Event handler for the "request confirm tap" message.
//!
boolean Handle_Message_Request_Confirm_Tap (Message message) {
  
  Add_Input_Module (message.source);
    // TODO: addPreviousModule(message.source, SEQUENCE_ID);
  
//  if (hasSwung) { // if this is the module that was swung
    // TODO: 
    Serial.println (">> Received REQUEST_CONFIRM_GESTURE_TAP");
    Queue_Message (message.source, CONFIRM_GESTURE_TAP);
//  }
  
}

//! Event handler for the "confirm tap" message.
//!
boolean Handle_Message_Confirm_Tap (Message message) {
  
  if (hasSwung) { // if this is the module that was swung
    // TODO: 
//    addMessage (message.source, CONFIRM_GESTURE_TAP);

    // TODO: Set the module as "next"
    Serial.println (">> Received CONFIRM_GESTURE_TAP");
    
    Stop_Blink_Light ();
    hasSwung = false;
  }
  
}

/**
 * "Right" module handle "tap to another, as left" message.
 */
boolean Handle_Message_Tap_To_Another_As_Left (Message message) {
    if (awaitingPreviousModule) {
      // Update message state
      awaitingPreviousModule = false;
      awaitingPreviousModuleConfirm = true;
      
//      Serial.println(">> Received ANNOUNCE_GESTURE_TAP_AS_LEFT");
      
      // Send ACK message to message.source to confirm linking operation
//      addMessage(message.source, REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
      Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
      
//      Serial.println("<< Sending REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT");
    }
}

/**
 * "Left" module handle request for confirmation of "tap to another, as left" message.
 */
boolean Handle_Message_Request_Confirm_Tap_To_Another_As_Left (Message message) {
        
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
    Queue_Message (message.source, CONFIRM_GESTURE_TAP_AS_LEFT);
    
    // HACK: Move this! This should be more robust, likely!
    // TODO: Make this map to the other module only when it is already sequenced!
    outputPinRemote = true;
    
    
  
//    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_LEFT");
    
    Add_Output_Module (message.source);
    // TODO: addNextModule(message.source, SEQUENCE_ID);
    
    // Add module to sequence
    isSequenced = true;
    setSequenceColor(0, 255, 0); // Set the color of the sequence
    
    // Update the module's color
    if (isSequenced) {
      Update_Color (sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    } else {
      Update_Color (defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
    }
    //setModuleColor(255, 255, 255);
    //setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    
//    addBroadcast(CONFIRM_GESTURE_TAP_AS_LEFT);
    Queue_Message (message.source, CONFIRM_GESTURE_TAP_AS_LEFT);

    Serial.println("<< Sending ");
  }
}

/**
 * "Right" module handle confirmation of "tap to another, as left" message.
 */
boolean Handle_Message_Confirm_Tap_To_Another_As_Left (Message message) {
  Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_LEFT");
}

/**
 * "Left" module handle "tap to another, as right" message.
 */
boolean Handle_Message_Tap_To_Another_As_Right (Message message) {
  // NOTE: Received by the "left" module from the "right" module
  
  // If receive "tap to another, as left", then check if this module performed "tap to another, as right" recently. If so, link the modules in a sequence, starting with the other module first.
  
  if (awaitingNextModule) {
    
//    Serial.println(">> Received ANNOUNCE_GESTURE_TAP_AS_RIGHT");
    
    // Update message state
    awaitingNextModule = false;
    awaitingNextModuleConfirm = true;
    
    // Send ACK message to message.source to confirm linking operation
    Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
//    addMessage(message.source, REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
    
//    Serial.println("<< Sending REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT");
  }
}

/**
 * "Right" module handle request for confirmation of "tap to another, as right" message.
 */
boolean Handle_Message_Request_Confirm_Tap_To_Another_As_Right (Message message) {
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
    
    Add_Input_Module (message.source);
    // TODO: addPreviousModule(message.source, SEQUENCE_ID);
    
    // Add module to sequence
    isSequenced = true;
    setSequenceColor(0, 255, 0); // Set the color of the sequence
    
    // Update the module's color
    if (isSequenced) {
      Update_Color (sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    } else {
      Update_Color (defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
    }
    //setModuleColor(255, 255, 255);
    //setColor(sequenceColor[0], sequenceColor[1], sequenceColor[2]);
    
    
//    addBroadcast(CONFIRM_GESTURE_TAP_AS_RIGHT);
    Queue_Message (message.source, CONFIRM_GESTURE_TAP_AS_RIGHT);

    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_RIGHT");
  }
}

/**
 * "Left" module handle confirmation of "tap to another, as left" message.
 */
boolean Handle_Message_Confirm_Tap_To_Another_As_Right (Message message) {
  Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_RIGHT");
  
  // TODO: Sequence the modules!
}

#endif
