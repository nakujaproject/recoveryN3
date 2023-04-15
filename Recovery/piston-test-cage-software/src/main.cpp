#include <Arduino.h>
#include <esp_now.h>
#include "soc/rtc.h"
#include "defs.h"
#include "HX711.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/* load cell pins  */
const int load_cell_dt = 25;
const int load_cell_sck = 26;

int real_average_reading, raw_average_reading;

bool led_state = 0;
const int led_pin = 2;
String dummy_force = "700";

/* create async web server object on port 80*/
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notify_all_clients(){
  ws.textAll(String(led_state));
}

void handle_web_socket_message(void *arg, uint8_t *data, size_t len){
  AwsFrameInfo* info = (AwsFrameInfo*) arg;

  if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT){
    data[len] = 0;

    if(strcmp((char*) data, "toggle") == 0){
      led_state != led_state;
      notify_all_clients;
    }
  }
}

void on_event(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len){
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handle_web_socket_message(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }

}

void init_websocket(){
  ws.onEvent(on_event);
  server.addHandler(&ws);
}

String processor(const String& var){
  debugln(var);

  if(var == "STATE"){
    if(led_state){
      return "ON";
    } else{
      return "OFF";
    }
  }

  return String();
}

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

void setup() {
  Serial.begin(115200);

  /* setup load cell */
  setup_load_cell();
  // callibrate_load_cell();

  /* set callibration factor */
  set_callibration_factor();

  /* set up connection variables */
  connect_to_wifi();

  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  init_websocket();

  /* route for root / page */
  server.on("/force", HTTP_GET, [](AsyncWebServerRequest* request){
    request->send_P(200, "text/plain", dummy_force.c_str(), processor);
  });

}

void loop() {

  raw_average_reading = get_raw_readings();
  debug("Raw:"); debugln(raw_average_reading); debugln();

  /* read  load cell */

  /* transmit to PC */
  ws.cleanupClients();
  digitalWrite(led_pin, led_state);

  
}