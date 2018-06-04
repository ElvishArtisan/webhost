// whwificonnection.cpp
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

#include "whwificonnection.h"

WHWifiConnection::WHWifiConnection()
{
  wifi_ssid="unknown";
  wifi_in_use=false;
  wifi_mode="unknown";
  wifi_channel=0;
  wifi_rate="unknown";
  wifi_signal=0;
  wifi_bars="";
  wifi_security="unknown";
}


QString WHWifiConnection::ssid() const
{
  return wifi_ssid;
}


void WHWifiConnection::setSsid(const QString &str)
{
  wifi_ssid=str;
}


bool WHWifiConnection::inUse() const
{
  return wifi_in_use;
}


void WHWifiConnection::setInUse(bool state)
{
  wifi_in_use=state;
}


QString WHWifiConnection::mode() const
{
  return wifi_mode;
}


void WHWifiConnection::setMode(const QString &str)
{
  wifi_mode=str;
}


int WHWifiConnection::channel() const
{
  return wifi_channel;
}


void WHWifiConnection::setChannel(int chan)
{
  wifi_channel=chan;
}


QString WHWifiConnection::rate() const
{
  return wifi_rate;
}


void WHWifiConnection::setRate(const QString &str)
{
  wifi_rate=str;
}


int WHWifiConnection::signal() const
{
  return wifi_signal;
}


void WHWifiConnection::setSignal(int sig)
{
  wifi_signal=sig;
}


QString WHWifiConnection::bars() const
{
  return wifi_bars;
}


void WHWifiConnection::setBars(const QString &str)
{
  wifi_bars=str;
}


QString WHWifiConnection::security() const
{
  return wifi_security;
}


void WHWifiConnection::setSecurity(const QString &str)
{
  wifi_security=str;
}
