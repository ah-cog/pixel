/*
"Pixel" Firmware (for the Visual Language, i.e., Looper), Rendition 2
Authors: Michael Gubbels
*/

// Notes for Teensy 3.1 (i.e., heuristics to get the CC3000 module working, based on experience):
// - Set "Board" to Teensy 3.1
// - Set "CPU Speed" to 24 MHz
// - Set clock to SPI_CLOCK_DIV8 (SPI_CLOCK_DIV2 also worked, but not too reliably)
//
// Notes about Adafruit CC3000 Breakout Board:
// - Firmware V. 1.24

#include <Wire.h>

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
};
#define VIRTUAL_PIN_COUNT 24
VirtualPin virtualPin[VIRTUAL_PIN_COUNT];

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "WebServer.h"

#define I2C_DEVICE_ADDRESS 2

// TODO: Implement list of changes to make to send to the Master (which executes gestural and the behavior code for the module)
// - TODO: Include status: "new", "sending", "sent", "confirmed" (after which, they're deleted)

// TODO: Implement web server and request handlers that, in response, update the queue of
//       changes to (1) make to Looper, and (2) to queue for sending to the other device 
//       over I2C upon request.

void setup () {
//  delay(2000);
  Serial.begin(9600); // Start serial for output
  Serial.println(F("Pixel, Firmware Denmark"));
  
  // Setup Wi-Fi and web server
  setupWebServer();
  
  // Setup I2C communication for device-device communication
  setupDeviceCommunication();
}

void setupDeviceCommunication() {
  Wire.begin(I2C_DEVICE_ADDRESS); // Join I2C bus with the device's address
  Wire.onReceive(i2cReceiveEvent);   // Register event handler to receive data from the master I2C device
  Wire.onRequest(i2cRequestEvent);   // Event handler to respond to a request for data from the I2C master device
}

/**
 * The event loop (i.e., this function is called repeatedly when the board is on)
 */
void loop () {
    
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = httpServer.available();
  
  if (client) {
    handleClientConnection (client);
  }
}

char i2cBuffer[32];
int i2cBufferSize = 0;
boolean hasMessage = false;

/**
 * function that executes whenever data is received from master
 * this function is registered as an event, see setup()
 */
void i2cReceiveEvent (int howMany) {
  
//  Serial.println("Receiving");
  
  while (Wire.available () > 0) { // loop through all but the last
    char c = Wire.read (); // receive byte as a character
//    Serial.print (c); // print the character
    
    i2cBuffer[i2cBufferSize] = c; // Buffer the character
    i2cBufferSize++; // Increment the buffer size
  }
  i2cBuffer[i2cBufferSize] = NULL; // Terminate the string
//  Serial.println();
  
  // Parse received pin value
  String split = String(i2cBuffer); // "hi this is a split test";
  String operation = getValue(split, ' ', 0);
  int pin = getValue(split, ' ', 1).toInt();
  int value = getValue(split, ' ', 2).toInt();
  
  // Update virtual device state
  virtualPin[pin].value = value;
  
  /*
  Serial.print("PIN ");
  Serial.print(pin);
  Serial.print(" = ");
  Serial.print(value);
  virtualPin[pin].value = value;
  // TODO: Update other state info for pin (or other state)
//  Serial.print(pin);
  Serial.println();
  */
  
  i2cBufferSize = 0;
  
  
  // TODO: Parse the message
  // TODO: Handle initial processing for message
  // TODO: Add to incoming I2C message queue (to process)
  // TODO: (elsewhere) Process messages one by one, in order
}

/**
 * function that executes whenever data is requested by master
 * this function is registered as an event, see setup()
 */
void i2cRequestEvent () {
  
  char buf[6]; // "-2147483648\0"
  
  if (behaviorTransformationCount > 0) {
    
    //Serial.print("behaviorTransformationCount = "); Serial.print(behaviorTransformationCount); Serial.println();

    // Send status
    Wire.write ("1 ");

    // Send serialized behavior
    Wire.write (itoa(behaviorTransformations[0].index, buf, 10)); Wire.write (" ");
    Wire.write (itoa(behaviorTransformations[0].operation, buf, 10)); Wire.write (" ");
    Wire.write (itoa(behaviorTransformations[0].pin, buf, 10));       Wire.write (" ");
    // Wire.write (itoa(behaviorTransformations[0].type, buf, 10));      Wire.write (" ");
    Wire.write (itoa(behaviorTransformations[0].mode, buf, 10));      Wire.write (" ");
    Wire.write (itoa(behaviorTransformations[0].value, buf, 10));     Wire.write (" ");
    
    // Remove the behavior from the processing queue once it's been sent over I2C
    removeBehaviorTransformation (0);
  } else {
    // Send status
    Wire.write ("0 ");
  }
  
  // pin, operation, type, mode, value
}
