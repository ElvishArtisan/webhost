// whnetwork.cpp
//
// Network information routines.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "whnetwork.h"

uint64_t WHInterfaceMacAddress(const QString &iface)
{
  struct ifreq ifr;
  int index=0;
  uint64_t mac;
  int sock;
  

  if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_IP))<0) {
    return 0;
  }
  memset(&ifr,0,sizeof(ifr));
  index=1;
  ifr.ifr_ifindex=index;
  while(ioctl(sock,SIOCGIFNAME,&ifr)==0) {
    if(ioctl(sock,SIOCGIFHWADDR,&ifr)==0) {
      mac=
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[0])<<40)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[1])<<32)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[2])<<24)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[3])<<16)+
	((0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[4])<<8)+
	(0xff&(uint64_t)ifr.ifr_ifru.ifru_hwaddr.sa_data[5]);
      if(iface==ifr.ifr_name) {
	close(sock);
	return mac;
      }
    }
    ifr.ifr_ifindex=++index;
  }
  close(sock);
  return 0;
}


QString WHMacAddressText(uint64_t mac)
{
  return QString().sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
			   0xFF&(unsigned)(mac>>40),
			   0xFF&(unsigned)(mac>>32),
			   0xFF&(unsigned)(mac>>24),
			   0xFF&(unsigned)(mac>>16),
			   0xFF&(unsigned)(mac>>8),
			   0xFF&(unsigned)mac);
}
