/*
MinIMU-9-Arduino-AHRS
Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)

Copyright (c) 2011 Pololu Corporation.
http://www.pololu.com/

MinIMU-9-Arduino-AHRS is based on sf9domahrs by Doug Weibel and Jose Julio:
http://code.google.com/p/sf9domahrs/

sf9domahrs is based on ArduIMU v1.5 by Jordi Munoz and William Premerlani, Jose
Julio and Doug Weibel:
http://code.google.com/p/ardu-imu/

MinIMU-9-Arduino-AHRS is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

MinIMU-9-Arduino-AHRS is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License along
with MinIMU-9-Arduino-AHRS. If not, see <http://www.gnu.org/licenses/>.
*/

// Uncomment the below line to use this axis definition: 
// X axis pointing forward
// Y axis pointing to the right 
// and Z axis pointing down.
// Positive pitch : nose up
// Positive roll : right wing down
// Positive yaw : clockwise
//int SENSOR_SIGN[9] = { 1, 1, 1, -1, -1, -1, 1, 1, 1 }; //Correct directions x,y,z - gyro, accelerometer, magnetometer

// Uncomment the below line to use this axis definition: 
   // X axis pointing forward
   // Y axis pointing to the left 
   // and Z axis pointing up.
// Positive pitch : nose down
// Positive roll : right wing down
// Positive yaw : counterclockwise
//int SENSOR_SIGN[9] = {1,-1,-1,-1,1,1,1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer

int SENSOR_SIGN[9] = { 1,-1,-1, 1,-1, 1, 1,-1,-1 }; // Correct directions x,y,z - gyro, accelerometer, magnetometer

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>

// LSM303 accelerometer (8g sensitivity)
// 3.8 mg/digit; 1 g = 256
#define GRAVITY 256  // this equivalent to 1G in the raw data coming from the accelerometer 

#define ToRad(x) ((x) * 0.01745329252)  // *pi/180
#define ToDeg(x) ((x) * 57.2957795131)  // *180/pi

// L3G4200D gyro (2000 dps full scale)
// 70 mdps/digit; 1 dps = 0.07
#define Gyro_Gain_X 0.07 // X axis Gyro gain
#define Gyro_Gain_Y 0.07 // Y axis Gyro gain
#define Gyro_Gain_Z 0.07 // Z axis Gyro gain
#define Gyro_Scaled_X(x) ((x) * ToRad(Gyro_Gain_X)) // Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) ((x) * ToRad(Gyro_Gain_Y)) // Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) ((x) * ToRad(Gyro_Gain_Z)) // Return the scaled ADC raw data of the gyro in radians for second

// LSM303 magnetometer calibration constants; use the Calibrate example from
// the Pololu LSM303 library to find the right values for your board
// TODO: Automate this calibration, make it on-going (?). Store the most-recent values in EEPROM.
#define M_X_MIN -447 // -421
#define M_Y_MIN -619 // -639
#define M_Z_MIN -515 // -238
#define M_X_MAX 595 // 424
#define M_Y_MAX 564 // 295
#define M_Z_MAX 487 // 472
//M min X: -540 Y: -655 Z: -570 M max X: 675 Y: 565 Z: 559

#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

/*For debugging purposes*/
//IMU_OUTPUT_MODE=1 will print the corrected data, 
//IMU_OUTPUT_MODE=0 will print uncorrected data of the gyros (with drift)
#define IMU_OUTPUT_MODE 1

//#define PRINT_DCM 0     // Will print the whole direction cosine matrix
#define PRINT_ANALOGS 1 // Will print the analog raw data
#define PRINT_EULER 1   // Will print the Euler angles Roll, Pitch and Yaw

float G_Dt = 0.02;    // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

long timer = 0;   // General purpuse timer
long timer_old;
long timer24 = 0; // Second timer used to print values 
int AN[6]; // Array that stores the gyro and accelerometer data
int AN_OFFSET[6] = { 0, 0, 0, 0, 0, 0 }; // Array that stores the Offset of the sensors

int gyro_x, gyro_y, gyro_z;
int accel_x, accel_y, accel_z;
int magnetom_x, magnetom_y, magnetom_z;
float c_magnetom_x, c_magnetom_y, c_magnetom_z;
float MAG_Heading;

float Accel_Vector[3] = { 0, 0, 0 }; // Store the acceleration in a vector
float Gyro_Vector[3] = { 0, 0, 0 };// Store the gyros turn rate in a vector
float Omega_Vector[3] = { 0, 0, 0 }; // Corrected Gyro_Vector data
float Omega_P[3] = { 0, 0, 0 };// Omega Proportional correction
float Omega_I[3] = { 0, 0, 0 };// Omega Integrator
float Omega[3] = { 0, 0, 0 };

