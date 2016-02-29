// sockettest.js
//
// Client-side routines for the webhost 'sockettest' test case.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

var sock;

function Connect()
{
    sock=new WebSocket('ws://localhost:8080/myconn','myproto');

    sock.onmessage=function(event) {
	Id('OUTPUT').innerHTML=event.data;
    }
}

function Send()
{
    sock.send(Id('INPUT').value);
}

function Id(id) {
    return document.getElementById(id);
}
