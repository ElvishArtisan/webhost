// whcgiobject.h
//
// Base class for CGI web objects
//
//   (C) Copyright 2015-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WHCGIOBJECT_H
#define WHCGIOBJECT_H

#include <QStringList>

#include <wh5/whcgipost.h>

class WHCgiObject
{
 public:
  WHCgiObject(WHCgiPost *post);
  ~WHCgiObject();
  int id() const;
  void setId(int id);
  virtual QString menuText() const;
  virtual QString titleText() const;
  QString mimeType() const;
  void setMimeType(const QString &str);
  virtual QString menuRef() const;
  virtual void renderHead();
  virtual void renderBodyStart();
  virtual void render()=0;
  virtual void renderBodyEnd();

 protected:
  WHSettings *settings();
  WHCgiPost *post();

 private:
  int page_id;
  QString page_mime_type;
  QString page_language;
  WHCgiPost *page_post;
  WHSettings *page_settings;
  QString page_menu_ref;
};


#endif  // WHCGIOBJECT_H
