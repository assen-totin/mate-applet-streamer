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

#include <string.h>
#include <unistd.h>
#include <libintl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <wait.h>
#include <gst/gst.h>
#include <sqlite3.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef HAVE_MATE
	#include <mate-panel-applet.h>
#elif HAVE_GNOME_2
	#include <panel-applet.h>
#endif

#ifdef HAVE_LIBMATENOTIFY
	#include <libmatenotify/notify.h>
#elif HAVE_LIBNOTIFY
	#include <libnotify/notify.h>
#endif

#ifdef HAVE_MATE
	#define APPLET_FACTORY "StreamerAppletFactory"
	#define APPLET_ID "StreamerApplet"
#elif HAVE_GNOME_2
	#define APPLET_FACTORY "OAFIID:StreamerApplet_Factory"
	#define APPLET_ID "OAFIID:StreamerApplet"
#endif

#define _(String) gettext (String)
#define APPLET_NAME "streamer"
#define APPLET_ICON_PLAY "applet_streamer_play.png"
#define APPLET_ICON_PAUSE "applet_streamer_pause.png"
#define APPLET_VERSION "1"
#define APPLET_HOME_DIR ".streamer_applet"
#define APPLET_SQLITE_DB_FILENAME "streamer.sqlite"
#define APPLET_SQLITE_DB_VERSION "1"
#define ICECAST_URL_XML "http://dir.xiph.org/yp.xml"
#define ICECAST_TMP_FILE "icecast_dnld"
#define DEFAULT_NOTIFICATION_DURATION 5
// GSettings
#define APPLET_GSETTINGS_SCHEMA "org.mate.panel.applet.StreamerApplet"
#define APPLET_GSETTINGS_PATH "/org/mate/panel/objects/streamer/"
#define APPLET_KEY_OPTION_1 "show-notifications"
#define APPLET_KEY_OPTION_2 "duration-notifications"

enum {
	TAB_FAVOURITES = 0,
	TAB_ICECAST, 
	TAB_CUSTOM
};


enum {
	FAVOURITES_COL_URL = 0,
	FAVOURITES_COL_NAME,
	FAVOURITES_NUM_COLS
};

enum {
	ICECAST_COL_URL = 0,
	ICECAST_COL_NAME,
	ICECAST_COL_GENRE,
	ICECAST_NUM_COLS
};

enum {
	CUSTOM_COL_URL = 0,
	CUSTOM_COL_NAME,
	CUSTOM_COL_GENRE,
	CUSTOM_NUM_COLS
};

struct url_hash {
	char hash[64];
	char url[1024];
	char name[1024];
} url_hash;

typedef struct {
	int show_notifications;
	int duration_notifications;
} streamer_settings;

#ifdef HAVE_MATE
	typedef MatePanelApplet MyPanelApplet;
	typedef MatePanelAppletBackgroundType MyPanelAppletBackgroundType;
#elif HAVE_GNOME_2
	typedef PanelApplet MyPanelApplet;
	typedef PanelAppletBackgroundType MyPanelAppletBackgroundType;
#endif

typedef struct {
	MyPanelApplet *applet;
	GMainLoop *loop;
	GtkActionGroup *action_group;
	GtkWidget *image;
	GtkWidget *event_box;
	GtkWidget *quitDialog;
	GtkWidget *progress_icecast;
	GtkWidget *progress_custom;
	GtkWidget *text_icecast;
	GtkWidget *text_custom;
	GtkWidget *butt_search_icecast;
	GtkWidget *butt_search_custom;
	streamer_settings settings;
	int db_version;
	char url[1024];
	char name[1024];
	char title_checksum[1024];
	char xmlfile[1024];
	int status;
	time_t timestamp;
	sqlite3 *sqlite;
	GstElement *gstreamer_playbin2;
	GtkListStore *tree_store_favourites;
	GtkWidget *tree_view_favourites;
	GtkListStore *tree_store_icecast;
	GtkWidget *tree_view_icecast;
	GtkListStore *tree_store_custom;
	GtkWidget *tree_view_custom;
#ifdef HAVE_MATE
	GSettings *gsettings;
#endif
	char xml_listen_url[1024];
	char xml_server_name[1024];
	char xml_bitrate[1024];
	char xml_genre[1024];
	int xml_total_entries;
	int xml_curr_entries;
	int icecast_total_entries;
	int custom_total_entries;
	gdouble progress_ratio;
	char ui_fav[10240];
	char ui_recent[1024];
	struct url_hash hash_fav[10];
	struct url_hash hash_recent[10];
#ifdef HAVE_GNOME_2
	BonoboUIVerb applet_menu_actions_gnome[23];
	int bonobo_counter;
#endif
} streamer_applet;

