#include <Wire.h>

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
  
  Serial.println(i2cMessageBuffer);
  
  // TODO: Parse incoming I2C message, then create a corresponding behavior node and insert it into the behavior program!
  
  
  // Interpretter:
  updateBehavior();
  behaviorLoopStep();
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
