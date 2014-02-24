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
 FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser rGeneral Public License for
 more details.
 
 You should have received a copy of the GNU Lesser General Public License along
 with MinIMU-9-Arduino-AHRS. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>
#include <aJSON.h>
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "utility/debug.h"
#include "utility/socket.h"

#include "Mesh.h" // For mesh network (i.e., the RadioBlocks modules)

/**
 * SD Card Setup
 */

// set up variables using the SD utility library functions:
#define SDCARD_CS 9
Sd2Card card;
SdVolume volume;
SdFile root;

/**
 * Wi-Fi Setup
 */

#define MESH_ENABLED true
#define WIFI_ENABLED false
#define SD_CARD_ENABLED false

// These are the interrupt and control pins for the CC3000 chip
#define ADAFRUIT_CC3000_IRQ   1 // 3  // This MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  4 // This can be any pin
#define ADAFRUIT_CC3000_CS    10 // This can be any pin
// Use hardware SPI for the remaining pins:c
//     Arduino UNO: SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2); // you can change this clock speed

#define WLAN_SSID "Collider" // Cannot be longer than 32 characters!
#define WLAN_PASS "supercollider"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY WLAN_SEC_WPA2 // WLAN_SEC_UNSEC

#define HTTP_LISTEN_PORT 80 // What TCP port to listen on for connections.  The echo protocol uses port 7.

Adafruit_CC3000_Server httpServer(HTTP_LISTEN_PORT);

/**
 * Accelerometer Setup
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
int SENSOR_SIGN[9] = { 1, -1, -1, 1, -1, 1, 1, -1, -1 }; // Correct directions x,y,z - gyro, accelerometer, magnetometer

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
int AN_OFFSET[6] = { 
    0, 0, 0, 0, 0, 0 }; // Array that stores the Offset of the sensors

int gyro_x, gyro_y, gyro_z;
int accel_x, accel_y, accel_z;
int magnetom_x, magnetom_y, magnetom_z;
float c_magnetom_x, c_magnetom_y, c_magnetom_z;
float MAG_Heading;

float Accel_Vector[3] = { 0, 0, 0 }; // Store the acceleration in a vector
float Gyro_Vector[3]  = { 0, 0, 0 }; // Store the gyros turn rate in a vector
float Omega_Vector[3] = { 0, 0, 0 }; // Corrected Gyro_Vector data
float Omega_P[3]      = { 0, 0, 0 }; // Omega Proportional correction
float Omega_I[3]      = { 0, 0, 0 }; // Omega Integrator
float Omega[3]        = { 0, 0, 0 };

// Euler angles (http://en.wikipedia.org/wiki/Euler_angles)
float roll, pitch, yaw;

float errorRollPitch[3] = { 0, 0, 0 }; 
float errorYaw[3]       = { 0, 0, 0 };

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

/**
 * RadioBlocks Setup
 */
 
#define OUR_ADDRESS   0x0001

#define RADIOBLOCK_PACKET_READ_TIMEOUT 100 // 40
#define PAYLOAD_START_INDEX 5 // Index of the first byte in the payload

// The module's pins 1, 2, 3, and 4 are connected to Arduino's pins 5, 4, 3, and 2.
RadioBlockSerialInterface interface = RadioBlockSerialInterface(-1, -1, 7, 8);

/**
 * Idenity Setup
 */

byte uuidNumber[16]; // UUIDs in binary form are 16 bytes long

/**
 * Device Setup
 */

boolean hasCounter = false;
unsigned long lastCount = 0;
#define NEIGHBOR_COUNT 2
unsigned short int neighbors[NEIGHBOR_COUNT];
unsigned short int next[1];

