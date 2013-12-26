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
#include <SPI.h>
#include <SD.h>

#include <MovingAvarageFilter.h>

MovingAvarageFilter movingAvarageFilter(20);

/**
 * SD Card Setup
 */

// set up variables using the SD utility library functions:
#define SDCARD_CS 9
Sd2Card card;
SdVolume volume;
SdFile root;
File dataFile;

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

#define GESTURE_COUNT 13
String gestures[GESTURE_COUNT] = {
    "at rest, on table",
    "at rest, in hand",
    "lift up",
    "set down",
    "tilt left",
    "tilt right",
    "tap to another, as left",
    "tap to another, as right",
    "double tap to another, as left",
    "double tap to another, as right",
    "shake, up and down",
    "shake, left to right",
    "shake, back to front"
};

String gestureDuration[GESTURE_COUNT] = { 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 };

void setup() {

    Serial.begin(115200);
    while (!Serial) { } // wait for serial port to connect. Needed for Leonardo only

    Serial.println(F("Pixel Firmware (Version 2013.12.24.07.44.59)"));

    //
    // Initialize IMU
    //

    Serial.println("Initializing IMU...");
    I2C_Init();

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
    // Initialize SD Card
    //

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
    
    // Clear file
    File dataFile = SD.open("datalog.txt", O_TRUNC);
    dataFile.close();
    
    delay (1000);
}

void calibrateVideo() {
    Serial.println("Count down to synchronize video (countdown at 1 second frequency)");
    for (int i = 10; i >= 0; i--) {
        Serial.print(i);
        Serial.println("...");
        delay(1000);
    }
}

unsigned long dataTime = 0UL;
unsigned long lastLoopTime = 0UL;

int gestureIndex;

void loop() {
    
    //lastLoopTime = millis();
    
    for (gestureIndex = 0; gestureIndex < GESTURE_COUNT; gestureIndex++) {
        
        dataFile = SD.open("GestureData.txt", FILE_WRITE);
        
        // Prompt for a gesture
        for (int i = 0; i < 100; i++) { Serial.println("\n"); }
        Serial.println("Do a \"" + gestures[gestureIndex] + "\" gesture.");
        
        char inChar = 0;
        while (inChar == 0) {
            if (Serial.available() > 0) {
                inChar = Serial.read();
            }
        }
        
        // Countdown
        delay(500);
        for (int i = 6; i >= 0; i--) {
            Serial.print(i);
            Serial.println("...");
            delay(1000);
        }
        Serial.println("Go!");
        delay(800);
    
        // Collect data for gesture
        lastLoopTime = millis();
        dataTime = lastLoopTime;
        while ((dataTime - lastLoopTime) < (30 * 1000)) {
            // Sense phsyical orientation data
            if (sensePhysicalData()) {
                //storePhysicalData();
                printData();
            }
            
            // Set timestamp of data capture
            dataTime = millis();
        }
        
        dataFile.flush();
        dataFile.close();
    }
}

void storePhysicalData() {

    if (dataFile) {
        
         // Time in milliseconds
        dataFile.print(dataTime);
        dataFile.print("\t");
        
        // Time in seconds
        dataFile.print(dataTime / 1000.0);
        dataFile.print("\t");
        
        // Gesture
        dataFile.print(gestures[gestureIndex]);
        dataFile.print("\t");

        //  Orientation angles (in degrees)
        dataFile.print(ToDeg(roll)); // Roll (Rotation about X)
        dataFile.print("\t");
        
        dataFile.print(ToDeg(pitch)); // Pitch (Rotation about Y)
        dataFile.print("\t");
        dataFile.print(ToDeg(yaw)); // Yaw (Rotation about Z)
        dataFile.print("\t");

        //  Serial.print(",AN:");
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
        dataFile.println();
        
        // print to the serial port too:
        //  Serial.println(dataString);
    } else { // if the file isn't open, pop up an error:
        Serial.println("error opening log.txt");
    } 
}

void printData (void) {
    
    // Time in milliseconds
    Serial.print(dataTime);
    Serial.print("\t");
    
    // Time in seconds
    Serial.print(dataTime / 1000.0);
    Serial.print("\t");
    
    // Gesture
    Serial.print(gestures[gestureIndex]);
    Serial.print("\t");

    Serial.print(ToDeg(roll)); // Roll (Rotation about X)
    Serial.print("\t");
        
        float averageRollDegree = movingAvarageFilter.process(ToDeg(roll));
        dataFile.print(averageRollDegree); // Roll (Rotation about X)
        dataFile.print("\t");
        
    Serial.print(ToDeg(pitch)); // Pitch (Rotation about Y)
    Serial.print("\t");
    Serial.print(ToDeg(yaw)); // Yaw (Rotation about Z)
    Serial.print("\t");
    
    Serial.print(AN[0]);  //(int)read_adc(0) // Gyro X
    Serial.print("\t");
    Serial.print(AN[1]); // Gyro Y
    Serial.print("\t");
    Serial.print(AN[2]); // Gyro Z
    Serial.print("\t");
    
    Serial.print(AN[3]); // Accelerometer X
    Serial.print("\t");
    Serial.print(AN[4]); // Accelerometer Y
    Serial.print("\t");
    Serial.print(AN[5]); // Accelerometer Z
    Serial.print("\t");
    
    Serial.print(c_magnetom_x);
    Serial.print("\t");
    Serial.print(c_magnetom_y);
    Serial.print("\t");
    Serial.print(c_magnetom_z);
    Serial.print ("\t");
    
    Serial.print(pressure);
    Serial.print ("\t");
    Serial.print(altitude);
    Serial.print ("\t");
    Serial.print(temperature);
    
    Serial.println();
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
    } else {
        return false; // Return false when not updated
    }
}
