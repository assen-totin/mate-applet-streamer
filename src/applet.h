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

typedef struct {
	GMainLoop *loop;
	MatePanelApplet *applet;
        GtkWidget *image;
        GtkWidget *event_box;
	GtkWidget *quitDialog;
	GtkWidget *progress;
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
	gdouble progress_ratio;
} streamer_applet;

void menu_cb_favourites(GtkAction *, streamer_applet *);
void menu_cb_recent(GtkAction *, streamer_applet *);
void menu_cb_all(GtkAction *, streamer_applet *);
void menu_cb_about(GtkAction *, streamer_applet *);

gboolean sqlite_connect(streamer_applet *);
gboolean sqlite_insert(streamer_applet *, char *);
gboolean sqlite_delete(streamer_applet *, char *);
gboolean sqlite_select(streamer_applet *, char *);

/*
static const GtkActionEntry applet_menu_actions [] = {
        { "Favourites", GTK_STOCK_PROPERTIES, "_Favourites", NULL, NULL, G_CALLBACK (menu_cb_favourites) },
	{ "Recent", GTK_STOCK_PROPERTIES, "_Recent", NULL, NULL, G_CALLBACK (menu_cb_recent) },
	{ "All", GTK_STOCK_PROPERTIES, "_All", NULL, NULL, G_CALLBACK (menu_cb_all) },
        { "About", GTK_STOCK_ABOUT, NULL, "_About", NULL, G_CALLBACK (menu_cb_about) }
};
*/

// Prototypes
//gboolean packagekit_main();
//int yumupdatesd_main();
//void yum_main();
//void aptcheck_main();
//void aptget_main();

void gstreamer_pause(streamer_applet *);
void gstreamer_play(streamer_applet *);
void gstreamer_init(streamer_applet *);

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
gboolean write_favourites(GtkTreeModel *, GtkTreePath *, GtkTreeIter *, gpointer);
int cb_sql_fav(void *, int, char **, char **);
void icecast_refresh(GtkWidget *, gpointer);

gboolean icecast_dnld(streamer_applet *);
gboolean icecast_xml(streamer_applet *);
void print_element_names(xmlNode *, streamer_applet *);
int count_elements(xmlNode * a_node, int counter);
int progress_update(); 
