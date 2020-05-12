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
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <chrono>
#include <thread>
#include <data_tx.pb.h>
#include <tclap/CmdLine.h>
#include "sx1276.hpp"
#include "DataTransfer.h"

using namespace std;
using namespace std::chrono_literals;

int shouldRun = true;

void sig_handler(int signo)
{
    if (signo == SIGINT)
        exit(0);
}

vector<uint8_t> load_file(string file)
{
    vector<uint8_t> data;
    FILE * f = fopen(file.c_str(), "rb");
    if (f != nullptr)
    {
        int v = fgetc(f);
        while (v != EOF)
        {
            data.push_back(v);
            v = fgetc(f);
        }
        fclose(f);
    }
    return data;
}

int main(int argc, char** argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    try {
    TCLAP::CmdLine cmd("SX1276 Tester", ' ', "0.9");
    TCLAP::ValueArg<int> tx_pwr("p","txpower","Transmit power", false, 14, "dBm", cmd);
    TCLAP::SwitchArg rx("r","rx","Receive node", cmd, false);
    TCLAP::ValueArg<int> bw("", "bw","Bandwidth", false, 25000, "Hz", cmd);
    TCLAP::ValueArg<int> dev("", "dev","Freq Deviation", false, 50000, "Hz", cmd);
    TCLAP::ValueArg<int> afc("", "afc","Bandwidth AFC", false, 83000, "Hz", cmd);
    TCLAP::ValueArg<int> datarate("", "datarate","Tx Datarate", false, 5000, "Hz", cmd);
    TCLAP::ValueArg<int> payload("", "payload","payload length", false, 242, "Bytes", cmd);
    TCLAP::ValueArg<int> delay("", "delay","delay between packet tx", false, 100, "ms", cmd);
    TCLAP::ValueArg<string> file("f", "file","File to send or receive", true, "", "file", cmd);
    TCLAP::SwitchArg crc("", "crc","Enable CRC", cmd, false);
    TCLAP::SwitchArg debug("", "debug","Enable debug", cmd, false);
    cmd.parse( argc, argv );

    signal(SIGINT, sig_handler);
    //! [Interesting]

    cout << "Hello File Transfer" << endl;

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
                       242,
                       crc.getValue(),
                       false,   // freqHopOn
                       0,       // hopPeriod
                       false,   // iqInverted
                       false);  // rxContinous

    int count = 0;
    std::chrono::milliseconds ms{delay.getValue()};

    if (debug.getValue())
    {
        sensor.debug(true);
    }

    DataTransfer xfer(sensor, 3000, debug.getValue());

    if (!rx.getValue())
    {
        vector<uint8_t> data = load_file(file.getValue());
        // vector<uint8_t> data(payload.getValue());
        // for (int i = 0; i < payload.getValue(); i++)
        // {
        //     data[i] = i;
        // }
        xfer.send(data);
        // cout << "Bytes loaded: " << data.size() << endl;
        // target_cam::target_data td;
        // cout << "Creating new protobuf" << endl;
        // td.set_name("Hello Target Cam");
        // td.set_img((void*)data.data(), data.size());
        // td.set_lat(string("42.1234"));
        // td.set_long_(string("-96.987"));
        // td.set_seq(2);
        // //fstream output("test.pb", ios::out | ios::trunc | ios::binary);
        // string output;
        // cout << "Serializing" << endl;
        // td.SerializeToString(&output);
        // int out_size = output.size();
        // const uint8_t * ptr = (const uint8_t*)output.data();
        // int payloadSize = payload.getValue();
        // int seq = 0;
        // while (out_size > 0)
        // {
        //     cout << "Bytes left " << out_size << endl;
        //     int transfer_size = out_size > payloadSize ? 255 : out_size;
        //     sensor.send(ptr, transfer_size, 3000);
        //     out_size -= transfer_size;
        //     // snprintf(buffer, buflen, "Ping %d", count++);
        // }
        // sensor.setSleep();
    }
    else
    {
        // receiving
        std::vector<uint8_t> buf;
        if (xfer.receive(buf))
        {
            cout << "Received file successfully" << endl;
            std::ofstream f(file.getValue(), std::ios::binary);
            for (auto v : buf)
            {
                f << v;
            }
            f.close();
        }
        else
        {
            cout << "Failed to receive file" << endl;
        }
        // cout << "Attempting to receive..." << endl;
        // int rv;
        // if ((rv = sensor.setRx(3000)))
        // {
        //     cout << "setRx returned " << rv << endl;
        // }
        // else
        // {
        //   uint8_t * rxData = sensor.getRxBuffer();
        //   uint32_t seqNum = (rxData[0] << 24) |
        //                     (rxData[1] << 16) |
        //                     (rxData[2] <<  8) |
        //                     (rxData[3]      );

        //   printf("RSSI: %3d  -- Received bytes: %d -- Seq Num: %d\n", sensor.getRxRSSI(), sensor.getRxLen(), seqNum);
        // }

        // // go back to upm_delay when done
        // std::this_thread::sleep_for(ms);
    }
    //! [Interesting]

    cout << "Exiting..." << endl;

    } catch (TCLAP::ArgException &e)  // catch any exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    return 0;
}
