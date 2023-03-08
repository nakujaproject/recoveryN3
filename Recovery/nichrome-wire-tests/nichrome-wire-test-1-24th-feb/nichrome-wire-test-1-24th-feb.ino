#define PIN_HIGH_TIME 6000
#define HEATING_PIN 7

void setup() {
  pinMode(HEATING_PIN, OUTPUT);
  
  digitalWrite(HEATING_PIN, HIGH);
  delay(PIN_HIGH_TIME);
  digitalWrite(HEATING_PIN, LOW);

}

void loop() {}
