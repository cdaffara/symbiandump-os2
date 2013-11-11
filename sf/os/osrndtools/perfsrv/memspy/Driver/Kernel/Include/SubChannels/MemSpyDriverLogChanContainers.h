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

#ifndef MEMSPYDRIVERLOGICALCHANCONTAINERS_H
#define MEMSPYDRIVERLOGICALCHANCONTAINERS_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// User includes
#include "MemSpyDriverLogChanContainerBase.h"

// Classes referenced
class TMemSpyDriverPAndSInfo;
class DMemSpyDriverDevice;
class TMemSpyDriverHandleInfoGeneric;
class TMemSpyDriverInternalContainerHandleParams;
class TMemSpyDriverInternalRefsToThreadOrProcess;
class TMemSpyDriverInternalCondVarSuspendedThreadParams;
class TMemSpyDriverCondVarSuspendedThreadInfo;


NONSHARABLE_CLASS(DMemSpyDriverLogChanContainers) : public DMemSpyDriverLogChanContainerBase
	{
public:
	DMemSpyDriverLogChanContainers( DMemSpyDriverDevice& aDevice, DThread& aThread );
	~DMemSpyDriverLogChanContainers();
    TInt Construct();

public: // From DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetContainerHandles( TMemSpyDriverInternalContainerHandleParams* aParams );
    TInt GetKernelObjectSizeApproximation( TMemSpyDriverContainerType aType, TInt* aSize );
    TInt GetGenericHandleInfo( TInt aTid, TMemSpyDriverHandleInfoGeneric* aParams );
    TInt GetReferencesToMyThread( TUint aTid, TDes8* aBufferSink );
    TInt GetReferencesToMyProcess( TUint aTid, TDes8* aBufferSink );
    TInt GetPAndSInfo( DObject* aHandle, TMemSpyDriverPAndSInfo* aInfo );
    TInt GetCondVarSuspendedThreads( TMemSpyDriverInternalCondVarSuspendedThreadParams* aParams );
    TInt GetCondVarSuspendedThreadInfo( TAny* aThreadHandle, TMemSpyDriverCondVarSuspendedThreadInfo* aParams );

    
private: // Internal methods
    TInt SearchThreadsFor( DObject* aHandleToLookFor, RMemSpyMemStreamWriter& aStream );
    TInt SearchProcessFor( DObject* aHandleToLookFor, RMemSpyMemStreamWriter& aStream );
    static TMemSpyDriverTimerState MapToMemSpyTimerState( TTimer::TTimerState aState );
    static TMemSpyDriverTimerType MapToMemSpyTimerType( TTimer::TTimerType aType );
	};


#endif
