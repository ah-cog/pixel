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
#include <LPS331.h>

L3G gyro; // Gyroscope (sensor of angular momentum)
LSM303 compass; // Compass
LPS331 ps; // Pressure sensor (measures the atmospheric pressure)

void setupInertialMeasurementUnit() {
  Wire.begin();
}

/**
 * Initialize the IMU peripheral (inertial measurement unit).
 */
boolean setupOrientationSensor() {

  Serial.println("Turning on orientation sensor...");
  setupInertialMeasurementUnit();

  // delay(1500);

  setupAccelerometer();
  setupCompass();
  setupGyroscope();
  setupAltimeter();

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

  Serial.print("Offset: ");
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

void setupGyroscope() {
  gyro.init();
  gyro.writeReg(L3G_CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz
  gyro.writeReg(L3G_CTRL_REG4, 0x20); // 2000 dps full scale
}

void getGyroscopeData() {
  gyro.read();

  AN[0] = gyro.g.x;
  AN[1] = gyro.g.y;
  AN[2] = gyro.g.z;
  gyro_x = SENSOR_SIGN[0] * (AN[0] - AN_OFFSET[0]);
  gyro_y = SENSOR_SIGN[1] * (AN[1] - AN_OFFSET[1]);
  gyro_z = SENSOR_SIGN[2] * (AN[2] - AN_OFFSET[2]);
}

void setupAccelerometer() {
  compass.init();
  if (compass.getDeviceType() == LSM303DLHC_DEVICE)
  {
    compass.writeAccReg(LSM303_CTRL_REG1_A, 0x47); // normal power mode, all axes enabled, 50 Hz
    compass.writeAccReg(LSM303_CTRL_REG4_A, 0x28); // 8 g full scale: FS = 10 on DLHC; high resolution output mode
  }
  else 
  {
    compass.writeAccReg(LSM303_CTRL_REG1_A, 0x27); // normal power mode, all axes enabled, 50 Hz
    compass.writeAccReg(LSM303_CTRL_REG4_A, 0x30); // 8 g full scale: FS = 11 on DLH, DLM
  }
}

// Reads x,y and z accelerometer registers
void getAccelerometerData() {
  compass.readAcc();

  AN[3] = compass.a.x;
  AN[4] = compass.a.y;
  AN[5] = compass.a.z;
  accel_x = SENSOR_SIGN[3] * (AN[3] - AN_OFFSET[3]);
  accel_y = SENSOR_SIGN[4] * (AN[4] - AN_OFFSET[4]);
  accel_z = SENSOR_SIGN[5] * (AN[5] - AN_OFFSET[5]);
}

// Initialize compass sensor
void setupCompass() {
  compass.writeMagReg(LSM303_MR_REG_M, 0x00); // continuous conversion mode
  // 15 Hz default
}

// Read compass sensor data
void getCompassData() {
  compass.readMag();

  magnetom_x = SENSOR_SIGN[6] * compass.m.x;
  magnetom_y = SENSOR_SIGN[7] * compass.m.y;
  magnetom_z = SENSOR_SIGN[8] * compass.m.z;
}

// Initialize altimeter sensor
void setupAltimeter() {
  if (!ps.init()) {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }
  ps.enableDefault();
}

// Read altimeter sensor data
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


