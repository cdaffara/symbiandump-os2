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

#ifndef MEMSPYDRIVERUSEREVENTMONITOR_H
#define MEMSPYDRIVERUSEREVENTMONITOR_H

// System includes
#include <e32cmn.h>
#include <kern_priv.h>
#ifdef __MARM__
#include <arm.h>
#endif

// User includes
#include "MemSpyDriverEventMonitor.h"

// Classes referenced
class DMemSpyDriverDevice;
class DMemSpyDriverClientEM;


NONSHARABLE_CLASS(DMemSpyDriverClientEMManager) : public DBase
    {
public:
    DMemSpyDriverClientEMManager( DMemSpyDriverDevice& aDevice );
    ~DMemSpyDriverClientEMManager();
    TInt Create();
    
public: // API
    DMemSpyDriverClientEM* EMOpen();
    TInt EMClose( TUint aHandle );
    DMemSpyDriverClientEM* EMInstance( TUint aHandle );

private: // Internal methods
    void FreeAllInstances();

private:
    DMemSpyDriverDevice& iDevice;
    TUint iNextHandle;
	SDblQue iEMInstances;
    };


/**
 * This class represents a client-originated Event Monitor request
 */
NONSHARABLE_CLASS(DMemSpyDriverClientEM) : public DBase, public MMemSpyEventMonitorObserver
	{
public:
	DMemSpyDriverClientEM( DMemSpyDriverDevice& aDevice, TUint aHandle );
	~DMemSpyDriverClientEM();
    TInt Create();

public: // API
    inline TUint Handle() const { return iHandle; }
    //
	TInt NotifyChanges( DThread* aClientThread, TRequestStatus* aClientRS, TAny* aClientContext );
	TInt NotifyChangesCancel();

public: // From MMemSpyEventMonitorObserver
    TUint EMTypeMask() const;
    void EMHandleProcessAdd( DProcess& aProcess );
    void EMHandleProcessRemoved( DProcess& aProcess );
    void EMHandleThreadAdd( DThread& aThread );
    void EMHandleThreadKilled( DThread& aThread );
    void EMHandleChunkAdd( DChunk& aChunk );
    void EMHandleChunkDeleted( DChunk& aChunk );

private: // Internal methods
    void ResetPendingChanges();
    void CompleteClientsRequest( TInt aCompletionCode, TUint aContext = 0 );

private: // Internal objects

    class TChange
        {
    public:
        inline TChange( TInt aCompletionCode, TUint aContext )
        :   iCompletionCode( aCompletionCode ), iContext( aContext )
            {
            }

    public: // Data members
        TInt iCompletionCode;
    	TUint iContext;
    	SDblQueLink iLink;
        };

private:
    DMemSpyDriverDevice& iDevice;
	DMutex* iLock;
    const TUint iHandle;
	SDblQue iPendingChanges;

    // Transient: client-originated
    DThread* iClientThread;
    TRequestStatus* iClientRS;
    TAny* iClientContext;

public:
    SDblQueLink iLink;
	};


#endif
