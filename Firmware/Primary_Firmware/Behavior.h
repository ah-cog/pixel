#ifndef BEHAVIOR_H
#define BEHAVIOR_H

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

#define BEHAVIOR_TYPE_SOUND  4 // TODO: Make this a "user defined" behavior in Looper
#define BEHAVIOR_TYPE_MOTION 5 // TODO: Make this a "user defined" behavior in Looper

#define BEHAVIOR_TYPE_IMMEDIATE 6 // TODO: Hack... consider removing this "message" behavior
#define BEHAVIOR_TYPE_ABSTRACT  7

struct Context;
struct Sequence;
//struct Point; // or "Dot"
//struct Line;
struct Loop;
struct Behavior;
struct Input;
struct Output;
struct Delay;

// Perspective:
// TODO: Create "Perspective" with pointers to Contexts, etc. as the intermediary for interacting with Contexts, Sequences, Behaviors, etc.
Context* context = NULL;
Sequence* currentSequence = NULL;

int behaviorCount = 0;

// Generates unique behavior ID
int Generate_Behavior_Identifier () {
  int behaviorIdentifier = behaviorCount;
  behaviorCount++;
  return behaviorIdentifier;
}

// The "behavioral context" which provides an unconstrained context for behaviors.
struct Context {
  // TOOD: List of sequences (i.e., loops)
  Sequence* origin; // The start sequence (i.e., the first sequence to execute in the context)
  Sequence* sequences;
  
  // TODO: Implement previous and next
  // Context* previous;
  // Context* next;
};

#define SEQUENCE_TYPE_NONE 0
//#define SEQUENCE_TYPE_DOT  1
//#define SEQUENCE_TYPE_LINE 2
#define SEQUENCE_TYPE_LOOP 3

struct Sequence {
  int uid;
  int type; // e.g., line or loop
  void* schema;
  Context* context;
  
  Behavior* behavior; // The first behavior in the sequence
  int size;
  
  Sequence* previous; // The previous sequence in the list
  Sequence* next; // THe next sequence in the list
};

// The "looping" behavioral filter, providing dynamical/behavioral form within the general, unconstrained context.
// This provides a constraining context (i.e., structure) for behaviors and sequences.
struct Loop {
  boolean continuous; // set to true
  boolean singleton; // set to false
};

//struct Line {
//  boolean continuous; // set to false
//  boolean singleton; // set to false
//};

//struct Point { // or "Dot"
//  boolean continuous;
//  boolean singleton;
//};

struct Behavior {
  int uid; // The behavior's unique ID (uid will recur on other modules since it's just a counter associated with behaviors added)
  int type; // i.e., digital or analog
  void* schema; // The schema for the specific behavior type (e.g., for the "set pin" behavior)
  
  // Activation conditions
  int conditionType; // 0 = NONE, 1 = <X> equals <Y>, 2 = <X> is not equal to <Y>
  
  Context* context; // The context containing the behavior
  
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

// TODO: char* behaviors = { "input", "output", "delay" }; // Populate this array with the behaviors implemented by the device, interfacing betwen the hardware and the Looper engine

// TODO: Update the following Behavior schema declarations to be (key,value (,type?)) values stored in dynamic memory... so platform implementers only have to define mappings between hardware-specific I/O functions and Looper

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

//! \struct Delay
//! The definition of the Delay structure.
//!
struct Delay {
  Behavior* behavior;
  
  int milliseconds;
  
  // Temporary variables (TODO: Consider moving these elsewhere, such as into a "schema" or similar structure.)
  unsigned long startTime;
  unsigned long currentTime;
};

// TODO: Make the following defined WITHIN Looper (and the above)

struct Sound {
  Behavior* behavior;
  
  int note;
  int duration;
};


struct Motion {
  Behavior* behavior;
  
  int position;
  
  int lenMicroSecondsOfPeriod; // = 25 * 1000; // 25 milliseconds (ms)
  int lenMicroSecondsOfPulse; // = 1 * 1000; // 1 ms is 0 degrees
  int first; // = 0.4 * 1000; //0.5ms is 0 degrees in HS-422 servo
  int end; // = 2.3 * 1000; // 3.7 * 1000;
  int increment; // = 0.01 * 1000;
};

//! \struct Immediate
//! The definition of the Immediate structure.
//!
struct Immediate {
  Behavior* behavior;
  
  int messageSize;
  char* message;
};

//! \struct Abstract
//! The definition of the Sequence structure.
//!
struct Abstract { // Sequence_Behavior {
  Behavior* behavior;
  
  Sequence* sequence;
};

// TODO: struct Periodic // i.e., a periodic behavior occurs every "x" seconds (converted from whatever unit of time is designated by the user)
// TODO: struct Counted // i.e., a counted behavior occurs "x" times only (then is handled similar to an executed "Immediate" behavior)
// TODO: struct Immediate // i.e., an immediate behavior is performed once, then stopped (deleted? disabled? deleted but a history is kept of it in the behavior history/timeline on remote server then deleted locally?)

#endif
