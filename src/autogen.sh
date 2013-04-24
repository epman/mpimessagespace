#!/bin/sh
#
# $Id: autogen.sh 884 2009-05-04 07:58:19Z epmancini $
#
aclocal && automake --gnu --add-missing && autoconf
