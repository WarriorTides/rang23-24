import gi

gi.require_version("Gst", "1.0")
from gi.repository import Gst

# Initialize GStreamer
Gst.init(None)

# Define the GStreamer pipeline
pipeline = Gst.parse_launch(
    "udpsrc port=5601 caps='application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264'! rtph264depay! avdec_h264! videoconvert! autovideosink"
)

# Start playing
pipeline.set_state(Gst.State.PLAYING)

# Wait for the pipeline to finish
pipeline.get_state(Gst.CLOCK_TIME_NONE)

# Clean up
pipeline.set_state(Gst.State.NULL)
