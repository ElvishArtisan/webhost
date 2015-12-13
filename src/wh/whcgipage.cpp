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
  printf("</head>\n");
}


void WHCgiPage::renderBodyStart()
{
  printf("<body>\n");
}


void WHCgiPage::renderBodyEnd()
{
  printf("</body></html>\n");
}


WHCgiPost *WHCgiPage::post()
{
  return page_post;
}
