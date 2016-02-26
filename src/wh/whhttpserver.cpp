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

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <QDateTime>
#include <QFile>

#include <openssl/evp.h>

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


void WHHttpServer::sendSocketMessage(int conn_id,WHSocketMessage::OpCode opcode,
				     const QByteArray &data)
{
  QByteArray packet;
  uint32_t key=random();
  uint8_t keychar[4];
  WHHttpConnection *conn=http_connections[conn_id];

  //
  // OpCode
  //
  packet.append(0x80|(0x0F&opcode));

  //
  // Payload Length
  //
  // WARNING: This breaks with messages longer than 4,294,967,295 bytes!
  //          See RFC 6455 Section 5.2
  //
  if(data.length()<126) {
    packet.append(0x80|data.length());
  }
  else {
    if(data.length()<65536) {
      packet.append(254);
      packet.append(data.length()>>8);
      packet.append(0xFF&data.length());
    }
    else {
      packet.append(255);
      packet.append((char)0);
      packet.append((char)0);
      packet.append((char)0);
      packet.append((char)0);
      packet.append(data.length()>>24);
      packet.append(0xFF&(data.length()>>16));
      packet.append(0xFF&(data.length()>>8));
      packet.append(0xFF&data.length());
    }
  }

  //
  // Masking Key
  //
  keychar[0]=key>>24;
  keychar[1]=0xFF&(key>>16);
  keychar[2]=0xFF&(key>>8);
  keychar[3]=0xFF&key;
  for(int i=0;i<4;i++) {
    packet.append(keychar[i]);
  }

  //
  // Payload
  //
  for(int i=0;i<data.length();i++) {
    packet.append(data[i]^keychar[i%4]);
  }
  conn->socket()->write(packet);
}


void WHHttpServer::sendSocketMessage(int conn_id,const QByteArray &data)
{
  sendSocketMessage(conn_id,WHSocketMessage::Binary,data);
}


void WHHttpServer::sendSocketMessage(int conn_id,const QString &str)
{
  sendSocketMessage(conn_id,WHSocketMessage::Text,str.toUtf8());
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


void WHHttpServer::addSocketSource(const QString &uri,const QString &proto,
				   const QString &realm)
{
  http_socket_uris.push_back(uri);
  http_socket_protocols.push_back(proto);
  http_socket_realms.push_back(realm);
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
	new WHHttpConnection(i,http_server->nextPendingConnection(),this);
      id=i;
      break;
    }
  }
  if(id<0) {
    id=http_connections.size();
    http_connections.
      push_back(new WHHttpConnection(id,http_server->nextPendingConnection(),
					this));
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
  WHHttpConnection *conn=http_connections[id];

  switch(conn->parseState()) {
  case 0:
    ReadMethodLine(conn);
    break;

  case 1:
    ReadHeaders(conn);
    break;

  case 2:
    ReadBody(conn);
    break;

  case 10:
    ReadWebsocket(conn);
    break;
  }
}


void WHHttpServer::disconnectedData(int id)
{
  if(http_connections[id]->isWebsocket()) {
    emit socketConnectionClosed(id);
  }
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


void WHHttpServer::ReadMethodLine(WHHttpConnection *conn)
{
  QStringList hdr_names;
  QStringList hdr_values;
  QString line;

  if(conn->socket()->canReadLine()) {
    line=QString(conn->socket()->readLine()).trimmed();
    //printf("METHODLINE: %s\n",(const char *)line.toUtf8());
    QStringList f0=line.split(" ");
    if(f0.size()!=3) {
      conn->sendError(400,"400 Bad Request<br>Malformed HTTP request");
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
      conn->sendError(501,"501 Not implemented",hdr_names,hdr_values);
      return;
    }

    conn->setUri(f0[1].trimmed());

    QStringList f1=f0[2].trimmed().split("/");
    if(f1.size()!=2) {
      conn->sendError(400,"400 Bad Request<br>Malformed HTTP request");
      return;
    }
    if(!conn->setProtocolVersion(f1[1])) {
      conn->sendError(400,"400 Bad Request<br>Malformed HTTP request");
      return;
    }
    if((conn->majorProtocolVersion()!=1)||(conn->minorProtocolVersion()>1)) {
      conn->sendError(505,"505 HTTP Version Not Supported<br>This server only supports HTTP v1.x");
      return;
    }
    conn->setParseState(1);
    ReadHeaders(conn);
  }
}


void WHHttpServer::ReadHeaders(WHHttpConnection *conn)
{
  QStringList hdr_names;
  QStringList hdr_values;
  QString line;
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
	  conn->
	    sendError(400,"400 Bad Request Malformed Content-Length: header");
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
	  conn->sendError(400,"400 Bad Request Malformed Host: header");
	  return;
	}
	processed=true;
      }
      if(hdr=="Referer") {
	conn->setReferrer(value);
	processed=true;
      }
      if(hdr=="Sec-WebSocket-Protocol") {
	conn->setSubProtocol(value);
	processed=true;
      }
      if(hdr=="Upgrade") {
	conn->setUpgrade(value);
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
	  conn->
	    sendError(400,"400 Bad Request -- Missing/malformed Host: header");
	  return;
	}
	if(conn->method()==WHHttpConnection::Get) {
	  ProcessRequest(conn);
	}
	else {
	  conn->setParseState(2);
	  ReadBody(conn);
	}
	return;
      }
    }
  }
}


