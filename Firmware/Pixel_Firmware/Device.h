//
// i.e., Device "virtual machine"
//
// TODO: Define "void setPinMode(Pixel pixel, int mode)" to update the state of another Pixel in the network (from any available Pixel)

#ifndef DEVICE_H
#define DEVICE_H

#define DEVICE_PIN_COUNT 24

// i.e., the "transcluded" device. Changes to this device are reflected in the associated physical device.
//struct PhysicalDevice {
//  int uid; // Universally unique ID
//  
//  int ipAddress;
//  int meshAddress;
//  // TODO: Include "physicalDevice" reference.
//  
//  boolean isUpdated;
//};
//PhysicalDevice physicalDevice;

struct PhysicalPin {
  int pin; // The pin number
  int type; // i.e., digital, analog, pwm, touch
  int mode; // i.e., input or output
  int value; // i.e., high or low
  
  int location; // Is it local or remote
  
  boolean isUpdated; // Denotes that the physical pin has been updated and it should be synced up with its virtual pins (and associated hardware).
};

PhysicalPin physicalPin[DEVICE_PIN_COUNT];

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

void setupDevice () {
  for (int i = 0; i < DEVICE_PIN_COUNT; i++) {
    physicalPin[i].pin   = i;
    physicalPin[i].type  = PIN_TYPE_DIGITAL; // Initialize these for the hardware being used (e.g., Teensy 3.1)
    physicalPin[i].mode  = PIN_MODE_INPUT; // Initialize pin mode to input
    physicalPin[i].value = 0; // Initialize pin value to 0
  }
}
  
void setPinType (int pin, int type) {
  physicalPin[pin].type = type;
  
  // NOTE: There's no "pin type" for Arduino. It's just a virtual construct for Pixel.
}

int getPinType (int pin) {
  return physicalPin[pin].type;
}

void setPinMode (int pin, int mode) {
  // Set the Pixel's pin mode (virtually)
  physicalPin[pin].mode = mode;
  
  // Set the actual hardware's pin's mode
  pinMode(pin, (mode == PIN_MODE_INPUT ? INPUT : OUTPUT));
  
  physicalPin[pin].isUpdated = true;
}

int getPinMode (int pin) {
  return physicalPin[pin].mode;
}

void setPinValue (int pin, int value) {
  physicalPin[pin].value = value;
  
  Serial.println(physicalPin[pin].value);
  
  // TODO: Consider not having this?
//  pinMode(0, INPUT);
//  setPinState (pin, 0, 1, value);
  setPinMode (pin, PIN_MODE_OUTPUT);
  
  // TODO: Include "pin mode" and "analog, digital, touch, pwm" in the output
  
  // Write to the pin
  digitalWrite (pin, (physicalPin[pin].value == PIN_VALUE_HIGH ? HIGH : LOW));
  
  physicalPin[pin].isUpdated = true;
}

int getLastPinValue (int pin) {
  
//  setPinMode(pin, PIN_MODE_INPUT);
  
  physicalPin[pin].value = digitalRead (pin);
  
  physicalPin[pin].isUpdated = true;
  return physicalPin[pin].value;
}

int getPinValue (int pin) {
  
  setPinMode(pin, PIN_MODE_INPUT);
  
  physicalPin[pin].value = digitalRead (pin);
  
  physicalPin[pin].isUpdated = true;
  return physicalPin[pin].value;
}

#endif
