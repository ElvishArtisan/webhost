// cgitest_server.cpp
//
// Test HTTP server for CGI test
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

#include "cgitest_server.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  char *cwd=NULL;

  cwd=getcwd(cwd,0);
  test_server=new WHHttpServer(this);
  if(!test_server->listen(8080)) {
    fprintf(stderr,"cgitest: unable to bind port 8080\n");
    exit(256);
  }
  QString cmd=QString(cwd)+"/src/tests/cgitest_script";
  printf("Script at: %s\n",(const char *)cmd.toUtf8());
  test_server->addCgiSource("/cgitest.cgi",cmd);
  printf("listening on port 8080\n");
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
