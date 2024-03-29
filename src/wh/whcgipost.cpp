// whcgipost.cpp
//
// POST data processor class for CGI applications
//
//   (C) Copyright 2015-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDir>
#include <QFileInfoList>
#include <QProcess>
#include <QVariant>

#include "whcgipost.h"
#include "whsettings.h"

#define WHCGIPOST_UPGRADE_DONE_FILE "/done"

WHCgiPost::WHCgiPost(bool auto_delete)
{
  bool ok=false;
  post_type=WHCgiPost::Cgi;
  post_auto_delete=auto_delete;

  if(!Initialize()) {
    return;
  }

  //
  // Verify Transfer Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    post_error=WHCgiPost::ErrorNotPost;
    return;
  }

  //
  // Verify Size
  //
  if(getenv("CONTENT_LENGTH")==NULL) {
    post_content_length=0;
  }
  else {
    post_content_length=QString(getenv("CONTENT_LENGTH")).toUInt(&ok);
  }

  //
  // Get the encoding type
  //
  post_error=WHCgiPost::ErrorOk;
  if(getenv("CONTENT_TYPE")!=NULL) {
    QStringList f0=QString(getenv("CONTENT_TYPE")).split(";");
    if(f0[0].trimmed().toLower()=="application/x-www-form-urlencoded") {
      post_encoding=WHCgiPost::UrlEncoded;
      LoadUrlEncoding();
    }
    if(f0[0].trimmed().toLower()=="multipart/form-data") {
      post_encoding=WHCgiPost::MultipartEncoded;
      LoadMultipartEncoding();
    }
  }
}


WHCgiPost::~WHCgiPost()
{
  if(post_auto_delete) {
    for(std::map<QString,bool>::const_iterator ci=post_filenames.begin();
	ci!=post_filenames.end();ci++) {
      if(ci->second) {
	unlink(post_values[ci->first].toString().toAscii());
      }
    }
    if(!post_tempdir.isNull()) {
      rmdir(post_tempdir.toAscii());
    }
  }
  delete post_socket;
  delete post_settings;
  delete post_config;
}


WHCgiPost::Type WHCgiPost::type() const
{
  return post_type;
}


WHCgiPost::Error WHCgiPost::error() const
{
  return post_error;
}


QStringList WHCgiPost::names() const
{
  QStringList list;
  for(std::map<QString,QVariant>::const_iterator ci=post_values.begin();
      ci!=post_values.end();ci++) {
    list.push_back(ci->first);
  }
  return list;
}


QVariant WHCgiPost::value(const QString &name,bool *ok)
{
  QVariant v;
  if(post_values.count(name)>0) {
    v=post_values[name];
  }
  if(ok!=NULL) {
    *ok=(post_values.count(name)>0);
  }
  return v;
}


bool WHCgiPost::getValue(const QString &name,QHostAddress *addr)
{
  QString str;
  bool ok=getValue(name,&str);
  if(!ok) {
    return false;
  }
  addr->setAddress(str);
  return true;
}


bool WHCgiPost::getValue(const QString &name,QString *str)
{
  *str="";
  if(post_values.count(name)>0) {
    *str=post_values[name].toString();
    return true;
  }
  return false;
}


bool WHCgiPost::getValue(const QString &name,QUrl *url)
{
  if(post_values.count(name)>0) {
    *url=QUrl(post_values[name].toString());
    return true;
  }
  *url=QUrl();
  return false;  
}


bool WHCgiPost::getValue(const QString &name,int *n)
{
  *n=0;
  if(post_values.count(name)>0) {
    *n=post_values[name].toInt();
    return true;
  }
  return false;
}


bool WHCgiPost::getValue(const QString &name,long *n)
{
  *n=0;
  if(post_values.count(name)>0) {
    *n=post_values[name].toLongLong();
    return true;
  }
  *n=0;
  return false;
}


