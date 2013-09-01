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
#include <mate-panel-applet.h>
#include <libintl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib.h>
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

#ifdef HAVE_LIBMATENOTIFY
	#include <libmatenotify/notify.h>
#elif HAVE_LIBNOTIFY
	#include <libnotify/notify.h>
#endif

#define _(String) gettext (String)
#define APPLET_FACTORY "StreamerAppletFactory"
#define APPLET_ID "StreamerApplet"
#define APPLET_NAME "streamer"
#define APPLET_ICON_PLAY "applet_streamer_play.png"
#define APPLET_ICON_PAUSE "applet_streamer_pause.png"
#define APPLET_VERSION "1"
#define APPLET_HOME_DIR ".streamer_applet"
#define APPLET_SQLITE_DB_FILENAME "streamer.sqlite"
#define APPLET_SQLITE_DB_VERSION "1"
#define ICECAST_URL_XML "http://dir.xiph.org/yp.xml"
#define ICECAST_TMP_FILE "icecast_dnld"

// Menu strings
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
"<menuitem name='MenuItem2' action='About' />"
;

enum {
        COL_URL = 0,
        COL_NAME,
        NUM_COLS
};

enum {
        COL_URL2 = 0,
        COL_NAME2,
	COL_GENRE2,
        NUM_COLS2
};

struct url_hash {
	char hash[64];
	char url[1024];
	char name[1024];
} url_hash;

typedef struct {
	GMainLoop *loop;
	MatePanelApplet *applet;
	GtkActionGroup *action_group;
        GtkWidget *image;
        GtkWidget *event_box;
	GtkWidget *quitDialog;
	GtkWidget *progress;
	GtkWidget *text;
	char url[1024];
	char name[1024];
	char xmlfile[1024];
	int status;
	time_t timestamp;
	sqlite3 *sqlite;
	GstElement *gstreamer_playbin2;
	GtkListStore *tree_store;
	GtkWidget *tree_view;
        GtkListStore *tree_store2;
        GtkWidget *tree_view2;
	char xml_listen_url[1024];
	char xml_server_name[1024];
	char xml_bitrate[1024];
	char xml_genre[1024];
	int xml_total_entries;
	int xml_curr_entries;
	int icecast_total_entries;
	gdouble progress_ratio;
	char ui_fav[10240];
	char ui_recent[1024];
	struct url_hash hash_fav[10];
	struct url_hash hash_recent[10];
} streamer_applet;

// util.c
void push_notification (gchar *, gchar *, gchar *);
gboolean cp(const char *, const char *);

// gstreamer.c
void gstreamer_pause(streamer_applet *);
void gstreamer_play(streamer_applet *);
void gstreamer_init(streamer_applet *);

// sqlite.c
gboolean sqlite_connect(streamer_applet *);
gboolean sqlite_insert(streamer_applet *, char *);
gboolean sqlite_select(streamer_applet *, char *);
int cb_sql_true(void *, int, char **, char **);
int cb_sql_recent(void *, int, char **, char **);
int cb_sql_recent_10(void *, int, char **, char **);
int cb_sql_fav(void *, int, char **, char **);
int cb_sql_fav_10(void *, int, char **, char **);
int cb_sql_icecast(void *, int, char **, char **);

// icecast.c
gboolean icecast_dnld(streamer_applet *);
gboolean icecast_xml(streamer_applet *);
void print_elements(xmlNode *, streamer_applet *);
int count_elements(xmlNode *, int);
void save_icecast(streamer_applet *);
gboolean write_icecast(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, gpointer);

//menu.c
void quitDialogClose(GtkWidget *, gpointer);
void menu_cb_all(GtkAction *, streamer_applet *);
void menu_cb_about(GtkAction *, streamer_applet *);
void create_view_and_model (streamer_applet *);
void create_view_and_model2 (streamer_applet *);
void cell_edit_name(GtkCellRendererText *, gchar *, gchar *, gpointer);
void cell_edit_url(GtkCellRendererText *, gchar *, gchar *, gpointer);
void clear_store(streamer_applet *);
void clear_store2(streamer_applet *);
void row_down(GtkWidget *, gpointer);
void row_up(GtkWidget *, gpointer);
void row_del(GtkWidget *, gpointer);
void row_add(GtkWidget *, gpointer);
void row_play(GtkWidget *, gpointer);
void row_copy(GtkWidget *, gpointer);
void save_favourites(streamer_applet *);
gboolean write_favourites(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, gpointer);
void icecast_refresh(GtkWidget *, gpointer);
void icecast_search(GtkWidget *, gpointer);
void play_menu (GtkAction *, streamer_applet *);
void do_play(streamer_applet *);
gboolean on_left_click (GtkWidget *, GdkEventButton *, streamer_applet *);

// main.c
void applet_back_change (MatePanelApplet *, MatePanelAppletBackgroundType, GdkColor *, GdkPixmap *, streamer_applet *);
void applet_destroy(MatePanelApplet *, streamer_applet *);

// Menu skeleton
static const GtkActionEntry applet_menu_actions[] = {
        { "Favourites", GTK_STOCK_GO_FORWARD, "_Favourites", NULL, NULL, NULL },
        { "Recent", GTK_STOCK_GO_FORWARD, "_Recent", NULL, NULL, NULL },
        { "All", GTK_STOCK_EXECUTE, "_All Stations", NULL, NULL, G_CALLBACK (menu_cb_all) },
        { "About", GTK_STOCK_ABOUT, "_About", NULL, NULL, G_CALLBACK (menu_cb_about) }
};
