// whhttpconnection.cpp
//
// HTTP connection state for WHHttpServer
//
// (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <time.h>
#include <unistd.h>

#include <QDateTime>
#include <QProcessEnvironment>

#include "whhttpserver.h"

WHHttpConnection::WHHttpConnection(QTcpSocket *sock,QObject *parent)
  : QObject(parent)
{
  conn_socket=sock;
  conn_request=new WHHttpRequest();
  conn_cgi_process=NULL;
  conn_cgi_headers_active=true;
  conn_parse_state=0;
}


WHHttpConnection::~WHHttpConnection()
{
  if(conn_cgi_process!=NULL) {
    delete conn_cgi_process;
  }
  delete conn_request;
  delete conn_socket;
}


WHHttpRequest *WHHttpConnection::request()
{
  return conn_request;
}


void WHHttpConnection::startCgiScript(const QString &filename)
{
  conn_cgi_process=new QProcess(this);
  connect(conn_cgi_process,SIGNAL(started()),this,SLOT(cgiStartedData()));
  connect(conn_cgi_process,SIGNAL(readyReadStandardOutput()),
	  this,SLOT(cgiReadyReadData()));
  connect(conn_cgi_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	  this,SLOT(cgiFinishedData(int,QProcess::ExitStatus)));
  connect(conn_cgi_process,SIGNAL(error(QProcess::ProcessError)),
	  this,SLOT(cgiErrorData(QProcess::ProcessError)));
  QProcessEnvironment env;
  if(request()->contentLength()>=0) {
    env.insert("CONTENT_LENGTH",
	       QString().sprintf("%ld",request()->contentLength()));
    env.insert("CONTENT_TYPE",request()->contentType());
  }
  env.insert("GATEWAY_INTERFACE","1.1");
  env.insert("HTTP_HOST",socket()->localAddress().toString());
  if(!request()->referrer().isEmpty()) {
    env.insert("HTTP_REFERER",request()->referrer());
  }
  if(!request()->userAgent().isEmpty()) {
    env.insert("HTTP_USER_AGENT",request()->userAgent());
  }
  env.insert("REMOTE_ADDR",socket()->peerAddress().toString());
  env.insert("REMOTE_HOST",socket()->peerAddress().toString());
  env.insert("REMOTE_PORT",QString().sprintf("%u",0xFFFF&socket()->peerPort()));
  if(request()->method()==WHHttpRequest::Post) {
    env.insert("REQUEST_METHOD","POST");
  }
  else {
    env.insert("REQUEST_METHOD","GET");
  }
  env.insert("REQUEST_URI",request()->uri());
  env.insert("SCRIPT_FILENAME",filename);
  env.insert("SCRIPT_NAME",filename);
  env.insert("SERVER_ADMIN","noone@nowhere.com");
  env.insert("SERVER_NAME",socket()->localAddress().toString());
  env.insert("SERVER_PORT",
	     QString().sprintf("%u",0xFFFF&socket()->localPort()));
  env.insert("SERVER_SOFTWARE",QString("Webhost-")+VERSION);
  conn_cgi_process->setProcessEnvironment(env);
  conn_cgi_process->start(filename);
}


void WHHttpConnection::sendResponseHeader(int stat_code)
{
  QString statline=QString().sprintf("HTTP/1.1 %d ",stat_code)+
    WHHttpRequest::statusText(stat_code)+"\r\n";
  socket()->write(statline.toUtf8());
  sendHeader("Date",WHHttpRequest::
	 datetimeStamp(QDateTime(QDate::currentDate(),QTime::currentTime())));
  sendHeader("Server",QString("Webhost/")+VERSION);
  sendHeader("Connection","close");// FIXME: Implement persistent connections
}


void WHHttpConnection::sendResponse(int stat_code,
				      const QStringList &hdr_names,
				      const QStringList &hdr_values,
				      const QByteArray &body,
				      const QString &mimetype)
{
  sendResponseHeader(stat_code);
  if(body.length()>0) {
    sendHeader("Content-Length",QString().sprintf("%d",body.length()));
    sendHeader("Content-Type",mimetype);
  }
  for(int i=0;i<hdr_names.size();i++) {
    sendHeader(hdr_names[i],hdr_values[i]);
  }
  sendHeader();
  socket()->write(body);
}


void WHHttpConnection::sendResponse(int stat_code,
				      const QByteArray &body,
				      const QString &mimetype)
{
  sendResponse(stat_code,QStringList(),QStringList(),body,mimetype);
}


void WHHttpConnection::sendError(int stat_code,const QString &msg,
				   const QStringList &hdr_names,
				   const QStringList &hdr_values)
{
  QString err_text=msg;
  if(err_text.isEmpty()) {
    err_text=
      QString().sprintf("%d ",stat_code)+WHHttpRequest::statusText(stat_code);
  }
  sendResponse(stat_code,hdr_names,hdr_values,err_text.toUtf8());
}


QTcpSocket *WHHttpConnection::socket()
{
  return conn_socket;
}


void WHHttpConnection::cgiStartedData()
{
  if(request()->method()==WHHttpRequest::Post) {
    conn_cgi_process->write(request()->body());
    conn_cgi_process->write("\r\n");
  }
}


void WHHttpConnection::cgiReadyReadData()
{
  QByteArray data;

  if(conn_cgi_headers_active) {
    data=conn_cgi_process->readLine().trimmed();
    if(data.length()==0) {
      sendResponseHeader(200);
      for(int i=0;i<conn_cgi_headers.size();i++) {
	socket()->write((conn_cgi_headers[i]+"\r\n").toUtf8());
      }
      socket()->write("\r\n",2);
      conn_cgi_headers_active=false;
      while(conn_cgi_process->bytesAvailable()>0) {
	data=conn_cgi_process->read(1024);
	socket()->write(data);
      }
    }
    else {
      conn_cgi_headers.push_back(data);
    }
  }
  else {
    while(conn_cgi_process->bytesAvailable()>0) {
      data=conn_cgi_process->read(1024);
      socket()->write(data);
    }
  }
}


void WHHttpConnection::cgiFinishedData(int exit_code,
					 QProcess::ExitStatus status)
{
  if(status!=QProcess::NormalExit) {
    sendError(500,"CGI process crashed");
  }
  else {
    if(exit_code!=0) {
      sendError(500,QString().
	  sprintf("CGI process returned non-zero exit code [%d]",exit_code));
    }
    else {
      cgiReadyReadData();
    }
  }
  socket()->close();
  emit cgiFinished();
}


void WHHttpConnection::cgiErrorData(QProcess::ProcessError err)
{
  QString err_text=QString().sprintf("unknown process error %d",err);
  switch(err) {
  case QProcess::FailedToStart:
    err_text="failed to start";
    break;

  case QProcess::Crashed:
    err_text="crashed";
    break;

  case QProcess::Timedout:
    err_text="Timed out";
    break;

  case QProcess::WriteError:
    err_text="write error";
    break;

  case QProcess::ReadError:
    err_text="read error";
    break;

  case QProcess::UnknownError:
    err_text="unknown error";
    break;
  }
  sendError(500,"500 CGI process error ["+err_text+"]");
  socket()->close();
  emit cgiFinished();
}




void WHHttpConnection::sendHeader(const QString &name,const QString &value)
{
  if(name.isEmpty()&&value.isEmpty()) {
    socket()->write("\r\n");
  }
  else {
    QString line=name+": "+value+"\r\n";
    socket()->write(line.toUtf8());
  }
}


int WHHttpConnection::parseState() const
{
  return conn_parse_state;
}


void WHHttpConnection::nextParseState()
{
  conn_parse_state++;
}
