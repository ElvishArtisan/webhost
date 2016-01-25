## automake.am
##
## Top level automake.am
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

SUBDIRS = conf\
          docs\
          src

install-exec-local:	
	mkdir -p $(DESTDIR)/usr/lib/systemd/system
	cp webhost.service $(DESTDIR)/usr/lib/systemd/system/webhost.service
	mkdir -p $(DESTDIR)@libdir@/pkgconfig
	cp webhost.pc $(DESTDIR)/@libdir@/pkgconfig
#	mkdir -p $(DESTDIR)/etc/init.d
#	cp rc.webhost $(DESTDIR)/etc/init.d/webhost

uninstall-local:	
	rm -f $(DESTDIR)/usr/lib/systemd/system/webhost.service
	rm -f $(DESTDIR)@libdir@/pkgconfig/webhost.pc
#	rm -f $(DESTDIR)/etc/init.d/webhost

rpm:	dist
	cp webhost-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux webhost.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/webhost-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/webhost-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/webhost-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/webhost-$(VERSION)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = webhost.pc

EXTRA_DIST = webhost.spec.in\
             autogen.sh\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION\
             rc.webhost.in\
             webhost.pc.in\
             webhost.service.in

CLEANFILES = *~\
             *.deb\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tgz\
             *.zip

DISTCLEANFILES = webhost.spec\
                 config.guess\
                 config.sub\
                 ltmain.sh\
                 webhost.pc\
                 webhost.service

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       config.guess\
                       config.status\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       libtool\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
