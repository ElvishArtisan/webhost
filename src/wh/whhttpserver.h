// whhttpserver.h
//
// HTTP Server
//
// (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WHHTTPSERVER_H
#define WHHTTPSERVER_H

#include <stdint.h>

#include <vector>

#include <QByteArray>
#include <QObject>
#include <QSignalMapper>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include <wh/whhttprequest.h>

class __WHHttpConnection
{
 public:
  __WHHttpConnection(QTcpSocket *sock);
  ~__WHHttpConnection();
  QTcpSocket *socket();
  WHHttpRequest *request();
  QString accum;

 private:
  QTcpSocket *conn_socket;
  WHHttpRequest *conn_request;
};


class WHHttpServer : public QObject
{
  Q_OBJECT;
 public:
  WHHttpServer(QObject *parent=0);
  ~WHHttpServer();
  bool listen(uint16_t port);
  bool listen(const QHostAddress &iface,uint16_t port);
  void addStaticSource(const QString &uri,const QString &mimetype,
		       const QString &filename);
  void sendResponse(int id,int stat_code,
		    const QStringList &hdr_names,const QStringList &hdr_values,
		    const QByteArray &body=QByteArray(),
		    const QString &mimetype="");
  void sendResponse(int id,int stat_code,
		    const QByteArray &body=QByteArray(),
		    const QString &mimetype="");
  void sendError(int id,int stat_code,const QString &msg="",
		 const QStringList &hdr_names=QStringList(),
		 const QStringList &hdr_values=QStringList());

 signals:
  void requestReceived(int id,WHHttpRequest *req);

 private slots:
  void newConnectionData();
  void readyReadData(int id);
  void disconnectedData(int id);
  void garbageData();

 private:
  void SendStaticSource(int id,int n);
  void SendHeader(int id,const QString &name="",const QString &value="") const;
  QStringList http_static_filenames;
  QStringList http_static_uris;
  QStringList http_static_mimetypes;
  QTcpServer *http_server;
  QSignalMapper *http_read_mapper;
  QSignalMapper *http_disconnect_mapper;
  std::vector<__WHHttpConnection *> http_connections;
  QTimer *http_garbage_timer;
};


#endif  // WHHTTPSERVER_H
