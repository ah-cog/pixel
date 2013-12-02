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
int SENSOR_SIGN[9] = { 1, 1, 1, -1, -1, -1, 1, 1, 1 }; //Correct directions x,y,z - gyro, accelerometer, magnetometer

// Uncomment the below line to use this axis definition: 
   // X axis pointing forward
   // Y axis pointing to the left 
   // and Z axis pointing up.
// Positive pitch : nose down (rotation about Y)
// Positive roll : right wing down (rotation about X)
// Positive yaw : counterclockwise (rotation about Z)
//int SENSOR_SIGN[9] = {
//  1, // gyro x
//  -1, // gyro y
//  -1, // gyro z
//  -1, // accel x
//  1, // accel y
//  1, // accel z
//  1, // compass x
//  -1, // compass y
//  -1 // compass z
//}; // Correct directions x,y,z - gyro, accelerometer, magnetometer

// tested with Arduino Uno with ATmega328 and Arduino Duemilanove with ATMega168

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>

// LSM303 accelerometer (8g sensitivity)
// 3.8 mg/digit; 1 g = 256
#define GRAVITY 256  //this equivalent to 1G in the raw data coming from the accelerometer 

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
// TODO: Automate this calibration. Store the values in EEPROM.
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

#define STATUS_LED 13 

float G_Dt = 0.02;    // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

long timer = 0;   // General purpuse timer
long timer_old;
long timer24 = 0; // Second timer used to print values 
int AN[6]; // Array that stores the gyro and accelerometer data
int AN_OFFSET[6] = { 0, 0, 0, 0, 0, 0 }; // Array that stores the Offset of the sensors

int gyro_x;
int gyro_y;
int gyro_z;
int accel_x;
int accel_y;
int accel_z;
int magnetom_x;
int magnetom_y;
int magnetom_z;
float c_magnetom_x;
float c_magnetom_y;
float c_magnetom_z;
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
#define RADIOBLOCK_PACKET_READ_TIMEOUT 5

unsigned char receivedStateMessage = 0x00;
unsigned char stateMessage = 0x00;
bool updateState = false;

// Set our known network addresses. How do we deal with 
// unexpected nodes...? This should be dynamic, and nodes should
// self-assign their addresses and broadcast the the mesh network.
// TODO: Broadcast asking for ACK from addresses in address space. 
//       If no direct response is given and no node responds on 
//       behalf of the requested address, take the address. Resolve 
//       or negotiate any collisions later if the address shows up.
//#define MODULE_ID 2
//#if MODULE_ID == 2
//  #define OUR_ADDRESS   0x0002
//  #define THEIR_ADDRESS 0x0003
//#elif MODULE_ID == 3
//  #define OUR_ADDRESS   0x0003
//  #define THEIR_ADDRESS 0x0002
//#endif

#define OUR_ADDRESS   0x0002
#define THEIR_ADDRESS 0x0003

// The module's pins 1, 2, 3, and 4 are connected to Arduino's pins 5, 4, 3, and 2.
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 8, 4);

unsigned short int neighbors[4];

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
  interface.setAddress(OUR_ADDRESS); // TODO: Dynamically set address based on other address in the area (and extended address space from shared state, and add collision fixing.)
  
  Serial.begin(115200);
  pinMode(STATUS_LED, OUTPUT); // Status LED
 
  //
  // Initialize IMU
  //
  
  I2C_Init();

  Serial.println("Pixel Firmware (Version 2013.12.01.17.50.58)");

  digitalWrite(STATUS_LED, LOW);
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
  digitalWrite(STATUS_LED, HIGH);
    
  timer = millis();
  delay(20);
  counter = 0;
}

