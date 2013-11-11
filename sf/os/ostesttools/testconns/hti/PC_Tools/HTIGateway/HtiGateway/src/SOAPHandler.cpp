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
*   This file contains implementation of SOAPHandler class
*/

#include "stdsoap2.h" //should be first because of WinSock2.h errors

#include "SOAPHandler.h"
#include "HtiMessage.h"

#include "datagateway.h" //for htidispatcher
#include "util.h"

#include <sstream>

SOAP_NMAC struct Namespace namespaces_l[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"ns1", "urn:hti", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};

//**********************************************************************************
// Class SOAPHandler
//
// This class is used to handle SOAP requests, send HtiMessages 
// and handle HTI responses by using loaded HtiPluginDll
//**********************************************************************************
SoapHandler::SoapHandler(const string& pluginName)
	: m_PluginName(pluginName),
	  m_Running(false),
	  m_HtiDispatcher(NULL),
	  m_SoapEnv(NULL),
	  m_ReceiveHtiMsg(NULL)
{
	m_hReceiveHtiEvent = CreateEvent(NULL, //sec att
								  FALSE, //not manual-reset
								  FALSE, //initial state non-signaled
								  NULL //name
								  );
	m_hReceiveSoapEvent = CreateEvent(NULL, //sec att
								  FALSE, //not manual-reset
								  FALSE, //initial state non-signaled
								  NULL //name
								  );

	m_hHandlerCanAcceptSoapRequest = CreateEvent(NULL, //sec att
								  TRUE, //manual-reset
								  TRUE, //initial state signaled
								  NULL //name
								  );

	m_hHandlerCanAcceptHtiMessage = CreateEvent(NULL, //sec att
								  TRUE, //manual-reset
								  TRUE, //initial state signaled
								  NULL //name
								  );

	m_hHandlerWaitsHtiMessage = CreateEvent(NULL, //sec att
								  TRUE, //manual-reset
								  FALSE, //initial state non-signaled
								  NULL //name
								  );


	m_HtiPlugin = new HtiPluginDll();
}

SoapHandler::~SoapHandler()
{
	Stop();
	CloseHandle(m_hHandlerWaitsHtiMessage);
	CloseHandle(m_hHandlerCanAcceptSoapRequest);
	CloseHandle(m_hHandlerCanAcceptHtiMessage);
	CloseHandle(m_hReceiveHtiEvent);
	CloseHandle(m_hReceiveSoapEvent);
	CleanSoapEnv();
	delete m_HtiPlugin;
}

void SoapHandler::CleanSoapEnv()
{
	if ( m_SoapEnv )
	{
		soap_destroy(m_SoapEnv); // dealloc C++ data 
		soap_end(m_SoapEnv); // dealloc data and clean up 
		soap_done(m_SoapEnv); // detach soap struct 
		free(m_SoapEnv);
		m_SoapEnv = NULL;
	}
}

/**
 * This loop waits until either a SOAP request or HTI response event has arrived
 * When one of them arrives, it is handled and the loop starts again to wait
 * for a new request or response
 */
void SoapHandler::Run()
{
	Util::Debug("SoapHandler::Run()");
	HANDLE events[2];
	events[0] = m_hReceiveSoapEvent;
	events[1] = m_hReceiveHtiEvent;
	m_Running = true;
	while (m_Running)
	{
		Util::Debug("SoapHandler::Run::WaitForMultipleObjects");
		DWORD r = WaitForMultipleObjects(
						2,
						events,
                        FALSE, //don't wait all
						LOOP_WAIT_INTERVAL);
		switch (r)
		{
		case WAIT_OBJECT_0:
			{
				//process soap request
				ResetEvent(m_hHandlerCanAcceptSoapRequest);
				ResetEvent(m_hHandlerCanAcceptHtiMessage);
				DoServeSoap();
				SetEvent(m_hHandlerCanAcceptSoapRequest);
				SetEvent(m_hHandlerCanAcceptHtiMessage);
			}
			break;
		case WAIT_OBJECT_0+1:
			{
				//process HTI response
				ResetEvent(m_hHandlerCanAcceptSoapRequest);
				ResetEvent(m_hHandlerCanAcceptHtiMessage);
				ProcessHtiResponse();
				SetEvent(m_hHandlerCanAcceptSoapRequest);
				SetEvent(m_hHandlerCanAcceptHtiMessage);
			}
			break;
		case WAIT_TIMEOUT:
			{
				//do nothing
				//just check for m_Running and waits again
			}
			break;
		default:
			{
			}
		}
	}
}

void SoapHandler::Stop()
{
	m_Running = false;
}

/**
 * wait for hti message
 * Suspend thread until HtiMessage for the loaded plug-in is received
 * Return true if hti message is received or false if timeout
 */
