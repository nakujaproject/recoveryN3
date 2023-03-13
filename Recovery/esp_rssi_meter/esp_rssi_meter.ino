/*
 * This code measures the strength of the  value of the WIFI
 *
 */

#include <WiFi.h>

const char* ssid = "test-station";
const char* pass = "123456789";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("[+]Connecting...");
    delay(500);
  }

  Serial.println("[+]Connected to "); Serial.print(ssid);
  
}

void loop() {

  // print the received signal strength if connected
  long rssi = WiFi.RSSI();
  Serial.print("[+]RSSI:");
  Serial.println(rssi);
  
}