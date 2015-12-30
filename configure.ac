AC_PREREQ([2.63])
AC_INIT([mate-applet-streamer], [0.3.5], [assen.totin@gmail.com])
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
if test "$libexecdir" != '${exec_prefix}/libexec'; then
        LIBEXECDIR=$libexecdir
fi
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
# For F20, change this to 0.18 and add --force to "autopoint" line in autogen.sh
AM_GNU_GETTEXT_VERSION(0.17)
AM_GNU_GETTEXT([external])
if test $USE_NLS != "yes" ; then
        echo "*** gettext not found. You're losing internationalization."
fi
AC_DEFINE_UNQUOTED([MATELOCALEDIR], ["$LOCALEDIR"], ["Locale Dir MATE"])

# Checks for programs
AC_PROG_CC
MAKEFILE_CC=`which gcc`
AC_SUBST([MAKEFILE_CC])

# Check for GTK and determine version
FORCE_GTK="no"
RPM_REQUIRE_GTK=" "
USE_GTK="0"
AC_ARG_ENABLE([gtk],[AS_HELP_STRING([--enable-gtk],[compile only for the specified GTK version, 2 or 3 (default: check both, use GTK-3, if not found, use GTK-2))])],[FORCE_GTK=$enableval])

PKG_CHECK_MODULES([PKG_GTK2], [gtk+-2.0], [FOUND_GTK2="1"], [FOUND_GTK2="0"])
PKG_CHECK_MODULES([PKG_GTK3], [gtk+-3.0], [FOUND_GTK3="1"], [FOUND_GTK3="0"])

if test $FORCE_GTK = "3" ; then
	if test $FOUND_GTK3 = "1" ; then
		AM_PATH_GTK_3_0([3.0.0], [USE_GTK="3"], [AC_MSG_ERROR(["*** GTK3 forced, but not found."])])
	else
		AC_MSG_ERROR(["*** GTK3 forced, but not found."])
	fi
elif test $FORCE_GTK = "2" ; then
	if test $FOUND_GTK2 = "1" ; then
		AM_PATH_GTK_2_0([2.24.0], [USE_GTK="2"], [AC_MSG_ERROR(["*** GTK2 forced, but not found."])])
	else
		AC_MSG_ERROR(["*** GTK2 forced, but not found."])
	fi
else
	if test $FOUND_GTK3 = "1" ; then
		AM_PATH_GTK_3_0([3.14.0], [USE_GTK="3"], [USE_GTK="0"])
	elif test $FOUND_GTK2 = "1" ; then
		AM_PATH_GTK_2_0([2.24.0], [USE_GTK="2"], [USE_GTK="0"])
	fi
fi

if test $USE_GTK = "3" ; then
	AC_DEFINE([HAVE_GTK3], [1], ["Define GTK3"])
	AC_DEFINE([GETTEXT_PACKAGE], ["gtk30"], ["GTK gettext"])
	AC_SUBST([GETTEXT_PACKAGE])
	RPM_REQUIRE_GTK="Requires: gtk3 >= 3.14.0"
	RPM_BUILDREQUIRE_GTK="Requires: gtk3 >= 3.14.0"
	SELECTED_GTK="GTK-3"
elif test $USE_GTK = "2" ; then
	AC_DEFINE([HAVE_GTK2], [1], ["Define GTK2"])
	AC_DEFINE([GETTEXT_PACKAGE], ["gtk20"], ["GTK gettext"])
	AC_SUBST([GETTEXT_PACKAGE])
	RPM_REQUIRE_GTK="Requires: gtk2 >= 2.24.0"
	RPM_BUILDREQUIRE_GTK="Requires: gtk2-devel >= 2.24.0"
	SELECTED_GTK="GTK-2"
else
	AC_MSG_ERROR(["*** GTK not found."])
fi

AC_SUBST([RPM_REQUIRE_GTK])
AC_SUBST([RPM_BUILDREQUIRE_GTK])

