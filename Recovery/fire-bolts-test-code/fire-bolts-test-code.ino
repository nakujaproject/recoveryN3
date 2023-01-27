/**
 * 
 * This program is for testing fire-bolts to be used for firing piston used for 
 * ejection on Nakuja N3 rocket
 * 
 * Board used: ESP32 Devkit
 * 
 * Author: Edwin Mwiti Maingi 
 * Email: emwiti658@gmail.com
 */

 #define FIRING_PIN 18
 #define BUZZER_PIN 4
 #define DELAY 3000 // 3 seconds
 #define BUZZER_DELAY 500

void setup() {
  pinMode(FIRING_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(FIRING_PIN, LOW);
  Serial.begin(115200);

  // pull the MOSFET gate pin high after 3 seconds
  delay(DELAY);

  // fire the pyro-bolt
  digitalWrite(FIRING_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  
  Serial.println("Fired!"); // some feedback via serial
  
  delay(BUZZER_DELAY); // this buzzer delay is to be determined from tests. The ideal time it takes to
                       // iginite the black powder

  // pull everything down
  digitalWrite(FIRING_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

}

void loop() {
  

}
