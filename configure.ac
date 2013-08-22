AC_PREREQ([2.66])
AC_INIT([mate-applet-streamer], [0.0.1], [assen.totin@gmail.com])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall])

# prefix
if test "x$prefix" = "xNONE" ; then
	prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE" ; then
	exec_prefix=$prefix
fi
PREFIX=$prefix
AC_SUBST([PREFIX])
BINDIR=$exec_prefix/bin
AC_SUBST([BINDIR])
SBINDIR=$exec_prefix/sbin
AC_SUBST([SBINDIR])
DATAROOTDIR=$prefix/share
AC_SUBST([DATAROOTDIR])
LIBEXECDIR=$prefix/libexec
AC_SUBST([LIBEXECDIR])
PIXMAPDIR=$prefix/share/pixmaps
AC_SUBST([PIXMAPDIR])
ICONSDIR=$prefix/share/icons
AC_SUBST([ICONSDIR])
LOCALEDIR=$prefix/share/locale
AC_SUBST([LOCALEDIR])
AC_DEFINE_UNQUOTED([LOCALEDIR],["$LOCALEDIR"],[Locale files])
TOPLEVELDIR=`pwd`
AC_SUBST([TOPLEVELDIR])

# gettext
AM_GNU_GETTEXT_VERSION(0.18.1)
AM_GNU_GETTEXT([external])
if test $USE_NLS != "yes" ; then
        echo "*** gettext not found. You're losing internationalization."
fi
AC_DEFINE([GETTEXT_PACKAGE], ["gtk20"], ["GTK gettext"])
AC_DEFINE_UNQUOTED([MATELOCALEDIR], ["$LOCALEDIR"], ["Locale Dir MATE"])

# Checks for programs
AC_PROG_CC
MAKEFILE_CC=`which gcc`
AC_SUBST([MAKEFILE_CC])

# libmatenotify or libnotify?
RPM_BUILDREQUIRES_NOTIFY=""
RPM_REQUIRES_NOTIFY=""
FORCE_NOTIFY="no"
AC_ARG_ENABLE([notify],[AS_HELP_STRING([--enable-notify],[use the specified notificatin library: libnotify, libmatenotify (default: pick up the first available from the list)])],[FORCE_NOTIFY=$enableval])
PKG_CHECK_MODULES([LIBMATENOTIFY], [libmatenotify], [FOUND_LMN="yes"], [FOUND_LMN="no"])
PKG_CHECK_MODULES([LIBNOTIFY], [libnotify], [FOUND_LN="yes"], [FOUND_LN="no"])
if test $FORCE_NOTIFY = "libmatenotify" ; then
        if test $FOUND_LMN = "yes" ; then
                SELECTED_NOTIFY="libmatenotify"
		AC_DEFINE([HAVE_LIBMATENOTIFY],[1],[Define if libmatenotify was found])
		RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libmatenotify-devel >= 1.2.0"
		RPM_REQUIRES_NOTIFY="Requires: libmatenotify >= 1.2.0"
        else
                AC_MSG_ERROR(["Forced notification library $FORCE_NOTIFY not found. Cannot continue."])
        fi
elif test $FORCE_NOTIFY = "libnotify" ; then
        if test $FOUND_LN = "yes" ; then
                SELECTED_NOTIFY="libnotify"
		AC_DEFINE([HAVE_LIBNOTIFY],[1],[Define if libnotify was found])
                RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.7.0"
                RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.7.0"
        else
		AC_MSG_ERROR(["Forced notification library $FORCE_NOTIFY not found. Cannot continue."])
        fi
else
	if test $FOUND_LN = "yes" ; then
                SELECTED_NOTIFY="libnotify"
		AC_DEFINE([HAVE_LIBNOTIFY],[1],[Define if libnotify was found])	
                RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.7.0"
                RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.7.0"
	elif test $FOUND_LMN = "yes" ; then
		SELECTED_NOTIFY="libmatenotify"
		AC_DEFINE([HAVE_LIBMATENOTIFY],[1],[Define if libmatenotify was found])
                RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libmatenotify-devel >= 1.2.0"
                RPM_REQUIRES_NOTIFY="Requires: libmatenotify >= 1.2.0"
	else
		AC_MSG_ERROR(["Neither libnotify nor libmatenotify found. Cannot continue."])
	fi
fi

AC_SUBST([RPM_BUILDREQUIRES_NOTIFY])
AC_SUBST([RPM_REQUIRES_NOTIFY])

# Check for GStreamer
ECHO_BACKEND_GSTREAMER="no"
FORCE_GSTREAMER="undef"
TEST_GST_0_10="no"
TEST_GST_1_0="no"
HAVE_GSTREAMER="0"
FOUND_GSTREAMER="no"
FOUND_GSTREAMER_INTERFACES="no"
FOUND_GSTREAMER_PLUGINS_BASE="no"
FOUND_GSTREAMER_VIDEO="no"
USE_GSTREAMER="no"
RPM_REQUIRE_GSTREAMER=" "
AC_ARG_ENABLE([gstreamer],[AS_HELP_STRING([--enable-gstreamer],[compile GStreamer version (default: try 1.0, if missing, try 0.10; to force, set to "1.0" or "0.10")])],[FORCE_GSTREAMER=$enableval])

if test $FORCE_GSTREAMER = "1.0" ; then
        TEST_GST_1_0="yes"
elif test $FORCE_GSTREAMER = "0.10" ; then
        TEST_GST_0_10="yes"
elif test $FORCE_GSTREAMER = "undef" ; then
        TEST_GST_0_10="yes";
        TEST_GST_1_0="yes"
fi

