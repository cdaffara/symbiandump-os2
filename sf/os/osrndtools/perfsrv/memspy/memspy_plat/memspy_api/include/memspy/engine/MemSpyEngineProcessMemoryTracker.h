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

#ifndef MEMSPYENGINEPROCESSMEMORYTRACKER_H
#define MEMSPYENGINEPROCESSMEMORYTRACKER_H

// System includes
#include <e32base.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Classes referenced
class CMemSpyEngine;



class MMemSpyEngineProcessMemoryTrackerObserver
    {
public: // From MMemSpyEngineProcessMemoryTrackerObserver
    virtual void HandleMemoryTrackingStartedL() = 0;
    virtual void HandleMemoryTrackingStoppedL() = 0;
    virtual void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared ) = 0;
    };


NONSHARABLE_CLASS( TMemSpyEngineProcessInspectionInfoWithPeakInfo ) : public TMemSpyDriverProcessInspectionInfo
    {
public:
    inline TMemSpyEngineProcessInspectionInfoWithPeakInfo()
        {
        }

    inline void ResetPeakFlags()
        {
        iPeakStack = EFalse;
        iPeakHeap = EFalse;
        iPeakChunkLocal = EFalse;
        iPeakChunkShared = EFalse;
        iPeakGlobalData = EFalse;
        }

public:
    TBool iPeakStack;
    TBool iPeakHeap;
    TBool iPeakChunkLocal;
    TBool iPeakChunkShared;
    TBool iPeakGlobalData;
    };




NONSHARABLE_CLASS( CMemSpyEngineProcessMemoryTracker ) : public CActive
    {
public:
    static CMemSpyEngineProcessMemoryTracker* NewLC( CMemSpyEngine& aEngine, const TProcessId& aProcessId );
    ~CMemSpyEngineProcessMemoryTracker();

private:
    CMemSpyEngineProcessMemoryTracker( CMemSpyEngine& aEngine, const TProcessId& aProcessId );
    void ConstructL();

public: // API
    IMPORT_C TBool AmTracking() const;
    IMPORT_C void StartL();
    IMPORT_C void Stop();
    IMPORT_C void AddObserverL( MMemSpyEngineProcessMemoryTrackerObserver& aObserver );
    IMPORT_C void RemoveObserver( MMemSpyEngineProcessMemoryTrackerObserver& aObserver );
    IMPORT_C void ResetHWML();
    //
    inline TProcessId ProcessId() const { return iProcessId; }
    inline const TMemSpyDriverProcessInspectionInfo& InfoCurrent() const { return iInfoCurrent; }
    inline const TMemSpyDriverProcessInspectionInfo& InfoPeaks() const { return static_cast< const TMemSpyDriverProcessInspectionInfo& >( iInfoPeaks ); }
    inline const TMemSpyDriverProcessInspectionInfo& InfoHWMIncShared() const { return iInfoHWMIncShared; }
    inline const TMemSpyDriverProcessInspectionInfo& InfoHWMExcShared() const { return iInfoHWMIncShared; }
    
private: // From CActive
    void RunL();
    void DoCancel();

private: // Internal methods
    void PrintInfoL( const TMemSpyDriverProcessInspectionInfo& aInfo );
    void UpdatePeaks( const TMemSpyDriverProcessInspectionInfo& aLatestInfo );
    void UpdateHWMIncShared( const TMemSpyDriverProcessInspectionInfo& aLatestInfo );
    void UpdateHWMExcShared( const TMemSpyDriverProcessInspectionInfo& aLatestInfo );

private: // Data members
    CMemSpyEngine& iEngine;
    const TProcessId iProcessId;
    TSecureId iSID;
    TBool iAmTracking;
    HBufC* iProcessName;
    TInt iNumberOfChangesReceived;
    RPointerArray<MMemSpyEngineProcessMemoryTrackerObserver> iObservers;

    TMemSpyDriverProcessInspectionInfo iInfoCurrent;
    TMemSpyDriverProcessInspectionInfo iInfoHWMIncShared;
    TMemSpyDriverProcessInspectionInfo iInfoHWMExcShared;
    TMemSpyEngineProcessInspectionInfoWithPeakInfo iInfoPeaks;
    };




#endif
