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

AC_CHECK_PROG([FOUND_MSGFMT],[msgfmt],["yes"],["no"])
if test $FOUND_MSGFMT = "no"; then
	echo "*** msgfmt not found. Cannot continue."
	exit
else
	MSGFMT=`which msgfmt`
	AC_SUBST([MSGFMT])
fi

# Check for backend tools
FOUND_BACKEND="no"
FORCE_BACKEND="no"
ADDITIONAL_PACKAGES=""
AC_ARG_ENABLE([backend],[AS_HELP_STRING([--enable-backend],[use the specified backend: package-kit, yum, yum-updatesd, apt-check (default: pick up the first available from the list)])],[FORCE_BACKEND=$enableval])

## Check for backend: packagekit
AC_CHECK_PROG([CHECK_PACKAGEKIT],[pkcon],[yes],[no])
if test $CHECK_PACKAGEKIT = "no" ; then
        echo "*** package-kit not found."
else
        FOUND_BACKEND="yes"
fi

## Check for backend: yum-updatesd
AC_CHECK_PROG([CHECK_YUMUPDATESD],[yum-updatesd],[yes],[no])
if test $CHECK_YUMUPDATESD = "no" ; then
	echo "*** yum-updatesd not found."
else
	FOUND_BACKEND="yes"
fi

## Check for backend: yum
AC_CHECK_PROG([CHECK_YUM],[yum],[yes],[no])
if test $CHECK_YUM = "no" ; then
        echo "*** yum not found."
else
        FOUND_BACKEND="yes"
        YUM_VIEWER=`which yum`
fi

## Check for backend: apt-check
AC_PATH_PROG([APTCHECK_PROG],[apt-check],[no],[/usr/lib/update-notifier:/var/lib/update-notifier:/usr/lib64/update-notifier:/var/lib64/update-notifier])
if test $APTCHECK_PROG = "no" ; then
	echo "*** apt-check not found."
else
	FOUND_BACKEND="yes"
fi

## Select which backend to use
SELECTED_BACKEND="none"
EXIT_NOW="no"
EXIT_MSG="none"
RPM_REQUIRES_BACKEND=""
RPM_BUILDREQUIRES_BACKEND=""
if test $FOUND_BACKEND = "no" ; then
	EXIT_NOW="yes"
	EXIT_MSG="*** No suitable backend found. Cannot continue."
elif test $FORCE_BACKEND = "package-kit" ; then
	if test $CHECK_PACKAGEKIT = "no" ; then
		EXIT_NOW="yes"
		EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
                RPM_REQUIRES_BACKEND="Requires: PackageKit"
                RPM_BUILDREQUIRES_BACKEND="BuildRequires: PackageKit-glib-devel, PackageKit"
	fi
elif test $FORCE_BACKEND = "yum-updatesd" ; then
        if test $CHECK_YUMUPDATESD = "no" ; then
                EXIT_NOW="yes"
                EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
		RPM_REQUIRES_BACKEND="Requires: yum-updatesd"
        fi
elif test $FORCE_BACKEND = "yum" ; then
        if test $CHECK_YUM = "no" ; then
                EXIT_NOW="yes"
                EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
		RPM_REQUIRES_BACKEND="Requires: yum"
        fi
elif test $FORCE_BACKEND = "apt-check" ; then
        if test $APTCHECK_PROG = "no" ; then
                EXIT_NOW="yes"
                EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
        fi
else
	echo "Chosing backend wisely..."
	if test $CHECK_PACKAGEKIT != "no" ; then
		SELECTED_BACKEND="package-kit"
		AC_DEFINE([HAVE_PACKAGEKIT],[1],[Define if package-kit was found])
		ADDITIONAL_PACKAGES="packagekit-glib2"
		RPM_REQUIRES_BACKEND="Requires: PackageKit"
		RPM_BUILDREQUIRES_BACKEND="BuildRequires: PackageKit-glib-devel, PackageKit"
	elif test $CHECK_YUMUPDATESD != "no" ; then
		SELECTED_BACKEND="yum-updatesd"
	        AC_DEFINE([HAVE_YUMUPDATESD],[1],[Define if yum-updatesd was found])
	        ADDITIONAL_PACKAGES="dbus-1 dbus-glib-1"
		RPM_REQUIRES_BACKEND="Requires: yum-updatesd"
	elif test $CHECK_YUM != "no" ; then
		SELECTED_BACKEND="yum"
	        AC_DEFINE([HAVE_YUM],[1],[Define if yum was found])
        	AC_DEFINE_UNQUOTED([YUM_BINARY],["$YUM_VIEWER"],[Define where is the yum binary])
		RPM_REQUIRES_BACKEND="Requires: yum"
	elif test $APTCHECK_PROG != "no" ; then
		SELECTED_BACKEND="apt-check"
	        AC_DEFINE([HAVE_APTCHECK],[1],[Define if apt-check was found])
        	AC_DEFINE_UNQUOTED([APTCHECK_BINARY],["$APTCHECK_PROG"],[Define where is the apt-check binary])
	fi
fi