bool SoapHandler::WaitForHtiMessage(DWORD timeout)
{
	SetEvent(m_hHandlerCanAcceptHtiMessage);
	SetEvent(m_hHandlerWaitsHtiMessage);
	//delete m_ReceiveHtiMsg;
	Util::Debug("SoapHandler::WaitForHtiMessage");
	DWORD r = WaitForSingleObject(m_hReceiveHtiEvent, timeout);

	ResetEvent(m_hHandlerCanAcceptHtiMessage);
	ResetEvent(m_hHandlerWaitsHtiMessage);
	switch ( r )
	{
	case WAIT_OBJECT_0:
		{
			return true;
		}
		break;
	case WAIT_TIMEOUT:
		{
		}
		break;
	default:
		{
		}
	}
	Util::Debug("WaitForHtiMessage OK");
	return false;
}

/**
 * This method is used to init HtiPluginDll
 */
bool SoapHandler::LoadPlugin()
{
	//code to load and init plugin dll m_PluginName
	return m_HtiPlugin->Init( m_PluginName.c_str() );
}

/**
 * This method tells whether or not this handler is currently busy processing request
 */
bool SoapHandler::IsBusyForSoapRequest()
{
	DWORD r = WaitForSingleObject(m_hHandlerCanAcceptSoapRequest, LOOP_CHECK_INTERVAL);
	switch ( r )
	{
	case WAIT_OBJECT_0:
		{
			return false;
		}
		break;
	case WAIT_TIMEOUT:
		{
		}
		break;
	default:
		{
		}
	}
	return true;
}

/**
 * This method tells whether or not this handler is currently busy processing hti message
 */
bool SoapHandler::IsBusyForHtiMessage()
{
	DWORD r = WaitForSingleObject(m_hHandlerCanAcceptHtiMessage, LOOP_CHECK_INTERVAL);
	switch ( r )
	{
	case WAIT_OBJECT_0:
		{
			return false;
		}
		break;
	case WAIT_TIMEOUT:
		{
		}
		break;
	default:
		{
		}
	}
	return true;
}

/*
 * This method is used to check if SoapHandler is currently waiting for Hti Message
 */
bool SoapHandler::IsWaitsForHtiMessage()
{
	DWORD r = WaitForSingleObject(m_hHandlerWaitsHtiMessage, LOOP_CHECK_INTERVAL);
	switch ( r )
	{
	case WAIT_OBJECT_0:
		{
			return true;
		}
		break;
	case WAIT_TIMEOUT:
		{
		}
		break;
	default:
		{
		}
	}
	return false;
}

/**
 * Just a wrapper around soap_serve() created by gSOAP
 * called by DataGatewaySOAPServerThread for a new SOAP request
 * return true if request can be processed
 * This method sets m_hReceiveSoapEvent to signaled state
 * Run method waits this event object and then handles the soap request when the event switches to signaled state
 */
bool SoapHandler::ServeSoap(struct soap* soapEnv)
{
	Util::Debug("SoapHandler::ServeSoap");
	//check can this handler process request in soapEnv
	if ( !IsBusyForSoapRequest() )
	{
		CleanSoapEnv();
		m_SoapEnv = soap_copy(soapEnv);
		//soap_free( soapEnv );
		m_SoapEnv->user = dynamic_cast<HtiSoapHandlerInterface*>( this );
		SetEvent(m_hReceiveSoapEvent);
		Util::Debug("SoapHandler::ServeSoap OK");
		return true;
	}
	else
	{
		Util::Debug("SoapHandler::ServeSoap NOK");
		return false;
	}
}

/**
 * This method is called when incoming Hti message has been read from CommChannelPlugin
 * The method sets m_hReceiveHtiEvent to signaled state(Run method waits this event object to become signaled)
 */
bool SoapHandler::ReceiveHtiMessage(HtiMessage* htiMessage)
{
	Util::Debug("SoapHandler::ReceiveHtiMessage");
	//_RPT0(_CRT_WARN, "SoapHandler::ReceiveHtiMessage");
	//check can this handler process HTI response
	if ( !IsBusyForHtiMessage() )
	{
		//_RPT1(_CRT_WARN, "delete %x\n", m_ReceiveHtiMsg);
		//_RPT1(_CRT_WARN, "received %x\n", htiMessage);
		ResetEvent(m_hHandlerCanAcceptHtiMessage);
		delete m_ReceiveHtiMsg;
		m_ReceiveHtiMsg = htiMessage;
		SetEvent(m_hReceiveHtiEvent);
		Util::Debug("SoapHandler::ReceiveHtiMessage OK");
		return true;
	}
	Util::Debug("SoapHandler::ReceiveHtiMessage NOK");
	return false;
}