void setup() {

    //
    // Generate a new UUID to identify the device uniquely among all of them
    // 
    
    randomSeed(analogRead(A9)); // Seed pseudo-random number generator
    
    delay(1000);
    
    byte value = EEPROM.read(0);
    if (value == 0xFF) { // Generate UUID and save to EEPROM
        Serial.print("Generating UUID... ");
        generateUuid(uuidNumber, 16); // Generate UUID
        Serial.println("Done.");
        
        // Write to EEPROM
        Serial.print("Writing UUID to EEPROM... ");
        for (int i = 0; i < 16; i++) {
            EEPROM.write(i, (uint8_t) uuidNumber[i]);
            delay(100);
        }
        Serial.println("Done.");
        
        
    } else { // Read UUID from EEPROM
        
        byte value = 0x00; // = EEPROM.read(uuidEepromAddress);
        Serial.print("Reading UUID from EEPROM... ");
        for (int i = 0; i < 16; i++) {
            value = EEPROM.read(i);
            uuidNumber[i] = value;
        }
        Serial.println("Done.");
    }
    
    //
    // Set up RadioBlock module
    //

    interface.setBaud(115200);
    interface.begin();
    delay(1500); // Give RadioBlock time to initialize

    // We need to set these values so other RadioBlocks can find us
    interface.setChannel(15);
    interface.setPanID(0xBAAD);

    interface.setLED(true); delay(100); 
    interface.setLED(false); delay(100);
    interface.setLED(true); delay(100); 
    interface.setLED(false); delay(100);
    interface.setLED(true); delay(100); 
    interface.setLED(false); delay(100);
    interface.setLED(false);

    // Set up address of RadioBlocks interface
    // TODO: Iterate until an address is set. Iterate through addresses to check for availability.
    // TODO: Broadcast message to "Get Acknowledgment State Request" and wait for response until timeout...
    interface.setAddress(OUR_ADDRESS); // TODO: Dynamically set address based on other address in the area (and extended address space from shared state, and add collision fixing.)

    Serial.begin(115200);
    //while (!Serial) { } // wait for serial port to connect. Needed for Leonardo only

    Serial.println(F("Pixel Firmware (Version 2013.12.24.07.44.59)"));


    ledOn(); delay(100); ledOff(); delay(600);
    ledOn(); delay(100); ledOff(); delay(40);
    ledOn(); delay(100); ledOff(); delay(40);
    ledOn(); delay(100); ledOff(); delay(40);
    ledOn(); delay(100); ledOff(); delay(40);
    ledOn(); delay(100); ledOff(); delay(40);
    ledOn();

    //
    // Initialize IMU
    //

    Serial.println("Initializing IMU...");
    I2C_Init();

    Serial.print("UUID: ");
    printUuid(uuidNumber);
    Serial.println();

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

    Serial.println("Offset:");
    for (int y = 0; y < 6; y++) {
        Serial.println(AN_OFFSET[y]);
    }

    delay(1000);

    timer = millis();
    delay(20);
    counter = 0;

    //
    // Setup lights
    //

    ledFadeOut(); // Fade off to indicate ready

    //
    // Initialize WiFI
    //
    
    if (WIFI_ENABLED) {
  
      // Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
      /* Initialise the module */
      Serial.println(F("\nInitializing Wi-Fi..."));
      if (!cc3000.begin()) {
          Serial.println(F("Couldn't begin()! Check your wiring?"));
          while(1);
      }
  
//      uint16_t firmware = checkFirmwareVersion();
//      if ((firmware != 0x113) && (firmware != 0x118)) {
//          hang(F("Wrong firmware version!"));
//      }
  
      Serial.println(F("\tConnecting to AP..."));
      if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
          Serial.println(F("\tFailed!"));
          while(1);
      }
  
      Serial.println(F("\tConnected!"));
  
      Serial.println(F("\tRequesting DHCP"));
      while (!cc3000.checkDHCP()) {
          delay(100); // ToDo: Insert a DHCP timeout!
      }
  
      /* Display the IP address DNS, Gateway, etc. */
      while (! displayConnectionDetails()) {
          delay(1000);
      }
      
      //
      // You can safely remove this to save some flash memory!
      //
      //  Serial.println(F("\r\nNOTE: This sketch may cause problems with other sketches"));
      //  Serial.println(F("since the .disconnect() function is never called, so the"));
      //  Serial.println(F("AP may refuse connection requests from the CC3000 until a"));
      //  Serial.println(F("timeout period passes.  This is normal behaviour since"));
      //  Serial.println(F("there isn't an obvious moment to disconnect with a server.\r\n"));
  
      // Start listening for connections
      httpServer.begin();
  
      Serial.println(F("\tListening for web connections..."));
    }
    
    //
    // Initialize SD Card
    //
    
    if (SD_CARD_ENABLED) {
        
      Serial.print("\nInitializing SD card... ");
      pinMode(SS, OUTPUT); // Hack! This is needed for Wi-Fi to initialize properly! Why?!
  
      // we'll use the initialization code from the utility libraries
      // since we're just testing if the card is working!
      if (!card.init(SPI_HALF_SPEED, SDCARD_CS)) {
          Serial.println("Failed.");
          // Serial.println("* is a card is inserted?");
          // Serial.println("* Is your wiring correct?");
          // Serial.println("* did you change the chip select (CS) pin to match your shield or module?");
          return;
      } else {
          Serial.println("Wiring is correct and a card is present."); 
      }
      
      // Print the type of card
      Serial.print("\nCard type: ");
      switch(card.type()) {
      case SD_CARD_TYPE_SD1:
          Serial.println("SD1");
          break;
      case SD_CARD_TYPE_SD2:
          Serial.println("SD2");
          break;
      case SD_CARD_TYPE_SDHC:
          Serial.println("SDHC");
          break;
      default:
          Serial.println("Unknown");
      }
      
      // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
      if (!volume.init(card)) {
          Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
          return;
      }
      
      // Print the type and size of the first FAT-type volume
      uint32_t volumesize;
      Serial.print("\nVolume type is FAT");
      Serial.println(volume.fatType(), DEC);
      Serial.println();
      
      volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
      volumesize *= volume.clusterCount();       // we'll have a lot of clusters
      volumesize *= 512;                            // SD card blocks are always 512 bytes
      Serial.print("Volume size (bytes): ");
      Serial.println(volumesize);
      Serial.print("Volume size (Kbytes): ");
      volumesize /= 1024;
      Serial.println(volumesize);
      Serial.print("Volume size (Mbytes): ");
      volumesize /= 1024;
      Serial.println(volumesize);
      
      Serial.println("\nFiles found on the card (name, date and size in bytes): ");
      root.openRoot(volume);
  //    if(logFile.open(&root, "log.txt", O_READ)) {
  //        Serial.println("IT WORKED!");
  //    }
      
      // list all files in the card with date and size
      root.ls(LS_R | LS_DATE | LS_SIZE);
  
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
  //    dataFile = SD.open("log.txt", FILE_WRITE);
  //
  //    // if the file is available, write to it:
  //    if (dataFile) {
  //        Serial.println("logFile opened");
  //        //    dataFile.println(dataString);
  //        dataFile.println("TEST,TEST");
  //        dataFile.close();
  //        //    // print to the serial port too:
  //        //    Serial.println(dataString);
  //    }  
  //    // if the file isn't open, pop up an error:
  //    else {
  //        Serial.println("error opening log.txt");
  //    } 
  //
  //
      if (!SD.begin(SDCARD_CS)) {
      }
       
  //    File dataFile = SD.open("datalog.txt", FILE_READ);
  //    if (dataFile) {
  //        
  //        //client.print('size: ' + dataFile.size());
  //        while (dataFile.available()) {
  //            Serial.write(dataFile.read());
  //            //Serial.println("READING FROM FILE...");
  //        }
  //        dataFile.close();
  //        Serial.println("DONE READING FILE");
  //    }
    }
}

