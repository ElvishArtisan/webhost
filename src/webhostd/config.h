// config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#include <QString>

#include <wh/whsettings.h>

#define WEBHOSTD_DEFAULT_NTP_CONF_FILE "/etc/ntp.conf"
#define WEBHOSTD_DEFAULT_NETWORK_INTERFACE "eth0"
#define WEBHOSTD_DEFAULT_NTP_SERVICE_NAME "ntpd"
#define WEBHOSTD_DEFAULT_SERVICE_RESPAWN_DELAY 100
#define WEBHOSTD_PROCESS_KILL_INTERVAL 5000

class Config
{
 public:
  Config();
  uint16_t controlPort() const;
  QString networkInterface(unsigned n) const;
  QString ntpConfigurationFile() const;
  QString ntpServiceName() const;
  QString serviceCommand() const;
  int serviceRespawnDelay() const;
  void load();

 private:
  uint16_t config_control_port;
  std::vector<QString> config_network_interfaces;
  QString config_ntp_configuration_file;
  QString config_ntp_service_name;
  QString config_service_command;
  int config_service_respawn_delay;
};


#endif  // CONFIG_H
