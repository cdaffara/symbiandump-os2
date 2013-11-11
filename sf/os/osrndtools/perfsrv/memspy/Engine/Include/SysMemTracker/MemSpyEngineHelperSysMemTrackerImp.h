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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERIMP_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERIMP_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>

// Classes referenced
class CMemSpyEngine;
class MMemSpyEngineHelperSysMemTrackerObserver;
class TMemSpyEngineHelperSysMemTrackerConfig;
class CMemSpyEngineHelperSysMemTrackerCycle;
class CMemSpyEngineHelperSysMemTrackerEntryManager;



NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerImp ) : public CTimer, public MDesCArray
    {
public:
    static CMemSpyEngineHelperSysMemTrackerImp* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperSysMemTrackerImp();

private:
    CMemSpyEngineHelperSysMemTrackerImp( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    void StartL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    void StopL();

    void SetObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver );
    void RemoveObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver );
    
    const RPointerArray< CMemSpyEngineHelperSysMemTrackerCycle >& CompletedCycles() const;

public: // From MDesCArray
    TInt MdcaCount() const;
    TPtrC MdcaPoint( TInt aIndex ) const;

public: // But not exported
    void Reset();
    void CheckForChangesNowL();
    //
    inline CMemSpyEngine& Engine() { return iEngine; }
    inline const TMemSpyEngineHelperSysMemTrackerConfig& Config() const { return iConfig; }

private: // Internal methods
    void PrepareInitialCycleL();
    void CheckForChangesL();
    void NotifyObserverCycleStartedL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void NotifyObserverCycleFinishedL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void AddCycleAndPopL( CMemSpyEngineHelperSysMemTrackerCycle* aCycle );
    CMemSpyEngineHelperSysMemTrackerCycle* LastCycleOrNull();

private: // From CActive
    void RunL();
    TInt RunError( TInt aError );

private:
    CMemSpyEngine& iEngine;
    TMemSpyEngineHelperSysMemTrackerConfig iConfig;
    CMemSpyEngineHelperSysMemTrackerEntryManager* iEntryManager;
    RPointerArray< CMemSpyEngineHelperSysMemTrackerCycle > iCompletedCycles;
    MMemSpyEngineHelperSysMemTrackerObserver* iObserver;
    };



#endif