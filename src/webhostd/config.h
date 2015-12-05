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

#define WEBHOST_DEFAULT_CONTROL_PORT 6352
#define WEBHOST_CONF_FILE "/etc/webhost.conf"

class Config
{
 public:
  Config();
  uint16_t controlPort() const;
  void load();

 private:
  uint16_t config_control_port;
};


#endif  // CONFIG_H