PKG_CHECK_MODULES([GLIB], [glib-2.0])

RPM_BUILDREQUIRES_PANEL=""
RPM_REQUIRES_PANEL=""
HAVE_MATE="0"
HAVE_GNOME_2="0"
PKG_CHECK_MODULES([LIBMATEPANELAPPLET_4], [libmatepanelapplet-4.0], [FOUND_LMPA_4="yes"], [FOUND_LMPA_4="no"])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET_3], [libmatepanelapplet-3.0], [FOUND_LMPA_3="yes"], [FOUND_LMPA_3="no"])
PKG_CHECK_MODULES([LIBPANELAPPLET_2], [libpanelapplet-2.0], [FOUND_LPA_2="yes"], [FOUND_LPA_2="no"])
if test $FOUND_LMPA_4 = "yes" ; then
        LIBMATEPANELAPPLET="libmatepanelapplet-4.0"
        HAVE_MATE="1"
        AC_DEFINE([HAVE_MATE], [1], [Define if we have MATE Desktop Environment])
		AC_SUBST([HAVE_MATE])
        RPM_BUILDREQUIRES_PANEL="BuildRequires: mate-panel-devel >= 1.3.0"
        RPM_REQUIRES_PANEL="Requires: mate-panel >= 1.3.0"
elif test $FOUND_LMPA_3 = "yes" ; then
        LIBMATEPANELAPPLET="libmatepanelapplet-3.0"
        HAVE_MATE="1"
        AC_DEFINE([HAVE_MATE], [1], [Define if we have MATE Desktop Environment])
		AC_SUBST([HAVE_MATE])
        RPM_BUILDREQUIRES_PANEL="BuildRequires: mate-panel-devel >= 1.3.0"
        RPM_REQUIRES_PANEL="Requires: mate-panel >= 1.3.0"
elif test $FOUND_LPA_2 = "yes" ; then
        LIBMATEPANELAPPLET="libpanelapplet-2.0"
        HAVE_GNOME_2="1"
        AC_DEFINE([HAVE_GNOME_2], [1], [Define if we have GNOME-2 Desktop Environment])
        AC_DEFINE_UNQUOTED([GNOMELOCALEDIR], ["$LOCALEDIR"], [GNOME-2 specific locale dir variable])
        RPM_BUILDREQUIRES_PANEL="BuildRequires: gnome-panel-devel >= 2.30.0"
        RPM_REQUIRES_PANEL="Requires: gnome-panel >= 2.30.0"
else
        AC_MSG_ERROR(["*** libmatepanelapplet not found."])
fi
AC_SUBST([RPM_REQUIRES_PANEL])
AC_SUBST([RPM_BUILDREQUIRES_PANEL])

# libmatenotify or libnotify?
RPM_BUILDREQUIRES_NOTIFY=""
RPM_REQUIRES_NOTIFY=""
FORCE_NOTIFY="no"
AC_ARG_ENABLE([notify],[AS_HELP_STRING([--enable-notify],[use the specified notification library: libnotify, libmatenotify (default: pick up the first available from the list)])],[FORCE_NOTIFY=$enableval])
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
		if test $HAVE_MATE = "1" ; then
	                RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.7.0"
        	        RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.7.0"
		elif test $HAVE_GNOME_2 = "1" ; then
			RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.5.0"
			RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.5.0"
		fi
        else
		AC_MSG_ERROR(["Forced notification library $FORCE_NOTIFY not found. Cannot continue."])
        fi
else
	if test $FOUND_LN = "yes" ; then
                SELECTED_NOTIFY="libnotify"
		AC_DEFINE([HAVE_LIBNOTIFY],[1],[Define if libnotify was found])
		if test $HAVE_MATE = "1" ; then
	                RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.7.0"
        	        RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.7.0"
		elif test $HAVE_GNOME_2 = "1" ; then
                        RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.5.0"
                        RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.5.0"
                fi
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
RPM_BUILDREQUIRE_GSTREAMER=" "
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
        fi
