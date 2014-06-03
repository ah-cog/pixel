#ifndef I2C_H
#define I2C_H

#define I2C_MESSAGE_BYTE_SIZE 20

char i2cMessageBuffer[128] = { 0 };
int i2cMessageBufferSize = 0;

// Message format:
// 00000000 00000000 00000000 00000000 00000000 00000000
// 
// Messages:
// - Pop next message
// - Clear all messages
// - Request top message (but don't remove it)
// - Remove top message (but don't send it, send removal status)

int slaveDevice = 2; // Slave device address

String getValue (String data, char separator, int index);

// TODO: Move this to "Ports.h"
void sendToSlave(char* text) {
  // Send to slave
  Wire.beginTransmission(slaveDevice); // transmit to device #4
//  Wire.write("x is ");        // sends five bytes
//  Wire.write(x);              // sends one byte  
  Wire.write(text);
  Wire.endTransmission();    // stop transmitting
}

// TODO: Move this to "Ports.h"
void digitalWritePin(int pin, int value) {
  
  // TODO: Consider not having this?
  pinMode(0, INPUT);
  
  // TODO: Include "pin mode" and "analog, digital, touch, pwm" in the output
  
  // Write to the pin
  digitalWrite(pin, value);
  
  // Update the state
  char buf[4]; // "-2147483648\0"
  Wire.beginTransmission(slaveDevice); // transmit to device #4
  Wire.write("pin ");
  Wire.write(itoa(pin, buf, 10)); // The pin number
  Wire.write(" ");
  Wire.write(value == HIGH ? "1" : "0"); // The pin's value
  //  Wire.write(";");        // sends five bytes
  Wire.endTransmission();    // stop transmitting 
}

int digitalReadPin(int pin) {
  
  // Write to the pin
  int value = digitalRead(pin);
  
  // Update the state
  char buf[4]; // "-2147483648\0"
  Wire.beginTransmission(slaveDevice); // transmit to device #4
  Wire.write("pin ");
  Wire.write(itoa(pin, buf, 10)); // The pin number
  Wire.write(" ");
  Wire.write(value == HIGH ? "1" : "0");
  //  Wire.write(";");        // sends five bytes
  Wire.endTransmission();    // stop transmitting 
  
  return value;
}



#define PIN_READ_DIGITAL 0
#define PIN_WRITE_DIGITAL 1
#define DELAY 2
#define ERASE 3

#define I2C_CONFIRM_UPDATE_STATE 1

/**
 * The "behavior" data structure and interpretter
 */
// TODO: Updates behavior (i.e., the state of the program being interpretted)
void updateBehavior() {
  // Request messages from slave
  Wire.requestFrom(slaveDevice, I2C_MESSAGE_BYTE_SIZE); // Request 6 bytes from slave device #2

  while(Wire.available ()) { // slave may send less than requested
  
    char c = Wire.read (); // receive a byte as character
//    Serial.print(c); // print the character
    
    // Copy byte into message buffer
    i2cMessageBuffer[i2cMessageBufferSize] = c;
    i2cMessageBufferSize++;
  }
  i2cMessageBuffer[i2cMessageBufferSize] = '\0'; // Terminate I2C message buffer

  i2cMessageBufferSize = 0; // Reset I2C message buffer size
  
  // TODO: Parse data
  if (strlen(i2cMessageBuffer) > 0) {
//  if (i2cMessageBufferSize > 0) {
    Serial.println(i2cMessageBuffer);
    
    // Split message by space
    String split = String(i2cMessageBuffer); // "hi this is a split test";
    
    // Parse instruction message relayed by the "slave" device from "Looper"
//    String pin       = getValue(split, ' ', 0).toInt();
//    String operation = getValue(split, ' ', 1).toInt();
//    String type      = getValue(split, ' ', 2).toInt();
//    String mode      = getValue(split, ' ', 3).toInt();
//    String value     = getValue(split, ' ', 4).toInt();
    int operation = getValue(split, ' ', 0).toInt();
    
    // Check operation and take handle it accordingly
    if (operation == ERASE) {
      
      eraseLoop();
      
    } else {
    
      // Parse behavior node's string form
      int pin       = getValue(split, ' ', 1).toInt();
      int type      = getValue(split, ' ', 2).toInt();
      int mode      = getValue(split, ' ', 3).toInt();
      int value     = getValue(split, ' ', 4).toInt();
      
      // TODO: Create node object from parsed data (i.e., "Behavior behavior = deserializeBehavior();").
      // TODO: Add node object to queue (i.e., the program) (i.e., "appendBehavior(behavior);")
      appendLoopNode(pin, operation, type, mode, value);
      
    }
    
  }
}




/**
 * TODO: Move this to "Loop.h" (and/or break it down into "Behavior.h")
 */
void behaviorLoopStep() {
  
  if (loopSize > 0) {
    Behavior* currentBehavior = &behaviorLoop[loopCounter]; // Get current behavior
    
    // Interpret the behavior
    
    // NOTE: right now, assuming the instruction type... pin I/O
    
//    Serial.println((*currentBehavior).pin);
//    Serial.println((*currentBehavior).value);
    
    if (true) { // if ((*currentBehavior).instructionType [equals] "pin I/O") 
    
      if ((*currentBehavior).operation == PIN_READ_DIGITAL) { // Read pin state
      
        int pinValue = digitalReadPin((*currentBehavior).pin);
      
//        int pinValue = digitalRead((*currentBehavior).pin);
//         Wire.beginTransmission(slaveDevice); // transmit to device #4
//  Wire.write("pin");
//          if (pinValue == 1) {
//          Wire.write("HIGH");
//        } else {
//          Wire.write("LOW");
//        }
//  Wire.endTransmission();    // stop transmitting
      
      } else if ((*currentBehavior).operation == PIN_WRITE_DIGITAL) { // Write to pin
      
        //digitalWrite((*currentBehavior).pin, ((*currentBehavior).value == 1 ? HIGH : LOW));
        digitalWritePin((*currentBehavior).pin, ((*currentBehavior).value == 1 ? HIGH : LOW));
        
        // TODO: Update state of memory model of self
      
//        if ((*currentBehavior).pin == 13) {
//          Serial.println("PIN 13");
//          
//          if ((*currentBehavior).value == 1) {
//            Serial.println("ON");
//            digitalWrite(13, HIGH);
//          } else {
//            Serial.println("OFF");
//            digitalWrite(13, LOW);
//            
//  //          sendToSlave("pin state from master!");
//          }
//        }
        
      } else if ((*currentBehavior).operation == DELAY) {
        
        Serial.println("DELAY 1000 MS");
          
        int milliseconds = 1000;
        delay(milliseconds);
        
      } else if ((*currentBehavior).operation == ERASE) {
        
        Serial.println("DELAY 1000 MS");
          
        int milliseconds = 1000;
        delay(milliseconds);
        
      }
    }
    
    // Advance the loop behavior counter
    if (loopSize == 0) {
      loopCounter = 0;
    }
    loopCounter = (loopCounter + 1) % loopSize; // Increment loop counter
    
    
    // Serial.print(".");
  }
}

String getValue (String data, char separator, int index) {
  
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring (strIndex[0], strIndex[1]) : "";
}

#endif