/**
* Actual SOAP request processing in the thread
* if it was accepted in ServeSoap()
* The request is processed using HtiPluginDll
*/
void SoapHandler::DoServeSoap()
{
	Util::Debug("SoapHandler::DoServeSoap");
	soap_set_namespaces( m_SoapEnv, m_HtiPlugin->serviceNamespaces()); 
	//soap_set_namespaces( m_SoapEnv, namespaces_l);

	if (soap_envelope_begin_in(m_SoapEnv)
		|| soap_recv_header(m_SoapEnv)
		|| soap_body_begin_in(m_SoapEnv) )
	{
		//soap_set_namespaces( m_SoapEnv, namespaces_l);
		soap_send_fault(m_SoapEnv);
		Util::Debug("SoapHandler::DoServeSoap NOK");
		return;
	}

	//call soap_serve(soap_server_request) from the dll plug-in
	//m_HtiPlugin->soap_serve( m_SoapEnv );
	
	if (m_HtiPlugin->soap_serve_request(m_SoapEnv)
		|| (m_SoapEnv->fserveloop && m_SoapEnv->fserveloop(m_SoapEnv)))
	{
		//soap_set_namespaces( m_SoapEnv, namespaces_l);
		soap_send_fault(m_SoapEnv);
		Util::Debug("SoapHandler::DoServeSoap NOK");
		return;
	}
	
	CleanSoapEnv();
	Util::Debug("SoapHandler::DoServeSoap OK");
}

/*
 * HtiPluginDll's call this method
 * It creates a HtiMessage of the data given as parameters and sends it using HtiDispatcher
 */
void SoapHandler::SendHtiMessage( DWORD serviceId, void* body, DWORD len )
{
	Util::Debug("SoapHandler::SendHtiMessage");
	HtiMessage* msg = new HtiMessage(serviceId, body, len);
	//_RPT2(_CRT_WARN, "send msg %x <%d>\n", msg, sizeof(HtiMessage));
	m_HtiDispatcher->SendHtiMessage( msg );
	//_RPT2(_CRT_WARN, "del send msg %x <%d>\n", msg, sizeof(HtiMessage));
	delete msg;
	Util::Debug("SoapHandler::SendHtiMessage OK");
}

/*
 * HtiPluginDll's call this method
 * It creates a HtiMessage of the data given as parameters and sends it using HtiDispatcher
 */
void SoapHandler::SendHtiMessage( DWORD serviceId, void* body, DWORD len, BYTE priority )
{
	Util::Debug("SoapHandler::SendHtiMessage");
	//delegate function to dispatcher
	HtiMessage* msg = new HtiMessage(serviceId, body, len, priority);
	//_RPT2(_CRT_WARN, "send msg %x <%d>\n", msg, sizeof(HtiMessage));
	m_HtiDispatcher->SendHtiMessage( msg );
	//_RPT2(_CRT_WARN, "del send msg %x <%d>\n", msg, sizeof(HtiMessage));
	delete msg;
	Util::Debug("SoapHandler::SendHtiMessage OK");
}

int SoapHandler::ReceivedHtiMessageBodySize()
{
	if ( m_ReceiveHtiMsg )
	{
		return m_ReceiveHtiMsg->GetBodySize();
	}
	return -1;
}

void* SoapHandler::ReceivedHtiMessageBody()
{
	if ( m_ReceiveHtiMsg )
	{
		return m_ReceiveHtiMsg->GetBody();
	}
	return NULL;
}

bool SoapHandler::IsReceivedHtiError()
{
	if ( m_ReceiveHtiMsg )
	{
		return m_ReceiveHtiMsg->IsErrorMessage();
	}
	return false;
}

int SoapHandler::HtiErrorCode()
{
	if ( m_ReceiveHtiMsg )
	{
		return m_ReceiveHtiMsg->HtiErrorCode();
	}
	return -1;
}

int SoapHandler::HtiServiceErrorCode()
{
	if ( m_ReceiveHtiMsg )
	{
		return m_ReceiveHtiMsg->ServiceErrorCode();
	}
	return -1;
}

char* SoapHandler::HtiServiceErrorDerscription()
{
	if ( m_ReceiveHtiMsg )
	{
		return m_ReceiveHtiMsg->ErrorDescription();
	}
	return NULL;
}

void SoapHandler::SendSoapFaultFromReceivedHtiError()
{
	if ( m_SoapEnv && IsReceivedHtiError() )
	{
        stringstream s;
		s<<"<htiError xmlns=\'urn:hti/fault\'><frameworkErrorCode>";
        s<<m_ReceiveHtiMsg->HtiErrorCode();
		s<<"</frameworkErrorCode><serviceErrorCode>";
		s<<m_ReceiveHtiMsg->ServiceErrorCode();
		s<<"</serviceErrorCode><serviceErrorDescription>";
		s<<m_ReceiveHtiMsg->ErrorDescription();
		s<<"</serviceErrorDescription>";
		s<<"</htiError>";
		
		soap_receiver_fault(m_SoapEnv,
			"HtiError", s.str().c_str() );
	}
}

/**
 * This method makes HtiPluginDll process HTI response 
 */
void SoapHandler::ProcessHtiResponse()
{
	m_HtiPlugin->hti_serve( dynamic_cast<HtiSoapHandlerInterface*>( this ) );
}
