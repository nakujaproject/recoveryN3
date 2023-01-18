## Communication Systems

### Introduction
Wireless radio technologies, such as Wi-Fi, are used daily to enable inter-device communications.

In opposition to standard wireless technologies referred to as "short range", long-range allow devices to communicate over
kilometers-wide distances at a low energy cost at an expense of the data rate.
Some existing long-range technologies.

| Technology    | Description                                                                                                                                |
|---------------|--------------------------------------------------------------------------------------------------------------------------------------------|
 | Zigbee        | Low-data rate, low power applications                                                                                                      |
| LoRaWAN       | Provides low power, wide range connectivity                                                                                                |
| Wifi          | Provides connection between devices within a range of 20m-40m                                                                              |
| Long Range RF | This is a long range RF link kit with up to 2km range<br/>[https://wiki.seeedstudio.com/2KM_Long_Range_RF_link_kits_w_encoder_and_decoder] |

From the above technologies a quick comparison between LoRaWan and the long range RF link we are able to breakdown some of the upsides and downsides of each.

|                    | LoRaWAN        | Long Range RF                     |
|--------------------|----------------|-----------------------------------|
| Operating Voltage  | 3.3V           | Receiver(3-5V), Transmitter(3-9V) |
| Bandwidth          | 125Khz-500Khz  | 1.5Mhz                            |
| Transmission Range | 7Km            | 20Km                              |
| Data Rate          | 300bps-500Kbps | <5Kbps                            |

From the comparison we get to find that the **Long Range RF Link** is the most convenient for our use case.

Available at pixel : https://www.pixelelectric.com/iot-and-wireless/transmitter-and-receiver/433mhz-rf-transmitter-and-receiver-link/

