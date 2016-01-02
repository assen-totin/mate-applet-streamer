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

void settings_notifications_toggle (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;

	int value = 0;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		applet->settings.show_notifications = 1;
		value = 1;
	}
	else {
		applet->settings.show_notifications = 0;
		value = 0;
	}

	// Save option
#ifdef HAVE_MATE
	g_settings_set_int(applet->gsettings, APPLET_KEY_OPTION_1, value);
#elif HAVE_GNOME_2
	panel_applet_gconf_set_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_1, value, NULL);
#endif
}


void settings_notifications_time (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;

	int value = (int) gtk_spin_button_get_value (GTK_SPIN_BUTTON(widget));
	applet->settings.duration_notifications = value;

	// Save option
#ifdef HAVE_MATE
	g_settings_set_int(applet->gsettings, APPLET_KEY_OPTION_2, value);
#elif HAVE_GNOME_2
	panel_applet_gconf_set_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_2, value, NULL);
#endif
}


void menu_cb_settings (GtkAction *action, streamer_applet *applet) {
	// Setting to show or not notifications
	GtkWidget *check_setting_1 = gtk_check_button_new_with_label (_("Show notifications with song titles"));
	gtk_widget_set_name(check_setting_1, "setting_1");
	g_signal_connect (G_OBJECT(check_setting_1), "clicked", G_CALLBACK (settings_notifications_toggle), (gpointer) applet);

	// If notifications are enabled, enable changing their duration
	if (applet->settings.show_notifications)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_setting_1), TRUE);

	// Setting to set duration of notifications
	int initial_duration = (applet->settings.duration_notifications) ? applet->settings.duration_notifications : DEFAULT_NOTIFICATION_DURATION;
	GtkObject *adjustment = gtk_adjustment_new (initial_duration, 1, 60, 1, 10, 10);

	GtkWidget *butt_setting_2 = gtk_spin_button_new (GTK_ADJUSTMENT(adjustment), 1.0, 0);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(butt_setting_2), applet->settings.duration_notifications);
	g_signal_connect (G_OBJECT(butt_setting_2), "value-changed", G_CALLBACK (settings_notifications_time), (gpointer) applet);

	// NB: GTK has NO way to disable the arrows of the spin button; setting the EDIATABLE property to OFF does not disable the arrows.
	//g_object_set(butt_setting_2, "editable", FALSE, NULL);

	// Pack settings widgets
	GtkWidget *settings_vbox;
#ifdef HAVE_GTK2
	settings_vbox = gtk_vbox_new (FALSE, 0);
#elif HAVE_GTK3
	settings_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(settings_vbox), check_setting_1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(settings_vbox), butt_setting_2, TRUE, TRUE, 0);

	// Assemble window
	applet->quitDialog = gtk_dialog_new_with_buttons (_("Settings"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
	GtkWidget *buttonClose = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL);

	gtk_dialog_set_default_response (GTK_DIALOG (applet->quitDialog), GTK_RESPONSE_CANCEL);
#ifdef HAVE_GTK2
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(applet->quitDialog)->vbox), settings_vbox);
#elif HAVE_GTK3
	gtk_container_add (GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(applet->quitDialog))), settings_vbox);
#endif

	g_signal_connect (G_OBJECT(buttonClose), "clicked", G_CALLBACK (quitDialogClose), (gpointer) applet);

	gtk_widget_show_all(GTK_WIDGET(applet->quitDialog));
}

