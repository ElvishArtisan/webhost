// filepage.cpp
//
// Default page for filedrop
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

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QStringList>

#include <wh5/whcgiapplication.h>

#include "filepage.h"

FilePage::FilePage(WHCgiPost *post)
  : WHCgiPage(post)
{
  setTitleText("FileDrop Test");
}


FilePage::~FilePage()
{
}


void FilePage::render()
{
  QString filename;
  QString tempdir;
  QStringList f0;

  //
  // Get package location
  //
  if(!post()->getValue("FILENAME",&filename)) {
    cgiapp->exit(400,"Missing FILENAME");
  }

  //
  // Apply it
  //
  post()->sendUpgradeCommand(filename);

  printf("Content-type: text/html\n\n");
  printf("FILENAME: %s<br>\n",(const char *)filename.toUtf8());
  printf("UID: %u  GID: %u<br>\n",getuid(),getgid());

  cgiapp->exit(200);
}
