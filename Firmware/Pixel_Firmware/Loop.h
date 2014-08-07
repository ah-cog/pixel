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
    
    // Set up support structures for the behavior
    if (operation == BEHAVIOR_DELAY) {
      // Set up timer
      delays[delayCount].startTime = 0; // Initialize/Reset the timer
      delays[delayCount].duration = behaviorLoop[loopSize].value;
      delays[delayCount].behavior = &behaviorLoop[loopSize];
      
      Serial.print("Creating delay...");
      Serial.print(delays[delayCount].duration);
      Serial.println();
      
      delayCount++;
    }
    
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
        
        // Update delays' that point to the moved behavior
        if (behaviorLoop[i].operation == BEHAVIOR_DELAY) {
          for (int j = 0; j < delayCount; j++) {
            if (delays[j].behavior == &behaviorLoop[i + 1]) {
              delays[j].behavior = &behaviorLoop[i];
            }
          }
        }
      }
      
      // Update the beahvior counter if needed
      if (loopCounter >= index) {
        loopCounter--;
      }
      
      loopSize--;
      
      return true;
    }
    
    return false;
  }
  
  else if (loopSize < DEFAULT_LOOP_CAPACITY) {

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
      
      // Update delays' that point to the moved behavior
      if (behaviorLoop[i].operation == BEHAVIOR_DELAY) {
        for (int j = 0; j < delayCount; j++) {
          if (delays[j].behavior == &behaviorLoop[i - 1]) {
            delays[j].behavior = &behaviorLoop[i];
          }
        }
      }
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
    
    // Set up support structures for the behavior
    if (operation == BEHAVIOR_DELAY) {
      // Set up timer
      delays[delayCount].startTime = 0; // Initialize/Reset the timer
      delays[delayCount].duration = behaviorLoop[index].value;
      delays[delayCount].behavior = &behaviorLoop[index];
      
      Serial.print("Creating delay...");
      Serial.print(delays[delayCount].duration);
      Serial.println();
      
      delayCount++;
    }
    
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


/**
 * Returns a copy of the behavior node at the specified index.
 */
Behavior getBehaviorCopy(int index) {
  
  if (index >= 0 && index < loopSize) {
    
    // Copy the behavior node at the specified index
    Behavior behaviorNode = {
      behaviorLoop[index].pin,
      behaviorLoop[index].operation,
      behaviorLoop[index].type,
      behaviorLoop[index].mode,
      behaviorLoop[index].value
    }; // Get behavior node at specified index
    
    return behaviorNode;
  }
}

/**
 * Returns a pointer to behavior node at specified index.
 */
Behavior* getBehavior(int index) {
  
  if (index >= 0 && index < loopSize) {
    
    // Get pointer to behavior node at specified index
    Behavior* behaviorNode = NULL;
    behaviorNode = &behaviorLoop[index]; // Get behavior node at specified index
    
    return behaviorNode;
  }
  
  return NULL;
}

#endif
