#ifndef LOOP_H
#define LOOP_H

#include "Behavior.h"

//struct Loop {
//  Behavior* nodes; // Behavior nodes
//  int nodeCount;
//  int counter; // i.e., the "program counter"
//};

// Behavior nodes that define the module's beahvior
#define DEFAULT_LOOP_CAPACITY 20
//Loop behaviorLoop[DEFAULT_LOOP_CAPACITY]; // TODO: FIx this!
//int loopSize = 0;
Behavior behaviorLoop[DEFAULT_LOOP_CAPACITY]; // Behavior nodes
int loopSize = 0;
int loopCounter = -1; // i.e., the "program counter"

boolean setupLoop() {
  // TODO: Dynamically allocate memory for behavior loop
  // TODO: Initialize empty behavior loop
}

long generateUuid() {
  long uuid = random(65000L);
  return uuid;
}

//Loop* createLoop(Substrate* substrate) {
//  
//}

Behavior* createBehavior2(String type, int pin, String mode, String signal, String data) {
  
//  if (loopSize < DEFAULT_LOOP_CAPACITY) {
  
  // Create behavior substrate
  if (substrate == NULL) {
    substrate              = (Substrate*) malloc (sizeof (Substrate));
    (*substrate).sequences = NULL;
    (*substrate).entry     = NULL;
  }
  
  // Create sequence
  // TODO: Add parameter "Substrate* substrate"
  if (substrate != NULL) {
    if ((*substrate).sequences == NULL) {
      
      // Create sequence
      Sequence* sequence = (Sequence*) malloc (sizeof (Sequence));
      (*sequence).uid      = NULL;
      (*sequence).type     = SEQUENCE_TYPE_LOOP;
      (*sequence).behavior = NULL;
      (*sequence).size     = 0;
      (*sequence).previous = NULL;
      (*sequence).next     = NULL;
      
      // TODO: Create sequence schema
      
      // Add sequence to substrate
      if ((*substrate).sequences == NULL) {
        Serial.println("First sequence");
        (*substrate).sequences = sequence;
      } else {
        
        // Get the last behavior in the loop
        Sequence* lastSequence = (*substrate).sequences;
        while ((*lastSequence).next != NULL) {
          Serial.println("Next sequence");
          lastSequence = (*lastSequence).next;
        }
        
        // Insert at end of the list (iterate to find the last behavior)
        (*sequence).previous = lastSequence; // Set up the pointer from the new behavior to the previous behavior.
        (*lastSequence).next = sequence; // Finally, set up the link to the new behavior.
      }
//      (*coreLoop).size = (*coreLoop).size + 1;
    }
  }
  
  //Behavior* behavior = &behaviorLoop[loopSize];
  // Create a behavior
  Behavior* behavior = (Behavior*) malloc (sizeof (Behavior));
  (*behavior).uid      = 0;
  (*behavior).type     = BEHAVIOR_TYPE_NONE;
  (*behavior).schema   = NULL;
  (*behavior).previous = NULL;
  (*behavior).next     = NULL;
  
  // Generate UUID for the behavior
  (*behavior).uid  = generateUuid();
  
  // Parse behavior type parameter
  Serial.println(type);
  (*behavior).type = BEHAVIOR_TYPE_INPUT;
  
  // Parse behavior schema parameters
  Serial.println(pin);
  Serial.println(mode);
  Serial.println(signal);
  Serial.println(data);
  
  // Set up the behavior schema
  Input* input = (Input*) malloc(sizeof(Input));
  (*behavior).schema = input;
  
  Serial.println((int)(*behavior).schema);
  
  if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
    Input* in = (Input*) (*behavior).schema;
  }
  
  // Add the behavior to the loop
  Sequence* coreLoop = (*substrate).sequences;
  if ((*coreLoop).behavior == NULL) {
    Serial.println("First");
    (*coreLoop).behavior = behavior;
  } else {
    
    // Get the last behavior in the loop
    Behavior* lastBehavior = (*coreLoop).behavior;
    while ((*lastBehavior).next != NULL) {
      Serial.println("Next");
      lastBehavior = (*lastBehavior).next;
    }
    
    // Insert at end of the list (iterate to find the last behavior)
    (*behavior).previous = lastBehavior; // Set up the pointer from the new behavior to the previous behavior.
    (*lastBehavior).next = behavior; // Finally, set up the link to the new behavior.
  }
  (*coreLoop).size = (*coreLoop).size + 1;
  
  Serial.print("Loop size: "); Serial.print((*coreLoop).size); Serial.print("\n");
  
  return behavior;
}

