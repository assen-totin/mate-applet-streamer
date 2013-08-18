# Author: Assen Totin <assen.totin@gmail.com>
#
# This software is released under GNU General Public License.
# See the LICENSE file for details or visit http://www.gnu.org/copyleft/gpl.html 
# for details.

APPLET_FLAGS=@APPLET_FLAGS@
APPLET_LIBS=@APPLET_LIBS@

AM_CPPFLAGS = @GTK_CFLAGS@ @APPLET_FLAGS@
LDADD = @GTK_LIBS@ @APPLET_LIBS@

libexec_PROGRAMS = softupd_applet

softupd_applet_SOURCES = applet.h main.c 

if WITH_YUM
softupd_applet_SOURCES += backend-yum.c 
endif

if WITH_YUMUPDATESD
softupd_applet_SOURCES += backend-yumupdatesd.c
endif

if WITH_APTCHECK
softupd_applet_SOURCES += backend-aptcheck.c 
endif

if WITH_PACKAGEKIT
softupd_applet_SOURCES += backend-packagekit.c
endif

appletdir = $(datadir)/mate-panel/applets
applet_DATA = org.mate.applets.SoftupdApplet.mate-panel-applet

servicedir = $(datadir)/dbus-1/services
service_DATA = org.mate.panel.applet.SoftupdApplet.service

