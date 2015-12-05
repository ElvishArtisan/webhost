// config.cpp
//
// webhostd(8) configuration
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

#include "config.h"
#include "profile.h"

Config::Config()
{
  config_control_port=WEBHOST_DEFAULT_CONTROL_PORT;
  config_ntp_configuration_file=WEBHOST_DEFAULT_NTP_CONF_FILE;
}


uint16_t Config::controlPort() const
{
  return config_control_port;
}


QString Config::ntpConfigurationFile() const
{
  return config_ntp_configuration_file;
}


QString Config::ntpServiceName() const
{
  return config_ntp_service_name;
}


void Config::load()
{
  Profile *p=new Profile();

  p->setSource(WEBHOST_CONF_FILE);
  config_control_port=p->
    intValue("Webhost","ControlPort",WEBHOST_DEFAULT_CONTROL_PORT);
  config_ntp_configuration_file=
    p->stringValue("Webhost","NtpConfigurationFile",
		   WEBHOST_DEFAULT_NTP_CONF_FILE);
  config_ntp_service_name=
    p->stringValue("Webhost","NtpServiceName",WEBHOST_DEFAULT_NTP_SERVICE_NAME);

  delete p;
}
