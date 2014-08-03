struct BehaviorTransformation { // formerly BehaviorTransformation
  int index; // The index in the loop of this behaviodr
  int pin; // The pin number
  int operation; // i.e., read, write, PWM, "remember"
  int type; // i.e., digital or analog
  int mode; // i.e., input or output
  int value; // i.e., high or low
};

// Behavior nodes that define the module's beahvior
#define BEHAVIOR_TRANSFORMATION_CAPACITY 20
BehaviorTransformation behaviorTransformations[BEHAVIOR_TRANSFORMATION_CAPACITY];
int behaviorTransformationCount = 0;

/**
 * Insert a behavior node into the sequence of behavior nodes at the specified index.
 */
boolean insertBehavior(int index, int operation, int pin, int type, int mode, int value) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  // If index is -1, set the index to the last position in the list
//  if (index == -1) {
//    index = behaviorTransformationCount;
//  }
  
  if (behaviorTransformationCount < BEHAVIOR_TRANSFORMATION_CAPACITY) {
//    if (index >= 0 && index < BEHAVIOR_TRANSFORMATION_CAPACITY) {
      
//      // If specified index is larger than the number of behaviors
//      if (index > behaviorTransformationCount) {
//        index = behaviorTransformationCount;
//      }
//      
//      // Push subsequent behaviors to end of queue
//      for (int i = index; i < behaviorTransformationCount; i++) {
//        // Insert behavior to queue
//        if (i + 1 < BEHAVIOR_TRANSFORMATION_CAPACITY) {
//          behaviorTransformations[i + 1].operation = behaviorTransformations[i].operation;
//          behaviorTransformations[i + 1].pin = behaviorTransformations[i].pin;
//          behaviorTransformations[i + 1].type = behaviorTransformations[i].type;
//          behaviorTransformations[i + 1].mode = behaviorTransformations[i].mode;
//          behaviorTransformations[i + 1].value = behaviorTransformations[i].value;
//        }
//      }
//      
//      // Insert behavior to queue
//      behaviorTransformations[index].operation = operation;
//      behaviorTransformations[index].pin = pin;
//      behaviorTransformations[index].type = type;
//      behaviorTransformations[index].mode = mode;
//      behaviorTransformations[index].value = value;

      // Insert behavior to queue
      behaviorTransformations[behaviorTransformationCount].index = index;
      behaviorTransformations[behaviorTransformationCount].operation = operation;
      behaviorTransformations[behaviorTransformationCount].pin = pin;
      behaviorTransformations[behaviorTransformationCount].type = type;
      behaviorTransformations[behaviorTransformationCount].mode = mode;
      behaviorTransformations[behaviorTransformationCount].value = value;
      
      behaviorTransformationCount++; // Increment the behavior node count
//    }
  }
  
  Serial.print("queueing behavior (size: ");
  Serial.print(behaviorTransformationCount);
  Serial.print(")\n");
}

/**
 * Removes the behavior node at the specified index from the sequence of nodes (if it exists).
 */
boolean removeBehaviorTransformation(int index) {
  
  if (behaviorTransformationCount > 0) { // Make sure there at least one behavior node exists
    
    if (index >= 0 && index < behaviorTransformationCount) {
      // Remove the behavior node from the sequence of behavior nodes
      for (int i = index; i < behaviorTransformationCount - 1; i++) {
        behaviorTransformations[i] = behaviorTransformations[i + 1];
      }
      behaviorTransformationCount--;
      
      return true;
    }
  }

  return false;
}
