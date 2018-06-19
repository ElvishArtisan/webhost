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
#include <syslog.h>

#include <QCoreApplication>
#include <QHostAddress>

#include <wh/whcmdswitch.h>

#include "webhostd.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  main_service_process=NULL;

  WHCmdSwitch *cmd=
    new WHCmdSwitch(qApp->argc(),qApp->argv(),VERSION,"webhost",WEBHOSTD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      syslog(LOG_ERR,"webhostd: unknown option \"%s\"\n",
	     (const char *)cmd->key(i).toUtf8());
      exit(256);
    }
  }

  //
  // Configuration
  //
  main_config=new WHConfig();
  main_config->load();

  //
  // Command Socket
  //
  main_command_socket=new QUdpSocket(this);
  if(!main_command_socket->bind(main_config->controlPort())) {
    syslog(LOG_ERR,"webhostd: cannot bind port %u\n",
	   main_config->controlPort());
    exit(256);
  }
  connect(main_command_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));

  //
  // Network Manager
  //
  QStringList args;
  args.push_back("--property=ActiveState");
  args.push_back("show");
  args.push_back("NetworkManager");
  QStringList f0=RunCommand("/bin/systemctl",args).split("\n");
  for(int i=0;i<f0.size();i++) {
    QStringList f1=f0.at(i).split("=");
    if(f1.size()==2) {
      if(f1.at(0)=="ActiveState") {
	if(main_config->useNetworkManager()&&(f1.at(1)=="inactive")) {
	  args.clear();
	  args.push_back("start");
	  args.push_back("NetworkManager");
	  RunCommand("/bin/systemctl",args);

	  args.clear();
	  args.push_back("enable");
	  args.push_back("NetworkManager");
	  RunCommand("/bin/systemctl",args);
	}
	if(!main_config->useNetworkManager()&&(f1.at(1)!="active")) {
	  args.clear();
	  args.push_back("stop");
	  args.push_back("NetworkManager");
	  RunCommand("/bin/systemctl",args);

	  args.clear();
	  args.push_back("disnable");
	  args.push_back("NetworkManager");
	  RunCommand("/bin/systemctl",args);
	}
      }
    }
  }

  //
  // Timers
  //
  main_garbage_timer=new QTimer(this);
  main_garbage_timer->setSingleShot(true);
  connect(main_garbage_timer,SIGNAL(timeout()),this,SLOT(garbageTimerData()));

  main_kill_timer=new QTimer(this);
  main_kill_timer->setSingleShot(true);
  connect(main_kill_timer,SIGNAL(timeout()),this,SLOT(killTimerData()));

  main_garbage_timer->start(0);
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


void MainObject::serviceErrorData(QProcess::ProcessError err)
{
  QString msg=tr("unknown error")+QString().sprintf(" [%u]",err);

  switch(err) {
  case QProcess::FailedToStart:
    msg=tr("failed to start");
    break;

  case QProcess::Crashed:
    msg=tr("crashed");
    break;

  case QProcess::Timedout:
    msg=tr("timed out");
    break;

  case QProcess::WriteError:
    msg=tr("write error");
    break;

  case QProcess::ReadError:
    msg=tr("read error");
    break;

  case QProcess::UnknownError:
    msg=tr("unknown error");
    break;
  }
  syslog(LOG_WARNING,"service process error: %s",(const char *)msg.toUtf8());
}


void MainObject::serviceFinishedData(int exit_code,QProcess::ExitStatus status)
{
  main_kill_timer->stop();

  if(status==QProcess::CrashExit) {
    fprintf(stderr,"service process crashed");
  }
  else {
    if(exit_code!=0) {
      fprintf(stderr,
	      "service process exited with non-zero exit code: %d [%s]\n",
	      exit_code,
	     (const char *)main_service_process->readAllStandardError().data());
    }
  }
  main_garbage_timer->start(main_config->serviceRespawnDelay());
}


void MainObject::garbageTimerData()
{
  //
  // Clean up old process
  //
  if(main_service_process!=NULL) {
    delete main_service_process;
    main_service_process=NULL;
  }

  //
  // Start new process
  //
  if(!main_config->serviceCommand().isEmpty()) {
    QStringList args;
    QStringList f0=main_config->serviceCommand().split(" ");
    for(int i=1;i<f0.size();i++) {
      args.push_back(f0[i]);
    }
    main_service_process=new QProcess(this);
    connect(main_service_process,SIGNAL(error(QProcess::ProcessError)),
	    this,SLOT(serviceErrorData(QProcess::ProcessError)));
    connect(main_service_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	    this,SLOT(serviceFinishedData(int,QProcess::ExitStatus)));
    main_service_process->start(f0[0],args);
  }
}


void MainObject::killTimerData()
{
  if(main_service_process!=NULL) {
    main_service_process->kill();
  }
}


void MainObject::ProcessCommand(const QString &cmd)
{
  QStringList cmds=cmd.split(" ");
  QString verb=cmds[0].toLower();

  if(verb=="adduser") {
    AddUser(cmds);
  }

  if(verb=="adduser2") {
    AddUser2(cmds);
  }

  if(verb=="deleteuser") {
    DeleteUser(cmds);
  }

  if(verb=="deleteuser2") {
    DeleteUser2(cmds);
  }

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

  if(verb=="wifi") {
    Wifi(cmds);
  }
}


QString MainObject::RunCommand(const QString &cmd,const QStringList &args)
{
  QString ret;

  QProcess *proc=new QProcess(this);
  proc->start(cmd,args);
  if(proc->waitForFinished()) {
    ret=proc->readAllStandardOutput();
  }
  else {
    syslog(LOG_WARNING,"command \"%s %s\" timed out",
	   (const char *)cmd.toUtf8(),(const char *)args.join(" ").toUtf8());
  }
  delete proc;

  return ret;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
