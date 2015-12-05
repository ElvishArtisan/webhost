// webhostd.h
//
// webhostd(8) routing daemon
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WEBHOST_H
#define WEBHOST_H

#include <QObject>
#include <QStringList>
#include <QUdpSocket>

#include "config.h"

#define WEBHOSTD_USAGE "-d\n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 public slots:
  void readyReadData();

 private:
  void Ip(const QStringList &cmds);
  void Ntp(const QStringList &cmds);
  void Reboot(const QStringList &cmds);
  void Restart(const QStringList &cmds);
  void Upgrade(const QStringList &cmds);
  void ProcessCommand(const QString &cmd);
  QUdpSocket *main_command_socket;
  Config *main_config;
  bool main_debug;
};


#endif  // WEBHOSTD_H
