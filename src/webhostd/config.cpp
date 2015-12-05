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
}


uint16_t Config::controlPort() const
{
  return config_control_port;
}


void Config::load()
{
  Profile *p=new Profile();

  p->setSource(WEBHOST_CONF_FILE);
  config_control_port=p->
    intValue("Webhost","ControlPort",WEBHOST_DEFAULT_CONTROL_PORT);

  delete p;
}
