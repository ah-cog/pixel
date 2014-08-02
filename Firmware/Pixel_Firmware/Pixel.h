#ifndef PIXEL_H
#define PIXEL_H

/**
 * i.e., Pixel "virtual machine"
 */

// Pixel's pin states (i.e., high or low)
#define PIN_VALUE_LOW 0
#define PIN_VALUE_HIGH 1
#define PIN_VALUE_NONE 2

// Pixel's pin modes (i.e., input or output)
#define PIN_MODE_INPUT 0
#define PIN_MODE_OUTPUT 1

// Pixel's pin type
#define PIN_TYPE_DIGITAL 0
#define PIN_TYPE_ANALOG 1
#define PIN_TYPE_PWM 2
#define PIN_TYPE_TOUCH 3

/**
 * Reflection
 */
 
// Pixel's reflection (i.e., model) of self
struct Pixel {
  // neighbor's mesh address
  // neighbor's web address (for Looper)
};

// TODO: Master board I/O state for (1) requested state and (2) reported state (by master).
struct PinReflection {
  int location; // Is it local or remote
  int pin; // The pin number
  int type; // i.e., digital, analog, pwm, touch
  int mode; // i.e., input or output
  int value; // i.e., high or low
};
PinReflection pinReflection[24];

void setupPixel () {
  for (int i = 0; i < 24; i++) {
    pinReflection[i].pin = i;
    pinReflection[i].type = 0; // TODO: Initialize these appropriately for the hardware being used (e.g., Teensy 3.1)
    pinReflection[i].mode = 1; // TODO: Initialize
    pinReflection[i].value = 0; // TDO: Initialize
  }
}

//void setPinState (int pin, int type, int mode, int value) {
//  pinReflection[pin].pin = pin;
//  pinReflection[pin].type = type;
//  pinReflection[pin].mode = mode;
//  pinReflection[pin].value = value;
//}

// TODO: Define "void setPinMode(Pixel pixel, int mode)" to update the state of another Pixel in the network (from any available Pixel)
  
void setPinType (int pin, int type) {
  pinReflection[pin].type = type;
  
  // NOTE: There's no "pin type" for Arduino. It's just a virtual construct for Pixel.
}

int getPinType (int pin) {
  return pinReflection[pin].type;
}

void setPinMode (int pin, int mode) {
  // Set the Pixel's pin mode (virtually)
  pinReflection[pin].mode = mode;
  
  // Set the actual hardware's pin's mode
  pinMode(pin, (mode == PIN_MODE_INPUT ? INPUT : OUTPUT));
}

int getPinMode (int pin) {
  return pinReflection[pin].mode;
}

void setPinValue (int pin, int value) {
  pinReflection[pin].value = value;
  
  // TODO: Consider not having this?
//  pinMode(0, INPUT);
//  setPinState (pin, 0, 1, value);
  setPinMode (pin, PIN_MODE_OUTPUT);
  
  // TODO: Include "pin mode" and "analog, digital, touch, pwm" in the output
  
  // Write to the pin
  digitalWrite (pin, (pinReflection[pin].value == PIN_VALUE_HIGH ? HIGH : LOW));
}

int getPinValue (int pin) {
  
  setPinMode(pin, PIN_MODE_INPUT);
  
  pinReflection[pin].value = digitalRead (pin);
  
  return pinReflection[pin].value;
}

#endif
