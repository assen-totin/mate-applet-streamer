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

	snprintf(&msg[0], 1023, "%s%s", _("PLAYING: "), &applet->name[0]);
	gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);

	applet->status = 1;
}

gboolean gstreamer_tag(GstBus *bus, GstMessage *message, gpointer data) {
	int i;
	char msg[1024];
	streamer_applet *applet = data;

	if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_TAG) {
		GstTagList *tags = NULL;
		gchar *title;

		// Extract list of tags
		gst_message_parse_tag (message, &tags);

		// Extract title
		gboolean has_title = gst_tag_list_get_string (tags, GST_TAG_TITLE, &title);
		if (has_title) {
			// See if the new title is different from current one
			GChecksum *checksum = g_checksum_new(G_CHECKSUM_MD5);
			g_checksum_update(checksum, (guchar *) title, -1);

			// Proceed only on change
			if (strcmp(g_checksum_get_string(checksum), &applet->title_checksum[0])) {
				// Save title checksum
				snprintf(&applet->title_checksum[0], 1023, "%s", g_checksum_get_string(checksum));

				// Set as tooltip
				snprintf(&msg[0], 1023, "%s%s (%s)", _("PLAYING: "), &applet->name[0], title);
				gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);

				// Push notification
				if (applet->settings.show_notifications)
					push_notification (_("Now Playing"), title, NULL, applet->settings.duration_notifications);
			}

			g_free (title);
		}

		gst_tag_list_unref (tags);
	}

	return TRUE;
}

void gstreamer_init(streamer_applet *applet) {
	gst_init(0, NULL);

#ifdef HAVE_GST_0_10
	applet->gstreamer_playbin2 = gst_element_factory_make ("playbin2", NULL);
#elif HAVE_GST_1_0
	applet->gstreamer_playbin2 = gst_element_factory_make ("playbin", NULL);

	// Add a watch and callback for messages
	GstBus *gstBus = gst_pipeline_get_bus (GST_PIPELINE(applet->gstreamer_playbin2));
	guint bus_watch_id = gst_bus_add_watch (gstBus, gstreamer_tag, (gpointer) applet);
	gst_object_unref (gstBus);
#endif
}


