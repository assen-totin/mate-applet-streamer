AC_PREREQ([2.66])
AC_INIT([mate-applet-streamer], [0.2.8], [assen.totin@gmail.com])
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
                EXIT_NOW="yes"
                EXIT_MSG="Forced notification library $FORCE_NOTIFY not found. Cannot continue."
        fi
elif test $FORCE_NOTIFY = "libnotify" ; then
        if test $FOUND_LN = "yes" ; then
                SELECTED_NOTIFY="libnotify"
		AC_DEFINE([HAVE_LIBNOTIFY],[1],[Define if libnotify was found])
                RPM_BUILDREQUIRES_NOTIFY="BuildRequires: libnotify-devel >= 0.7.0"
                RPM_REQUIRES_NOTIFY="Requires: libnotify >= 0.7.0"
        else
                EXIT_NOW="yes"
                EXIT_MSG="Forced notification library $FORCE_NOTIFY not found. Cannot continue."
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
		EXIT_NOW="yes"
		EXIT_MSG="Neither libnotify nor libmatenotify found. Cannot continue."
	fi
fi

AC_SUBST([RPM_BUILDREQUIRES_NOTIFY])
AC_SUBST([RPM_REQUIRES_NOTIFY])

if test $EXIT_NOW = "yes" ; then
        echo
        echo $EXIT_MSG
        echo
        exit
fi

# Checks for libraries.

# Check for packages
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([PKGKIT_BLIB], [packagekit-glib2])
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

# Grand finale
AC_OUTPUT

echo
echo "Configuration complete:"
echo " * Selected notification library: $SELECTED_NOTIFY"
echo
echo "Now, run make."
echo 
