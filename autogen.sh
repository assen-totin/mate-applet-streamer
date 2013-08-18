find | grep \\.c$ | sed s/^..//g > po/POTFILES.in
autopoint
aclocal -I m4
autoheader
automake --add-missing
autoconf
