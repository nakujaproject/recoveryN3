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


int real_average_reading, raw_average_reading;

bool led_state = 0;
const int led_pin = 14;
int nichrome_high_time = 0;
String dummy_force = "700";

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

void setup_load_cell(){
  load_cell.begin(load_cell_dt, load_cell_sck);
}

void callibrate_load_cell(){
  /**
   * Callibrate the cell before use
   * Use a known weight
   * Place the weight on the load cell plates
   * Get the reading
   * Callibration factor = (reading) / (known weight)
   * 
   */
  if(load_cell.is_ready()){
    load_cell.set_scale();
    debugln("[+]Please remove any weight on the load cell plates");    
    delay(4000);

    load_cell.tare();
    debugln("[]Tare done");

    debugln("[+]Place a known weight on the scale..");
    debugln("[..]Measuring...");
    delay(4000);

    long reading = load_cell.get_units(10);
    
    debugln("Done. Reading = "); debug(reading); debugln();
  } else{
    debugln("[-]Could not find HX711");
  }

  delay(1000);
}
/* End of load cell functions */

int get_raw_readings(){
  /* get raw readings */
  /* get average of 20 readings from the ADC */
  int r = load_cell.read_average(20);
  return r;
}

void set_callibration_factor(){
  load_cell.set_scale(CALLIBRATION_FACTOR);
}

int get_real_reading(){
  /* get real readings from the load cell */
  
  load_cell.tare(); // reset the scale to 0
  real_average_reading = load_cell.read_average(20);

  return real_average_reading;
}

void transmit_readings(){
  /* transmit readings over wifi*/
}

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

      // buzz 2 seconds
      buzz();

      // countdown 3 seconds
      // light up LED each second countdown
      countdown();

      // WRITE NICHROME HIGH
      // digitalWrite(nichrome, HIGH);
      digitalWrite(led2, HIGH); // test

      // delay nichrome-delay-time seconds
      delay(nichrome_high_time*1000); // this is where the explosion happens

      // turn off nichrome wire
      // digitalWrite(nichrome, LOW);
      digitalWrite(led2, LOW); // test

      // buzz 2 seconds
      buzz();

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

  /* setup load cell */
  // setup_load_cell();
  // callibrate_load_cell();

  /* set callibration factor */
  // set_callibration_factor();

  /* set up connection variables */
  connect_to_wifi();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(load_cell_dt, INPUT);
  pinMode(load_cell_sck, INPUT);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);

  // systems check
  buzz();
  countdown();

}

void loop(){

  if(!client.connected()){
    reconnect();
  }
  client.loop();

  // raw_average_reading = get_raw_readings();
  // debug("Raw:"); debugln(raw_average_reading); debugln();

  /* read real value from load cell */
  int force = 89;

  /* convert the value to char array */
  char temp_value[8];
  dtostrf(force, 1, 2, temp_value);

  // send force value to PC client
  client.publish(publish_topic, temp_value);

}