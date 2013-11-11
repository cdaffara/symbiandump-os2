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

#ifndef MEMSPYENGINEHELPERPROCESS_H
#define MEMSPYENGINEHELPERPROCESS_H

// System includes
#include <e32base.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyenginemidwife.h>
#include <memspy/engine/memspyengineundertaker.h>

// Classes referenced
class CMemSpyProcess;
class CMemSpyEngine;
class CMemSpyEngineProcessMemoryTracker;


NONSHARABLE_CLASS( CMemSpyEngineHelperProcess ) : public CBase, public MMemSpyEngineMidwifeObserver, MMemSpyEngineUndertakerObserver
    {
public:
    static CMemSpyEngineHelperProcess* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperProcess();

private:
    CMemSpyEngineHelperProcess( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C void ImpersonateL( TUint32 aSID = 0 );
    IMPORT_C void OutputProcessInfoL( const CMemSpyProcess& aProcess );
    IMPORT_C void OutputProcessInfoDetailedL( const CMemSpyProcess& aProcess );
    IMPORT_C void SetMemoryTrackingAutoStartProcessListL( const RArray<TUid>& aProcessUids );
    inline const RArray<TUid>& MemoryTrackingAutoStartProcessList() const { return iMemoryTrackerAutoStartProcessList; }
    CMemSpyEngineProcessMemoryTracker* TrackerOrNull( const TProcessId& aId );
    CMemSpyEngineProcessMemoryTracker& TrackerL( const TProcessId& aId );
    CMemSpyEngineProcessMemoryTracker& TrackerL( const CMemSpyProcess& aProcess );

private: // From MMemSpyEngineMidwifeObserver
	void ThreadIsBornL( const TThreadId& aId, const RThread& aThread );
	void ProcessIsBornL( const TProcessId& aId, const RProcess& aProcess );

private: // From MMemSpyEngineUndertakerObserver
	void ThreadIsDeadL( const TThreadId& aId, const RThread& aThread );
	void ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess );

private: // Internal methods
    void OutputProcessInfoL( const CMemSpyProcess& aProcess, TBool aDetailed );
    TBool IsAutoStartupProcess( TUid aUid ) const;

private:
    CMemSpyEngine& iEngine;
    CMemSpyEngineUndertaker* iUndertaker;
    RArray<TUid> iMemoryTrackerAutoStartProcessList;
    RPointerArray<CMemSpyEngineProcessMemoryTracker> iMemoryTrackers;
    };




#endif