bool WHCgiPost::getValue(const QString &name,bool *n)
{
  *n=false;
  if(post_values.count(name)>0) {
    *n=post_values[name].toBool();
    return true;
  }
  *n=false;
  return false;
}


bool WHCgiPost::isFile(const QString &name)
{
  return post_filenames[name];
}


QString WHCgiPost::tempDir() const
{
  return post_tempdir;
}


WHSettings *WHCgiPost::settings()
{
  return post_settings;
}


WHCgiPost::InterfaceMode WHCgiPost::interfaceMode(unsigned iface) const
{
  return post_interface_modes[iface];
}


QHostAddress WHCgiPost::ipAddress(unsigned iface) const
{
  return post_ip_addresses[iface];
}


QHostAddress WHCgiPost::ipNetmask(unsigned iface) const
{
  return post_netmask_addresses[iface];
}


QHostAddress WHCgiPost::ipGateway() const
{
  return post_gateway_address;
}


QHostAddress WHCgiPost::dnsAddress(unsigned n) const
{
  return post_dns_addresses[n];
}


bool WHCgiPost::wifiActive() const
{
  return post_wifi_active;
}


QString WHCgiPost::cpuinfoModelName() const
{
  return post_cpuinfo_model_name;
}


QString WHCgiPost::cpuinfoHardware() const
{
  return post_cpuinfo_hardware;
}


QString WHCgiPost::cpuinfoRevision() const
{
  return post_cpuinfo_revision;
}


QString WHCgiPost::cpuinfoSerial() const
{
  return post_cpuinfo_serial;
}


QList<WHWifiConnection *> WHCgiPost::wifiConnections() const
{
  return post_wifi_connections;
}


QString WHCgiPost::ntpHostname(unsigned n) const
{
  return post_ntp_hostnames[n];
}


QStringList WHCgiPost::timezoneList() const
{
  QStringList ret;
  QStringList args;

  args.push_back("--no-pager");
  args.push_back("--no-ask-password");
  args.push_back("list-timezones");

  QProcess *proc=new QProcess();
  proc->start("timedatectl",args);
  proc->waitForFinished();
  ret=QString(proc->readAllStandardOutput()).
    split("\n",QString::SkipEmptyParts);
  delete proc;

  return ret;
}


QString WHCgiPost::currentTimezone() const
{
  QStringList lines;
  QStringList args;

  args.push_back("--no-pager");
  args.push_back("--no-ask-password");
  args.push_back("status");

  QProcess *proc=new QProcess();
  proc->start("timedatectl",args);
  proc->waitForFinished();
  lines=QString(proc->readAllStandardOutput()).split("\n");
  delete proc;

  for(int i=0;i<lines.size();i++) {
    QStringList f0=lines[i].split(":");
    if(f0.size()>=2) {
      if((f0[0].toLower().trimmed()=="time zone")||
	 (f0[0].toLower().trimmed()=="timezone")){
	QStringList f1=f0[1].split(" ",QString::SkipEmptyParts);
	return f1[0].trimmed();
      }
    }
  }

  return QString();
}


void WHCgiPost::sendUdpPacket(const QByteArray &data,uint16_t port)
{
  post_socket->writeDatagram(data,QHostAddress("127.0.0.1"),port);
}


void WHCgiPost::sendAddUserCommand(const QString &htpasswd_file,
				   const QString &username,
				   const QString &password) const
{
  SendCommand("ADDUSER "+
	      htpasswd_file+" "+
	      username+" "+
	      password+"!");
}


void WHCgiPost::sendAddUser2Command(const QString &htpasswd_file,
				    const QString &realm,
				    const QString &username,
				    const QString &password) const
{
  SendCommand("ADDUSER2 "+
	      htpasswd_file+" "+
	      realm+" "+
	      username+" "+
	      password+"!");
}


void WHCgiPost::sendDeleteUserCommand(const QString &htpasswd_file,
				      const QString &username)
{
  SendCommand("DELETEUSER "+
	      htpasswd_file+" "+
	      username+"!");
}


