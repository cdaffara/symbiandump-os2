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
*   This file contains implementation of HtiDispatcher class.
*/

#include "stdsoap2.h" //should be first because of WinSock2.h errors

#include "common.h"
#include "HtiDispatcher.h"
#include "soapHandler.h"

#include "HtiMessage.h"
#include "util.h"
#include <sstream>

#include <crtdbg.h>

const static char* HTI_PLUGIN_FOLDER = "ServicePlugins/";
//used to redispatch hti framework error messages (like not authorized)

/**
* namespace table is needed to correclty send fault messages
* each service plugin should explicitly set its one namespace table before processing
* request
*/
SOAP_NMAC struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{NULL, NULL, NULL, NULL}
};

//**********************************************************************************
// Class HtiDispatcher
//
// This class 
// -forwards Soap requests and Hti messages to correct SOAPHandlers
// -Is used to initiate SOAPHandlers by reading them from dll's and start them 
//**********************************************************************************
HtiDispatcher::HtiDispatcher(SafeQueue<Data*>* qIn,
                             SafeQueue<Data*>* qOut)
	: m_Running(true),
	m_QueueIn(qIn),
	m_QueueOut(qOut),
	m_IncomingHtiMessage(NULL)
{
}

HtiDispatcher::~HtiDispatcher()
{
	for( htiSoapActionHashMap::const_iterator i = m_SoapHandlers.begin();
		 i != m_SoapHandlers.end();
		 ++i)
	{
		delete i->second;
	}
}

/**
 * This method is used to read all the available SoapHandler dll's and initialize an instance of them and store a reference to m_SoapHandlers(by soap action)
 * and m_HanlersUidMap(by service uid) maps.
 */
void HtiDispatcher::InitHandlers()
{
	Util::Info( "HtiDispatcher::InitHandlers" );
	//1. read exe/plugin directory
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 
	bool fFinished = false;
 
	// Start searching for .dll files
	string searchMask = HTI_PLUGIN_FOLDER;
	searchMask.append( "*.dll" );
 	hSearch = FindFirstFile( searchMask.c_str(), &FileData );
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		Util::Info("No DLLs found."); 
		return;
	} 
 
	while (!fFinished) 
	{ 
		
	//2. get list of all DLLs
	//SoapHandler* t = new SoapHandler("ws_hti.dll");
		string pluginPath = HTI_PLUGIN_FOLDER;
		pluginPath.append( FileData.cFileName );
		Util::Info("Try to load:");
		Util::Info(pluginPath);
		SoapHandler* t = new SoapHandler( pluginPath );
		if( t->LoadPlugin() )
		{
			//3. add them to m_SoapHandlers
			char* soapAction = t->soapAction();
			m_SoapHandlers[ soapAction ] = t;
			//add to uid map as well
			int uid = t->serviceUID();
			m_HanlersUidMap[ uid ] = t;
			
			stringstream s;
			s<<"Loaded plugin: ";
			s<<soapAction;
			s<<", service UID: ";
			s<<uid;
			
			Util::Info( s.str().c_str() );
		}
		else
		{
			delete t;
			//
			Util::Error("Failed load DLL as a plugin");
		}
	 
		if (!FindNextFile(hSearch, &FileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				fFinished = TRUE; 
			} 
			else 
			{ 
				Util::Error("Couldn't find next plugin.");
				fFinished = TRUE;
				//return;
			} 
		}
	} 
	 
	// Close the search handle. 
	 
	FindClose(hSearch);

	stringstream s;
	s<<"Plugins loaded ";
	s<<m_SoapHandlers.size();
	Util::Info( s.str().c_str() );
}

/**
 * This method is used to start all the SoapHandler instances that are stored in htiSoapActionHashMap
 */
void HtiDispatcher::StartHandlers()
{
	for( htiSoapActionHashMap::const_iterator i = m_SoapHandlers.begin();
		 i != m_SoapHandlers.end();
		 ++i)
	{
		(i->second)->SetDispatcher(this);
		(i->second)->Start();
	}
}

/*
 * This loop is used to forward incoming Hti messages(arriving from CommChannelPlugin to m_QueueOut side) to correct SoapHandlers
 */
