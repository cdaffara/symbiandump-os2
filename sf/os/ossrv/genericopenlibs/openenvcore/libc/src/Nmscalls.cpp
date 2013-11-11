/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: connectors for re-entrant networking system calls
*/
// connectors for re-entrant networking system calls

#include "sysif.h"
#include "fdesc.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "lposix.h"
#include "sysreent.h"
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include <string.h>
#include <es_sock.h>

#include "netdb_r.h"
#include "sysusrinclude.h"

extern "C" {

EXPORT_C int socket (int family, int style, int protocol)
	{
	return _socket_r(&errno, family, style, protocol);
	}

/*
Receives a message from a socket. 
The recv() call can be used on a connection mode socket or a bound, 
connectionless socket. If no messages are available at the socket, 
the recv() call waits for a message to arrive unless the socket is nonblocking. 
*/
EXPORT_C int recv (int fd, void *buf, size_t cnt, int flags)
	{
	return _recvfrom_r(&errno, fd, (char*) buf, cnt, flags, 0, 0);
	}


EXPORT_C int recvfrom (int fd, void *buf, size_t cnt, int flags, struct sockaddr* from, size_t* fromsize)
	{
	 return _recvfrom_r(&errno, fd, (char*) buf, cnt, flags, from, fromsize);
	}
	

/*
initiates transmission of a message from the specified socket to its peer.
The send() function sends a message only when the socket is connected.
*/
EXPORT_C int send (int fd, const void *buf, size_t cnt, int flags)
	{
	 return _sendto_r(&errno, fd, (char*) buf, cnt, flags, 0, 0);
  	}


EXPORT_C int sendto (int fd, const void *buf, size_t cnt, int flags, const struct sockaddr* to, size_t tosize)
	{
	return _sendto_r(&errno, fd, (const char*) buf, cnt, flags, (struct sockaddr*) to, tosize);
 	}


/*
shuts down all or part of a full-duplex connection on the socket
associated with fd
*/
EXPORT_C int shutdown (int fd, int how)
	{
	return _shutdown_r(&errno, fd , how) ;
	}

/*
Marks a connection-mode socket, specified by the socket argument fd,
as accepting connections, and limits the number of outstanding connections 
in the socket's listen queue to the value specified by the n argument. 
The socket fd is put into 'passive' mode where incoming connection 
requests are acknowledged and queued pending acceptance by the process.
*/
EXPORT_C int listen (int fd, int n)
	{
	return _listen_r(&errno, fd, n);
	}

/*
accepts a connection on a socket. An incoming connection is acknowledged and associated with an immediately created socket. The original socket is returned to the listening state.
*/
EXPORT_C int accept (int fd, struct sockaddr *addr, size_t *size)
	{
	return _accept_r(&errno, fd , addr , size);
	}


/*
Associate that socket with a port.
*/
EXPORT_C int bind (int fd, const struct sockaddr *addr, size_t size)
	{
	return _bind_r(&errno, fd, addr, size);
	}
	

/*
Used by a client program to establish communication with a remote entity
*/
EXPORT_C int connect (int fd, const  struct sockaddr *addr, size_t size)
	{
	return _connect_r(&errno, fd, addr, size);
	}
	

/*
returns the current name for the specified socket. The namelen parameter should be initialized to indicate the amount of space pointed to by name. When returned, namelen contains the actual size (in bytes) of the name returned.
*/
EXPORT_C int getsockname (int fd, struct sockaddr *addr, size_t* size)
	{
	return _getsockname_r (&errno, fd, addr, size);
	}


EXPORT_C int getpeername (int fd, struct sockaddr *addr, size_t* size)
	{
	return _getpeername_r(&errno, fd, addr, size);
	}
	

/*
Manipulates options associated with a socket.
*/
EXPORT_C int getsockopt (int fd, int level, int opt, void* buf, size_t* len)
	{
	return _getsockopt_r(&errno, fd, level, opt, buf, len);
	}
	

/*
manipulates options associated with a socket. Options can exist at multiple protocol levels. However, the options are always present at the uppermost socket level. Options affect socket operations, such as the routing of packets, out-of-band data transfer, and so on.
*/
EXPORT_C int setsockopt (int fd, int level, int opt, const  void* buf, size_t len)
	{
	return _setsockopt_r(&errno , fd, level, opt, (void *)buf, len);
	}
	

EXPORT_C  uint16_t htons(uint16_t hs)
	{
	return  ByteOrder::Swap16(hs);
	}

EXPORT_C uint32_t htonl(uint32_t hl)
	{
	return ByteOrder::Swap32(hl);
	}


/*
Get the internet name of this host. Actually this will always return a null 
string with TCPIP 030 and onwards because the "name" of a mobile host
isn't really very meaningful - in practice the IP address is chosen dynamically
once you start doing real networking, at which time the ISP can resolve the 
IP address into a name of some sort if you really want.
*/
EXPORT_C int gethostname(char *name, size_t size) 
	{	
	return _gethostname(name, size);
	}
	

/*
Get the internet name of the host by address.
*/
EXPORT_C struct hostent* gethostbyaddr (const char* addr, int length, int format)
	{
	return _gethostbyaddr_r(_REENT, addr, length, format);
	}

/*
Get the internet name of the host by name.
*/
EXPORT_C struct hostent* gethostbyname (const char* name)
	{
	if (name == NULL)
		{
		h_errno = HOST_NOT_FOUND;
		return NULL;
		}
	return _gethostbyname_r(_REENT, name);
	}

static protoent* reent_function(struct _reent* rp)
	{
	struct protoent *p=NULL;
	if (rp->_netdb)
		{
		p = (struct protoent*)(rp->_netdb);
		BackendFree(p->p_name);
		BackendFree(p);
		}

	p = (protoent*)BackendAlloc(sizeof(protoent));
	rp->_netdb = p;
	return p;
	}

int _gethostname (char* name, size_t size)
	{
	if (name == NULL || size == 0)
		{
		errno = EFAULT;
		return -1;
		}
		
	return _gethostname_r(&errno, name, size);
	}

EXPORT_C protoent* getprotobyname(const char* name)
	{
	if (name == NULL)
		{
		// set errno to something?
		return NULL;
		}

	TProtocolDesc protoInfo;
	if (_getprotobyname_r(&errno, name, &protoInfo) == -1)
		{
		return NULL;
		}
	
	protoent* p = reent_function(_REENT);
	p->p_aliases = NULL;

	const int BUFSIZE = 128;
	
	char buf[BUFSIZE] ;
	size_t nbytes;
	
	if((nbytes=wcstombs(buf,(wchar_t*)protoInfo.iName.PtrZ(),BUFSIZE)) == (size_t)-1)
		return NULL;
	
	p->p_name = (char*)BackendAlloc(nbytes+1);
	if (!p->p_name)
		{
		return NULL;
		}
	
	strncpy(p->p_name, buf,nbytes+1);
	p->p_proto = protoInfo.iProtocol;
	
	return p;    
	}


EXPORT_C protoent* getprotobynumber(TInt proto)
	{
	TProtocolDesc protoInfo;
	if (_getprotobynumber_r(&errno, proto, &protoInfo) == -1)
		{
		return NULL;
		}
	
    protoent* p = reent_function(_REENT);
	p->p_aliases = NULL;
	
	const int BUFSIZE = 128;
	
	char buf[BUFSIZE] ;
	size_t nbytes;
	
	if((nbytes=wcstombs(buf,(wchar_t*)protoInfo.iName.PtrZ(),BUFSIZE)) == (size_t)-1)
		return NULL;
	
	p->p_name = (char*)BackendAlloc(nbytes+1);
	if (!p->p_name)
		{
		return NULL;
		}
	strncpy(p->p_name, buf,nbytes+1);
	p->p_proto = protoInfo.iProtocol;
	
	return p;
	}


EXPORT_C int setdefaultif( const struct ifreq* ifReq )
	{
	return _setdefaultif_r(&errno,ifReq);
	}

EXPORT_C int unsetdefaultif()
    {
    return _unsetdefaultif_r(&errno);
    }
	
} // extern "C"
