#ifndef BEHAVIOR_H
#define BEHAVIOR_H

// turn digital pin on
// turn digital pin off
// get state of digital pin
// delay

struct Behavior {
  // TODO: add "instruction" so can add "delay" etc.
  int pin; // The pin number
  int operation; // i.e., write, read, PWM, "remember value at this time"
  int type; // i.e., digital or analog
  int mode; // i.e., input or output
  int value; // i.e., high or low
};

#endif
