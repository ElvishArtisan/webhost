// upgrade.cpp
//
//   IP command implementation for webhostd(8)
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

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QProcess>
#include <QStringList>

#include "webhostd.h"

void MainObject::Upgrade(const QStringList &cmds)
{
  QProcess *proc=NULL;
  QStringList args;
  QString action;
  QString tempdir;

  if(cmds.size()==2) {
    //
    // Get temporary directory
    //
    QStringList f0=cmds[1].split("/");
    f0.erase(f0.begin()+f0.size()-1);
    tempdir=f0.join("/");

    //
    // Apply Package
    //
    args.push_back("--oldpackage");
    args.push_back("-U");
    args.push_back(cmds[1]);
    proc=new QProcess(this);
    proc->start("/bin/rpm",args);
    proc->waitForFinished();
    sync();
    
    //
    // Signal completion
    //
    int fd;
    if((fd=open((tempdir+"/done").toUtf8(),O_CREAT|O_WRONLY,
		S_IRWXU|S_IRWXG|S_IRWXO))>0) {
      close(fd);
    }
  }

  //
  // Cleanup temp directory
  //
  unlink(cmds[1].toUtf8());
  unlink((tempdir+"/done").toUtf8());
  rmdir(tempdir.toUtf8());

  //
  // Kill service process
  //
  if(main_service_process!=NULL) {
    main_service_process->kill();
  }
  exit(0);
}
