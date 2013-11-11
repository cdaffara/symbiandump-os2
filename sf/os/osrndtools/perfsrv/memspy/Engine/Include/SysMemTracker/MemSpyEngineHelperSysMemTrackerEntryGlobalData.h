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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYGLOBALDATA_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYGLOBALDATA_H

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



class CMemSpyEngineHelperSysMemTrackerGlobalData : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerGlobalData* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyProcess& aProcess, const TMemSpyDriverChunkInfo& aChunkInfo );
    ~CMemSpyEngineHelperSysMemTrackerGlobalData();

private:
    CMemSpyEngineHelperSysMemTrackerGlobalData( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    void ConstructL( CMemSpyProcess& aProcess, const TMemSpyDriverChunkInfo& aChunkInfo );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;

private: // Internal methods
    TBool IsProcessGlobalData() const;
    TInt CurrentGlobalDataSize() const;

private: // Data members
    TMemSpyDriverChunkInfoWithoutName iCurrent;
    TMemSpyDriverChunkInfoWithoutName* iLast;
    HBufC* iChunkName;
    };







/**
 * Change descriptor associated with global data-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData* NewLC( TUint8 aAttribs, const TDesC& aChunkName, const TMemSpyDriverChunkInfoWithoutName& aCurrent, const TMemSpyDriverChunkInfoWithoutName* aLast );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData( TUint8 aAttribs, const TMemSpyDriverChunkInfoWithoutName& aCurrent );
    void ConstructL( const TDesC& aChunkName, const TMemSpyDriverChunkInfoWithoutName* aLast );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iChunkName;
    TMemSpyDriverChunkInfoWithoutName iCurrent;
    TMemSpyDriverChunkInfoWithoutName* iLast;
    };






#endif