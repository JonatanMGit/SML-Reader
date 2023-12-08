# SML-Reader

An W5500-EVB-Pico (and others!) based SML Smart Meter reader that transmits your Energy Meter data via MQTT to your Home Automation System like Home Assistant.

## Features

SML-Reader is capable of reading SML supported Smart Meters and forward their Data via MQTT to your Home Automation System. Alternatively to other projects this one is based on the W5500-EVB-Pico which is a very cheap and easy to use Microcontroller Board with an Ethernet Port which is usefull for basements with weak WiFi coverage.

## Supported Smart Meters

Any Smart Meter that supports the SML Protocol should work. I tested it with the following Smart Meters:

- [Apator PICUS](https://www.apator.com/de/produkte/strom/strommessung/moderne-messeinrichtung/picus)

The advertised data depends on what's supported and configured in your Smart Meter. For example you might need to disable the PIN and enable "Erweiterte Betriebsart (InF)" or even contact your Energy Provider to enable certain features trough the LMN Interface.

## Hardware

- [W5500-EVB-Pico](https://docs.wiznet.io/Product/iEthernet/W5500/w5500-evb-pico)
- [Smart Meter](#supported-smart-meters)
- Serial IR Reading Head(With a magnet to hold onto the Smart Meter)

## Setup

1. Open the `config.h` file and change values to your needs.
1. In main.cpp check your Ethernet Network and your OBIS Codes.
1. Compile and upload the code to your W5500-EVB-Pico.
1. Get data! (You can also use OTA now)

More information about the configuration can be found on my [Blog](https://miarecki.eu/posts/ha-sml-electricity-meter/).
