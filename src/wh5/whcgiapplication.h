// whcgiapplication.h
//
// Base class for CGI Applications
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

#ifndef WHCGIAPPLICATION_H
#define WHCGIAPPLICATION_H

#include <vector>

#include <QHostAddress>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <wh5/whcgiobject.h>
#include <wh5/whcgipage.h>
#include <wh5/whcgipost.h>
#include <wh5/whsettings.h>

class WHCgiApplication : public QObject
{
  Q_OBJECT;
 public:
  WHCgiApplication(QObject *parent=0,bool auto_delete=true);
  ~WHCgiApplication();
  void addPage(int cmd_id,WHCgiObject *page);
  void exit(int resp_code,const QString &msg="",bool no_cleanup=false);
  void renderPage(int id);
  void redirect(const QString &url,bool no_cleanup=false);

 private slots:
  void renderData();

 protected:
  WHCgiPost *post() const;

 private:
  void RenderMenu(int id);
  WHCgiObject *GetPage(int id);
  std::vector<WHCgiObject *> app_pages;
  WHCgiPost *app_post;
  QTimer *app_render_timer;
};

extern WHCgiApplication *cgiapp;

#endif  // WHCGIAPPLICATION_H
