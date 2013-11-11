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
*   This file contains implementation of the DataGatewaySOAPServerThread, 
*   DataGatewayClientThread and DataGateway classes.
*/

// INCLUDES
#include "stdsoap2.h" //should be first because of WinSock2.h errors
#include "datagateway.h"
#include "hticommon.h"
#include "HtiMessage.h"
#include <sstream>

/*
 * This method is used to print the SOAP fault to a string
 */
void soap_sprint_fault(struct soap *soap, char *fd)
{ if (soap->error)
  { const char *c, *v = NULL, *s, **d;
    d = soap_faultcode(soap);
    if (!*d)
      soap_set_fault(soap);
    c = *d;
    if (soap->version == 2)
      v = *soap_faultsubcode(soap);
    s = *soap_faultstring(soap);
    d = soap_faultdetail(soap);
    sprintf(fd, "%s%d fault: %s [%s]\n\"%s\"\nDetail: %s\n", soap->version ? "SOAP 1." : "Error ", soap->version ? (int)soap->version : soap->error, c, v ? v : "no subcode", s ? s : "[no reason]", d && *d ? *d : "[no detail]");
  }
}

//**********************************************************************************
// Class DataGatewaySOAPServerThread
//
// This thread acts as a SOAP server, it listens to SOAP requests and forwards them
// to HtiDispatcher which then forwards them to correct SOAPHandlers
//**********************************************************************************

DataGatewaySOAPServerThread::DataGatewaySOAPServerThread(HtiDispatcher* htiDispatcher,
														int port)
	: m_HtiDispatcher(htiDispatcher),m_TcpPort(port),m_Running(true)
{
}

/*
 * This loop listens to incoming Soap reuests and forwards them to HtiDispatcher
 */
void DataGatewaySOAPServerThread::Run()
{
	struct soap soap;
 	//Initializes a static/stack-allocated runtime environment 
	soap_init(&soap);
	//soap_init2(&soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE);
	//_CrtDbgReport(_CRT_ERROR, _CRTDBG_MODE_WNDW);
/*
#ifdef _DEBUG
   HANDLE hLogFile;
   hLogFile = CreateFile("c:\\log.txt", GENERIC_WRITE, FILE_SHARE_WRITE,
      NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
   _CrtSetReportFile(_CRT_WARN, hLogFile);

#endif
*/
	while (m_Running)
	{
		int m, s; // master and slave sockets
		Util::Debug("soap_init");

		//Returns master socket (backlog = max. queue size for requests). When host==NULL: host is the machine on which the service runs 
		m = soap_bind(&soap, NULL, m_TcpPort, 100);
		if (m < 0)
		{
			//Util::Error("Failed to open socket", m_TcpPort);
			//soap_print_fault(&soap, stderr);
			char temp[512];
			soap_sprint_fault(&soap, temp);
			Util::Error(temp);
			break;
		}
		else
		{
			Util::Debug("Socket connection successful");
/*
#ifdef _DEBUG
			_CrtMemState startMem, endMem, diffMem;
			//Sleep(5000);
#endif
*/
			for (int i = 1; ; i++)
			{
				//_RPT1(_CRT_WARN, "---++++=======Iter %d=======++++---\n", i);
				//_RPT0(_CRT_WARN, "---Start dump---\n");

				//_CrtMemCheckpoint(&startMem);
				//_CrtMemDumpStatistics(&startMem);
				//Returns slave socket
				s = soap_accept(&soap);
				if (s < 0)
				{
					//soap_print_fault(&soap, stderr);
					char temp[512];
					soap_sprint_fault(&soap, temp);
					Util::Error(temp);

					break;
				}
				Util::Debug(" accepted connection");

				//start of my dispatching code
				if ( !m_HtiDispatcher->DispatchSoapServe( &soap ) ) // process RPC request
				{
					//soap_print_fault(&soap, stderr); // print error
					// clean up class instances
					soap_destroy(&soap);
					// clean up everything and close socket
					soap_end(&soap); 
				}
				else
				{
					// clean up allcated data
					soap_dealloc(&soap, NULL);
					// clean up class instances
					soap_destroy(&soap); 
					//cleanup temp data
					soap_free(&soap); 

					//soap_end(&soap); // clean up everything and close socket
				}
/*
#ifdef _DEBUG
				//Sleep(2000); //wait when hadler thread is over

				//copy
				memcpy( &startMem, &endMem, sizeof(_CrtMemState) );
				_CrtMemCheckpoint(&endMem);
				_CrtMemDumpStatistics(&endMem);

				_RPT0(_CRT_WARN, "==========End diff==========\n");

				if (_CrtMemDifference( &diffMem, &startMem, &endMem ) )
				{

					_CrtMemDumpStatistics(&diffMem);
					_RPT0(_CRT_WARN, "########## Objects #############\n");
					//_CrtMemDumpAllObjectsSince( &diffMem );
					_RPT0(_CRT_WARN, "++++++End dump++++++\n");
				}
#endif
*/
				Util::Debug("request dispatched");
			}
		}
	}
	// Clean up deserialized data (except class instances) and temporary data 
	soap_end(&soap);
	// close master socket and detach environment
	soap_done(&soap); 
	Stop();
//#ifdef _DEBUG
//   CloseHandle(hLogFile);
//#endif
}

