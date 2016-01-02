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

// Menu strings
#ifdef HAVE_MATE
static const gchar *ui1 = 
"<menu name='SubMenu1' action='Recent'>"
;
static const gchar *ui2 = 
"</menu>"
"<menu name='SubMenu2' action='Favourites'>"
;
static const gchar *ui3 =
"</menu>"
"<menuitem name='MenuItem1' action='All' />"
"<menuitem name='MenuItem2' action='Settings' />"
"<menuitem name='MenuItem3' action='About' />"
;
#elif HAVE_GNOME_2
static const gchar *ui1 =
"<popup name='button3'>"
"<submenu name='SubMenu1' label='Recent'>"
;
static const gchar *ui2 =
"</submenu>"
"<submenu name='SubMenu2' label='Favourites'>"
;
static const gchar *ui3 =
"</submenu>"
"<menuitem name='MenuItem1' verb='All' label='All'/>"
"<menuitem name='MenuItem2' verb='About' label='About' pixtype='stock' pixname='gnome-stock-about'/>"
"</popup>"
;
#endif

#ifdef HAVE_GNOME_2
void applet_back_change (MyPanelApplet *a, MyPanelAppletBackgroundType type, GdkColor *color, GdkPixmap *pixmap, streamer_applet *applet) {
	// taken from the TrashApplet
	GtkRcStyle *rc_style;
	GtkStyle *style;

	// reset style
	gtk_widget_set_style (GTK_WIDGET(applet->applet), NULL);
	gtk_widget_set_style (GTK_WIDGET(applet->event_box), NULL);
	rc_style = gtk_rc_style_new ();
	gtk_widget_modify_style (GTK_WIDGET(applet->applet), rc_style);
	gtk_widget_modify_style (GTK_WIDGET(applet->event_box), rc_style);
	g_object_unref (rc_style);

	switch (type) {
		case PANEL_COLOR_BACKGROUND:
			gtk_widget_modify_bg (GTK_WIDGET(applet->applet), GTK_STATE_NORMAL, color);
			gtk_widget_modify_bg (GTK_WIDGET(applet->event_box), GTK_STATE_NORMAL, color);
			break;

		case PANEL_PIXMAP_BACKGROUND:
			style = gtk_style_copy (gtk_widget_get_style (GTK_WIDGET(applet->applet)));
			if (style->bg_pixmap[GTK_STATE_NORMAL])
				g_object_unref (style->bg_pixmap[GTK_STATE_NORMAL]);
			style->bg_pixmap[GTK_STATE_NORMAL] = g_object_ref(pixmap);
			gtk_widget_set_style (GTK_WIDGET(applet->applet), style);
			gtk_widget_set_style (GTK_WIDGET(applet->event_box), style);
			g_object_unref (style);
			break;

		case PANEL_NO_BACKGROUND:
		default:
			break;
	}
}

#elif HAVE_MATE
	#ifdef HAVE_GTK2
