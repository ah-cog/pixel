#ifndef BEHAVIOR_TRANSFORMATION_H
#define BEHAVIOR_TRANSFORMATION_H

//struct BehaviorTransformation { // formerly BehaviorTransformation
//  int index; // The index in the loop of this behaviodr
//  int pin; // The pin number
//  int operation; // i.e., read, write, PWM, "remember"
//  int type; // i.e., digital or analog
//  int mode; // i.e., input or output
//  int value; // i.e., high or low
//};

// Behavior nodes that define the module's beahvior
//#define BEHAVIOR_TRANSFORMATION_CAPACITY 20
//BehaviorTransformation behaviorTransformations[BEHAVIOR_TRANSFORMATION_CAPACITY];
//int behaviorTransformationCount = 0;

///**
// * Insert a behavior node into the sequence of behavior nodes at the specified index.
// */
//boolean insertBehavior(int index, int operation, int pin, int type, int mode, int value) {
//  
//  if (behaviorTransformationCount < BEHAVIOR_TRANSFORMATION_CAPACITY) {
//
//      // Insert behavior to queue
//      behaviorTransformations[behaviorTransformationCount].index = index;
//      behaviorTransformations[behaviorTransformationCount].operation = operation;
//      behaviorTransformations[behaviorTransformationCount].pin = pin;
//      behaviorTransformations[behaviorTransformationCount].type = type;
//      behaviorTransformations[behaviorTransformationCount].mode = mode;
//      behaviorTransformations[behaviorTransformationCount].value = value;
//      
//      behaviorTransformationCount++; // Increment the behavior node count
//  }
//  
//  Serial.print("queueing behavior (size: ");
//  Serial.print(behaviorTransformationCount);
//  Serial.print(")\n");
//}

///**
// * Removes the behavior node at the specified index from the sequence of nodes (if it exists).
// */
//boolean removeBehaviorTransformation(int index) {
//  
//  if (behaviorTransformationCount > 0) { // Make sure there at least one behavior node exists
//    
//    if (index >= 0 && index < behaviorTransformationCount) {
//      // Remove the behavior node from the sequence of behavior nodes
//      for (int i = index; i < behaviorTransformationCount - 1; i++) {
//        behaviorTransformations[i] = behaviorTransformations[i + 1];
//      }
//      behaviorTransformationCount--;
//      
//      return true;
//    }
//  }
//
//  return false;
//}

#endif
