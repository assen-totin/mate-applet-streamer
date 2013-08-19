// This file is a part of Voody Blue Subtitler suit.
// 
// Author: Assen Totin <assen.totin@gmail.com>
//
// Home page: http://www.zavedil.com/software-desktop-vbs
//
// This software is released under GNU General Public License.
// See the LICENSE file for details or visit http://www.gnu.org/copyleft/gpl.html 
// for details.

#include "../common/common.h"
#include "video-gstreamer.h"

void gstreamer_pause() {
	gst_element_set_state (config.vbsm.gstreamer_playbin2, GST_STATE_PAUSED);
}


void gstreamer_play() {
	gst_element_set_state (config.vbsm.gstreamer_playbin2, GST_STATE_PLAYING);
}


void gstreamer_init(char file_name[1024], int *import_error_flag) {
	gst_init(0, NULL);

	char uri[2048];
	sprintf(&uri[0], "file://%s", &file_name[0]);
	GstElement *pipeline = gst_pipeline_new ("my-pipeline");

#ifdef HAVE_GST_0_10
	config.vbsm.gstreamer_playbin2 = gst_element_factory_make ("playbin2", "playbin2");
#elif HAVE_GST_1_0
	config.vbsm.gstreamer_playbin2 = gst_element_factory_make ("playbin", "playbin");
#endif

	g_object_set (G_OBJECT (config.vbsm.gstreamer_playbin2), "uri", &uri[0], NULL);

	GstElement *videosink = gst_element_factory_make (&config.vbsm.gstreamer_video_sink[0], "videosink");

	gst_bin_add_many (GST_BIN (pipeline), videosink, NULL);

	g_object_set (G_OBJECT (config.vbsm.gstreamer_playbin2), "video-sink", pipeline, NULL);

	GstStateChangeReturn ret = gst_element_set_state (config.vbsm.gstreamer_playbin2, GST_STATE_PAUSED);
	if (ret == GST_STATE_CHANGE_FAILURE) 
		*import_error_flag = 1;

	sleep(1);
}

static gboolean bus_cb (GstBus *bus, GstMessage *msg, gpointer data) {
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS: {
			g_debug ("end-of-stream");
			break;
		}
		case GST_MESSAGE_ERROR: {
			gchar *debug;
			GError *err;

			gst_message_parse_error (msg, &err, &debug);
			g_free (debug);

			g_warning ("Error: %s", err->message);
			g_error_free (err);
			break;
		}
		default:
			break;
	}

	return TRUE;
}


