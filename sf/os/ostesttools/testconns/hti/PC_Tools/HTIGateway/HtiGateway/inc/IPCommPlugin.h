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
*   This file contains headers of IPCommPlugin,
*	IPCommReaderThread, IPCommWriterThread and IPCommMonitorThread classes.
*/

#ifndef IPCOMMPLUGIN_H
#define IPCOMMPLUGIN_H

//#pragma warning ( disable : 4786 )
#include <iostream>
#include <process.h>
#include <string>
#include <map>
#include <comdef.h>

#include "common.h"
#include "plugin.h"
#include "safequeue.h"
#include "thread.h"
#include "datagateway.h" // for g_DataGatewayDefaultTcpIpBufferSize

// forward declarations
class Socket;

using namespace std;

#define IP_INI_FILE_NAME             "ipcommplugin.ini"

#define IP_INI_LOCAL_PORT_PARAM             "LOCAL_PORT"
#define IP_INI_REMOTE_HOST_PARAM            "REMOTE_HOST"
#define IP_INI_REMOTE_PORT_PARAM            "REMOTE_PORT"
#define IP_INI_RECV_BUFFER_SIZE_PARAM       "RECV_BUFFER_SIZE"

#define PARAM_SWITCH_LOCAL_PORT_PARAM       "-LOCAL_PORT"
#define PARAM_SWITCH_REMOTE_HOST_PARAM      "-REMOTE_HOST"
#define PARAM_SWITCH_REMOTE_PORT_PARAM      "-REMOTE_PORT"
#define PARAM_SWITCH_RECV_BUFFER_SIZE_PARAM "-RECV_BUFFER_SIZE"

const DWORD g_IPCommDefaultTimeout    = 40000; //ms
const DWORD g_IPPluginTimeout         = 10000; //not used

// Maximum time to wait
extern long g_MaximumShutdownWaitTime;

//**********************************************************************************
// Class IPCommReaderThread
//
// This thread is used to read bytes from TCP/IP socket, encapsulate the bytes to Data objects 
// and push them to incoming queue 
//**********************************************************************************

class IPCommReaderThread : public Thread<IPCommReaderThread>
{
public:
	IPCommReaderThread(SafeQueue<Data*>* q, long bufsize);
	/*
	 * Main execution loop which reads bytes from socket, encapsulates the bytes to Data object and pushes them to incoming queue
	 */		
	void Run();
	void Stop();
	bool IsRunning();

private:
	friend class IPCommMonitorThread;
	//incoming queue
	SafeQueue<Data*>* m_Queue;
	//Socket connected to mobile device
	Socket* m_Socket;
	bool m_Running;
	//size of buffer used when reading data from socket
	long m_TcpIpBufferSize;
};

//**********************************************************************************
// Class IPCommWriterThread
//
// This thread is used to write data from outgoing queue to TCP/IP port
//**********************************************************************************

class IPCommWriterThread : public Thread<IPCommWriterThread>
{
public:
	IPCommWriterThread(SafeQueue<Data*>* q);
	/*
	 * Main execution loop which gets Data from outgoing queue and sends it to socket
	 */	
	void Run();
	void Stop();
	bool IsRunning();

private:
	friend class IPCommMonitorThread;
	//outgoing queue
	SafeQueue<Data*>* m_Queue;
	//Socket connected to mobile device	
	Socket* m_Socket;
	bool m_Running;
};

//**********************************************************************************
// Class IPCommMonitorThread
//
// This thread creates and starts reader and writer threads
// The thread also monitors if reader and writer threads are running and restarts them in case either isn't running
//**********************************************************************************

class IPCommMonitorThread : public Thread<IPCommMonitorThread>
{
public:
	IPCommMonitorThread(SafeQueue<Data*>* TxQueue,
		                SafeQueue<Data*>* RxQueue,
						int LocalPort,
						string& RemoteHost,
						int RemotePort,
						long RecvBufferSize);
	~IPCommMonitorThread();
	/*
	 * Main execution loop of thread
	 * -Creates reader and writer threads and starts them
	 * -Monitors if either reader or writer thread aren't running and restarts them if not
	 */	
	void Run();
	void Stop();
	bool IsRunning();

private:
	/*
	 * This method has two functionalities
	 * -It waits for incoming connections if local port is defined
	 * -It tries to connect to remote host if local host is not defined
	 */
	void Connect(Socket*& s);

private:
	bool                m_Running;
	//outgoing queue
	SafeQueue<Data*>*   m_TxQueue;
	//incoming queue
	SafeQueue<Data*>*   m_RxQueue;
	//used to read data from TCP/IP port and push them to incoming queue
	IPCommReaderThread* m_ReaderThread;
	//used to write data from outgoing queue to TCP/IP port	
	IPCommWriterThread* m_WriterThread;
	//port that thread should listen to
	int					m_LocalPort;
	//hostname that thread should connect to	
	string&				m_RemoteHost;
	//Port that thread should connect to
	int					m_RemotePort;
	long				m_RecvBufferSize;
};

//**********************************************************************************
// Class IPCommPlugin
//
// This class implements a CommChannelPlugin used in TCP/IP communication with device
//**********************************************************************************

class IPCommPlugin : public CommChannelPlugin
{
public:
	/*
	 * This method initializes IPCommPlugin and Starts IPCommMonitorThread
	 */
	DWORD Init();
	/*
	 * This method is used to push given data to outgoing queue and then 
	 * wait for data to become available and read all data into single Data object 
	 */	
	DWORD SendReceive( Data* data_in, Data** data_out, long timeout = g_IPPluginTimeout );
	/*
	 * This method pushes the given Data object(of type Data::EData) to outgoing queue
	 */		
	DWORD Send( Data* data_in, long timeout = g_IPPluginTimeout );
	/*
	 * This method is used to read all data in incoming queue to single Data object and store the result
	 * to the data object given parameter
	 */		
	DWORD Receive( Data** data_out, long timeout = g_IPPluginTimeout );
	/*
	 * This method is used to wait for data to become available in incoming queue 
	 * and then read all data into single Data object which is given as parameter
	 */		
	DWORD ReceiveWait( Data** data_out, long timeout = g_IPPluginTimeout );
	/*
	 * This method checks if data is available on incoming queue
	 */		
	bool IsDataAvailable();
	DWORD Open();
	DWORD Close();
	IPCommPlugin( const CommChannelPluginObserver* observer = NULL );
	~IPCommPlugin();

private:
	/*
	 * This method initializes class member variables from values in map
	 */	
	void CheckProperties( map<string, string>& props );

private:
	//outgoing queue
	SafeQueue<Data*>	m_TxQueue;
	//incoming queue
	SafeQueue<Data*>	m_RxQueue;
	//thread which starts and monitors reader and writer threads
	IPCommMonitorThread* m_MonitorThread;
	//port that IPCommPlugin should listen to
	int					m_PropertyLocalPort;
	//hostname that IPCommPlugin should connect to
	string				m_PropertyRemoteHost;
	//port that IPCommPlugin should connect to
	int					m_PropertyRemotePort;
	long                m_PropertyRecvBufferSize;
};

#endif

// End of the file