short address = -1;
boolean verifiedAddress = false;
boolean hasValidAddress = false;
boolean hasValidNeighbors = false;
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
        neighbors[0] = 2;
      } else if (address == 2) {
        neighbors[0] = 0;
      }
      hasValidNeighbors = true;
    }
  }
  
  
  
  //
  // Read RadioBlock data
  //

  if (interface.readPacket(1000)) { // Waits a maximum of <i>timeout</i> milliseconds for a response packet before timing out; returns true if packet is read. Returns false if timeout or error occurs.
  //interface.readPacket(); // Read the packet (NOTE: Seemingly must do this for isAvailable() to work properly.)
  //if (interface.getResponse().isAvailable()) {
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
        
        int payloadStartIndex = 5; // Index of first byte of "Payload"
        
        Serial.print("Raw Payload Data: [ ");
        for (int i = 0; i < (frameDataLength - payloadStartIndex - 1); i++) {
          Serial.print(interface.getResponse().getFrameData()[5 + i] , HEX);
          Serial.print(" : ");
        }
        Serial.print(interface.getResponse().getFrameData()[5 + (frameDataLength - payloadStartIndex - 1)], HEX);
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
            
            while(payloadOffset < (frameDataLength - payloadStartIndex)) {
              
              // Protect against infinite loop with this conditional
              if (loopCount > maxLoopCount) {
                break;
              }
              
              codeAndType = interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset];
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
                Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]);
                
                payloadOffset = payloadOffset + sizeof(unsigned char) + 1;
                
              } else if (payloadDataType == TYPE_INT8) {
                
                Serial.println("   Data type is TYPE_INT8. High and low bytes:");
                Serial.print("    High part: ");
                Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]);
                
                payloadOffset = payloadOffset + sizeof(char) + 1;
                
              } 
  //            else if (payloadDataType == TYPE_UINT16) {
  //              
  //              Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
  //              Serial.print("    High part: ");
  //              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1]); 
  //              Serial.print("    Low part: ");
  //              Serial.println(interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2]);
  //              
  //              short unsigned int data = interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 1] << 8 | ((unsigned short int) interface.getResponse().getFrameData()[payloadStartIndex + payloadOffset + 2]);
  //              Serial.print("    Value: ");
  //              Serial.println(data);
  //            }
              
              loopCount++;
              
            } // while()
          }
        }
  
        /// TODO...
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
    
//    Serial.print("Packet length: ");
//    Serial.print(interface.getResponse().getPacketLength(), DEC);
//    Serial.print(", Command: ");
//    Serial.print(interface.getResponse().getCommandId(), HEX);
//    Serial.print(", Payload: <below>");
//    Serial.print(", CRC: ");
//    Serial.print(interface.getResponse().getCrc(), HEX);
//    Serial.println("");
  }
  
  
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
    Read_Alt();
    
    // Calculations...
    Matrix_update(); 
    Normalize();
    Drift_correction();
    Euler_angles();
    // ***
    
//    printData();
    printGesture();
  }
  
  //
  // Send state to other module
  //
  
//  if (updateState) {
//
//    updateState = false;
//    
//    interface.setupMessage(THEIR_ADDRESS);
//    
//    // TL: 0b00000001
//    // TR: 0b00000010
//    // BL: 0b00000100
//    // BR: 0b00001000
//    interface.addData(0x1, stateMessage);
//
//    // Send data over the air (OTA)
//    interface.sendMessage();
//    
//    Serial.println("Sent message.");
//    
//    delay(1200);
//  }
  
  //
  // Read an incoming packet if available within the specified number of milliseconds (the timeout value).
  //

