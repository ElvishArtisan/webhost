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

#include <wh/whprofile.h>

#include "config.h"

Config::Config()
{
  config_control_port=WEBHOST_DEFAULT_CONTROL_PORT;
  config_ntp_configuration_file=WEBHOSTD_DEFAULT_NTP_CONF_FILE;
  config_service_command="";
  config_service_respawn_delay=WEBHOSTD_DEFAULT_SERVICE_RESPAWN_DELAY;
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


QString Config::serviceCommand() const
{
  return config_service_command;
}


int Config::serviceRespawnDelay() const
{
  return config_service_respawn_delay;
}


void Config::load()
{
  WHProfile *p=new WHProfile();

  p->setSource(WEBHOST_CONF_FILE);
  config_control_port=p->
    intValue("Webhost","ControlPort",WEBHOST_DEFAULT_CONTROL_PORT);
  config_ntp_configuration_file=
    p->stringValue("Webhost","NtpConfigurationFile",
		   WEBHOSTD_DEFAULT_NTP_CONF_FILE);
  config_ntp_service_name=
    p->stringValue("Webhost","NtpServiceName",WEBHOSTD_DEFAULT_NTP_SERVICE_NAME);
  config_service_command=p->stringValue("Webhost","ServiceCommand");
  config_service_respawn_delay=
    p->intValue("Webhost","ServiceRespawnDelay",
		WEBHOSTD_DEFAULT_SERVICE_RESPAWN_DELAY);

  delete p;
}