fi

if test $FOUND_GSTREAMER = "no" ; then
        AC_MSG_ERROR(["*** GStreamer not found."])
fi

if test $FOUND_GSTREAMER_PLUGINS_BASE = "no" ; then
        AC_MSG_ERROR(["*** GStreamer found, but GStreamer-plugins-base not found."])
fi

if test $FOUND_GSTREAMER = "0.10" ; then
	if test $FOUND_GSTREAMER_INTERFACES = "no" ; then
        	AC_MSG_ERROR(["*** GStreamer found, but GStreamer-interfaces not found."])
	fi
	RPM_REQUIRE_GSTREAMER="Requires: gstreamer-plugins-base >= 0.10"
	RPM_BUILDREQUIRE_GSTREAMER="BuildRequires: gstreamer-devel >= 0.10"
	# Automake is idiotic and treats --libs as linking argument, thus not allowing it in LDADD - and with Windows, linking order matters
	GSTREAMER_DASH_LIBS=`$PKG_CONFIG --libs gstreamer-0.10 gstreamer-interfaces-0.10`
	GSTREAMER_DASH_CFLAGS=`$PKG_CONFIG --cflags gstreamer-0.10 gstreamer-interfaces-0.10`
	AC_DEFINE([HAVE_GST_0_10], [1], [GStreamer 0.10])
elif test $FOUND_GSTREAMER = "1.0" ; then
	RPM_REQUIRE_GSTREAMER="Requires: gstreamer1-plugins-base >= 1.0"
	RPM_BUILDREQUIRE_GSTREAMER="BuildRequires: gstreamer1-devel >= 1.0"
	GSTREAMER_DASH_LIBS=`$PKG_CONFIG --libs gstreamer-1.0`
	GSTREAMER_DASH_CFLAGS=`$PKG_CONFIG --cflags gstreamer-1.0`
	AC_DEFINE([HAVE_GST_1_0], [1], [GStreamer 1.0])
fi
AC_SUBST([GSTREAMER_DASH_LIBS])
AC_SUBST([GSTREAMER_DASH_CFLAGS])
AC_SUBST([RPM_REQUIRE_GSTREAMER])
AC_SUBST([RPM_BUILDREQUIRE_GSTREAMER])

# Checks for libraries.
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0], [FOUND_LIBXML2="yes"], [FOUND_LIBXML2="no"])
if test $FOUND_LIBXML2 = "no" ; then
	AC_MSG_ERROR(["*** libxml2 not found."])
fi

PKG_CHECK_MODULES([SQLITE3], [sqlite3], [FOUND_SQLITE3="yes"], [FOUND_SQLITE3="no"])
if test $FOUND_SQLITE3 = "no" ; then
        AC_MSG_ERROR(["*** sqlite3 not found."])
fi

APPLET_FLAGS=`$PKG_CONFIG --cflags-only-I glib-2.0 libxml-2.0 $SELECTED_NOTIFY $LIBMATEPANELAPPLET`
APPLET_LIBS=`$PKG_CONFIG --libs glib-2.0 libxml-2.0 $SELECTED_NOTIFY $LIBMATEPANELAPPLET`
AC_SUBST([APPLET_FLAGS])
AC_SUBST([APPLET_LIBS])

# Makefiles
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in po/Makefile images/Makefile])

# Service files
AC_CONFIG_FILES([src/org.mate.applets.StreamerApplet.mate-panel-applet src/org.mate.panel.applet.StreamerApplet.service src/StreamerAppletFactory.server mate-applet-streamer.spec src/org.mate.panel.applet.StreamerApplet.gschema.xml])

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
echo " * Selected GTK version: $SELECTED_GTK"
echo " * Selected GStgreamer version: $FOUND_GSTREAMER"
echo " * Selected notification library: $SELECTED_NOTIFY"
echo
echo "Now, run make."
echo 
