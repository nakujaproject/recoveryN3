#include <Arduino.h>
#include <TinyGPSPlus.h>

// The TinyGPSPlus object

TinyGPSPlus gps;

void setup() {

  Serial.begin(9600);

  Serial2.begin(9600);

  delay(3000);

}
void displayInfo()

{
  Serial.print(F("Location: "));

  if (gps.location.isValid()){

    Serial.print("Lat: ");

    Serial2.print(gps.location.lat(), 6);

    Serial.print(F(","));

    Serial.print("Lng: ");

    Serial2.print(gps.location.lng(), 6);

    Serial.println();

  }  

  else

  {

    Serial.print(F("INVALID"));

  }

}

void loop() {

  //updateSerial();

  while (Serial2.available() > 0)

    if (gps.encode(Serial2.read()))

      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)

  {

    Serial.println(F("No GPS detected: check wiring."));

    while (true);

  }

}


void updateSerial()

{

  delay(500);

  while (Serial2.available())

  {

    Serial.write(Serial2.read());//Forward what Serial received to Software Serial Port

  }

  while (Serial.available())

  {

    Serial.write(Serial2.read());//Forward what Software Serial received to Serial Port

  }

}


