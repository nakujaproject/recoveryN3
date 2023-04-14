#include <Arduino.h>
#include <esp_now.h>
#include "soc/rtc.h"
#include "defs.h"
#include "HX711.h"

/* load cell pins  */
const int load_cell_dt = 25;
const int load_cell_sck = 26;

int real_average_reading, raw_average_reading;

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
    delay(1000);
    debugln("[..]Measuring...");
    delay(4000);

    load_cell.tare();
    debugln("[]Tare done");

    debugln("[+]Place a known weight on the scale..");

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
  debugln("[+]Before setting up the scale");

  load_cell.read();

  /* get average of 20 readings from the ADC */
  raw_average_reading = load_cell.read_average(20);

  /* reset cell to zero */
  
  return raw_average_reading;
}

int get_real_reading(){
  /* get real readings from the load cell */
  load_cell.set_scale(CALLIBRATION_FACTOR);
  load_cell.tare(); // reset the scale to 0

  real_average_reading = load_cell.read_average(20);

  return real_average_reading;
}

void transmit_readings(){
  /* transmit readings over wifi*/
}

void setup() {
  Serial.begin(115200);
  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  setup_load_cell();
  callibrate_load_cell();
}

void loop() {
  debugln(get_raw_readings);

  /* read  load cell */

  /* transmit to PC */

  
}