void WHCgiPost::sendDeleteUser2Command(const QString &htpasswd_file,
				       const QString &realm,
				       const QString &username)
{
  SendCommand("DELETEUSER2 "+
	      htpasswd_file+" "+
	      realm+" "+
	      username+"!");
}


void WHCgiPost::sendIpCommand(unsigned iface_num,const QHostAddress &addr,
			      const QHostAddress &mask,
			      const QHostAddress &gw,const QHostAddress &dns1,
			      const QHostAddress &dns2,bool enable_dhcp) const
{
  SendCommand("IP "+QString().sprintf("%u ",iface_num)+addr.toString()+" "+
	      mask.toString()+" "+gw.toString()+" "+
	      dns1.toString()+" "+dns2.toString()+
	      QString().sprintf(" %d!",enable_dhcp));
}


void WHCgiPost::sendDisableIpCommand(unsigned iface_num) const
{
  SendCommand("IP "+QString().sprintf("%u!",iface_num));
}


void WHCgiPost::sendNtpCommand(const QString &timezone,QString ntp1,
			       QString ntp2) const
{
  if(ntp1.isEmpty()) {
    ntp1="0.0.0.0";
  }
  if(ntp2.isEmpty()) {
    ntp2="0.0.0.0";
  }
  SendCommand("NTP "+timezone+" "+ntp1+" "+ntp2+"!");
}


void WHCgiPost::sendRebootCommand() const
{
  SendCommand("REBOOT!");
}


void WHCgiPost::sendRestartCommand(const QString &sysname) const
{
  SendCommand("RESTART "+sysname+"!");
}


void WHCgiPost::sendUpgradeCommand(const QString &filename) const
{
  QStringList f0;
  QString tempdir;
  int count=0;

  SendCommand("UPGRADE "+filename+"!");

  //
  // Wait for webhostd to signal completion
  // (because "wonderful" systemd deletes the package file after the CGI
  // process exits).
  //
  f0=filename.split("/");
  f0.erase(f0.begin()+f0.size()-1);
  tempdir=f0.join("/");

  int fd=-1;
  f0=filename.split("/");
  f0.erase(f0.begin()+f0.size()-1);
  do {
    sleep(1);
    fd=open((f0.join("/")+WHCGIPOST_UPGRADE_DONE_FILE).toUtf8(),O_RDONLY);
  } while((count++<30)&&(fd<0));
  if(fd>0) {
    close(fd);
  }

  unlink(filename.toUtf8());
  unlink((tempdir+WHCGIPOST_UPGRADE_DONE_FILE).toUtf8());
  rmdir(tempdir.toUtf8());
}


void WHCgiPost::sendConnectWifiCommand(const QString &ssid,
				       const QString &passwd) const
{
  SendCommand("WIFI "+ssid+" "+passwd+"!");
}


void WHCgiPost::sendDisconnectWifiCommand() const
{
  SendCommand("WIFI!");
}


QString WHCgiPost::dump()
{
  QString ret;

  ret+=QString().sprintf("Content-type: text/html\n\n");
  ret+=QString().
    sprintf("<table>\n");
  ret+=QString().sprintf("<tr>\n");
  ret+=QString().
    sprintf("<td colspan=\"3\" align=\"center\"><strong>WHCgiPost Data Dump</strong></td>\n");
  ret+=QString().sprintf("</tr>\n");

  ret+=QString().sprintf("<tr>\n");
  ret+=QString().sprintf("<th align=\"center\">NAME</th>\n");
  ret+=QString().sprintf("<th align=\"center\">VALUE</th>\n");
  ret+=QString().sprintf("<th align=\"center\">FILE</th>\n");
  ret+=QString().sprintf("</tr>\n");
  
  for(std::map<QString,QVariant>::const_iterator ci=post_values.begin();
      ci!=post_values.end();ci++) {
    ret+=QString().sprintf("<tr>\n");
    ret+=QString().sprintf("<td align=\"left\">|%s|</td>\n",
			   (const char *)ci->first.toAscii());
    ret+=QString().sprintf("<td align=\"left\">|%s|</td>\n",
	   (const char *)ci->second.toString().toAscii());
    if(post_filenames[ci->first]) {
      ret+=QString().sprintf("<td align=\"center\">Yes</td>\n");
    }
    else {
      ret+=QString().sprintf("<td align=\"center\">No</td>\n");
    }
    ret+=QString().sprintf("</tr>\n");
  }

  ret+=QString().sprintf("</table>\n");
  return ret;
}