void WHHttpServer::ReadWebsocket(WHHttpConnection *conn)
{
  QByteArray data=conn->socket()->readAll();
  uint32_t plen=0;
  int offset=0;
  WHSocketMessage *msg=NULL;

  if((0x70&data[0])!=0) {  // Extension bits set
    conn->socket()->close();
  }
  if((0x80&data[1])==0) {  // Mask not set
    conn->socket()->close();
  }
  WHSocketMessage::OpCode opcode=(WHSocketMessage::OpCode)(0x0F&data[0]);
  if(WHSocketMessage::isControlMessage(opcode)) {
    msg=conn->cntlSocketMessage();
  }
  else {
    msg=conn->appSocketMessage();
  }

  bool finished=(0x80&data[0])!=0;

  if(opcode!=WHSocketMessage::Continuation) {
    msg->setOpCode(opcode);
    msg->clearPayload();
  }
  
  //
  // Payload Length
  //
  // WARNING: This breaks with messages longer than 4,294,967,295 bytes!
  //          See RFC 6455 Section 5.2
  //
  plen=0x7F&data[1];
  if(plen==126) {
    plen=((0xFF&data[2])<<8)+(0xFF&data[3]);
    offset=2;
  }
  else {
    if(plen==127) {
      plen=((0xFF&data[6])<<24)+((0xFF&data[7])<<16)+((0xFF&data[8])<<8)+
	(0xFF&data[9]);
      offset=8;
    }
  }

  //
  // Extract Payload
  //
  for(uint32_t i=0;i<plen;i++) {
    msg->appendPayload(data[offset+6+i]^data[offset+2+(i%4)]);
  }

  //
  // Disposition
  //
  switch(msg->opCode()) {
  case WHSocketMessage::Text:
  case WHSocketMessage::Binary:
  case WHSocketMessage::AppReserv3:
  case WHSocketMessage::AppReserv4:
  case WHSocketMessage::AppReserv5:
  case WHSocketMessage::AppReserv6:
  case WHSocketMessage::AppReserv7:
    if(finished) {
      emit socketMessageReceived(conn->id(),msg);
    }
    break;

  case WHSocketMessage::Close:
    sendSocketMessage(conn->id(),WHSocketMessage::Close,msg->payload());
    conn->socket()->close();
    break;

  case WHSocketMessage::Ping:
    sendSocketMessage(conn->id(),WHSocketMessage::Pong,msg->payload());
    break;

  case WHSocketMessage::Continuation:
  case WHSocketMessage::Pong:
  case WHSocketMessage::CntlReserv11:
  case WHSocketMessage::CntlReserv12:
  case WHSocketMessage::CntlReserv13:
  case WHSocketMessage::CntlReserv14:
  case WHSocketMessage::CntlReserv15:
    break;
  }
}


void WHHttpServer::ReadBody(WHHttpConnection *conn)
{
  conn->appendBody(conn->socket()->
		  read(conn->contentLength()-conn->body().length()));
  if(conn->body().length()==conn->contentLength()) {
    ProcessRequest(conn);
  }
}


