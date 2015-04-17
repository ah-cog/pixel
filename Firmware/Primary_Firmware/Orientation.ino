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

#include <L3G.h>
#include <LSM303.h>
#include <LPS.h>

L3G gyro; // Gyroscope (sensor of angular momentum)
LSM303 compass; // Compass
LPS ps; // Pressure sensor (measures the atmospheric pressure)

void Setup_Inertial_Measurement_Unit() {
  Wire.begin(); // Join I2C bus (address optional for master)
}

/**
 * Initialize the IMU peripheral (inertial measurement unit).
 */
boolean Setup_Orientation_Sensing() {

  Serial.println ("Orientation sensors are active");
  Setup_Inertial_Measurement_Unit ();

  // delay(1500);

  Setup_Accelerometer();
  Setup_Compass();
  Setup_Gyroscope();
  Setup_Altimeter();

  delay(20); // Wait for a small duration for the IMU sensors to initialize (?)

  for (int i = 0; i < 32; i++) { // We take some initial readings... (to warm the IMU up?)
    getGyroscopeData();
    getAccelerometerData();
    for (int y = 0; y < 6; y++) { // Cumulate values
        AN_OFFSET[y] += AN[y];
    }
    delay(20);
  }

  for (int y = 0; y < 6; y++) {
    AN_OFFSET[y] = AN_OFFSET[y] / 32;
  }

  AN_OFFSET[5] -= GRAVITY * SENSOR_SIGN[5];

  Serial.print("The offset is ");
  for (int y = 0; y < 6; y++) {
    Serial.print(AN_OFFSET[y]);
    Serial.print(" ");
  }
  Serial.print("\n");

  // delay(1000); // TODO: Why is this here? Try to get rid of it! Wake up quickly!

  timer = millis();
  delay(20);
  counter = 0;
}

