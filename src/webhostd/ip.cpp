// ip.cpp
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

#include <errno.h>

#include <QHostAddress>
#include <QProcess>
#include <QStringList>

#include "webhostd.h"

void MainObject::Ip(const QStringList &cmds)
{
  QStringList params;
  QStringList values;
  FILE *f=NULL;
  char line[1024];
  QString ifcfg_file="/etc/sysconfig/network-scripts/ifcfg-"+cmds[1];
  QProcess *proc=NULL;
  QStringList args;

  //
  // Read Current Config
  //
  if(cmds.size()==7) {
    if((f=fopen(ifcfg_file.toUtf8(),"r"))
       !=NULL) {
      while(fgets(line,1024,f)!=NULL) {
	QStringList f0=QString(line).trimmed().split("=");
	if(f0.size()==2) {
	  params.push_back(f0[0]);
	  values.push_back(f0[1]);
	}
      }
      fclose(f);

      for(int i=0;i<params.size();i++) {
	if(params[i]=="IPADDR") {
	  values[i]=cmds[2];
	}
	if(params[i]=="NETMASK") {
	  values[i]=cmds[3];
	}
	if(params[i]=="GATEWAY") {
	  values[i]=cmds[4];
	}
	if(params[i]=="DNS1") {
	  if(!QHostAddress(cmds[5]).isNull()) {
	    values[i]=cmds[5];
	  }
	}
	if(params[i]=="DNS2") {
	  if(!QHostAddress(cmds[6]).isNull()) {
	    values[i]=cmds[6];
	  }
	}
      }
    }

    //
    // Save new configuration
    //
    if((f=fopen((ifcfg_file+".back").toUtf8(),"w"))
       !=NULL) {
      for(int i=0;i<params.size();i++) {
	fprintf(f,"%s=%s\n",(const char *)params[i].toUtf8(),
		(const char *)values[i].toUtf8());
      }      
      fclose(f);
      rename((ifcfg_file+".back").toUtf8(),ifcfg_file.toUtf8());

      //
      // Apply new configuration
      //
      args.push_back("network");
      args.push_back("restart");
      proc=new QProcess(this);
      proc->start("service",args);
      proc->waitForFinished();
      delete proc;
    }
    else {
      fprintf(stderr,"unable to open interface configuration [%s]\n",
	      strerror(errno));
    }
  }
}