// Euler angles (http://en.wikipedia.org/wiki/Euler_angles)
float roll;
float pitch;
float yaw;

float avgRoll = 0;
float avgPitch = 0;
float avgYaw = 0;
int avgCount = 0;

float errorRollPitch[3] = { 0, 0, 0 }; 
float errorYaw[3] = { 0, 0, 0 };

unsigned int counter = 0;
byte gyro_sat = 0;

float pressure = 0;
float altitude = 0;
float temperature = 0;

// The "Direction Cosine Matrix" (DCM), also known as the "Rotation Matrix"
float DCM_Matrix[3][3] = {
  { 1, 0, 0 },
  { 0, 1, 0 },
  { 0, 0, 1 }
}; 
float Update_Matrix[3][3] = { // Gyros here
  { 0, 1, 2 },
  { 3, 4, 5 },
  { 6, 7, 8 }
};

float Temporary_Matrix[3][3] = {
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 }
};

//
// Set up RadioBlocks
//

#define ENABLE_DEBUG_MODE 0 // Enable RadioBlocks debug mode
#define RADIOBLOCK_PACKET_READ_TIMEOUT 10

// The module's pins 1, 2, 3, and 4 are connected to Arduino's pins 5, 4, 3, and 2.
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 8, 4);

unsigned long lastSendTime = 0;

bool hasCounter = 0;
unsigned long lastCount = 0;
#define NEIGHBOR_COUNT 2
unsigned short int neighbors[NEIGHBOR_COUNT];
unsigned short int next[1];

// These #define's are copied from the RadioBlock.cpp file.
#define TYPE_UINT8 	1
#define TYPE_INT8	2
#define	TYPE_UINT16	3
#define TYPE_INT16	4
#define TYPE_UINT32	5
#define TYPE_INT32	6
#define TYPE_UINT64	7
#define TYPE_INT64	8
#define TYPE_FLOAT	9
#define TYPE_FIXED8_8	10
#define TYPE_FIXED16_8	11
#define TYPE_8BYTES	12
#define TYPE_16BYTES	13
#define TYPE_ASCII	14
 
void setup() {
  
  //
  // Set up RadioBlock module
  //
  
  interface.begin(); 
  
  // Give RadioBlock time to initialize
  delay(500);
  
  // We need to set these values so other RadioBlocks can find us
  interface.setChannel(15);
  interface.setPanID(0xBAAD);
  
  // Set up address of RadioBlocks interface
  // TODO: Iterate until an address is set. Iterate through addresses to check for availability.
  // Broadcast message to "Get Acknowledgment State Request" and wait for response until timeout...
//  interface.setAddress(OUR_ADDRESS); // TODO: Dynamically set address based on other address in the area (and extended address space from shared state, and add collision fixing.)
  
  Serial.begin(115200);
 
  //
  // Initialize IMU
  //
  
  I2C_Init();

  Serial.println("Pixel Firmware (Version 2013.12.14.22.58.36)");

  delay(1500);
 
  Accel_Init();
  Compass_Init();
  Gyro_Init();
  Alt_Init();
  
  delay(20); // Wait for a small duration for the IMU sensors to initialize (?)
  
  for (int i = 0;i < 32; i++) { // We take some initial readings... (to warm the IMU up?)
    Read_Gyro();
    Read_Accel();
    for (int y = 0; y < 6; y++) { // Cumulate values
      AN_OFFSET[y] += AN[y];
    }
    delay(20);
  }
    
  for (int y = 0; y < 6; y++) {
    AN_OFFSET[y] = AN_OFFSET[y]/32;
  }
  
  AN_OFFSET[5] -= GRAVITY * SENSOR_SIGN[5];
  
  // Serial.println("Offset:");
  for (int y = 0; y < 6; y++) {
    Serial.println(AN_OFFSET[y]);
  }
  
  delay(2000);
    
  timer = millis();
  delay(20);
  counter = 0;
  
  //
  // Setup lights
  //
  
  // TODO:
}

short address = -1;
boolean verifiedAddress = false;
boolean hasValidAddress = false;
boolean hasValidNeighbors = false;
boolean hasInitialized = false;