// On error, print PROGMEM string to serial monitor and stop
//void hang(const __FlashStringHelper *str) {
//    Serial.println(str);
//    for(;;);
//}

short address = -1;
boolean verifiedAddress = false;
boolean hasValidAddress = false;
boolean hasValidNeighbors = false;
boolean hasInitialized = false;

byte messageQueue = 0x00;

void loop() {
  
  Serial.print("verifiedAddress = ");
  Serial.print(verifiedAddress);
  Serial.print(", hasInitialized = ");
  Serial.print(hasInitialized);
  Serial.print(", hasValidAddress = ");
  Serial.print(hasValidAddress);
  Serial.print(", hasValidNeighbors = ");
  Serial.print(hasValidNeighbors);
  Serial.println();

    //
    // Initialize device (before gestures and communication are available)
    //

//    if (!verifiedAddress) {
//        delay(500);
//        interface.getAddress();
//        verifiedAddress = true;
//    }
    
    if (SD_CARD_ENABLED) {
      File dataFile = SD.open("datalog.txt", FILE_READ);
      if (dataFile) {
          
          //client.print('size: ' + dataFile.size());
          while (dataFile.available()) {
              Serial.write(dataFile.read());
              //Serial.println("READING FROM FILE...");
          }
          dataFile.close();
          Serial.println("DONE READING FILE");
      }
    }
    
    if (!hasValidAddress) {
      address = OUR_ADDRESS;
      hasValidAddress = true;
    }

    // HACK (for testing/debugging):
    verifiedAddress = true;
//    hasInitialized = true;

    if (!hasInitialized) {
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
                } 
                else if (address == 1) {
                    neighbors[0] = 0;
                    neighbors[1] = 2;
                    next[0] = 2;
                    hasCounter = false;
                    hasInitialized = true;
                } 
                else if (address == 2) {
                    neighbors[0] = 0;
                    neighbors[1] = 1;
                    next[0] = 1;
                    hasCounter = false;
                    hasInitialized = true;
                }
                hasValidNeighbors = true;
            }
        }
    }
    
    if (WIFI_ENABLED) {
  
      //
      // Read Web data
      //
  
      if (getWebData()) {
          // Yep!
      }
    }

    //
    // Read RadioBlock data
    //

    if (getMeshData()) {
        // Message was received from a neighbor in the mesh
        if (processMessage()) {
            // Message parsed successfully
        }
    }

    if (hasInitialized) {

        // Primary event loop

        if (sensePhysicalData()) { // Get IMU data
            // storePhysicalData(); //printData();
            if (detectMovements()) {
                // TOOD: Detect gesture (?)
            }
        }

        // detectGestures(); // TODO: Recognize gestures in movement patterns

        // Serial.println(hasCounter + "\t" + lastCount);

        if (hasCounter == true && lastCount == 0) {
            lastCount = millis();
            ledOn();
            Serial.println("Received counter.");
        }

        //    if (!hasCounter) {
        //      hasCounter = true;
        //      lastCount = 0;
        //      // update time that counter received
        //    }


        //
        // Check if current node has the counter
        //

        if (hasCounter) {
            unsigned long currentTime = millis();
            if (currentTime - lastCount > 1000) {
                ledOff();
                clearCounter();
                sendCounter();
                //        hasCounter = false;
                //        lastCount = 0L;
            }
        }

    }
}



