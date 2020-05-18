#pragma once

#include <stdint.h>
#include <vector>
//#include <thread>
#include <functional>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class vector;

/**
 * @brief GStreamer Image Grabber
 * @details The purpose of this class is to grab images and return them in a
 * buffer
 *
 */
class ImageGrabber
{
public:
    typedef std::function<void(std::vector<uint8_t>)> cbfunc;

    /**
     * @brief Constructor
     *
     * @param timeout timeout in milliseconds
     * @param debug true to enable debug false otherwise
     */
    ImageGrabber(int timeout, bool debug);
    virtual ~ImageGrabber();

    /**
     * @brief Initializes the gstreamer pipeline and start capture
     */
    void start();

    /**
     * @brief Grabs and image
     * @details Grabs and image and returns it as a vector
     *
     * @param payload reference to a vector
     * @return true if successful, false if it timed out and CRC didn't match
     */
    bool grab(std::vector<uint8_t> &payload);

    bool running()              {return _running;}

    void registerCallback(cbfunc &cb) {_cb=cb;}

    void get_data();

private:

private:
    int         _timeout;
    int         _debug;
    bool        _running;
    GstAppSink *_sink;
    cbfunc      _cb;
};