void DataGatewaySOAPServerThread::Stop()
{
	m_Running = false;
}

bool DataGatewaySOAPServerThread::IsRunning()
{
	return m_Running;
}

//**********************************************************************************
// Class DataGatewayClientThread
//
// This thread serves DataGateway's clients
// Gets Data objects from incoming queue and forwards them to CommChannelPlugin.
// The Data objects are actually SOAP requests which were received by DataGatewaySOAPServerThread handled by SOAPHandler and transferred to HtiMessages and eventually Data objects
// The thread also reads incoming data from CommChannelPlugin and forwards them to outgoing queue which
// HtiDispatcher then reads and forwards to correct SOAPHandler
//**********************************************************************************

DataGatewayClientThread::DataGatewayClientThread(int port,
												 long bufsize,
												 const string& commchannel)
	: m_HtiDispatcher(&m_ReaderQueue, &m_WriterQueue),
	  m_SoapListener(&m_HtiDispatcher, port),
  	  m_CommChannelPluginName(commchannel),
	  m_Running(true)
{
	m_CCLateInit = true;

	if (bufsize > 0)
	{
		m_TcpIpBufferSize = bufsize;
	}
	else
	{
		m_TcpIpBufferSize = g_DataGatewayDefaultTcpIpBufferSize;
	}
}

