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

void push_notification (gchar *title, gchar *body, gchar *icon) {
        NotifyNotification* notification;
	GError* error = NULL;

	notify_init(PACKAGE_NAME);

#ifdef HAVE_LIBMATENOTIFY
        notification = notify_notification_new (title, body, icon, NULL);
#elif HAVE_LIBNOTIFY
	notification = notify_notification_new (title, body, icon);
#endif

        notify_notification_set_timeout (notification, 5000);

        notify_notification_show (notification, &error);

        g_object_unref (G_OBJECT (notification));
        notify_uninit ();
}


static gboolean applet_on_click (GtkWidget *event_box, GdkEventButton *event, streamer_applet *applet) {
	static GtkWidget *label;
	char msg[1024], image_file[1024];

	// We only process left clicks here
	if (event->button != 1)
		return FALSE;

	// No URL loaded? 
	if (strlen(applet->url) == 0) {
		push_notification(_("No stream selected."), _("Right-click to load one."), NULL);

		return TRUE;
	}

	// If we are playing, then swap icon and pause
	if (applet->status == 1) {
		applet->status = 0;
                sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PAUSE);
                sprintf(&msg[0], "%s%s", _("PAUSED: "), &applet->name[0]);
                gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);
		gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);
		applet->timestamp = time(NULL);
		gstreamer_pause(applet);
		return TRUE;
	}

        // If we are paused, then swap icon and pause
        if (applet->status == 0) {
		applet->status = 1;
                sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PLAY);
                sprintf(&msg[0], "%s%s", _("PLAYING: "), &applet->name[0]);
                gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);
                gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);
		
		// Reconnect if we were paused for more than 10 seconds
		time_t now = time(NULL);
		if ((now - applet->timestamp) > 10) {
			gst_element_set_state (applet->gstreamer_playbin2, GST_STATE_READY);
			g_object_set (G_OBJECT (applet->gstreamer_playbin2), "uri", &applet->url[0], NULL);
		}

                gstreamer_play(applet);

                return TRUE;
	}
                
	return FALSE;
}


static void applet_back_change (MatePanelApplet *a, MatePanelAppletBackgroundType type, GdkColor *color, GdkPixmap *pixmap, streamer_applet *applet) {
        /* taken from the TrashApplet */
        GtkRcStyle *rc_style;
        GtkStyle *style;

        /* reset style */
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

static void applet_destroy(MatePanelApplet *applet_widget, streamer_applet *applet) {
	sqlite3_close(applet->sqlite);
	g_main_loop_quit(applet->loop);
        g_assert(applet);
        g_free(applet);
        return;
}


int cb_sql_recent(void *data, int argc, char **argv, char **azColName) {
        streamer_applet *applet = data;
	char msg[1024];

        sprintf(&applet->url[0], "%s", argv[1]);
        sprintf(&applet->name[0], "%s", argv[0]);
	sprintf(&msg[0], "%s%s", _("PAUSED: "), &applet->name[0]);
	gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);

	g_object_set (G_OBJECT (applet->gstreamer_playbin2), "uri", argv[1], NULL);
	
        return 0;
}

int cb_sql_recent_10(void *data, int argc, char **argv, char **azColName) {
        streamer_applet *applet = data;

	GChecksum *checksum = g_checksum_new(G_CHECKSUM_MD5);
	g_checksum_update(checksum, argv[1], -1);

        GtkActionEntry action;
        action.name = g_checksum_get_string(checksum);
        action.label = argv[0];
        action.callback = G_CALLBACK(play_menu);

        gtk_action_group_add_actions(applet->action_group, &action, 1, applet);

	sprintf(&applet->ui[0], "%s<menuitem action='%s' />", &applet->ui[0], g_checksum_get_string(checksum));

        return 0;
}

void play_menu (GtkAction *action, streamer_applet *applet) {

}

const gchar *ui1 = "<menu name='SubMenu1' action='Recent'>";

const gchar *ui2 = "</menu><menu name='SubMenu2' action='Favourites'>";

const gchar *ui3 = 
"</menu>"
"<menuitem name='MenuItem1' action='All' />"
"<menuitem name='MenuItem2' action='About' />"
;

static const GtkActionEntry applet_menu_actions[] = {
	{ "Favourites", GTK_STOCK_GO_FORWARD, "_Favourites", NULL, NULL, NULL },
        { "Recent", GTK_STOCK_GO_FORWARD, "_Recent", NULL, NULL, NULL },
        { "All", GTK_STOCK_EXECUTE, "_All Stations", NULL, NULL, G_CALLBACK (menu_cb_all) },
        { "About", GTK_STOCK_ABOUT, "_About", NULL, NULL, G_CALLBACK (menu_cb_about) }
};