void Setup_Gyroscope () {
  gyro.init();
  gyro.writeReg(L3G_CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz
  gyro.writeReg(L3G_CTRL_REG4, 0x20); // 2000 dps full scale
}

void Setup_Accelerometer () {
  compass.init();
  compass.enableDefault();
  switch (compass.getDeviceType())
  {
    case LSM303::device_D:
      compass.writeReg(LSM303::CTRL2, 0x18); // 8 g full scale: AFS = 011
      break;
    case LSM303::device_DLHC:
      compass.writeReg(LSM303::CTRL_REG4_A, 0x28); // 8 g full scale: FS = 10; high resolution output mode
      break;
    default: // DLM, DLH
      compass.writeReg(LSM303::CTRL_REG4_A, 0x30); // 8 g full scale: FS = 11
  }
//  compass.init();
//  if (compass.getDeviceType() == LSM303DLHC_DEVICE)
//  {
//    compass.writeAccReg(LSM303_CTRL_REG1_A, 0x47); // normal power mode, all axes enabled, 50 Hz
//    compass.writeAccReg(LSM303_CTRL_REG4_A, 0x28); // 8 g full scale: FS = 10 on DLHC; high resolution output mode
//  }
//  else 
//  {
//    compass.writeAccReg(LSM303_CTRL_REG1_A, 0x27); // normal power mode, all axes enabled, 50 Hz
//    compass.writeAccReg(LSM303_CTRL_REG4_A, 0x30); // 8 g full scale: FS = 11 on DLH, DLM
//  }
}

// Initialize compass sensor
void Setup_Compass () {
//  compass.writeMagReg(LSM303_MR_REG_M, 0x00); // continuous conversion mode
  // 15 Hz default
}

// Initialize altimeter sensor
void Setup_Altimeter () {
  if (!ps.init()) {
    Serial.println("Error: Failed to autodetect pressure sensor!");
    while (1);
  }
  ps.enableDefault();
}

/**
 * Read the IMU sensor data and estimate the module's orientation. Orientation is 
 * estimated using the DCM (Direction Cosine Matrix).
 */
boolean Sense_Orientation () {
  
  if ((millis() - timer) >= 20) { // Main loop runs at 50Hz
    counter++;
    timer_old = timer;
    timer = millis();
    if (timer > timer_old) {
      G_Dt = (timer-timer_old) / 1000.0; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
    } else {
      G_Dt = 0;
    }

    // DCM algorithm:

    // Data adquisition
    getGyroscopeData(); // This read gyro data
    getAccelerometerData(); // Read I2C accelerometer

    if (counter > 5) { // Read compass data at 10 Hz... (5 loop runs)
      counter = 0;
      getCompassData(); // Read I2C magnetometer
      calculateCompassHeading(); // Calculate magnetic heading
    }

    // Read pressure/altimeter
    getAltimeterData();

    // Calculations...
    DCM_Matrix_update(); 
    Normalize();
    Drift_correction();
    Euler_Angles();
    
    return true;
  } else {
    return false;
  }
}

/**
 * Read gyroscope sensor data
 */
void getGyroscopeData() {
  gyro.read();

  AN[0] = gyro.g.x;
  AN[1] = gyro.g.y;
  AN[2] = gyro.g.z;
  gyro_x = SENSOR_SIGN[0] * (AN[0] - AN_OFFSET[0]);
  gyro_y = SENSOR_SIGN[1] * (AN[1] - AN_OFFSET[1]);
  gyro_z = SENSOR_SIGN[2] * (AN[2] - AN_OFFSET[2]);
}

/**
 * Reads x, y and z accelerometer registers.
 */
void getAccelerometerData() {
  compass.readAcc();

  AN[3] = compass.a.x;
  AN[4] = compass.a.y;
  AN[5] = compass.a.z;
  accel_x = SENSOR_SIGN[3] * (AN[3] - AN_OFFSET[3]);
  accel_y = SENSOR_SIGN[4] * (AN[4] - AN_OFFSET[4]);
  accel_z = SENSOR_SIGN[5] * (AN[5] - AN_OFFSET[5]);
}

/**
 * Read compass sensor data
 */
void getCompassData() {
  compass.readMag();

  magnetom_x = SENSOR_SIGN[6] * compass.m.x;
  magnetom_y = SENSOR_SIGN[7] * compass.m.y;
  magnetom_z = SENSOR_SIGN[8] * compass.m.z;
}

/**
 * Read altimeter sensor data
 */
void getAltimeterData() {

  pressure = ps.readPressureInchesHg();
  altitude = ps.pressureToAltitudeFeet(pressure);
  temperature = ps.readTemperatureF();

  //  Serial.print("p: ");
  //  Serial.print(pressure);
  //  Serial.print(" inHg\ta: ");
  //  Serial.print(altitude);
  //  Serial.print(" ft\tt: ");
  //  Serial.print(temperature);
  //  Serial.println(" deg F");
}

/**************************************************/
// from "Compass"

/**
 * Calculate compass heading from sensor data.
 */
void calculateCompassHeading() {
  
  float MAG_X;
  float MAG_Y;
  float cos_roll;
  float sin_roll;
  float cos_pitch;
  float sin_pitch;
  
  cos_roll  = cos(roll);
  sin_roll  = sin(roll);
  cos_pitch = cos(pitch);
  sin_pitch = sin(pitch);
  
  // Adjust for LSM303 compass axis offsets/sensitivity differences by scaling to +/-0.5 range
  c_magnetom_x = (float) (magnetom_x - SENSOR_SIGN[6]*M_X_MIN) / (M_X_MAX - M_X_MIN) - (SENSOR_SIGN[6] * 0.5);
  c_magnetom_y = (float) (magnetom_y - SENSOR_SIGN[7]*M_Y_MIN) / (M_Y_MAX - M_Y_MIN) - (SENSOR_SIGN[7] * 0.5);
  c_magnetom_z = (float) (magnetom_z - SENSOR_SIGN[8]*M_Z_MIN) / (M_Z_MAX - M_Z_MIN) - (SENSOR_SIGN[8] * 0.5);
  
  // Calculate heading
  MAG_X = (c_magnetom_x * cos_pitch) + (c_magnetom_y * sin_roll * sin_pitch) + (c_magnetom_z * cos_roll * sin_pitch); // Tilt compensated Magnetic filed X
  MAG_Y = (c_magnetom_y * cos_roll) - (c_magnetom_z * sin_roll); // Tilt compensated Magnetic filed Y
  MAG_Heading = atan2(-MAG_Y, MAG_X); // Magnetic Heading
}

/**************************************************/
// from "Vector"

/**
 * Computes the dot product of two vectors
 */
float Vector_Dot_Product(float vector1[3], float vector2[3]) {
  float op = 0;
  
  for(int c=0; c<3; c++) {
    op += vector1[c] * vector2[c];
  }
  
  return op; 
}

/**
 * Computes the cross product of two vectors
 */
void Vector_Cross_Product(float vectorOut[3], float v1[3], float v2[3]) {
  vectorOut[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  vectorOut[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  vectorOut[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

/**
 * Multiply the vector by a scalar. 
 */
void Vector_Scale(float vectorOut[3], float vectorIn[3], float scale2) {
  for (int c = 0; c < 3; c++) {
    vectorOut[c] = vectorIn[c] * scale2;
  }
}

/**
 * Add the vectors. 
 */
void Vector_Add(float vectorOut[3], float vectorIn1[3], float vectorIn2[3]) {
  for (int c = 0; c < 3; c++) {
    vectorOut[c] = vectorIn1[c] + vectorIn2[c];
  }
}

/**************************************************/
// from "DCM"

void Normalize (void) {
  float error = 0;
  float temporary[3][3];
  float renorm = 0;
  
  error = -Vector_Dot_Product(&DCM_Matrix[0][0], &DCM_Matrix[1][0]) * 0.5; //eq.19

  Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
  Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19
  
  Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]); //eq.19
  Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]); //eq.19
  
  Vector_Cross_Product(&temporary[2][0], &temporary[0][0], &temporary[1][0]); // c= a x b // eq.20
  
  renorm = 0.5 *(3 - Vector_Dot_Product(&temporary[0][0], &temporary[0][0])); // eq.21
  Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);
  
  renorm = 0.5 * (3 - Vector_Dot_Product(&temporary[1][0], &temporary[1][0])); // eq.21
  Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);
  
  renorm = 0.5 * (3 - Vector_Dot_Product(&temporary[2][0], &temporary[2][0])); // eq.21
  Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
}

