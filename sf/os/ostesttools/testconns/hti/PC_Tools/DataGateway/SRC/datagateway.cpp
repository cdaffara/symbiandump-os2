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
*   This file contains the header file of the DataGatewaySocketWriterThread,
*	DataGatewaySocketReaderThread, DataGatewayClientThread and DataGateway
*	classes.
*/

// INCLUDES
#include "datagateway.h"

//**********************************************************************************
// Class DataGatewaySocketWriterThread
//
// This thread is used to read Data from outgoing queue 
// and write it to Socket(connected to program using DataGateway)
//**********************************************************************************

DataGatewaySocketWriterThread::DataGatewaySocketWriterThread(SafeQueue<Data*>* q,
															 Socket* s)
	: m_Running(true)
{
	m_Queue = q;
	m_Socket = s;
}

/*
 * Main loop of thread
 * Reads Data from outgoing queue and writes it to Socket(connected to program using DataGateway)
 */
void DataGatewaySocketWriterThread::Run()
{
	while (m_Running)
	{
		// Sending to TCP/IP port
		//Util::Debug("[DataGatewaySocketWriterThread] try to send");
		try
		{
			Data* d = m_Queue->front(50);
			char* p = (char *)d->GetData();
			//DWORD l = (DWORD)((((BYTE)p[g_HtiOffsetMessageSize] << 8) | (BYTE)p[g_HtiOffsetMessageSize+1]));
			DWORD l = d->GetLength();

			if (Util::GetVerboseLevel() == Util::debug)
			{
				char tmp[64];
				sprintf(tmp, "[DataGatewaySocketWriterThread] HTI MsgSize = %d", l);
				string s(tmp);
				Util::Debug(s);
				//Util::Hex(p, min(16,d->GetLength()));
			}

			//m_Socket->SendBytes((const unsigned char *)&p[g_HtiOffsetMessageData], l);
			m_Socket->SendBytes((const unsigned char *)p, l);
			Util::Debug("[DataGatewaySocketWriterThread] msg send");
			m_Queue->pop();
			delete d;
			d = NULL;

		} catch (TimeoutException te)
		{
			//Util::Debug("[DataGatewaySocketWriterThread]timeout exception");
		}
	}
}

void DataGatewaySocketWriterThread::Stop()
{
	m_Running = false;
}

bool DataGatewaySocketWriterThread::IsRunning()
{
	return m_Running;
}

//**********************************************************************************
// Class DataGatewaySocketReaderThread
//
// This thread is used to read incoming bytes from Socket(connected to program using DataGateway)
// which it then encapsulates into Data objects and forwards to outgoing queue
//**********************************************************************************

DataGatewaySocketReaderThread::DataGatewaySocketReaderThread(SafeQueue<Data*>* q,
															 long bufsize,
															 Socket* s)
	: m_Running(true)
{
	m_Queue = q;
	m_Socket = s;
	if (bufsize > 0)
	{
		m_TcpIpBufferSize = bufsize;
	}
	else
	{
		m_TcpIpBufferSize = g_DataGatewayDefaultTcpIpBufferSize;
	}
}

/*
 * Main loop of thread
 * Reads bytes from Socket(connected to program using DataGateway), encapsulates them to Data object and puts these to outgoing queue
 */
void DataGatewaySocketReaderThread::Run()
{
	BYTE* buffer = new BYTE[m_TcpIpBufferSize];
	while (m_Running)
	{
		// Reading from TCP/IP port
		Util::Debug("[DataGatewaySocketReaderThread] try to read");
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
			if (Util::GetVerboseLevel() == Util::debug)
			{
				char tmp[64];
				sprintf(tmp, "m_Socket->ReceiveBytes (%d (dec) bytes):", d->GetLength());
				string s(tmp);
				Util::Debug(s);
				//Util::Hex(p, d->GetLength());
			}

			m_Queue->push(d);
			d = NULL;
		}
		Sleep(50);
	}
	delete[] buffer;
	buffer = NULL;
}

void DataGatewaySocketReaderThread::Stop()
{
	m_Running = false;
}

bool DataGatewaySocketReaderThread::IsRunning()
{
	return m_Running;
}

//**********************************************************************************
// Class DataGatewayClientThread
//
// This thread serves DataGateway's clients
// Gets Data from incoming queue to which DataGatewaySocketReader has pushed it and forwards
// them to CommChannelPlugin.
// The thread also reads incoming data from CommChannelPlugin and forwards them to outgoing queue which
// DataGatewaySocketWriter then reads
//**********************************************************************************