void loop() {
  
  if (!verifiedAddress) {
    delay(500);
    interface.getAddress();
    verifiedAddress = true;
  }
  
  if (verifiedAddress) {
    if (hasValidAddress && !hasValidNeighbors) {
      Serial.println("Manually setting neighbors.");
      // Listen for address
      // neighbors
      if (address == 0) {
        neighbors[0] = 1;
        neighbors[1] = 2;
        next[0] = 1;
        hasCounter = false;
        hasInitialized = true;
      } else if (address == 1) {
        neighbors[0] = 0;
        neighbors[1] = 2;
        next[0] = 0;
        hasCounter = false;
        hasInitialized = true;
      } else if (address == 2) {
        neighbors[0] = 0;
        neighbors[1] = 1;
        next[0] = 0;
        hasCounter = false;
        hasInitialized = true;
      }
      hasValidNeighbors = true;
    }
  }
  
  //
  // Read RadioBlock data
  //

  if (interface.readPacket(RADIOBLOCK_PACKET_READ_TIMEOUT)) { // Waits a maximum of <i>timeout</i> milliseconds for a response packet before timing out; returns true if packet is read. Returns false if timeout or error occurs.
    // interface.readPacket(); // Read the packet (NOTE: Seemingly must do this for isAvailable() to work properly.)
    // if (interface.getResponse().isAvailable()) {
    // TODO: Change this to interface.isAvailable() so it doesn't block and wait for any time, so it just reads "when available" (in quotes because it's doing polling)
  
    // General command format (sizes are in bytes), Page 4:
    //
    // [ Start Byte (1) | Size (1) | Payload (Variable) | CRC (2) ]
    //
    // - Start Byte (1 byte)
    // - Size (1 byte): Size of the command id + options + payload field
    // - Payload (Variable): command payload
    // - CRC (2 bytes): 16 bit CRC calculated over payload with initial value 0x1234 (i.e., it is 
    //                  calculated over the command id, options, and payload if any.)
    
    // Data request command format (this is the Payload field in the command format):
    // 
    // [ Command ID (1) | Destination (2) | Options (1) | Handle (1) ]
    //
    // NOTE: I believe this constitutes a "frame".
    
    // Data response command format (this is the Payload field in the command format):
    // 
    // [ Command ID (1) | Source Address (2) | Options (1) | LQI (1) | RSSI (1) | Payload (Variable) ]
    //
    // NOTE: I believe this constitutes a "frame".
    
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      Serial.println("\n\n\n\nReceived packet.");
      
      int commandId = interface.getResponse().getCommandId();
      
      if (commandId == APP_COMMAND_ACK) {
        // "Every command is confirmed with an acknowledgment command even if it is impossible
       //   to immediately execute the command. There is no particular order in which responses 
       //   are sent, so for example Data Indication Command might be sent before 
       //   Acknowledgment Command."
       
       // Acknowledgment command format: Page 5 of SimpleMesh Serial Protocol document.
        
        Serial.println("Received Ack");
        
        Serial.print("  Status: ");
        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Source address
        
      } else if (commandId == APP_COMMAND_DATA_IND) { // (i.e., 0x22) [Page 15]
      
  //        Serial.print(interface.getResponse().getFrameData()[0], HEX); // Frame options
  //        Serial.print(" | ");
  //        Serial.print(interface.getResponse().getFrameData()[1], HEX); // Frame options
  //        Serial.print(" | ");
  //        Serial.print(interface.getResponse().getFrameData()[2], HEX); // Frame options
  //        Serial.print(" | ");
  //        Serial.print(interface.getResponse().getFrameData()[3], HEX); // Frame options
  //        Serial.print(" | ");
  //        Serial.print(interface.getResponse().getFrameData()[4], HEX); // Frame options
      
  
        int frameDataLength = interface.getResponse().getFrameDataLength();
        Serial.print("  Payload length: ");
        Serial.println(frameDataLength, DEC); // "Payload" length (minus "Command Id" (1 byte))
        
  //        Serial.print("  Frame Command Id: ");
  //        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
        
        // Compute source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
        short sourceAddress = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
        Serial.print("  Source address: ");
        Serial.println(sourceAddress, HEX); // Source address
  
        // Frame Options [Page 15]
        // i.e.,
        // 0x00 None
        // 0x01 Acknowledgment was requested
        // 0x02 Security was used
        Serial.print("  Frame options: ");
        Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
  
        Serial.print("  Link Quality Indicator: ");
        Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
  
        Serial.print("  Received Signal Strength Indicator: ");
        Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator
        
        // NOTE: Payload starts at index position 5
        
        //
        // Parse payload data
        //
        
//#define PAYLOAD_START_INDEX 5
        
        int PAYLOAD_START_INDEX = 5; // Index of first byte of "Payload"
        
        Serial.print("Raw Payload Data: [ ");
        for (int i = 0; i < (frameDataLength - PAYLOAD_START_INDEX - 1); i++) {
          Serial.print(interface.getResponse().getFrameData()[5 + i] , HEX);
          Serial.print(" : ");
        }
        Serial.print(interface.getResponse().getFrameData()[5 + (frameDataLength - PAYLOAD_START_INDEX - 1)], HEX);
        Serial.println(" ]");
        
        Serial.println("Parsed Payload Data:");
        
        // Get the method the sending unit used to construct the packet
        int sendMethod = -1;
        if (frameDataLength == 6) {
          sendMethod = 0; // The sender used sendData(). More data is expected to come along...
        } else if (frameDataLength > 6) {
          sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
        }
        
        // Parse payload data based on whether sendData() or the three functions setupMessage(), 
        // addData(), and sendMessage() were used.
        
        if (sendMethod != -1) { // Check if sendMethod is valid... if < 6, no data was attached...
        
          // "sendData()" was used, so only one byte of data was sent (since this function sends only one byte).
          // Therefore, extract the one byte of data from the first byte of the "Payload". [Page 15]
          if (sendMethod == 0) {
            
            Serial.print("  Sent Data: ");
            Serial.println(interface.getResponse().getFrameData()[5], HEX);
            
          } else if (sendMethod == 1) {
            
            // The "Payload" field is packed in pairs, each pair consisting of a 1 byte code followed by a 
            // variable number of bytes of data, determinable by the 1 byte code.
            
            unsigned int codeAndType = 0;
            unsigned int payloadCode = 0;
            unsigned int payloadDataType = 0;
            
            int payloadOffset = 0;
            
            int loopCount = 0;
            int maxLoopCount = 20;
            
            while(payloadOffset < (frameDataLength - PAYLOAD_START_INDEX)) {
              
              // Protect against infinite loop with this conditional
              if (loopCount > maxLoopCount) {
                break;
              }
              
              codeAndType = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset];
              Serial.print(" Encoded send code and original data type: ");
              Serial.println(codeAndType, HEX); // The actual data
              
              payloadDataType = codeAndType & 0xF;
              payloadCode = (codeAndType >> 4) & 0xF;
              Serial.print("  The sent code was (in hex): ");
              Serial.println(payloadCode, HEX);
              Serial.print("  The original data type was: ");
              Serial.println(payloadDataType);
            
              //
              // Extract and type cast the data based on data type
              //
              
              if (payloadDataType == TYPE_UINT8) {
                
                Serial.println("   Data type is TYPE_UINT8. High and low bytes:");
                Serial.print("    High part: ");
                Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);
                
                payloadOffset = payloadOffset + sizeof(unsigned char) + 1;
                
                
                
                
                
                //
                // Process received data, update state
                //
        
                // Turn on if received counter
                if (!hasCounter) {
                  hasCounter = true;
                  lastCount = 0;
                  // update time that counter received
                }
                ledOn();
                
              } else if (payloadDataType == TYPE_INT8) {
                
                Serial.println("   Data type is TYPE_INT8. High and low bytes:");
                Serial.print("    High part: ");
                Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);
                
                payloadOffset = payloadOffset + sizeof(char) + 1;
                
              }
              
              /* 
              else if (payloadDataType == TYPE_UINT16) {
                
                Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
                Serial.print("    High part: ");
                Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]); 
                Serial.print("    Low part: ");
                Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 2]);
                
                short unsigned int data = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1] << 8 | ((unsigned short int) interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 2]);
                Serial.print("    Value: ");
                Serial.println(data);
              }
              */
              
              loopCount++;
              
            } // while()
            
          }
        }
        
      } else if (commandId == APP_COMMAND_GET_ADDR_RESP) { // (i.e., 0x25) [Page 15]
        
  //        Serial.print("  Frame Command Id: ");
  //        Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options
        
        // Computer source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
        address = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
        Serial.print("  Device address: ");
        Serial.println(address, HEX); // Source address
        
        hasValidAddress = true;
      
      }
    } else {
      Serial.println("Error: Failed to receive packet.");
    }
  } 
  
  if (hasInitialized) {
   
    //
    // Get IMU data
    //
    
    if ((millis() - timer) >= 20) { // Main loop runs at 50Hz
    
      counter++;
      timer_old = timer;
      timer = millis();
      if (timer > timer_old) {
        G_Dt = (timer-timer_old) / 1000.0;    // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
      } else {
        G_Dt = 0;
      }
  
      // *** DCM algorithm
      // Data adquisition
      Read_Gyro(); // This read gyro data
      Read_Accel(); // Read I2C accelerometer
      
      if (counter > 5) { // Read compass data at 10 Hz... (5 loop runs)
        counter = 0;
        Read_Compass(); // Read I2C magnetometer
        Compass_Heading(); // Calculate magnetic heading
      }
        
      // Read pressure/altimeter
      Read_Altimeter();
      
      // Calculations...
      Matrix_update(); 
      Normalize();
      Drift_correction();
      Euler_angles();
      // ***
      
  //    if (avgCount < 10) {
  //      avgRoll += roll;
  //      avgPitch += pitch;
  //      avgYaw += yaw;
  //      
  //      avgCount++;
  //    } else {
  //      avgRoll = avgRoll / 10;
  //      avgPitch = avgPitch / 10;
  //      avgYaw = avgYaw / 10;
  //      Serial.print("!");
  //      Serial.print(ToDeg(avgRoll)); // Roll (Rotation about X)
  //      Serial.print("\t");
  //      Serial.print(ToDeg(avgPitch)); // Pitch (Rotation about Y)
  //      Serial.print("\t");
  //      Serial.println(ToDeg(avgYaw)); // Yaw (Rotation about Z)
  //      avgRoll = roll;
  //      avgPitch = pitch;
  //      avgYaw = yaw;
  //      
  //      avgCount = 0;
  //    }
      //printData();
      detectGesture();
    }
    
    if (hasCounter && lastCount == 0) {
      Serial.println("Received counter.");
      lastCount = millis();
    }
    
    //
    // Check if current node has the counter
    //
    
    if (hasCounter) {
      if (millis() - lastCount > 2000) {
        sendCounter();
        hasCounter = false;
        lastCount = 0L;
        ledOff();
      }
    }
    
  }
}



