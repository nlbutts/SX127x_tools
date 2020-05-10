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

int
main(int argc, char** argv)
{
    try {
    TCLAP::CmdLine cmd("SX1276 Tester", ' ', "0.9");
    TCLAP::ValueArg<int> pwr_pin("","power","Power pin", false, 7, "BCM pin", cmd);
    TCLAP::ValueArg<int> tx_pwr("p","txpower","Transmit power", false, 14, "dBm", cmd);
    TCLAP::SwitchArg rx("r","rx","Receive node", cmd, false);
    TCLAP::ValueArg<int> sf("", "sf","Spread Factor", false, 7, "Hz", cmd);
    TCLAP::ValueArg<int> bw("", "bw","Bandwidth", false, 125000, "Hz", cmd);
    cmd.parse( argc, argv );

    signal(SIGINT, sig_handler);
    //! [Interesting]

    cout << "Hello Lora" << endl;

    //! [Interesting]
    cout << "Specify an argument to go into receive mode.  Default is transmit" << endl;

    // Instantiate an SX1276 using default parameters
    //SX1276 sensor;
    SX1276 sensor(SX1276::chipRevision,
                  0,  // bus
                  7,  // SPI CS
                  25);// RESET

    // 915Mhz
    sensor.setChannel(915000000);

    // LORA configuration (rx and tx must be configured the same):
    // Tx output power = 14 dBm
    // LORA bandwidth = 125000 (can also be 250K and 500K)
    // LORA spreading factor = 7
    // LORA coding rate = 1 (4/5)
    // LORA preamble len = 8
    // LORA symbol timeout = 5
    // LORA fixed payload = false
    // LORA IQ inversion = false
    // LORA (rx) continuous Rx mode = true

    sensor.setTxConfig(sensor.MODEM_LORA,
                       tx_pwr.getValue(),
                       0,       // deviation
                       bw.getValue(),
                       sf.getValue(),
                       1,       // Code Rate (4/5)
                       8,       // Preamble length
                       true,    // fixLen
                       true,    // CRC
                       false,   // freqHopOn
                       0,       // hopPeriod
                       false);  // iqInverted

    sensor.setRxConfig(sensor.MODEM_LORA,
                       bw.getValue(),
                       sf.getValue(),
                       1,       // coderate (4/5)
                       0,       // afc
                       8,       // Preamble length
                       5,       // symTimeout
                       true,    // fixLen
                       32,      // payloadLen
                       true,    // crcOn
                       false,   // freqHopOn
                       0,       // hopPeriod
                       false,   // iqInverted
                       true);   // rxContinous

    int count = 0;
    int buflen = 64;
    char buffer[buflen];

    while (shouldRun) {
        if (!rx.getValue())
        {
            snprintf(buffer, buflen, "Ping %d", count++);
            cout << "Sending..." << std::string(buffer) << endl;
            sensor.sendStr(string(buffer), 3000);
            sensor.setSleep();
            std::this_thread::sleep_for(100ms);
        }
        else
        {
            // receiving
            cout << "Attempting to receive..." << endl;
            int rv;
            if ((rv = sensor.setRx(3000))) {
                cout << "setRx returned " << rv << endl;
            } else {
                cout << "Received Buffer: " << sensor.getRxBufferStr() << endl;
            }

            // go back to upm_delay when done
            //sensor.setSleep();
            //std::this_thread::sleep_for(250ms);
        }
    }

    //! [Interesting]

    cout << "Exiting..." << endl;

    } catch (TCLAP::ArgException &e)  // catch any exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

    return 0;
}
