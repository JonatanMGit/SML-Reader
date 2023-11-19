#include <SPI.h>
#include <Ethernet.h>
#include <sml.h>

#define OTETHERNET
#include <ArduinoOTA.h>

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

EthernetServer server(80);

// Store each OBIS value in a variable
double WhIn, WhOut, Curr;
typedef struct
{
    const unsigned char OBIS[6];
    void (*Handler)();
} OBISHandler;

sml_states_t currentState;

void PowerInHandler()
{
    smlOBISWh(WhIn);
}
void PowerOutHandler()
{
    smlOBISWh(WhOut);
}
void PowerCurrUsageHandler()
{
    smlOBISW(Curr);
}

// OBIS handlers for Apator Picus ehz.060.d
// Check your own meter for the correct OBIS values
OBISHandler OBISHandlers[] = {
    {{0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, &PowerInHandler},  /* 1-0:1.8.0*255 kWh */
    {{0x01, 0x00, 0x02, 0x08, 0x00, 0xff}, &PowerOutHandler}, /* 1-0:2.8.0*255 kWh */
    {{0x01, 0x00, 0x10, 0x07, 0x00, 0xff}, &PowerCurrUsageHandler},

    {{0, 0}}};

void setup()
{
    Serial.begin(115200);

    // Two possible ways to set the pins
    // TX: GPIO04, RX: GPIO05
    Serial2.setTX(4);
    Serial2.setRX(5);
    // SML baud rate is 9600
    Serial2.begin(9600);

    // set pin 25 for onboard LED
    pinMode(LED_PIN, OUTPUT);

    // Setp the WS5500;
    Ethernet.init(CS_PIN);

    // start the Ethernet connection and check if DHCP succeeds
    if (Ethernet.begin(mac) == 0)
    {
        // blink the led 5 times if DHCP fails
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
        }
    }
    ArduinoOTA.begin(Ethernet.localIP(), "w5500-evb-pico", "had!J89H", InternalStorage);

    server.begin();

    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
}

void readByte(unsigned char currentChar)
{
    unsigned int iHandler = 0;
    currentState = smlState(currentChar);
    if (currentState == SML_START)
    {
        // reset values
        WhIn = WhOut = Curr = -2;
    }
    else if (currentState = SML_LISTEXTENDED)
    {
        /* check handlers on last received list */
        for (iHandler = 0; OBISHandlers[iHandler].Handler != 0 &&
                           !(smlOBISCheck(OBISHandlers[iHandler].OBIS));
             iHandler++)
            ;
        if (OBISHandlers[iHandler].Handler != 0)
        {
            OBISHandlers[iHandler].Handler();
        }
    }
    else if (currentState == SML_UNEXPECTED)
    {
        Serial.print(F(">>> Unexpected byte\n"));
    }
    else if (currentState == SML_FINAL)
    {
        Serial.print(F(">>> Successfully received a complete message!\n"));
    }
    else if (currentState == SML_CHECKSUM_ERROR)
    {
        Serial.print(F(">>> Checksum error.\n"));
    }
}

void webServer()
{
    EthernetClient client = server.available();

    // send all the values of the OBIS values to the client
    if (client)
    {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println("Refresh: 1"); // refresh the page automatically every 5 sec to automatically update the values
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");

        client.print("<h1>Powermeter</h1>");
        client.print("<p>Power in: ");
        client.print(WhIn);
        client.print(" Wh</p>");

        client.print("<p>Power out: ");
        client.print(WhOut);
        client.print(" Wh</p>");

        client.print("<p>Power consumption: ");
        client.print(Curr);
        client.print(" W</p>");

        client.print("</html>");
        client.flush();
        client.stop();
    }
}

void loop()
{
    ArduinoOTA.poll();
    Ethernet.maintain();
    webServer();
    // Read Serial2 for SML messages
    while (Serial2.available())
    {
        readByte(Serial2.read());
    }
}