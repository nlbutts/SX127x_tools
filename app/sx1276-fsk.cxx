/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <chrono>
#include <thread>
#include <tclap/CmdLine.h>
#include "sx1276.hpp"

using namespace std;
using namespace std::chrono_literals;

int shouldRun = true;

void
sig_handler(int signo)
{
    if (signo == SIGINT)
        shouldRun = false;
}

int main(int argc, char** argv)
{
    try {
    TCLAP::CmdLine cmd("SX1276 Tester", ' ', "0.9");
    TCLAP::ValueArg<int> tx_pwr("p","txpower","Transmit power", false, 14, "dBm", cmd);
    TCLAP::SwitchArg rx("r","rx","Receive node", cmd, false);
    TCLAP::ValueArg<int> bw("", "bw","Bandwidth", false, 25000, "Hz", cmd);
    TCLAP::ValueArg<int> dev("", "dev","Freq Deviation", false, 50000, "Hz", cmd);
    TCLAP::ValueArg<int> afc("", "afc","Bandwidth AFC", false, 83000, "Hz", cmd);
    TCLAP::ValueArg<int> datarate("", "datarate","Tx Datarate", false, 5000, "Hz", cmd);
    TCLAP::ValueArg<int> payload("", "payload","payload length", false, 32, "Bytes", cmd);
    TCLAP::ValueArg<int> delay("", "delay","delay between packet tx", false, 100, "ms", cmd);
    TCLAP::SwitchArg crc("", "crc","Enable CRC", cmd, false);
    TCLAP::SwitchArg debug("", "debug","Enable debug", cmd, false);
    cmd.parse( argc, argv );

    signal(SIGINT, sig_handler);
    //! [Interesting]

    cout << "Hello FSK" << endl;

    // Instantiate an SX1276 using default parameters
    // PiRfHat
    //SX1276 sensor(SX1276::chipRevision, 1, 8, 6);
    // Adafruit board
    SX1276 sensor(SX1276::chipRevision,
                  0,  // bus
                  7,  // SPI CS
                  25);// RESET

    // 915Mhz
    sensor.setChannel(915000000);

    // FSK configuration (rx and tx must be configured the same):
    // Tx output power = 14 dBm
    // FSK freq deviation = 25000 Hz
    // FSK bandwidth = 50000 bps
    // FSK AFC bandwidth = 83333 Hz
    // FSK datarate = 50000 bps
    // FSK preamble len = 5
    // FSK fixed length payload = false
    // FSK CRC check = true
    // FSK (rx) continuous Rx mode = false

    sensor.setTxConfig(sensor.MODEM_FSK,
                       tx_pwr.getValue(),
                       dev.getValue(),
                       0,       // bandwidth NA for TX
                       datarate.getValue(),
                       0,       // code rate NA for FSK
                       5,       // Preamble length
                       true,    //fixLen
                       crc.getValue(),
                       false,   // FreqHopOn
                       0,       // hopPeriod
                       false);  // iqInverted

    sensor.setRxConfig(sensor.MODEM_FSK,
                       bw.getValue(),
                       datarate.getValue(),
                       0,       // coderate NA for FSK
                       afc.getValue(),
                       5,       // preamble length
                       0,       // symTimeout NA for FSK
                       true,    // fixLen
                       payload.getValue(),
                       crc.getValue(),
                       false,   // freqHopOn
                       0,       // hopPeriod
                       false,   // iqInverted
                       false);  // rxContinous

    int count = 0;
    int buflen = 64;
    char buffer[buflen];
    std::chrono::milliseconds ms{delay.getValue()};

    uint32_t bufSize = payload.getValue();
    uint8_t * databuffer = new uint8_t[bufSize];

    if (debug.getValue())
    {
      sensor.debug(true);
    }

    while (shouldRun) {
        if (!rx.getValue())
        {
            for (int i = 0; i < bufSize; i++)
            {
              databuffer[i] = rand();
            }
            databuffer[0] = (count >> 24) & 0xFF;
            databuffer[1] = (count >> 16) & 0xFF;
            databuffer[2] = (count >>  8) & 0xFF;
            databuffer[3] = (count      ) & 0xFF;
            sensor.send(databuffer, bufSize, 3000);
            snprintf(buffer, buflen, "Ping %d", count++);
            cout << "Sending..." << std::string(buffer) << endl;
            //sensor.sendStr(string(buffer), 3000);

            sensor.setSleep();
            std::this_thread::sleep_for(ms);
        } else {
            // receiving
            cout << "Attempting to receive..." << endl;
            int rv;
            if ((rv = sensor.setRx(3000)))
            {
                cout << "setRx returned " << rv << endl;
            }
            else
            {
              uint8_t * rxData = sensor.getRxBuffer();
              uint32_t seqNum = (rxData[0] << 24) |
                                (rxData[1] << 16) |
                                (rxData[2] <<  8) |
                                (rxData[3]      );

              //cout << "RSSI: " << rssi << " -- Received Buffer: " << sensor.getRxBufferStr() << endl;
              //printf("RSSI: %3d  -- Received Buffer: %s\n", sensor.getRxRSSI(), sensor.getRxBufferStr().c_str());
              printf("RSSI: %3d  -- Received bytes: %d -- Seq Num: %d\n", sensor.getRxRSSI(), sensor.getRxLen(), seqNum);
            }

            // go back to upm_delay when done
            //sensor.setSleep();
            std::this_thread::sleep_for(ms);
        }
    }
    //! [Interesting]

    cout << "Exiting..." << endl;

    } catch (TCLAP::ArgException &e)  // catch any exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    return 0;
}
