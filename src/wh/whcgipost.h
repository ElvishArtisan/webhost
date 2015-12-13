// whcgipost.h
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

#ifndef WHCGIPOST_H
#define WHCGIPOST_H

#include <QHostAddress>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUdpSocket>

#include <wh/whsettings.h>

class WHCgiPost
{
 public:
  enum Encoding {UrlEncoded=0,MultipartEncoded=1,AutoEncoded=2};
  enum Error {ErrorOk=0,ErrorNotPost=1,ErrorNoTempDir=2,ErrorMalformedData=3,
	      ErrorPostTooLarge=4,ErrorInternal=5,ErrorNotInitialized=6};
  WHCgiPost(unsigned maxsize=0,bool auto_delete=true);
  ~WHCgiPost();
  Error error() const;
  QStringList names() const;
  QVariant value(const QString &name,bool *ok=NULL);
  bool getValue(const QString &name,QHostAddress *addr);
  bool getValue(const QString &name,QString *str);
  bool getValue(const QString &name,int *n);
  bool getValue(const QString &name,long *n);
  bool getValue(const QString &name,bool *n);
  bool isFile(const QString &name);
  QString tempDir() const;
  WHSettings *settings();
  void sendIpCommand(const QHostAddress &addr,const QHostAddress &mask,
		     const QHostAddress &gw,const QHostAddress &dns1,
		     const QHostAddress &dns2) const;
  void sendNtpCommand(const QHostAddress &ntp1,const QHostAddress &ntp2) const;
  void sendRebootCommand() const;
  void sendRestartCommand(const QString &sysname) const;
  void sendUpgradeCommand(const QString &filename) const;
  QString dump();
  static QString errorString(Error err);
  static QString dumpEnvironment();

 private:
  void SendCommand(const QString &cmd) const;
  void LoadUrlEncoding(char first);
  void LoadMultipartEncoding(char first);
  QString UrlDecode(const QString &str) const;
  Encoding post_encoding;
  Error post_error;
  std::map<QString,QVariant> post_values;
  std::map<QString,bool> post_filenames;
  QString post_tempdir;
  bool post_auto_delete;
  unsigned post_content_length;
  WHSettings *post_settings;
  QUdpSocket *post_socket;
};


#endif  // WHCGIPOST_H