DataGatewayClientThread::DataGatewayClientThread(Socket** s,
												 long bufsize,
												 const string& commchannel)
	: m_ReaderThread(&m_ReaderQueue, bufsize, *s),
	  m_WriterThread(&m_WriterQueue, *s),
  	  m_CommChannelPluginName(commchannel),
	  m_Running(true)
{
	m_CCLateInit = true;
	m_Socket = s;
	if (bufsize > 0)
	{
		m_TcpIpBufferSize = bufsize;
	}
	else
	{
		m_TcpIpBufferSize = g_DataGatewayDefaultTcpIpBufferSize;
	}
}

DataGatewayClientThread::DataGatewayClientThread(Socket** s,
												 long bufsize,
										         CommChannelPlugin** f)
	: m_ReaderThread(&m_ReaderQueue, bufsize, *s),
	  m_WriterThread(&m_WriterQueue, *s),
  	  m_CommChannelPluginName((*f)->GetName()),
	  m_Running(true)
{
	m_CCLateInit = false;
	m_Socket = s;
	m_CommChannelPlugin = *f;
	if (bufsize > 0)
	{
		m_TcpIpBufferSize = bufsize;
	}
	else
	{
		m_TcpIpBufferSize = g_DataGatewayDefaultTcpIpBufferSize;
	}
}

DataGatewayClientThread::~DataGatewayClientThread()
{
	Util::Debug("DataGatewayClientThread::~DataGatewayClientThread()");
	if (m_Running)
	{
		Stop();
	}
}

/*
 * Main loop of thread
 * Gets Data from incoming queue to which DataGatewaySocketReader has pushed it and forwards
 * them to CommChannelPlugin.
 * Reads incoming data from CommChannelPlugin and forwards them to outgoing queue which
 * DataGatewaySocketWriter then reads
 */
void DataGatewayClientThread::Run()
{
	DWORD res;

	if (m_CCLateInit)
	{		
		m_CommChannelPlugin = CommChannelPlugin::Instance(m_CommChannelPluginName);
		if (m_CommChannelPlugin == NULL)
		{
			g_ErrorCode = ERR_DG_COMMCHANNEL;
			return;
		}
		if ((res = m_CommChannelPlugin->Connect()) != NO_ERRORS)
		{
			Util::Error("[DataGateway] Error - Cannot connect to the target.");
			m_CommChannelPlugin->Disconnect();
			g_ErrorCode = res;
			return;
		}
		Util::Info("[DataGateway] Communication Channel Plugin loaded succesfully");
	}
	
	//Start DataGatewaySocketReader and DataGatewaySocketWriter threads
	m_ReaderThread.Start();
	m_WriterThread.Start();

	// Flush comm input buffer;
	Data* dummy;
	while (m_CommChannelPlugin->Receive(&dummy) == NO_ERRORS) continue;
	dummy = NULL;

	while (m_Running)
	{
		if (!m_ReaderThread.IsRunning() ||
			!m_WriterThread.IsRunning())
		{
			Stop();
			break;
		}
			
		// Receiving from TCP/IP port and
		// sending to CommChannelPlugin
		try {
			Data* d = m_ReaderQueue.front(50);
			m_ReaderQueue.pop();
			m_CommChannelPlugin->Send(d);
			d = NULL;
		} catch (TimeoutException te) {}
			
		// Receiving from CommChannelPlugin and
		// sending data to TCP/IP port. If message
		// is error or control message it is also
		// handled here.
		Data* out;

		if (m_CommChannelPlugin->Receive(&out) != NO_ERRORS) continue;

		switch (out->GetType())
		{
			case Data::EData:
			{
				m_WriterQueue.push(out);
				out = NULL;				
			}
			break;
			case Data::EControl:
			{
				Util::Debug("ClientThread: Control Message Received");
				switch (*(BYTE*)out->GetData())
				{
					case ControlPhonePowered:
						{
							Util::Info("[DataGateway] Phone powered up");
						}
					break;
				}
			}
			break;
			case Data::EError:
			{
				Util::Debug("ClientThread: Error Message Received");
				Stop();
			}
			break;

			default:
				{
					Util::Debug("ClientThread: Unknown Message Received");
				}
			break;
		}
		delete out;
		out = NULL;
	}
	delete *m_Socket;
	m_Socket = NULL;

	if (m_CCLateInit)
	{
		m_CommChannelPlugin->Disconnect();
		Util::Info("[DataGateway] Communication Channel Plugin unloaded");
		m_CommChannelPlugin = NULL;
	}
}

void DataGatewayClientThread::Stop()
{
	m_Running = false;
	m_WriterThread.Stop();
	m_ReaderThread.Stop();
	HANDLE handles[2];
	handles[0] = m_WriterThread.ThreadHandle();
	handles[1] = m_ReaderThread.ThreadHandle();
	WaitForMultipleObjects(2, handles, TRUE, g_MaximumShutdownWaitTime);
}

