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

#ifndef MEMSPYENGINEMIDWIFE_H
#define MEMSPYENGINEMIDWIFE_H

// System includes
#include <e32base.h>

// Classes referenced
class RMemSpyDriverClient;


class MMemSpyEngineMidwifeObserver
	{
public: // From MMemSpyEngineMidwifeObserver
	virtual void ThreadIsBornL( const TThreadId& aId, const RThread& aThread ) = 0;
	virtual void ProcessIsBornL( const TProcessId& aId, const RProcess& aProcess ) = 0;
	};


NONSHARABLE_CLASS( CMemSpyEngineMidwife ) : public CActive
    {
public:
	static CMemSpyEngineMidwife* NewL( RMemSpyDriverClient& aDriver, TInt aPriority = CActive::EPriorityStandard );
	~CMemSpyEngineMidwife();

private:
	CMemSpyEngineMidwife( RMemSpyDriverClient& aDriver, TInt aPriority );
	void ConstructL();

public: // API
    void AddObserverL( MMemSpyEngineMidwifeObserver& aObserver );
    void RemoveObserver( MMemSpyEngineMidwifeObserver& aObserver );

private: // From CActive
	void RunL();
	void DoCancel();

private: // Internal methods
    void Request();
    void NotifyProcessBornL( const TProcessId& aId );
    void NotifyThreadBornL( const TThreadId& aId );

private: // Data members
    TUint iId;
    RMemSpyDriverClient& iDriver;
    TUint iEventMonitorHandle;
	RPointerArray< MMemSpyEngineMidwifeObserver > iObservers;
    };




#endif
