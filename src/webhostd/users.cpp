// users.cpp
//
//   User methods for http-basic users.
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

#include <syslog.h>

#include <unistd.h>

#include <QProcess>
#include <QStringList>

#include "webhostd.h"

void MainObject::AddUser(const QStringList &cmds)
{
  QProcess *proc=NULL;
  QStringList args;

  syslog(LOG_NOTICE,"AddUser()");
  if((cmds.size()==4)||(cmds.size()==3)) {
    args.push_back("-b");
    args.push_back(cmds[1]);
    args.push_back(cmds[2]);
    if(cmds.size()==4) {
      args.push_back(cmds[3]);
    }
    else {
      args.push_back("");
    }
    syslog(LOG_NOTICE,"CMD: /usr/bin/htpasswd %s",(const char *)args.join(" ").toUtf8());
    proc=new QProcess(this);
    proc->start("/usr/bin/htpasswd",args);
    proc->waitForFinished();
    delete proc;
  }
}


void MainObject::DeleteUser(const QStringList &cmds)
{
  QProcess *proc=NULL;
  QStringList args;

  if(cmds.size()==3) {
    args.push_back("-D");
    args.push_back(cmds[1]);
    args.push_back(cmds[2]);
    proc=new QProcess(this);
    proc->start("/usr/bin/htpasswd",args);
    proc->waitForFinished();
    delete proc;
  }
}