void HtiDispatcher::Run()
{
	Util::Debug("HtiDispatcher::Run");
	//m_testHandler.SetDispatcher(this);
	//m_testHandler.Start();
	InitHandlers();
	StartHandlers();

	BYTE* shortData = NULL; //keep data if it's too short to have HtiHeader
							//or if one Data msg have more than one HtiMessage
	DWORD msgBufferSize = 8096;
	BYTE* msgBuffer = (BYTE*)malloc( msgBufferSize );
	DWORD shortDataLen = 0;

	// By setting this threads priority below others will give soaphandler threads 
	// more runtime to process the received messages.. This is to avoid the situation
	// where two messages arrive so close to each other that m_hReceiveHtiEvent is 
	// signalled twice before the first message has even been processed by the soap plugin.
	if(!SetThreadPriority( ThreadHandle(), THREAD_PRIORITY_BELOW_NORMAL ))
		Util::Info("Warning: Could not set HtiDispatcher priority!");
	
	while (m_Running)
	{
		try
		{	
			Data* d = m_QueueOut->front(50);
			BYTE* p = (BYTE *)d->GetData();
			DWORD l = d->GetLength();
			//printf("\td = %d\n", m_QueueOut->size());

			if (Util::GetVerboseLevel() == Util::VerboseLevel::debug)
			{
				char tmp[64];
				sprintf(tmp, "[HtiDispatcher] HTI MsgSize = %d", l);
				string s(tmp);
				Util::Debug(s);
				//Util::Hex(p, d->GetLength());
			}

			//Util::Debug("leftovers");
			//copy leftovers to the beginning of the buffer
			if ( shortDataLen > 0 )
			{
				memcpy( msgBuffer, shortData, shortDataLen );
			}
			shortData = msgBuffer; //set shortData to the beginning

			//copy data to buffer
			if ( shortDataLen + l > msgBufferSize )
			{
				msgBufferSize = shortDataLen + l;
				msgBuffer = (BYTE*)realloc(msgBuffer, msgBufferSize);
				shortData = msgBuffer;
			}
			//copy data gotten from queue to the end of shortData
			memcpy(shortData + shortDataLen, p, l );
			shortDataLen = l + shortDataLen;

			while ( shortDataLen != 0 &&
				    (shortDataLen >= HtiMessage::MinHeaderSize() ||
				    m_IncomingHtiMessage != NULL ) )
			{
				//new message
				if ( m_IncomingHtiMessage == NULL )
				{
					if ( shortDataLen >= HtiMessage::MinHeaderSize() )
					{
						if ( HtiMessage::CheckValidHtiHeader(shortData) )
						{
							m_IncomingHtiMessage = new HtiMessage( shortData, shortDataLen );

							if (Util::GetVerboseLevel() == Util::VerboseLevel::debug)
							{
								char tmp[64];
								sprintf(tmp,"New hti message %d", m_IncomingHtiMessage->HtiDataSize());
								string s(tmp);
								Util::Debug(s);
								//Util::Hex(p, d->GetLength());
							}
							
							//_RPT2(_CRT_WARN, "income msg %x <%d>\n", m_IncomingHtiMessage, sizeof(HtiMessage));
							//check message
							if ( m_IncomingHtiMessage->IsMessageComplete() )
							{
								Util::Debug("HTI message complete");
								DWORD msgSize = m_IncomingHtiMessage->HtiDataSize();
								if (  msgSize < shortDataLen  )
								{
									//remove used part
									//BYTE* temp = new BYTE[shortDataLen-msgSize];
									//_RPT2(_CRT_WARN, "temp %x <%d>\n", shortData , shortDataLen-msgSize);
									//memcpy(temp, shortData + msgSize, shortDataLen-msgSize);
									//_RPT1(_CRT_WARN, "del shortData %x\n", shortData);
									//delete[] shortData;
									//shortData = temp;
									shortData += msgSize; //just move pointer
									shortDataLen -= msgSize;

								}
								else
								{
									//_RPT1(_CRT_WARN, "del shortData %x\n", shortData);
									//delete[] shortData;
									//shortData = NULL;
									shortDataLen = 0;
								}
								//Dispatch incoming message
								DispatchToSoapHandlers();
							}
							else
							{
								//_RPT1(_CRT_WARN, "del shortData %x\n", shortData);
								//delete[] shortData;
								//shortData = NULL;
								shortDataLen = 0;
							}
						}
						else
						{
							//invalid header
							Util::Error("Invalid HTI header, dismiss Data message");
							Util::Hex(shortData, HtiMessage::MinHeaderSize() );
							//_RPT1(_CRT_WARN, "del shortData %x\n", shortData);
							//delete[] shortData;
							//shortData = NULL;
							shortDataLen = 0;
						}
					}
				}
				else //body parts
				{
					Util::Debug("add");
					DWORD added = m_IncomingHtiMessage->AddToBody( shortData,
																   shortDataLen );
					//printf("reminder %d\n", m_IncomingHtiMessage->Reminder());
					if ( added < shortDataLen )
					{
						//only part of message was added
						//remove added part
						//BYTE* temp = new BYTE[shortDataLen-added];
						//_RPT2(_CRT_WARN, "temp %x <%d>\n", shortData , shortDataLen-added);
						//memcpy(temp, shortData + added, shortDataLen-added);
						//_RPT1(_CRT_WARN, "del shortData %x\n", shortData );
						//delete[] shortData;
						//shortData = temp;
						shortData += added;
						shortDataLen -= added;
					}
					else //all data were added
					{
						//_RPT1(_CRT_WARN, "del shortData %x\n", shortData );
						//delete[] shortData;
						//shortData = NULL;
						shortDataLen = 0;
					}

					if ( m_IncomingHtiMessage->IsMessageComplete() )
					{
						Util::Debug("HTI message complete");
						//Dispatch incoming message
						DispatchToSoapHandlers();
					}
				}
			}

			m_QueueOut->pop();
			//_RPT1(_CRT_WARN, "del data %x\n", d);
			delete d;
			d = NULL;
		} catch (TimeoutException te)
		{
			//Util::Debug("[DataGatewaySocketWriterThread]timeout exception");
		}
	}
	free( msgBuffer );
}

