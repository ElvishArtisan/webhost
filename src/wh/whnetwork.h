// whnetwork.h
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

#ifndef WHNETWORK_H
#define WHNETWORK_H

#include <stdint.h>

#include <QHostAddress>
#include <QString>
#include <QStringList>

void WHInterfaceInfo(uint64_t *mac,QHostAddress *addr,QHostAddress *mask,
		     QHostAddress *bcst,const QString &iface);
QHostAddress WHInterfaceIPv4Address(const QString &iface);
QHostAddress WHInterfaceIPv4Netmask(const QString &iface);
QHostAddress WHInterfaceIPv4Broadcast(const QString &iface);
uint64_t WHInterfaceMacAddress(const QString &iface);
QString WHMacAddressText(uint64_t mac);


#endif  // WHNETWORK_H
