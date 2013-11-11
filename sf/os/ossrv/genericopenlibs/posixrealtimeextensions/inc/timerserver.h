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
*
*/

#ifndef __TIMERSERVER_H__
#define __TIMERSERVER_H__

#include <e32base.h>
#include "timerutils.h"

enum TMyPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessagePtr2& aMessage,TMyPanic TMyPanic);

NONSHARABLE_CLASS(CShutdown) : public CTimer
   {
public:
    CShutdown() : CTimer(EPriorityStandard)
    	{
    	// nada
    	}
    void Start();
    void ConstructL();
private:
    static const TUint KTimerServerShutdownDelay=3000000;
    void RunL();
    };

NONSHARABLE_CLASS(CTimerServer) : public CServer2
    {
public:
	// We want the transfers to be done on a priority. Hence the EPriorityHigh
	CTimerServer() : CServer2(EPriorityHigh,ESharableSessions)
    	{
    	// nada
    	}
    	
    ~CTimerServer();
    static CTimerServer* NewLC();
    
    TInt AddToScheduler();
    void RemoveFromScheduler();
    TInt RemoveInternal(const TInt);
    TInt SetTimer(const TInt);
	void StartServer();
	static TInt NewTimerServerL();
    void AddSession();
    void DropSession();
    
    CActiveScheduler* iOldScheduler;
    
private:
    void ConstructL();
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
    TInt iSessionCount;
    CShutdown* iShutdown;
    };

NONSHARABLE_CLASS(CTimerServerSession) : public CSession2
    {
public:
    void CreateL();
private:
    ~CTimerServerSession();
    CTimerServer& Server();
    void ServiceL(const RMessage2& aMessage);
    void ServiceError(const RMessage2& aMessage, TInt aError);
	};
    
#endif //__TIMERSERVER_H__
