#pragma once

#include <stdint.h>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>

// Forward declariation
class SX1276;


/**
 * @brief GStreamer Image Grabber
 * @details The purpose of this class is to grab images and return them in a
 * buffer
 *
 */
class DataCollector
{
public:
    /**
     * @brief Constructor
     *
     * @param timeout timeout in milliseconds
     * @param debug true to enable debug false otherwise
     */
    DataCollector(int timeout, int debug, SX1276 &radio, int payload);
    virtual ~DataCollector();

    void pushData(std::vector<uint8_t> data);

private:
    void sendDataThread();

private:
    int                 _timeout;
    int                 _debug;
    SX1276             *_radio;
    int                 _payload;
    bool                _running;
    std::queue<uint8_t> _q;
    std::thread        *_t;
    std::mutex          _m;
};