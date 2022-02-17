// whsettings.cpp
//
// WebHost settings
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

#include "whsettings.h"

WHSettings::WHSettings()
{
  set_language="en";
  set_active_link_color="#FFFFFF";
  set_link_color="#FFFFFF";
  set_visited_link_color="#FFFFFF";
  set_background_color="#D6D3CE";
}


QString WHSettings::language() const
{
  return set_language;
}


void WHSettings::setLanguage(const QString &str)
{
  set_language=str;
}


QString WHSettings::activeLinkColor() const
{
  return set_active_link_color;
}


void WHSettings::setActiveLinkColor(const QString &color)
{
  set_active_link_color=color;
}


QString WHSettings::linkColor() const
{
  return set_link_color;
}


void WHSettings::setLinkColor(const QString &color)
{
  set_link_color=color;
}


QString WHSettings::visitedLinkColor() const
{
  return set_visited_link_color;
}


void WHSettings::setVisitedLinkColor(const QString &color)
{
  set_visited_link_color=color;
}


QString WHSettings::backgroundColor() const
{
  return set_background_color;
}


void WHSettings::setBackgroundColor(const QString &color)
{
  set_background_color=color;
}
