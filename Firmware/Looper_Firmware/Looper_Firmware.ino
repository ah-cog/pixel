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
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "VirtualDevice.h"
#include "WebServer.h"

#define I2C_DEVICE_ADDRESS 2

// TODO: Implement list of changes to make to send to the Master (which executes gestural and the behavior code for the module)
// - TODO: Include status: "new", "sending", "sent", "confirmed" (after which, they're deleted)

// TODO: Implement web server and request handlers that, in response, update the queue of
//       changes to (1) make to Looper, and (2) to queue for sending to the other device 
//       over I2C upon request.

void setup () {

  Serial.begin(9600); // Start serial for output
  Serial.println(F("Looper Firmware"));
  
  setupLooper();
//  Propagation* propagation = Create_Propagation ("create substrate 55ff68064989"); // 55ff68064989495329092587
//  Queue_Propagation (propagator, propagation);
  
  // Setup Wi-Fi and web server
  setupWebServer();
  
  // Setup I2C communication for device-device communication
  setupDeviceCommunication();
}

//! Set up I2C communication for device-device communication.
//!
void setupDeviceCommunication() {
  Wire.begin(I2C_DEVICE_ADDRESS); // Join I2C bus with the device's address
  Wire.onReceive(i2cReceiveHandler);   // Register event handler to receive data from the master I2C device
  Wire.onRequest(i2cRequestHandler);   // Event handler to respond to a request for data from the I2C master device
  
  // Send reboot message to master device
//  insertBehavior(0, 30, 0, 0, 0, 0);
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
  
//  // Create behavior substrate
//  substrate = Create_Substrate ();
//  
//  // Create sequence (i.e., a "(behavior) transformation context"... this is a transformation context for behaviors like a pottery wheel is a context for transforming clay in a particular way) for behaviors
//  // In this analogy, transforming behaviors in a unconstrained graph would be akin to transforming clay with only one's hands (and possibly, but not likely, some basic clay knives... these are likely other things in the analogous situation).
//  Sequence* sequence = Create_Sequence (substrate);
//  boolean isAdded = Update_Sequence_Substrate (sequence, substrate);
  
//  // Create behaviors in the sequence:
//  Sequence* sequence = (*substrate).sequences;
//  int sequenceType = Get_Sequence_Type (sequence);
//  Serial.print ("sequenceType: "); Serial.print (sequenceType); Serial.print ("\n");
//  
//  // Create Input Behavior
//  Behavior* inputBehavior = Create_Input_Behavior (substrate, 5, "digital", "off");
//  Update_Behavior_Sequence (inputBehavior, sequence);
//  
//  // Create Output Behavior
//  Behavior* outputBehavior = Create_Output_Behavior (substrate, 5, "digital", "on");
//  Update_Behavior_Sequence (outputBehavior, sequence);
//  
//  // Create Delay Behavior
//  Behavior* delayBehavior = Create_Delay_Behavior (substrate, 1000);
//  Update_Behavior_Sequence (delayBehavior, sequence);
  
//  // Create performer
//  Performer* performer = Create_Performer (substrate);
  
  // Perform the next behavior
//  while (true) {
    boolean performanceResult = Perform_Behavior (performer);
    delay (800);
//  }

//  Propagator* propagator = Create_Propagator ();
//  Propagation* propagation = Create_Propagation ("create substrate 55ff68064989"); // 55ff68064989495329092587
////  String data = Get_Propagation_Data (propagation);
////  Serial.println (data);
//  Queue_Propagation (propagator, propagation);
//  Propagate (propagator, I2C_CHANNEL);
//  Delete_Propagator (propagator);
//  
//  delay (2000);
  
//  while (true);
}

#define I2C_BUFFER_BYTE_SIZE 32
char i2cBuffer[I2C_BUFFER_BYTE_SIZE];
int i2cBufferSize = 0;
boolean hasMessage = false;

/**
 * function that executes whenever data is received from master
 * this function is registered as an event, see setup()
 */
void i2cReceiveHandler (int howMany) {
  
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
  String split     = String(i2cBuffer);
  String operation = getValue(split, ' ', 0);
  
  // Handle operation
  if (operation.compareTo("reboot") == 0) {
    
    _reboot_Teensyduino_();
//    while(1) { /* NOTE: This is an infinite loop! */ }
    
  } else {
    
    int pin          = getValue(split, ' ', 1).toInt();
    int value        = getValue(split, ' ', 2).toInt();
    
    // Update virtual device state
    // virtualPin[pin].value = value;
    setPinValue(pin, value);
    
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
}

/**
 * function that executes whenever data is requested by master
 * this function is registered as an event, see setup()
 */
void i2cRequestHandler () {
  
//  if ((*propagator).queueSize > 0) {
  
//  Propagator* propagator = Create_Propagator ();
//  Propagation* propagation = Create_Propagation ("create substrate 55ff68064989"); // 55ff68064989495329092587
//  String data = Get_Propagation_Data (propagation);
//  Serial.println (data);
//  Queue_Propagation (propagator, propagation);

//  Propagation* propagation = Create_Propagation ("create substrate 55ff68064"); // 55ff68064989495329092587
//  Queue_Propagation (propagator, propagation);

  Serial.println ("i2cRequestHandler");
  Serial.println ((int) propagator);
  
  if (propagator != NULL) {
    
    Serial.println ((int) (*propagator).propagation);
    
    if ((*propagator).propagation != NULL) {
      
      Propagate (propagator, I2C_CHANNEL);
  //    Delete_Propagator (propagator);
    }
    
  } else {
    
    Wire.write("0 ");
    
  }



  
//  char buf[6]; // "-2147483648\0"
//  
//  if (behaviorTransformationCount > 0) {
//    
//    //Serial.print("behaviorTransformationCount = "); Serial.print(behaviorTransformationCount); Serial.println();
//
//    // Send status
//    Wire.write ("1 ");
//
//    // Send serialized behavior
//    Wire.write (itoa(behaviorTransformations[0].index, buf, 10)); Wire.write (" ");
//    Wire.write (itoa(behaviorTransformations[0].operation, buf, 10)); Wire.write (" ");
//    Wire.write (itoa(behaviorTransformations[0].pin, buf, 10));       Wire.write (" ");
//    // Wire.write (itoa(behaviorTransformations[0].type, buf, 10));      Wire.write (" ");
//    Wire.write (itoa(behaviorTransformations[0].mode, buf, 10));      Wire.write (" ");
//    Wire.write (itoa(behaviorTransformations[0].value, buf, 10));     Wire.write (" ");
//    
//    // Remove the behavior from the processing queue once it's been sent over I2C
//    removeBehaviorTransformation (0);
//  } else {
//    // Send status
//    Wire.write ("0 ");
//  }
}
