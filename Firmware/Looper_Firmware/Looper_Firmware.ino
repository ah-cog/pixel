/*
"Pixel" Firmware (for the Visual Language, i.e., Looper), Rendition 2
Authors: Michael Gubbels
*/

// Notes for Teensy 3.1 (i.e., heuristics to get the CC3000 module working, based on experience):
// - Set "Board" to Teensy 3.1
// - Set "CPU Speed" to 24 MHz or 48 MHz (96 MHz doesn't seem to work)
// - Set clock to SPI_CLOCK_DIV8 (SPI_CLOCK_DIV2 also worked, but not too reliably)
//
// Notes about Adafruit CC3000 Breakout Board:
// - Firmware V. 1.24

#include <Wire.h>
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "VirtualDevice.h"
#include "WebServer.h"

// TODO: Implement list of changes to make to send to the Master (which executes gestural and the behavior code for the module)
// - TODO: Include status: "new", "sending", "sent", "confirmed" (after which, they're deleted)

// TODO: Implement web server and request handlers that, in response, update the queue of
//       changes to (1) make to Looper, and (2) to queue for sending to the other device 
//       over I2C upon request.

#define DEVICE_SERIAL Serial3

boolean setupBridge () {
  DEVICE_SERIAL.begin (115200);
}

void setup () {
  
  setupBridge ();

  Serial.begin (115200); // Start serial for output
  Serial.println (F ("Looper Firmware"));
  
  setupLooper ();
//  Propagation* transformation = Create_Propagation ("create substrate 55ff68064989"); // 55ff68064989495329092587
//  Queue_Propagation (propagator, transformation);
  
  // Setup Wi-Fi and web server
  setupWebServer ();
}

/**
 * The event loop (i.e., this function is called repeatedly when the board is on)
 */
void loop () {
    
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = httpServer.available ();
  
  if (client) {
    handleClientConnection (celient);
  }
  
  // Propagate data to the main device
  if (propagator != NULL) {
//    Serial.println ((int) (*propagator).transformation);
    if ((*propagator).transformation != NULL) {
      Propagate (propagator, SERIAL_CHANNEL);
  //    Delete_Propagator (propagator);
    }
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
//  Propagation* transformation = Create_Propagation ("create substrate 55ff68064989"); // 55ff68064989495329092587
////  String data = Get_Propagation_Data (transformation);
////  Serial.println (data);
//  Queue_Propagation (propagator, transformation);
//  Propagate (propagator, I2C_CHANNEL);
//  Delete_Propagator (propagator);
//  
//  delay (2000);
  
//  while (true);
}