/**
 * Insert a behavior node into the behavior loop at the specified index.
 */
Behavior* createBehavior(String type, int pin, String mode, String signal, String data) {
  
  if (loopSize < DEFAULT_LOOP_CAPACITY) {
    
    Behavior* behavior = &behaviorLoop[loopSize];
    
    // Add behavior to queue
    (*behavior).uid  = generateUuid();
    (*behavior).type = BEHAVIOR_TYPE_INPUT;
    
    Serial.println(type);
    Serial.println(pin);
    Serial.println(mode);
    Serial.println(signal);
    Serial.println(data);
    
    Input* input = (Input*) malloc(sizeof(Input));
    (*behavior).schema = input;
    
    Serial.println((int)(*behavior).schema);
    
    if ((*behavior).type == BEHAVIOR_TYPE_INPUT) {
      Input* in = (Input*) (*behavior).schema;
    }
    
//    if (strcmp (type, "pin") == 0) {
//      insertBehavior(index, operation, pin, 0, 1, value);
//    }
    
    
//    behaviorLoop[loopSize].operation = operation;
//    behaviorLoop[loopSize].pin = pin;
//    behaviorLoop[loopSize].type = type;
//    // behaviorLoop[loopSize].mode = mode;
//    behaviorLoop[loopSize].value = value;
//    
//    // Set up support structures for the behavior
//    if (operation == BEHAVIOR_DELAY) {
//      // Set up timer
//      delays[delayCount].startTime = 0; // Initialize/Reset the timer
//      delays[delayCount].duration = behaviorLoop[loopSize].value;
//      delays[delayCount].behavior = &behaviorLoop[loopSize];
//      
//      Serial.print("Creating delay...");
//      Serial.print(delays[delayCount].duration);
//      Serial.println();
//      
//      delayCount++;
//    }
    
    loopSize++; // Increment the loop size
    
    return &behaviorLoop[loopSize - 1];
  }
  
  return NULL;
}

/**
 * Returns a pointer to behavior node at specified index.
 */
Behavior* getBehavior(int id) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  for (int i = 0; i < loopSize; i++) {
    if (behaviorLoop[i].uid == id) {
      behavior = &behaviorLoop[i]; // Get behavior node at specified index
      break;
    }
  }
  
  return behavior;
}

/**
 * Returns a pointer to behavior node at specified index.
 */
Behavior* getBehavior2(int uid) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // TODO: Search the sequences for the specified behavior
  
  // Search the loop for the behavior with the specified UID.
  if (substrate != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*substrate).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          return soughtBehavior;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }
  
  return behavior;
}

Behavior* updateBehavior(int id) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  for (int i = 0; i < loopSize; i++) {
    if (behaviorLoop[i].uid == id) {
      behavior = &behaviorLoop[i]; // Get behavior node at specified index
      
      // TODO: Update behavior with specified values.
      
      break;
    }
  }
  
  return behavior;
}

Behavior* updateBehavior2(int uid) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // Search the loop for the behavior with the specified UID.
  if (substrate != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*substrate).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          
          // TODO: Update the behavior
          
          return soughtBehavior;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }
  
  return behavior;
}

