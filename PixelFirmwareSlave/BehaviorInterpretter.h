struct BehaviorNode {
  int pin; // The pin number
  int operation; // i.e., read, write, PWM, "remember"
  int type; // i.e., digital or analog
  int mode; // i.e., input or output
  int value; // i.e., high or low
};

// Behavior nodes that define the module's beahvior
#define BEHAVIOR_NODE_CAPACITY 20
BehaviorNode behaviorNodes[BEHAVIOR_NODE_CAPACITY];
int behaviorNodeCount = 0;

/**
 * Insert a behavior node into the sequence of behavior nodes at the specified index.
 */
boolean insertBehaviorNode(int index, int operation, int pin, int type, int mode, int value) {
  // TODO: Add message to queue... and use sendMessage to send the messages...
  
  if (behaviorNodeCount < BEHAVIOR_NODE_CAPACITY) {
    // Add behavior to queue
    behaviorNodes[behaviorNodeCount].operation = operation;
    behaviorNodes[behaviorNodeCount].pin = pin;
    behaviorNodes[behaviorNodeCount].type = type;
    behaviorNodes[behaviorNodeCount].mode = mode;
    behaviorNodes[behaviorNodeCount].value = value;
    behaviorNodeCount++; // Increment the behavior node count
  }
  
  Serial.print("queueing behavior (size: ");
  Serial.print(behaviorNodeCount);
  Serial.print(")\n");
}

/**
 * Removes the behavior node at the specified index from the sequence of nodes (if it exists).
 */
boolean removeBehaviorNode(int index) {
  
  if (behaviorNodeCount > 0) { // Make sure there at least one behavior node exists
    
    if (index >= 0 && index < behaviorNodeCount) {
      // Remove the behavior node from the sequence of behavior nodes
      for (int i = index; i < behaviorNodeCount - 1; i++) {
        behaviorNodes[i] = behaviorNodes[i + 1];
      }
      behaviorNodeCount--;
      
      return true;
    }
  }

  return false;
}
