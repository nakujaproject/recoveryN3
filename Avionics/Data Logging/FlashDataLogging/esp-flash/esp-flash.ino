//importing libraries for use in the project
#include <Arduino.h>

#include <SPIMemory.h>

// SPIFlash flash(SS, &SPI);
SPIFlash flash;

void setup() {
  Serial.begin(115200);

  while(!Serial); // wait for serial monitor to open
  delay(100); 

  if(flash.begin()){
    Serial.println("Flash found");
  } else {
    Serial.println("No flash found");
  }

  Serial.print(F("Flash size is: "));
  Serial.print((long)(flash.getCapacity()/1000));
  Serial.println(F("kb"));
 
}

void loop() {


}
