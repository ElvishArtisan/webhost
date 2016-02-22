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
  connect(test_server,SIGNAL(requestReceived(int,WHHttpRequest *)),
	  this,SLOT(requestReceivedData(int,WHHttpRequest *)));
  if(!test_server->listen(8080)) {
    fprintf(stderr,"httpserver: unable to bind port 8080\n");
    exit(256);
  }
  test_server->addStaticSource("/testfile.txt","text/plain",
			       "/home/fredg/temp/voa-streams.txt");
  printf("listening on port 8080\n");
}


void MainObject::requestReceivedData(int id,WHHttpRequest *req)
{
  printf("Received Request:\n");
  printf("%s",(const char *)req->dump().toUtf8());
  test_server->sendResponse(id,200,"Hello World!","text/html");
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
