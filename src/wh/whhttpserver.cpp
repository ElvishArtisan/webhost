// whhttpserver.cpp
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

#include <stdio.h>
#include <time.h>

#include <QDateTime>
#include <QFile>

#include "whhttpserver.h"

__WHHttpConnection::__WHHttpConnection(QTcpSocket *sock)
{
  conn_socket=sock;
  conn_request=new WHHttpRequest();
  accum="";
}


__WHHttpConnection::~__WHHttpConnection()
{
  delete conn_request;
  delete conn_socket;
}


WHHttpRequest *__WHHttpConnection::request()
{
  return conn_request;
}


QTcpSocket *__WHHttpConnection::socket()
{
  return conn_socket;
}




WHHttpServer::WHHttpServer(QObject *parent)
  : QObject(parent)
{
  http_server=new QTcpServer(this);
  connect(http_server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));

  http_read_mapper=new QSignalMapper(this);
  connect(http_read_mapper,SIGNAL(mapped(int)),this,SLOT(readyReadData(int)));

  http_disconnect_mapper=new QSignalMapper(this);
  connect(http_disconnect_mapper,SIGNAL(mapped(int)),
	  this,SLOT(disconnectedData(int)));

  http_garbage_timer=new QTimer(this);
  http_garbage_timer->setSingleShot(true);
  connect(http_garbage_timer,SIGNAL(timeout()),this,SLOT(garbageData()));
}


WHHttpServer::~WHHttpServer()
{
  for(unsigned i=0;i<http_connections.size();i++) {
    delete http_connections[i];
  }
  delete http_garbage_timer;
  delete http_disconnect_mapper;
  delete http_read_mapper;
  delete http_server;
}


bool WHHttpServer::listen(uint16_t port)
{
  return http_server->listen(QHostAddress::Any,port);
}


bool WHHttpServer::listen(const QHostAddress &iface,uint16_t port)
{
  return http_server->listen(iface,port);
}


void WHHttpServer::addStaticSource(const QString &uri,const QString &mimetype,
				   const QString &filename)
{
  http_static_uris.push_back(uri);
  http_static_mimetypes.push_back(mimetype);
  http_static_filenames.push_back(filename);
}


void WHHttpServer::sendResponse(int id,int stat_code,
				const QStringList &hdr_names,
				const QStringList &hdr_values,
				const QByteArray &body,
				const QString &mimetype)
{
  QString statline=QString().sprintf("HTTP/1.1 %d ",stat_code)+
    WHHttpRequest::statusText(stat_code)+"\r\n";
  http_connections[id]->socket()->write(statline.toUtf8());
  SendHeader(id,"Date",WHHttpRequest::
	 datetimeStamp(QDateTime(QDate::currentDate(),QTime::currentTime())));
  SendHeader(id,"Server",QString("Webhost/")+VERSION);
  SendHeader(id,"Connection","close");// FIXME: Implement persistent connections
  if(body.length()>0) {
    SendHeader(id,"Content-Length",QString().sprintf("%d",body.length()));
    SendHeader(id,"Content-Type",mimetype);
  }
  for(int i=0;i<hdr_names.size();i++) {
    SendHeader(id,hdr_names[i],hdr_values[i]);
  }
  SendHeader(id);
  http_connections[id]->socket()->write(body);
}


void WHHttpServer::sendResponse(int id,int stat_code,
				const QByteArray &body,const QString &mimetype)
{
  sendResponse(id,stat_code,QStringList(),QStringList(),body,mimetype);
}


void WHHttpServer::sendError(int id,int stat_code,const QString &msg,
			     const QStringList &hdr_names,
			     const QStringList &hdr_values)
{
  QString err_text=msg;
  if(err_text.isEmpty()) {
    err_text=
      QString().sprintf("%d ",stat_code)+WHHttpRequest::statusText(stat_code);
  }
  sendResponse(id,stat_code,hdr_names,hdr_values,err_text.toUtf8());
}


