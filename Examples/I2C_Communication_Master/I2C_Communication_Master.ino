#include <Wire.h>

#include "Loop.h"

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

void setup() {
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
  
  
  Wire.begin(1); // Join I2C bus (address optional for master)
  Serial.begin(115200); // Start serial for output
  Serial.println("Master started");
}

byte x = 0;

int slaveDevice = 4; // Slave device address

void sendToSlave(char* text) {
  // Send to slave
  Wire.beginTransmission(slaveDevice); // transmit to device #4
//  Wire.write("x is ");        // sends five bytes
//  Wire.write(x);              // sends one byte  
  Wire.write(text);
  Wire.endTransmission();    // stop transmitting
}

void loop() {
  
  // Send to slave
  Wire.beginTransmission(slaveDevice); // transmit to device #4
  Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte  
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
  
  // Request messages from slave
  Wire.requestFrom(slaveDevice, I2C_MESSAGE_BYTE_SIZE);    // request 6 bytes from slave device #2

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
    int pin       = getValue(split, ' ', 0).toInt();
    
    // Check operation and take handle it accordingly
    if (pin == 2) {
      eraseLoop();
    }
    
    // Parse behavior node's string form
    int operation = getValue(split, ' ', 1).toInt();
    int type      = getValue(split, ' ', 2).toInt();
    int mode      = getValue(split, ' ', 3).toInt();
    int value     = getValue(split, ' ', 4).toInt();
    
    // TODO: Create node object from parsed data (i.e., "Behavior behavior = deserializeBehavior();").
    // TODO: Add node object to queue (i.e., the program) (i.e., "appendBehavior(behavior);")
    appendLoopNode(pin, operation, type, mode, value);
    
//    // TODO: Add print it! (or run it...)
//    Serial.println(value.length());
//    Serial.print("value: ");
//    Serial.print(value);
//    Serial.print("\n");
//    if (pin.compareTo("13") == 0) {
//      Serial.println("PIN 13");
//      
////      if (value2.compareTo("1") == 0) {
//      if (value.toInt() == 1) {
//        Serial.println("ON");
//        digitalWrite(13, HIGH);
//      } else {
//        Serial.println("OFF");
//        digitalWrite(13, LOW);
//        
//        sendToSlave("pin state from master!");
//      }
//    }
//    //Serial.println(pin);
  }
  
  // TODO: Parse incoming I2C message, then create a corresponding behavior node and insert it into the behavior program!
  
  
  // Interpretter:
  updateBehavior();
  
  behaviorLoopStep(); //:
  if (loopSize > 0) {
    BehaviorNode* currentBehavior = &behaviorLoop[loopCounter]; // Get current behavior
    
    // Interpret the behavior
    
    // NOTE: right now, assuming the instruction type... pin I/O
    
    Serial.println((*currentBehavior).pin);
    Serial.println((*currentBehavior).value);
    
    if (true) { // if ((*currentBehavior).instructionType [equals] "pin I/O") 
      
      if ((*currentBehavior).pin == 0) {
        
        Serial.println("DELAY 1000 MS");
          
        int milliseconds = 1000;
        delay(milliseconds);
        
      } else if ((*currentBehavior).pin == 13) {
        Serial.println("PIN 13");
        
        if ((*currentBehavior).value == 1) {
          Serial.println("ON");
          digitalWrite(13, HIGH);
        } else {
          Serial.println("OFF");
          digitalWrite(13, LOW);
          
//          sendToSlave("pin state from master!");
        }
      }
    }
    loopCounter = (loopCounter + 1) % loopSize; // Increment loop counter
    
//    delay(500);
    Serial.print(".");
  }
  
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/**
 * The "behavior" data structure and interpretter
 */
 
void updateBehavior() {
  // TODO: Updates behavior (i.e., the state of the program being interpretted)
}

void behaviorLoopStep() {
}

void parseI2CMessage() {
  
}