boolean deleteBehavior(int id) {
  
  if (loopSize > 0) { // Make sure there at least one behavior node exists
    
    for (int i = 0; i < loopSize; i++) {
      if (behaviorLoop[i].uid == id) {
        
        // Delete behavior by found index for behavior with specified ID
        if (i >= 0 && i < loopSize) {
          // Remove the behavior node from the sequence of behavior nodes
          for (int j = i; j < loopSize - 1; j++) {
            behaviorLoop[j] = behaviorLoop[j + 1];
          }
          loopSize--;
          
          return true;
        }
        
      }
    }
  }

  return false;
}

boolean deleteBehavior2(int uid) {
    
  // Get pointer to behavior node at specified index
  Behavior* behavior = NULL;
  
  // Search the loop for the behavior with the specified UID.
  if (substrate != NULL) {
    
    // Get the last behavior in the loop
    Sequence* currentSequence = (*substrate).sequences;
    while (currentSequence != NULL) {
      Serial.println("Searching sequence");
      
      // Get the last behavior in the loop
      Behavior* soughtBehavior = (*currentSequence).behavior;
      while (soughtBehavior != NULL) {
        Serial.println("Searching behavior");
        
        // Return the behavior if it has been found
        if ((*soughtBehavior).uid == uid) {
          
          Serial.println("Deleting behavior");
          
          // Update behavior topology
          Behavior* previousBehavior = (*soughtBehavior).previous;
          Behavior* nextBehavior     = (*soughtBehavior).next;
          
          // Update the forward sequence
          if (previousBehavior != NULL) {
            (*previousBehavior).next = nextBehavior;
          }
          
          // Update the backward sequence
          if (nextBehavior != NULL) {
            (*nextBehavior).previous = previousBehavior;
          }
          
          // Update sequence if needed
          if ((*currentSequence).behavior == soughtBehavior) {
            if ((*soughtBehavior).next == NULL) {
              (*currentSequence).behavior = NULL;
            } else {
              (*currentSequence).behavior = (*soughtBehavior).next;
            }
          }
          
          // Resize the sequence
          (*currentSequence).size = (*currentSequence).size - 1;
          
          // Free the behavior from memory
//          assert(soughtBehavior != NULL);
          free((*soughtBehavior).schema); // Free the behavior's schema from memory
          free(soughtBehavior); // Free the behavior from memory
          
          return true;
        }
        
        soughtBehavior = (*soughtBehavior).next;
      }
      
      currentSequence = (*currentSequence).next;
    }
    
  }
  
//  // Search the loop for the behavior with the specified UID.
//  if (coreLoop != NULL) {
//    
//    // Check if the current behavior is the sought behavior
//    Behavior* soughtBehavior = (*coreLoop).behavior;
//    while (soughtBehavior != NULL) {
//      Serial.println("Searching");
//      
//      // Return the behavior if it has been found
//      if ((*soughtBehavior).uid == uid) {
//        
//        Serial.println("Deleting");
//        
//        // Update behavior topology
//        Behavior* previousBehavior = (*soughtBehavior).previous;
//        Behavior* nextBehavior     = (*soughtBehavior).next;
//        
//        // Update the forward sequence
//        if (previousBehavior != NULL) {
//          (*previousBehavior).next = nextBehavior;
//        }
//        
//        // Update the backward sequence
//        if (nextBehavior != NULL) {
//          (*nextBehavior).previous = previousBehavior;
//        }
//        
//        // Free the behavior from memory
////        assert(soughtBehavior != NULL);
//        free((*soughtBehavior).schema); // Free the behavior's schema from memory
//        free(soughtBehavior); // Free the behavior from memory
//        
//        // TODO: Resize the loop!
//        
//        return true;
//      }
//      
//      soughtBehavior = (*soughtBehavior).next;
//    }
//    
//  }

  return false;
}

/**
 * Insert a behavior node into the behavior loop at the specified index.
 */