void storePhysicalData() {
//    if (dataFile) {
//        dataFile.flush();
//        dataFile.close();
//    }
//    if (!SD.begin(SDCARD_CS)
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
        dataFile.println("000t000\t000");
        dataFile.flush();
        dataFile.close();
        // print to the serial port too:
        //  Serial.println(dataString);
        Serial.println("Wrote physical data to log file.");
        
//         dataFile = SD.open("log.txt", FILE_READ);
//        if (dataFile) {
//            
//            //client.print('size: ' + dataFile.size());
//            while (dataFile.available()) {
//                Serial.write(dataFile.read());
//                //Serial.println("READING FROM FILE...");
//            }
//            dataFile.close();
//            Serial.println("DONE READING FILE");
//        }
        
        return;

        //  Serial.print("ANG:");
        dataFile.print(ToDeg(roll)); // Roll (Rotation about X)
        dataFile.print("\t");
        dataFile.print(ToDeg(pitch)); // Pitch (Rotation about Y)
        dataFile.print("\t");
        dataFile.print(ToDeg(yaw)); // Yaw (Rotation about Z)

        //  Serial.print(",AN:");
        dataFile.print("\t");
        dataFile.print(AN[0]);  //(int)read_adc(0) // Gyro X
        dataFile.print("\t");
        dataFile.print(AN[1]); // Gyro Y
        dataFile.print("\t");
        dataFile.print(AN[2]); // Gyro Z
        dataFile.print("\t");
        dataFile.print(AN[3]); // Accelerometer X
        dataFile.print("\t");
        dataFile.print(AN[4]); // Accelerometer Y
        dataFile.print("\t");
        dataFile.print(AN[5]); // Accelerometer Z
        dataFile.print("\t");
        dataFile.print(c_magnetom_x);
        dataFile.print("\t");
        dataFile.print(c_magnetom_y);
        dataFile.print("\t");
        dataFile.print(c_magnetom_z);
        dataFile.print ("\t");
        dataFile.print(pressure);
        dataFile.print ("\t");
        dataFile.print(altitude);
        dataFile.print ("\t");
        dataFile.print(temperature);

        dataFile.flush();

        dataFile.close();
        // print to the serial port too:
        //  Serial.println(dataString);
        Serial.println("Wrote physical data to log file.");
    } else { // if the file isn't open, pop up an error:
        Serial.println("error opening log.txt");
    } 
}



void setCounter() {
    hasCounter = true;
    lastCount = 0;
}

void clearCounter() {
    hasCounter = false;
}



