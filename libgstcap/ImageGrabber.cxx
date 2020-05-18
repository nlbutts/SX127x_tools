#include <stdio.h>
#include <gst/app/gstappsink.h>

#include "ImageGrabber.h"

using std::vector;

  // "eos" :  void user_function (GstElement* object,
  //                              gpointer user_data);
  // "new-preroll" :  GstFlowReturn user_function (GstElement* object,
  //                                               gpointer user_data);
  // "new-sample" :  GstFlowReturn user_function (GstElement* object,
  //                                              gpointer user_data);

GstFlowReturn new_preroll (GstAppSink * appsink)
{
    printf("new_preroll\n");
    return GST_FLOW_OK;
}

GstFlowReturn new_sample (GstAppSink * appsink)
{
    printf("new_sample\n");
    return GST_FLOW_OK;
}

void get_data(ImageGrabber *ig)
{
    GstAppSink * sink = (GstAppSink*)ig->get_sink();
    while (ig->running())
    {
        GstSample * sample = gst_app_sink_pull_sample(sink);
        printf("Have sample\n");
        GstBuffer *buffer;
        GstCaps *caps;
        GstStructure *s;
        gint width, height;
        gboolean res;

        /* get the snapshot buffer format now. We set the caps on the appsink so
         * that it can only be an rgb buffer. The only thing we have not specified
         * on the caps is the height, which is dependant on the pixel-aspect-ratio
         * of the source material */
        // caps = gst_sample_get_caps (sample);
        // if (!caps) {
        //   g_print ("could not get snapshot format\n");
        //   exit (-1);
        // }
        // s = gst_caps_get_structure (caps, 0);

        buffer = gst_sample_get_buffer(sample);
        gsize bufSize = gst_buffer_get_size(buffer);
        printf("Got buffer of size %u\n", (uint32_t)bufSize);
        // uint8_t * data = new uint8_t[bufSize];
        // if (data)
        // {
        //     if (gst_buffer_extract(buffer, 0, data, bufSize) == bufSize)
        //     {
        //         // Copied the correct amount, write it to the file
        //         ofs.write((const char *)data, bufSize);
        //     }
        //     delete [] data;
        // }

        gst_sample_unref(sample);
    }
}

ImageGrabber::ImageGrabber(int timeout, bool debug)
: _timeout(timeout)
, _debug(debug)
, _running(false)
{

}

ImageGrabber::~ImageGrabber()
{

}

void ImageGrabber::init()
{
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    /* Initialize GStreamer */
    gst_init (NULL, NULL);

    printf("Trying to setup gstreamer pipeline\n");

    /* Build the pipeline */
    //const gchar * initstr = "gst-launch-1.0 -v rpicamsrc num-buffers=-1 ! video/x-raw,width=640,height=480, framerate=41/1 ! timeoverlay time-mode=\"buffer-time\" ! omxh264enc ! h264parse ! matroskamux ! filesink location=test.mkv";
    const gchar * initstr = "rpicamsrc num-buffers=-1 ! video/x-raw,width=640,height=480, framerate=15/1 ! timeoverlay time-mode=\"buffer-time\" ! omxh264enc ! h264parse ! matroskamux ! appsink name=sink";
    pipeline = gst_parse_launch(initstr, NULL);

    printf("pipeline: %08x\n", pipeline);

    _sink = gst_bin_get_by_name (GST_BIN (pipeline), "sink");
    printf("Got sink: %08X\n", _sink);

    _running = true;
    std::thread t1(get_data, this);

    /* Start playing */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus (pipeline);
    msg = gst_bus_timed_pop_filtered (bus,
                                      GST_CLOCK_TIME_NONE,
                                      GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    printf("Stopping thread\n");
    _running = false;
    t1.join();
    printf("Thread joined\n");

    /* Free resources */
    if (msg != NULL)
        gst_message_unref (msg);
    gst_object_unref (bus);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
}

bool ImageGrabber::grab(vector<uint8_t> &payload)
{
    bool success = false;
    return success;
}

