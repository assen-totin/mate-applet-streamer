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
 *  MATE Software Update applet written by Assen Totin <assen.totin@gmail.com>
 *  
 */

#include <string.h>
#include <unistd.h>
#include <mate-panel-applet.h>
#include <libintl.h>
#include <sys/stat.h>
#include <glib.h>
#include <stdlib.h>
#include <wait.h>

#ifdef HAVE_LIBMATENOTIFY
	#include <libmatenotify/notify.h>
#elif HAVE_LIBNOTIFY
	#include <libnotify/notify.h>
#endif

#define APPLET_FACTORY "StreamerAppletFactory"
#define APPLET_ID "StreamerApplet"
#define APPLET_NAME "streamer"
#define APPLET_ICON_PLAY "applet_streamer_play.png"
#define APPLET_ICON_PAUSE "applet_streamer_pause.png"
#define APPLET_VERSION "1"

typedef struct {
	GMainLoop *loop;
	MatePanelApplet *applet;
        GtkWidget *image;
        GtkWidget *event_box;
	GtkWidget *quitDialog;
	char url[1024];
	char name[1024];
	int status;
	int unsigned timestamp;
} streamer_applet;

// Prototypes
//gboolean packagekit_main();
//int yumupdatesd_main();
//void yum_main();
//void aptcheck_main();
//void aptget_main();
