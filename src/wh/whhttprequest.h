// whhttprequest.h
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

#ifndef WHHTTPREQUEST_H
#define WHHTTPREQUEST_H

#include <stdint.h>

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QStringList>

class WHHttpRequest
{
 public:
  enum Method {None=0,Get=1,Post=2};
  WHHttpRequest(Method meth,const QString &uri,
		const QStringList &hdr_names,const QStringList &hdr_values,
		const QByteArray &body=QByteArray());
  WHHttpRequest();
  unsigned majorProtocolVersion() const;
  unsigned minorProtocolVersion() const;
  bool setProtocolVersion(const QString &str);
  Method method() const;
  void setMethod(Method meth);
  QString uri() const;
  void setUri(const QString &uri);
  QString hostName() const;
  uint16_t hostPort() const;
  bool setHost(const QString &str);
  int64_t contentLength() const;
  void setContentLength(int64_t len);
  QString contentType() const;
  void setContentType(const QString &mimetype);
  QString referrer() const;
  void setReferrer(const QString &str);
  QString userAgent() const;
  void setUserAgent(const QString &str);
  QStringList headerNames() const;
  QStringList headerValues() const;
  QString headerValue(const QString &name) const;
  void addHeader(const QString &name,const QString &value);
  QByteArray body() const;
  void appendBody(const QByteArray &data);
  QString dump() const;
  static QString statusText(int stat_code);
  static int timezoneOffset();
  static QString datetimeStamp(const QDateTime &dt);

 private:
  Method http_method;
  unsigned http_major_protocol_version;
  unsigned http_minor_protocol_version;
  QString http_uri;
  QString http_host_name;
  uint16_t http_host_port;
  int64_t http_content_length;
  QString http_content_type;
  QString http_referrer;
  QString http_user_agent;
  QStringList http_header_names;
  QStringList http_header_values;
  QByteArray http_body;
};


#endif  // WHHTTPREQUEST_H
