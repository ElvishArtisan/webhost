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

#include <map>
#include <vector>

#include <QByteArray>
#include <QObject>
#include <QSignalMapper>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include <wh/whhttpconnection.h>
#include <wh/whhttprequest.h>
#include <wh/whhttpuser.h>

class WHHttpServer : public QObject
{
  Q_OBJECT;
 public:
  WHHttpServer(QObject *parent=0);
  ~WHHttpServer();
  bool listen(uint16_t port);
  bool listen(const QHostAddress &iface,uint16_t port);
  QStringList userRealms() const;
  QStringList userNames(const QString &realm);
  void addUser(const QString &realm,const QString &name,const QString &passwd);
  void removeUser(const QString &realm,const QString &name);
  bool loadUsers(const QString &filename);
  bool saveUsers(const QString &filename);
  void addStaticSource(const QString &uri,const QString &mimetype,
		       const QString &filename,const QString &realm="");
  void addCgiSource(const QString &uri,const QString &filename,
		    const QString &realm="");
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

 protected:
  virtual void requestReceived(WHHttpConnection *conn);
  virtual bool authenticateUser(const QString &realm,const QString &name,
				const QString &passwd);

 private slots:
  void newConnectionData();
  void readyReadData(int id);
  void disconnectedData(int id);
  void cgiFinishedData(int id);
  void garbageData();

 private:
  void ReadMethodLine(int id);
  void ReadHeaders(int id);
  void ReadBody(int id);
  void ProcessRequest(int id);
  void SendStaticSource(int id,int n);
  void SendCgiSource(int id,int n);
  bool IsCgiScript(const QString &uri) const;
  bool AuthenticateRealm(int id,const QString &realm,const QString &name,
			 const QString &passwd);
  QStringList http_static_filenames;
  QStringList http_static_uris;
  QStringList http_static_mimetypes;
  QStringList http_static_realms;
  QStringList http_cgi_filenames;
  QStringList http_cgi_uris;
  QStringList http_cgi_realms;
  QTcpServer *http_server;
  QSignalMapper *http_read_mapper;
  QSignalMapper *http_disconnect_mapper;
  QSignalMapper *http_cgi_finished_mapper;
  std::vector<WHHttpConnection *> http_connections;
  QTimer *http_garbage_timer;
  //  int http_istate;
  std::map<QString,std::vector<WHHttpUser *> > http_users;
};


#endif  // WHHTTPSERVER_H
