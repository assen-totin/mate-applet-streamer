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

void option_set (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;

	// Option 1: show notifications
	if (strcmp (gtk_button_get_label (GTK_BUTTON (widget)), "option_1")) {
		int value_1 = 0;

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			applet->options.show_notifications = 1;
			value_1 = 1;
		}
		else {
			applet->options.show_notifications = 0;
			value_1 = 0;
		}

		// Save option
#ifdef HAVE_MATE
		g_settings_set_int(applet->gsettings, APPLET_KEY_OPTION_1, value_1);
#elif HAVE_GNOME_2
		panel_applet_gconf_set_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_1, value_1, NULL);
#endif
	}
}


