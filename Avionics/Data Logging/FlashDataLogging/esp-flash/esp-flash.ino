//This project seeks to collect data from two sensors: BMP180 and MPU6050 through an ESP32 
//   and store that data to a flash memory.
// In this case we use the Windbond W25Q128FV flash memory which is a 128MB flash memory chip 
//  that supports SPI and QPI



//importing libraries for use in the project
#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SPIMemory.h>

Adafruit_MPU6050 mpu;
Adafruit_BMP085 bmp;

SPIFlash flash(SS, &SPI);

void setup() {
  //setup BMP180, MPU6050 to write data to esp32
  //setup sdcard and eeprom to read data from esp32
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

  //setup flash memory
  flash.begin();
  flash.eraseChip();

  Serial.print(F("Flash size is: "));
  Serial.print((long)(flash.getCapacity()/1000));
  Serial.println(F("kb"));

  delay(100);  
}

void loop() {
  //read MPU values
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

  //write data to flash memory
  unsigned long address = 0;
  while (address < 9){
    flash.writeFloat(address,sensors_v[address]);
    Serial.print("Data recorded : ");
    Serial.print(sensors_v[address]);
    Serial.print(", At address: ");
    Serial.print(address);
    address++;  
    }
  Serial.println("Done writing to Flash!");

  //read data from flash memory
  Serial.println("Reading Flash...");
  int j = 0;
  while (j < 9){
    Serial.print("Data written at flash: ");
    Serial.println(flash.readFloat(j));
    j++;
  }
  
  delay(500);

  while(1); 

}