DataGatewayClientThread::DataGatewayClientThread(int port,
												 long bufsize,
										         CommChannelPlugin** f)
	: m_HtiDispatcher(&m_ReaderQueue, &m_WriterQueue),
	  m_SoapListener(&m_HtiDispatcher, port),
  	  m_CommChannelPluginName((*f)->GetName()),
	  m_Running(true)
{
	m_CCLateInit = false;

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
 *
 * Gets Data from incoming queue and forwards it to CommChannelPlugin.
 * Reads incoming data from CommChannelPlugin and forwards them to outgoing queue
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
			Util::Error("[HtiGateway] Error - Cannot connect to the target.");
			m_CommChannelPlugin->Disconnect();
			g_ErrorCode = res;
			return;
		}
		Util::Info("[HtiGateway] Communication Channel Plugin loaded succesfully");
	}

	m_SoapListener.Start();
	m_HtiDispatcher.Start();

	// Flush comm input buffer;
	Data* dummy;
	while (m_CommChannelPlugin->Receive(&dummy) == NO_ERRORS)
	{
		dummy->FreeData();
		continue;
	}
	dummy = NULL;

	while (m_Running)
	{
		if (!m_SoapListener.IsRunning() ||
			!m_HtiDispatcher.IsRunning())
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
			//_RPT2(_CRT_WARN,"DataGateway::Send %x %x\n", d, d->GetData());
			d = NULL;
		} catch (TimeoutException te)
		{
			//Util::Debug("DataGatewayClientThread::TimeoutException");
		}

		// Receiving from CommChannelPlugin and
		// sending data to TCP/IP port. If message
		// is error or control message it is also
		// handled here.
		Data* out;

		if (m_CommChannelPlugin->Receive(&out) != NO_ERRORS) continue;
		//_RPT2(_CRT_WARN,"DataGateway::Receive %x %x\n", out, out->GetData());
		//printf(">>>>>>>>>Type %d clt<<<<<<\n", out->GetType());
		switch (out->GetType())
		{
			case Data::EData:
			{

				//printf("\t\tpush = %d\n", m_WriterQueue.size());
				m_WriterQueue.push(out);
				//_RPT0(_CRT_WARN,"DataGateway::Write out NULL\n");
				out = NULL;
			}
			break;
			case Data::EControl:
			{
				Util::Debug("ClientThread: Control Message Received");
				/*
				switch (*(BYTE*)out->GetData())
				{
					case ControlPhonePowered:
						{
							Util::Info("[HtiGateway] Phone powered up");
						}
					break;
				}
				*/
				//generate HTI error message for waiting handlers
				//putting control message content in the detail field
				HtiMessage* errMsg = HtiMessage::CreateErrorMessage(0, (char*)out->GetData() );
				out->FreeData();
				out->SetData( errMsg->HtiData(), errMsg->HtiDataSize(), Data::EData);
				m_WriterQueue.push(out);
				out = NULL;
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

	if (m_CCLateInit)
	{
		m_CommChannelPlugin->Disconnect();
		Util::Info("[HtiGateway] Communication Channel Plugin unloaded");
		m_CommChannelPlugin = NULL;
	}
}

void DataGatewayClientThread::Stop()
{
	m_Running = false;
	m_SoapListener.Stop();
	m_HtiDispatcher.Stop();
	HANDLE handles[2];
	handles[0] = m_SoapListener.ThreadHandle();
	handles[1] = m_HtiDispatcher.ThreadHandle();
	WaitForMultipleObjects(2, handles, TRUE, g_MaximumShutdownWaitTime);
}

//**********************************************************************************
// Class DataGateway
//
// Main class/thread of HtiGateway
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
 * Main loop of HtiGateway
 * This loop:
 * -creates instance of CommChannelPlugin if lateinit isn't set on
 * -starts DataGatewayClient thread to serve the client
 */
void DataGateway::Run()
{
	DWORD res;

	try
	{
		if (Util::GetVerboseLevel() >= Util::VerboseLevel::info)
		{
			char tmp[256];
			sprintf(tmp, "[HtiGateway] Using TCP/IP port %d", m_TcpIpPort);
			string s(tmp);
			Util::Info(s);

			//sprintf(tmp, "[HtiGateway] TCP/IP receive buffer size is %d bytes", m_TcpIpBufferSize);
			//s.assign(tmp);
			//Util::Info(s);

			sprintf(tmp, "[HtiGateway] Loading Communication Channel Plugin for [%s]", m_CommChannelPluginName.c_str());
			s.assign(tmp);
			Util::Info(s);
		}

		//SocketServer in(m_TcpIpPort, 1);
		//Util::Info("[HtiGateway] TCP/IP port opened");

		if (!m_CCLateInit)
		{
			m_CommChannelPlugin = CommChannelPlugin::Instance(m_CommChannelPluginName);
			if (m_CommChannelPlugin == NULL)
			{
				throw UtilError("[HtiGateway] Error loading Communication Channel.", ERR_DG_COMMCHANNEL);
			}
			if ((res = m_CommChannelPlugin->Connect()) != NO_ERRORS)
			{
				m_CommChannelPlugin->Disconnect();
				throw UtilError("[HtiGateway] Error connecting to the target.", res);
			}
			Util::Info("[HtiGateway] Communication Channel Plugin loaded succesfully");
		}
		else
		{
			Util::Info("[HtiGateway] Communication Channel Plugin uses late initialization.");
		}

			g_ErrorCode = NO_ERRORS;
			Util::Info("[HtiGateway] Waiting connection");
			//Socket* s = in.Accept();
			Util::Info("[HtiGateway] Connection established");
			DataGatewayClientThread* client;
			if (m_CCLateInit)
			{
				client = new DataGatewayClientThread(m_TcpIpPort,
													m_TcpIpBufferSize,
													m_CommChannelPluginName);
			}
			else
			{
				client = new DataGatewayClientThread(m_TcpIpPort,
													m_TcpIpBufferSize,
													&m_CommChannelPlugin);
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
			Util::Info("[HtiGateway] Connection closed.");
			delete client;
			client = NULL;
			//if (!m_StayAlive) break;
		if (!m_CCLateInit)
		{
			m_CommChannelPlugin->Disconnect();
			Util::Info("[HtiGateway] Communication Channel Plugin unloaded");
			m_CommChannelPlugin = NULL;
		}
	} catch (char* s) {
		char tmp[64];
		sprintf(tmp, "[HtiGateway] Error opening TCP/IP port - %s", s);
		Util::Error(tmp);
		g_ErrorCode = ERR_DG_SOCKET;
	} catch (UtilError ue) {
		Util::Error(ue.iError, ue.iResult);
		g_ErrorCode = ue.iResult;
	}
	Util::Info("[HtiGateway] Closed");
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