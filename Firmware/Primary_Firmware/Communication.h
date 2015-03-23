#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define MESH_SERIAL Serial1

boolean Setup_Communication () {
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
unsigned long neighborAge[NEIGHBOR_LIMIT]; // i.e., the time last received a message from the neighbor

#define BROADCAST_ADDRESS 0xFFFF

//#define RADIOBLOCK_PACKET_READ_TIMEOUT 100 // 40 // 100
//#define PAYLOAD_START_INDEX 5 // Index of the first byte in the payload
#define PACKET_WRITE_TIMEOUT 0 // 120 // 200

#define SEQUENCE_REQUEST_TIMEOUT 1000

unsigned long lastMessageSendTime = 0; // Time at which the previously-sent message was sent.

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

// TODO: Dynamically adjust frequency, depending on the module's use
unsigned long lastSentActiveTimeout = 1000L; // 30000L; // Broadcast heartbeat every 30 seconds

//! Message (i.e., Message) to propagate.
//!
//! TODO: Rename to Transformation
struct Message {
  char* content;
  int size;
  
  int source;
  int destination;
  int channel; // what could this do? allow multiple channels that can be themselves programmed? like, they can be programmed to stretch time or warm signals?
  
  Message* previous;
  Message* next;
};

Message* outgoingMessageQueue = NULL; // outgoing message queue
Message* incomingMessageQueue = NULL; // incoming message queue

//! Create message (i.e., a behavior transformation).
//!
Message* Create_Message (int source, int destination, String content) {
  
//  Serial.println ("Create_Message");
  
  // Create substrate
  Message* message = (Message*) malloc (sizeof (Message));
  
  // Source
  (*message).source = source;
  
  // Destination
  (*message).destination = destination;
  
  // Initialize message
  (*message).size = content.length (); // NOTE: Add 1 to the length to account for the '\0' character.
  (*message).content = (char*) malloc (((*message).size + 1) * sizeof (char));
  content.toCharArray ((*message).content, ((*message).size + 1));
  
  (*message).previous = NULL;
  (*message).next = NULL;
  
//  Serial.print ("Initialized Message "); Serial.print ((int) message); Serial.print ("\n");
  
  // Return sequence
  return message;
  
}

//! Frees the message from dynamic memory.
//!
boolean Delete_Message (Message* message) {
  
//  Serial.println ("Delete_Message");
  
  if (message != NULL) {
    
    // Free the data payload
    if ((*message).content != NULL) {
      free ((*message).content);
    }
    
    // Free the message object
    free (message);
    
  }
  
}

//! Queue the outgoing message.
//!
Message* Queue_Outgoing_Message (Message* message) {
  
//  Serial.println ("Queue_Outgoing_Message");
  
  if (outgoingMessageQueue == NULL) {
    
    // Push to the top of the stack (as the first element)
    outgoingMessageQueue = message;
    
    // Set up the forward and back links
    (*message).previous = NULL;
    (*message).next     = NULL;
    
  } else {
    
    // Get the last message in the queue
    Message* lastMessage = outgoingMessageQueue;
    while ((*lastMessage).next != NULL) {
      Serial.print ("\tnext");
      lastMessage = (*lastMessage).next;
    }
    
    // Push to the last position in the queue
    // i.e., add to [ m0, m1, ..., mn ] at m(n + 1) as in [ m0, m1, ..., mn, m(n + 1) ]
    (*lastMessage).next = message;
    
    // Set up the backward and forward queue links
    (*message).previous = lastMessage;
    (*message).next = NULL;
    
  }
  
  // Return message
  return message;
  
}

//! Dequeue the next outgoing message.
//!
Message* Dequeue_Outgoing_Message () {
  
//  Serial.println ("Dequeue_Outgoing_Message");
  
  Message* message = NULL;
  
  if (outgoingMessageQueue != NULL) {
    
    // Get the transformation at the front of the propagator's queue
    message = outgoingMessageQueue;
    
    // Update the message queue. Set the message following the dequeued message as the front of the queue.
    outgoingMessageQueue = (*message).next;
    
    // Dissociate the dequeued message. Update the backward and forward links of the dequeued message.
    (*message).next = NULL;
    (*message).previous = NULL;
    
  }
  
  return message;
  
}

//! Queue the incoming message.
//!
Message* Queue_Incoming_Message (Message* message) {
  
//  Serial.println ("Queue_Incoming_Message");
  
  if (incomingMessageQueue == NULL) {
    
    // Push to the front of the queue (as the first message)
    incomingMessageQueue = message;
    
    // Set up the forward and back links
    (*message).previous = NULL;
    (*message).next     = NULL;
    
  } else {
    
    // Get the last message in the queue
    Message* lastMessage = incomingMessageQueue;
    
    // Push the message to the end of the queue
    while ((*lastMessage).next != NULL) {
      Serial.print ("\tnext");
      lastMessage = (*lastMessage).next;
    }
    
    // Push to the last position in the queue
    // i.e., add to [ m0, m1, ..., mn ] at m(n + 1) as in [ m0, m1, ..., mn, m(n + 1) ]
    (*lastMessage).next = message;
    
    // Set up the backward and forward queue links
    (*message).previous = lastMessage;
    (*message).next = NULL;
    
  }
  
  // Return message
  return message;
  
}

//! Dequeue the next incoming message.
//!
Message* Dequeue_Incoming_Message () {
  
//  Serial.println ("Dequeue_Incoming_Message");
  
  Message* message = NULL;
  
  if (incomingMessageQueue != NULL) {
    
    // Get the transformation at the front of the propagator's queue
    message = incomingMessageQueue;
    
    // Update the message queue. Set the message following the dequeued message as the front of the queue.
    incomingMessageQueue = (*message).next;
    
    // Dissociate the dequeued message. Update the backward and forward links of the dequeued message.
    (*message).next = NULL;
    (*message).previous = NULL;
    
  }
  
  return message;
  
}

//! Push a message onto the queue of messages to be processed and sent via the mesh network.
//!
//boolean Queue_Broadcast (String content) {
//  Serial.println ("Queue_Broadcast");
//
////  Message* message = Create_Message (BROADCAST_ADDRESS, platformUuid, content);
//  Queue_Outgoing_Message (message);
//  
//  return true;
//}

//! Push a message onto the queue of messages to be processed and sent via the mesh network.
//!
boolean Queue_Message (int source, int destination, String content) {
//  Serial.println ("Queue_Message");

  Message* message = Create_Message (source, destination, content);
  if (destination == platformUuid) {
    Queue_Incoming_Message (message);
  } else {
    Queue_Outgoing_Message (message);
  }
  
  return true;
}

//! Actually release the message on the mesh network.
//!
boolean Send_Mesh_Message () { // boolean Send_Message () {
//  Serial.println ("Release_Message");
  
  if (outgoingMessageQueue != NULL) { //! Check if there are any messages to be sent
        
    // Dequeue the next message from the front of the queue
    Message* message = Dequeue_Outgoing_Message ();
    
    // Serialize the message (i.e., encode the message for radio transmission)
    // String encodedMessage = String ("{ to: ") + String ((*message).source) + String (" , from: ") + String (platformUuid) + String (" , content: ") + String ((*message).content) + String (" }");
    String encodedMessage = String ("(") + String ((*message).source) + String (",") + String ((*message).destination) + String (",") + String ((*message).content) + String (")");

//    Serial.print ("Sending message ");
//    Serial.print (encodedMessage);
//    Serial.print ("\n");
    
    // Transmit the message
    const int serialBufferSize = 64;
    char charData[serialBufferSize];
    encodedMessage.toCharArray (charData, serialBufferSize);
    int bytesSent = MESH_SERIAL.write (charData);
    
    // Save the current time of broadcast for future reference
    lastBroadcastTime = millis ();
    
    // Delete the message now that it's been sent
    Delete_Message (message);
  }
}

// TODO: Collect, Distribute, Scatter, Saturate, Suggest (stored in a module's memory if it's existing memory is more or less consistent with the memory, kind of like a brain)

//! Captures any available messages on the mesh.
//!
boolean Receive_Mesh_Messages () {
  // Serial.println ("Capture_Messages");
  
  // Receive any data received over the mesh network.
  if (MESH_SERIAL.available () > 0) {
    
    // Serial.println ("Receive_Mesh_Messages");
    
    int incomingByte = MESH_SERIAL.read ();
  
    // Check if the message has been received in its entirety
    if (incomingByte == ')') {
      
      // Terminate the buffer
      serialBuffer[serialBufferSize] = incomingByte;
      serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
      serialBuffer[serialBufferSize] = '\0';
        
      // TODO: Terminate the buffer and return it for parsing!
      String encodedMessage = String (serialBuffer).substring (1, serialBufferSize - 1);
//      Serial.println (encodedMessage);

      // Parse tuple encoded as follows "{ <to>, <from>, <message> }".
      // TODO: Turn this into a Lisp format.
      int source      = getValue (encodedMessage, ',', 0).toInt ();
      int destination = getValue (encodedMessage, ',', 1).toInt ();
      String content  = getValue (encodedMessage, ',', 2);
        
      // Add the module from which the message was received to the list of neighbors if it's not there.
      boolean hasNeighbor = false;
      for (int i = 0; i < neighborCount; i++) {
        if (neighbors[i] == source) {
          hasNeighbor = true; // TODO: Delete this!
          neighborAge[i] = millis ();
          break;
        }
      }
      if (hasNeighbor == false) {
        neighbors[neighborCount] = source;
        neighborAge[neighborCount] = millis ();
        neighborCount++;
        Serial.print ("Added neighbor "); Serial.print (neighborCount); Serial.print (": "); Serial.print (source); Serial.print ("\n");
      }
        
      // TODO: Check timestamps when last received a broadcast, and ping those not reached for a long time, and remove them if needed.
      
      // Add incoming data to the incoming data queue (i.e., messages to be processed)
      // int dataParam = dataParameter.toInt ();
//      Serial.print (">> (source: "); Serial.print (source); Serial.print (", ");
//      Serial.print ("destination: "); Serial.print (destination); Serial.print (", ");
//      Serial.print ("content: "); Serial.print (content); Serial.print (")\n\n");
      
      Message* message = Create_Message (source, destination, content);
      Queue_Incoming_Message (message);
      
      serialBufferSize = 0;
      
      return true;
        
    } else {
      
      serialBuffer[serialBufferSize] = incomingByte;
      serialBufferSize = (serialBufferSize + 1) % SERIAL_BUFFER_LIMIT;
      
      return false;
      
    }
  }
  
  return false;
  
}

//! Adds the specified module as a previous module if it hasn't already been added.
//!
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

//! Removes the specified module from the set of previous modules if it's in the set.
//!
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

//! Removes all previous modules
//!
boolean Remove_Input_Modules () {
  previousModuleCount = 0;
}

//! Checks if the specified module is in the set of previous modules.
//!
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

//! Adds the specified module as a next module if it hasn't already been added.
//!
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

//! Removes the specified module from the set of next modules if it's in the set.
//!
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

//! Removes all next modules
//!
boolean Remove_Output_Modules () {
  nextModuleCount = 0;
}

//! Checks if the specified module is in the set of next modules.
//!
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

boolean Handle_Message_Active (Message* message) {
//  Serial.println (">> Received ANNOUNCE_ACTIVE");
  
  // TODO: Make sure that a "neighbor" with the source address of the message is in this module's table of neighbors!
}

boolean Handle_Message_Swing (Message* message) {
  
  // TODO: Place module in "respond to swung module" mode.
  
  Serial.println (">> Received Handle_Message_Swing");
  
//  if (lastSwingAddress == -1) {
  if (hasSwung) {
    
    Serial.println ("Linking");
    
    // Handler code for the module that initiated the "swing"
//    if (hasSwung) {
      
      // HACK: Move this! This should be more robust, likely!
      // TODO: Make this map to the other module only when it is already sequenced!
      if (outputPinRemote == true) {
        outputPinRemote = false;
  //      removeNextModule(message.source);
        Remove_Input_Modules ();
        Remove_Output_Modules ();
      } else {
        outputPinRemote = true;
        Add_Output_Module ((*message).source);
      }
      
//      Queue_Message (platformUuid, (*message).source, "activate output when ");
      
//      Update_Color (0, 0, 0);
      
      // TODO: Message ("change color to <link color>");
      
      Stop_Blink_Light ();
      
      int red = random (256);
      int green = random (256);
      int blue = random (256);
      Queue_Message (platformUuid, (*message).source, String ("change input color to ") + String (red) + String (" ") + String (green) + String (" ") + String (blue));
      
      Queue_Message (platformUuid, platformUuid, String ("change output color to ") + String (red) + String (" ") + String (green) + String (" ") + String (blue));
      
      
    
  //    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_LEFT");
      
      // TODO: REMOVE THIS!!
  //    addNextModule(message.source);
      
      // Check of the received message was within the time limit
      
      // Send ACK message to message.source to confirm linking operation
  //    Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP);
      // Queue_Broadcast ("request confirm gesture tap");
  //    Queue_Broadcast ("fyi tap");
//      Queue_Message (platformUuid, (*message).source, "observe notice gesture swing");
      // Queue_Broadcast (String ("send \"confirm gesture tap\" to ") + String (platformUuid));
      // Queue_Broadcast (String ("send confirmation");
//    }
    
  } 
  
  else { // i.e., not swung
    
    Serial.println ("Waiting for link");
    
    if ((*message).source != platformUuid) { // TODO: Restore this! Removed for FutureMakers demo
      
      lastSwingAddress = (*message).source;
      
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
}

boolean Handle_Message_Shake (Message* message) {
  
  // TODO: Place module in "respond to swung module" mode.
  
//  Serial.println (">> Received ANNOUNCE_GESTURE_SHAKE");
  
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
boolean Handle_Message_Tap (Message* message) {
  
  Serial.println (">> Received ANNOUNCE_GESTURE_TAP");
  
  // Handler code for the module that initiated the "swing"
  if (hasSwung) {
    
    // HACK: Move this! This should be more robust, likely!
    // TODO: Make this map to the other module only when it is already sequenced!
    if (outputPinRemote == true) {
      outputPinRemote = false;
//      removeNextModule(message.source);
      Remove_Input_Modules ();
      Remove_Output_Modules ();
      Update_Color (255, 255, 255);
      Queue_Message (platformUuid, (*message).source, "change color to white"); // Set remote color
    } else {
      outputPinRemote = true;
      Add_Output_Module ((*message).source);
      Update_Color (0, 0, 255); // Set local color
      Queue_Message (platformUuid, (*message).source, "change color to blue"); // Set remote color
    }
    
//    Update_Color (255, 255, 255);
    
    
  
//    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_LEFT");
    
    // TODO: REMOVE THIS!!
//    addNextModule(message.source);
    
    
    
    
    // Check of the received message was within the time limit
    
    Serial.println (">> Received ANNOUNCE_GESTURE_TAP");
    
    // Send ACK message to message.source to confirm linking operation
//    Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP);
    // Queue_Broadcast ("request confirm gesture tap");
//    Queue_Broadcast ("fyi tap");
    Queue_Message (platformUuid, BROADCAST_ADDRESS, "@all fyi tap");
    // Queue_Broadcast (String ("send \"confirm gesture tap\" to ") + String (platformUuid));
    // Queue_Broadcast (String ("send confirmation");
  }
  
}

//! Event handler for the "request confirm tap" message.
//!
boolean Handle_Message_Request_Confirm_Tap (Message* message) {
  
  Add_Input_Module ((*message).source);
    // TODO: addPreviousModule(message.source, SEQUENCE_ID);
  
//  if (hasSwung) { // if this is the module that was swung
    // TODO: 
    Serial.println (">> Received REQUEST_CONFIRM_GESTURE_TAP");
    //Queue_Message (message.source, CONFIRM_GESTURE_TAP);
    // TODO: Make this message send immediately!
    Queue_Message (platformUuid, (*message).source, "fyi got tap");
//  }
  
}

//! Event handler for the "confirm tap" message.
//!
boolean Handle_Message_Confirm_Tap (Message* message) {
  
  if (hasSwung) { // if this is the module that was swung
    // TODO: 
//    addMessage (message.source, CONFIRM_GESTURE_TAP);

    // TODO: Set the module as "next"
    Serial.println (">> Received CONFIRM_GESTURE_TAP");
    
    Stop_Blink_Light ();
    hasSwung = false;
  }
  
}

//! "Right" module handle "tap to another, as left" message.
//!
boolean Handle_Message_Tap_To_Another_As_Left (Message* message) {
    if (awaitingPreviousModule) {
      // Update message state
      awaitingPreviousModule = false;
      awaitingPreviousModuleConfirm = true;
      
      Serial.println(">> Received ANNOUNCE_GESTURE_TAP_AS_LEFT");
      
      // Send ACK message to message.source to confirm linking operation
//      addMessage(message.source, REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
//      Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
//      Queue_Broadcast ("fyi confirm gesture tap as left");
      Queue_Message (platformUuid, BROADCAST_ADDRESS, "@all fyi confirm gesture tap as left");
      
//      Serial.println("<< Sending REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT");
    }
}

//! "Left" module handle request for confirmation of "tap to another, as left" message.
//!
boolean Handle_Message_Request_Confirm_Tap_To_Another_As_Left (Message* message) {
        
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
//    Queue_Message (message.source, CONFIRM_GESTURE_TAP_AS_LEFT);
    Queue_Message (platformUuid, (*message).source, String ("confirm gesture tap as left"));
    
    // HACK: Move this! This should be more robust, likely!
    // TODO: Make this map to the other module only when it is already sequenced!
    outputPinRemote = true;
    
    
  
//    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_LEFT");
    
    Add_Output_Module ((*message).source);
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
//    Queue_Message (message.source, CONFIRM_GESTURE_TAP_AS_LEFT);
    Queue_Message (platformUuid, (*message).source, String ("confirm gesture tap as right"));

    Serial.println("<< Sending ");
  }
}

//! "Right" module handle confirmation of "tap to another, as left" message.
//!
boolean Handle_Message_Confirm_Tap_To_Another_As_Left (Message* message) {
  Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_LEFT");
}

//! "Left" module handle "tap to another, as right" message.
//!
boolean Handle_Message_Tap_To_Another_As_Right (Message* message) {
  // NOTE: Received by the "left" module from the "right" module
  
  // If receive "tap to another, as left", then check if this module performed "tap to another, as right" recently. If so, link the modules in a sequence, starting with the other module first.
  
  if (awaitingNextModule) {
    
    Serial.println(">> Received ANNOUNCE_GESTURE_TAP_AS_RIGHT");
    
    // Update message state
    awaitingNextModule = false;
    awaitingNextModuleConfirm = true;
    
    // Send ACK message to message.source to confirm linking operation
    // Queue_Broadcast (REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
//    Queue_Broadcast ("fyi confirm gesture tap as right");
    Queue_Message (platformUuid, BROADCAST_ADDRESS, String ("@all confirm gesture tap as right"));
//    addMessage(message.source, REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
    
//    Serial.println("<< Sending REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT");
  }
}

//! "Right" module handle request for confirmation of "tap to another, as right" message.
//!
boolean Handle_Message_Request_Confirm_Tap_To_Another_As_Right (Message* message) {
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
    
    Add_Input_Module ((*message).source);
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
//    Queue_Message (message.source, CONFIRM_GESTURE_TAP_AS_RIGHT);
    Queue_Message (platformUuid, (*message).source, String ("confirm gesture tap as right"));

    Serial.println("<< Sending CONFIRM_GESTURE_TAP_AS_RIGHT");
  }
}

//! "Left" module handle confirmation of "tap to another, as left" message.
//!
boolean Handle_Message_Confirm_Tap_To_Another_As_Right (Message* message) {
  Serial.println(">> Receiving CONFIRM_GESTURE_TAP_AS_RIGHT");
  
  // TODO: Sequence the modules!
}

#endif
