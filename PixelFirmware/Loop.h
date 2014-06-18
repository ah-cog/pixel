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
boolean appendLoopNode(int pin, int operation, int type, int mode, int value) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (loopSize < DEFAULT_LOOP_CAPACITY) {
    // Add behavior to queue
//    behaviorLoop[loopSize].id = generateBehaviorIdentifier();
    behaviorLoop[loopSize].operation = operation;
    behaviorLoop[loopSize].pin = pin;
    behaviorLoop[loopSize].type = type;
    behaviorLoop[loopSize].mode = mode;
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
  delayCount = 0; // Reset delay structures
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