void sendConfirm(short sourceAddress) {
    //We use the 'setupMessage()' call if we want to use a bunch of data,
    //otherwise can use sendData() calls to directly send a few bytes

        Serial.println("sendConfirm()");

    return;

    for (int i = 0; i < NEIGHBOR_COUNT; i++) {
        //This is the OTHER guys address
        //    interface.setupMessage(THEIR_ADDRESS);
        //    interface.setupMessage(0x00);
        //interface.setupMessage(neighbors[i]);
        interface.setupMessage(next[i]);

        // Package the data payload for transmission
        interface.addData(1, (byte) 0x01); // TYPE_INT8
        interface.sendMessage(); //Send data OTA
    }
}



void sendGesture(char gestureCode) {
    //We use the 'setupMessage()' call if we want to use a bunch of data,
    //otherwise can use sendData() calls to directly send a few bytes

    Serial.println("sendGesture()");
    //hasCounter = true;
    setCounter();

    delay(200);

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
        // Set the destination address
        interface.setupMessage(next[i]);

        // Package the data payload for transmission
        interface.addData(1, (byte) 0x1F); // TYPE_INT8
        interface.sendMessage(); // Send data OTA

        // Wait for confirmation
        // delayUntilConfirmation();
    }
}

//boolean delayUntilConfirmation() {
////  if (interface.readPacket(RADIOBLOCK_PACKET_READ_TIMEOUT)) {
////  }
//  readPacketUntilAvailable();
//  // TODO: Wait until receive "confirmed receive" message received
//}

unsigned long lastJerkUp = 0;
unsigned long lastJerkDown = 0;
boolean detectMovements() {

    // Check for "Jerk Up"
    if (AN[5] > (255 + 400)) {
        unsigned long currentTime = millis();
        Serial.println("Jerk up");
        lastJerkUp = currentTime; // Update time of last jerk up
    }

    // Check for "Jerk Down"
    if ((255 - 400) > AN[5]) {
        unsigned long currentTime = millis();
        Serial.println("Jerk down");
        lastJerkDown = currentTime; // Update time of last jerk down

        sendGesture(2);
        return true;
    }

    // Check for "Shake" (Jerk Up + Jerk Down multiple times within a certain time period)
    //  if (-10 < AN[5] && AN[5] < 10) {
    //    Serial.println("Shaking");
    //  }

    // Check for "Freefall"
    if (-10 < AN[5] && AN[5] < 10) {
        //    Serial.println("Free fall");
    }

    // Check for "Lift Up"
    if ((255 + 100) < AN[5]) {
        ledOn();

        unsigned long currentTime = millis();
        Serial.println("Pick Up");
        lastJerkUp = currentTime; // Update time of last jerk up

        sendGesture(1);
        return true;
    }

    // Check for "Set Down"
    if (AN[5] < (255 - 200)) {
        ledFadeOut();

        unsigned long currentTime = millis();
        Serial.println("Set Down");
        lastJerkUp = currentTime; // Update time of last jerk up

        sendGesture(0);
        return true;
    }

    return false;
}

boolean detectGestures() {
    return true;
}

void ledOn() {
    analogWrite(6, 0);
}

void ledOff() {
    analogWrite(6, 255);
}

void ledFadeOut() {
    // Fade out from max to min in increments of 5 points
    // delay(50);
    for(int fadeValue = 0; fadeValue <= 255; fadeValue += 15) {
        analogWrite(6, fadeValue); // Sets the value (range from 0 to 255)
        delay(10); // wait for a few milliseconds to see the dimming effect      
    }
    analogWrite(6, 255);
}

/**
 * Read data from the inertial measurement unit (IMU) sensors.
 */
// TODO: Create a timer interrupt that calls this at 50 Hz
boolean sensePhysicalData() {

    if ((millis() - timer) >= 20) { // Main loop runs at 50Hz

        counter++;
        timer_old = timer;
        timer = millis();
        if (timer > timer_old) {
            G_Dt = (timer-timer_old) / 1000.0; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
        } 
        else {
            G_Dt = 0;
        }

        // > start of DCM algorithm

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
        // ^ end of DCM algorithm

        return true; // Return true when the IMU data were updated

    } 
    else {
        return false; // Return false when not updated
    }
}

/**
 * Read received (and buffered) data from the RadioBlock.
 */
