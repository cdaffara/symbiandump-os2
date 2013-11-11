/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : readrequesthandler.h
* Part of     : stdio server
* To handle request for read
* 
*
*/


 


#ifndef _READREQUEST_H_
#define _READREQUEST_H_

#include<e32base.h>//CActive

//Enumarations for Polling
//duplicate of fdesc.h
enum TPollMode
    {
    EReadyForReading = 1,
    EReadyForWriting = 2,
    EAnyException = 4
    };

class CStreamBase;
	
//-----------------------------------------------------------------------
//Class name: 	TReadRequest
//Description: 	Request object created for every read request from a
//				client. These are queued as a singly linked list.
//-----------------------------------------------------------------------

class TReadRequest
	{
public:
	inline TReadRequest(const RMessage2&);
	const RMessage2 iMessage2;
	
	TSglQueLink   iLink;
	};



//-----------------------------------------------------------------------
//Class name: 	CReadRequestHandler
//Description: 	Request Handler class created for Read media. This 
//				Class is an active object and will handle all read
//				requests in the form of RReadRequest objects.
//-----------------------------------------------------------------------

class CReadRequestHandler : public CActive
	{
public:
	static CReadRequestHandler* NewL(CStreamBase* aConsoleRead);
	~CReadRequestHandler();
	void RunL();
	void ServiceL(const RMessage2& aMessage2);
	void NotifyActivityL(const RMessage2& aMessage2);
	void CancelNotify();
	void DoCancel();
	
private:
	CReadRequestHandler();
	CReadRequestHandler(CStreamBase* aConsoleRead);
	void Complete(const RMessage2 &aMessage);
	TSglQue<TReadRequest> iQueue; 
	CStreamBase* iReadStream;
	TInt iNotifyHandle;
	TPtr8 iPtr;
	};
	
#endif //_READREQUEST_H_
	 


