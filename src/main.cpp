#include <SPI.h>
#include <Ethernet.h>
#include <sml.h>

#define OTETHERNET
#include <ArduinoOTA.h>

#include <config.h>

// Random MAC address with the Wiznet OUI
static byte mac[] = {0x00, 0x08, 0xDC, 0x4A, 0x2F, 0x7E};
static byte ip[] = {192, 168, 178, 2}; // Set your own IP address

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
// If you want to find codes print out the SML file in comma seperated HEX and use this Regex to find the codes: 0x01, 0x00, 0x[0-9a-fA-F]{2}, 0x[0-9a-fA-F]{2}, 0x00, 0x[0-9a-fA-F]{2}
OBISHandler OBISHandlers[] = {
    {{0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, &PowerInHandler},        /* 1.8.0 */
    {{0x01, 0x00, 0x02, 0x08, 0x00, 0xff}, &PowerOutHandler},       /* 2.8.0 */
    {{0x01, 0x00, 0x10, 0x07, 0x00, 0xff}, &PowerCurrUsageHandler}, /* 16.7.0 */
    {0, 0}

};

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
    Ethernet.begin(mac, ip);

    ArduinoOTA.begin(Ethernet.localIP(), DEVICE_NAME, OTA_PASSWORD, InternalStorage);

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
        // WhIn = WhOut = Curr = -2;
        return;
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

        return;
    }
    else if (currentState == SML_UNEXPECTED)
    {
        Serial.print(F(">>> Unexpected byte\n"));
        return;
    }
    else if (currentState == SML_FINAL)
    {
        Serial.print(F(">>> Successfully received a complete message!\n"));

        return;
    }
    else if (currentState == SML_CHECKSUM_ERROR)
    {
        Serial.print(F(">>> Checksum error.\n"));
        return;
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
    // Serial.println("Hello World");
}