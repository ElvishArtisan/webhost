// wifi.cpp
//
//   WIFI command implementation for webhostd(8)
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <unistd.h>

#include <QHostAddress>
#include <QProcess>
#include <QStringList>

#include "webhostd.h"

void MainObject::wifiProcessFinishedData(int exit_code,QProcess::ExitStatus status)
{
  bool deleted=true;
  while(deleted) {
    deleted=false;
    for(int i=0;i<main_wifi_processes.size();i++) {
      if(main_wifi_processes.at(i)->state()==QProcess::NotRunning) {
	main_wifi_processes.at(i)->deleteLater();
	main_wifi_processes.removeAt(i);
	deleted=true;
	break;
      }
    }
  }
}


void MainObject::Wifi(const QStringList &cmds)
{
  QStringList args;

  if(main_config->useNetworkManager()) {
    if(cmds.size()==1) {
      args.clear();
      args.push_back("-t");
      args.push_back("conn");
      args.push_back("show");
      QString resp=RunCommand("/bin/nmcli",args);
      QStringList f0=resp.split("\n");
      for(int i=0;i<f0.size();i++) {
	QStringList f1=f0.at(i).split(":");
	if(f1.size()>=3) {
	  if(f1.at(2)=="802-11-wireless") {
	    args.clear();
	    args.push_back("conn");
	    args.push_back("delete");
	    args.push_back(f1.at(0));
	    main_wifi_processes.push_back(new QProcess(this));
	    connect(main_wifi_processes.back(),
		    SIGNAL(finished(int,QProcess::ExitStatus)),
		    this,
		    SLOT(wifiProcessFinishedData(int,QProcess::ExitStatus)));
	    main_wifi_processes.back()->start("/bin/nmcli",args);
	  }
	}
      }
      return;
    }
    if(cmds.size()>1) {
      args.clear();
      args.push_back("device");
      args.push_back("wifi");
      args.push_back("connect");
      args.push_back(cmds.at(1));
      if(cmds.size()>2) {
	QString password=cmds.at(2);
	/*
	for(int i=3;i<cmds.size();i++) {
	  password+=" "+cmds.at(i);
	}
	*/
	args.push_back("password");
	args.push_back(password);
      }
      main_wifi_processes.push_back(new QProcess(this));
      connect(main_wifi_processes.back(),
	      SIGNAL(finished(int,QProcess::ExitStatus)),
	      this,SLOT(wifiProcessFinishedData(int,QProcess::ExitStatus)));
      main_wifi_processes.back()->start("/bin/nmcli",args);
    }
  }
}