QString WHCgiPost::errorString(WHCgiPost::Error err)
{
  QString str="Unknown error";

  switch(err) {
  case WHCgiPost::ErrorOk:
    str="OK";
    break;

  case WHCgiPost::ErrorNotPost:
    str="Request is not POST";
    break;

  case WHCgiPost::ErrorNoTempDir:
    str="Unable to create temporary directory";
    break;

  case WHCgiPost::ErrorMalformedData:
    str="The data is malformed";
    break;

  case WHCgiPost::ErrorPostTooLarge:
    str="POST is too large";
    break;

  case WHCgiPost::ErrorInternal:
    str="Internal error";
    break;

  case WHCgiPost::ErrorNotInitialized:
    str="POST class not initialized";
    break;

  case WHCgiPost::ErrorCannotSaveFile:
    str="Cannot save temporary file";
    break;
  }
  return str;
}


QString WHCgiPost::dumpEnvironment()
{
  QString ret;
  unsigned i=0;

  while(environ[i]!=NULL) {
    ret+=environ[i];
    ret+="\n";
    i++;
  }
  return ret;
}


bool WHCgiPost::Initialize()
{
  char tempdir[PATH_MAX];

  post_encoding=WHCgiPost::AutoEncoded;
  post_error=WHCgiPost::ErrorNotInitialized;
  post_settings=new WHSettings();

  post_config=new WHConfig();
  post_config->load();

  ReadIpConfig();

  //
  // Command Socket
  //
  post_socket=new QUdpSocket();

  //
  // Initialize Temp Directory Path
  //
  if(getenv("TMPDIR")!=NULL) {
    strcpy(tempdir,getenv("TMPDIR"));
  }
  else {
    strcpy(tempdir,"/tmp");
  }
  strcat(tempdir,"/webhostXXXXXX");
  post_tempdir=mkdtemp(tempdir);
  if(post_tempdir.isNull()) {
    post_error=WHCgiPost::ErrorNoTempDir;
    return false;
  }
  return true;
}


