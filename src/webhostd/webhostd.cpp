// webhostd.cpp
//
// webhostd(8) routing daemon
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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
#include <QHostAddress>

#include "cmdswitch.h"
#include "webhostd.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  main_debug=false;

  CmdSwitch *cmd=
    new CmdSwitch(qApp->argc(),qApp->argv(),"webhost",WEBHOSTD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="-d") {
      main_debug=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"webhostd: unknown option \"%s\"\n",
	      (const char *)cmd->key(i).toUtf8());
      exit(256);
    }
  }

  //
  // Configuration
  //
  main_config=new Config();
  main_config->load();

  //
  // Command Socket
  //
  main_command_socket=new QUdpSocket(this);
  if(!main_command_socket->bind(main_config->controlPort())) {
    fprintf(stderr,"webhostd: cannot bind port %u\n",
	    main_config->controlPort());
    exit(256);
  }
  connect(main_command_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
}



void MainObject::readyReadData()
{
  char data[1500];
  int n=0;
  QHostAddress addr;
  QString accum;

  while((n=main_command_socket->readDatagram(data,1500,&addr))>0) {
    //
    // Only accept commands from localhost
    //
    if((addr.toIPv4Address()&0xFF000000)==0x7F000000) {
      for(int i=0;i<n;i++) {
	switch(data[i]) {
	case 10:
	case 13:
	  break;

	case '!':
	  ProcessCommand(accum);
	  accum="";
	  break;

	default:
	  accum+=data[i];
	  break;
	}
      }
    }
  }
}


void MainObject::ProcessCommand(const QString &cmd)
{
  QStringList cmds=cmd.split(" ");
  QString verb=cmds[0].toLower();

  if(verb=="ip") {
    Ip(cmds);
  }

  if(verb=="ntp") {
    Ntp(cmds);
  }

  if(verb=="reboot") {
    Reboot(cmds);
  }

  if(verb=="restart") {
    Restart(cmds);
  }

  if(verb=="upgrade") {
    Upgrade(cmds);
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
