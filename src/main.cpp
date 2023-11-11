#include <SPI.h>
#include <Ethernet.h>
#include <sml.h>

// Blue LED on the W5500-EVB-Pico
#define LED_PIN 25

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
#define CS_PIN 17

// Random MAC address with the Wiznet OUI
static byte mac[] = {0x00, 0x08, 0xDC, 0x4A, 0x2F, 0x7E};

void setup()
{
    Serial.begin(115200);

    // Two possible ways to set the pins
    // TX: GPIO04, RX: GPIO05
    // TX: GPIO08, RX: GPIO09
    Serial1.setTX(8);
    Serial1.setRX(9);
    // SML baud rate is 9600
    Serial1.begin(9600);

    // set pin 25 for blinking the LED
    pinMode(LED_PIN, OUTPUT);

    // Setp the WS5500;
    Ethernet.init(CS_PIN);

    // start the Ethernet connection and check if DHCP succeeds
    if (Ethernet.begin(mac) == 0)
    {
        while (1)
        {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
        }
    }

    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
    if (Serial1.available())
    {
        // Read the SML data
        int data = Serial1.read();
        Serial.print(data, HEX);
    }
}