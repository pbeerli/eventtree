#!/bin/sh
# Run this to generate all the initial makefiles, etc.
aclocal
autoheader
automake -a
autoconf
./configure 
