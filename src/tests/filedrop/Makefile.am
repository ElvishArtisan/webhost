## automake.am
##
## Automake.am for webhost tests/filedrop
##
##   (C) Copyright 2015-2022 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -std=c++11 -fPIC @QT5_CFLAGS@ @OPENSSL_CFLAGS@ -I$(top_srcdir)/src
LIBS = -L$(top_srcdir)/src/wh5 @OPENSSL_LIBS@
MOC = @QT5_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@

noinst_PROGRAMS = filedrop.cgi

dist_filedrop_cgi_SOURCES = filedrop.cpp filedrop.h\
                            filepage.cpp filepage.h

filedrop_cgi_LDADD = @QT5_LIBS@ -lwebhost5

EXTRA_DIST = filedrop.html

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
