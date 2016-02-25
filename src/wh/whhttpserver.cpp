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
#include "whprofile.h"

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

  http_cgi_finished_mapper=new QSignalMapper(this);
  connect(http_cgi_finished_mapper,SIGNAL(mapped(int)),
	  this,SLOT(cgiFinishedData(int)));

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


QStringList WHHttpServer::userRealms() const
{
  QStringList ret;

  for(std::map<QString,std::vector<WHHttpUser *> >::const_iterator it=http_users.begin();
      it!=http_users.end();it++) {
    ret.push_back(it->first);
  }

  return ret;
}


QStringList WHHttpServer::userNames(const QString &realm)
{
  QStringList ret;

  for(unsigned i=0;i<http_users[realm].size();i++) {
    ret.push_back(http_users[realm][i]->name());
  }

  return ret;
}


void WHHttpServer::addUser(const QString &realm,const QString &name,
			   const QString &passwd)
{
  for(unsigned i=0;i<http_users[realm].size();i++) {
    if(http_users[realm][i]->name()==name) {
      http_users[realm][i]->setPassword(passwd);
      return;
    }
  }
  http_users[realm].push_back(new WHHttpUser(name,passwd));
}


void WHHttpServer::removeUser(const QString &realm,const QString &name)
{
  for(unsigned i=0;i<http_users[realm].size();i++) {
    if(http_users[realm][i]->name()==name) {
      delete http_users[realm][i];
      http_users[realm].erase(http_users[realm].begin()+i);
      return;
    }
  }
}


bool WHHttpServer::loadUsers(const QString &filename)
{
  WHProfile *p=new WHProfile();
  if(!p->setSource(filename)) {
    return false;
  }
  for(std::map<QString,std::vector<WHHttpUser *> >::const_iterator it=http_users.begin();
      it!=http_users.end();it++) {
    for(unsigned i=0;i<it->second.size();i++) {
      delete it->second[i];
    }
  }
  http_users.clear();

  int count=0;
  QString realm;
  QString name;
  QString passwd;
  bool ok=false;
  QString section=QString().sprintf("WebHostUser%d",count+1);

  realm=p->stringValue(section,"Realm","",&ok);
  while(ok) {
    name=p->stringValue(section,"Name");
    passwd=p->stringValue(section,"Password");
    addUser(realm,name,passwd);
    count++;
    section=QString().sprintf("WebHostUser%d",count+1);
    realm=p->stringValue(section,"Realm","",&ok);
  }

  return true;
}


bool WHHttpServer::saveUsers(const QString &filename)
{
  FILE *f=NULL;
  int count=0;

  if((f=fopen((filename+".back").toUtf8(),"w"))==NULL) {
    return false;
  }
  for(std::map<QString,std::vector<WHHttpUser *> >::const_iterator it=http_users.begin();
      it!=http_users.end();it++) {
    for(unsigned i=0;i<it->second.size();i++) {
      fprintf(f,"[WebHostUser%d]\n",count+1);
      fprintf(f,"Realm=%s\n",(const char *)it->first.toUtf8());
      fprintf(f,"Name=%s\n",(const char *)it->second[i]->name().toUtf8());
      fprintf(f,"Password=%s\n",
	      (const char *)it->second[i]->password().toUtf8());
      fprintf(f,"\n");
      count++;
    }
  }
  fclose(f);
  rename((filename+".back").toUtf8(),filename.toUtf8());

  return true;
}


void WHHttpServer::addStaticSource(const QString &uri,const QString &mimetype,
				   const QString &filename,const QString &realm)
{
  http_static_uris.push_back(uri);
  http_static_mimetypes.push_back(mimetype);
  http_static_filenames.push_back(filename);
  http_static_realms.push_back(realm);
}


void WHHttpServer::addCgiSource(const QString &uri,const QString &filename,
				const QString &realm)
{
  http_cgi_uris.push_back(uri);
  http_cgi_filenames.push_back(filename);
  http_cgi_realms.push_back(realm);
}


void WHHttpServer::sendResponse(int id,int stat_code,
				const QStringList &hdr_names,
				const QStringList &hdr_values,
				const QByteArray &body,
				const QString &mimetype)
{
  http_connections[id]->
    sendResponse(stat_code,hdr_names,hdr_values,body,mimetype);
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
  http_connections[id]->sendError(stat_code,msg,hdr_names,hdr_values);
}