//**********************************************************************************
// Class DataGateway
//
// This is the main thread of DataGateway
//**********************************************************************************

DataGateway::DataGateway(int port,
						 long bufsize,
						 const string& commchannel,
						 bool stayalive,
						 bool cclateinit)
	: m_TcpIpPort(port),
	  m_TcpIpBufferSize(bufsize),
  	  m_CommChannelPluginName(commchannel),
	  m_StayAlive(stayalive),
	  m_CCLateInit(cclateinit),
	  m_Running(true)
{
	m_CommChannelPlugin = NULL;
}

/*
 * Main loop of DataGateway
 * This loop:
 * -creates instance of CommChannelPlugin if lateinit isn't set on
 * -starts listening to socket for incoming connections
 * -after connection has arrived starts DataGatewayClient thread to serve the client
 */
void DataGateway::Run()
{
	DWORD res;
	try
	{
		if (Util::GetVerboseLevel() >= Util::info)
		{
			char tmp[256];
			sprintf(tmp, "[DataGateway] Using TCP/IP port %d", m_TcpIpPort);
			string s(tmp);
			Util::Info(s);
			
			sprintf(tmp, "[DataGateway] TCP/IP receive buffer size is %d bytes", m_TcpIpBufferSize);
			s.assign(tmp);
			Util::Info(s);

			sprintf(tmp, "[DataGateway] Loading Communication Channel Plugin for [%s]", m_CommChannelPluginName.c_str());
			s.assign(tmp);
			Util::Info(s);
		}

		
		//Util::Info("[DataGateway] TCP/IP port opened");
		
		if (!m_CCLateInit)
		{
		    //if not lateinit, CommChannelPlugin may be started
			m_CommChannelPlugin = CommChannelPlugin::Instance(m_CommChannelPluginName);
			if (m_CommChannelPlugin == NULL)
			{
				throw UtilError("[DataGateway] Error loading Communication Channel.", ERR_DG_COMMCHANNEL);
			}
			if ((res = m_CommChannelPlugin->Connect()) != NO_ERRORS)
			{
				m_CommChannelPlugin->Disconnect();
				throw UtilError("[DataGateway] Error connecting to the target.", res);
			}
			Util::Info("[DataGateway] Communication Channel Plugin loaded succesfully");
		}
		else
		{
			Util::Info("[DataGateway] Communication Channel Plugin uses late initialization.");
		}

		while (m_Running)
		{
			g_ErrorCode = NO_ERRORS;
			Util::Info("[DataGateway] Waiting connection");
			Socket* s = new Socket(); 
			SocketServer in;
			// will use the socket for listening and return a new socket when connection 
			// is established
			String remoteHost( "[DataGateway] Connection established! Remote host : " );
			in.Accept(s, m_TcpIpPort, 1, remoteHost); 
			Util::Info(remoteHost);
			DataGatewayClientThread* client;
			if (m_CCLateInit)
			{
				// passes ownership of 's'
				client = new DataGatewayClientThread(&s, m_TcpIpBufferSize, m_CommChannelPluginName);
			}
			else
			{
				// passes ownership of 's'
				client = new DataGatewayClientThread(&s, m_TcpIpBufferSize, &m_CommChannelPlugin);
			}
			client->Start();

			HANDLE handles[2];
			handles[0] = client->ThreadHandle();
			handles[1] = m_ShutdownEvent.EventHandle();
			DWORD dwResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
			switch (dwResult)
			{
				case WAIT_OBJECT_0 + 0:
				{
					Util::Debug("DataGateway::Run() Client thread stopped");
				}
				break;
				case WAIT_OBJECT_0 + 1:
				{
					Util::Debug("DataGateway::Run() Request to shutdown");
					client->Stop();
					WaitForSingleObject(client->ThreadHandle(), g_MaximumShutdownWaitTime);
				}
				break;
			}
			Util::Info("[DataGateway] Connection closed.");
			delete client;
			client = NULL;
			
			if (!m_StayAlive) break;
		}
		if (!m_CCLateInit)
		{
			m_CommChannelPlugin->Disconnect();
			Util::Info("[DataGateway] Communication Channel Plugin unloaded");
			m_CommChannelPlugin = NULL;
		}
	} catch (char* s) {
		char tmp[64];
		sprintf(tmp, "[DataGateway] Error opening TCP/IP port - %s", s);
		Util::Error(tmp);
		g_ErrorCode = ERR_DG_SOCKET;
	} catch (UtilError ue) {
		Util::Error(ue.iError, ue.iResult);
		g_ErrorCode = ue.iResult;
	}

	Util::Info("[DataGateway] Closed");
}

void DataGateway::Stop()
{
	m_Running = false;
	m_ShutdownEvent.Set();
}

bool DataGateway::IsRunning()
{
	return m_Running;
}

// End of the file