void WHCgiPost::ReadIpConfig()
{
  FILE *f=NULL;
  char line[1024];
  QStringList params;
  QStringList f0;
  QStringList f1;
  bool ok=false;
  bool eth_active=false;

  post_wifi_active=false;
  for(unsigned i=0;i<post_config->interfaceQuantity();i++) {
    QString netdev=post_config->interfaceName(i);
    post_interface_modes.push_back(WHCgiPost::Static);
    post_ip_addresses.push_back(QHostAddress());
    post_netmask_addresses.push_back(QHostAddress());
    if(post_config->useNetworkManager()) {
      QStringList args;
      QString data;

      //
      // Connection State
      //
      args.clear();
      args.push_back("-t");
      args.push_back("con");
      args.push_back("show");
      args.push_back("--active");
      data=CommandOutput("/bin/nmcli",args);
      f0=data.split("\n");
      for(int i=0;i<f0.size();i++) {
	f1=f0.at(i).trimmed().split(":");
	if(f1.size()==4) {
	  if(f1.at(3).trimmed()==post_config->interfaceName(post_ip_addresses.size()-1)) {
	    eth_active=true;
	  }
	}
      }

      if(eth_active) {
	//
	// DHCP Status
	//
	args.clear();
	args.push_back("-g");
	args.push_back("ipv4.method");
	args.push_back("connection");
	args.push_back("show");
	args.push_back(post_config->interfaceName(post_ip_addresses.size()-1));
	data=CommandOutput("/bin/nmcli",args);
	if(data.trimmed().toLower()=="auto") {
	  post_interface_modes.back()=WHCgiPost::Dhcp;
	}
	else {
	  post_interface_modes.back()=WHCgiPost::Static;
	}

	//
	// IP Address/Netmask
	//
	args.clear();
	args.push_back("-g");
	args.push_back("ipv4.addresses");
	args.push_back("connection");
	args.push_back("show");
	args.push_back(post_config->interfaceName(post_ip_addresses.size()-1));
	data=CommandOutput("/bin/nmcli",args);
	f0=data.split("/");
	if(f0.size()==2) {
	  if(post_ip_addresses.back().setAddress(f0.at(0))) {
	    unsigned masksize=f0.at(1).toUInt(&ok);
	    unsigned mask=0;
	    if(ok) {
	      for(unsigned i=0;i<masksize;i++) {
		mask=(mask<<1)+1;
	      }
	      for(unsigned i=masksize;i<32;i++) {
		mask=mask<<1;
	      }
	      post_netmask_addresses.back().setAddress(mask);
	    }
	  }
	}

	//
	// Gateway
	//
	args.clear();
	args.push_back("-g");
	args.push_back("ip4.gateway");
	args.push_back("connection");
	args.push_back("show");
	args.push_back(post_config->interfaceName(post_ip_addresses.size()-1));
	data=CommandOutput("/bin/nmcli",args);
	QHostAddress addr;
	addr.setAddress(data);
	if(!addr.isNull()) {
	  post_gateway_address=addr;
	}

	//
	// DNS
	//
	post_dns_addresses[0]=QHostAddress();
	post_dns_addresses[1]=QHostAddress();
	args.clear();
	args.push_back("-g");
	args.push_back("ip4.dns");
	args.push_back("connection");
	args.push_back("show");
	args.push_back(post_config->interfaceName(post_ip_addresses.size()-1));
	data=CommandOutput("/bin/nmcli",args);
	f0=data.split("|");
	for(int i=0;i<f0.size();i++) {
	  if(i<2) {
	    post_dns_addresses[i].setAddress(f0.at(i).trimmed());
	  }
	}
      }
      else {
	post_interface_modes.back()=WHCgiPost::Disabled;
      }

      //
      // Wifi Active
      //
      args.clear();
      args.push_back("-t");
      args.push_back("device");
      args.push_back("status");
      data=CommandOutput("/bin/nmcli",args);
      f0=data.split("\n");
      for(int i=0;i<f0.size();i++) {
	f1=f0.at(i).trimmed().split(":");
	if(f1.size()==4) {
	  post_wifi_active=post_wifi_active||(f1.at(1)=="wifi");
	}
      }

      //
      // Wifi Connections
      //
      args.clear();
      args.push_back("-t");
      args.push_back("device");
      args.push_back("wifi");
      args.push_back("list");
      data=CommandOutput("/bin/nmcli",args);
      f0=data.split("\n");
      for(int i=0;i<f0.size();i++) {
	f1=f0.at(i).split(":");
	if((f1.size()==8)&&(!f1.at(1).trimmed().isEmpty())) {
	  post_wifi_connections.push_back(new WHWifiConnection());
	  post_wifi_connections.back()->
	    setInUse(!f1.at(0).trimmed().isEmpty());
	  post_wifi_connections.back()->setSsid(f1.at(1).trimmed());
	  post_wifi_connections.back()->setMode(f1.at(2).trimmed());
	  post_wifi_connections.back()->setChannel(f1.at(3).toInt());
	  post_wifi_connections.back()->setRate(f1.at(4).trimmed());
	  post_wifi_connections.back()->setSignal(f1.at(5).toInt());
	  post_wifi_connections.back()->setBars(f1.at(6));
	  post_wifi_connections.back()->setSecurity(f1.at(7));
	}
      }
    }
    else {
      if((f=fopen(("/etc/sysconfig/network-scripts/ifcfg-"+netdev).toUtf8(),"r"))
	 !=NULL) {
	while(fgets(line,1024,f)!=NULL) {
	  f0=QString(line).trimmed().split("=");
	  if(f0.size()==2) {
	    f0[1]=f0[1].replace("\"","");
	    if(f0[0]=="IPADDR") {
	      post_ip_addresses.back().setAddress(f0[1]);
	    }
	    if(f0[0]=="GATEWAY") {
	      post_gateway_address.setAddress(f0[1]);
	    }
	    if(f0[0]=="NETMASK") {
	      post_netmask_addresses.back().setAddress(f0[1]);
	    }
	    if(f0[0]=="DNS1") {
	      post_dns_addresses[0].setAddress(f0[1]);
	    }
	    if(f0[0]=="DNS2") {
	      post_dns_addresses[1].setAddress(f0[1]);
	    }
	    if(f0[0]=="BOOTPROTO") {
	      if(f0[1]=="dhcp") {
		post_interface_modes.back()=WHCgiPost::Dhcp;
	      }
	      else {
		post_interface_modes.back()=WHCgiPost::Static;
	      }
	    }
	  }
	}
      }
    }
  }

  int count=0;
  if((f=fopen("/etc/ntp.conf","r"))!=NULL) {
    while((fgets(line,1024,f)!=NULL)&&(count<WEBHOST_MAX_NTP_SERVERS)) {
      f0=QString(line).split(" ");
      if((f0[0]=="server")&&(f0.size()>=2)) {
	post_ntp_hostnames[count]=f0[1];
	count++;
      }
    }
    fclose(f);
  }

  if((f=fopen("/proc/cpuinfo","r"))!=NULL) {
    while(fgets(line,1024,f)!=NULL) {
      QStringList f0=QString(line).trimmed().split(":");
      if(f0.size()==2) {
	if(f0.at(0).trimmed().toLower()=="model name") {
	  post_cpuinfo_model_name=f0.at(1).trimmed();
	}
	if(f0.at(0).trimmed().toLower()=="hardware") {
	  post_cpuinfo_hardware=f0.at(1).trimmed();
	}
	if(f0.at(0).trimmed().toLower()=="revision") {
	  post_cpuinfo_revision=f0.at(1).trimmed();
	}
	if(f0.at(0).trimmed().toLower()=="serial") {
	  post_cpuinfo_serial=f0.at(1).trimmed();
	}
      }
    }
    fclose(f);
  }
}

