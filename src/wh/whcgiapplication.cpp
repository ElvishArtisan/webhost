// whcgipost.cpp
//
// Base class for CGI Applications
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <QVariant>

#include "whcgiapplication.h"

WHCgiApplication::WHCgiApplication(QObject *parent,unsigned maxsize,bool auto_delete)
  : QObject(parent)
{
  app_post=new WHCgiPost();

  app_render_timer=new QTimer(this);
  app_render_timer->setSingleShot(true);
  connect(app_render_timer,SIGNAL(timeout()),this,SLOT(renderData()));
  app_render_timer->start(0);
}


WHCgiApplication::~WHCgiApplication()
{
  delete app_post;
}


void WHCgiApplication::addPage(int cmd_id,WHCgiPage *page)
{
  if(app_pages[cmd_id]==NULL) {
    app_pages[cmd_id]=page;
  }
  else {
    WHCgiPage::exit(400,
	QString().sprintf("attempted to add page with duplicate ID \"%d\"",
			  cmd_id));
  }
}


void WHCgiApplication::renderData()
{
  int id=0;

  post()->getValue("COMMAND",&id);
  if(app_pages[id]==NULL) {
    WHCgiPage::exit(500,QString().sprintf("Unknown page ID %d received",id));
  }
  app_pages[id]->renderHead();
  app_pages[id]->renderBodyStart();
  app_pages[id]->render();
  app_pages[id]->renderBodyEnd();
  WHCgiPage::exit(200);
}


WHCgiPost *WHCgiApplication::post() const
{
  return app_post;
}
