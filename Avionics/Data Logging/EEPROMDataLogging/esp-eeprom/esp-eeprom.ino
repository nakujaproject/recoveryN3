//This project seeks to collect data from two sensors: BMP180 and MPU6050 through an ESP32 
//   and store that data to an EEPROM memory.
// In this case we use the AT24C256 EEPROM module which has a 32KB EEPROM chip 
//  and supports communication over I2C
// The EEPROM library was downloaded from: https://github.com/Polohpi/AT24C256 

#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <AT24C256.h>

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;
AT24C256 EEPROM = AT24C256();

void setup() {
  //setup BMP180, MPU6050 , EEPROM
  Serial.begin(115200);

  //Check for status of sensors
  if(!bmp.begin()){
    Serial.println("BMP180 not found");
    while (1) {
      delay(10);
    }
  }
  if (!mpu.begin()){
    Serial.println("MPU6050 not found");
    while (1){
      delay(10);
    }
  }

  Serial.println("Adafruit MPU6050 found!");
  Serial.println("Adafruit BMP180 found!");

  //setup the MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  Serial.println(mpu.getAccelerometerRange());
 
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  Serial.println(mpu.getGyroRange());
 
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.println("Filter bandwidth set to: ");
  Serial.println(mpu.getFilterBandwidth());
  

  Serial.println("");
  delay(100);
}
void loop() {
  //read mpu6050 values
  sensors_event_t acc, rot, temp;
  mpu.getEvent(&acc, &rot , &temp);

  float x_acc = acc.acceleration.x;
  float y_acc = acc.acceleration.y;
  float z_acc = acc.acceleration.z;

  float x_gyro = rot.gyro.x;
  float y_gyro = rot.gyro.y;
  float z_gyro = rot.gyro.z;

  float temps = temp.temperature;

  //read BMP values
  float alt = bmp.readAltitude();
  float pressure = bmp.readPressure();

  //store sensor values in array
  float sensors_v[]={alt, pressure, x_acc, y_acc, z_acc, x_gyro, y_gyro, z_gyro, temps};

  //write to EEPROM
  int address = 0 ; 
  while (address < 9){
    EEPROM.write((char) sensors_v[address], address);
    address++;
  }
  Serial.println("Done writing to EEPROM!");

  // read from EEPROM
  Serial.println("Reading EEPROM...");
  int j = 0;
  while (j < 9){
    Serial.print("Data written at EEPROM: ");
    Serial.println(EEPROM.read(j));
    j++;
  } 

  delay(500);
  while(1);
}