void WHHttpServer::newConnectionData()
{
  int id=-1;

  for(unsigned i=0;i<http_connections.size();i++) {
    if(http_connections[i]==NULL) {
      http_connections[i]=
	new __WHHttpConnection(http_server->nextPendingConnection());
      id=i;
      break;
    }
  }
  if(id<0) {
    http_connections.
      push_back(new __WHHttpConnection(http_server->nextPendingConnection()));
    id=http_connections.size()-1;
  }
  connect(http_connections[id]->socket(),SIGNAL(readyRead()),
	  http_read_mapper,SLOT(map()));
  http_read_mapper->setMapping(http_connections[id]->socket(),id);

  connect(http_connections[id]->socket(),SIGNAL(disconnected()),
	  http_disconnect_mapper,SLOT(map()));
  http_disconnect_mapper->setMapping(http_connections[id]->socket(),id);
}


void WHHttpServer::readyReadData(int id)
{
  QStringList hdr_names;
  QStringList hdr_values;
  QString line;
  __WHHttpConnection *conn=http_connections[id];
  WHHttpRequest *req=conn->request();

  while(conn->socket()->canReadLine()) {
    line=QString(conn->socket()->readLine()).trimmed();
    if(req->method()==WHHttpRequest::None) {  // Method Line
      //printf("LINE: %s\n",(const char *)line.toUtf8());
      QStringList f0=line.split(" ");
      if(f0.size()!=3) {
	sendError(id,400,"400 Bad Request<br>Malformed HTTP request");
	return;
      }

      //
      // The HTTP Method
      //
      if(f0[0].trimmed()=="GET") {
	req->setMethod(WHHttpRequest::Get);
      }
      if(req->method()==WHHttpRequest::None) {
	hdr_names.push_back("Allow");
	hdr_values.push_back("GET");
	sendError(id,501,"501 Not implemented",hdr_names,hdr_values);
	return;
      }

      req->setUri(f0[1].trimmed());

      QStringList f1=f0[2].trimmed().split("/");
      if(f1.size()!=2) {
	sendError(id,400,"400 Bad Request<br>Malformed HTTP request");
	return;
      }
      if(!req->setProtocolVersion(f1[1])) {
	sendError(id,400,"400 Bad Request<br>Malformed HTTP request");
	return;
      }
      if((req->majorProtocolVersion()!=1)||(req->minorProtocolVersion()>1)) {
	sendError(id,505,"505 HTTP Version Not Supported<br>This server only supports HTTP v1.x");
	return;
      }
    }
    else {
      bool processed=false;
      QStringList f0=line.split(": ",QString::KeepEmptyParts);
      if(f0.size()>=2) {
	QString hdr=f0[0];
	f0.erase(f0.begin());
	QString value=f0.join(": ");
	if(hdr=="Host") {
	  if(!req->setHost(value)) {
	    sendError(id,400,"400 Bad Request<br>Malformed Host: header");
	    return;
	  }
	  processed=true;
	}
	if(!processed) {
	  req->addHeader(hdr,value);
	}
      }
      else {
	if(line.isEmpty()) {  // End of headers
	  if((req->minorProtocolVersion()==1)&&(req->hostPort()==0)) {
	    sendError(id,400,"400 Bad Request<br>Missing/malformed Host: header");
	    return;
	  }
	  for(int i=0;i<http_static_uris.size();i++) {
	    if(req->uri()==http_static_uris[i]) {
	      SendStaticSource(id,i);
	      return;
	    }
	  }
	  emit requestReceived(id,req);
	}
      }
    }
  } 
}


void WHHttpServer::disconnectedData(int id)
{
  http_garbage_timer->start(0);
}


void WHHttpServer::garbageData()
{
  for(unsigned i=0;i<http_connections.size();i++) {
    if(http_connections[i]->socket()->state()!=QAbstractSocket::ConnectedState) {
      delete http_connections[i];
      http_connections[i]=NULL;
    }
  }
}


void WHHttpServer::SendStaticSource(int id,int n)
{
  QFile file(http_static_filenames[n]);

  if(!file.exists()) {
    sendError(id,404);
    return;
  }
  if(!file.open(QIODevice::ReadOnly)) {
    sendError(id,500);
    return;
  }
  QByteArray data=file.readAll();
  sendResponse(id,200,data,http_static_mimetypes[n]);
  file.close();
}


void WHHttpServer::SendHeader(int id,const QString &name,const QString &value)
  const
{
  if(name.isEmpty()&&value.isEmpty()) {
    http_connections[id]->socket()->write("\r\n");
  }
  else {
    QString line=name+": "+value+"\r\n";
    http_connections[id]->socket()->write(line.toUtf8());
  }
}
