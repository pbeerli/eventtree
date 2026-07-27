#!/bin/sh
# Run this to generate all the initial makefiles, etc.

# wxwin.m4, which defines AM_PATH_WXCONFIG, ships with wxWidgets and is
# not always on aclocal's default search path: a Homebrew aclocal in
# /opt/homebrew does not look in /usr/local/share/aclocal, where a
# /usr/local wxWidgets installs it.  Find it and point aclocal at it.
ACLOCAL_FLAGS=""
for dir in /usr/local/share/aclocal /opt/homebrew/share/aclocal \
           /opt/local/share/aclocal /usr/share/aclocal ; do
	if test -f "$dir/wxwin.m4" ; then
		ACLOCAL_FLAGS="-I $dir"
		break
	fi
done
if test -z "$ACLOCAL_FLAGS" ; then
	echo "autogen.sh: warning: wxwin.m4 not found, configure will fail" >&2
	echo "            with an undefined AM_PATH_WXCONFIG macro." >&2
fi

set -e
aclocal $ACLOCAL_FLAGS
autoheader
automake -a
autoconf
./configure "$@"