boolean getMeshData() {

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

            } 
            else if (commandId == APP_COMMAND_DATA_IND) { // (i.e., 0x22) [Page 15]

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
                //        Serial.print("  Payload length: ");
                //        Serial.println(frameDataLength, DEC); // "Payload" length (minus "Command Id" (1 byte))

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
                //        Serial.print("  Frame options: ");
                //        Serial.println(interface.getResponse().getFrameData()[2], HEX); // Frame options
                //  
                //        Serial.print("  Link Quality Indicator: ");
                //        Serial.println(interface.getResponse().getFrameData()[3], HEX); // Link quality indicator
                //  
                //        Serial.print("  Received Signal Strength Indicator: ");
                //        Serial.println(interface.getResponse().getFrameData()[4], HEX); // Received Signal Strength Indicator

                // NOTE: Payload starts at index position 5

                //
                // Parse payload data
                //

                //        int PAYLOAD_START_INDEX = 5; // Index of first byte of "Payload"

                //        Serial.print("Raw Payload Data: [ ");
                //        for (int i = 0; i < (frameDataLength - PAYLOAD_START_INDEX - 1); i++) {
                //          Serial.print(interface.getResponse().getFrameData()[5 + i] , HEX);
                //          Serial.print(" : ");
                //        }
                //        Serial.print(interface.getResponse().getFrameData()[5 + (frameDataLength - PAYLOAD_START_INDEX - 1)], HEX);
                //        Serial.println(" ]");
                //        
                //        Serial.println("Parsed Payload Data:");

                // Get the method the sending unit used to construct the packet
                int sendMethod = -1;
                if (frameDataLength == 6) {
                    sendMethod = 0; // The sender used sendData(). More data is expected to come along...
                } 
                else if (frameDataLength > 6) {
                    sendMethod = 1; // The sender used setupMessage(), addData(), and sendMessage()
                }

                // Parse payload data based on whether sendData() or the three functions setupMessage(), 
                // addData(), and sendMessage() were used.

                if (sendMethod != -1) { // Check if sendMethod is valid... if < 6, no data was attached...

                    // "sendData()" was used, so only one byte of data was sent (since this function sends only one byte).
                    // Therefore, extract the one byte of data from the first byte of the "Payload". [Page 15]
                    if (sendMethod == 0) {

                        //            Serial.print("  Sent Data: ");
                        //            Serial.println(interface.getResponse().getFrameData()[5], HEX);

                    } 
                    else if (sendMethod == 1) {

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
                                Serial.println("WARNING: loopCount > maxLoopCount");
                                interface.getResponse().reset(); // TODO: Possibly remove this. This might be a bad idea.
                                clearCounter();
                                return false;
                                break;
                            }

                            codeAndType = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset];
                            //              Serial.print(" Encoded send code and original data type: ");
                            //              Serial.println(codeAndType, HEX); // The actual data

                            payloadDataType = codeAndType & 0xF;
                            payloadCode = (codeAndType >> 4) & 0xF;
                            //              Serial.print("  The sent code was (in hex): ");
                            //              Serial.println(payloadCode, HEX);
                            //              Serial.print("  The original data type was: ");
                            //              Serial.println(payloadDataType);

                            //
                            // Extract and type cast the data based on data type
                            //

                            if (payloadDataType == TYPE_UINT8) {

                                Serial.println("   Data type is TYPE_UINT8. High and low bytes:");
                                Serial.print("    High part: ");
                                Serial.println(interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1]);

                                // Append message to message queue
                                messageQueue = interface.getResponse().getFrameData()[PAYLOAD_START_INDEX + payloadOffset + 1];

                                payloadOffset = payloadOffset + sizeof(unsigned char) + 1;

                            } 
                            else if (payloadDataType == TYPE_INT8) {

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

                        //
                        // Process received data, update state
                        //

                        // Turn on if received counter
                        //            setCounter();

                    }
                } 

                // Return true if a packet was read (i.e., received) successfully
                // TODO: Add "return true" and "return false" cases to different conditions above, in this block.
                return true;

            } 
            else if (commandId == APP_COMMAND_DATA_CONF) {

                Serial.println("APP_COMMAND_DATA_CONF");

                return false; // Return true if a packet was read (i.e., received) successfully

            } 
            else if (commandId == APP_COMMAND_GET_ADDR_RESP) { // (i.e., 0x25) [Page 15]

                Serial.print("  Frame Command Id: ");
                Serial.println(interface.getResponse().getFrameData()[0], HEX); // Frame options

                // Computer source address using bitwise operators (combine two bytes into a "short", a 16-bit integer data type)
                address = interface.getResponse().getFrameData()[0] | ((short) interface.getResponse().getFrameData()[1] << 8);
                Serial.print("  Device address: ");
                Serial.println(address, HEX); // Source address

                hasValidAddress = true;

                return false; // Return true if a packet was read (i.e., received) successfully
            }

        } 
        else {

            Serial.println("Error: Failed to receive packet.");
            // TODO: Clear the buffer, check if this is causing messages to stop sending back and forth.
            // TODO: Reset!!
            interface.getResponse().reset(); // TODO: Possibly remove this. This might be a bad idea.
            clearCounter();
            return false; // Return true if a packet was read (i.e., received) successfully
        }

        // return true; // Return true if a packet was read (i.e., received) successfully

    } 
    else { // Timeout or error occurred
        return false;
    }
}

