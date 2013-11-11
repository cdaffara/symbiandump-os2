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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYHANDLEGENERIC_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYHANDLEGENERIC_H

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



class CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TAny* aHandle, TMemSpyDriverContainerType aType );
    ~CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric();

private:
    CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TMemSpyDriverContainerType aType );
    void ConstructL( TAny* aHandle );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iName;
    TMemSpyDriverContainerType iHandleType;
    };





/**
 * Change descriptor associated with handle-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric* NewLC( TUint8 aAttribs, const TDesC& aName, TUint32 aHandle, TMemSpyDriverContainerType aType );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric( TUint8 aAttribs, TUint32 aHandle, TMemSpyDriverContainerType aType );
    void ConstructL( const TDesC& aName );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iName;
    TUint32 iHandle;
    TMemSpyDriverContainerType iType;
    };



#endif