// httpserver.cpp
//
// Test harness for the WHHttp classes
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

#include <QCoreApplication>

#include "httpserver.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  test_server=new WHHttpServer(this);
  connect(test_server,SIGNAL(requestReceived(WHHttpConnection *)),
	  this,SLOT(requestReceivedData(WHHttpConnection *)));
  if(!test_server->listen(80)) {
    fprintf(stderr,"httpserver: unable to bind port 8080\n");
    exit(256);
  }
  test_server->addCgiSource("/","/var/www/cgi-bin/glassplayerhost.cgi");
  test_server->addCgiSource("/cgi-bin/glassplayerhost.cgi",
			    "/var/www/cgi-bin/glassplayerhost.cgi");
  printf("listening on port 80\n");
}


void MainObject::requestReceivedData(WHHttpConnection *conn)
{
  printf("Received Request:\n");
  printf("%s",(const char *)conn->dump().toUtf8());
  /*
  conn->sendResponse(200,"Hello World!","text/html");
  */
  conn->sendError(404,"404 Not found");
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
