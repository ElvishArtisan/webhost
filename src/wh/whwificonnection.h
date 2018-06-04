// whwificonnection.h
//
// Wifi connection state.
//
// (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WHWIFICONNECTION_H
#define WHWIFICONNECTION_H

#include <stdint.h>

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include <wh/whsocketmessage.h>

class WHWifiConnection : public QObject
{
 public:
  WHWifiConnection();
  QString ssid() const;
  void setSsid(const QString &str);
  bool inUse() const;
  void setInUse(bool state);
  QString mode() const;
  void setMode(const QString &str);
  int channel() const;
  void setChannel(int chan);
  QString rate() const;
  void setRate(const QString &str);
  int signal() const;
  void setSignal(int sig);
  QString bars() const;
  void setBars(const QString &str);
  QString security() const;
  void setSecurity(const QString &str);

 private:
  QString wifi_ssid;
  bool wifi_in_use;
  QString wifi_mode;
  int wifi_channel;
  QString wifi_rate;
  int wifi_signal;
  QString wifi_bars;
  QString wifi_security;
};


#endif  // WHWIFICONNECTION_H
