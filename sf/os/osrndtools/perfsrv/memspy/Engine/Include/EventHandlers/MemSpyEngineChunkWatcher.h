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

#ifndef MEMSPYENGINECHUNKWATCHER_H
#define MEMSPYENGINECHUNKWATCHER_H

// System includes
#include <e32base.h>

// Classes referenced
class RMemSpyDriverClient;


class MMemSpyEngineChunkWatcherObserver
	{
public: // From MMemSpyEngineChunkWatcherObserver
	virtual void HandleChunkAddL( TUint aChunkHandle ) = 0;
    virtual void HandleChunkDestroyL( TUint aChunkHandle ) = 0;
	};


NONSHARABLE_CLASS( CMemSpyEngineChunkWatcher ) : public CActive
    {
public:
	static CMemSpyEngineChunkWatcher* NewL( RMemSpyDriverClient& aDriver, TInt aPriority = CActive::EPriorityStandard );
	~CMemSpyEngineChunkWatcher();

private:
	CMemSpyEngineChunkWatcher( RMemSpyDriverClient& aDriver, TInt aPriority );
	void ConstructL();

public: // API
    void AddObserverL( MMemSpyEngineChunkWatcherObserver& aObserver );
    void RemoveObserver( MMemSpyEngineChunkWatcherObserver& aObserver );

private: // From CActive
	void RunL();
	void DoCancel();

private: // Internal methods
    void Request();
    void NotifyChunkAddL( TUint aChunkHandle );
    void NotifyChunkDestroyL( TUint aChunkHandle );

private: // Data members
    TUint iId;
    TUint iEventMonitorHandle;
    RMemSpyDriverClient& iDriver;
	RPointerArray< MMemSpyEngineChunkWatcherObserver > iObservers;
    };




#endif
