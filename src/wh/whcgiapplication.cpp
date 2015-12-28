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
  app_pages.push_back(page);
  app_pages.back()->setId(cmd_id);
}


void WHCgiApplication::renderData()
{
  int id;
  WHCgiPage *page=NULL;

  post()->getValue("COMMAND",&id);
  if((page=GetPage(id))==NULL) {
    if((page=GetPage(0))==NULL) {
      WHCgiPage::exit(500,QString().sprintf("Unknown page ID %d received",id));
    }
  }
  page->renderHead();
  page->renderBodyStart();
  if(id!=0) {
    RenderMenu(id);
  }
  page->render();
  page->renderBodyEnd();
  WHCgiPage::exit(200);
}


WHCgiPost *WHCgiApplication::post() const
{
  return app_post;
}


void WHCgiApplication::RenderMenu(int id)
{
  printf("<table border=0 cellpadding=1 cellspacing=0><tr class=\"tab-head\">\n");
  printf("<td>|</td>\n");
  for(unsigned i=0;i<app_pages.size();i++) {
    if(!app_pages[i]->menuRef().isEmpty()) {
      printf("<td nowrap>&#160;");
      if(app_pages[i]->id()==id) {
	printf("%s",(const char *)app_pages[i]->menuText().toUtf8());
      }
      else {
	printf("<a href=\"%s\" class=\"tab-head\">%s</a>",
	       (const char *)app_pages[i]->menuRef().toUtf8(),
	       (const char *)app_pages[i]->menuText().toUtf8());

      }
      printf("&#160;</td><td>|</td>\n");
    }
  }
  printf("</tr></table>\n");
  printf("<br>\n");
}


WHCgiPage *WHCgiApplication::GetPage(int id)
{
  for(unsigned i=0;i<app_pages.size();i++) {
    if(app_pages[i]->id()==id) {
      return app_pages[i];
    }
  }

  return NULL;
}
