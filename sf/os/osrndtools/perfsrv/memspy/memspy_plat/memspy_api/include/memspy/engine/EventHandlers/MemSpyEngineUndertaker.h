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

#ifndef MEMSPYENGINEUNDERTAKER_H
#define MEMSPYENGINEUNDERTAKER_H

// System includes
#include <e32base.h>

// Classes referenced
class RMemSpyDriverClient;


class MMemSpyEngineUndertakerObserver
	{
public: // From MMemSpyEngineUndertakerObserver
	virtual void ThreadIsDeadL( const TThreadId& aId, const RThread& aThread ) = 0;         // aThread may not be initialised
	virtual void ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess ) = 0;     // aProcess may not be initialised
	};


NONSHARABLE_CLASS( CMemSpyEngineUndertaker ) : public CActive
    {
public:
	static CMemSpyEngineUndertaker* NewL( RMemSpyDriverClient& aDriver, TInt aPriority = CActive::EPriorityStandard );
	~CMemSpyEngineUndertaker();

private:
	CMemSpyEngineUndertaker( RMemSpyDriverClient& aDriver, TInt aPriority );
	void ConstructL();

public: // API
    void AddObserverL( MMemSpyEngineUndertakerObserver& aObserver );
    void RemoveObserver( MMemSpyEngineUndertakerObserver& aObserver );

private: // From CActive
	void RunL();
	void DoCancel();

private: // Internal methods
    void Request();
    void NotifyProcessDeadL( const TProcessId& aId );
    void NotifyThreadDeadL( const TThreadId& aId );

private: // Data members
    TUint iId;
    TUint iEventMonitorHandle;
    RMemSpyDriverClient& iDriver;
	RPointerArray< MMemSpyEngineUndertakerObserver > iObservers;
    };




#endif