boolean appendLoopNode(int pin, int operation, int type, int value) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (loopSize < DEFAULT_LOOP_CAPACITY) {
    // Add behavior to queue
//    behaviorLoop[loopSize].id = generateBehaviorIdentifier();
    behaviorLoop[loopSize].operation = operation;
    behaviorLoop[loopSize].pin = pin;
    behaviorLoop[loopSize].type = type;
    // behaviorLoop[loopSize].mode = mode;
    behaviorLoop[loopSize].value = value;
    
    loopSize++; // Increment the loop size
    
    return true;
  }
  
  return false;
}

/**
 * Insert a behavior node into the behavior loop at the specified index.
 */
boolean applyBehaviorTransformation(int index, int pin, int operation, int type, int value) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (operation == BEHAVIOR_DELETE) {
    
    // Check if index is in valid range
    if (index >= 0 && index < DEFAULT_LOOP_CAPACITY) {
      Serial.print("index = "); Serial.print(index); Serial.println();
      
      // Move any behaviors back if needed
      //for (int i = index; i < loopSize; i++) {
      for (int i = index; i < loopSize; i++) {
        behaviorLoop[i].uid = behaviorLoop[i + 1].uid;
        behaviorLoop[i].operation = behaviorLoop[i + 1].operation;
        behaviorLoop[i].pin = behaviorLoop[i + 1].pin;
        behaviorLoop[i].type = behaviorLoop[i + 1].type;
        // behaviorLoop[i].mode = behaviorLoop[i + 1].mode;
        behaviorLoop[i].value = behaviorLoop[i + 1].value;
      }
      
      // Update the beahvior counter if needed
      if (loopCounter >= index) {
        loopCounter--;
      }
      
      loopSize--;
      
      return true;
    }
    
    return false;
    
  } else if (operation == BEHAVIOR_UPDATE) {

    // Check if the behavior index is in bounds
    if (index < 0  || index >= loopSize) {
      return false;
    }
    
    Serial.print("index = "); Serial.print(index); Serial.println();
    
    // Add behavior to queue
    //behaviorLoop[loopSize].uid = generateBehaviorIdentifier();
    //behaviorLoop[index].operation = operation;
    behaviorLoop[index].pin = pin;
    behaviorLoop[index].type = type;
    // behaviorLoop[index].mode = mode;
    behaviorLoop[index].value = value;
    
    Serial.print("value = "); Serial.print(behaviorLoop[index].value); Serial.println();
    
//    setPinValue2 (index, value);
    
    // TODO: Update the delay behavior.
    // Set up support structures for the behavior
//    if (operation == BEHAVIOR_DELAY) {
//      // Set up timer
//      delays[delayCount].startTime = 0; // Initialize/Reset the timer
//      delays[delayCount].duration = behaviorLoop[index].value;
//      delays[delayCount].behavior = &behaviorLoop[index];
//      
//      Serial.print("Creating delay...");
//      Serial.print(delays[delayCount].duration);
//      Serial.println();
//      
//      delayCount++;
//    }

    return true;
    
  } else if (operation == BEHAVIOR_REBOOT) {
    
    int SLAVE_DEVICE_ADDRESS = 2;
    
    // Send message to slave to reboot
    char buf[8];
    Wire.beginTransmission(SLAVE_DEVICE_ADDRESS); // transmit to device #4
    Wire.write("reboot  ");
    Wire.endTransmission();    // stop transmitting
    
    _reboot_Teensyduino_();
//    while(1) { /* NOTE: This is an infinite loop! */ }
    
    return true;
    
  } else if (operation == STATUS_WIFI_CONNECTED) {
    
    Serial.println("Wi-Fi Connected!");
    
    return true;
    
  } else if (loopSize < DEFAULT_LOOP_CAPACITY) {

    // If index is -1 or greater than the loopSize then add the beahvior transformation to the end of the loop
    if (index == -1 || index > loopSize) {
      index = loopSize;
    }
    
    Serial.print("index = "); Serial.print(index); Serial.println();
    
    // Move any behaviors back if needed
    //for (int i = index; i < loopSize; i++) {
    for (int i = loopSize; i > index; i--) {
      behaviorLoop[i].uid = behaviorLoop[i - 1].uid;
      behaviorLoop[i].operation = behaviorLoop[i - 1].operation;
      behaviorLoop[i].pin = behaviorLoop[i - 1].pin;
      behaviorLoop[i].type = behaviorLoop[i - 1].type;
      // behaviorLoop[i].mode = behaviorLoop[i - 1].mode;
      behaviorLoop[i].value = behaviorLoop[i - 1].value;
    }
    
    // Update the beahvior counter if needed
    if (loopCounter >= index) {
      loopCounter++;
    }
    
    // Add behavior to queue
    behaviorLoop[loopSize].uid = generateBehaviorIdentifier();
    behaviorLoop[index].operation = operation;
    behaviorLoop[index].pin = pin;
    behaviorLoop[index].type = type;
    // behaviorLoop[index].mode = mode;
    behaviorLoop[index].value = value;
    
    loopSize++; // Increment the loop size
    
    return true;
  }
  
  return false;
}