void WHCgiPost::SendCommand(const QString &cmd) const
{
  post_socket->writeDatagram(cmd.toUtf8(),cmd.length(),
			     QHostAddress("127.0.0.1"),
			     WEBHOST_DEFAULT_CONTROL_PORT);
}



void WHCgiPost::LoadUrlEncoding()
{
  char *data=new char[post_content_length+1];
  int n;
  QStringList lines;
  QStringList line;

  if((n=read(0,data,post_content_length))>0) {
    data[post_content_length]=0;
    lines=QString(data).split("&");
    for(int i=0;i<lines.size();i++) {
      line=lines[i].split("=");
      if(line.size()==2) {
	post_values[line[0]]=UrlDecode(line[1]);
	post_filenames[line[0]]=false;
      }
    }
  }
  post_error=WHCgiPost::ErrorOk;
  delete data;
}


void WHCgiPost::LoadMultipartEncoding()
{
  std::map<QString,QString> headers;
  bool header=true;
  char *data=NULL;
  ssize_t n=0;
  unsigned total_bytes=0;
  QString sep;
  QString name;
  QString filename;
  QString tempdir;
  int fd=-1;
  int index;

  if((n=getline(&data,(size_t *)&n,stdin))<=0) {
    post_error=WHCgiPost::ErrorOk;
    return;
  }
  total_bytes+=n;
  sep=QString(data).simplified();

  //
  // Get message parts
  //
  while(((n=getline(&data,(size_t *)&n,stdin))>0)&&
	(total_bytes<post_content_length)) {
    total_bytes+=n;
    if(QString(data).simplified().contains(sep)>0) {  // End of part
      if(fd>=0) {
	ftruncate(fd,lseek(fd,0,SEEK_CUR)-2);  // Remove extraneous final CR/LF
	::close(fd);
	fd=-1;
      }
      name="";
      filename="";
      headers.clear();
      header=true;
      continue;
    }
    if(header) {  // Read header
      if(QString(data).simplified().isEmpty()) {
	if(!headers["content-disposition"].isNull()) {
	  QStringList fields;
	  fields=headers["content-disposition"].split(";");
	  if(fields.size()>0) {
	    if(fields[0].toLower().simplified()=="form-data") {
	      for(int i=1;i<fields.size();i++) {
		QStringList pairs;
		pairs=fields[i].split("=");
		if(pairs[0].toLower().simplified()=="name") {
		  name=pairs[1].simplified();
		  name.replace("\"","");
		}
		if(pairs[0].toLower().simplified()=="filename") {
		  if((index=pairs[1].simplified().lastIndexOf("\\"))<0) {
		    index=0;
		  }
		  filename=post_tempdir+"/"+pairs[1].simplified().
		    right(pairs[1].simplified().length()-index-1);
		  filename.replace("\"","");
		  if((fd=open(filename.
			      toAscii(),O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))<0) {
		    post_error=WHCgiPost::ErrorCannotSaveFile;
		    return;
		  }
		}
	      }
	    }
	  }
	}
	header=false;
      }
      else {
	QStringList hdr;
	hdr=QString(data).simplified().split(":");
	// Reconcaternate trailing sections so we don't split on the 
	// useless M$ drive letter supplied by IE
	for(int i=2;i<hdr.size();i++) {
	  hdr[1]+=hdr[i];
	}
	headers[hdr[0].toLower()]=hdr[1];
      }
    }
    else {  // Read data
      if(filename.isEmpty()) {
	QString str=post_values[name].toString();
	str+=QString::fromUtf8(data);
	post_filenames[name]=false;
	post_values[name]=str.simplified();
      }
      else {
	post_filenames[name]=true;
	post_values[name]=filename;
	write(fd,data,n);
      }
    }
  }
  free(data);
  post_error=WHCgiPost::ErrorOk;
}


QString WHCgiPost::UrlDecode(const QString &str) const
{
  int istate=0;
  unsigned n;
  QString code;
  QString ret;
  bool ok=false;

  for(int i=0;i<str.length();i++) {
    switch(istate) {
    case 0:
      if(str.at(i)==QChar('+')) {
	ret+=" ";
      }
      else {
	if(str.at(i)==QChar('%')) {
	  istate=1;
	}
	else {
	  ret+=str.at(i);
	}
      }
      break;

    case 1:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code=str.mid(i,1);
      istate=2;
      break;

    case 2:
      n=str.mid(i,1).toUInt(&ok);
      if((!ok)||(n>9)) {
	istate=0;
      }
      code+=str.mid(i,1);
      ret+=QChar(code.toInt(&ok,16));
      istate=0;
      break;
    }
  }

  return ret;
}


QString WHCgiPost::CommandOutput(const QString &cmd,
				 const QStringList &args) const
{
  QString ret;

  QProcess *proc=new QProcess();
  proc->start(cmd,args);
  proc->waitForFinished();
  ret=proc->readAllStandardOutput();
  delete proc;

  return ret;
}
