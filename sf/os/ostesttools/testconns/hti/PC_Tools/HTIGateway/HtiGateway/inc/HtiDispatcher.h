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
*   This file contains headers of HtiDispatcher class.
*/

#ifndef __HTI_DISPATCHER_H__
#define __HTI_DISPATCHER_H__

#include <windows.h>
#include <map>
#include <string>

#include "safequeue.h"
#include "thread.h"

//forward declarations
struct soap;
class SoapHandler;
class HtiMessage;
class Data;

typedef map<string, SoapHandler*> htiSoapActionHashMap;
typedef map<int, SoapHandler*> htiUIDHashMap;

using namespace std;
//**********************************************************************************
// Class HtiDispatcher
//
// This class 
// -forwards Soap requests and Hti messages to correct SOAPHandlers
// -Is used to initiate SOAPHandlers by reading them from dll's and start them 
//**********************************************************************************
class HtiDispatcher : public Thread<HtiDispatcher>
{
public:
	HtiDispatcher(SafeQueue<Data*>* qIn, SafeQueue<Data*>* qOut);
	~HtiDispatcher();
	void Run();
	void Stop();
	bool IsRunning();

	bool DispatchSoapServe(struct soap* soapEnv);
	//pass m_IncomingHtiMessage to available handlers
	void DispatchToSoapHandlers();

	void SendHtiMessage(HtiMessage* msg);

private:
	/**
	* read dll plugins and load handlers
	*/
	void InitHandlers();
	void StartHandlers();
	//void ProcessIncomingDataMessage();

private:
	//incoming queue from PC side, outgoing to CommChannelPlugin side
	SafeQueue<Data*>* m_QueueIn;
	//outgoing queue to PC side, incoming from CommChannelPlugin side
	SafeQueue<Data*>* m_QueueOut;

	//handlers hash map divided by soap action, it owns handlers
	htiSoapActionHashMap m_SoapHandlers;
	//the same as above but of service UID; doesn't own handlers!!!
	htiUIDHashMap m_HanlersUidMap; 

	HtiMessage* m_IncomingHtiMessage;

	bool m_Running;
};

#endif // __HTI_DISPATCHER_H__