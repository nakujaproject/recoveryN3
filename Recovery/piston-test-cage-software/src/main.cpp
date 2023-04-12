#include <Arduino.h>
#include "soc/rtc.h"
#include "defs.h"
#include "HX711.h"

// load cell pins
const int load_cell_dt = 25;
const int load_cell_sck = 26;

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

void setup() {
  Serial.begin(115200);
  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  setup_load_cell();
}

void loop() {

  callibrate_load_cell();
  
}