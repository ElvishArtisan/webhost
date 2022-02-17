// sockettest_server.h
//
// Test server for the webhost WebSocket classes
//
//   (C) Copyright 2016-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SOCKETTEST_SERVER_H
#define SOCKETTEST_SERVER_H

#include <QObject>

#include <wh5/whhttpserver.h>

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void newSocketConnectionData(int id,const QString &uri,const QString &proto);
  void socketMessageReceivedData(int id,WHSocketMessage *msg);
  void socketConnectionClosedData(int id,uint16_t status,const QByteArray &body);

 private:
  WHHttpServer *test_server;
  WHHttpConnection *test_connection;
};


#endif  // SOCKETTEST_SERVER_H
