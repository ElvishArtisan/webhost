// whcgipage.cpp
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

#include <syslog.h>

#include <QObject>

#include "whcgipage.h"

WHCgiPage::WHCgiPage(WHCgiPost *post)
  : WHCgiObject(post)
{
  page_menu_text="";
  page_title_text="";
  page_mime_type="text/html";
  page_menu_ref="";
}


QString WHCgiPage::menuText() const
{
  syslog(LOG_NOTICE,"WHCgiPage::menuText: %s",(const char *)page_menu_text.toUtf8());
  return page_menu_text;
}


void WHCgiPage::setMenuText(const QString &str)
{
  page_menu_text=str;
}


QString WHCgiPage::titleText() const
{
  return page_title_text;
}


void WHCgiPage::setTitleText(const QString &str)
{
  page_title_text=str;
}


QString WHCgiPage::mimeType() const
{
  return page_mime_type;
}


void WHCgiPage::setMimeType(const QString &str)
{
  page_mime_type=str;
}


QString WHCgiPage::menuRef() const
{
  return page_menu_ref;
}


void WHCgiPage::setMenuRef(const QString &str)
{
  page_menu_ref=str;
}


void WHCgiPage::addScript(const QString &scriptname)
{
  page_scripts.push_back(scriptname);
}


QString WHCgiPage::onLoadEvent() const
{
  return page_on_load_event;
}


void WHCgiPage::setOnLoadEvent(const QString &str)
{
  page_on_load_event=str;
}


QString WHCgiPage::styleSheet() const
{
  return page_style_sheet;
}


void WHCgiPage::setStyleSheet(const QString &str)
{
  page_style_sheet=str;
}


void WHCgiPage::renderHead()
{
  printf("Content-type: %s\n",(const char *)page_mime_type.toUtf8());
  printf("\n");
  printf("<!doctype html>\n");
  printf("<html itemscope=\"\" itemtype=\"http://schema.org/WebPage\" lang=\"%s\">\n",(const char *)settings()->language().toUtf8());
  printf("<head>\n");
  printf("<title>%s</title>\n",(const char *)page_title_text.toUtf8());
  printf("<meta content=\"text/html; charset=utf-8\" http-equiv=Content-Type>\n");
  for(int i=0;i<page_scripts.size();i++) {
    printf("<script type=\"text/javascript\" src=\"%s\"></script>\n",
	   (const char *)page_scripts[i].toUtf8());
  }
  printf("<style>\n");

  //
  // Main Document Style
  //
  QString style=QString("body {")+
    "background-color: "+settings()->backgroundColor()+";}";
  style+=QString("a:active {")+
    "color: "+settings()->activeLinkColor()+";}";
  style+=QString("a:link {")+
    "color: "+settings()->linkColor()+";}";
  style+=QString("a:visited {")+
    "color: "+settings()->visitedLinkColor()+";}";
  style+=QString("td {")+
    "padding: 0px;"+
    "border-spacing: 0px;"+
    "border: 0px;"+
    "}";

  //
  // Control Bar Style
  //
  style+=QString(".tab-head {")+
    "background-color: #6383B7;"+
    "color: white;"+
    "text-align: center;"+
    "vertical-align: middle;"+
    "white-space: nowrap;"+
    "padding: 0px;"+
    "border-spacing: 0px;"+
    "}";

  //
  // Custom Styles
  //
  if(!styleSheet().isEmpty()) {
    printf("%s\n",(const char *)styleSheet().toUtf8());
  }

  printf("%s\n",(const char *)style.toUtf8());
  printf("</style>\n");
  printf("</head>\n");
}


void WHCgiPage::renderBodyStart()
{
  QString on_load="";
  if(page_on_load_event.isEmpty()) {
    printf("<body>\n");
  }
  else {
    printf("<body onload=\"%s\">\n",
	   (const char *)page_on_load_event.toUtf8());
  }
}


void WHCgiPage::renderBodyEnd()
{
  printf("</body></html>\n");
}
