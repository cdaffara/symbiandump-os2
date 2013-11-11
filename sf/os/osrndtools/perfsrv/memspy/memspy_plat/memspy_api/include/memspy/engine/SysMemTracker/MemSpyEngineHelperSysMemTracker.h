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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKER_H
#define MEMSPYENGINEHELPERSYSMEMTRACKER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyenginehelpersysmemtrackerobserver.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineHelperSysMemTrackerImp;
class TMemSpyEngineHelperSysMemTrackerConfig;
class CMemSpyEngineHelperSysMemTrackerCycle;
class MMemSpyEngineHelperSysMemTrackerObserver;
class CMemSpyEngineHelperSysMemTrackerOutputFormatter;


NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTracker ) : public CBase, public MMemSpyEngineHelperSysMemTrackerObserver, public MDesCArray
    {
public:
    static CMemSpyEngineHelperSysMemTracker* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperSysMemTracker();

private:
    CMemSpyEngineHelperSysMemTracker();
    void ConstructL( CMemSpyEngine& aEngine );

public: // API
    IMPORT_C TBool IsActive() const;
    IMPORT_C void StartL();
    IMPORT_C void StartL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    IMPORT_C void StopL();
    //
    IMPORT_C void SetConfigL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    IMPORT_C void GetConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    //
    IMPORT_C void SetObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver );
    IMPORT_C void RemoveObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver );
    //
    IMPORT_C const RPointerArray< CMemSpyEngineHelperSysMemTrackerCycle >& CompletedCycles() const;

    IMPORT_C void CheckForChangesNowL();

public: // But not exported
    void Reset();

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // From MMemSpyEngineHelperSysMemTrackerObserver
    void HandleCyclesResetL();
    void HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private:
    CMemSpyEngineHelperSysMemTrackerImp* iImp;
    MMemSpyEngineHelperSysMemTrackerObserver* iRealObserver;
    CMemSpyEngineHelperSysMemTrackerOutputFormatter* iOutputFormatter;
    };



#endif