# This file is a part of mate-applet-softupd
# 
# Author: Assen Totin <assen.totin@gmail.com>
#
# This software is released under GNU General Public License.
# See the COPYING file for details or visit http://www.gnu.org/copyleft/gpl.html 
# for details.

SUBDIRS = src images po

dist_doc_DATA = AUTHORS BUGS ChangeLog COPYING INSTALL NEWS README TODO

EXTRA_DIST = mate-applet-softupd.spec

rpm:
	rpmbuild -bb --define "_builddir @TOPLEVELDIR@" mate-applet-softupd.spec

