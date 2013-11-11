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

#ifndef MEMSPYDRIVERSUSPENSIONMANAGER_H
#define MEMSPYDRIVERSUSPENSIONMANAGER_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>

// Classes referenced
class DMemSpyDriverDevice;


NONSHARABLE_CLASS(DMemSpySuspensionManager) : public DBase
	{
public: // Construct & destruct
	DMemSpySuspensionManager( DMemSpyDriverDevice& aDevice );
	~DMemSpySuspensionManager();
	TInt Construct();

public: // API
    TBool IsSuspended( TUint aPid ) const;
    TBool IsSuspended( DThread& aThread ) const;
    TBool IsSuspended( DProcess& aProcess ) const;
    TInt SuspendAllThreadsInProcess( TUint aPid, DThread& aClientThread );
    TInt ResumeAllThreadsInProcess( TUint aPid, DThread& aClientThread );

private: // Internal methods
    TInt DoSuspendAllThreadsInProcess( TUint aPid, DThread* aClientThread = NULL );
    TInt DoResumeAllThreadsInProcess( TUint aPid, DThread* aClientThread = NULL );
    TBool IsProcessTheClientThread( TUint aPid, DThread& aClientThread ) const;
    TBool CheckProcessSuspended( TUint aExpectedPid, DThread& aClientThread ) const;
	TInt OpenTempObject(TUint aId, TObjectType aType );
	void CloseTempObject();

private: // Data members
    DMemSpyDriverDevice& iDevice;
	DObject* iTempObj;
    TBool iAlreadySuspended;
	TUint iSuspendedProcessId;
	TInt iSuspendCount;
	};


#endif
