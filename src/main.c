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


void applet_back_change (MatePanelApplet *a, MatePanelAppletBackgroundType type, GdkColor *color, GdkPixmap *pixmap, streamer_applet *applet) {
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

void applet_destroy(MatePanelApplet *applet_widget, streamer_applet *applet) {
	sqlite3_close(applet->sqlite);
	g_main_loop_quit(applet->loop);
        g_assert(applet);
        g_free(applet);
        return;
}


/*
static const GtkActionEntry applet_menu_actions[] = {
	{ "Favourites", GTK_STOCK_GO_FORWARD, "_Favourites", NULL, NULL, NULL },
        { "Recent", GTK_STOCK_GO_FORWARD, "_Recent", NULL, NULL, NULL },
        { "All", GTK_STOCK_EXECUTE, "_All Stations", NULL, NULL, G_CALLBACK (menu_cb_all) },
        { "About", GTK_STOCK_ABOUT, "_About", NULL, NULL, G_CALLBACK (menu_cb_about) }
};
*/

gboolean applet_main (MatePanelApplet *applet_widget, const gchar *iid, gpointer data) {
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

	// Define menu action group
	applet->action_group = gtk_action_group_new ("Streamer_Applet_Actions");
	gtk_action_group_add_actions (applet->action_group, applet_menu_actions, G_N_ELEMENTS (applet_menu_actions), applet);

        // Get last 10 entried from Recent & Fav, then fetch last URL
        if (!sqlite_connect(applet))
                return FALSE;
        zErrMsg = 0;
        res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url FROM recent GROUP BY listen_url ORDER BY unix_timestamp DESC LIMIT 10", cb_sql_recent_10, (void*) applet, &zErrMsg);
        if (res != SQLITE_OK) {
                push_notification(_("Streamer Applet Error"), zErrMsg, NULL);
                sqlite3_free(zErrMsg);
                return FALSE;
        }

        res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url FROM favourites LIMIT 10", cb_sql_fav_10, (void*) applet, &zErrMsg);
        if (res != SQLITE_OK) {
                push_notification(_("Streamer Applet Error"), zErrMsg, NULL);
                sqlite3_free(zErrMsg);
                return FALSE;
        }

        res = sqlite3_exec(applet->sqlite, "SELECT * FROM recent ORDER BY unix_timestamp DESC LIMIT 1", cb_sql_recent, (void*) applet, &zErrMsg);
        if (res != SQLITE_OK) {
                push_notification(_("Streamer Applet Error"), zErrMsg, NULL);
                sqlite3_free(zErrMsg);
                return FALSE;
        }

	sqlite3_free(zErrMsg);
	sqlite3_close(applet->sqlite);

	// Build menu
	sprintf(&ui[0], "%s %s %s %s %s", ui1, &applet->ui_recent[0], ui2, &applet->ui_fav[0], ui3);
	mate_panel_applet_setup_menu(applet->applet, &ui[0], applet->action_group);

	// Merge menu
	//GError **error;
	//char ui[10240];
	//sprintf(&ui[0], "<menu action='SubMenu1'>\n<menuitem action='All'/>\n<menuitem action='All'/></menu>");
	//guint merge_id = gtk_ui_manager_add_ui_from_string (applet->applet->priv->ui_manager, &ui[0], -1, error);

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

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY (APPLET_FACTORY, PANEL_TYPE_APPLET, APPLET_NAME, applet_main, NULL)

