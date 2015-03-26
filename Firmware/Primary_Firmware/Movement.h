#ifndef MOVEMENT_H
#define MOVEMENT_H

/**
 * Accelerometer Setup
 */

// Uncomment the below line to use this axis definition: 
// X axis pointing forward, Y axis pointing to the left, and Z axis pointing up.
// Positive pitch: nose down
// Positive roll: right wing down
// Positive yaw: counterclockwise
int SENSOR_SIGN[9] = {1, -1, -1, -1, 1, 1, 1, -1, -1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer
//int SENSOR_SIGN[9] = { 1, -1, -1, 1, -1, 1, 1, -1, -1 }; // Correct directions x,y,z - gyro, accelerometer, magnetometer

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

char serialDataString[512];

void printData (void) {
    
    // NOTE: There's sprintf documentation at http://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm along with some examples showing its usage.
    sprintf(serialDataString, "! %f\t%f\t%f\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\n", roll, pitch, yaw, AN[0], AN[1], AN[2], AN[3], AN[4], AN[5], c_magnetom_x, c_magnetom_y, c_magnetom_z, pressure, altitude, temperature);
    Serial.print(serialDataString);
}

void Store_Data () {
  
  // Shift points left by one
  if (gestureCandidateSize >= (GESTURE_CANDIDATE_SIZE - 1)) {
    for (int axis = 0; axis < AXIS_COUNT; axis++) {
      for (int point = 0; point < GESTURE_CANDIDATE_SIZE - 1; point++) {
        gestureCandidate[axis][point] = gestureCandidate[axis][point + 1]; // Shift point left by one
      }
    }
  }
  
  // Push latest accelerometer data point onto the end of the array
  gestureCandidate[0][gestureCandidateSize] = AN[0]; // gyro x
  gestureCandidate[1][gestureCandidateSize] = AN[1]; // gyro y
  gestureCandidate[2][gestureCandidateSize] = AN[2]; // gyro z
  
  // Increment gesture candidate size (if less than the maximum possible)
  if (gestureCandidateSize < (GESTURE_CANDIDATE_SIZE - 1)) {
    // Increment gesture candidate size
    gestureCandidateSize = gestureCandidateSize + 1;
  }
}

void Store_Data_Accelerometer() {
  
  // Shift points left by one
  if (gestureCandidateSize >= (GESTURE_CANDIDATE_SIZE - 1)) {
    for (int axis = 0; axis < AXIS_COUNT; axis++) {
      for (int point = 0; point < GESTURE_CANDIDATE_SIZE - 1; point++) {
        gestureCandidate[axis][point] = gestureCandidate[axis][point + 1]; // Shift point left by one
      }
    }
  }
  
  // Push latest accelerometer data point onto the end of the array
  gestureCandidate[0][gestureCandidateSize] = AN[3]; // accelerometer x
  gestureCandidate[1][gestureCandidateSize] = AN[4]; // accelerometer y
  gestureCandidate[2][gestureCandidateSize] = AN[5]; // accelerometer z
  
  // Increment gesture candidate size (if less than the maximum possible)
  if (gestureCandidateSize < (GESTURE_CANDIDATE_SIZE - 1)) {
    // Increment gesture candidate size
    gestureCandidateSize = gestureCandidateSize + 1;
  }
}

#endif
