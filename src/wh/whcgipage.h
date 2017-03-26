// whcgipage.h
//
// Base class for CGI web pages
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

#ifndef WHCGIPAGE_H
#define WHCGIPAGE_H

#include <QStringList>

#include <wh/whcgiobject.h>
#include <wh/whcgipost.h>

class WHCgiPage : public WHCgiObject
{
 public:
  WHCgiPage(WHCgiPost *post);
  QString menuText() const;
  void setMenuText(const QString &str);
  QString titleText() const;
  void setTitleText(const QString &str);
  QString mimeType() const;
  void setMimeType(const QString &str);
  QString menuRef() const;
  void setMenuRef(const QString &str);
  void addScript(const QString &scriptname);
  QString onLoadEvent() const;
  void setOnLoadEvent(const QString &str);
  QString styleSheet() const;
  void setStyleSheet(const QString &str);
  virtual void renderHead();
  virtual void renderScripts();
  virtual void renderBodyStart();
  virtual void render()=0;
  virtual void renderBodyEnd();

 private:
  QString page_menu_text;
  QString page_title_text;
  QString page_mime_type;
  QString page_language;
  QStringList page_scripts;
  QString page_menu_ref;
  QString page_on_load_event;
  QString page_style_sheet;
};


#endif  // WHCGIPAGE_H
