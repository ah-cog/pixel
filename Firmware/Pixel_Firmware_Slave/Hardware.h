#ifndef HARDWARE_H
#define HARDWARE_H

#define HARDWARE_PIN_COUNT 24

struct Hardware;
struct Port;

struct Hardware {
  Port pins[HARDWARE_PIN_COUNT];
};
//Hardware hardware;

struct Port {
  int number;
  int type;
  int mode;
  int data;
  
  int hasUpdated;
};

// Update the pin's state
boolean Update_Virtual_Pin (Hardware* hardware, int pin, int signal, int data) {
  
  // TODO:
//  (*hardware).pins[pin].mode = data;
//  (*hardware).pins[pin].data = data;
  
  // Update the hardware's actual pin's state
  Update_Pin (pin, signal, data);
  
}

boolean Update_Pin (Hardware* hardware, int pin, int signal, int data) {
  
  // TODO: Define for the device (e.g., Arduino)
  // e.g., pinMode (pin, OUTPUT);
  
}

//// TODO: Rename to Output
//boolean Update_Port (int pin, int signal, int data) {
//  // TODO: Define for the device (e.g., Arduino)
//  
//  pinMode (pin, OUTPUT);
//}

#endif
