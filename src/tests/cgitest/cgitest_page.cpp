// cgitest_page.cpp
//
// CGI page component for CGI test
//
//   (C) Copyright 2016-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <unistd.h>

#include <wh5/whcgiapplication.h>
#include <wh5/whnetwork.h>
#include <wh5/whprofile.h>

#include "cgitest_page.h"

CgiPage::CgiPage(WHCgiPost *post)
  : WHCgiPage(post)
{
  setTitleText("Webhost CGI Test");
}


CgiPage::~CgiPage()
{
}


void CgiPage::render()
{
  QString field1;
  QString field2;
  QString field3;
  QString file;

  if(!post()->getValue("FIELD_1",&field1)) {
    cgiapp->exit(400,"Missing FIELD_1");
  }
  printf("FIELD_1: %s<br>\n",(const char *)field1.toUtf8());

  if(!post()->getValue("FIELD_2",&field2)) {
    cgiapp->exit(400,"Missing FIELD_2");
  }
  printf("FIELD_2: %s<br>\n",(const char *)field2.toUtf8());

  if(!post()->getValue("FIELD_3",&field3)) {
    cgiapp->exit(400,"Missing FIELD_3");
  }
  printf("FIELD_3: %s<br>\n",(const char *)field3.toUtf8());

  if(!post()->getValue("FILE",&file)) {
    cgiapp->exit(400,"Missing FILE");
  }
  if(post()->isFile("FILE")) {
    link(file.toUtf8(),"/tmp/cgitest_fileoutput.dat");
  }
}
