#ifndef DEFS_H
#define DEFS_H

/* Debug code*/
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

#define BAUD_RATE 115200

// pins used
const int green_led = 5;
const int red_led = 19;
const int orange_led = 21;
const int buzzer_pin = 4;
const int firing_pin = 18;

// timing variables
#define COUNTDOWN_TIMER 800
#define FIRING_PIN_HOLD_TIME 5000
#define BUZZ_TIME 500

#endif