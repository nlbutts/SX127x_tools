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
sudo apt install -y cmake libprotobuf-dev gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-tools gstreamer1.0-omx gstreamer1.0-plugins-base gstreamer1.0-plugins-base-apps libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev
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


# File Transfer Protocol

The file transfer app is pretty simple. The payload is preprended with
the 16-bit CCITT CRC, 32-bits of the total bytes to follow, then the payload.

| 32-bit payload size | 16-bit CCITT | Payload |

Everything is stored in bit endian format. For example if the CRC is 0x1234
the first byte transmitted would be 0x12 and the second byte would be 0x34.

We would call this new data structure the **txpayload**. The **txpayload**
is further split into 250 data bytes and a 16-bit sequence counter. The
SX127x payload is then 252 bytes

| Sequence number | TxPayload |

## Examples
These settings work pretty well for the file transfer:
Transmitter
```
/fxfer -f test.bin --datarate 200000 --dev 110000 -p 5 --crc
```

Receiver
```
./fxfer -f test.bin --datarate 200000 --bw 180000 -r --crc
```

If you add some error correcting codes, then you can remove the CRC and fix
bit errors.

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
