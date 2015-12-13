#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Library General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
#  USA.
#
#  MATE Streamer applet written by Assen Totin <assen.totin@gmail.com>
#

HAVE_MATE=@HAVE_MATE@

APPLET_FLAGS=@APPLET_FLAGS@
APPLET_LIBS=@APPLET_LIBS@

AM_CPPFLAGS = @GTK_CFLAGS@ @APPLET_FLAGS@ @GSTREAMER_DASH_CFLAGS@
LDADD = @GTK_LIBS@ @APPLET_LIBS@ @GSTREAMER_DASH_LIBS@ -lsqlite3

libexec_PROGRAMS = streamer_applet

streamer_applet_SOURCES = applet.h util.c icecast.c custom.c gstreamer.c sqlite.c menu.c options.c main.c

appletdir = $(datadir)/mate-panel/applets
applet_DATA = org.mate.applets.StreamerApplet.mate-panel-applet

servicedir = $(datadir)/dbus-1/services
service_DATA = org.mate.panel.applet.StreamerApplet.service

serverdir = $(libdir)/bonobo/servers
server_DATA = StreamerAppletFactory.server

sqlitedir = @SQLITE_SKEL_DIR@
sqlite_DATA = streamer.sqlite

gsettingsdir = $(datadir)/glib-2.0/schemas
gsettings_DATA = org.mate.panel.applet.StreamerApplet.gschema.xml

EXTRA_DIST = streamer.sqlite

install-data-hook:
	if test $(HAVE_MATE) = "1" ; then glib-compile-schemas $(DESTDIR)$(datadir)/glib-2.0/schemas 2>/dev/null; fi

