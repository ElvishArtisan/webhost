// whcgiobject.cpp
//
// Base class for CGI web objects
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

#include "whcgiobject.h"

WHCgiObject::WHCgiObject(WHCgiPost *post)
{
  page_id=-1;
  page_mime_type="text/html";
  page_menu_ref="";
  page_post=post;
  page_settings=new WHSettings(*post->settings());
}


WHCgiObject::~WHCgiObject()
{
  delete page_settings;
}


int WHCgiObject::id() const
{
  return page_id;
}


void WHCgiObject::setId(int id)
{
  page_id=id;
}


QString WHCgiObject::menuText() const
{
  syslog(LOG_NOTICE,"WHCgiObject::menuText()");
  return QString("");
}


QString WHCgiObject::titleText() const
{
  return QString("");
}


QString WHCgiObject::mimeType() const
{
  return page_mime_type;
}


void WHCgiObject::setMimeType(const QString &str)
{
  page_mime_type=str;
}


QString WHCgiObject::menuRef() const
{
  return QString("");
}


WHSettings *WHCgiObject::settings()
{
  return page_settings;
}


void WHCgiObject::renderHead()
{
}


void WHCgiObject::renderBodyStart()
{
}


void WHCgiObject::renderBodyEnd()
{
}


WHCgiPost *WHCgiObject::post()
{
  return page_post;
}
