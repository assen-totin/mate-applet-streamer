# Author: Assen Totin <assen.totin@gmail.com>
#
# This software is released under GNU General Public License.
# See the LICENSE file for details or visit http://www.gnu.org/copyleft/gpl.html 
# for details.

APPLET_FLAGS=@APPLET_FLAGS@
APPLET_LIBS=@APPLET_LIBS@

AM_CPPFLAGS = @GTK_CFLAGS@ @APPLET_FLAGS@
LDADD = @GTK_LIBS@ @APPLET_LIBS@

libexec_PROGRAMS = streamer_applet

streamer_applet_SOURCES = applet.h main.c 

appletdir = $(datadir)/mate-panel/applets
applet_DATA = org.mate.applets.StreamerApplet.mate-panel-applet

servicedir = $(datadir)/dbus-1/services
service_DATA = org.mate.panel.applet.StreamerApplet.service

