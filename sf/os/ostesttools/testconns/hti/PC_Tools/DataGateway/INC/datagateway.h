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
*   This file contains the headers of DataGatewaySocketWriterThread,
*	DataGatewaySocketReaderThread, DataGatewayClientThread and DataGateway
*	classes.
*/

#ifndef DATAGATEWAY_H
#define DATAGATEWAY_H

#pragma warning ( disable : 4786 )

#include "Socket.h"

#include <windows.h>
#include <process.h>

#include "util.h"
#include "common.h"
#include "plugin.h"
#include "safequeue.h"
#include "thread.h"

const int g_DataGatewayDefaultTcpIpPort       = 2000;
const int g_DataGatewayDefaultTcpIpBufferSize = 8 * 1024;

// Maximum time to wait
extern long g_MaximumShutdownWaitTime;

extern DWORD g_ErrorCode;

//**********************************************************************************
// Class DataGatewaySocketWriterThread
//
// This thread is used to read Data from outgoing queue 
// and write it to Socket(connected to program using DataGateway)
//**********************************************************************************

class DataGatewaySocketWriterThread : public Thread<DataGatewaySocketWriterThread>
{
public:
	DataGatewaySocketWriterThread(SafeQueue<Data*>* q, Socket* s);
	/*
	 * Main loop of thread
	 * Reads Data from outgoing queue and writes it to Socket(connected to program using DataGateway)
	 */
	void Run();
	void Stop();
	bool IsRunning();

private:
	//outgoing queue
	SafeQueue<Data*>* m_Queue;
	//socket to which software using DataGateway is connected to
	Socket* m_Socket;
	bool m_Running;
};

//**********************************************************************************
// Class DataGatewaySocketReaderThread
//
// This thread is used to read incoming bytes from Socket(connected to program using DataGateway)
// which it then encapsulates into Data objects and forwards to outgoing queue
//**********************************************************************************

class DataGatewaySocketReaderThread : public Thread<DataGatewaySocketReaderThread>
{
public:
	DataGatewaySocketReaderThread(SafeQueue<Data*>* q, long bufsize, Socket* s);
	/*
	 * Main loop of thread
	 * Reads bytes from Socket(connected to program using DataGateway), encapsulates them to Data object and puts these to outgoing queue
	 */
	void Run();
	void Stop();
	bool IsRunning();

private:
	//incoming queue
	SafeQueue<Data*>* m_Queue;
	//socket to which software using DataGateway is connected to
	Socket* m_Socket;
	bool m_Running;
	long m_TcpIpBufferSize;
};

//**********************************************************************************
// Class DataGatewayClientThread
//
// This thread serves DataGateway's clients
// Gets Data from incoming queue to which DataGatewaySocketReader has pushed it and forwards
// them to CommChannelPlugin.
// The thread also reads incoming data from CommChannelPlugin and forwards them to outgoing queue which
// DataGatewaySocketWriter then reads
//**********************************************************************************

class DataGatewayClientThread : public Thread<DataGatewayClientThread>
{
public:
	DataGatewayClientThread(Socket** s, long bufsize, const string& commchannel);
	DataGatewayClientThread(Socket** s, long bufsize, CommChannelPlugin** f);
	~DataGatewayClientThread();
	/*
	 * Main loop of thread
	 * Gets Data from incoming queue to which DataGatewaySocketReader has pushed it and forwards
	 * them to CommChannelPlugin.
	 * Reads incoming data from CommChannelPlugin and forwards them to outgoing queue which
	 * DataGatewaySocketWriter then reads
	 */
	void Run();
	void Stop();

private:
	//incoming queue
	SafeQueue<Data*> m_ReaderQueue;
	//outgoing queue
	SafeQueue<Data*> m_WriterQueue;
	//used to read data from socket to incoming queue
	DataGatewaySocketReaderThread m_ReaderThread;
	//used to write data from outgoing queue to socket
	DataGatewaySocketWriterThread m_WriterThread;
	//Socket to which software using DataGateway is connected to
	Socket** m_Socket;
	//The CommChannelPlugin that is used in communication
	CommChannelPlugin* m_CommChannelPlugin;
	//determines which CommChannelPlugin is loaded
	const string& m_CommChannelPluginName;
	bool m_Running;
	//This value states whether or not Communication Channel Plugin uses late initialization
	bool m_CCLateInit;
	//This value tells the buffer size that is used
	long m_TcpIpBufferSize;
};

//**********************************************************************************
// Class DataGateway
//
// This is the main thread of DataGateway
//**********************************************************************************

class DataGateway : public Thread<DataGateway>
{
public:
	DataGateway(int port = g_DataGatewayDefaultTcpIpPort,
		        long bufsize = g_DataGatewayDefaultTcpIpBufferSize,
				const string& commchannel = "",
				bool stayalive = false,
				bool cclateinit = false);
	void Run();
	void Stop();
	bool IsRunning();

private:
	//determines which CommChannelPlugin is loaded
	const string& m_CommChannelPluginName;
	//reference to CommChannelPlugin
	CommChannelPlugin* m_CommChannelPlugin;
	Event m_ShutdownEvent;
	bool m_Running;
	//tells whether DataGateway should stay alive after first client has disconnected
	bool m_StayAlive;
	//this value states whether or not Communication Channel Plugin uses late initialization
	bool m_CCLateInit;
	//the port which DataGateway listens for incoming connections
    int m_TcpIpPort;
	//buffer size used in communication
    long m_TcpIpBufferSize;
};

#endif

// End of the file