/**
 * Read received (and buffered) data from the Internet.
 */
boolean getWebData() {
    char clientline[100];
    int index = 0;
    
    // Try to get a client which is connected.
    Adafruit_CC3000_ClientRef client = httpServer.available();
    if (client) {
//        Serial.println("Client");
        boolean currentLineIsBlank = true;
        // reset the input buffer
        index = 0;
        while(client.connected()) {
            // Serial.println("Client connected");
            if (client.available() > 0) { // Check if there is data available to read.
                // Serial.println("Client available");
                
                uint8_t c = client.read(); // Read a byte and write it to all clients
                //client.write(c);
        
                // If it isn't a new line, add the character to the buffer
                if (c != '\n' && c != '\r') {
                    clientline[index] = c;
                    index++;
                    // are we too big for the buffer? start tossing out data
                    if (index >= BUFSIZ) {
                        index = BUFSIZ -1;
                    }
                    
                    // continue to read more data!
                    continue;
                }
                
                // got a \n or \r new line, which means the first line of the request string is done
                clientline[index] = 0;
                
                // Print it out for debugging
                Serial.println(clientline);

                if (c == '\n' && currentLineIsBlank) {
                    
                    // GET /
                    if (strstr(clientline, "GET / ") != 0) {
                        
                        // send a standard http response header
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");  // the connection will be closed after completion of the response
                        // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
                        client.println();
                        client.println("<!DOCTYPE HTML>");
                        client.println("<html>");
                        client.println("<head>");
                        client.println("<link href='http://fonts.googleapis.com/css?family=Josefin+Sans:100,300,400,600,700,100italic,300italic,400italic,600italic,700italic|Comfortaa:400,300,700' rel='stylesheet' type='text/css'>");
                        client.println("<script src='https://cdn.firebase.com/v0/firebase.js'></script>");
                        client.println("<script src='https://ajax.googleapis.com/ajax/libs/jquery/1.9.0/jquery.min.js'></script>");
                        // client.println("<script src='https://raw.github.com/NeilFraser/JS-Interpreter/master/interpreter.js'></script>");
                        client.println("</head>");
                        client.println("<body style=\"background-color: #e75e53; font-family: 'Josefin Sans', sans-serif;\">");
                        client.println("<h1 style=\"font-family: 'Comfortaa', cursive; font-size: 72px; font-weight: normal; color: #96d4f0;\">pixel</h1>");
                        client.println("<h4 style=\"font-family: 'Comfortaa', cursive; font-size: 14px; font-weight: normal; color: #ffd966ff;\">firmware & introspection</h4>");
//                        // output the value of each analog input pin
//                        for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//                            int sensorReading = analogRead(analogChannel);
//                            client.print("analog input ");
//                            client.print(analogChannel);
//                            client.print(" is ");
//                            client.print(sensorReading);
//                            client.println("<br />");
//                        }
                        client.println("</body>");
                        client.println("</html>");
                        break;
                        
                    // GET /<filename>
                    } else if (strstr(clientline, "GET /") != 0) {
                        
                      // this time no space after the /, so a sub-file!
                      char *filename;
                      
                      filename = clientline + 5; // look after the "GET /" (5 chars)
                      // a little trick, look for the " HTTP/1.1" string and 
                      // turn the first character of the substring into a 0 to clear it out.
                      (strstr(clientline, " HTTP"))[0] = 0;
                      
                      // print the file we want
                      Serial.println(filename);
                      
                      // TODO: Open file
                      
                      Serial.println("Opened!");
                                
                      client.println("HTTP/1.1 200 OK");
                      client.println("Content-Type: text/plain");
                      client.println();
                      
                      // TODO: Write file contents into response for client
                      
                      break;

                    // POST /led/13
                    } else if (strstr(clientline, "POST /led/13") != 0) {
                        pinMode(13, OUTPUT);
                        digitalWrite(13, HIGH);
                        // TODO: Finish reading the input... the "body" data, JSON.
                        // TODO: Invoke callback, both any set HTTP callback, the local callback, the mesh callback, and any HTTP client callbacks.
                        break;
                    }
                }

                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } 
                
                else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }

        // Delay to give the web browser time to receive the data
        delay(1);

        // Close the connection to the client
        client.close();
    }

    return true;
}

