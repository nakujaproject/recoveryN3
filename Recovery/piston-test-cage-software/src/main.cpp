#include <Arduino.h>
#include <esp_now.h>
#include "soc/rtc.h"
#include "defs.h"
#include "HX711.h"

/* load cell pins  */
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


  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
            // by the SCALE parameter (not set yet)
            
  scale.set_scale(INSERT YOUR CALIBRATION FACTOR);
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
            // by the SCALE parameter set with set_scale

  Serial.println("Readings:");



void setup() {
  Serial.begin(115200);
  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  setup_load_cell();
}

void loop() {
  callibrate_load_cell();
  
}