/**
 * This method is used to forward the incoming HTI message to correct SoapHandler
 * The correct SoapHandler is found by service uid
 */
void HtiDispatcher::DispatchToSoapHandlers()
{
	htiUIDHashMap::const_iterator i;
	int targetServiceUid;
	if ( m_IncomingHtiMessage->IsErrorMessage() )
	{
		targetServiceUid = m_IncomingHtiMessage->ErrorServiceUid();

		stringstream s;
		s<<"Received HTI error message\nhtiErrorCode: ";
		s<<m_IncomingHtiMessage->HtiErrorCode();
		s<<"\nserviceUid: ";
		s<<m_IncomingHtiMessage->ErrorServiceUid();
		s<<"\nserviceErrorCode: ";
		s<<m_IncomingHtiMessage->ServiceErrorCode();
		s<<"\nErrorDescription: ";
		s<<m_IncomingHtiMessage->ErrorDescription();
		s<<"\ntargetServiceUid: ";
		s<<targetServiceUid;

		Util::Error(s.str().c_str());
	}
	else
	{
		targetServiceUid = m_IncomingHtiMessage->GetServiceUID();
	}

	if ( targetServiceUid == HTI_SYSTEM_SERVICE_UID &&
		 m_IncomingHtiMessage->IsErrorMessage() )
	{
		Util::Debug("dispatch error");
		//if system plugin doesn't wait for a message then find plugin that
		//is waiting for a message and delivery the incoming msg to it
		i = m_HanlersUidMap.find( targetServiceUid  );
		if ( i != m_HanlersUidMap.end() )
		{
			if ( (i->second)->IsWaitsForHtiMessage() )
			{
				Util::Debug("dispatch error to system cause it waits");
				if ( !(i->second)->ReceiveHtiMessage(m_IncomingHtiMessage) )
				{
					Util::Error("Failed to dispatch hti message");
					Util::Hex(m_IncomingHtiMessage->HtiData(), HtiMessage::MinHeaderSize());
					//dismiss message
					delete m_IncomingHtiMessage;
				}
			}
			else
			{
				Util::Debug("find handler that waits");
				//find the handler that waits for hti message
				for( htiUIDHashMap::const_iterator i = m_HanlersUidMap.begin();
					i != m_HanlersUidMap.end();
					++i)
				{
					Util::Debug((i->second)->soapAction());
					if ( (i->second)->IsWaitsForHtiMessage() )
					{
						Util::Debug("found");
						if ( !(i->second)->ReceiveHtiMessage(m_IncomingHtiMessage) )
						{
							Util::Error("Failed to dispatch hti message");
							Util::Hex(m_IncomingHtiMessage->HtiData(), HtiMessage::MinHeaderSize());
							//dismiss message
							delete m_IncomingHtiMessage;
						}
						break;
					}
				}
			}
		}
	}
	else
	{
		i = m_HanlersUidMap.find( targetServiceUid  );
		if ( i != m_HanlersUidMap.end() )
		{
			if ( !(i->second)->ReceiveHtiMessage(m_IncomingHtiMessage) )
			{
				Util::Error("Failed to dispatch hti message");
				Util::Hex(m_IncomingHtiMessage->HtiData(), HtiMessage::MinHeaderSize());
				//dismiss message
				delete m_IncomingHtiMessage;
			}
		}
		else
		{
			Util::Error("Failed to dispatch hti message, no plug-in with appropriate uid",m_IncomingHtiMessage->GetServiceUID() );
			Util::Hex(m_IncomingHtiMessage->HtiData(), HtiMessage::MinHeaderSize());
			//dismiss message
			delete m_IncomingHtiMessage;
		}
	}
	m_IncomingHtiMessage = NULL;

	// This will give other threads (soaphandlers) some runtime to process the
	// received message.
	Sleep(0);
}