// util.c
void push_notification (gchar *, gchar *, gchar *, int);
gboolean cp(const char *, const char *);
void debug(char *);

// gstreamer.c
void gstreamer_pause(streamer_applet *);
void gstreamer_play(streamer_applet *);
void gstreamer_init(streamer_applet *);

// sqlite.c
gboolean sqlite_connect(streamer_applet *);
gboolean sqlite_insert(streamer_applet *, char *);
gboolean sqlite_select(streamer_applet *, char *);
gboolean sqlite_delete(streamer_applet *t, char *);
int cb_sql_true(void *, int, char **, char **);
int cb_sql_recent(void *, int, char **, char **);
int cb_sql_recent_10(void *, int, char **, char **);
int cb_sql_fav(void *, int, char **, char **);
int cb_sql_fav_10(void *, int, char **, char **);
int cb_sql_icecast(void *, int, char **, char **);
int cb_sql_custom(void *, int, char **, char **);
int cb_sql_version(void *, int, char **, char **);

// icecast.c
gboolean icecast_dnld(streamer_applet *);
gboolean icecast_xml(streamer_applet *);
void icecast_print_elements(xmlNode *, streamer_applet *);
int icecast_count_elements(xmlNode *, int);
void icecast_save(streamer_applet *);
gboolean icecast_write(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, gpointer);

// custom.c
gboolean custom_xml(streamer_applet *);
void custom_print_elements(xmlNode *, streamer_applet *);
int custom_count_elements(xmlNode *, int);
void custom_save(streamer_applet *);
gboolean custom_write(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, gpointer);

//menu.c
void quitDialogClose(GtkWidget *, gpointer);
void menu_cb_all(GtkAction *, streamer_applet *);
void menu_cb_about(GtkAction *, streamer_applet *);
void create_view_and_model_favourites (streamer_applet *);
void create_view_and_model_icecast (streamer_applet *);
void create_view_and_model_custom (streamer_applet *);
void cell_edit_name(GtkCellRendererText *, gchar *, gchar *, gpointer);
void cell_edit_url(GtkCellRendererText *, gchar *, gchar *, gpointer);
void clear_store(streamer_applet *, int);
void row_down(GtkWidget *, gpointer);
void row_up(GtkWidget *, gpointer);
void row_del(GtkWidget *, gpointer);
void row_add(GtkWidget *, gpointer);
void row_play(GtkWidget *, gpointer);
void row_copy(GtkWidget *, gpointer);
void save_favourites(streamer_applet *);
gboolean write_favourites(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, gpointer);
void icecast_refresh(GtkWidget *, gpointer);
void custom_refresh(GtkWidget *, gpointer);
void custom_warning_import (GtkWidget *widget, gpointer window);
void search_station(GtkWidget *, gpointer);
void do_play(streamer_applet *);
gboolean on_left_click (GtkWidget *, GdkEventButton *, streamer_applet *);
void change_default_widget(GtkNotebook *, GtkWidget *, guint, gpointer);
#ifdef HAVE_MATE
void play_menu_mate (GtkAction *, streamer_applet *);
#elif HAVE_GNOME_2
void play_menu_gnome (BonoboUIComponent *, gpointer, char *);
#endif

// settings.c
void settings_notifications_toggle (GtkWidget *, gpointer);
void menu_cb_settings (GtkAction *, streamer_applet *);

// main.c
#ifdef HAVE_GTK2
void applet_back_change (MyPanelApplet *, MyPanelAppletBackgroundType, GdkColor *, GdkPixmap *, streamer_applet *);
#elif HAVE_GTK3
void applet_back_change (MyPanelApplet *, MyPanelAppletBackgroundType, GdkRGBA *, cairo_pattern_t *, streamer_applet *);
#endif
void applet_destroy(MyPanelApplet *, streamer_applet *);

// Menu skeleton - MATE version
#ifdef HAVE_MATE
static const GtkActionEntry applet_menu_actions_mate[] = {
        { "Favourites", GTK_STOCK_GO_FORWARD, N_("Favourites"), NULL, NULL, NULL },
        { "Recent", GTK_STOCK_GO_FORWARD, N_("Recent"), NULL, NULL, NULL },
        { "All", GTK_STOCK_EXECUTE, N_("All Stations"), NULL, NULL, G_CALLBACK (menu_cb_all) },
        { "Settings", GTK_STOCK_EXECUTE, N_("Settings"), NULL, NULL, G_CALLBACK (menu_cb_settings) },
        { "About", GTK_STOCK_ABOUT, N_("About"), NULL, NULL, G_CALLBACK (menu_cb_about) }
};
#endif

