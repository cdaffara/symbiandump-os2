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
*   This file contains the header file of the IPCommPlugin,
*	IPCommReaderThread, IPCommWriterThread and IPCommMonitorThread classes.
*/

// INCLUDES
#include "socket.h"
#include "IPCommPlugin.h"
#include "util.h"


const static int g_IPMaxResendNumber = 2;

//**********************************************************************************
// Class IPCommPlugin
//
// This class implements a CommChannelPlugin which is used to communicate with device using TCP/IP
//**********************************************************************************

IPCommPlugin::IPCommPlugin(const CommChannelPluginObserver* observer)
    : CommChannelPlugin(observer),
      m_TxQueue(),
      m_RxQueue(),
	  m_PropertyLocalPort(0),
	  m_PropertyRemotePort(0)
{
    m_MonitorThread = NULL;
}

IPCommPlugin::~IPCommPlugin()
{
    Util::Debug("IPCommPlugin::~IPCommPlugin()");
    if (m_Open)
    {
        Close();
    }
    if (m_MonitorThread != NULL)
    {
        delete m_MonitorThread;
        m_MonitorThread = NULL;
    }
}

/*
 * This method initializes IPCommPlugin and Starts IPCommMonitorThread
 */
DWORD IPCommPlugin::Init()
{
    Util::Debug("IPCommPlugin::Init()");

    std::string filename = IP_INI_FILE_NAME;
	map<string, string> IPCommPluginProperties;
	Util::ReadProperties(filename.c_str(), IPCommPluginProperties);

	CheckProperties(IPCommPluginProperties);

    CommChannelPlugin::Init();

	m_MonitorThread = new IPCommMonitorThread(&m_TxQueue,
		                                      &m_RxQueue,
											  m_PropertyLocalPort,
											  m_PropertyRemoteHost,
											  m_PropertyRemotePort,
											  m_PropertyRecvBufferSize);

	m_MonitorThread->Start();

    Util::Debug("IPCommPlugin::Init() IPComm opened");
    m_Open = true;
    Util::Debug("IPCommPlugin::Init() OK");
    return NO_ERRORS;
}

/*
 * This method initializes class member variables from values in map
 */
void IPCommPlugin::CheckProperties(map<string, string>& props)
{
    char tmp[256];

    // Local port
    string val = props[IP_INI_LOCAL_PORT_PARAM];
	Util::CheckCommandlineParam( PARAM_SWITCH_LOCAL_PORT_PARAM, val );
    if (!val.empty())
    {
        m_PropertyLocalPort = atol(val.c_str());
    }

    // Receive TCP/IP buffer size
    val = props[IP_INI_RECV_BUFFER_SIZE_PARAM];
	Util::CheckCommandlineParam( PARAM_SWITCH_RECV_BUFFER_SIZE_PARAM, val );
    if (!val.empty())
    {
        m_PropertyRecvBufferSize = atol(val.c_str());
    }
	else
	{
		// Use 8*1024 bytes (8KB) as default value
		m_PropertyRecvBufferSize = 8*1024; 
	}

	if( m_PropertyLocalPort )
	{
		sprintf(tmp, "[IPComm] Local port : %d", m_PropertyLocalPort );
		string s(tmp);
		Util::Info(s);
	}
	else
	{
		// Remote host
		m_PropertyRemoteHost = props[IP_INI_REMOTE_HOST_PARAM];
		//Check and replace if -REMOTE_HOST was given as command line parameter
		Util::CheckCommandlineParam( PARAM_SWITCH_REMOTE_HOST_PARAM, m_PropertyRemoteHost );
		if(m_PropertyRemoteHost.empty())
		{
			throw "No remote host specified!";
		}
		sprintf(tmp, "[IPComm] Remote host : '%s'", m_PropertyRemoteHost.c_str());
		string s = tmp;
		Util::Info(s);

		// Remote port
		val = props[IP_INI_REMOTE_PORT_PARAM];
		//Check and replace if -REMOTE_POST was given as command line parameter		
		Util::CheckCommandlineParam( PARAM_SWITCH_REMOTE_PORT_PARAM, val );
		if (!val.empty())
		{
			m_PropertyRemotePort = atol(val.c_str());
		}
		if( m_PropertyRemotePort == 0)
			throw "Invalid remote port specified!";

		sprintf(tmp, "[IPComm] Remote port : %d", m_PropertyRemotePort );
		s = tmp;
		Util::Info(s);
	}
}

/*
 * This method checks if data is available on incoming queue
 */
bool IPCommPlugin::IsDataAvailable()
{
    return (!m_RxQueue.empty());
}

/*
 * This method is used to push given data to outgoing queue and then 
 * wait for data to become available and read all data into single Data object 
 */
DWORD IPCommPlugin::SendReceive(Data* data_in, Data** data_out, long timeout)
{
    DWORD res;
    if ((res = Send(data_in, timeout)) == NO_ERRORS &&
        (res = ReceiveWait(data_out, timeout)) == NO_ERRORS)
    {
        return NO_ERRORS;
    }
    cout << "IPCommPlugin::SendReceive: error" << endl;
    return res;
}

