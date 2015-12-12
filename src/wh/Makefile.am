## automake.am
##
## Makefile webhost library
##
## (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
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

AM_CPPFLAGS = -Wall -Wno-strict-aliasing -I$(top_srcdir) @QT4_CFLAGS@
MOC = @MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

clean-local:	
	rm -rf debug release

lib_LTLIBRARIES = libwebhost.la
dist_libwebhost_la_SOURCES = whcmdswitch.cpp whcmdswitch.h\
                             whprofile.cpp whprofile.h

#nodist_libwebhost_la_SOURCES = 

includedir = $(prefix)/include/wh

include_HEADERS = whcmdswitch.h\
                  whprofile.h

libwebhost_la_LDFLAGS = -version-info $(INTERFACE_CURRENT):$(INTERFACE_REVISION):$(INTERFACE_AGE)

EXTRA_DIST = sy.pro

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*
MAINTAINERCLEANFILES = *~\
                       Makefile.in
