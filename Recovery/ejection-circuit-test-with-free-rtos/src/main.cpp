#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include "defs.h"

QueueHandle_t countdown_timer_queue;
TaskHandle_t send_commands_task_handle;

typedef enum{
  ALL_OFF = 0,
  GREEN_ON = 1,
  GREEN_OFF = 2,
  RED_ON = 3,
  RED_OFF = 4,
  ORANGE_ON = 5,
  ORANGE_OFF = 6,
  FIRE = 7
} indicators;

void send_commands(void* parameter){
  for(;;){
    for (int i = 0; i < 8; i++){
      uint8_t command = (indicators) i;

      // write to queue
      xQueueSend(countdown_timer_queue, &command, portMAX_DELAY);
      vTaskDelay(COUNTDOWN_TIMER / portTICK_PERIOD_MS);
    }
  }
}

void receive_commands(void* parameter){
  uint8_t received_command = 0; // this will hold the command that has been read fromthe queue

  for (;;){
    if(xQueueReceive(countdown_timer_queue, &received_command, portMAX_DELAY) == pdTRUE){
      // process the received commands
      // green on, green off, red on, red off, orange on, orange off, fire
      switch (received_command){
        case ALL_OFF:
          digitalWrite(green_led, LOW);
          digitalWrite(red_led, LOW);
          digitalWrite(orange_led, LOW);
          break;

        case GREEN_ON:
          digitalWrite(green_led, HIGH);
          break;

        case GREEN_OFF:
          digitalWrite(green_led, LOW);
          break;

        case RED_ON:
          digitalWrite(red_led, HIGH);
          break;

        case RED_OFF:
          digitalWrite(red_led, LOW);
          break;

        case ORANGE_ON:
          digitalWrite(orange_led, HIGH);
          break;

        case ORANGE_OFF:
          digitalWrite(orange_led, LOW);
          break;
        
        case FIRE:
          // do the actual firing by writing the MOSFET firing pin HIGH
          // remain high for some time and  go LOW
          digitalWrite(firing_pin, HIGH);
          delay(FIRING_PIN_HOLD_TIME);
          digitalWrite(firing_pin, LOW);

          // buzzer notification
          digitalWrite(buzzer_pin, HIGH);
          delay(BUZZ_TIME);
          digitalWrite(buzzer_pin, LOW);

          debugln("[+] Fired!");

          break;
        
        default:
          break;
      }
    }
  }
}

void setup() {
  // set up pins 
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(orange_led, OUTPUT);
  pinMode(firing_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  Serial.begin(115200);

  debugln("[+]Creating queue...");
  // create the queue
  countdown_timer_queue = xQueueCreate(2, sizeof(uint8_t));

  // null check
  if(countdown_timer_queue == NULL){
    debugln("[-] Error. Could not create the queue");
  }

  debugln("[+]Queue created");
  debugln("[+]Initializing tasks...");

  // create tasks on the core
  xTaskCreatePinnedToCore(
    receive_commands,
    "receive_commands",
    1024,
    NULL,
    2,
    NULL, 
    1
  );

  xTaskCreatePinnedToCore(
    send_commands,
    "send_commands",
    1024,
    NULL,
    1,
    &send_commands_task_handle, 
    1
  );

  debugln("[+]Tasks initialized...");

}

void loop() {}