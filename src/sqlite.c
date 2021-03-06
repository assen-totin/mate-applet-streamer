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


gboolean sqlite_connect(streamer_applet *applet) {
	char dbfile[1024];

	struct passwd *pw = getpwuid(getuid());

	sprintf(&dbfile[0], "%s/%s/%s", pw->pw_dir, APPLET_HOME_DIR, APPLET_SQLITE_DB_FILENAME);
	int res = sqlite3_open(&dbfile[0], &applet->sqlite);
	if (res) {
		push_notification(_("Streamer Applet Error"), _("Unable to connect to DB."), NULL, DEFAULT_NOTIFICATION_DURATION);
		return FALSE;
	}
	return TRUE;
}


gboolean sqlite_insert(streamer_applet *applet, char *sql) {
	//if (!sqlite_connect(applet)) 
	//		return FALSE;

	char *zErrMsg = 0;
	int res = sqlite3_exec(applet->sqlite, sql, cb_sql_true, 0, &zErrMsg);
	sqlite3_free(zErrMsg);

	//sqlite3_close(applet->sqlite);

	if (res != SQLITE_OK)
		return FALSE;
	return TRUE;
}


gboolean sqlite_delete(streamer_applet *applet, char *sql) {
	if (!sqlite_connect(applet)) 
		return FALSE;

	char *zErrMsg = 0;
	int res = sqlite3_exec(applet->sqlite, sql, cb_sql_true, 0, &zErrMsg);
	sqlite3_free(zErrMsg);

	sqlite3_close(applet->sqlite);

	if (res != SQLITE_OK)
		return FALSE;

	return TRUE;
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
	gboolean match = FALSE;
	GtkAction *existing_action;
	GtkActionEntry action;
	GList *element;
	int i;

	GChecksum *checksum = g_checksum_new(G_CHECKSUM_MD5);
	g_checksum_update(checksum, (guchar *) argv[1], -1);

	int strl = strlen(&applet->ui_recent[0]);
#ifdef HAVE_MATE
	GList *list = gtk_action_group_list_actions(applet->action_group);
	for(element = g_list_first(list); element; element = g_list_next(element)) {
		existing_action = element->data;
		if (!strcmp(gtk_action_get_name(existing_action), g_checksum_get_string(checksum))) {
			match = TRUE;
			break;
		}
	}

	if (!match) {
		action.name = g_checksum_get_string(checksum);
		action.label = argv[0];
		action.stock_id = NULL;
		action.accelerator = NULL;
		action.tooltip = NULL;
		action.callback = G_CALLBACK(play_menu_mate);
		gtk_action_group_add_actions(applet->action_group, &action, 1, applet);
	}

	sprintf(&applet->ui_recent[strl], "<menuitem action='%s' />", g_checksum_get_string(checksum));
#elif HAVE_GNOME_2
	BonoboUIVerb bnb = BONOBO_UI_UNSAFE_VERB_DATA (g_checksum_get_string(checksum), G_CALLBACK (play_menu_gnome), applet);
	applet->applet_menu_actions_gnome[applet->bonobo_counter] = bnb;

	sprintf(&applet->ui_recent[strl], "<menuitem name='Bonobo%u' verb='%s' label='%s' />", applet->bonobo_counter, g_checksum_get_string(checksum), argv[0]);
	applet->bonobo_counter++;
#endif

	for (i=0; i<10; i++) {
		if (strlen(&applet->hash_recent[i].hash[0]) == 0) {
			sprintf(&applet->hash_recent[i].hash[0], "%s", g_checksum_get_string(checksum));
			sprintf(&applet->hash_recent[i].url[0], "%s", argv[1]);
			sprintf(&applet->hash_recent[i].name[0], "%s", argv[0]);
			break;
		}
	}

	return 0;
}


int cb_sql_true(void *data, int argc, char **argv, char **azColName) {
	return 1;
}