//  if (interface.readPacket(RADIOBLOCK_PACKET_READ_TIMEOUT)) { // NOTE: Every time this is called, the response returned by getResponse() is overwritten.
////    digitalWrite(RELAY_ENABLE_PIN, HIGH);
////    digitalWrite(13, HIGH);
//
//    if (ENABLE_DEBUG_MODE) {
//      Serial.println("Received a packet:");
//    }
//    
//    // Get error code for response
//    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
//      if (ENABLE_DEBUG_MODE) {
//        Serial.println("Success: Good packet.");
//      }
//    } else {
//      if (ENABLE_DEBUG_MODE) {
//        Serial.println("Failure: Bad packet.");
//      }
//    }
//    if (ENABLE_DEBUG_MODE) {
//      Serial.print("Len: ");
//      Serial.print(interface.getResponse().getPacketLength(), DEC);
//      Serial.print(", Command: ");
//      Serial.print(interface.getResponse().getCommandId(), HEX);
//      Serial.print(", CRC: ");
//      Serial.print(interface.getResponse().getCrc(), HEX); // Cyclic redundancy check (CRC) [Source: http://en.wikipedia.org/wiki/Cyclic_redundancy_check]
//      Serial.println("");
//    }
//    
//    //
//    // Parse Frame Data
//    //
//    
//    // General command format (sizes are in bytes), Page 4:
//    // | Start Byte (1) | Size (1) | Payload (Variable) | CRC (2) |
//    
//    // COMMAND ID   MEANING
//    // 0x20         This command is used to send data over the network, Page 13
//    
//    int frameDataLength = 0;
//    int sendMethod = -1;
//    // Send method will be:
//    // 0 = unknown
//    // 1 = sendData()
//    // 2 = sendMessage()
//    
//    int commandId = -1;
//    unsigned int codeAndType = 0;
//    unsigned int payloadCode = 0;
//    unsigned int payloadDataType = 0;
//    
//    // We can use this to determine which commands the sending unit used to construct the packet:
//    // If the length == 6, the sender used sendData()
//    // If the length > 6, the sender used setupMessage(), addData(), and sendMessage() 
//    //
//    // If the sender used the second method, we need to do more parsing of the payload to pull out
//    // the sent data. See "Data or start of payload" below at array offset of 5.
//     
//     frameDataLength = interface.getResponse().getFrameDataLength();
//     if (ENABLE_DEBUG_MODE) {
//       Serial.print("Length of Frame Data: ");
//       Serial.println(frameDataLength);
//     }
//     
//     // Get the method the sending unit used to construct the packet
//     if (frameDataLength == 6) {
//       sendMethod = 0; // The sender used sendData()
//     } else if (frameDataLength > 6) {
//       sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
//     }
//     
//     // The following "meanings" for these bytes are from page 15 of the SimpleMesh_Serial_Protocol.pdf from Colorado Micro Devices.
//     if (ENABLE_DEBUG_MODE) {
//       Serial.println("Frame Data: ");
//     }
//     
//     //Serial.println(interface.getResponse().getFrameData()[0], HEX);
//     //Serial.println(interface.getResponse().getFrameData()[1]);
//     //Serial.println(interface.getResponse().getFrameData()[2]);
//     
//     // Process message based on the its Command ID
//     
//     commandId = interface.getResponse().getCommandId();
//     if (commandId == 0x22) { //APP_COMMAND_DATA_IND) { // 0x22
//      
//       if (ENABLE_DEBUG_MODE) {
//         Serial.print("  Source address: ");
//         Serial.println(interface.getResponse().getFrameData()[1], HEX); // Source address
//         
//         Serial.print("  Frame options: ");
//         // 0x00 None
//         // 0x01 Acknowledgment was requested
//         // 0x02 Security was used
//         Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
//         
//         Serial.print("  Link Quality Indicator: ");
//         Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
//         
//         Serial.print("  Received Signal Strength Indicator: ");
//         Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator
//       }
//       
//       // Parse Data or Payload:
//       
//       if (sendMethod == 0) {
//         if (ENABLE_DEBUG_MODE) {
//           Serial.print("  Sent Data: ");
//           Serial.println(interface.getResponse().getFrameData()[5], HEX);
//         }
//       } else if (sendMethod == 1) {
//         codeAndType = interface.getResponse().getFrameData()[5]; 
//         
//         
//         if (ENABLE_DEBUG_MODE) {
//           Serial.print(" Encoded send code and original data type: ");
//           Serial.println(codeAndType, HEX); // The actual data
//         }
//         
//         payloadDataType = codeAndType & 0xf;
//         payloadCode = (codeAndType >> 4) & 0xf;
//         
//         if (ENABLE_DEBUG_MODE) {
//           Serial.print("  The sent code was (in hex): ");
//           Serial.println(payloadCode, HEX);
//           Serial.print("  The original data type was: ");
//           Serial.println(payloadDataType);
//         }
//         
//         if (payloadDataType == 1) {
//           if (ENABLE_DEBUG_MODE) {
//             Serial.println("   Data type is TYPE_UINT8. Data:");
//             Serial.print("    The data: ");
//             Serial.println(interface.getResponse().getFrameData()[6]);
//           }
//           
//           
//           // TODO: CHANGE THIS!!! HACKY!!!!
//           
//           receivedStateMessage = interface.getResponse().getFrameData()[6];
////           digitalWrite(RELAY_ENABLE_PIN, HIGH); // HACK: Turn on output
//           
//           
//           
//           
//           
//           
//           
//           
//         } else if (payloadDataType == 2) {
//           if (ENABLE_DEBUG_MODE) {
//             Serial.println("   Data type is TYPE_INT8. High and low bytes:");
//             Serial.print("    High part: ");
//             Serial.println(interface.getResponse().getFrameData()[6]); 
//             Serial.print("    Low part: ");
//             Serial.println(interface.getResponse().getFrameData()[7]);
//           }
//         } else if (payloadDataType == 3) {
//           Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
//           Serial.print("    High part: ");
//           Serial.println(interface.getResponse().getFrameData()[6]); 
//           Serial.print("    Low part: ");
//           Serial.println(interface.getResponse().getFrameData()[7]);
//         } else if (payloadDataType == 4) {
//           Serial.println("   Data type is TYPE_INT16. High and low bytes:");
//           Serial.print("    High part: ");
//           Serial.println(interface.getResponse().getFrameData()[6]); 
//           Serial.print("    Low part: ");
//           Serial.println(interface.getResponse().getFrameData()[7]);
//         } else if (payloadDataType == 5) {
//           Serial.println("   Data type is TYPE_UINT32. Four bytes:");
//           Serial.print("    MSB: ");
//           Serial.println(interface.getResponse().getFrameData()[6]); 
//           Serial.print("    : ");
//           Serial.println(interface.getResponse().getFrameData()[7]);
//           Serial.print("    :");
//           Serial.println(interface.getResponse().getFrameData()[8]);
//           Serial.print("    LSB:");
//           Serial.println(interface.getResponse().getFrameData()[9]);
//         } else {
//           Serial.println("   Data type is not coded for yet...");
//           // Debugging: 
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[6]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[7]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[8]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[9]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[10]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[11]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[12]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[13]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[14]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[15]);
//           Serial.print("   Raw byte:");
//           Serial.println(interface.getResponse().getFrameData()[16]);
//           // End debugging
//         }
//         
//       }
//     }
//  }
}



void sendGesture(char gestureCode) {
  //We use the 'setupMessage()' call if we want to use a bunch of data,
  //otherwise can use sendData() calls to directly send a few bytes
  
  Serial.println("sendGesture()");
  
  //This is the OTHER guys address
  interface.setupMessage(THEIR_ADDRESS);
  
  // Package the data payload for transmission
  interface.addData(1, (byte) gestureCode); // TYPE_INT8
  interface.sendMessage(); //Send data OTA
  
  delay(500);
}

unsigned long lastJerkUp = 0;
unsigned long lastJerkDown = 0;
void printGesture() {
  
  // Jerk Up
  if (AN[5] > (255 + 400)) {
    unsigned long currentTime = millis();
    Serial.println("Jerk up");
    lastJerkUp = currentTime; // Update time of last jerk up
    
    sendGesture(1);
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
//    Serial.println("Pick up");
  }
  
  // Set down
  if (AN[5] < (255 - 200)) {
//    Serial.println("Set down");
  }
}