void sendGesture(char gestureCode) {
  //We use the 'setupMessage()' call if we want to use a bunch of data,
  //otherwise can use sendData() calls to directly send a few bytes
  
  Serial.println("sendGesture()");
  hasCounter = true;
  
  delay(500);
  
  return;
  
  for (int i = 0; i < NEIGHBOR_COUNT; i++) {
    //This is the OTHER guys address
//    interface.setupMessage(THEIR_ADDRESS);
//    interface.setupMessage(0x00);
    //interface.setupMessage(neighbors[i]);
    interface.setupMessage(next[i]);
    
    // Package the data payload for transmission
    interface.addData(1, (byte) gestureCode); // TYPE_INT8
    interface.sendMessage(); //Send data OTA
  }
  
  delay(500);
}

void sendCounter() {
  Serial.println("sendCounter()");
  
  for (int i = 0; i < 1; i++) {
    //This is the OTHER guys address
//    interface.setupMessage(THEIR_ADDRESS);
//    interface.setupMessage(0x00);
    //interface.setupMessage(neighbors[i]);
    interface.setupMessage(next[i]);
    
    // Package the data payload for transmission
    interface.addData(1, (byte) 0x00); // TYPE_INT8
    interface.sendMessage(); //Send data OTA
  }
}

unsigned long lastJerkUp = 0;
unsigned long lastJerkDown = 0;
void detectGesture() {
  
  // Jerk Up
  if (AN[5] > (255 + 400)) {
    unsigned long currentTime = millis();
    Serial.println("Jerk up");
    lastJerkUp = currentTime; // Update time of last jerk up
  }
  
  // Jerk Down
  if ((255 - 400) > AN[5]) {
    unsigned long currentTime = millis();
    Serial.println("Jerk down");
    lastJerkDown = currentTime; // Update time of last jerk down
    
    sendGesture(2);
  }
  
  // Shake (Jerk Up + Jerk Down multiple times within a certain time period)
//  if (-10 < AN[5] && AN[5] < 10) {
//    Serial.println("Shaking");
//  }
  
  // Freefall
  if (-10 < AN[5] && AN[5] < 10) {
//    Serial.println("Free fall");
  }
  
  // Pickup
  if ((255 + 100) < AN[5]) {
    ledOn();
    
    unsigned long currentTime = millis();
    Serial.println("Pick Up");
    lastJerkUp = currentTime; // Update time of last jerk up
    
    sendGesture(1);
  }
  
  // Set down
  if (AN[5] < (255 - 200)) {
    ledFadeOut();
    
    unsigned long currentTime = millis();
    Serial.println("Set Down");
    lastJerkUp = currentTime; // Update time of last jerk up
    
    sendGesture(0);
  }
}

void ledOn() {
  analogWrite(6, 0);
}

void ledOff() {
  analogWrite(6, 255);
}

void ledFadeOut() {
  // Fade out from max to min in increments of 5 points
  //delay(50);
  for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=15) { 
    // sets the value (range from 0 to 255):
    analogWrite(6, fadeValue);
    delay(10); // wait for a few milliseconds to see the dimming effect      
  }
}
