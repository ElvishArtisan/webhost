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

#include <QProcess>
#include <QStringList>

#include "webhostd.h"

void MainObject::Upgrade(const QStringList &cmds)
{
  QProcess *proc=NULL;
  QStringList args;
  QString action;

  if(cmds.size()>1) {
    args.push_back("--force");
    args.push_back("-U");
    for(int i=1;i<cmds.length();i++) {
      args.push_back(cmds[i]);
    }
    proc=new QProcess(this);
    proc->start("/bin/rpm",args);
    proc->waitForFinished();
  }
}
