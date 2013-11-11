#ifndef __SNTPCLIENTENGINE_H__
#define __SNTPCLIENTENGINE_H__/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>

#include "commandlineargs.h"

enum TSNTPClientState {
	EStateIdle = 0,
	EStateResolve,
	EStateWrite,
	EStateRead,
	EStateComplete,
	EStateFailed,
	EStateAborted
	};

class MTimeOutNotify
	{
public:
	virtual void TimerExpired() = 0;
	};
	
// timeout handler

class CTimeOutTimer: public CTimer
	{
public:
	static CTimeOutTimer* NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify);
	~CTimeOutTimer();

protected:
    CTimeOutTimer(const TInt aPriority);
	void ConstructL(MTimeOutNotify& aTimeOutNotify);
    virtual void RunL();

private:
	MTimeOutNotify* iNotify;
	};
	
// The main client engine
	
class CSNTPClient : public CActive, public MTimeOutNotify
	{

public:
	static CSNTPClient* NewL(TCommandLineArgs& aArgs);
	static CSNTPClient* NewLC(TCommandLineArgs& aArgs);
	
	TSNTPClientState State();
	void Start();
	
	~CSNTPClient();
	
private:
	CSNTPClient(TCommandLineArgs& aArgs);

	void ConstructL();
	
	/* CActive methods */
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	/* MTimeOutNotify methods */
	void TimerExpired();
	
	void SetTimeL(); 
	
private:
	RSocketServ	iSockServ;
	RSocket iSock;
	RHostResolver iResolver;

	TCommandLineArgs& iArgs;
	TSNTPClientState iState;
	TNameEntry iNameEntry;
	
	CTimeOutTimer* iTimer;
	
	// An NTP packet is exactly 48 bytes
	TBuf8<48> iBuffer;
	
	TInt iServerIndex;
	};



#endif /* _SNTPCLIENTENGINE_H__ */