void applet_back_change (MyPanelApplet *a, MyPanelAppletBackgroundType type, GdkColor *color, GdkPixmap *pixmap, streamer_applet *applet) {
	#elif HAVE_GTK3
void applet_back_change (MyPanelApplet *a, MyPanelAppletBackgroundType type, GdkRGBA *color, cairo_pattern_t *pattern, streamer_applet *applet) {
	#endif

	// Use MATE-provided wrapper to change the background (same for both GTK2 and GTK3)
	mate_panel_applet_set_background_widget (a, GTK_WIDGET(applet->applet));
	mate_panel_applet_set_background_widget (a, GTK_WIDGET(applet->event_box));
}
#endif

void applet_destroy(MyPanelApplet *applet_widget, streamer_applet *applet) {
	sqlite3_close(applet->sqlite);
	g_main_loop_quit(applet->loop);
	g_assert(applet);
	g_free(applet);
	return;
}


gboolean applet_main (MyPanelApplet *applet_widget, const gchar *iid, gpointer data) {
	streamer_applet *applet;
	char *zErrMsg;
	int res, i;
	char ui[24576];

	if (strcmp (iid, APPLET_ID) != 0) 
		return FALSE;

	// i18n
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE_NAME, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE_NAME, "utf-8");
	textdomain (PACKAGE_NAME);

	// Init 
	applet = g_malloc0(sizeof(streamer_applet));
	applet->applet = applet_widget;
	memset(&applet->xml_listen_url[0], '\0', 1024);
	memset(&applet->xml_bitrate[0], '\0', 1024);
	memset(&applet->xml_server_name[0], '\0', 1024);
	memset(&applet->xml_genre[0], '\0', 1024);
	memset(&applet->url[0], '\0', 1024);	
	memset(&applet->ui_fav[0], '\0', 10240);
	memset(&applet->ui_recent[0], '\0', 10240);
	applet->timestamp = time(NULL);
	applet->xml_curr_entries = 0;
	applet->icecast_total_entries = 0;
	applet->status = 0;

	for (i=0; i<10; i++) {
		memset(&applet->hash_fav[i].hash[0], '\0', 64);
		memset(&applet->hash_recent[i].hash[0], '\0', 64);
	}

	applet->settings.show_notifications = 0;
	applet->settings.duration_notifications = 5;

#ifdef HAVE_GNOME_2
	applet->bonobo_counter = 0;
#endif

	// Check home dir, copy skel database
	char applet_home_dir[1024], skel_file[1024], local_file[1024];
	struct stat stat_buf;
	struct passwd *pw = getpwuid(getuid());
	sprintf(&applet_home_dir[0], "%s/%s", pw->pw_dir, APPLET_HOME_DIR);
	int stat_res = stat(&applet_home_dir[0], &stat_buf);
	int errsv = errno;
	if ((stat_res == 0) && (!S_ISDIR(stat_buf.st_mode))){
			push_notification(_("Streamer Applet Error"), _("Cannot access configuration directory. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
			return FALSE;
	}
	else if (stat_res == -1) {
		if (errsv == ENOENT) {
			int mkdir_res = mkdir(&applet_home_dir[0], 0755);
			if (mkdir_res == 1) {
				push_notification(_("Streamer Applet Error"), _("Cannot create configuration directory. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
				return FALSE;
			}
		}
		else {
			push_notification(_("Streamer Applet Error"), _("Cannot verify configuration directory. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
			return FALSE;
		}
	}
	sprintf(&skel_file[0], "%s/%s", APPLET_SKEL_PATH, APPLET_SQLITE_DB_FILENAME);
	sprintf(&local_file[0], "%s/%s/%s", pw->pw_dir, APPLET_HOME_DIR, APPLET_SQLITE_DB_FILENAME);
	stat_res = stat(&local_file[0], &stat_buf);
	errsv = errno;
	if ((stat_res == 0) && (!S_ISREG(stat_buf.st_mode))){
		push_notification(_("Streamer Applet Error"), _("Database file is not a regular file. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
	}
	else if (stat_res == -1) {
		if (errsv == ENOENT) {
			if (!cp(&local_file[0], &skel_file[0])) {
				push_notification(_("Streamer Applet Error"), _("Cannot copy database file to configuration directory. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
				return FALSE;
			}
		}
		else {
			push_notification(_("Streamer Applet Error"), _("Cannot verify database file. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
			return FALSE;
		}
	}
	
	// Test DB connection, upgrade DB if necessary
	if (!sqlite_connect(applet)) {
		push_notification(_("Streamer Applet Error"), _("Unable to connect to DB. Exiting."), NULL, DEFAULT_NOTIFICATION_DURATION);
		return FALSE;
	}

	zErrMsg = 0;
	res = sqlite3_exec(applet->sqlite, "SELECT version FROM version", cb_sql_version, (void*) applet, &zErrMsg);
	if (res != SQLITE_OK) {
		push_notification(_("Streamer Applet Error"), zErrMsg, NULL, DEFAULT_NOTIFICATION_DURATION);
		sqlite3_free(zErrMsg);
		return FALSE;
	}

	if (applet->db_version == 1) {
		// Upgrade DB to version 2
		sqlite_insert(applet, "CREATE TABLE custom_stations (server_name VARCHAR(255), listen_url VARCHAR(255), bitrate VARCHAR(255), genre VARCHAR(255))");
		sqlite_insert(applet, "ALTER TABLE stations RENAME TO icecast_stations");
		sqlite_insert(applet, "UPDATE version SET version=2");
	}

	sqlite3_close(applet->sqlite);

	// Init GStreamer
	gstreamer_init(applet);

	// Get an image
	char image_file[1024];
	sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PAUSE);
	applet->image = gtk_image_new_from_file (&image_file[0]);

	// Put the image into a container (it needs to receive actions)
	applet->event_box = gtk_event_box_new();
	gtk_container_add (GTK_CONTAINER (applet->event_box), applet->image);

	// Put the container into the applet
	gtk_container_add (GTK_CONTAINER (applet->applet), applet->event_box);

	// Define menu action group
#ifdef HAVE_MATE
	applet->action_group = gtk_action_group_new ("Streamer_Applet_Actions");
	gtk_action_group_set_translation_domain(applet->action_group, PACKAGE_NAME);
	gtk_action_group_add_actions (applet->action_group, applet_menu_actions_mate, G_N_ELEMENTS (applet_menu_actions_mate), applet);
#endif

	// Get last 10 entried from Recent & Fav, then fetch last URL
	if (!sqlite_connect(applet))
		return FALSE;

	zErrMsg = 0;
	memset(&applet->ui_recent[0], '\0', 1);

	res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url FROM recent GROUP BY listen_url ORDER BY unix_timestamp DESC LIMIT 10", cb_sql_recent_10, (void*) applet, &zErrMsg);
	if (res != SQLITE_OK) {
		push_notification(_("Streamer Applet Error"), zErrMsg, NULL, DEFAULT_NOTIFICATION_DURATION);
		sqlite3_free(zErrMsg);
		return FALSE;
	}

	memset(&applet->ui_fav[0], '\0', 1);
	res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url FROM favourites LIMIT 10", cb_sql_fav_10, (void*) applet, &zErrMsg);
	if (res != SQLITE_OK) {
		push_notification(_("Streamer Applet Error"), zErrMsg, NULL, DEFAULT_NOTIFICATION_DURATION);
		sqlite3_free(zErrMsg);
		return FALSE;
	}

	res = sqlite3_exec(applet->sqlite, "SELECT * FROM recent ORDER BY unix_timestamp DESC LIMIT 1", cb_sql_recent, (void*) applet, &zErrMsg);
	if (res != SQLITE_OK) {
		push_notification(_("Streamer Applet Error"), zErrMsg, NULL, DEFAULT_NOTIFICATION_DURATION);
		sqlite3_free(zErrMsg);
		return FALSE;
	}

	sqlite3_free(zErrMsg);
	sqlite3_close(applet->sqlite);

	// Build menu
	sprintf(&ui[0], "%s %s %s %s %s", ui1, &applet->ui_recent[0], ui2, &applet->ui_fav[0], ui3);
#ifdef HAVE_MATE
	mate_panel_applet_setup_menu(applet->applet, &ui[0], applet->action_group);
#elif HAVE_GNOME_2
	BonoboUIVerb bnb1 = BONOBO_UI_UNSAFE_VERB ("All", G_CALLBACK (menu_cb_all));
	applet->applet_menu_actions_gnome[applet->bonobo_counter] = bnb1;
	applet->bonobo_counter++;
	BonoboUIVerb bnb2 = BONOBO_UI_UNSAFE_VERB ("About", G_CALLBACK (menu_cb_about));
	applet->applet_menu_actions_gnome[applet->bonobo_counter] = bnb2;
	applet->bonobo_counter++;
	BonoboUIVerb bnb3 = BONOBO_UI_VERB_END;
	applet->applet_menu_actions_gnome[applet->bonobo_counter] = bnb3;
	applet->bonobo_counter++;
	panel_applet_setup_menu(applet->applet, &ui[0], applet->applet_menu_actions_gnome, applet);
#endif

	// Merge menu
	//GError **error;
	//char ui[10240];
	//sprintf(&ui[0], "<menu action='SubMenu1'>\n<menuitem action='All'/>\n<menuitem action='All'/></menu>");
	//guint merge_id = gtk_ui_manager_add_ui_from_string (applet->applet->priv->ui_manager, &ui[0], -1, error);

	// Settings: Prepare DConf - GNOME2 only
#ifdef HAVE_GNOME_2
	if (!panel_applet_gconf_get_bool(PANEL_APPLET(applet->applet), "have_settings", NULL)) {
		panel_applet_gconf_set_bool(PANEL_APPLET(applet->applet), "have_settings", TRUE, NULL);
		panel_applet_gconf_set_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_1, 0, NULL);
		panel_applet_gconf_set_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_2, 5, NULL);
	}
#endif

	// Load settings
#ifdef HAVE_MATE
	applet->gsettings = g_settings_new_with_path(APPLET_GSETTINGS_SCHEMA, APPLET_GSETTINGS_PATH);
	applet->settings.show_notifications = g_settings_get_int(applet->gsettings, APPLET_KEY_OPTION_1);
	applet->settings.duration_notifications = g_settings_get_int(applet->gsettings, APPLET_KEY_OPTION_2);
#elif HAVE_GNOME_2
	applet->settings.show_notifications = panel_applet_gconf_get_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_1, NULL);
	applet->settings.duration_notifications = panel_applet_gconf_get_int(PANEL_APPLET(applet->applet), APPLET_KEY_OPTION_2, NULL);
#endif

	// Signals
	g_signal_connect(G_OBJECT(applet->event_box), "button_press_event", G_CALLBACK (on_left_click), (gpointer)applet);
	g_signal_connect(G_OBJECT(applet->applet), "change_background", G_CALLBACK (applet_back_change), (gpointer)applet);
	g_signal_connect(G_OBJECT(applet->applet), "destroy", G_CALLBACK(applet_destroy), (gpointer)applet);

	// Tooltip
	gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), _("No stream selected. Right-click to load one."));

	// Show applet
	gtk_widget_show_all (GTK_WIDGET (applet->applet));

	// Run
	applet->loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (applet->loop);

	return TRUE;
}

#ifdef HAVE_MATE
MATE_PANEL_APPLET_OUT_PROCESS_FACTORY (APPLET_FACTORY, PANEL_TYPE_APPLET, APPLET_NAME, applet_main, NULL)
#elif HAVE_GNOME_2
PANEL_APPLET_BONOBO_FACTORY (APPLET_FACTORY, PANEL_TYPE_APPLET, APPLET_NAME, APPLET_VERSION, applet_main, NULL)
#endif