void WHHttpServer::ProcessRequest(WHHttpConnection *conn)
{
  if(conn->upgrade()=="websocket") {
    for(int i=0;i<http_socket_uris.size();i++) {
      if((conn->uri()==http_socket_uris[i])||
	 (conn->subProtocol()==http_socket_protocols[i])) {
	StartWebsocket(conn,i);
	return;
      }
    }
    requestReceived(conn);
    return;
  }
  for(int i=0;i<http_static_uris.size();i++) {
    if(conn->uri()==http_static_uris[i]) {
      SendStaticSource(conn,i);
      return;
    }
  }
  for(int i=0;i<http_cgi_uris.size();i++) {
    if(conn->uri()==http_cgi_uris[i]) {
      SendCgiSource(conn,i);
      return;
    }
  }
  requestReceived(conn);
}


void WHHttpServer::StartWebsocket(WHHttpConnection *conn,int n)
{
  QStringList hdrs=conn->headerNames();
  QStringList values=conn->headerValues();
  QString key;
  QByteArray resp;

  if(!AuthenticateRealm(conn,http_socket_realms[n],
			conn->authName(),conn->authPassword())) {
    return;
  }

  for(int i=0;i<hdrs.size();i++) {
    if(hdrs[i]=="Sec-WebSocket-Key") {
      key=values[i].trimmed();
    }
  }
  if((!conn->protocolAtLeast(1,1))||(conn->method()!=WHHttpConnection::Get)||
     key.isEmpty()) {
    conn->sendError(400);
    return;
  }
  resp=GetWebsocketHandshake(key);
  if(resp.length()==0) {
    conn->sendError(500,"500 Internal processing error");
    return;
  }
  QString statline="HTTP/1.1 101 Switching Protocols\r\n";
  conn->socket()->write(statline.toUtf8());
  conn->sendHeader("Upgrade","websocket");
  conn->sendHeader("Connection","upgrade");
  conn->sendHeader("Sec-WebSocket-Accept",resp);
  conn->sendHeader();
  conn->setParseState(10);
  conn->setWebsocket(true);
  emit newSocketConnection(conn->id(),conn->uri(),conn->subProtocol());
}


void WHHttpServer::SendStaticSource(WHHttpConnection *conn,int n)
{
  QFile file(http_static_filenames[n]);

  if(!AuthenticateRealm(conn,http_static_realms[n],
			conn->authName(),conn->authPassword())) {
    return;
  }

  if(!file.exists()) {
    conn->sendError(404);
    return;
  }
  if(!file.open(QIODevice::ReadOnly)) {
    conn->sendError(500);
    return;
  }
  QByteArray data=file.readAll(); 
  conn->sendResponse(200,data,http_static_mimetypes[n]);
  file.close();
}


void WHHttpServer::SendCgiSource(WHHttpConnection *conn,int n)
{
  if(!AuthenticateRealm(conn,http_cgi_realms[n],
			conn->authName(),conn->authPassword())) {
    return;
  }
  conn->startCgiScript(http_cgi_filenames[n]);
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


bool WHHttpServer::AuthenticateRealm(WHHttpConnection *conn,
				     const QString &realm,const QString &name,
				     const QString &passwd)
{
  QStringList hdrs;
  QStringList values;

  if(realm.isEmpty()||authenticateUser(realm,name,passwd)) {
    return true;
  }
  hdrs.push_back("WWW-Authenticate");
  values.push_back("Basic realm=\""+realm+"\"");
  conn->sendResponse(401,hdrs,values,"401 Unauthorized");
  return false;
}


QByteArray WHHttpServer::GetWebsocketHandshake(const QString &key) const
{
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned md_len;

  OpenSSL_add_all_digests();
  if((md=EVP_get_digestbyname("sha1"))==NULL) {
    return QByteArray();
  }
  mdctx=EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx,md,NULL);
  EVP_DigestUpdate(mdctx,(key+WEBSOCKET_MAGIC_STRING).toUtf8(),
		   (key+WEBSOCKET_MAGIC_STRING).length());
  EVP_DigestFinal_ex(mdctx,md_value,&md_len);
  EVP_MD_CTX_destroy(mdctx);

  return QByteArray((const char *)md_value).toBase64();
}
