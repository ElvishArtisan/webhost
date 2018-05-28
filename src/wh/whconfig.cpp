// whconfig.cpp
//
// Class for reading webhost configuration.
//
//   (C) Copyright 2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <wh/whsettings.h>

#include "whconfig.h"

WHConfig::WHConfig()
{
}


uint16_t WHConfig::controlPort() const
{
  return config_control_port;
}


bool WHConfig::useNetworkManager() const
{
  return config_use_network_manager;
}


unsigned WHConfig::interfaceQuantity() const
{
  return config_interface_names.size();
}


QString WHConfig::interfaceName(unsigned n) const
{
  if(n>=config_interface_names.size()) {
    return QString();
  }
  return config_interface_names[n];
}


QString WHConfig::ntpConfigurationFile() const
{
  return config_ntp_configuration_file;
}


QString WHConfig::ntpServiceName() const
{
  return config_ntp_service_name;
}


QString WHConfig::serviceCommand() const
{
  return config_service_command;
}


int WHConfig::serviceRespawnDelay() const
{
  return config_service_respawn_delay;
}


bool WHConfig::load()
{
  QString iface;
  int inum=0;
  bool ok=false;
  WHProfile *p=new WHProfile();

  p->setSource(WEBHOST_CONF_FILE);
  iface=p->stringValue("Webhost",
		       QString().sprintf("NetworkInterface%d",inum+1),"",&ok);
  while(ok) {
    config_interface_names.push_back(iface);
    inum++;
    iface=p->stringValue("Webhost",
			 QString().sprintf("NetworkInterface%d",inum+1),"",&ok);
  }
  config_control_port=
    p->intValue("Webhost","ControlPort",WEBHOST_DEFAULT_CONTROL_PORT);
  config_use_network_manager=p->intValue("Webhost","UseNetworkManager",true);
  config_ntp_configuration_file=
    p->stringValue("Webhost","NtpConfigurationFile","/etc/ntp.conf");
  config_ntp_service_name=p->stringValue("Webhost","NtpServiceName","ntpd");
  config_service_command=p->stringValue("Webhost","ServiceCommand");
  config_service_respawn_delay=p->intValue("Webhost","ServiceRespawnDelay",100);

  delete p;

  return true;
}
