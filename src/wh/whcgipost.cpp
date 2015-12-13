// whcgipost.cpp
//
// POST data processor class for CGI applications
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <QVariant>

#include "whcgipost.h"

WHCgiPost::WHCgiPost(unsigned maxsize,bool auto_delete)
{
  char tempdir[PATH_MAX];
  bool ok=false;

  post_encoding=WHCgiPost::AutoEncoded;
  post_error=WHCgiPost::ErrorNotInitialized;
  post_auto_delete=auto_delete;

  //
  // Verify Transfer Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    post_error=WHCgiPost::ErrorNotPost;
    return;
  }
  if(QString(getenv("REQUEST_METHOD")).toLower()!="post") {
    post_error=WHCgiPost::ErrorNotPost;
    return;
  }

  //
  // Verify Size
  //
  if(getenv("CONTENT_LENGTH")==NULL) {
    post_error=WHCgiPost::ErrorPostTooLarge;
    return;
  }
  post_content_length=QString(getenv("CONTENT_LENGTH")).toUInt(&ok);
  if((!ok)||((maxsize>0)&&(post_content_length>maxsize))) {
    post_error=WHCgiPost::ErrorPostTooLarge;
    return;
  }

  //
  // Initialize Temp Directory Path
  //
  if(getenv("TMPDIR")!=NULL) {
    strcpy(tempdir,getenv("TMPDIR"));
  }
  else {
    strcpy(tempdir,"/tmp");
  }
  strcat(tempdir,"/lwrouterXXXXXX");
  post_tempdir=mkdtemp(tempdir);
  if(post_tempdir.isNull()) {
    post_error=WHCgiPost::ErrorNoTempDir;
    return;
  }

  //
  // Autodetect the encoding type
  //
  char first[2];
  read(0,first,1);
  if(first[0]=='-') {
    post_encoding=WHCgiPost::MultipartEncoded;
  }
  else {
    post_encoding=WHCgiPost::UrlEncoded;
  }

  switch(post_encoding) {
  case WHCgiPost::UrlEncoded:
    LoadUrlEncoding(first[0]);
    break;

  case WHCgiPost::MultipartEncoded:
    LoadMultipartEncoding(first[0]);
    break;

  case WHCgiPost::AutoEncoded:
    break;
  }
}


WHCgiPost::~WHCgiPost()
{
  if(post_auto_delete) {
    for(std::map<QString,bool>::const_iterator ci=post_filenames.begin();
	ci!=post_filenames.end();ci++) {
      if(ci->second) {
	unlink(post_values[ci->first].toString().toAscii());
      }
    }
    if(!post_tempdir.isNull()) {
      rmdir(post_tempdir.toAscii());
    }
  }
}


WHCgiPost::Error WHCgiPost::error() const
{
  return post_error;
}


QStringList WHCgiPost::names() const
{
  QStringList list;
  for(std::map<QString,QVariant>::const_iterator ci=post_values.begin();
      ci!=post_values.end();ci++) {
    list.push_back(ci->first);
  }
  return list;
}


QVariant WHCgiPost::value(const QString &name,bool *ok)
{
  QVariant v;
  if(post_values.count(name)>0) {
    v=post_values[name];
  }
  if(ok!=NULL) {
    *ok=(post_values.count(name)>0);
  }
  return v;
}


bool WHCgiPost::getValue(const QString &name,QHostAddress *addr)
{
  QString str;
  bool ok=getValue(name,&str);
  if(!ok) {
    return false;
  }
  addr->setAddress(str);
  return true;
}


bool WHCgiPost::getValue(const QString &name,QString *str)
{
  if(post_values.count(name)>0) {
    *str=post_values[name].toString();
    return true;
  }
  return false;
}


bool WHCgiPost::getValue(const QString &name,int *n)
{
  if(post_values.count(name)>0) {
    *n=post_values[name].toInt();
    return true;
  }
  return false;
}


bool WHCgiPost::getValue(const QString &name,long *n)
{
  if(post_values.count(name)>0) {
    *n=post_values[name].toLongLong();
    return true;
  }
  *n=0;
  return false;
}


bool WHCgiPost::getValue(const QString &name,bool *n)
{
  if(post_values.count(name)>0) {
    *n=post_values[name].toBool();
    return true;
  }
  *n=false;
  return false;
}


bool WHCgiPost::isFile(const QString &name)
{
  return post_filenames[name];
}


QString WHCgiPost::tempDir() const
{
  return post_tempdir;
}


QString WHCgiPost::dump()
{
  QString ret;

  ret+=QString().sprintf("Content-type: text/html\n\n");
  ret+=QString().
    sprintf("<table cellpadding=\"5\" cellspacing=\"0\" border=\"1\">\n");
  ret+=QString().sprintf("<tr>\n");
  ret+=QString().
    sprintf("<td colspan=\"3\" align=\"center\"><strong>WHCgiPost Data Dump</strong></td>\n");
  ret+=QString().sprintf("</tr>\n");

  ret+=QString().sprintf("<tr>\n");
  ret+=QString().sprintf("<th align=\"center\">NAME</th>\n");
  ret+=QString().sprintf("<th align=\"center\">VALUE</th>\n");
  ret+=QString().sprintf("<th align=\"center\">FILE</th>\n");
  ret+=QString().sprintf("</tr>\n");
  
  for(std::map<QString,QVariant>::const_iterator ci=post_values.begin();
      ci!=post_values.end();ci++) {
    ret+=QString().sprintf("<tr>\n");
    ret+=QString().sprintf("<td align=\"left\">|%s|</td>\n",
			   (const char *)ci->first.toAscii());
    ret+=QString().sprintf("<td align=\"left\">|%s|</td>\n",
	   (const char *)ci->second.toString().toAscii());
    if(post_filenames[ci->first]) {
      ret+=QString().sprintf("<td align=\"center\">Yes</td>\n");
    }
    else {
      ret+=QString().sprintf("<td align=\"center\">No</td>\n");
    }
    ret+=QString().sprintf("</tr>\n");
  }

  ret+=QString().sprintf("</table>\n");
  return ret;
}


