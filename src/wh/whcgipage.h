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

#include <wh/whcgipost.h>

class WHCgiPage
{
 public:
  WHCgiPage(WHCgiPost *post);
  ~WHCgiPage();
  QString menuText() const;
  void setMenuText(const QString &str);
  QString titleText() const;
  void setTitleText(const QString &str);
  QString mimeType() const;
  void setMimeType(const QString &str);
  virtual void renderHead();
  virtual void renderBodyStart();
  virtual void render()=0;
  virtual void renderBodyEnd();
  static void exit(int resp_code,const QString &msg="");

 protected:
  WHSettings *settings();
  WHCgiPost *post();

 private:
  QString page_menu_text;
  QString page_title_text;
  QString page_mime_type;
  QString page_language;
  WHCgiPost *page_post;
  WHSettings *page_settings;
};


#endif  // WHCGIPAGE_H
