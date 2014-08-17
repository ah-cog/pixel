#ifndef BEHAVIOR_H
#define BEHAVIOR_H

// Instruction operation codes (op. codes)
#define PIN_READ_DIGITAL 0
#define PIN_WRITE_DIGITAL 1
#define BEHAVIOR_DELAY 2
#define BEHAVIOR_ERASE 3
#define BEHAVIOR_DELETE 4
#define BEHAVIOR_UPDATE 5
#define BEHAVIOR_REBOOT 20
#define STATUS_WIFI_CONNECTED 30

// Loop
#define CREATE_LOOP 1

// Behavior
#define CREATE_BEHAVIOR 10
#define GET_BEHAVIOR 11
#define UPDATE_BEHAVIOR 12
#define DELETE_BEHAVIOR 13

// Direction
#define START_LOOP 20
#define PERFORM_LOOP 22
#define STOP 29

// Inspection
#define GET_STATUS 30

// Behavior types
#define BEHAVIOR_TYPE_NONE   0
#define BEHAVIOR_TYPE_INPUT  1
#define BEHAVIOR_TYPE_OUTPUT 2
#define BEHAVIOR_TYPE_DELAY  3

int behaviorCount = 0;

// Generates unique behavior ID
int generateBehaviorIdentifier() {
  int behaviorIdentifier = behaviorCount;
  behaviorCount++;
  return behaviorIdentifier;
}

struct Sequence;

struct Behavior {
  int uid; // The behavior's unique ID (uid will recur on other modules since it's just a counter associated with behaviors added)
  int type; // i.e., digital or analog
  void* schema; // The schema for the specific behavior type (e.g., for the "set pin" behavior)
  
  Sequence* sequence; // The sequence containing the behavior (if any)
  Behavior* previous; // The previous behavior in the sequence
  Behavior* next; // The next behavior in the sequence
  
  // TODO: Remove these!
  int operation; // i.e., write, read, PWM, "remember value at this time"
  int pin; // The pin number
  int mode; // i.e., input or output
  int value; // i.e., high or low
};

struct Input {
  Behavior* behavior;
  
  int pin;
  int mode;
  int signal;
  int data;
};

struct Output {
  Behavior* behavior;
  
  int type; // i.e., Behavior type
  
  int pin;
  int mode;
  int signal;
  int data;
};

struct Delay {
  Behavior* behavior;
  
  int milliseconds;
};

// The "looping" behavioral filter, providing dynamical/behavioral form within the general, unconstrained substrate.
// This provides a constraining context (i.e., structure) for behaviors and sequences.
struct Loop {
  boolean continuous;
};

#define SEQUENCE_TYPE_NONE 0
//#define SEQUENCE_TYPE_DOT  1
//#define SEQUENCE_TYPE_LINE 2
#define SEQUENCE_TYPE_LOOP 3

struct Sequence {
  int uid;
  int type; // e.g., line or loop
  
  Behavior* behavior; // The first behavior in the sequence
  int size;
  
  Sequence* previous;
  Sequence* next;
};

// The "behavioral substrate" which provides an unconstrained context for behaviors.
struct Substrate {
  // TOOD: List of sequences (i.e., loops)
  Sequence* entry; // The start sequence
  Sequence* sequences;
  
  // TODO?: Add previous and next
};

Substrate* substrate = NULL;

class Behavior2 {
  public:
    Behavior2();
    
    int type;
    int operation;
    int pin;
    int mode;
    int value;
    
    Behavior* previous; // Previous behavior
    Behavior* next; // Next behavior
    
  private:
    int uid;  
};

#endif