if test $TEST_GST_0_10 = "yes" ; then
        PKG_CHECK_MODULES([GSTREAMER_0_10], [gstreamer-0.10], [FOUND_GSTREAMER="0.10"], [FOUND_GSTREAMER="no"])
        if test $FOUND_GSTREAMER = "0.10" ; then
                PKG_CHECK_MODULES([GSTREAMER_INTERFACES_0_10], [gstreamer-interfaces-0.10], [FOUND_GSTREAMER_INTERFACES="yes"], [FOUND_GSTREAMER_INTERFACES="no"])
                PKG_CHECK_MODULES([GSTREAMER_PLUGINS_BASE_0_10], [gstreamer-plugins-base-0.10], [FOUND_GSTREAMER_PLUGINS_BASE="yes"], [FOUND_GSTREAMER_PLUGINS_BASE="no"])
        fi
fi
if test $TEST_GST_1_0 = "yes" ; then
        PKG_CHECK_MODULES([GSTREAMER_1_0], [gstreamer-1.0], [FOUND_GSTREAMER="1.0"],[EMPTY=""])
        if test $FOUND_GSTREAMER = "1.0" ; then
                PKG_CHECK_MODULES([GSTREAMER_PLUGINS_BASE_1_0], [gstreamer-plugins-base-1.0], [FOUND_GSTREAMER_PLUGINS_BASE="yes"],[FOUND_GSTREAMER_PLUGINS_BASE="no"])
                PKG_CHECK_MODULES([GSTREAMER_VIDEO_1_0], [gstreamer-video-1.0], [FOUND_GSTREAMER_VIDEO="yes"],[FOUND_GSTREAMER_VIDEO="no"])
        fi
fi

if test $FOUND_GSTREAMER = "no" ; then
        AC_MSG_ERROR(["*** GStreamer not found."])
fi

if test $FOUND_GSTREAMER_PLUGINS_BASE = "no" ; then
        AC_MSG_ERROR(["*** GStreamer found, but GStreamer-plugins-base not found."])
elif test $FOUND_GSTREAMER_INTERFACES = "no" ; then
        AC_MSG_ERROR(["*** GStreamer found, but GStreamer-interfaces not found."])
fi

if test $FOUND_GSTREAMER = "0.10" ; then
        RPM_REQUIRE_GSTREAMER="Requires: gstreamer-plugins-base >= 0.10"
        # Automake is idiotic and treats --libs as linking argument, thus not allowing it in LDADD - and with Windows, linking order matters
        GSTREAMER_DASH_LIBS=`$PKG_CONFIG --libs gstreamer-0.10 gstreamer-interfaces-0.10`
        GSTREAMER_DASH_CFLAGS=`$PKG_CONFIG --cflags gstreamer-0.10 gstreamer-interfaces-0.10`
        AC_DEFINE([HAVE_GST_0_10], [1], [GStreamer 0.10])
elif test  $FOUND_GSTREAMER = "1.0" ; then
        RPM_REQUIRE_GSTREAMER="Requires: gstreamer1-plugins-base >= 1.0"
        GSTREAMER_DASH_LIBS=`$PKG_CONFIG --libs gstreamer-1.0 gstreamer-video-1.0`
        GSTREAMER_DASH_CFLAGS=`$PKG_CONFIG --cflags gstreamer-1.0 gstreamer-video-1.0`
        AC_DEFINE([HAVE_GST_1_0], [1], [GStreamer 1.0])
fi
AC_SUBST([GSTREAMER_DASH_LIBS])
AC_SUBST([GSTREAMER_DASH_CFLAGS])
AC_SUBST([RPM_REQUIRE_GSTREAMER])

# Checks for libraries.

# Check for packages
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([PKGKIT_GLIB], [packagekit-glib2])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET_4], [libmatepanelapplet-4.0], [FOUND_LMPA_4="yes"], [FOUND_LMPA_4="no"])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET_3], [libmatepanelapplet-3.0], [FOUND_LMPA_3="yes"], [FOUND_LMPA_3="no"])
if test $FOUND_LMPA_4 = "yes" ; then
	LIBMATEPANELAPPLET="libmatepanelapplet-4.0"
elif test $FOUND_LMPA_3 = "yes" ; then
	LIBMATEPANELAPPLET="libmatepanelapplet-3.0"
else
	AC_MSG_ERROR(["*** libmatepanelapplet not found."])
fi

APPLET_FLAGS=`$PKG_CONFIG --cflags-only-I glib-2.0 $SELECTED_NOTIFY $LIBMATEPANELAPPLET $ADDITIONAL_PACKAGES`
APPLET_LIBS=`$PKG_CONFIG --libs glib-2.0 $SELECTED_NOTIFY $LIBMATEPANELAPPLET $ADDITIONAL_PACKAGES`
AC_SUBST([APPLET_FLAGS])
AC_SUBST([APPLET_LIBS])

# Makefiles
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in po/Makefile images/Makefile])

# Service files
AC_CONFIG_FILES([src/org.mate.applets.StreamerApplet.mate-panel-applet src/org.mate.panel.applet.StreamerApplet.service mate-applet-streamer.spec])

# Icons
AC_DEFINE_UNQUOTED([APPLET_ICON_PATH], ["$PIXMAPDIR"], [Define icons location])

# SQLite skeleton DB
SQLITE_SKEL_DIR=$DATAROOTDIR/streamer_applet
AC_DEFINE_UNQUOTED([APPLET_SKEL_PATH], ["$SQLITE_SKEL_DIR"], [Define SQLite skeleton DB location])
AC_SUBST([SQLITE_SKEL_DIR])

# Grand finale
AC_OUTPUT

echo
echo "Configuration complete:"
echo " * Selected notification library: $SELECTED_NOTIFY"
echo
echo "Now, run make."
echo 
