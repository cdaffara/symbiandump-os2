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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERCYCLE_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERCYCLE_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Classes referenced
class RFs;
class CMemSpyEngineOutputList;
class CMemSpyEngineOutputSink;
class TMemSpyEngineHelperSysMemTrackerConfig;
class CMemSpyEngineHelperSysMemTrackerCycleChange;



/**
 * A cycle object represents one or more changes that have taken place 
 * in the entire OS
 *
 * The individual elements that make up the overall cycle description are
 * obtained by iterating through the various CMemSpyEngineHelperSysMemTrackerCycleChange
 * instances owned by this object.
 *
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycle ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycle* NewLC( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig, const CMemSpyEngineHelperSysMemTrackerCycle* aPreviousCycle = NULL );
    ~CMemSpyEngineHelperSysMemTrackerCycle();

private:
    CMemSpyEngineHelperSysMemTrackerCycle( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig, const CMemSpyEngineHelperSysMemTrackerCycle* aPreviousCycle = NULL );
    void ConstructL();

public: // API - info about this cycle
    IMPORT_C TInt CycleNumber() const;
    IMPORT_C const TTime& Time() const;
    IMPORT_C const TDesC& TimeFormatted() const;
    IMPORT_C const TDesC& Caption() const;
    IMPORT_C const TInt64& MemoryFree() const;
    IMPORT_C TInt64 MemoryDelta() const;
    IMPORT_C TInt64 MemoryFreePreviousCycle() const;

public: // API - access specific changes
    IMPORT_C TInt ChangeCount() const;

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

public: // Internal API
    void FinalizeL();
    void AddAndPopL( CMemSpyEngineHelperSysMemTrackerCycleChange* aInfo );
    void DiscardChanges();
    CMemSpyEngineHelperSysMemTrackerCycleChange& ChangeAt( TInt aIndex );
    void GetDataFolderL( RFs& aFsSession, TDes& aFolder );
    void DataStreamBeginL( CMemSpyEngineOutputSink& aSink, const TDesC& aContext );
    void DataStreamEndL( CMemSpyEngineOutputSink& aSink );

public: // Misc
    const TMemSpyEngineHelperSysMemTrackerConfig& Config() const;
    //
    void AddToMemoryUsed( TInt aValue );
    void AddToMemoryHeapAllocs( TInt aValue );
    void AddToMemoryHeapFrees( TInt aValue );
    void AddToCellCountFree( TInt aValue );
    void AddToCellCountAlloc( TInt aValue );
    //
    const TInt64& MemoryUsed() const;
    const TInt64& MemoryHeapAllocs() const;
    const TInt64& MemoryHeapFrees() const;
    const TInt64& MemoryHeapCellCountAlloc() const;
    const TInt64& MemoryHeapCellCountFree() const;

private: // Data members
    const TMemSpyEngineHelperSysMemTrackerConfig& iConfig;
    const CMemSpyEngineHelperSysMemTrackerCycle* iPreviousCycle;
    TInt iCycleNumber;
    TTime iTime;
    TInt64 iMemoryFree;
    HBufC* iTimeFormatted;
    HBufC* iCaption;
    TInt iChangeCount;
    CMemSpyEngineOutputList* iFixedItems;
    RPointerArray< CMemSpyEngineHelperSysMemTrackerCycleChange > iChangeDescriptors;

private: // Data members - transient
    TInt64 iMemoryUsed;
    TInt64 iMemoryHeapFrees;
    TInt64 iMemoryHeapAllocs;
    TInt64 iCellCountsFrees;
    TInt64 iCellCountsAllocs;
    };





#endif