# This file is a part of mate-applet-streamer
#  
# Author: Assen Totin <assen.totin@gmail.com>
#
# This software is released under GNU General Public License.
# See the COPYING file for details or visit http://www.gnu.org/copyleft/gpl.html 
# for details.

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = applet_streamer_play.png applet_streamer_pause.png 
EXTRA_DIST = $(pixmap_DATA)

icons = applet_streamer.16.png applet_streamer.24.png applet_streamer.22.png applet_streamer.32.png
EXTRA_DIST += $(icons)

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons

install-data-local:
	for icon in $(icons); do \
		SIZE=`echo $$icon | cut -d. -f2`; \
		FILE=`echo $$icon | cut -d. -f1,3`; \
		if [ "$$SIZE" = "svg" ]; then \
			SIZE="scalable"; \
			FILE="$$FILE.svg"; \
		else \
			SIZE="$${SIZE}x$${SIZE}"; \
		fi; \
		mkdir -p $(DESTDIR)$(datadir)/icons/hicolor/$$SIZE/apps/; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(datadir)/icons/hicolor/$$SIZE/apps/$$FILE; \
	done
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated. After install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

