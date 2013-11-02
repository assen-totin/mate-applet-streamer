/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *  USA.
 *
 *  MATE Streamer applet written by Assen Totin <assen.totin@gmail.com>
 *  
 */

#include "../config.h"
#include "applet.h"

void gstreamer_pause(streamer_applet *applet) {
	char msg[1024];

	gst_element_set_state (applet->gstreamer_playbin2, GST_STATE_PAUSED);

        sprintf(&msg[0], "%s%s", _("PAUSED: "), &applet->name[0]);
        gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);

	applet->status = 0;
}


void gstreamer_play(streamer_applet *applet) {
	char msg[1024];

	gst_element_set_state (applet->gstreamer_playbin2, GST_STATE_PLAYING);

        sprintf(&msg[0], "%s%s", _("PLAYING: "), &applet->name[0]);
        gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);

	applet->status = 1;
}


void gstreamer_init(streamer_applet *applet) {
	gst_init(0, NULL);

	GstElement *pipeline = gst_pipeline_new ("my-pipeline");

#ifdef HAVE_GST_0_10
	applet->gstreamer_playbin2 = gst_element_factory_make ("playbin2", NULL);
#elif HAVE_GST_1_0
	applet->gstreamer_playbin2 = gst_element_factory_make ("playbin", NULL);
#endif

	//g_object_set (G_OBJECT (applet->gstreamer_playbin2), "uri", &applet->url[0], NULL);

	//sleep(1);
}