/*
 * This method pushes the given Data object(of type Data::EData) to outgoing queue
 */
DWORD IPCommPlugin::Send(Data* data_in, long timeout)
{
    Data::DataType type = data_in->GetType();
    if (type == Data::EData)
    {
        DWORD length = data_in->GetLength();
        m_TxQueue.push(data_in);
        return NO_ERRORS;
    }
    else if (type == Data::EControl)
    {
        Util::Debug("IPCommPlugin::Send: Control Message");
        return NO_ERRORS;
    }
    return ERR_DG_COMM_DATA_SEND;
}

/*
 * This method is used to wait for data to become available in incoming queue 
 * and then read all data into single Data object which is given as parameter
 */
DWORD IPCommPlugin::ReceiveWait(Data** data_out, long timeout)
{
    long elapsed = 0;
    while (elapsed < timeout && !IsDataAvailable())
    {
        elapsed += 25;
        Sleep(25);
    }
    if (elapsed >= timeout)
    {
        return ERR_DG_COMM_DATA_RECV_TIMEOUT;
    }
    return Receive(data_out, timeout);
}

/*
 * This method is used to read all data in incoming queue to single Data object and store the result
 * to the data object given parameter
 */
DWORD IPCommPlugin::Receive(Data** data_out, long timeout)
{
    if (!m_RxQueue.empty())
    {
		*data_out = m_RxQueue.front();
        m_RxQueue.pop();
        return NO_ERRORS;
    }
    return ERR_DG_COMM_DATA_RECV;
}


DWORD IPCommPlugin::Open()
{
    return (m_Open ? NO_ERRORS : ERR_DG_COMM_OPEN);
}

DWORD IPCommPlugin::Close()
{
    m_MonitorThread->Stop();
    WaitForSingleObject(m_MonitorThread->ThreadHandle(), g_MaximumShutdownWaitTime);
    return NO_ERRORS;
}


//**********************************************************************************
// Class IPCommReaderThread
//
// This thread is used to read bytes from TCP/IP socket, encapsulate the bytes to Data objects 
// and push them to incoming queue 
//**********************************************************************************

IPCommReaderThread::IPCommReaderThread(SafeQueue<Data*>* q,
                                       long bufsize)
	:m_Running(false),
    m_Socket(NULL)
{
	m_Queue = q;
	m_TcpIpBufferSize = bufsize;
}

/*
 * Main execution loop which reads bytes from socket, encapsulates the bytes to Data object and pushes them to incoming queue
 */	
void IPCommReaderThread::Run()
{
	if( m_Socket )
		m_Running = true;

	BYTE* buffer = new BYTE[m_TcpIpBufferSize];
	while (m_Running)
	{
		// Reading from TCP/IP port
		//Util::Debug("[IPCommReaderThread] try to read");
		int bytes_read = -1;
		bytes_read = m_Socket->ReceiveBytes(buffer, m_TcpIpBufferSize);
		if (bytes_read < 0)
		{
			Stop();
			break;
		}
		if (bytes_read > 0)
		{
			Data* d = new Data((void *)buffer, bytes_read, Data::EData);
			if (Util::GetVerboseLevel() == Util::VerboseLevel::debug)
			{
				char tmp[64];
				sprintf(tmp, "m_Socket->ReceiveBytes (%d (dec) bytes):", d->GetLength());
				string s(tmp);
				Util::Debug(s);
			}
			m_Queue->push(d);
			d = NULL;
		}
		Sleep(0);
	}
	delete[] buffer;
	buffer = NULL;
}

void IPCommReaderThread::Stop()
{
	m_Running = false;
}

bool IPCommReaderThread::IsRunning()
{
	return m_Running;
}



//**********************************************************************************
// Class DataGatewaySocketWriterThread
//
// This thread is used to write data from outgoing queue to TCP/IP socket
//**********************************************************************************

IPCommWriterThread::IPCommWriterThread(SafeQueue<Data*>* q)
	:m_Running(false),
	m_Socket(NULL)
{
	m_Queue = q;
}

/*
 * This method contains the main execution loop which gets Data from outgoing queue and sends it to socket
 */
void IPCommWriterThread::Run()
{
	if( m_Socket )
		m_Running = true;

	while (m_Running)
	{
		// Sending to TCP/IP port
		//Util::Debug("[IPCommWriterThread] try to send");
		try
		{
			Data* d = m_Queue->front(50);
			char* p = (char *)d->GetData();
			DWORD l = d->GetLength();

			if (Util::GetVerboseLevel() == Util::VerboseLevel::debug)
			{
				char tmp[64];
				sprintf(tmp, "[IPCommWriterThread] HTI MsgSize = %d", l);
				string s(tmp);
				Util::Debug(s);
			}

			m_Socket->SendBytes((const unsigned char *)p, l);
			m_Queue->pop();
			delete d;
			d = NULL;

		} catch (TimeoutException te)
		{
		//Util::Debug("[IPCommWriterThread]timeout exception");
		}
	}
}

