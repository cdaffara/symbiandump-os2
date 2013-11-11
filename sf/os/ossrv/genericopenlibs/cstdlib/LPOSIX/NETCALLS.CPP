// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// connectors for re-entrant networking system calls
// 
//

#include "SYSIF.H"
#include "LPOSIX.H"

#include <reent.h>
#include <sys/socket.h>

extern "C" {

/**
@param family Specifies the address family used in the communications domain.	
@param style Type of socket.
@param protocol Protocol used with that socket.

@return On Success, non-negative integer, the socket file descriptor.
		On Failure, returns -1, eerno may be set.
*/
EXPORT_C int socket (int family, int style, int protocol)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _socket_r (r, family, style, protocol);
	}
EXPORT_C int _socket_r (struct _reent *r, int family, int style, int protocol)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.socket(family,style,protocol,r->_errno);
	}

/**
Receives a message from a socket. 
The recv() call can be used on a connection mode socket or a bound, 
connectionless socket. If no messages are available at the socket, 
the recv() call waits for a message to arrive unless the socket is nonblocking. 

@param fd Specifies a socket descriptor to use for the send.
@param buf Points to the buffer containing message to send.
@param cnt Specifies the length of the message in bytes.
@param flags Lets the sender control the way data is sent. 

@return On Success, returns the number of bytes received.
		On Failure, returns -1, eerno may be set.
*/
EXPORT_C int recv (int fd, char *buf, size_t cnt, int flags)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _recvfrom_r (r, fd, buf, cnt, flags, 0, 0);
	}

/**
@return On Succcess, returns length of message in bytes, can be 0. 
		On Failure, returns -1, errno may be set.
*/	
EXPORT_C int recvfrom (int fd, char *buf, size_t cnt, int flags, struct sockaddr* from, size_t* fromsize)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _recvfrom_r (r, fd, buf, cnt, flags, from ,fromsize);
	}
EXPORT_C int _recvfrom_r (struct _reent *r, int fd, char *buf, size_t nbyte, int flags, struct sockaddr* from, size_t* fromsize)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.recvfrom(fd,buf,nbyte,flags,from,(unsigned long*)fromsize,r->_errno);
	}

/**
Initiates transmission of a message from the specified socket to its peer.
The send() function sends a message only when the socket is connected.

@param fd Specifies a socket descriptor to use for the send.
@param buf Points to the buffer containing message to send.
@param cnt Specifies the length of the message in bytes.
@param flags Lets the sender control the way data is sent.

@return On Success, returns the numbers of characters sent.
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int send (int fd, const char *buf, size_t cnt, int flags)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _sendto_r (r, fd, buf, cnt, flags, 0, 0);
	}

/**
@return	On Success, returns the numbers of characters sent.
		On Failure, returns -1, errno may be set.
*/	
EXPORT_C int sendto (int fd, const char *buf, size_t cnt, int flags, struct sockaddr* to, size_t tosize)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _sendto_r (r, fd, buf, cnt, flags, to, tosize);
	}
EXPORT_C int _sendto_r (struct _reent *r, int fd, const char *buf, size_t nbyte, int flags, struct sockaddr* to, size_t tosize)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.sendto(fd,buf,nbyte,flags,to,tosize,r->_errno);
	}