static gboolean applet_main (MatePanelApplet *applet_widget, const gchar *iid, gpointer data) {
	streamer_applet *applet;
	char *zErrMsg;
	int res;

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
	applet->status = 0;
	sprintf(&applet->url[0], '\0');
	applet->timestamp = time(NULL);
	memset(&applet->xml_listen_url[0], '\0', 1024);
	memset(&applet->xml_bitrate[0], '\0', 1024);
	memset(&applet->xml_server_name[0], '\0', 1024);
	memset(&applet->xml_genre[0], '\0', 1024);
	applet->xml_curr_entries = 0;
	applet->icecast_total_entries = 0;
	memset(&applet->ui[0], '\0', 8096);

	// Check home dir, copy skel database
	char applet_home_dir[1024], skel_file[1024], local_file[1024];
	struct stat stat_buf;
	struct passwd *pw = getpwuid(getuid());
	sprintf(&applet_home_dir[0], "%s/%s", pw->pw_dir, APPLET_HOME_DIR);
	int stat_res = stat(&applet_home_dir[0], &stat_buf);
        int errsv = errno;
        if ((stat_res == 0) && (!S_ISDIR(stat_buf.st_mode))){
			push_notification(_("Streamer Applet Error"), _("Cannot access configuration directory. Exiting."), NULL);
			return FALSE;
        }
        else if (stat_res == -1) {
                if (errsv == ENOENT) {
                        int mkdir_res = mkdir(&applet_home_dir[0], 0755);
                        if (mkdir_res == 1) {
				push_notification(_("Streamer Applet Error"), _("Cannot create configuration directory. Exiting."), NULL);
				return FALSE;
			}
                }
                else {
			push_notification(_("Streamer Applet Error"), _("Cannot verify configuration directory. Exiting."), NULL);
			return FALSE;
		}
        }
	sprintf(&skel_file[0], "%s/%s", APPLET_SKEL_PATH, APPLET_SQLITE_DB_FILENAME);
	sprintf(&local_file[0], "%s/%s/%s", pw->pw_dir, APPLET_HOME_DIR, APPLET_SQLITE_DB_FILENAME);
	stat_res = stat(&local_file[0], &stat_buf);
	errsv = errno;
	if ((stat_res == 0) && (!S_ISREG(stat_buf.st_mode))){
		push_notification(_("Streamer Applet Error"), _("Database file is not a regular file. Exiting."), NULL);
	}
	else if (stat_res == -1) {
		if (errsv == ENOENT) {
			if (!cp(&local_file[0], &skel_file[0])) {
				push_notification(_("Streamer Applet Error"), _("Cannot copy database file to configuration directory. Exiting."), NULL);
				return FALSE;
			}
		}
		else {
			push_notification(_("Streamer Applet Error"), _("Cannot verify database file. Exiting."), NULL);
			return FALSE;
		}
	}
	
	// Test DB connection
	if (!sqlite_connect(applet)) {
		push_notification(_("Streamer Applet Error"), _("Unable to connect to DB. Exiting."), NULL);
		return FALSE;
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

	// Menu action group
	applet->action_group = gtk_action_group_new ("Streamer_Applet_Actions");
	gtk_action_group_add_actions (applet->action_group, applet_menu_actions, G_N_ELEMENTS (applet_menu_actions), applet);

        // Menu prepare - Recent & Fav
        if (!sqlite_connect(applet))
                return FALSE;
        zErrMsg = 0;
        res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url FROM recent GROUP BY listen_url ORDER BY unix_timestamp DESC LIMIT 10", cb_sql_recent_10, (void*) applet, &zErrMsg);
        if (res != SQLITE_OK) {
                push_notification(_("Streamer Applet Error"), zErrMsg, NULL);
                sqlite3_free(zErrMsg);
                return FALSE;
        }

	sqlite3_close(applet->sqlite);

	// Menu
	//GtkActionGroup *action_group = gtk_action_group_new ("Streamer Applet Actions");
	//gtk_action_group_add_actions (action_group, applet_menu_actions, G_N_ELEMENTS (applet_menu_actions), applet);
	//mate_panel_applet_setup_menu_from_file(applet->applet, "/usr/share/mate-2.0/ui/streamer-applet-menu.xml", action_group);
	//mate_panel_applet_setup_menu_from_file(applet->applet, "/usr/share/mate-2.0/ui/streamer-applet-menu.xml", applet->action_group);
	
	char unf[8096];
	sprintf(&unf[0], "%s %s %s %s", ui1, &applet->ui[0], ui2, ui3);
	mate_panel_applet_setup_menu(applet->applet, &unf[0], applet->action_group);

	// Merge menu
	//GError **error;
	//char ui[10240];
	//sprintf(&ui[0], "<menu action='SubMenu1'>\n<menuitem action='All'/>\n<menuitem action='All'/></menu>");
	//guint merge_id = gtk_ui_manager_add_ui_from_string (applet->applet->priv->ui_manager, &ui[0], -1, error);

	// Signals
        g_signal_connect(G_OBJECT(applet->event_box), "button_press_event", G_CALLBACK (applet_on_click), (gpointer)applet);
        g_signal_connect(G_OBJECT(applet->applet), "change_background", G_CALLBACK (applet_back_change), (gpointer)applet);
	g_signal_connect(G_OBJECT(applet->applet), "destroy", G_CALLBACK(applet_destroy), (gpointer)applet);

	// Tooltip
	gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), _("No stream selected. Right-click to load one."));

        // Fetch last URL
        if (!sqlite_connect(applet)) {
                push_notification(_("Streamer Applet Error"), _("Unable to connect to DB. Exiting."), NULL);
                return FALSE;
        }
        zErrMsg = 0;
        res = sqlite3_exec(applet->sqlite, "SELECT * FROM recent ORDER BY unix_timestamp DESC LIMIT 1", cb_sql_recent, (void*) applet, &zErrMsg);
        if (res != SQLITE_OK) {
		//push_notification(_("Streamer Applet Error"), _("Unable to read DB. Exiting"), NULL);
		push_notification(_("Streamer Applet Error"), zErrMsg, NULL);
		sqlite3_free(zErrMsg);
		return FALSE;
	}
	sqlite3_free(zErrMsg);
	sqlite3_close(applet->sqlite);

        // Show applet
        gtk_widget_show_all (GTK_WIDGET (applet->applet));

	// Run
        applet->loop = g_main_loop_new (NULL, FALSE);
        g_main_loop_run (applet->loop);

	return TRUE;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY (APPLET_FACTORY, PANEL_TYPE_APPLET, APPLET_NAME, applet_main, NULL)

