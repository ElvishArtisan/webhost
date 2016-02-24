// whhttprequest.cpp
//
// Abstract an HTTP client request
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

#include <QStringList>

#include "whhttprequest.h"

WHHttpRequest::WHHttpRequest(Method meth,const QString &uri,
			     const QStringList &hdr_names,
			     const QStringList &hdr_values,
			     const QByteArray &body)
{
  http_method=meth;
  http_host_port=0;
  http_uri=uri;
  http_content_length=0;
  http_content_type="application/octet-stream";
  http_header_names=hdr_names;
  http_header_values=hdr_values;
  http_body=body;
  http_auth_type=WHHttpRequest::AuthNone;
  http_major_protocol_version=0;
  http_minor_protocol_version=0;
}


WHHttpRequest::WHHttpRequest()
{
  http_method=WHHttpRequest::None;
  http_major_protocol_version=0;
  http_minor_protocol_version=0;
  http_host_port=0;
  http_auth_type=WHHttpRequest::AuthNone;
  http_content_length=0;
  http_content_type="application/octet-stream";
}


unsigned WHHttpRequest::majorProtocolVersion() const
{
  return http_major_protocol_version;
}


unsigned WHHttpRequest::minorProtocolVersion() const
{
  return http_minor_protocol_version;
}


bool WHHttpRequest::setProtocolVersion(const QString &str)
{
  QStringList f0=str.split(".");
  bool ok=false;

  if(f0.size()!=2) {
    return false;
  }
  http_major_protocol_version=f0[0].toInt(&ok);
  if(!ok) {
    return false;
  }
  http_minor_protocol_version=f0[1].toInt(&ok);
  if(!ok) {
    return false;
  }
  return true;
}


WHHttpRequest::Method WHHttpRequest::method() const
{
  return http_method;
}


void WHHttpRequest::setMethod(Method meth)
{
  http_method=meth;
}


QString WHHttpRequest::uri() const
{
  return http_uri;
}


void WHHttpRequest::setUri(const QString &uri)
{
  http_uri=uri;
}


QString WHHttpRequest::hostName() const
{
  return http_host_name;
}


uint16_t WHHttpRequest::hostPort() const
{
  return http_host_port;
}


bool WHHttpRequest::setHost(const QString &str)
{
  QStringList f0=str.split(":");
  bool ok=false;

  if(f0.size()>2) {
    return false;
  }
  http_host_name=f0[0];
  if(f0.size()==2) {
    http_host_port=f0[1].toUInt(&ok);
    if((!ok)||(http_host_port==0)) {
      return false;
    }
  }
  else {
    http_host_port=80;
  }
  return true;
}


WHHttpRequest::AuthType WHHttpRequest::authType() const
{
  return http_auth_type;
}


QString WHHttpRequest::authName() const
{
  return http_auth_name;
}


QString WHHttpRequest::authPassword() const
{
  return http_auth_password;
}


bool WHHttpRequest::setAuthorization(const QString &str)
{
  bool ret=false;

  QStringList f0=str.trimmed().split(" ");
  if((f0[0].toLower()=="basic")&&(f0.size()==2)) {
    QStringList f1=QString(QByteArray::fromBase64(f0[1].toAscii())).
      split(":",QString::KeepEmptyParts);
    if(f1.size()>=2) {
      http_auth_name=f1[0];
      f1.erase(f1.begin());
      http_auth_password=f1.join(":");
      ret=true;
    }
  }

  return ret;
}


int64_t WHHttpRequest::contentLength() const
{
  return http_content_length;
}


void WHHttpRequest::setContentLength(int64_t len)
{
  http_content_length=len;
}


QString WHHttpRequest::contentType() const
{
  return http_content_type;
}


void WHHttpRequest::setContentType(const QString &mimetype)
{
  http_content_type=mimetype;
}


QString WHHttpRequest::referrer() const
{
  return http_referrer;
}


void WHHttpRequest::setReferrer(const QString &str)
{
  http_referrer=str;
}


QString WHHttpRequest::userAgent() const
{
  return http_user_agent;
}


void WHHttpRequest::setUserAgent(const QString &str)
{
  http_user_agent=str;
}


QStringList WHHttpRequest::headerNames() const
{
  return http_header_names;
}


QStringList WHHttpRequest::headerValues() const
{
  return http_header_values;
}


QString WHHttpRequest::headerValue(const QString &name) const
{
  for(int i=0;i<http_header_names.size();i++) {
    if(http_header_names[i]==name) {
      return http_header_values[i];
    }
  }

  return QString();
}


void WHHttpRequest::addHeader(const QString &name,const QString &value)
{
  http_header_names.push_back(name);
  http_header_values.push_back(value);
}


