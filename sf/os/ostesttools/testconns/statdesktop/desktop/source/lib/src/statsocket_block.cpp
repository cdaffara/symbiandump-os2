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
* Description:  
*
*/




#include "stdafx.h"
#include "statsocket_block.h"

////////////////////////////////////////////////////////////////////////////////////////
// Constructor
CSTATSocket::CSTATSocket() : pBuffer(NULL), iBufferLength(0)
{
}


////////////////////////////////////////////////////////////////////////////////////////
// Destructor
CSTATSocket::~CSTATSocket()
{
	Disconnect();
	Release();
}


////////////////////////////////////////////////////////////////////////////////////////
// Initialise
int CSTATSocket::Initialise(void)
{
	WORD version;
	WSADATA wsaData;

	// start the windows socket server
	version = MAKEWORD( 2, 2 );
	if (WSAStartup( version, &wsaData ))
		return E_SOCKETSTARTUP;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Connect
int CSTATSocket::Connect(const char *pAddress)
{
	int iPort = ST_DEFAULTDEVICEPORT;

	// parameter will either be MACHINE_NAME:PORT_NUMBER if connecting
	// or just PORT_NUMBER if listening
	char *p = strrchr(pAddress, ':');
	char *q = p;
	if (p)
	{
		(*p) = (char)0;

		p++;
		if (p && *p)
			iPort = atoi(p);

		// local address
		struct hostent *hp;
		memset(&sin,0,sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons((unsigned short)iPort);
		hp = gethostbyname(pAddress);

		// put this back to restore original string
		*q = ':';

		if (hp)
		{

			// create a client socket
			sock = socket( AF_INET, SOCK_STREAM, 0 ) ;
			if( sock == INVALID_SOCKET )
				return E_SOCKETCREATE;

			memcpy(&(sin.sin_addr),hp->h_addr,hp->h_length);

			// connect to server
			if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0)
				return E_SOCKETCONNECT;

			// Set the socket I/O mode
			// FIONBIO enables or disables the blocking mode for the 
			// socket:
			// 0   => blocking is enabled; 
			// !=0 => non-blocking mode is enabled.
			u_long mode = 1;
			ioctlsocket(sock, FIONBIO, &mode);

		}
		else
			return E_SOCKETHOSTNAME;
	}
	else
	{
		// create a listener socket
		listener = socket(AF_INET, SOCK_STREAM, 0);
		if( listener == INVALID_SOCKET )
			return E_SOCKETCREATE;

		iPort = atoi(pAddress);
		if (!iPort)
			iPort = ST_DEFAULTDEVICEPORT;

		sin.sin_family = AF_INET;
		sin.sin_port = htons((unsigned short)iPort);
		sin.sin_addr.s_addr = INADDR_ANY;

		if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) != 0)
			return E_SOCKETBIND;

		if (listen(listener, 1) != 0)
			return E_SOCKETLISTEN;

		int sin_size = sizeof(remote_sin);
		memset((void *)&remote_sin, 0, sizeof(remote_sin));
		sock = accept(listener, (struct sockaddr*)&remote_sin, &sin_size);

		// shut down the listener now we have our connection
		shutdown(listener, SD_SEND);
		shutdown(listener, SD_RECEIVE);
		closesocket(listener);

		if (sock == INVALID_SOCKET)
			return E_OUTOFMEM;
	}

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Send
int CSTATSocket::Send(const char cIdentifier, const char *pData, const unsigned long ulLength)
{
	int ret = ITS_OK;
	unsigned long ID = htonl(cIdentifier);
	unsigned long Length = htonl(ulLength);

	// send the identifier
	ret = send( sock, (char*)&ID, sizeof(unsigned long), 0 );
	if (ret != sizeof(unsigned long))
		return E_SOCKETSEND;

	// send the length
	ret = send( sock, (char*)&Length, sizeof(unsigned long), 0 );
	if (ret != sizeof(unsigned long))
		return E_SOCKETSEND;

	if (pData && ulLength && (ulLength <= GetMaxPacketSize( )) )
	{
		char *pSendData = new char[ulLength + 1];
		if (pSendData)
		{
			memset(pSendData, 0, ulLength + 1);
			memcpy(pSendData, pData, ulLength);
		}
		else
			return E_OUTOFMEM;

		// send the length
		ret = send( sock, pSendData, ulLength, 0 );

		if (pSendData)
			delete [] pSendData;

		if (ret != (int)ulLength)
			return E_SOCKETSEND;
	}

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Receive
int CSTATSocket::Receive(char *cIdentifier, char **ppData, unsigned long *pLength)
{
	int ret = ITS_OK;
	unsigned long ID = 0;
	unsigned long Length = 0;

	TIMEVAL timeout;
	timeout.tv_sec = SOCKETTIMEOUT;
	timeout.tv_usec = 0;

	FD_SET reader;
	
	FD_ZERO(&reader);
	FD_SET(sock, &reader);

	// receive the identifier

	if(select(0, &reader, NULL, NULL, &timeout)==0)
	{
		return E_SOCKETRECV;
	}

	ret = recv( sock, (char*)&ID, sizeof(unsigned long), 0 );
	if (ret != sizeof(unsigned long))
		return E_SOCKETRECV;


	if(select(0, &reader, NULL, NULL, &timeout)==0)
	{
		return E_SOCKETRECV;
	}

	// receive the length
	ret = recv( sock, (char*)&Length, sizeof(unsigned long), 0 );
	if (ret != sizeof(unsigned long))
		return E_SOCKETRECV;

	(*cIdentifier) = (char)ntohl(ID);
	(*pLength) = ntohl(Length);

	// if the length is zero then there is no more to do
	if( *pLength == 0 ) {
		return ITS_OK;
	}

	if( *pLength > GetMaxPacketSize( ) ) {
		SetError( "Received command ID: %c successfully length %d", (*cIdentifier), *pLength );
		return ITS_OK;
	}

	if (*pLength)
	{
		if( *pLength > iBufferLength )
		{
			if( pBuffer ) {
				delete [] pBuffer;
				pBuffer = NULL;
				iBufferLength = 0;
			}
			pBuffer = new char [*pLength + 1];
			if( pBuffer == NULL ) {
				SetError( "Unable to allocate %ld bytes of memory to hold data", *pLength );
				return E_OUTOFMEM;
			}
			memset(pBuffer, 0, (*pLength) + 1);
			iBufferLength = *pLength;
		}

		char *p = pBuffer;

		// receive the data
		int iThisAmount = 0;
		int iTotalReceived = 0;
		while (iTotalReceived < (int)(*pLength))
		{

			if(select(0, &reader, NULL, NULL, &timeout)==0)
			{
				return E_SOCKETRECV;
			}

			iThisAmount = recv( sock, p, (*pLength) - iTotalReceived, 0 );
			if (iThisAmount == SOCKET_ERROR)
				return E_SOCKETRECV;

			iTotalReceived += iThisAmount;
			p += iThisAmount;
			Sleep(100);
		}

		(*ppData) = pBuffer;
	}

	return ITS_OK;
}
	
	
////////////////////////////////////////////////////////////////////////////////////////
// Disconnect
int CSTATSocket::Disconnect(void)
{
	// release previous resources
	if (pBuffer)
	{
		delete [] pBuffer;
		pBuffer = NULL;
		iBufferLength = 0;
	}

	shutdown(sock, SD_SEND);
	shutdown(sock, SD_RECEIVE);

	if (closesocket(sock) == SOCKET_ERROR)
		return E_SOCKETCLOSE;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int CSTATSocket::Release(void)
{
	if (WSACleanup() == SOCKET_ERROR)
		return E_SOCKETSHUTDOWN;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
//	PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////
