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

#define MESH_INCOMING_QUEUE_CAPACITY 20
//unsigned short int meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY] = { 0 };
Message meshIncomingMessages[MESH_INCOMING_QUEUE_CAPACITY];
int meshIncomingMessageQueueSize = 0;

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

/**
 * Adds the specified module as a next module if it hasn't already been added.
 */
boolean addNextModule(int module) {
  
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



//----------




/**
 * Push a message onto the queue of messages to be processed and sent via the mesh network.
 */
boolean queueIncomingMeshMessage(int source, int message) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (meshIncomingMessageQueueSize < MESH_INCOMING_QUEUE_CAPACITY) {
    // Add message to queue
    meshIncomingMessages[meshIncomingMessageQueueSize].source = source;
    meshIncomingMessages[meshIncomingMessageQueueSize].message = message; // Add message to the back of the queue
    meshIncomingMessageQueueSize++; // Increment the message count
  }
  
//  Serial.print("queueing message (size: ");
//  Serial.print(messageQueueSize);
//  Serial.print(")\n");
}

/**
 * Sends the top message on the mesh's message queue.
 */
Message dequeueIncomingMeshMessage() {
  
  if (meshIncomingMessageQueueSize > 0) {
    
    // Get the next message from the front of the queue
    //unsigned short int message = meshIncomingMessages[0].message; // Get message on front of queue
    Message message = { meshIncomingMessages[0].source, meshIncomingMessages[0].message }; // Get message on front of queue
    meshIncomingMessageQueueSize--;
    
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
