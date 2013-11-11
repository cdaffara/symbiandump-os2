/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description:
*   This file contains implemention of Socket, SocketClient, SocketServer and
*   SocketSelect classes.
*/
#include <iostream>
#include <sstream>
#include <errno.h>

#include "Socket.h"
#include "util.h"

using namespace std;

int Socket::nofSockets_= 0;

void Socket::Start() {
  if (!nofSockets_) {
    WSADATA info;
    if (WSAStartup(MAKEWORD(2,0), &info)) {
      throw "Could not start WSA";
    }
  }
   ++nofSockets_;
}

void Socket::End() {
  WSACleanup();
}


Socket::Socket() : s_(0) {
  Start();
  // UDP: use SOCK_DGRAM instead of SOCK_STREAM
  s_ = socket(AF_INET,SOCK_STREAM,0);

  refCounter_ = new int(1);
}

Socket::Socket(SOCKET s) : s_(s) {
  Start();
  refCounter_ = new int(1);
};


Socket::~Socket() {
  if (! --(*refCounter_)) {
	Close();
    delete refCounter_;
  }

  --nofSockets_;
  if (!nofSockets_) End();
}

Socket::Socket(const Socket& o) {
  refCounter_=o.refCounter_;
  (*refCounter_)++;
  s_         =o.s_;

  nofSockets_++;
}

Socket& Socket::operator =(Socket& o) {
  (*o.refCounter_)++;

  refCounter_=o.refCounter_;
  s_         =o.s_;

  nofSockets_++;

  return *this;
}


void Socket::Close() {
  closesocket(s_);
}


String Socket::ReceiveBytes() {
  String ret;
  char buf[1024];
  int rv;

  while ((rv=recv(s_, buf, 1024,0)) == WSAEMSGSIZE) {

	String t;
    t.assign(buf,1024);
    ret +=t;
  }
  String t;
  t.assign(buf, 0, rv);
  ret += t;

  printf("t = %s, rv = %d", t.c_str(), rv);

  return ret;
}

int Socket::ReceiveBytes(BYTE *data, int bytes)
{
  int rv = SOCKET_ERROR;
  while (rv == SOCKET_ERROR)
  {
	rv = recv(s_, (char *)data, bytes, 0);
	if (rv == 0 || rv == WSAECONNRESET)
		return -1;
	else
		break;
  }
  return rv;
}

String Socket::ReceiveLine() {
  String ret;
   while (1) {
     char r;

     switch(recv(s_, &r, 1, 0)) {
       case 0: // not connected anymore;
         return "";
       case -1:
          if (errno == EAGAIN) {
             return ret;
          } else {
            // not connected anymore
           return "";
         }
     }

     ret += r;
     if (r == '\n')  return ret;
   }
}

void Socket::SendLine(String s) {
  s += '\n';
  send(s_,s.c_str(),s.length(),0);
}

void Socket::SendBytes(const String& s) {
  send(s_,s.c_str(),s.length(),0);
}

int Socket::SendBytes(const BYTE* data, int bytes)
{
	if (bytes <= 0) return 0;
	send(s_,(char *)data, bytes, 0);
	return 0;
}

SocketServer::SocketServer() {
  /*
	// create a socket
  s_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }
  */
}


void SocketServer::Accept( Socket*& s,int port, int connections, String& remoteHost ) {

  sockaddr_in sa;
  memset(&sa, 0, sizeof(sa)); /* clear our address */
  sa.sin_family = PF_INET;
  sa.sin_port = htons(port);

  // Use the given socket for listening
  Socket* listenSocket = s;

  // bind the socket to the internet address
  if (bind(listenSocket->s_, (sockaddr *)&sa, sizeof(sockaddr_in)) ==
      SOCKET_ERROR) {
    throw "INVALID_SOCKET";
  }

  listen(listenSocket->s_, connections);

  sockaddr_in remoteAddr;
  int remoteAddrLen = sizeof(sockaddr_in);
  SOCKET new_sock = accept(listenSocket->s_, (sockaddr*) &remoteAddr, &remoteAddrLen);
  if (new_sock == INVALID_SOCKET)
  {
	  throw "Invalid Socket";
  }
  // Return remote host and port to caller
  stringstream msg;
  msg<<inet_ntoa(remoteAddr.sin_addr)<<":"<<ntohs(remoteAddr.sin_port);
  remoteHost.append( msg.str() );


  s = new Socket(new_sock);
  delete listenSocket;
}

void SocketServer::Connect(Socket* s, const char* remoteHost, int remotePort) {

  Socket* connectSocket = s;
  sockaddr_in sa;
  memset(&sa, 0, sizeof(sa)); /* clear our address */
  sa.sin_family = AF_INET;
  if( isalpha(remoteHost[0]) )
  {
    HOSTENT* Hostent;
    if( (Hostent = gethostbyname( remoteHost )) != 0 )
      sa.sin_addr.s_addr = *(long*) Hostent->h_addr;
	else
	  throw "Unknown host!";
  }
  else
    sa.sin_addr.s_addr = inet_addr( remoteHost );

  sa.sin_port = htons( remotePort );

  // Connect to server.
  if ( connect( connectSocket->s_, (SOCKADDR*) &sa, sizeof(sa) ) == SOCKET_ERROR) {
    throw "SOCKET_ERROR";
  }
}


SocketClient::SocketClient(const String& host, int port) : Socket() {
  String error;

  hostent *he;
  if ((he = gethostbyname(host.c_str())) == NULL) {
    error = strerror(errno);
    throw error;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *((in_addr *)he->h_addr);
  memset(&(addr.sin_zero), 0, 8);

  if (::connect(s_, (sockaddr *) &addr, sizeof(sockaddr))) {
    error = strerror(WSAGetLastError());
    throw error;
  }
}


SocketSelect::SocketSelect(Socket const * const s1, Socket const * const s2) {
  FD_ZERO(&fds_);
  FD_SET(const_cast<Socket*>(s1)->s_,&fds_);
  FD_SET(const_cast<Socket*>(s2)->s_,&fds_);

  if (select (0, &fds_, (fd_set*) 0, (fd_set*) 0, (timeval*) 0)
      == SOCKET_ERROR) throw "Error in select";
}


bool SocketSelect::Readable(Socket const * const s) {
  if (FD_ISSET(s->s_,&fds_)) return true;
  return false;
}

// End of the file