void WHHttpServer::requestReceived(WHHttpConnection *conn)
{
  fprintf(stderr,"URI \"%s\" not found\n",
	  (const char *)conn->uri().toUtf8());
  conn->sendError(404,"404 Not found");
}


bool WHHttpServer::authenticateUser(const QString &realm,const QString &name,
				    const QString &passwd)
{
  for(unsigned i=0;i<http_users[realm].size();i++) {
    if(http_users[realm][i]->isValid(name,passwd)) {
      return true;
    }
  }
  return false;
}


void WHHttpServer::newConnectionData()
{
  int id=-1;

  for(unsigned i=0;i<http_connections.size();i++) {
    if(http_connections[i]==NULL) {
      http_connections[i]=
	new WHHttpConnection(http_server->nextPendingConnection(),this);
      id=i;
      break;
    }
  }
  if(id<0) {
    http_connections.
      push_back(new WHHttpConnection(http_server->nextPendingConnection(),
				     this));
    id=http_connections.size()-1;
  }
  connect(http_connections[id]->socket(),SIGNAL(readyRead()),
	  http_read_mapper,SLOT(map()));
  http_read_mapper->setMapping(http_connections[id]->socket(),id);

  connect(http_connections[id]->socket(),SIGNAL(disconnected()),
	  http_disconnect_mapper,SLOT(map()));
  http_disconnect_mapper->setMapping(http_connections[id]->socket(),id);

  connect(http_connections[id],SIGNAL(cgiFinished()),
	  http_cgi_finished_mapper,SLOT(map()));
  http_cgi_finished_mapper->setMapping(http_connections[id]->socket(),id);
}


void WHHttpServer::readyReadData(int id)
{
  switch(http_connections[id]->parseState()) {
  case 0:
    ReadMethodLine(id);
    break;

  case 1:
    ReadHeaders(id);
    break;

  case 2:
    ReadBody(id);
    break;
  }
}


void WHHttpServer::disconnectedData(int id)
{
  http_garbage_timer->start(0);
}


void WHHttpServer::cgiFinishedData(int id)
{
  http_garbage_timer->start(0);
}


void WHHttpServer::garbageData()
{
  for(unsigned i=0;i<http_connections.size();i++) {
    if(http_connections[i]!=NULL) {
      if(http_connections[i]->socket()->state()!=
	 QAbstractSocket::ConnectedState) {
	delete http_connections[i];
	http_connections[i]=NULL;
      }
    }
  }
}


void WHHttpServer::ReadMethodLine(int id)
{
  QStringList hdr_names;
  QStringList hdr_values;
  QString line;
  WHHttpConnection *conn=http_connections[id];

  if(conn->socket()->canReadLine()) {
    line=QString(conn->socket()->readLine()).trimmed();
    //printf("METHODLINE: %s\n",(const char *)line.toUtf8());
    QStringList f0=line.split(" ");
    if(f0.size()!=3) {
      sendError(id,400,"400 Bad Request<br>Malformed HTTP request");
      return;
    }

    //
    // The HTTP Method
    //
    if(f0[0].trimmed()=="GET") {
      conn->setMethod(WHHttpConnection::Get);
    }
    if(f0[0].trimmed()=="POST") {
      conn->setMethod(WHHttpConnection::Post);
    }
    if(conn->method()==WHHttpConnection::None) {
      hdr_names.push_back("Allow");
      hdr_values.push_back("GET");
      if(IsCgiScript(f0[1].trimmed())) {
	hdr_values.back()+=",POST";
      }
      sendError(id,501,"501 Not implemented",hdr_names,hdr_values);
      return;
    }

    conn->setUri(f0[1].trimmed());

    QStringList f1=f0[2].trimmed().split("/");
    if(f1.size()!=2) {
      sendError(id,400,"400 Bad Request<br>Malformed HTTP request");
      return;
    }
    if(!conn->setProtocolVersion(f1[1])) {
      sendError(id,400,"400 Bad Request<br>Malformed HTTP request");
      return;
    }
    if((conn->majorProtocolVersion()!=1)||(conn->minorProtocolVersion()>1)) {
      sendError(id,505,"505 HTTP Version Not Supported<br>This server only supports HTTP v1.x");
      return;
    }
    conn->nextParseState();
    ReadHeaders(id);
  }
}


