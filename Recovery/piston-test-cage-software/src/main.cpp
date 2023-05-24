#include <Arduino.h>
#include "defs.h"
#include "HX711.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <string.h>

/* mqtt setup */
WiFiClient esp_client;
PubSubClient client(esp_client);
long last_msg = 0;
char msg[50];
int value = 0;
const char* subscribe_topic = "piston/activate";
const char* publish_topic = "piston/force";
const char* init_topic = "piston/initialized";
void callback(char* topic, byte* message, unsigned int length);
bool ignited = false;


int real_average_reading, raw_average_reading;

bool led_state = 0;
const int led_pin = 14;
int nichrome_high_time = 0;

void connect_to_wifi(){
  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debugln("[+]Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  debugln(WiFi.localIP());
}

/* Load cell functions */
HX711 load_cell;

void buzz(){
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
}

void countdown(){
  digitalWrite(led1, HIGH);
  delay(500);
  digitalWrite(led1, LOW);
  delay(500);

  digitalWrite(led2, HIGH);
  delay(500);
  digitalWrite(led2, LOW);
  delay(500);

  digitalWrite(led3, HIGH);
  delay(500);
  digitalWrite(led3, LOW);
  delay(500);
}

void callback(char* topic, byte* message, unsigned int length){
  String message_temp;

  for (int i = 0; i< length; i++){
    message_temp += (char)message[i];
  }

  // attempt to write pin HIGH
  if(String(topic) == subscribe_topic){
    if(message_temp == "FIRE"){
      debug("FIRE");

      // WRITE NICHROME HIGH
      digitalWrite(nichrome, HIGH);
      // delay nichrome-delay-time seconds
      debugln("nichrome on");
    } else if(message_temp == "ABORT"){
      debugln("ABORT");
      buzz();
    } else {
      // we have received pin high time from PC

      // set nichrome delay time to this value
      nichrome_high_time = message_temp.toInt();

      // buzz for 2 seconds 
      buzz();
    
      // todo: send back an acknowledge signal to master

      debugln(message_temp);

    }
  }
}

void reconnect(){
  while (!client.connected()){
    debugln("Attempting to reconnect");

    if(client.connect("PISTON-MASTER")){
      debugln("connected");

      client.subscribe(subscribe_topic);
    }else{
      debug("failed, rc=");
      debug(client.state());
      debugln("Try again in 3 seconds");

      delay(3000);
    }
  }
  
}


void setup() {
  Serial.begin(115200);
  connect_to_wifi();
  load_cell.begin(load_cell_dt, load_cell_sck);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(nichrome, OUTPUT);
  pinMode(buzzer, OUTPUT);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);

  // systems check
  // buzz();
  // countdown();

}

void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  /* read real value from load cell */
  float reading = (load_cell.read()-153600)/float(10900);
  if(reading>10 && !ignited){
      ignited=true;
      digitalWrite(nichrome, LOW);
      debugln("nichrome off");
  }
  // float reading = 90.01;

  /* convert the value to char array */
  char force[8];
  dtostrf(reading, 1, 2, force);

  // send force value to PC client
  client.publish(publish_topic, force);
}