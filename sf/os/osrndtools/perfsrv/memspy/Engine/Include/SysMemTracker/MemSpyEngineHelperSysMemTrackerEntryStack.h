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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYSTACK_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYSTACK_H

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




class CMemSpyEngineHelperSysMemTrackerEntryStack : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryStack* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyThread& aThread, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo );
    ~CMemSpyEngineHelperSysMemTrackerEntryStack();

private:
    CMemSpyEngineHelperSysMemTrackerEntryStack( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo );
    void ConstructL( CMemSpyThread& aThread );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );

private: // Internal methods
    TInt StackSize() const;

private: // Data members
    TMemSpyDriverChunkInfo iChunkInfo;
    TThreadStackInfo iStackInfo;
    HBufC* iThreadName;
    };







/**
 * Change descriptor associated with stack-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeStack ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeStack* NewLC( TUint8 aAttribs, const TDesC& aThreadName, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeStack();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeStack( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo );
    void ConstructL( const TDesC& aThreadName );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Internal methods
    TInt StackSize() const;

private: // Data members
    const TMemSpyDriverChunkInfo iChunkInfo;
    const TThreadStackInfo iStackInfo;
    HBufC* iThreadName;
    };








#endif