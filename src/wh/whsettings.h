// whsettings.h
//
// WebHost settings
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

#ifndef WHSETTINGS_H
#define WHSETTINGS_H

#include <QString>

#define WEBHOST_DEFAULT_CONTROL_PORT 6352
#define WEBHOST_CONF_FILE "/etc/webhost.conf"

class WHSettings
{
 public:
  WHSettings();
  QString language() const;
  void setLanguage(const QString &str);

 private:
  QString set_language;
};


#endif  // WHSETTINGS_H