QString WHCgiPost::errorString(WHCgiPost::Error err)
{
  QString str="Unknown error";

  switch(err) {
  case WHCgiPost::ErrorOk:
    str="OK";
    break;

  case WHCgiPost::ErrorNotPost:
    str="Request is not POST";
    break;

  case WHCgiPost::ErrorNoTempDir:
    str="Unable to create temporary directory";
    break;

  case WHCgiPost::ErrorMalformedData:
    str="The data is malformed";
    break;

  case WHCgiPost::ErrorPostTooLarge:
    str="POST is too large";
    break;

  case WHCgiPost::ErrorInternal:
    str="Internal error";
    break;

  case WHCgiPost::ErrorNotInitialized:
    str="POST class not initialized";
    break;
  }
  return str;
}


QString WHCgiPost::dumpEnvironment()
{
  QString ret;
  unsigned i=0;

  while(environ[i]!=NULL) {
    ret+=environ[i];
    ret+="\n";
    i++;
  }
  return ret;
}


void WHCgiPost::LoadUrlEncoding(char first)
{
  char *data=new char[post_content_length+1];
  int n;
  QStringList lines;
  QStringList line;

  data[0]=first;
  if((n=read(0,data+1,post_content_length-1))<0) {
    post_error=WHCgiPost::ErrorMalformedData;
    return;
  }
  data[post_content_length]=0;
  lines=QString(data).split("&");
  for(int i=0;i<lines.size();i++) {
    line=lines[i].split("=");
    if(line.size()==2) {
      post_values[line[0]]=UrlDecode(line[1]);
      post_filenames[line[0]]=false;
    }
  }

  post_error=WHCgiPost::ErrorOk;
  delete data;
}


void WHCgiPost::LoadMultipartEncoding(char first)
{
  std::map<QString,QString> headers;
  bool header=true;
  char *data=NULL;
  FILE *f=NULL;
  ssize_t n=0;
  QString sep;
  QString name;
  QString filename;
  QString tempdir;
  int fd=-1;
  int index;

  if((f=fdopen(0,"r"))==NULL) {
    post_error=WHCgiPost::ErrorInternal;
    return;
  }
  if((n=getline(&data,(size_t *)&n,f))<=0) {
    post_error=WHCgiPost::ErrorMalformedData;
    return;
  }
  sep=QString(data).simplified();

  //
  // Get message parts
  //
  while((n=getline(&data,(size_t *)&n,f))>0) {
    if(QString(data).simplified().contains(sep)>0) {  // End of part
      if(fd>=0) {
	ftruncate(fd,lseek(fd,0,SEEK_CUR)-2);  // Remove extraneous final CR/LF
	::close(fd);
	fd=-1;
      }
      name="";
      filename="";
      headers.clear();
      header=true;
      continue;
    }
    if(header) {  // Read header
      if(QString(data).simplified().isEmpty()) {
	if(!headers["content-disposition"].isNull()) {
	  QStringList fields;
	  fields=headers["content-disposition"].split(";");
	  if(fields.size()>0) {
	    if(fields[0].toLower().simplified()=="form-data") {
	      for(int i=1;i<fields.size();i++) {
		QStringList pairs;
		pairs=fields[i].split("=");
		if(pairs[0].toLower().simplified()=="name") {
		  name=pairs[1].simplified();
		  name.replace("\"","");
		}
		if(pairs[0].toLower().simplified()=="filename") {
		  if((index=pairs[1].simplified().lastIndexOf("\\"))<0) {
		    index=0;
		  }
		  filename=post_tempdir+"/"+pairs[1].simplified().
		    right(pairs[1].simplified().length()-index-1);
		  filename.replace("\"","");
		  fd=open(filename.toAscii(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
		}
	      }
	    }
	  }
	}
	header=false;
      }
      else {
	QStringList hdr;
	hdr=QString(data).simplified().split(":");
	// Reconcaternate trailing sections so we don't split on the 
	// useless M$ drive letter supplied by IE
	for(int i=2;i<hdr.size();i++) {
	  hdr[1]+=hdr[i];
	}
	headers[hdr[0].toLower()]=hdr[1];
      }
    }
    else {  // Read data
      if(filename.isEmpty()) {
	QString str=post_values[name].toString();
	str+=QString(data);
	post_filenames[name]=false;
	post_values[name]=str.simplified();
      }
      else {
	post_filenames[name]=true;
	post_values[name]=filename;
	write(fd,data,n);
      }
    }
  }
  free(data);
  post_error=WHCgiPost::ErrorOk;
}


QString WHCgiPost::UrlDecode(const QString &str) const
{
  int istate=0;
  unsigned n;
  QString code;
  QString ret;
  bool ok=false;

  for(int i=0;i<str.length();i++) {
    switch(istate) {
    case 0:
      if(str.at(i)==QChar('+')) {
	ret+=" ";
      }
      else {
	if(str.at(i)==QChar('%')) {
	  istate=1;
	}
	else {
	  ret+=str.at(i);
	}
      }
      break;

    case 1:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code=str.mid(i,1);
      istate=2;
      break;

    case 2:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code+=str.mid(i,1);
      ret+=QChar(code.toInt(&ok,16));
      istate=0;
      break;
    }
  }

  return ret;
}
