## automake.am
##
## Makefile for the webhostd(8) daemon
##
## (C) Copyright 2015-2022 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/src -Wno-strict-aliasing -std=c++11 -fPIC @QT5_CFLAGS@ @OPENSSL_CFLAGS@
LIBS = -L$(top_srcdir)/src/wh5
MOC = @QT5_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@QT5_MOC@ $< -o $@


sbin_PROGRAMS = webhostd

dist_webhostd_SOURCES = ip.cpp\
                        ntp.cpp\
                        reboot.cpp\
                        restart.cpp\
                        upgrade.cpp\
                        users.cpp\
                        webhostd.cpp webhostd.h\
                        wifi.cpp

nodist_webhostd_SOURCES = moc_webhostd.cpp

webhostd_LDADD = @QT5_LIBS@ -lwebhost5 @OPENSSL_LIBS@
webhostd_LDFLAGS = 

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

MAINTAINERCLEANFILES = *~\
                       Makefile.in