/**************************************************/
void Drift_correction(void) {
  
  float mag_heading_x;
  float mag_heading_y;
  float errorCourse;
  //Compensation the Roll, Pitch and Yaw drift. 
  static float Scaled_Omega_P[3];
  static float Scaled_Omega_I[3];
  float Accel_magnitude;
  float Accel_weight;
  
  
  //*****Roll and Pitch***************

  // Calculate the magnitude of the accelerometer vector
  Accel_magnitude = sqrt(Accel_Vector[0] * Accel_Vector[0] + Accel_Vector[1] * Accel_Vector[1] + Accel_Vector[2] * Accel_Vector[2]);
  Accel_magnitude = Accel_magnitude / GRAVITY; // Scale to gravity.
  // Dynamic weighting of accelerometer info (reliability filter)
  // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
  Accel_weight = constrain(1 - 2 * abs(1 - Accel_magnitude), 0, 1);  //  

  Vector_Cross_Product(&errorRollPitch[0], &Accel_Vector[0], &DCM_Matrix[2][0]); //adjust the ground of reference
  Vector_Scale(&Omega_P[0], &errorRollPitch[0], Kp_ROLLPITCH * Accel_weight);
  
  Vector_Scale(&Scaled_Omega_I[0], &errorRollPitch[0], Ki_ROLLPITCH * Accel_weight);
  Vector_Add(Omega_I, Omega_I, Scaled_Omega_I);     
  
  //*****YAW***************
  // We make the gyro YAW drift correction based on compass magnetic heading
 
  mag_heading_x = cos(MAG_Heading);
  mag_heading_y = sin(MAG_Heading);
  errorCourse = (DCM_Matrix[0][0] * mag_heading_y) - (DCM_Matrix[1][0] * mag_heading_x);  //Calculating YAW error
  Vector_Scale(errorYaw, &DCM_Matrix[2][0], errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.
  
  Vector_Scale(&Scaled_Omega_P[0], &errorYaw[0], Kp_YAW);//.01proportional of YAW.
  Vector_Add(Omega_P, Omega_P, Scaled_Omega_P);//Adding  Proportional.
  
  Vector_Scale(&Scaled_Omega_I[0], &errorYaw[0], Ki_YAW);//.00001Integrator
  Vector_Add(Omega_I, Omega_I, Scaled_Omega_I);//adding integrator to the Omega_I
}

/**************************************************/
/*
void Accel_adjust(void) {
 Accel_Vector[1] += Accel_Scale(speed_3d*Omega[2]);  // Centrifugal force on Acc_y = GPS_speed * GyroZ
 Accel_Vector[2] -= Accel_Scale(speed_3d*Omega[1]);  // Centrifugal force on Acc_z = GPS_speed * GyroY 
}
*/
/**************************************************/

void DCM_Matrix_update(void) {
  Gyro_Vector[0] = Gyro_Scaled_X(gyro_x); // gyro x roll
  Gyro_Vector[1] = Gyro_Scaled_Y(gyro_y); // gyro y pitch
  Gyro_Vector[2] = Gyro_Scaled_Z(gyro_z); // gyro Z yaw
  
  Accel_Vector[0] = accel_x;
  Accel_Vector[1] = accel_y;
  Accel_Vector[2] = accel_z;
    
  Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
  Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term

  //Accel_adjust();    //Remove centrifugal acceleration.   We are not using this function in this version - we have no speed measurement
  
#if IMU_OUTPUT_MODE==1         
  Update_Matrix[0][0] = 0;
  Update_Matrix[0][1] = -G_Dt * Omega_Vector[2]; // -z
  Update_Matrix[0][2] = G_Dt * Omega_Vector[1]; // y
  Update_Matrix[1][0] = G_Dt * Omega_Vector[2]; // z
  Update_Matrix[1][1] = 0;
  Update_Matrix[1][2] = -G_Dt * Omega_Vector[0]; // -x
  Update_Matrix[2][0] = -G_Dt * Omega_Vector[1]; // -y
  Update_Matrix[2][1] = G_Dt * Omega_Vector[0]; // x
  Update_Matrix[2][2] = 0;
#else                    // Uncorrected data (no drift correction)
  Update_Matrix[0][0] = 0;
  Update_Matrix[0][1] = -G_Dt * Gyro_Vector[2]; // -z
  Update_Matrix[0][2] = G_Dt * Gyro_Vector[1]; // y
  Update_Matrix[1][0] = G_Dt * Gyro_Vector[2]; // z
  Update_Matrix[1][1] = 0;
  Update_Matrix[1][2] = -G_Dt * Gyro_Vector[0];
  Update_Matrix[2][0] = -G_Dt * Gyro_Vector[1];
  Update_Matrix[2][1] = G_Dt * Gyro_Vector[0];
  Update_Matrix[2][2] = 0;
#endif
 
  Matrix_Multiply(DCM_Matrix, Update_Matrix, Temporary_Matrix); // a * b = c

  for (int x = 0; x < 3; x++) { // Matrix Addition (update)
    for (int y = 0; y < 3; y++) {
      DCM_Matrix[x][y] += Temporary_Matrix[x][y];
    } 
  }
}

/**
 * Computes the "Euler angles" (i.e., roll, pitch, and yaw).
 */
void Euler_Angles (void) {
  pitch = -asin(DCM_Matrix[2][0]);
  roll  = atan2(DCM_Matrix[2][1], DCM_Matrix[2][2]);
  yaw   = atan2(DCM_Matrix[1][0], DCM_Matrix[0][0]);
}

/**************************************************/
// from "Matrix"

// Multiply two 3x3 matrices. This function developed by Jordi can be easily adapted to multiple n*n matrix's. (Pero me da flojera!). 
void Matrix_Multiply(float a[3][3], float b[3][3], float mat[3][3]) {
  float op[3]; 
  for(int x = 0; x < 3; x++) {
    for(int y = 0; y < 3; y++) {
      for(int w = 0; w < 3; w++) {
        op[w] = a[x][w] * b[w][y];
      } 
      mat[x][y] = 0;
      mat[x][y] = op[0]+op[1] + op[2];
      
      float test = mat[x][y];
    }
  }
}

/**************************************************/
// from "Output"

//void printData (void) {
//    
//    // Time in milliseconds
//    dataFile.print(dataTime);
//    dataFile.print("\t");
//    
//    // Time in seconds
//    dataFile.print(dataTime / 1000.0);
//    dataFile.print("\t");
//    
//    // Gesture
//    dataFile.print(gestures[gestureIndex]);
//    dataFile.print("\t");
//
//#if PRINT_EULER == 1
////  Serial.print("ANG:");
//  Serial.print(ToDeg(roll)); // Roll (Rotation about X)
//  Serial.print("\t");
//  Serial.print(ToDeg(pitch)); // Pitch (Rotation about Y)
//  Serial.print("\t");
//  Serial.print(ToDeg(yaw)); // Yaw (Rotation about Z)
//#endif      
//#if PRINT_ANALOGS ==1
////  Serial.print(",AN:");
//  Serial.print("\t");
//  Serial.print(AN[0]);  //(int)read_adc(0) // Gyro X
//  Serial.print("\t");
//  Serial.print(AN[1]); // Gyro Y
//  Serial.print("\t");
//  Serial.print(AN[2]); // Gyro Z
//  Serial.print("\t");
//  Serial.print(AN[3]); // Accelerometer X
//  Serial.print("\t");
//  Serial.print(AN[4]); // Accelerometer Y
//  Serial.print("\t");
//  Serial.print(AN[5]); // Accelerometer Z
//  Serial.print("\t");
//  Serial.print(c_magnetom_x);
//  Serial.print("\t");
//  Serial.print(c_magnetom_y);
//  Serial.print("\t");
//  Serial.print(c_magnetom_z);
//  Serial.print ("\t");
//  Serial.print(pressure);
//  Serial.print ("\t");
//  Serial.print(altitude);
//  Serial.print ("\t");
//  Serial.print(temperature);
//#endif

/*
#if PRINT_DCM == 1
  Serial.print (",DCM:");
  Serial.print(convert_to_dec(DCM_Matrix[0][0]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[0][1]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[0][2]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[1][0]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[1][1]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[1][2]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[2][0]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[2][1]));
  Serial.print (",");
  Serial.print(convert_to_dec(DCM_Matrix[2][2]));
#endif
*/
//  Serial.println();
//}
//
//long convert_to_dec(float x) {
//  return x * 10000000;
//}

