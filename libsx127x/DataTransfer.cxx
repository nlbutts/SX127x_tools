#include <algorithm>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <fstream>

#include "DataTransfer.h"
#include "crc-ccitt.h"
#include "sx1276.hpp"

using std::vector;
using namespace std::chrono_literals;

DataTransfer::DataTransfer(SX1276 &radio, int timeout, bool debug)
: _radio(&radio)
, _timeout(timeout)
, _debug(debug)
{

}

DataTransfer::~DataTransfer()
{

}

bool DataTransfer::send(vector<uint8_t> &payload)
{
    uint16_t crc = crc_ccitt(0, payload.data(), payload.size());
    printf("Calculated CRC: %04x\n", crc);
    uint32_t size = payload.size() + DataOverhead;
    // Create the TxPayload
    vector<uint8_t> txpayload(size);
    // We are only transmitting 32-bits so force it into 32-bits
    auto it = txpayload.begin();
    *it++ = (size >> 24) & 0xFF;
    *it++ = (size >> 16) & 0xFF;
    *it++ = (size >>  8) & 0xFF;
    *it++ = (size      ) & 0xFF;
    *it++ = (crc >> 8  ) & 0xFF;
    *it++ = (crc       ) & 0xFF;
    std::copy(payload.begin(), payload.end(), it);

    int txPayloadSize = txpayload.size();
    int seq = 0;
    auto txPayloadIt = txpayload.begin();

    printf("Transmitting %d bytes \n", txPayloadSize);

    while (txPayloadSize > 0)
    {
        //printf("Bytes left: %d\n", txPayloadSize);
        int transfer_size = txPayloadSize > PayloadSize ? PayloadSize : txPayloadSize;
        int remaining = PayloadSize - transfer_size;
        txPayloadSize -= transfer_size;
        printf("Seq: %d -- txPayloadSize: %d\n", seq, txPayloadSize);
        // Copy to tx array
        vector<uint8_t> buf(TxPayloadSize);
        auto it = buf.begin();
        *it++ = (seq >> 8) & 0xFF;
        *it++ = (seq     ) & 0xFF;
        //std::copy(txPayloadIt, txPayloadIt + transfer_size + 2, it);
        while (transfer_size-- > 0)
        {
            // I tried to use std::copy, this is so much easier to understand
            *it++ = *txPayloadIt++;
        }

        while (remaining-- > 0)
        {
            // Pad
            *it++ = 0;
        }

        print_vec(buf.data(), buf.size());

        _radio->send(buf.data(), buf.size(), _timeout);
        //_radio->setSleep();
        //std::this_thread::sleep_for(100ms);
        seq++;
    }
    _radio->setSleep();

    return false;
}

bool DataTransfer::receive(vector<uint8_t> &payload)
{
    bool success = false;
    int expectedSeq = 0;
    int seq = 0;
    int payloadSize = 0;
    std::vector<uint8_t> rxPayload;
    uint32_t expectedTotalSize = 0;
    uint16_t expectedCRC;
    uint32_t receivedSize = 0;
    bool done = false;

    // Wait for the first packet
    printf("Attempting to receive...\n");
    while (!done && SX1276::REVENT_DONE == _radio->setRx(_timeout))
    {
        // successfully received a packet.
        int rxLen = _radio->getRxLen();
        uint8_t * ptr = _radio->getRxBuffer();

        print_vec(ptr, 242);

        if (rxLen == TxPayloadSize)
        {
            seq = (ptr[0] << 8) | (ptr[1]);
            printf("Rx seq: %d RSSI: %d\n", seq, _radio->getRxRSSI());
            if (seq == expectedSeq)
            {
                // We have a good seq number, put the data into rxPayload buffer
                for (int i = SeqSize; i < TxPayloadSize; i++)
                {
                    rxPayload.push_back(ptr[i]);
                }

                if (expectedSeq == 0)
                {
                    // Extract CRC and total size
                    auto it = rxPayload.cbegin();
                    expectedTotalSize = 0;
                    expectedTotalSize |= *it++ << 24;
                    expectedTotalSize |= *it++ << 16;
                    expectedTotalSize |= *it++ << 8;
                    expectedTotalSize |= *it++;
                    expectedCRC  = 0;
                    expectedCRC |= *it++ << 8;
                    expectedCRC |= *it++;
                    receivedSize += PayloadSize;
                    printf("Expected bytes: %d -- CRC: %04x\n", expectedTotalSize, expectedCRC);
                }
                else
                {
                    receivedSize += PayloadSize;
                    //printf("Received bytes: %d\n", receivedSize);
                }
                expectedSeq++;
            }
            else
            {
                printf("Invalid sequence encountered\n");
                done = true;
            }
        }
        else
        {
            printf("Only received %d bytes\n", rxLen);
            done = true;
        }

        if ((expectedTotalSize != 0) && (receivedSize >= expectedTotalSize))
        {
            printf("rxPayload size: %d bytes\n", rxPayload.size());
            print_vec(rxPayload.data(), rxPayload.size());
            uint16_t calculatedCRC = crc_ccitt(0, rxPayload.data() + DataOverhead, expectedTotalSize - DataOverhead);
            printf("Expected CRC: %04x -- Calculated CRC: %04x\n", expectedCRC, calculatedCRC);
            if (calculatedCRC == expectedCRC)
            {
                printf("CRC PASSED\n");
                success = true;
                payload.assign(rxPayload.begin()+6, rxPayload.end());
            }
            else
            {
                printf("CRC FAILED\n");
            }
            done = true;
        }
    }

    return success;
}

void DataTransfer::print_vec(uint8_t * buf, int num)
{
    if (_debug)
    {
        for (int i = 0; i < num; i++)
        {
            printf("%02x ", buf[i]);
            if ((i % 16) == 15)
            {
                printf("\n");
            }
        }
        printf("\n");
    }
}