/**
Shuts down all or part of a full-duplex connection on the socket associated with fd.

@param fd Is the socket descriptor to shut down.
@param how Specifies the type of shutdown.

@return On Success, returns 0.
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int shutdown (int fd, int how)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _shutdown_r (r, fd, how);
	}
EXPORT_C int _shutdown_r (struct _reent *r, int fd, int how)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.shutdown(fd,how,r->_errno);
	}

/**
Marks a connection-mode socket, specified by the socket argument fd,
as accepting connections, and limits the number of outstanding connections 
in the socket's listen queue to the value specified by the n argument. 
The socket fd is put into 'passive' mode where incoming connection 
requests are acknowledged and queued pending acceptance by the process.

@param fd Is a descriptor identifying a bound, unconnected socket.
@param n Is the maximum length that the queue of pending connections
	   may grow to. If this value is SOMAXCONN, then the underlying service provider
       responsible for socket fd sets the backlog to a maximum "reasonable" value.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int listen (int fd, int n)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _listen_r (r, fd, n);
	}
EXPORT_C int _listen_r (struct _reent *r, int fd, int n)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.listen(fd,n,r->_errno);
	}

/**
accepts a connection on a socket. An incoming connection is acknowledged and associated with
an immediately created socket. The original socket is returned to the listening state.

@param fd Is the integer descriptor of the desired socket.
@param addr Points to a sockaddr structure containing the socket address.
@param size Points to an integer that states the address length in bytes.

@return On Success, returns a non-negative integer, which is a descriptor of the accepted socket.
        Upon return, addrlen contains the actual length in bytes of the address returned. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int accept (int fd, struct sockaddr *addr, size_t *size)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _accept_r (r, fd, addr, size);
	}
EXPORT_C int _accept_r (struct _reent *r, int fd, struct sockaddr *addr, size_t *size)
	{
	MSystemInterface& sysIf=Interface(r);
	int newfd=sysIf.accept(fd,r->_errno);
	if (newfd>=0)
		sysIf.sockname(newfd,addr,(unsigned long*)size,1,r->_errno);	// getpeername
	return newfd;
	}

/**
Associate that socket with a port.

@param fd Is the integer descriptor of the desired socket.
@param addr Points to a sockaddr structure containing the socket address.
@param size Points to an integer that states the address length in bytes.

@return	On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int bind (int fd, struct sockaddr *addr, size_t size)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).	
	return _bind_r (r, fd, addr, size);
	}
EXPORT_C int _bind_r (struct _reent *r, int fd, struct sockaddr *addr, size_t size)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.bind(fd,addr,size,r->_errno);
	}

/**
Used by a client program to establish communication with a remote entity

@param fd Is the integer descriptor of the desired socket.
@param addr Points to a sockaddr structure containing the socket address.
@param size Points to an integer that states the address length in bytes.

@return	On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int connect (int fd, struct sockaddr *addr, size_t size)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _connect_r (r, fd, addr, size);
	}
EXPORT_C int _connect_r (struct _reent *r, int fd, struct sockaddr *addr, size_t size)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.connect(fd,addr,size,r->_errno);
	}

/**
Returns the current name for the specified socket. The namelen parameter should be initialized
to indicate the amount of space pointed to by name. When returned, namelen contains the actual
size (in bytes) of the name returned.

@param fd Is the integer descriptor of the desired socket.
@param addr Points to a sockaddr structure containing the socket address.
@param size Points to an integer that states the address length in bytes.

@return	On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int getsockname (int fd, struct sockaddr *addr, size_t* size)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _getsockname_r (r, fd, addr, size);
	}
EXPORT_C int _getsockname_r (struct _reent *r, int fd, struct sockaddr *addr, size_t* size)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.sockname(fd,addr,(unsigned long*)size,0,r->_errno);
	}

/**
Returns the peer address of the specified socket.

@return	On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int getpeername (int fd, struct sockaddr *addr, size_t* size)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _getpeername_r (r, fd, addr, size);
	}
EXPORT_C int _getpeername_r (struct _reent *r, int fd, struct sockaddr *addr, size_t* size)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.sockname(fd,addr,(unsigned long*)size,1,r->_errno);
	}

/**
Manipulates options associated with a socket.

@return	On Success, returns 0. 
   		On Failure, returns -1, errno may be set.
*/
EXPORT_C int getsockopt (int fd, int level, int opt, void* buf, size_t* len)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _getsockopt_r (r, fd, level, opt, buf, len);
	}
EXPORT_C int _getsockopt_r (struct _reent *r, int fd, int level, int opt, void* buf, size_t* len) 
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.getsockopt(fd,level,opt,buf,(unsigned long*)len,r->_errno);
	}

/**
Manipulates options associated with a socket. Options can exist at multiple protocol levels.
However, the options are always present at the uppermost socket level. Options affect socket
operations, such as the routing of packets, out-of-band data transfer, and so on.

@param fd Specifies a socket for which an option should be set.
@param level Identifies whether the operation applies to the socket itself or to the underlying
	   protocol being used. The socket itself is identified by the symbolic constant SOL_SOCKET.
	   Other protocol levels require the protocol number for the appropriate protocol
	   controlling the option.
@param opt Specifies a single option to which the request applies, 
            negative option values are not support on Symbian OS.
@param buf Specifies a value for the option.
@param len Specifies the length of the option value.

@return	On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int setsockopt (int fd, int level, int opt, void* buf, size_t len)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _setsockopt_r (r, fd, level, opt, buf, len);
	}
EXPORT_C int _setsockopt_r (struct _reent *r, int fd, int level, int opt, void* buf, size_t len) 
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.setsockopt(fd,level,opt,buf,len,r->_errno);
	}


} // extern "C"
