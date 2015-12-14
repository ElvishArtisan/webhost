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

#include <QObject>

#include "whcgipage.h"

WHCgiPage::WHCgiPage(WHCgiPost *post)
{
  page_menu_text=QObject::tr("CGI Page");
  page_title_text=QObject::tr("CGI Page");
  page_mime_type="text/html";
  page_menu_ref="";
  page_post=post;
  page_settings=new WHSettings(*post->settings());
}


WHCgiPage::~WHCgiPage()
{
  delete page_settings;
}


QString WHCgiPage::menuText() const
{
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


void WHCgiPage::exit(int resp_code,const QString &msg)
{
  if(!msg.isEmpty()) {
    printf("Content-type: text/html\n");
    printf("Status: %d\n",resp_code);
    printf("\n");
    printf("%s\n",(const char *)msg.toUtf8());
  }
  ::exit(0);
}


WHSettings *WHCgiPage::settings()
{
  return page_settings;
}


void WHCgiPage::renderHead()
{
  printf("Content-type: %s\n",(const char *)page_mime_type.toUtf8());
  printf("\n");
  printf("<!doctype html>\n");
  printf("<html itemscope=\"\" itemtype=\"http://schema.org/WebPage\" lang=\"%s\">\n",(const char *)page_settings->language().toUtf8());
  printf("<head>\n");
  printf("<title>%s</title>\n",(const char *)page_title_text.toUtf8());
  printf("<meta content=\"text/html; charset=windows-1252\" http-equiv=Content-Type>\n");
  for(int i=0;i<page_scripts.size();i++) {
    printf("<script type=\"text/javascript\" src=\"%s\"></script>\n",
	   (const char *)page_scripts[i].toUtf8());
  }
  printf("<style>\n");
  printf(".tab-head { background:#6383b7; color:white; text-align: center; }\n");
  printf("</style>\n");
  printf("</head>\n");
}


void WHCgiPage::renderBodyStart()
{
  printf("<body bgColor=\"%s\" alink=\"%s\" link=\"%s\" vlink=\"%s\">\n",
	 (const char *)page_settings->backgroundColor().toUtf8(),
	 (const char *)page_settings->activeLinkColor().toUtf8(),
	 (const char *)page_settings->linkColor().toUtf8(),
	 (const char *)page_settings->visitedLinkColor().toUtf8());
}


void WHCgiPage::renderBodyEnd()
{
  printf("</body></html>\n");
}


WHCgiPost *WHCgiPage::post()
{
  return page_post;
}
