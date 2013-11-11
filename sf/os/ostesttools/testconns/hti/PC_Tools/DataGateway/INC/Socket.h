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
*   This file contains headers of Socket, SocketClient, SocketServer and
*   SocketSelect classes.
*/

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <WinSock2.h>

#include <string>


typedef std::string String;

//**********************************************************************************
// Class Socket
//
// This class encapsulates Windows Sockets API Socket functionality
//**********************************************************************************

class Socket {
public:
	Socket(SOCKET s);
	Socket();
	virtual ~Socket();
	Socket(const Socket&);
	Socket& operator=(Socket&);

	/*
	 * This method is used to read characters from socket until line separator is encountered
	 * Returns String including line separator
	 */
	String ReceiveLine();
	/*
	 * This method is used to read bytes from socket. The method reads all available and combines them to returned String object
	 */
	String ReceiveBytes();
	/*
	 * This method reads up to maximum amount of bytes from socket to the Data object given as parameter
	 * The maximum amount is given as parameter
	 */
	int ReceiveBytes(BYTE *data, int bytes);

	void   Close();

	/*
	 * This method is used to send String as a line
	 * The parameter of SendLine is not a const reference
	 * because SendLine modifes the String passed.
	 */
	void   SendLine (String);

	/*
	 * This method is used to send String as bytes
	 * The parameter is a const reference
	 * because the methos does not modify the String passed 
	 * (in contrast to SendLine).
	 */
	void   SendBytes(const String&);
	/*
	 * This method is used to send specific amount of bytes of String
	 * The parameter is a const reference
	 * because the methos does not modify the String passed 
	 * (in contrast to SendLine).
	 */
	int SendBytes(const BYTE* data, int bytes);

protected:
	friend class SocketServer;
	friend class SocketSelect;
	//winsock socket
	SOCKET s_;

	int* refCounter_;

private:
	static void Start();
	static void End();
	static int  nofSockets_;
};

//**********************************************************************************
// Class SocketClient
//
//**********************************************************************************

class SocketClient : public Socket {
public:
  SocketClient(const String& host, int port);
};

//**********************************************************************************
// Class SocketServer
//
// This class is used to:
// -listen for incoming connections to a Socket
// -connect to socket
//**********************************************************************************

class SocketServer// : public Socket 
{
public:
	SocketServer();
	/*
	 * This method binds and opens the given socket to listening mode 
	 */
	void Accept(Socket*& s, int port, int connections, String& remoteHost);
	/*
	 * This method establishes a connection to the socket
	 */
	void Connect(Socket* s, const char* remoteHost, int remotePort);
};

//**********************************************************************************
// Class SocketSelect
//
//**********************************************************************************

class SocketSelect {
  public:
    SocketSelect(Socket const * const s1, Socket const * const s2);

    bool Readable(Socket const * const s);

  private:
    fd_set fds_;
}; 


#endif
