#ifndef BEHAVIOR_H
#define BEHAVIOR_H

//// Instruction operation codes (op. codes)
//#define PIN_READ_DIGITAL 0
//#define PIN_WRITE_DIGITAL 1
//#define BEHAVIOR_DELAY 2
//#define BEHAVIOR_ERASE 3
//#define BEHAVIOR_DELETE 4
//#define BEHAVIOR_UPDATE 5
//#define BEHAVIOR_REBOOT 20
//#define STATUS_WIFI_CONNECTED 30


// Behavior Transformations
#define CREATE 1
#define GET 2
#define UPDATE 3
#define DELETE 4

// Behaviors
#define BEHAVIOR 1
#define DOT 2
#define LINE 3
#define LOOP 4

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

struct Substrate;
struct Sequence;
//struct Dot;
//struct Line;
struct Loop;
struct Behavior;
struct Input;
struct Output;
struct Delay;

// The "behavioral substrate" which provides an unconstrained context for behaviors.
struct Substrate {
  // TOOD: List of sequences (i.e., loops)
  Sequence* origin; // The start sequence (i.e., the first sequence to execute in the substrate)
  Sequence* sequences;
  
  // TODO: Implement previous and next
  // Substrate* previous;
  // Substrate* next;
};

#define SEQUENCE_TYPE_NONE 0
//#define SEQUENCE_TYPE_DOT  1
//#define SEQUENCE_TYPE_LINE 2
#define SEQUENCE_TYPE_LOOP 3

struct Sequence {
  int uid;
  int type; // e.g., line or loop
  void* schema;
  Substrate* substrate;
  
  Behavior* behavior; // The first behavior in the sequence
  int size;
  
  Sequence* previous; // The previous sequence in the list
  Sequence* next; // THe next sequence in the list
};

// The "looping" behavioral filter, providing dynamical/behavioral form within the general, unconstrained substrate.
// This provides a constraining context (i.e., structure) for behaviors and sequences.
struct Loop {
  boolean continuous; // set to true
  boolean singleton; // set to false
};

//struct Line {
//  boolean continuous; // set to false
//  boolean singleton; // set to false
//};

//struct Dot {
//  boolean continuous;
//  boolean singleton;
//};

Substrate* substrate = NULL;

struct Behavior {
  int uid; // The behavior's unique ID (uid will recur on other modules since it's just a counter associated with behaviors added)
  int type; // i.e., digital or analog
  void* schema; // The schema for the specific behavior type (e.g., for the "set pin" behavior)
  
  Substrate* substrate; // The substrate containing the behavior
  
  Sequence* sequence; // The sequence containing the behavior (if any)
  
  Behavior* previous; // The previous behavior in the sequence
  Behavior* next; // The next behavior in the sequence
  
//  // TODO: Remove these!
//  int operation; // i.e., write, read, PWM, "remember value at this time"
//  int pin; // The pin number
//  int mode; // i.e., input or output
//  int value; // i.e., high or low
};

#define MODE_OUTPUT 0
#define MODE_INPUT  1

#define SIGNAL_DIGITAL 0
#define SIGNAL_ANALOG  1

#define DATA_OFF 0
#define DATA_ON  1

struct Input {
  Behavior* behavior;
  
  int pin;
//  int mode;
  int signal;
//  int data;
};

//! \struct Output
//! The definition of the Output structure.
//!
struct Output {
  Behavior* behavior;
  
//  int type; // i.e., Behavior type
  
  int pin;
//  int mode;
  int signal;
  int data;
};

struct Delay {
  Behavior* behavior;
  
  int milliseconds;
};

#endif
