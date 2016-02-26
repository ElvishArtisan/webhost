## automake.am
##
## Automake.am for webhost tests
##
##   (C) Copyright 2015-2016 Fred Gleason <fredg@paravelsystems.com>
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

noinst_PROGRAMS = cgitest_script\
                  cgitest_server

dist_cgitest_script_SOURCES = cgitest_page.cpp cgitest_page.h\
                              cgitest_script.cpp cgitest_script.h
cgitest_script_LDADD = @QT4_LIBS@ -lwebhost

dist_cgitest_server_SOURCES = cgitest_server.cpp cgitest_server.h
nodist_cgitest_server_SOURCES = moc_cgitest_server.cpp
cgitest_server_LDADD = @QT4_LIBS@ -lwebhost

localstate_DATA = cgitest.html

EXTRA_DIST = cgitest.html

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
