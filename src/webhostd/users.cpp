// users.cpp
//
//   User methods for http-basic users.
//
//   (C) Copyright 2016-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <QProcess>
#include <QStringList>

#include <wh5/whprofile.h>

#include "webhostd.h"

void MainObject::AddUser(const QStringList &cmds)
{
  QProcess *proc=NULL;
  QStringList args;

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


void MainObject::AddUser2(const QStringList &cmds)
{
  FILE *f=NULL;

  if(cmds.size()>=4) {
    QString filename=cmds[1];
    QString new_realm=cmds[2];
    QString new_name=cmds[3];
    QString new_passwd="";
    for(int i=4;i<cmds.size();i++) {
      new_passwd+=cmds[i]+" ";
    }
    if(!new_passwd.isEmpty()) {
      new_passwd=new_passwd.left(new_passwd.length()-1);
    }
    if((f=fopen((filename+".back").toUtf8(),"w"))!=NULL) {
      WHProfile *p=new WHProfile();
      p->setSource(filename);
      bool ok=false;
      int count=0;
      QString section=QString().sprintf("WebHostUser%d",count+1);
      QString realm=p->stringValue(section,"Realm","",&ok);
      bool used=false;
      while(ok) {
	QString name=p->stringValue(section,"Name");
	QString passwd=p->stringValue(section,"Password");
	if((realm==new_realm)&&(name==new_name)) {
	  passwd=new_passwd;
	  used=true;
	}
	fprintf(f,"[WebHostUser%d]\n",count+1);
	fprintf(f,"Realm=%s\n",(const char *)realm.toUtf8());
	fprintf(f,"Name=%s\n",(const char *)name.toUtf8());
	fprintf(f,"Password=%s\n",(const char *)passwd.toUtf8());
	count++;
	section=QString().sprintf("WebHostUser%d",count+1);
	realm=p->stringValue(section,"Realm","",&ok);
      }
      if(!used) {
	fprintf(f,"[WebHostUser%d]\n",count+1);
	fprintf(f,"Realm=%s\n",(const char *)new_realm.toUtf8());
	fprintf(f,"Name=%s\n",(const char *)new_name.toUtf8());
	fprintf(f,"Password=%s\n",(const char *)new_passwd.toUtf8());
      }
      delete p;
      fclose(f);
      rename((filename+".back").toUtf8(),filename.toUtf8());
      if(main_service_process!=NULL) {
	if(main_service_process->state()==QProcess::Running) {
	  kill(main_service_process->pid(),SIGHUP);
	}
      }
    }
  }
}


void MainObject::DeleteUser2(const QStringList &cmds)
{
  FILE *f=NULL;

  if(cmds.size()==4) {
    QString filename=cmds[1];
    QString new_realm=cmds[2];
    QString new_name=cmds[3];
    if((f=fopen((filename+".back").toUtf8(),"w"))!=NULL) {
      WHProfile *p=new WHProfile();
      p->setSource(filename);
      bool ok=false;
      int read_count=0;
      int write_count=0;
      QString section=QString().sprintf("WebHostUser%d",read_count+1);
      QString realm=p->stringValue(section,"Realm","",&ok);
      while(ok) {
	QString name=p->stringValue(section,"Name");
	QString passwd=p->stringValue(section,"Password");
	if((realm!=new_realm)||(name!=new_name)) {
	  fprintf(f,"[WebHostUser%d]\n",write_count+1);
	  fprintf(f,"Realm=%s\n",(const char *)realm.toUtf8());
	  fprintf(f,"Name=%s\n",(const char *)name.toUtf8());
	  fprintf(f,"Password=%s\n",(const char *)passwd.toUtf8());
	  write_count++;
	}
	read_count++;
	section=QString().sprintf("WebHostUser%d",read_count+1);
	realm=p->stringValue(section,"Realm","",&ok);
      }
      delete p;
      fclose(f);
      rename((filename+".back").toUtf8(),filename.toUtf8());
      if(main_service_process!=NULL) {
	if(main_service_process->state()==QProcess::Running) {
	  kill(main_service_process->pid(),SIGHUP);
	}
      }
    }
  }
}
