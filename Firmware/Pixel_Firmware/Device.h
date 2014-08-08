#ifndef DEVICE
#define DEVICE

#define PHYSICAL_PIN_COUNT 24

// "Transcluded" device. Changes to this device are reflected in the associated physical device.
struct PhysicalDevice {
  int uid; // Universally unique ID
  
  int ipAddress;
  int meshAddress;
  // TODO: Include "physicalDevice" reference.
  
  boolean isUpdated;
};
PhysicalDevice physicalDevice;

// TODO: Master board I/O state for (1) requested state and (2) reported state (by master).
struct PhysicalPin {
  int pin; // The pin number
  int type; // i.e., digital, analog, pwm, touch
  int mode; // i.e., input or output
  int value; // i.e., high or low
  
  boolean isUpdated; // Denotes that the physical pin has been updated and it should be synced up with its virtual pins (and associated hardware).
};

PhysicalPin physicalPin[PHYSICAL_PIN_COUNT];

boolean setPinValue(int pin, int value) {
  physicalPin[pin].value = value;
  
  physicalPin[pin].isUpdated = true;
  return true;
}

int getPinValue(int pin) {
  return physicalPin[pin].value;
}

boolean setPinMode(int pin, int mode) {
  physicalPin[pin].mode = mode;
  
  physicalPin[pin].isUpdated = true;
  return true;
}

int getPinMode(int pin) {
  return physicalPin[pin].mode;
}

boolean setPinType(int pin, int type) {
  physicalPin[pin].type = type;
  
  physicalPin[pin].isUpdated = true;
  return true;
}

int getPinType(int pin) {
  return physicalPin[pin].type;
}

#endif