void WHHttpServer::ReadHeaders(int id)
{
  QStringList hdr_names;
  QStringList hdr_values;
  QString line;
  WHHttpConnection *conn=http_connections[id];
  bool ok=false;

  while(conn->socket()->canReadLine()) {
    line=QString(conn->socket()->readLine()).trimmed();
    //printf("HEADER: %s\n",(const char *)line.toUtf8());
    bool processed=false;
    QStringList f0=line.split(": ",QString::KeepEmptyParts);
    if(f0.size()>=2) {
      QString hdr=f0[0];
      f0.erase(f0.begin());
      QString value=f0.join(": ");
      if(hdr=="Authorization") {
	conn->setAuthorization(value);
	processed=true;
      }
      if(hdr=="Content-Length") {
	int64_t len=value.toLongLong(&ok);
	if(!ok) {
	  sendError(id,400,
		    "400 Bad Request Malformed Content-Length: header");
	  return;
	}
	conn->setContentLength(len);
	processed=true;
      }
      if(hdr=="Content-Type") {
	QStringList f1=value.split(";");
	conn->setContentType(f1[0]);
	processed=true;
      }
      if(hdr=="Host") {
	if(!conn->setHost(value)) {
	  sendError(id,400,"400 Bad Request Malformed Host: header");
	  return;
	}
	processed=true;
      }
      if(hdr=="Referer") {
	conn->setReferrer(value);
	processed=true;
      }
      if(hdr=="User-Agent") {
	conn->setUserAgent(value);
	processed=true;
      }
      if(!processed) {
	conn->addHeader(hdr,value);
      }
    }
    else {
      if(line.isEmpty()) {  // End of headers
	if((conn->minorProtocolVersion()==1)&&(conn->hostPort()==0)) {
	  sendError(id,400,
		    "400 Bad Request -- Missing/malformed Host: header");
	  return;
	}
	if(conn->method()==WHHttpConnection::Get) {
	  ProcessRequest(id);
	}
	else {
	  conn->nextParseState();
	  ReadBody(id);
	}
	return;
      }
    }
  }
}


void WHHttpServer::ReadBody(int id)
{
  WHHttpConnection *conn=http_connections[id];
  conn->appendBody(conn->socket()->
		  read(conn->contentLength()-conn->body().length()));
  if(conn->body().length()==conn->contentLength()) {
    ProcessRequest(id);
  }
}


void WHHttpServer::ProcessRequest(int id)
{
  WHHttpConnection *conn=http_connections[id];

  for(int i=0;i<http_static_uris.size();i++) {
    if(conn->uri()==http_static_uris[i]) {
      SendStaticSource(id,i);
      return;
    }
  }
  for(int i=0;i<http_cgi_uris.size();i++) {
    if(conn->uri()==http_cgi_uris[i]) {
      SendCgiSource(id,i);
      return;
    }
  }
  requestReceived(http_connections[id]);
}


void WHHttpServer::SendStaticSource(int id,int n)
{
  QFile file(http_static_filenames[n]);

  if(!AuthenticateRealm(id,http_static_realms[n],
			http_connections[id]->authName(),
			http_connections[id]->authPassword())) {
    return;
  }

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


void WHHttpServer::SendCgiSource(int id,int n)
{
  if(!AuthenticateRealm(id,http_cgi_realms[n],
			http_connections[id]->authName(),
			http_connections[id]->authPassword())) {
    return;
  }
  http_connections[id]->startCgiScript(http_cgi_filenames[n]);
}


bool WHHttpServer::IsCgiScript(const QString &uri) const
{
  for(int i=0;i<http_cgi_uris.size();i++) {
    if(http_cgi_uris[i]==uri) {
      return true;
    }
  }
  return false;
}


bool WHHttpServer::AuthenticateRealm(int id,const QString &realm,
				     const QString &name,const QString &passwd)
{
  QStringList hdrs;
  QStringList values;

  if(realm.isEmpty()||authenticateUser(realm,name,passwd)) {
    return true;
  }
  hdrs.push_back("WWW-Authenticate");
  values.push_back("Basic realm=\""+realm+"\"");
  http_connections[id]->sendResponse(401,hdrs,values,"401 Unauthorized");
  return false;
}
