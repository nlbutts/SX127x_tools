#pragma once

#include <stdint.h>
#include <vector>
#include <thread>
#include <gst/gst.h>

/**
 * @brief GStreamer Image Grabber
 * @details The purpose of this class is to grab images and return them in a
 * buffer
 *
 */
class ImageGrabber
{
public:
    /**
     * @brief Constructor
     *
     * @param timeout timeout in milliseconds
     * @param debug true to enable debug false otherwise
     */
    ImageGrabber(int timeout, bool debug);
    virtual ~ImageGrabber();

    /**
     * @brief Initializes the gstreamer pipeline.
     * @details Initializes the gstreamer pipeline and blocks
     */
    void init();

    /**
     * @brief Grabs and image
     * @details Grabs and image and returns it as a vector
     *
     * @param payload reference to a vector
     * @return true if successful, false if it timed out and CRC didn't match
     */
    bool grab(std::vector<uint8_t> &payload);

    bool running()              {return _running;}
    GstElement* get_sink()      {return _sink;}

private:

private:
    int         _timeout;
    int         _debug;
    bool        _running;
    GstElement *_sink;
};