// ntp.cpp
//
//   NTP command implementation for webhostd(8)
//
//   (C) Copyright 2015-2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QHostAddress>
#include <QProcess>
#include <QStringList>

#include "webhostd.h"

void MainObject::Ntp(const QStringList &cmds)
{
  FILE *f=NULL;
  QProcess *proc;
  QStringList args;

  //
  // Set the Timezone
  //
  args.clear();
  args.push_back("set-timezone");
  args.push_back(cmds[1]);
  proc=new QProcess(this);
  proc->start("timedatectl",args);
  proc->waitForFinished();
  delete proc;

  //
  // Write new configuration file
  //
  if((f=fopen((main_config->ntpConfigurationFile()+".back").toUtf8(),"w"))!=
     NULL) {
    for(int i=2;i<cmds.size();i++) {
      if(cmds[i]!="0.0.0.0") {
	fprintf(f,"server %s iburst\n",(const char *)cmds[i].toUtf8());
      }
    }
    fclose(f);
    rename((main_config->ntpConfigurationFile()+".back").toUtf8(),
	   main_config->ntpConfigurationFile().toUtf8());    
    sync();
  }

  //
  // Restart NTP server
  //
  args.clear();
  args.push_back("restart");
  args.push_back(main_config->ntpServiceName());
  proc=new QProcess(this);
  proc->start("systemctl",args);
  proc->waitForFinished();
  delete proc;
}