void IPCommWriterThread::Stop()
{
	m_Running = false;
}

bool IPCommWriterThread::IsRunning()
{
	return m_Running;
}


//**********************************************************************************
// Class IPCommMonitorThread
//
// This thread creates and starts reader and writer threads
// The thread also monitors if reader and writer threads are running and restarts them in case either isn't running
//**********************************************************************************

IPCommMonitorThread::IPCommMonitorThread(SafeQueue<Data*>* TxQueue,
										 SafeQueue<Data*>* RxQueue,
										 int LocalPort,
                                         string& RemoteHost,
                                         int RemotePort,
										 long RecvBufferSize)
	: m_Running(false),
	m_TxQueue(TxQueue),
	m_RxQueue(RxQueue),
	m_ReaderThread(NULL),
	m_WriterThread(NULL),
	m_LocalPort(LocalPort),
    m_RemoteHost(RemoteHost),
	m_RemotePort(RemotePort),
	m_RecvBufferSize(RecvBufferSize)
{
}

IPCommMonitorThread::~IPCommMonitorThread()
{
	if(m_ReaderThread)
	{
		delete m_ReaderThread;
		m_ReaderThread = NULL;
	}
	if(m_WriterThread)
	{
		delete m_WriterThread;
		m_WriterThread = NULL;
	}
}

/*
 * This method has two functionalities
 * -It waits for incoming connections if local port is defined
 * -It tries to connect to remote host if local host is not defined
 */
void IPCommMonitorThread::Connect(Socket*& s)
{
	// This trickery here is because if there are no sockets (Socket::nofSockets_)
	// WSACleanup gets called and then SOAP gets messed up.
	// And creating a new socket for a new connection seems to work better
	// than using the old when re-connecting / re-listening.
	Socket* new_s = new Socket();
	delete s;
	s = new_s;

	// If local port is defined start open listening socket
	SocketServer ss;
	if( m_LocalPort )
	{
		Util::Info("[IPComm] Listen for incoming connection...");
		String remoteHost( "[IPComm] Connected! Remote host : " );
		ss.Accept( s, m_LocalPort, 1, remoteHost );
		Util::Info(remoteHost);
	}
	// If not start connecting
	else
	{
		Util::Info("[IPComm] Connecting...");
		ss.Connect( s, m_RemoteHost.c_str(), m_RemotePort );
		Util::Info("[IPComm] Connected!");
	}
}

/*
 * Main execution loop of thread
 * -Creates reader and writer threads and starts them
 * -Monitors if either reader or writer thread aren't running and restarts them if not
 */
void IPCommMonitorThread::Run()
{
	Socket* s = NULL;

	m_ReaderThread = new IPCommReaderThread( m_RxQueue, m_RecvBufferSize );
	m_WriterThread = new IPCommWriterThread( m_TxQueue ) ;

	m_Running = true;
	while (m_Running)
	{
		// Reader thread should stop running when connection is lost
		if( !m_ReaderThread->IsRunning() || !m_WriterThread->IsRunning() )
		{
			Util::Info( "[IPComm] Disconnected!" );

			// Stop the treads
			m_ReaderThread->Stop();
		    WaitForSingleObject(m_ReaderThread->ThreadHandle(), g_MaximumShutdownWaitTime);
			m_WriterThread->Stop();
			WaitForSingleObject(m_WriterThread->ThreadHandle(), g_MaximumShutdownWaitTime);

			// Try to connect again.
			BOOL connected = false;
			while( m_Running && connected == false)
			{
				try{
					Connect(s);
					connected = true;
					m_ReaderThread->m_Socket = s;
					m_WriterThread->m_Socket = s;

					// Start threads
					m_ReaderThread->Start();
					m_WriterThread->Start();
					SetThreadPriority( m_ReaderThread->ThreadHandle(), THREAD_PRIORITY_LOWEST);
					int priority = GetThreadPriority(m_ReaderThread->ThreadHandle());
					int i = 0;
				}
				catch( char* ){
					Sleep(1000);
				}
			}
		}
		Sleep(1000);
	}

	// Stop the treads
	m_ReaderThread->Stop();
	WaitForSingleObject(m_ReaderThread->ThreadHandle(), g_MaximumShutdownWaitTime);
	m_WriterThread->Stop();
	WaitForSingleObject(m_WriterThread->ThreadHandle(), g_MaximumShutdownWaitTime);
	// and close the current socket
	if( s )
		delete s;
}

void IPCommMonitorThread::Stop()
{
	m_Running = false;
	m_ReaderThread->Stop();
	m_WriterThread->Stop();
}

bool IPCommMonitorThread::IsRunning()
{
	return m_Running;
}

// End of the file