AM_CONDITIONAL([WITH_PACKAGEKIT], [test $SELECTED_BACKEND = "package-kit"])
AM_CONDITIONAL([WITH_YUMUPDATESD], [test $SELECTED_BACKEND = "yum-updatesd"])
AM_CONDITIONAL([WITH_YUM], [test $SELECTED_BACKEND = "yum"])
AM_CONDITIONAL([WITH_APTCHECK], [test $SELECTED_BACKEND = "apt-check"])
AC_SUBST([RPM_REQUIRES_BACKEND])
AC_SUBST([RPM_BUILDREQUIRES_BACKEND])

# Check which software installer to use
FOUND_INSTALLER="no"
FORCE_INSTALLER="no"
AC_ARG_ENABLE([installer],[AS_HELP_STRING([--enable-installer],[use the specified backend: yumex, gpk-update-viewer, update-manager (default: pick up the first available from the list)])],[FORCE_INSTALLER=$enableval])

# Check for yumex
AC_CHECK_PROG([CHECK_YUMEXVIEWER],[yumex],[yes],[no])
if test $CHECK_YUMEXVIEWER = "no" ; then
        echo "*** yumex not found."
else
        FOUND_INSTALLER="yes"
        YUMEXVIEWER_INSTALLER=`which yumex`
fi

# Check for gpk-update-viewer
AC_CHECK_PROG([CHECK_GPKUPDATEVIEWER],[gpk-update-viewer],[yes],[no])
if test $CHECK_GPKUPDATEVIEWER = "no" ; then
        echo "*** gpk-update-viewer not found."
else
        FOUND_INSTALLER="yes"
        GPKUPDATEVIEWER_INSTALLER=`which gpk-update-viewer`
fi

# Check for update-manager
AC_CHECK_PROG([CHECK_UPDATEMANAGER],[update-manager],[yes],[no])
if test $CHECK_UPDATEMANAGER = "no" ; then
        echo "*** update-manager not found."
else
        FOUND_INSTALLER="yes"
        UPDATEMANAGER_INSTALLER=`which update-manager`
fi

# Select which software installer to use
if test $FORCE_INSTALLER = "yumex" ; then
	if test $CHECK_YUMEXVIEWER = "yes" ; then
		SELECTED_INSTALLER="yumex"
	else 
                EXIT_NOW="yes"
                EXIT_MSG="Forced installer $FORCE_INSTALLER not found. Cannot continue."	
	fi
elif test $FORCE_INSTALLER = "gpk-update-viewer" ; then
        if test $CHECK_GPKUPDATEVIEWER = "yes" ; then
                SELECTED_INSTALLER="gpk-update-viewer"
        else
                EXIT_NOW="yes"
                EXIT_MSG="Forced installer $FORCE_INSTALLER not found. Cannot continue." 
	fi
elif test $FORCE_INSTALLER = "update-manager" ; then
        if test $CHECK_UPDATEMANAGER = "yes" ; then
                SELECTED_INSTALLER="update-manager"
        else
                EXIT_NOW="yes"
                EXIT_MSG="Forced installer $FORCE_INSTALLER not found. Cannot continue." 
        fi
else
	echo "Chosing installer wisely..."
	if test $CHECK_YUMEXVIEWER = "yes" ; then
        	SELECTED_INSTALLER="yumex"
	elif test $CHECK_GPKUPDATEVIEWER = "yes" ; then
        	SELECTED_INSTALLER="gpk-update-viewer"
	elif test $CHECK_UPDATEMANAGER = "yes" ; then
        	SELECTED_INSTALLER="update-manager"
	else
		SELECTED_INSTALLER="none"
	fi
fi

RPM_REQUIRES_INSTALLER=""
RPM_BUILDREQUIRES_INSTALLER=""
if test $SELECTED_INSTALLER = "yumex" ; then
        AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$YUMEXVIEWER_INSTALLER"],[Define where is the installer binary])
	RPM_REQUIRES_INSTALLER="Requires: yumex"
	RPM_BUILDREQUIRES_INSTALLER="BuildRequires:yumex"
elif test $SELECTED_INSTALLER = "gpk-update-viewer" ; then
        AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$GPKUPDATEVIEWER_INSTALLER"],[Define where is the installer binary])
	RPM_REQUIRES_INSTALLER="Requires: gnome-packagekit"
elif test $SELECTED_INSTALLER = "update-manager" ; then
        AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$UPDATEMANAGER_INSTALLER"],[Define where is the installer binary])
else
	echo "*** No suitable installer found. You will have to update the system manually."
fi
AC_SUBST([RPM_REQUIRES_INSTALLER])
AC_SUBST([RPM_BUILDREQUIRES_INSTALLER])

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
AC_CONFIG_FILES([src/org.mate.applets.SoftupdApplet.mate-panel-applet src/org.mate.panel.applet.SoftupdApplet.service mate-applet-streamer.spec])

# Icons
AC_DEFINE_UNQUOTED([APPLET_ICON_PATH], ["$PIXMAPDIR"], [Define icons location])

# Grand finale
AC_OUTPUT

echo
echo "Configuration complete:"
echo " * Selected backend: $SELECTED_BACKEND"
echo " * Selected installer: $SELECTED_INSTALLER"
echo " * Selected notification library: $SELECTED_NOTIFY"
echo
echo "Now, run make."
echo 
