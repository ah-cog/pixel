#ifndef VIRTUAL_DEVICE_H
#define VIRTUAL_DEVICE_H

// "Transcluded" device. Changes to this device are reflected in the associated physical device.
//struct VirtualDevice {
//  int ipAddress;
//  int meshAddress;
//  // TODO: Include "physicalDevice" reference.
//};
//VirtualDevice virtualDevice;

// TODO: Impelment PhysicalDevice class.
// #define PHYSICAL_PIN_COUNT 24

// TODO: Master board I/O state for (1) requested state and (2) reported state (by master).
struct VirtualPin {
  int pin; // The pin number
  int type; // i.e., digital, analog, pwm, touch
  int mode; // i.e., input or output
  int value; // i.e., high or low
  
  boolean isUpdated; // Denotes that the virtual pin has been updated and it should be synced up with the other virtual pins (and associated hardware).
};
#define VIRTUAL_PIN_COUNT 24
VirtualPin virtualPin[VIRTUAL_PIN_COUNT];

boolean setPinValue(int pin, int value) {
  virtualPin[pin].value = value;
  
  virtualPin[pin].isUpdated = true;
  return true;
}

int getPinValue(int pin) {
  return virtualPin[pin].value;
}

boolean setPinMode(int pin, int mode) {
  virtualPin[pin].mode = mode;
  
  virtualPin[pin].isUpdated = true;
  return true;
}

int getPinMode(int pin) {
  return virtualPin[pin].mode;
}

boolean setPinType(int pin, int type) {
  virtualPin[pin].type = type;
  
  virtualPin[pin].isUpdated = true;
  return true;
}

int getPinType(int pin) {
  return virtualPin[pin].type;
}

#endif
