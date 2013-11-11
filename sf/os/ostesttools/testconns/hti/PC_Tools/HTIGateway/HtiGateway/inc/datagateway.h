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
*   This file contains the headers of DataGatewaySOAPServerThread, DataGatewayClientThread and DataGateway
*	classes.
*/

#ifndef DATAGATEWAY_H
#define DATAGATEWAY_H

#pragma warning ( disable : 4786 )

#include <windows.h>
#include <process.h>

#include "util.h"
#include "common.h"
#include "plugin.h"
#include "safequeue.h"
#include "HtiDispatcher.h"
#include "thread.h"

const int g_DataGatewayDefaultTcpIpPort       = 2000;
const int g_DataGatewayDefaultTcpIpBufferSize = 8 * 1024;

// Maximum time to wait
extern long g_MaximumShutdownWaitTime;

extern DWORD g_ErrorCode;

//**********************************************************************************
// Class DataGatewaySOAPServerThread
//
// This thread acts as a SOAP server, it listens to SOAP requests and forwards them
// to HtiDispatcher which then forwards them to correct SOAPHandlers
//**********************************************************************************

class DataGatewaySOAPServerThread : public Thread<DataGatewaySOAPServerThread>
{
public:
	DataGatewaySOAPServerThread(HtiDispatcher* htiDispatcher, int port);
	void Run();
	void Stop();
	bool IsRunning();

private:
	//SafeQueue<Data*>* m_Queue;
	HtiDispatcher* m_HtiDispatcher;
	int m_TcpPort;

	bool m_Running;
};

//**********************************************************************************
// Class DataGatewayClientThread
//
// This thread serves DataGateway's clients
// Gets Data objects from incoming queue and forwards them to CommChannelPlugin.
// The Data objects are actually SOAP requests which were received by DataGatewaySOAPServerThread handled by SOAPHandler and transferred to HtiMessages and eventually Data objects
// The thread also reads incoming data from CommChannelPlugin and forwards them to outgoing queue which
// HtiDispatcher then reads and forwards to correct SOAPHandler
//**********************************************************************************

class DataGatewayClientThread : public Thread<DataGatewayClientThread>
{
public:
	DataGatewayClientThread(int port, long bufsize, const string& commchannel);
	DataGatewayClientThread(int port, long bufsize, CommChannelPlugin** f);
	~DataGatewayClientThread();
	void Run();
	void Stop();

private:
	//incoming queue from PC side, outgoing to CommChannelPlugin side
	SafeQueue<Data*> m_ReaderQueue;
	//outgoing queue to PC side, incoming from CommChannelPlugin side
	SafeQueue<Data*> m_WriterQueue;

	DataGatewaySOAPServerThread m_SoapListener;
	HtiDispatcher m_HtiDispatcher;

	CommChannelPlugin* m_CommChannelPlugin;
	const string& m_CommChannelPluginName;
	bool m_Running;
	bool m_CCLateInit;
	long m_TcpIpBufferSize;
};

//**********************************************************************************
// Class DataGateway
//
// Main class/thread of HtiGateway
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
	const string& m_CommChannelPluginName;
	CommChannelPlugin* m_CommChannelPlugin;
	Event m_ShutdownEvent;
	bool m_Running;
	bool m_StayAlive;
	bool m_CCLateInit;
	int m_TcpIpPort;
	long m_TcpIpBufferSize;
};

#endif

// End of the file