///**
// * Insert a behavior node into the behavior loop at the specified index.
// */
//boolean insertLoopNode(int index, int pin, int operation, int type, int mode, int value) {
//  // TODO: Add message to queue... and use sendMessage to send the messages...
//  
//  if (behaviorNodeCount < BEHAVIOR_NODE_CAPACITY) {
//    // Add behavior to queue
//    behaviorLoop[behaviorNodeCount].pin = pin;
//    behaviorLoop[behaviorNodeCount].operation = operation;
//    behaviorLoop[behaviorNodeCount].type = type;
//    behaviorLoop[behaviorNodeCount].mode = mode;
//    behaviorLoop[behaviorNodeCount].value = value;
//    behaviorNodeCount++; // Increment the behavior node count
//  }
//  
////  Serial.print("queueing message (size: ");
////  Serial.print(messageQueueSize);
////  Serial.print(")\n");
//}

/**
 * Removes the behavior node at the specified index from the sequence of nodes (if it exists).
 */
boolean removeLoopNode(int index) {
  
  if (loopSize > 0) { // Make sure there at least one behavior node exists
    
    if (index >= 0 && index < loopSize) {
      // Remove the behavior node from the sequence of behavior nodes
      for (int i = index; i < loopSize - 1; i++) {
        behaviorLoop[i] = behaviorLoop[i + 1];
      }
      loopSize--;
      
      return true;
    }
  }

  return false;
}

/**
 * Removes all behaviors from the loop (if any exist).
 */
boolean eraseLoop() {
  
  loopSize = 0; // Reset behavior loop
//  delayCount = 0; // Reset delay structures
  return true;
}


///**
// * Returns a copy of the behavior node at the specified index.
// */
//Behavior getBehaviorCopy(int index) {
//  
//  if (index >= 0 && index < loopSize) {
//    
//    // Copy the behavior node at the specified index
//    Behavior behaviorNode = {
//      behaviorLoop[index].pin,
//      behaviorLoop[index].operation,
//      behaviorLoop[index].type,
//      behaviorLoop[index].mode,
//      behaviorLoop[index].value
//    }; // Get behavior node at specified index
//    
//    return behaviorNode;
//  }
//}

/**
 * Returns a pointer to behavior node at specified index.
 */
Behavior* getBehaviorByIndex(int index) {
  
  if (index >= 0 && index < loopSize) {
    
    // Get pointer to behavior node at specified index
    Behavior* behaviorNode = NULL;
    behaviorNode = &behaviorLoop[index]; // Get behavior node at specified index
    
    return behaviorNode;
  }
  
  return NULL;
}

#endif
