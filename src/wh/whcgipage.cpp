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

#include "whcgipage.h"

WHCgiPage::WHCgiPage(WHCgiPost *post)
{
  page_post=post;
}


WHCgiPage::~WHCgiPage()
{
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


WHCgiPost *WHCgiPage::post()
{
  return page_post;
}
