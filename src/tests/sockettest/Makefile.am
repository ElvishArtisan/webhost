## automake.am
##
## Automake.am for webhost tests/sockettest
##
##   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" @QT4_CFLAGS@ @OPENSSL_CFLAGS@ -I$(top_srcdir)/src
LIBS = -L$(top_srcdir)/src/wh @OPENSSL_LIBS@
MOC = @MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@

noinst_PROGRAMS = sockettest_server

dist_sockettest_server_SOURCES = sockettest_server.cpp sockettest_server.h

nodist_sockettest_server_SOURCES = moc_sockettest_server.cpp

sockettest_server_LDADD = @QT4_LIBS@ -lwebhost

EXTRA_DIST = sockettest.html\
             sockettest.js

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
