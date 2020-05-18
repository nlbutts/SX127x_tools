#include <stdio.h>

#include <thread>
#include <sx1276.hpp>
#include "DataCollector.h"

using std::vector;

DataCollector::DataCollector(int timeout, int debug, SX1276 &radio, int payload)
: _timeout(timeout)
, _debug(debug)
, _radio(&radio)
, _payload(_payload)
, _running(true)
{
    printf("Starting datacollector thread\n");
    _t = new std::thread(&DataCollector::sendDataThread, this);
}

DataCollector::~DataCollector()
{

}

void DataCollector::pushData(std::vector<uint8_t> data)
{
    //printf("Received callback data, datasize: %d\n", data.size());
    _m.lock();
    for (auto v : data)
    {
        _q.push(v);
    }
    _m.unlock();
}

void DataCollector::sendDataThread()
{
    printf("Running sendDataThread\n");
    std::vector<uint8_t> txData;
    while (_running)
    {
        _m.lock();
        if (_q.size() > _payload)
        {
            txData.clear();
            for (int i = 0; i < _payload; i++)
            {
                txData.push_back(_q.front());
                _q.pop();
            }
            _radio->send(txData.data(), txData.size(), 3000);
            _radio->setSleep();
            printf("Queue size: %d\n", _q.size());
        }
        _m.unlock();
    }
}
