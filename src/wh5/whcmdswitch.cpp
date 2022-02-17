// whcmdswitch.cpp
//
// Process Command-Line Switches
//
//   (C) Copyright 2012-2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include "whcmdswitch.h"

WHCmdSwitch::WHCmdSwitch(const char *modname,const char *version,
			 const char *usage)
{
  unsigned l=0;
  bool handled=false;
  QStringList args=QCoreApplication::arguments();


  for(int i=1;i<args.size();i++) {
#ifndef WIN32
    if(args.at(i)=="--version") {
      printf("%s v%s\n",modname,version);
      exit(0);
    }
#endif  // WIN32
    if(args.at(i)=="--help") {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    l=args.at(i).length();
    handled=false;
    for(unsigned j=0;j<l;j++) {
      if(args.at(i).at(j)==QChar('=')) {
	switch_keys.push_back(QString(args.at(i)).left(j));
	switch_values.push_back(QString(args.at(i)).right(l-(j+1)));
	switch_processed.push_back(false);
	j=l;
	handled=true;
      }
    }
    if(!handled) {
      switch_keys.push_back(QString(args.at(i)));
      switch_values.push_back(QString(""));
      switch_processed.push_back(false);
    }
  }
}

/*
WHCmdSwitch::WHCmdSwitch(int argc,char *argv[],const char *modname,
			 const char *version,const char *usage)
{
  unsigned l=0;
  bool handled=false;

  for(int i=1;i<argc;i++) {
#ifndef WIN32
    if(!strcmp(argv[i],"--version")) {
      printf("%s v%s\n",modname,version);
      exit(0);
    }
#endif  // WIN32
    if(!strcmp(argv[i],"--help")) {
      printf("\n%s %s\n",modname,usage);
      exit(0);
    }
    l=strlen(argv[i]);
    handled=false;
    for(unsigned j=0;j<l;j++) {
      if(argv[i][j]=='=') {
	switch_keys.push_back(QString(argv[i]).left(j));
	switch_values.push_back(QString(argv[i]).right(l-(j+1)));
	switch_processed.push_back(false);
	j=l;
	handled=true;
      }
    }
    if(!handled) {
      switch_keys.push_back(QString(argv[i]));
      switch_values.push_back(QString(""));
      switch_processed.push_back(false);
    }
  }
}
*/


unsigned WHCmdSwitch::keys() const
{
  return switch_keys.size();
}


QString WHCmdSwitch::key(unsigned n) const
{
  return switch_keys[n];
}


QString WHCmdSwitch::value(unsigned n) const
{
  return switch_values[n];
}


bool WHCmdSwitch::processed(unsigned n) const
{
  return switch_processed[n];
}


void WHCmdSwitch::setProcessed(unsigned n,bool state)
{
  switch_processed[n]=state;
}


bool WHCmdSwitch::allProcessed() const
{
  for(unsigned i=0;i<switch_processed.size();i++) {
    if(!switch_processed[i]) {
      return false;
    }
  }
  return true;
}
