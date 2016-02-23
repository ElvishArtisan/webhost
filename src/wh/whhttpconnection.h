// whhttpconnection.h
//
// HTTP connection state for WHHttpServer
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

#ifndef WHHTTPCONNECTION_H
#define WHHTTPCONNECTION_H

#include <stdint.h>

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include <wh/whhttprequest.h>

class WHHttpConnection : public QObject
{
  Q_OBJECT;
 public:
  WHHttpConnection(QTcpSocket *sock,QObject *parent=0);
  ~WHHttpConnection();
  QTcpSocket *socket();
  WHHttpRequest *request();
  void startCgiScript(const QString &filename);
  void sendResponseHeader(int stat_code);
  void sendResponse(int stat_code,
		    const QStringList &hdr_names,const QStringList &hdr_values,
		    const QByteArray &body=QByteArray(),
		    const QString &mimetype="");
  void sendResponse(int stat_code,
		    const QByteArray &body=QByteArray(),
		    const QString &mimetype="");
  void sendError(int stat_code,const QString &msg="",
		 const QStringList &hdr_names=QStringList(),
		 const QStringList &hdr_values=QStringList());
  void sendHeader(const QString &name="",const QString &value="");

 signals:
  void cgiFinished();

 private slots:
  void cgiStartedData();
  void cgiReadyReadData();
  void cgiFinishedData(int exit_code,QProcess::ExitStatus status);
  void cgiErrorData(QProcess::ProcessError err);

 private:
  QTcpSocket *conn_socket;
  WHHttpRequest *conn_request;
  QProcess *conn_cgi_process;
  QStringList conn_cgi_headers;
  bool conn_cgi_headers_active;
};


#endif  // WHHTTPCONNECTION_H
