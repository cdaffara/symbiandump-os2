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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYHEAP_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYHEAP_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include "MemSpyEngineHelperSysMemTrackerEntries.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h> 

// Classes referenced
class CMemSpyEngine;
class CMemSpyThread;
class CMemSpyProcess;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;



class CMemSpyEngineHelperSysMemTrackerEntryHeap : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryHeap* NewUserLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyThread& aThread );
    static CMemSpyEngineHelperSysMemTrackerEntryHeap* NewKernelLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    ~CMemSpyEngineHelperSysMemTrackerEntryHeap();

private:
    CMemSpyEngineHelperSysMemTrackerEntryHeap( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TMemSpyEngineSysMemTrackerType aType );
    void ConstructL();
    void ConstructL( CMemSpyThread& aThread );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );
    void SetAsShared( TBool aShared );

private: // Internal methods
    void UpdateHeapInfoL( TMemSpyHeapInfo& aInfo );
    TBool HasHeapSizeChanged() const;
    TBool HaveFreeCellsChanged() const;
    TBool HaveAllocCellsChanged() const;

private: // Data members
    TMemSpyHeapInfo iLast;
    TMemSpyHeapInfo iCurrent;
    HBufC* iThreadName;
    };



/**
 * Change descriptor associated with heap-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeHeap ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeHeap* NewLC( TUint8 aAttribs, const TDesC& aThreadName, const TMemSpyHeapInfo& aCurrent, const TMemSpyHeapInfo* aLast = NULL );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeHeap();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeHeap( TUint8 aAttribs, const TMemSpyHeapInfo& aCurrent );
    void ConstructL( const TDesC& aThreadName, const TMemSpyHeapInfo* aLast );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Internal methods
    TBool IsKernel() const;
    void FormatAttributes( TDes& aBuffer ) const;

private: // Data members
    HBufC* iThreadName;
    TMemSpyHeapInfo iCurrent;
    TMemSpyHeapInfo* iLast;
    };






#endif