QByteArray WHHttpRequest::body() const
{
  return http_body;
}


void WHHttpRequest::appendBody(const QByteArray &data)
{
  http_body.append(data);
}


QString WHHttpRequest::dump() const
{
  QString ret="";

  switch(method()) {
  case WHHttpRequest::Get:
    ret+="Method: GET\n";
    break;

  case WHHttpRequest::None:
    ret+="Method: [none]\n";
    break;

  default:
    ret+="Method: Unknown\n";
    break;
  }
  ret+=QString().sprintf("HTTP Protocol: %u.%u\n",majorProtocolVersion(),
			 minorProtocolVersion());
  ret+="URI: "+uri()+"\n";
  ret+="Host: "+hostName()+":"+QString().sprintf("%u",hostPort())+"\n";
  ret+="User-Agent: "+userAgent()+"\n";

  ret+="HEADERS\n";
  for(int i=0;i<http_header_names.size();i++) {
    ret+="  "+http_header_names[i]+": "+http_header_values[i]+"\n";
  }
  return ret;
}


QString WHHttpRequest::statusText(int stat_code)
{
  //
  // From RFC 2616 Section 10
  //
  QString ret="Unknown";

  if((stat_code>=100)&&(stat_code<200)) {
    ret="Continue";
  }
  if((stat_code>=200)&&(stat_code<300)) {
    ret="OK";
  }
  if((stat_code>=300)&&(stat_code<400)) {
    ret="Multiple Choices";
  }
  if((stat_code>=400)&&(stat_code<500)) {
    ret="Bad Request";
  }
  if((stat_code>=500)&&(stat_code<600)) {
    ret="Internal Server Error";
  }

  switch(stat_code) {
  case 100:
    ret="Continue";
    break;

  case 101:
    ret="Switching Protocols";
    break;

  case 200:
    ret="OK";
    break;

  case 201:
    ret="Created";
    break;

  case 202:
    ret="Accepted";
    break;

  case 203:
    ret="Non-Authoritative Information";
    break;

  case 204:
    ret="No Content";
    break;

  case 205:
    ret="Reset Content";
    break;

  case 206:
    ret="Partial Content";
    break;

  case 300:
    ret="Multiple Choices";
    break;

  case 301:
    ret="Moved Permanently";
    break;

  case 302:
    ret="Found";
    break;

  case 303:
    ret="See Other";
    break;

  case 304:
    ret="Not Modified";
    break;

  case 305:
    ret="Use Proxy";
    break;

  case 306:
    ret="(Unused)";
    break;

  case 307:
    ret="Temporary Redirect";
    break;

  case 400:
    ret="Bad Request";
    break;

  case 401:
    ret="Unauthorized";
    break;

  case 402:
    ret="Payment Required";
    break;

  case 403:
    ret="Forbidden";
    break;

  case 404:
    ret="Not Found";
    break;

  case 405:
    ret="Method Not Allowed";
    break;

  case 406:
    ret="Not Acceptable";
    break;

  case 407:
    ret="Proxy Authentication Required";
    break;

  case 408:
    ret="Request Timeout";
    break;

  case 409:
    ret="Conflict";
    break;

  case 410:
    ret="Gone";
    break;

  case 411:
    ret="Length Required";
    break;

  case 412:
    ret="Precondition Failed";
    break;

  case 413:
    ret="Request Entity Too Large";
    break;

  case 414:
    ret="Request-URI Too Long";
    break;

  case 415:
    ret="Unsupported Media Type";
    break;

  case 416:
    ret="Requested Range Not Satisfiable";
    break;

  case 417:
    ret="Expectation Failed";
    break;

  case 500:
    ret="Internal Server Error";
    break;

  case 501:
    ret="Not Implemented";
    break;

  case 502:
    ret="Bad Gateway";
    break;

  case 503:
    ret="Service Unavailable";
    break;

  case 504:
    ret="Gateway Timeout";
    break;

  case 505:
    ret="HTTP Version Not Supported";
    break;
  }
  return ret;
}


int WHHttpRequest::timezoneOffset()
{
  time_t t=time(&t);
  struct tm *tm=localtime(&t);
  time_t local_time=3600*tm->tm_hour+60*tm->tm_min+tm->tm_sec;
  tm=gmtime(&t);
  time_t gmt_time=3600*tm->tm_hour+60*tm->tm_min+tm->tm_sec;

  return gmt_time-local_time;
}


QString WHHttpRequest::datetimeStamp(const QDateTime &dt)
{
  return dt.addSecs(WHHttpRequest::timezoneOffset()).
    toString("ddd, dd MMM yyyy hh:mm:ss")+" GMT";
}
