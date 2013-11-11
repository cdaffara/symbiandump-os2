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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYCHUNK_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYCHUNK_H

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





class CMemSpyEngineHelperSysMemTrackerEntryChunk : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryChunk* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo );
    static CMemSpyEngineHelperSysMemTrackerEntryChunk* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo, CMemSpyProcess& aProcess );
    ~CMemSpyEngineHelperSysMemTrackerEntryChunk();

private:
    CMemSpyEngineHelperSysMemTrackerEntryChunk( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    void ConstructL( const TMemSpyDriverChunkInfo& aInfo, const TDesC& aChunkFullName );
    void ConstructL( const TMemSpyDriverChunkInfo& aInfo, CMemSpyProcess& aProcess );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;

private: // Data members
    TMemSpyDriverChunkInfo iLast;
    TMemSpyDriverChunkInfo iCurrent;
    HBufC* iChunkName;
    };




/**
 * Change descriptor associated with chunk-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeChunk ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeChunk* NewLC( TUint8 aAttribs, const TDesC& aChunkName, const TMemSpyDriverChunkInfo& aCurrent, const TMemSpyDriverChunkInfo* aLast = NULL );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeChunk();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeChunk( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aCurrent );
    void ConstructL( const TDesC& aChunkName, const TMemSpyDriverChunkInfo* aLast );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iChunkName;
    TMemSpyDriverChunkInfo iCurrent;
    TMemSpyDriverChunkInfo* iLast;
    };






#endif