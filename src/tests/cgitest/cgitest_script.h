// cgitest_script.h
//
// CGI script component for CGI test
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

#ifndef CGITEST_SCRIPT_H
#define CGITEST_SCRIPT_H

#include <wh5/whcgiapplication.h>

class MainObject : public WHCgiApplication
{
 public:
  MainObject(QObject *parent=0);
};


#endif  // CGITEST_SCRIPT_H
