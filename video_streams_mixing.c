#include <stdio.h>
#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

int tutorial_main(int argc, char *argv[])
{
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Build the pipeline */
    pipeline = gst_parse_launch("\
    uridecodebin uri=rtmp://192.168.64.4/live/screen ! videoscale ! videoconvert ! videorate ! video/x-raw, width=1280, height=720 ! queue ! \
    videomixer name=m sink_1::xpos=0 sink_1::ypos=720 sink_2::xpos=1280 sink_2::ypos=0 ! videoconvert ! autovideosink sync=false \
    uridecodebin uri=rtmp://192.168.64.5/live/screen ! videoscale ! videoconvert ! videorate ! video/x-raw, width=1280, height=720 ! queue ! m.",
                                NULL);
    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg =
        gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                   GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* See next tutorial for proper error message handling/parsing */
    if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR)
    {
        g_error("An error occurred! Re-run with the GST_DEBUG=*:WARN environment "
                "variable set for more details.");
    }

    /* Free resources */
    gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}

int main(int argc, char *argv[])
{
    setenv("GST_DEBUG", "*:WARN", 1);
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
    printf("Running on macOS\n");
    return gst_macos_main(tutorial_main, argc, argv, NULL);
#else
    return tutorial_main(argc, argv);
#endif
}
