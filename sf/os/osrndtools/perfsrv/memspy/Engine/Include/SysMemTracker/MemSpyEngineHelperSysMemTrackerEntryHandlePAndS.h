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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYHANDLEPANDS_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYHANDLEPANDS_H

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
class CMemSpyEngineCodeSegEntry;
class CMemSpyEngineOutputSink;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;



class CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TAny* aHandle );
    ~CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS();

private:
    CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    void ConstructL( TAny* aHandle );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iName;
    HBufC* iThreadName;
    TMemSpyDriverPAndSInfo iInfo;
    };





/**
 * Change descriptor associated with handle-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS* NewLC( TUint8 aAttribs, TUint32 aHandle, const TMemSpyDriverPAndSInfo& aInfo, const TDesC& aName, const TDesC& aThreadName );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS( TUint8 aAttribs, TUint32 aHandle, const TMemSpyDriverPAndSInfo& aInfo );
    void ConstructL( const TDesC& aName, const TDesC& aThreadName );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iName;
    HBufC* iThreadName;
    TUint32 iHandle;
    TMemSpyDriverPAndSInfo iInfo;
    };



#endif