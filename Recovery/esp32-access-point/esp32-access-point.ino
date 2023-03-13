// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "test-station";
const char* password = "123456789";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("[+]Setting AP");

  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.println();
  Serial.print("[+]IP address: ");
  Serial.println(IP);
  
}

void loop(){

}