//boolean postData() {
//     Adafruit_CC3000_Client client = cc3000.connectTCP(ip, 80);
//    if (client.connected()) {
//        Serial.println("Connected!");
//        //    client.println("POST /message_list.json HTTP/1.0");
//        //    client.println("Host: api.xively.com");
//        //    client.println("X-ApiKey: " + String(API_key));
//        //    client.println("Content-Length: " + String(length));
//        //    client.print("Connection: close");
//        //    client.println();
//        //    client.print(data);
//        //    client.println();
//    } 
//    else {
//        Serial.println(F("Connection failed"));    
//        return;
//    }
//    
//    return true;
//}

//boolean sendRequest2(Adafruit_CC3000_ClientRef client) {
//    Serial.print(F("OK\r\nIssuing HTTP request..."));
//
//    // Unlike the hash prep, parameters in the HTTP request don't require sorting.
//    client.fastrprint(F("POST "));
//    client.fastrprint(F2(endpoint));
//    client.fastrprint(F(" HTTP/1.1\r\nHost: "));
//    client.fastrprint(host);
//    client.fastrprint(F("\r\nUser-Agent: "));
//    client.fastrprint(F2(agent));
//    client.fastrprint(F("\r\nConnection: close\r\n"
//                       "Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n"
//                       "Content-Length: "));
//    client.print(7 + encodedLength(msg));
//    client.fastrprint(F("\r\nAuthorization: Oauth oauth_consumer_key=\""));
//    client.fastrprint(F2(consumer_key));
//    client.fastrprint(F("\", oauth_nonce=\""));
//    client.fastrprint(nonce);
//    client.fastrprint(F("\", oauth_signature=\""));
//    urlEncode(client, b64, false, false);
//    client.fastrprint(F("\", oauth_signature_method=\"HMAC-SHA1\", oauth_timestamp=\""));
//    client.fastrprint(searchTime);
//    client.fastrprint(F("\", oauth_token=\""));
//    client.fastrprint(F2(access_token));
//    client.fastrprint(F("\", oauth_version=\"1.0\"\r\n\r\nstatus="));
//    urlEncode(client, msg, false, false);
//
//    Serial.print(F("OK\r\nAwaiting response..."));
//    int c = 0;
//    // Dirty trick: instead of parsing results, just look for opening
//    // curly brace indicating the start of a successful JSON response.
//    while(((c = timedRead()) > 0) && (c != '{'));
//    if(c == '{')   Serial.println(F("success!"));
//    else if(c < 0) Serial.println(F("timeout"));
//    else           Serial.println(F("error (invalid Twitter credentials?)"));
//    client.close();
//    return (c == '{');
//}

/**
 * Process received message data, update state
 */
boolean processMessage() {

    // TODO: Dequeue message from front of the queue
    byte message = messageQueue;

    if (message == 0x1F) {
        Serial.println("processMessage(0x1F)");
        setCounter(); // Turn on if received counter
        messageQueue = 0x00; // TODO: Free the message from memory
        return true;
    } 
    else if (message == 0x00) {
        // NOOP (i.e., "no operation")
        return true;
    }

    return false; // By default, return false
}









//
// Tries to read the IP address and other connection details
//
bool displayConnectionDetails (void) {
    uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

    if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv)) {
        //    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
        return false;
    } 
    else {
        Serial.print(F("\nIP Addr: ")); 
        cc3000.printIPdotsRev(ipAddress);
        //    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
        //    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
        //    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
        //    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
        Serial.println();
        return true;
    }
}

