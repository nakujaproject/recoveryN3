/* debug parameters for use during testing - disable before launch */
#define DEBUG 1

#if DEBUG == 1

#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x, y) Serial.printf(x, y)

#else

#define debug(x)
#define debugln(x)
#define debugf(x, y)

#endif

#define CALLIBRATION_FACTOR 100

const char* SSID = "Galaxy";
const char* PASSWORD = "luwa2131";

const char* MQTT_BROKER = "192.168.208.19";
uint16_t MQTT_PORT = 1883;

// pinouts
const int buzzer = 4;
const int nichrome = 32;
const int led1 = 23;
const int led2 = 22;
const int led3 = 21;
const int load_cell_dt = 27;
const int load_cell_sck = 26;