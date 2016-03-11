// sockettest_server.cpp
//
// Test server for the webhost WebSocket classes
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QCoreApplication>

#include "sockettest_server.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  char *cwd=NULL;

  QString path=QString(getcwd(cwd,0));
  
  test_server=new WHHttpServer(this);
  connect(test_server,
	  SIGNAL(newSocketConnection(int,const QString &,const QString &)),
	  this,
	  SLOT(newSocketConnectionData(int,const QString &,const QString &)));
  connect(test_server,SIGNAL(socketMessageReceived(int,WHSocketMessage *)),
	  this,SLOT(socketMessageReceivedData(int,WHSocketMessage *)));
  connect(test_server,
	  SIGNAL(socketConnectionClosed(int,uint16_t,const QByteArray &)),
	  this,
	  SLOT(socketConnectionClosedData(int,uint16_t,const QByteArray &)));
  if(!test_server->listen(8080)) {
    fprintf(stderr,"httpserver: unable to bind port 8080\n");
    exit(256);
  }
  test_server->addStaticSource("/sockettest.js","application/javascript",
			       path+"/src/tests/sockettest/sockettest.js");
  test_server->addStaticSource("/sockettest.html","text/html",
			       path+"/src/tests/sockettest/sockettest.html");
  test_server->addSocketSource("/myconn","myproto");

  printf("listening at http://localhost:8080/sockettest.html\n");
}


void MainObject::newSocketConnectionData(int id,const QString &uri,
					 const QString &proto)
{
  printf("new WebSocket connection %d: uri: %s  proto: %s\n",id,
	 (const char *)uri.toUtf8(),(const char *)proto.toUtf8());
}


void MainObject::socketMessageReceivedData(int id,WHSocketMessage *msg)
{
  printf("echoing connection %d: %s\n",id,msg->payload().constData());
  test_server->sendSocketMessage(id,QString(msg->payload()));
}


void MainObject::socketConnectionClosedData(int id,uint16_t status,
					    const QByteArray &body)
{
  printf("WebSocket connection %d dropped [status code: %u]\n",id,0xFFFF&status);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