/*
 * This method is used to forward soap request to correct SOAPHandler
 * Correct SOAPHandler is found by soap action
 */
bool HtiDispatcher::DispatchSoapServe(struct soap* soapEnv)
{
	Util::Debug("HtiDispatcher::DispatchSoapServe()");
/*	
	_CrtMemState localMem;
	_CrtMemCheckpoint( &localMem );
*/
	soap_begin( soapEnv );
	if (soap_begin_recv(soapEnv))
	{
		soap_set_namespaces( soapEnv, namespaces);
		soap_send_fault(soapEnv);
		return false;
	}

	if ( !(soapEnv->action) )
	{
		//Util::Error("soapAction is missing");
		soap_set_namespaces( soapEnv, namespaces);
		soapEnv->error = soap_sender_fault(soapEnv, "soapAction is missing", NULL); 
		soap_send_fault(soapEnv);
		return false;
	}
/*
	//_RPT0(_CRT_WARN, "!!!!!!!!!!!!!!!! Local Objects !!!!!!!!!!!!!!!!\n");

	_CrtMemDumpAllObjectsSince( &localMem );
	//_RPT1(_CRT_WARN, "action address %x\n", soapEnv->action);
*/
	htiSoapActionHashMap::const_iterator it;
	it = m_SoapHandlers.find( soapEnv->action );
	if ( it != m_SoapHandlers.end() )
	{
		return (it->second)->ServeSoap( soapEnv );
	}
	else
	{
		//Util::Error("soapAction is unknown:");
		//Util::Error(soapEnv->action);
		//soapEnv->error = SOAP_NO_METHOD;
		soap_set_namespaces( soapEnv, namespaces);
		soapEnv->error = soap_sender_fault(soapEnv, "No plugin found", "no plugin found for requested service in actionSOAP header field"); 
		soap_send_fault(soapEnv);
		return false;
	}

	Util::Debug("HtiDispatcher::DispatchSoapServe() OK");
}

/*
 * SoapHandler calls this method
 * The method creates a Data object from the HtiMessage given as parameter 
 * and puts the Data object into incoming queue(going eventually to CommChannelPlugin)
 */
void HtiDispatcher::SendHtiMessage(HtiMessage* msg)
{
	Util::Debug("HtiDispatcher::SendHtiMessage()");
	if (msg)
	{
		Data* d = new Data(msg->HtiData(), msg->HtiDataSize(), Data::EData);
		//_RPT2(_CRT_WARN, "d %x <%d>\n", d , sizeof(Data));

		if (Util::GetVerboseLevel() == Util::VerboseLevel::debug)
		{
			char tmp[64];
			sprintf(tmp, "[HtiDispatcher] HTI MsgSize = %d", msg->HtiDataSize());
			string s(tmp);
			Util::Debug(s);
			Util::Hex( (char*)(msg->HtiData()), 16);
		}
		m_QueueIn->push(d);
		//delete msg;
	}
	Util::Debug("HtiDispatcher::SendHtiMessage() OK");
}

void HtiDispatcher::Stop()
{
	m_Running = false;
	HANDLE* handles = new HANDLE[ m_SoapHandlers.size() ];
	int h = 0;
	for( htiSoapActionHashMap::const_iterator i = m_SoapHandlers.begin();
		 i != m_SoapHandlers.end();
		 ++i)
	{
		(i->second)->Stop();
		handles[ h++ ] = (i->second)->ThreadHandle();
	}

	WaitForMultipleObjects(m_SoapHandlers.size(),
					       handles,
						   TRUE,
						   5000/*g_MaximumShutdownWaitTime*/);

	delete[] handles;
}

bool HtiDispatcher::IsRunning()
{
	return m_Running;
}
