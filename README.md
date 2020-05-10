# Overview

This library is meant to control an SX127x radio module from a Raspberry Pi.
Specifically it was meant to use the [Adafruit RFM9x display Bonnet](https://learn.adafruit.com/adafruit-radio-bonnets)

But it will work with any SX127x/RFM9x device as long as it is probably
connected to the Raspberry Pi

# Pin Connections

The following pins are used to control the SX127x/RFM9x device

| Header Pin | BCM Pin | Usage      | Notes       |
| ---------- | ------- | ---------- | ----------- |
| 22         | 25      | RFM Reset  | Active low  |
| 26         |  7      | RFM CS     | Active low  |
| 15         | 22      | DIO0       |             |
| 16         | 23      | DIO1       |             |
| 18         | 24      | DIO2       |             |
| 23         | 11      | RFM SCLK   |             |
| 19         | 10      | RFM MOSI   |             |
| 21         |  9      | RFM MISO   |             |


# Setup
## Install the prereqs

```
sudo apt update
sudo apt install -y cmake protobuf
```

## Build
Copy the files to the Raspberry Pi, then run the following:
```
cd <git checkout directory>
cmake ..
make -j
```

It will create a static library for the SX127x code and several apps.
* fsk - program to send/receive random data using FSK
* lora - program to send/receive random data using LORA
* fxfer - program to send/receive a file using FSK


# Adafruit Bonnect Pinout Reference

This is just for reference, as I got tired looking for the schematic

| Header Pin | BCM Pin | Usage      | Notes       |
| ---------- | ------- | ---------- | ----------- |
| 1          |         | +3.3V      |             |
| 2          |         | +5V        |             |
| 3          | 2       | SDA        | Connects to the display             |
| 4          |         | +5V        |             |
| 5          | 3       | SCL        | Connects to the display             |
| 6          |         | GND        |              |
| 7          | 4       | GPIO       |              |
| 8          | 14      | TXD        |              |
| 9          |         | GND        |              |
| 10         | 15      | RXD        |              |
| 11         | 17      | GPIO       |              |
| 12         | 18      | GPIO       |              |
| 13         | 27      | GPIO       |              |
| 15         | 22      | GPIO       |              |
| 16         | 23      | DIO1       |              |
| 17         |         | +3.3V      |              |
| 18         | 24      | DIO2       |              |
| 19         | 10      | RFM MOSI   |              |
| 20         |         | GND        |              |
| 21         | 9       | RFM MISO   |              |
| 22         | 25      | RFM RST    | Active Low   |
| 23         | 11      | RFM SCLK   | Active Low   |
| 24         | 8       | CS0        |    |
| 25         |         | GND        |    |
| 26         | 7       | RFM CS     | CS1   |
| 27         | 0       | EE DATA    |    |
| 28         | 1       | EE CLK     |    |
| 29         | 5       | GPIO       | Button   |
| 30         |         | GND        |    |
| 31         | 6       | GPIO       | Button   |
| 32         | 12      | GPIO       | Button   |
| 33         | 13      | GPIO       |     |
| 34         |         | GND        |     |
| 35         | 19      | GPIO       |     |
| 36         | 16      | GPIO       |     |
| 37         | 26      | GPIO       |     |
| 38         | 20      | GPIO       |     |
| 39         |         | GND        |     |
| 40         | 21      | GND        |     |
