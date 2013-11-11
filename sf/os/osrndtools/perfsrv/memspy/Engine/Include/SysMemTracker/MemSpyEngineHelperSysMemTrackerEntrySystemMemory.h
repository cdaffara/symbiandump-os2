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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYSYSTEMMEMORY_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYSYSTEMMEMORY_H

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


enum TSystemMemoryType
    {
    ETypeTotal = 0,
    ETypeFree
    };


NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerEntrySystemMemory ) : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntrySystemMemory* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TSystemMemoryType aType );
    ~CMemSpyEngineHelperSysMemTrackerEntrySystemMemory();

private:
    CMemSpyEngineHelperSysMemTrackerEntrySystemMemory( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TSystemMemoryType aType );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;

private: // Data members
    const TSystemMemoryType iType;
    TInt iLast;
    TInt iCurrent;
    };





/**
 * Change descriptor associated with Ram-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory* NewLC( TUint8 aAttribs, TSystemMemoryType aType, TInt aCurrent, TInt aLast );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory( TUint8 aAttribs, TSystemMemoryType aType, TInt aCurrent, TInt aLast );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    const TSystemMemoryType iType;
    const TInt iCurrent;
    const TInt iLast;
    };





#endif