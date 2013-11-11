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
*   This file contains header of SOAPHandler class
*/

#ifndef __SOAP__HANDLER_H_
#define __SOAP__HANDLER_H_

#include "thread.h"
#include "HtiSoapHandlerInterface.h"
#include "HtiPluginDll.h"

#include <string>

using namespace std;

#define LOOP_WAIT_INTERVAL INFINITE //in ms
#define LOOP_CHECK_INTERVAL 100 //in ms

//forward decl.
class HtiDispatcher;
class HtiMessage;

//**********************************************************************************
// Class SOAPHandler
//
// This class is used to handle SOAP requests, send HtiMessages 
// and handle HTI responses by using loaded HtiPluginDll
//**********************************************************************************
class SoapHandler: public Thread<SoapHandler>, public HtiSoapHandlerInterface
{
public:
	SoapHandler(const string& pluginName);
	virtual ~SoapHandler();

	inline void SetDispatcher( HtiDispatcher* dispatcher);

	/**
	 * This method is used to init HtiPluginDll
	 */
	bool LoadPlugin();

	inline char* soapAction(){return m_HtiPlugin->soapActionName();};
	inline int serviceUID(){return m_HtiPlugin->serviceUID();};

	/**
	* Just a wrapper around soap_serve() created by gSOAP
	* called by DataGatewaySOAPServerThread for a new SOAP request
	* return true if request can be processed
	*/
	bool ServeSoap(struct soap* soapEnv);

	/**
	* Actual SOAP request processing in the thread
	* if it was accepted in ServeSoap()
	* The request is processed using HtiPluginDll
	*/
	void DoServeSoap();

	/**
	 * This method tells whether or not this handler is currently busy processing request
	 */
	bool IsBusyForSoapRequest();

	/**
	 * This method tells whether or not this handler is currently busy processing hti message
	 */
	bool IsBusyForHtiMessage();
	
	/*
	 * This method is used to check if SoapHandler is currently waiting for Hti Message
	 */
	bool IsWaitsForHtiMessage();

	/**
	* Notification function called when handler receive
	* new HTI message from Symbian side
	*/
	void ProcessHtiResponse();
	
	/**
	 * This loop waits until either a SOAP request or HTI response event has arrived
	 * When one of them arrives, it is handled and the loop starts again to wait
	 * for a new request or response
	 */
	void Run();
	void Stop();

	/**
	* wait for hti message
	* Suspend thread until HtiMessage for the loaded plug-in is received
	* Return true if hti message is received or false if timeout
	*/ 
	bool WaitForHtiMessage( DWORD timeout );
	inline bool WaitForHtiMessage( ){return WaitForHtiMessage( INFINITE );};

	int ReceivedHtiMessageBodySize();
	void* ReceivedHtiMessageBody();

	/*
	 * HtiPluginDll's call this method
	 * It creates a HtiMessage of the data given as parameters and sends it using HtiDispatcher
	 */
	void SendHtiMessage( DWORD serviceId, void* body, DWORD len );
	void SendHtiMessage( DWORD serviceId, void* body, DWORD len, BYTE priority );

	//error message
	bool IsReceivedHtiError();
	int HtiErrorCode();
	int HtiServiceErrorCode();
	char* HtiServiceErrorDerscription();
	void SendSoapFaultFromReceivedHtiError();

	/**
	 * This method is called when incoming data has been read from CommChannelPlugin
	 * The method sets m_hReceiveHtiEvent to signaled state(Run method waits this event object to become signaled)
	 */
	bool ReceiveHtiMessage(HtiMessage* message);

private:

	void CleanSoapEnv();

// data members
protected:
	string m_PluginName;

	HtiPluginDll* m_HtiPlugin;

	HtiDispatcher* m_HtiDispatcher;

	/**
	* Message passed to the handler by AcceptHtiMessage()
	* It is usually processed than 
	*/
	HtiMessage* m_ReceiveHtiMsg;

	bool m_Running;

private:
	/**
	* used to wait for messages from symbian-side
	*/
	HANDLE m_hReceiveHtiEvent;

	/**
	* used to wait for SOAP request
	*/
	HANDLE m_hReceiveSoapEvent;

	/**
	* Used to allow define either thread is busy
	*/
	HANDLE m_hHandlerCanAcceptSoapRequest;
	HANDLE m_hHandlerCanAcceptHtiMessage;

	/**
	* Needed to dispatch error messages without tartet service uid
	**/
	HANDLE m_hHandlerWaitsHtiMessage;

	/**
	* Local gSOAP env
	*/
	struct soap* m_SoapEnv;
};
/*
inline HtiMessage* SoapHandler::GetReceivedHtiMessage()
{
	HtiMessage* r = m_ReceiveHtiMsg;
	m_ReceiveHtiMsg = NULL;
	return r;
}
*/

inline void SoapHandler::SetDispatcher( HtiDispatcher* dispatcher)
{
	m_HtiDispatcher = dispatcher;
}

#endif //__SOAP__HANDLER_H_