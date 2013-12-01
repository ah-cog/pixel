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

#define ENABLE_DEBUG_MODE 0 // Enable RadioBlocks debug mode

//#define MAKEY_INPUT_PIN A0
#define RELAY_ENABLE_PIN 12
//
//#define MAKEY_INPUT_SENSITIVITY 300
//#define MAKEY_INPUT_SENSITIVITY_CEILING 301 // 600

//
// Set up RadioBlocks
//

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
#define MODULE_ID 2
#if MODULE_ID == 2
  #define OUR_ADDRESS   0x1002
  #define THEIR_ADDRESS 0x1003
#elif MODULE_ID == 3
  #define OUR_ADDRESS   0x1003
  #define THEIR_ADDRESS 0x1002
#endif

// The module's pins 1, 2, 3, and 4 are connected to Arduino's pins 5, 4, 3, and 2.
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 8, 4);
 
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

  Serial.println("Pixel Firmware 2013-11-29-19-14-02");

  digitalWrite(STATUS_LED, LOW);
  delay(1500);
 
  Accel_Init();
  Compass_Init();
  Gyro_Init();
  Alt_Init();
  
  delay(20);
  
  for (int i = 0;i < 32; i++) {    // We take some readings...
    Read_Gyro();
    Read_Accel();
    for (int y = 0; y < 6; y++) {   // Cumulate values
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

void loop() {
  
  if (receivedStateMessage != 0) {
    
    // TODO: Check if the current module is active, if the iterator is on this module.
    
    //    if (!check2) { // TODO: Only execute when the incoming state changes... (TODO: Create ~previousPreviousModuleInputState)
    // Keyboard.print("d");
    digitalWrite(RELAY_ENABLE_PIN, HIGH);
    //      Serial.println(averageInputValue);
    
    // Check for transition from opened ("low") to closed ("high")
    //      if (previousAverageInputValue > MAKEY_INPUT_SENSITIVITY_CEILING) {
    //        updateState = true;
    stateMessage = 0x01;
    //      }
    
    //      check2 = !check2;   
//    }
    
  }
  if (receivedStateMessage == 0) {
    // TODO: Check if the current module is active, if the iterator is on this module.
    
//    if (!check2) { // TODO: Only execute when the incoming state changes... (TODO: Create ~previousPreviousModuleInputState)
      // Keyboard.print("d");
      digitalWrite(RELAY_ENABLE_PIN, LOW);
//      Serial.println(averageInputValue);
      
      // Check for transition from opened ("low") to closed ("high")
//      if (previousAverageInputValue < MAKEY_INPUT_SENSITIVITY) {
//        updateState = true;
        stateMessage = 0x00;
//      }
      
//      check2 = !check2;   
//    }
  }
  
  
  
  

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
  
  if (updateState) {

    updateState = false;
    
    interface.setupMessage(THEIR_ADDRESS);
    
    // TL: 0b00000001
    // TR: 0b00000010
    // BL: 0b00000100
    // BR: 0b00001000
    interface.addData(0x1, stateMessage);

    // Send data over the air (OTA)
    interface.sendMessage();
    
    Serial.println("Sent message.");
    
    delay(1200);
  }
  
  //
  // Read an incoming packet if available within the specified number of milliseconds (the timeout value).
  //

  if (interface.readPacket(RADIOBLOCK_PACKET_READ_TIMEOUT)) { // NOTE: Every time this is called, the response returned by getResponse() is overwritten.
//    digitalWrite(RELAY_ENABLE_PIN, HIGH);
//    digitalWrite(13, HIGH);

    if (ENABLE_DEBUG_MODE) {
      Serial.println("Received a packet:");
    }
    
    // Get error code for response
    if (interface.getResponse().getErrorCode() == APP_STATUS_SUCESS) {
      if (ENABLE_DEBUG_MODE) {
        Serial.println("Success: Good packet.");
      }
    } else {
      if (ENABLE_DEBUG_MODE) {
        Serial.println("Failure: Bad packet.");
      }
    }
    if (ENABLE_DEBUG_MODE) {
      Serial.print("Len: ");
      Serial.print(interface.getResponse().getPacketLength(), DEC);
      Serial.print(", Command: ");
      Serial.print(interface.getResponse().getCommandId(), HEX);
      Serial.print(", CRC: ");
      Serial.print(interface.getResponse().getCrc(), HEX); // Cyclic redundancy check (CRC) [Source: http://en.wikipedia.org/wiki/Cyclic_redundancy_check]
      Serial.println("");
    }
    
    //
    // Parse Frame Data
    //
    
    // General command format (sizes are in bytes), Page 4:
    // | Start Byte (1) | Size (1) | Payload (Variable) | CRC (2) |
    
    // COMMAND ID   MEANING
    // 0x20         This command is used to send data over the network, Page 13
    
    int frameDataLength = 0;
    int sendMethod = -1;
    // Send method will be:
    // 0 = unknown
    // 1 = sendData()
    // 2 = sendMessage()
    
    int commandId = -1;
    unsigned int codeAndType = 0;
    unsigned int payloadCode = 0;
    unsigned int payloadDataType = 0;
    
    // We can use this to determine which commands the sending unit used to construct the packet:
    // If the length == 6, the sender used sendData()
    // If the length > 6, the sender used setupMessage(), addData(), and sendMessage() 
    //
    // If the sender used the second method, we need to do more parsing of the payload to pull out
    // the sent data. See "Data or start of payload" below at array offset of 5.
     
     frameDataLength = interface.getResponse().getFrameDataLength();
     if (ENABLE_DEBUG_MODE) {
       Serial.print("Length of Frame Data: ");
       Serial.println(frameDataLength);
     }
     
     // Get the method the sending unit used to construct the packet
     if (frameDataLength == 6) {
       sendMethod = 0; // The sender used sendData()
     } else if (frameDataLength > 6) {
       sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
     }
     
     // The following "meanings" for these bytes are from page 15 of the SimpleMesh_Serial_Protocol.pdf from Colorado Micro Devices.
     if (ENABLE_DEBUG_MODE) {
       Serial.println("Frame Data: ");
     }
     
     //Serial.println(interface.getResponse().getFrameData()[0], HEX);
     //Serial.println(interface.getResponse().getFrameData()[1]);
     //Serial.println(interface.getResponse().getFrameData()[2]);
     
     // Process message based on the its Command ID
     
     commandId = interface.getResponse().getCommandId();
     if (commandId == 0x22) { //APP_COMMAND_DATA_IND) { // 0x22
      
       if (ENABLE_DEBUG_MODE) {
         Serial.print("  Source address: ");
         Serial.println(interface.getResponse().getFrameData()[1], HEX); // Source address
         
         Serial.print("  Frame options: ");
         // 0x00 None
         // 0x01 Acknowledgment was requested
         // 0x02 Security was used
         Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
         
         Serial.print("  Link Quality Indicator: ");
         Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
         
         Serial.print("  Received Signal Strength Indicator: ");
         Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator
       }
       
       // Parse Data or Payload:
       
       if (sendMethod == 0) {
         if (ENABLE_DEBUG_MODE) {
           Serial.print("  Sent Data: ");
           Serial.println(interface.getResponse().getFrameData()[5], HEX);
         }
       } else if (sendMethod == 1) {
         codeAndType = interface.getResponse().getFrameData()[5]; 
         
         
         if (ENABLE_DEBUG_MODE) {
           Serial.print(" Encoded send code and original data type: ");
           Serial.println(codeAndType, HEX); // The actual data
         }
         
         payloadDataType = codeAndType & 0xf;
         payloadCode = (codeAndType >> 4) & 0xf;
         
         if (ENABLE_DEBUG_MODE) {
           Serial.print("  The sent code was (in hex): ");
           Serial.println(payloadCode, HEX);
           Serial.print("  The original data type was: ");
           Serial.println(payloadDataType);
         }
         
         if (payloadDataType == 1) {
           if (ENABLE_DEBUG_MODE) {
             Serial.println("   Data type is TYPE_UINT8. Data:");
             Serial.print("    The data: ");
             Serial.println(interface.getResponse().getFrameData()[6]);
           }
           
           
           // TODO: CHANGE THIS!!! HACKY!!!!
           
           receivedStateMessage = interface.getResponse().getFrameData()[6];
//           digitalWrite(RELAY_ENABLE_PIN, HIGH); // HACK: Turn on output
           
           
           
           
           
           
           
           
         } else if (payloadDataType == 2) {
           if (ENABLE_DEBUG_MODE) {
             Serial.println("   Data type is TYPE_INT8. High and low bytes:");
             Serial.print("    High part: ");
             Serial.println(interface.getResponse().getFrameData()[6]); 
             Serial.print("    Low part: ");
             Serial.println(interface.getResponse().getFrameData()[7]);
           }
         } else if (payloadDataType == 3) {
           Serial.println("   Data type is TYPE_UINT16. High and low bytes:");
           Serial.print("    High part: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    Low part: ");
           Serial.println(interface.getResponse().getFrameData()[7]);
         } else if (payloadDataType == 4) {
           Serial.println("   Data type is TYPE_INT16. High and low bytes:");
           Serial.print("    High part: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    Low part: ");
           Serial.println(interface.getResponse().getFrameData()[7]);
         } else if (payloadDataType == 5) {
           Serial.println("   Data type is TYPE_UINT32. Four bytes:");
           Serial.print("    MSB: ");
           Serial.println(interface.getResponse().getFrameData()[6]); 
           Serial.print("    : ");
           Serial.println(interface.getResponse().getFrameData()[7]);
           Serial.print("    :");
           Serial.println(interface.getResponse().getFrameData()[8]);
           Serial.print("    LSB:");
           Serial.println(interface.getResponse().getFrameData()[9]);
         } else {
           Serial.println("   Data type is not coded for yet...");
           // Debugging: 
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[6]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[7]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[8]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[9]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[10]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[11]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[12]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[13]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[14]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[15]);
           Serial.print("   Raw byte:");
           Serial.println(interface.getResponse().getFrameData()[16]);
           // End debugging
         }
         
       }
     }
  }
}

unsigned long lastJerkUp = 0;
unsigned long lastJerkDown = 0;
void printGesture() {
  
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
