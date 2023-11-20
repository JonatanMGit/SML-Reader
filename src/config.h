/*
W5500-EVB-Pico Pinout
I/O	Pin Name	Description
I	GPIO16	Connected to MISO on W5500
O	GPIO17	Connected to CSn on W5500
O	GPIO18	Connected to SCLK on W5500
O	GPIO19	Connected to MOSI on W5500
O	GPIO20	Connected to RSTn on W5500
I	GPIO21	Connected to INTn on W5500
I	GPIO24	VBUS sense - high if VBUS is present, else low
O	GPIO25	Connected to user LED
I	GPIO29	Used in ADC mode (ADC3) to measure VSYS/3

https://docs.wiznet.io/Product/iEthernet/W5500/w5500-evb-pico/
*/

// Blue LED on the W5500-EVB-Pico
#define LED_PIN 25
#define CS_PIN 17

#define DEVICE_NAME "w5500-evb-pico"
#define OTA_PASSWORD "had!J89H"

#define MQTT_SERVER ""
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_ID "w5500-evb-pico"