int cb_sql_fav(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
	GtkTreeIter iter;

	gtk_list_store_append (applet->tree_store_favourites, &iter);
	gtk_list_store_set (applet->tree_store_favourites, &iter, FAVOURITES_COL_NAME, argv[0], FAVOURITES_COL_URL, argv[1], -1);

	return 0;
}


int cb_sql_fav_10(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
	int i;
	gboolean match = FALSE;
	GtkAction *existing_action;
	GtkActionEntry action;
	GList *element;

	GChecksum *checksum = g_checksum_new(G_CHECKSUM_MD5);
	g_checksum_update(checksum, (guchar *) argv[1], -1);

	int strl = strlen(&applet->ui_fav[0]);
#ifdef HAVE_MATE
	GList *list = gtk_action_group_list_actions(applet->action_group);
	for(element = g_list_first(list); element; element = g_list_next(element)) {
		existing_action = element->data;
		if (!strcmp(gtk_action_get_name(existing_action), g_checksum_get_string(checksum))) {
			match = TRUE;
			break;
		}
	}

	if (!match) {
		action.name = g_checksum_get_string(checksum);
		action.label = argv[0];
		action.accelerator = NULL;
                action.stock_id = NULL;
                action.tooltip = NULL;
		action.callback = G_CALLBACK(play_menu_mate);

		gtk_action_group_add_actions(applet->action_group, &action, 1, applet);
	}

	sprintf(&applet->ui_fav[strl], "<menuitem action='%s' />", g_checksum_get_string(checksum));
#elif HAVE_GNOME_2
	BonoboUIVerb bnb = BONOBO_UI_UNSAFE_VERB_DATA (g_checksum_get_string(checksum), G_CALLBACK (play_menu_gnome), applet);
	applet->applet_menu_actions_gnome[applet->bonobo_counter] = bnb;

	sprintf(&applet->ui_fav[strl], "<menuitem name='Bonobo%u' verb='%s' label='%s' />", applet->bonobo_counter, g_checksum_get_string(checksum), argv[0]);
	applet->bonobo_counter++;
#endif

	for (i=0; i<10; i++) {
		if (strlen(&applet->hash_fav[i].hash[0]) == 0) {
			sprintf(&applet->hash_fav[i].hash[0], "%s", g_checksum_get_string(checksum));
			sprintf(&applet->hash_fav[i].url[0], "%s", argv[1]);
			sprintf(&applet->hash_fav[i].name[0], "%s", argv[0]);
			break;
		}
	}
	return 0;
}


int cb_sql_icecast(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
	GtkTreeIter iter;

	gtk_list_store_append (applet->tree_store_icecast, &iter);
	gtk_list_store_set (applet->tree_store_icecast, &iter, ICECAST_COL_NAME, argv[0], ICECAST_COL_URL, argv[1], ICECAST_COL_GENRE, argv[2], -1);

	applet->icecast_total_entries ++;

	return 0;
}


int cb_sql_rbrowser(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
	GtkTreeIter iter;

	gtk_list_store_append (applet->tree_store_rbrowser, &iter);
	gtk_list_store_set (applet->tree_store_rbrowser, &iter, RBROWSER_COL_NAME, argv[0], RBROWSER_COL_URL, argv[1], RBROWSER_COL_GENRE, argv[2], -1);

	applet->rbrowser_total_entries ++;

	return 0;
}

int cb_sql_custom(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
	GtkTreeIter iter;

	gtk_list_store_append (applet->tree_store_custom, &iter);
	gtk_list_store_set (applet->tree_store_custom, &iter, CUSTOM_COL_NAME, argv[0], CUSTOM_COL_URL, argv[1], CUSTOM_COL_GENRE, argv[2], -1);

	applet->icecast_total_entries ++;

	return 0;
}


int cb_sql_version(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;

	applet->db_version = atoi(argv[0]);

	return 0;
}

