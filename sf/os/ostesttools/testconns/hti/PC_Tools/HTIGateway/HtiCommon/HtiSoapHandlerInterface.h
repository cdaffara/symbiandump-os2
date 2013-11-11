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
*/
#ifndef __HTI__SOAP_HANDLER_INTERFACE_H_
#define __HTI__SOAP_HANDLER_INTERFACE_H_

#include "hticommon.h"

class HtiSoapHandlerInterface
{
public:

	/**
	* wait for hti message
	* Suspend thread until HtiMessage for the loaded plug-in is received
	* Return true if hti message is received or false if timeout
	**/ 
	virtual bool WaitForHtiMessage( DWORD timeout ) = 0;
	virtual bool WaitForHtiMessage( ) = 0;

	/**
	* Returns received hti message
	**/
	//virtual HtiMessage* GetReceivedHtiMessage() = 0;
	virtual int ReceivedHtiMessageBodySize() = 0;
	virtual void* ReceivedHtiMessageBody() = 0;

	/**
	* Send HtiMessage to symbian side
	**/
	virtual void SendHtiMessage( DWORD serviceId, void* body, DWORD len ) = 0;
	virtual void SendHtiMessage( DWORD serviceId, void* body, DWORD len, BYTE priority ) = 0;

	//error message
	virtual bool IsReceivedHtiError()=0;
	virtual int HtiErrorCode()=0;
	virtual int HtiServiceErrorCode()=0;
	virtual char* HtiServiceErrorDerscription()=0;
	virtual void SendSoapFaultFromReceivedHtiError()=0;
};

#endif //__HTI__SOAP_HANDLER_INTERFACE_H_