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
 rtmpsrc location=rtmp://192.168.64.4/live/screen ! flvdemux ! h264parse ! \
    queue max-size-buffers=0 max-size-bytes=0 max-size-time=0 min-threshold-buffers=1 ! \
    rtph264pay pt=96 config-interval=1 ! \
    udpsink host=127.0.0.1 port=5000 sync=false async=false \
  rtmpsrc location=rtmp://192.168.64.5/live/screen ! flvdemux ! h264parse ! \
    queue max-size-buffers=0 max-size-bytes=0 max-size-time=0 min-threshold-buffers=1 ! \
    rtph264pay pt=96 config-interval=1 ! \
    udpsink host=127.0.0.1 port=5001 sync=false async=false \
  rtpbin name=rtpbin latency=10 \
    udpsrc caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264\" port=5000 ! rtpbin.recv_rtp_sink_0 \
    udpsrc caps=\"application/x-rtcp\" port=5000 ! rtpbin.recv_rtcp_sink_0 \
    udpsrc caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264\" port=5001 ! rtpbin.recv_rtp_sink_1 \
    udpsrc caps=\"application/x-rtcp\" port=5001 ! rtpbin.recv_rtcp_sink_1 \
  rtpbin. ! rtph264depay ! h264parse ! avdec_h264 ! \
    videorate ! videoconvert ! autovideosink sync=false",
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
