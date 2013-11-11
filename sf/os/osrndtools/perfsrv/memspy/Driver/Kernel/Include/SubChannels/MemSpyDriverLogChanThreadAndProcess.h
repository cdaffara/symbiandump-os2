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

#ifndef MEMSPYDRIVERLOGICALCHANTHREADANDPROCESS_H
#define MEMSPYDRIVERLOGICALCHANTHREADANDPROCESS_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanBase.h"

// Classes referenced
class DMemSpyDriverDevice;
class TMemSpyDriverProcessInfo;
class TMemSpyDriverInternalThreadInfoParams;


NONSHARABLE_CLASS(DMemSpyDriverLogChanThreadAndProcess) : public DMemSpyDriverLogChanBase
	{
public:
	DMemSpyDriverLogChanThreadAndProcess( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanThreadAndProcess();
    TInt Construct();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
	TInt GetInfoThread(TUint aTid, TMemSpyDriverInternalThreadInfoParams* aParams );
	TInt GetInfoProcess(TUint aTid, TMemSpyDriverProcessInfo* aParams );
    TInt EndThread( TUint aId, TExitType aType );
    TInt OpenThread( TUint aId );
    TInt OpenProcess( TUint aId );
    TInt SuspendAllThreadsInProcess( TUint aPid );
    TInt ResumeAllThreadsInProcess( TUint aPid );
    TInt GetThreadsForProcess( TUint aPid, TDes8* aBufferSink );
    TInt SetPriority( TUint aTid, TThreadPriority aPriority );

private: // Internal methods
    static TInt MapToMemSpyExitReason( TExitType aType );

private: // Data members
	};


#endif
