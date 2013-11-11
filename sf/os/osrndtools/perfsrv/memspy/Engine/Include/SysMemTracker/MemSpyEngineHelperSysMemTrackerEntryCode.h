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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYCODE_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYCODE_H

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
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;



class CMemSpyEngineHelperSysMemTrackerEntryCode : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryCode* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const CMemSpyEngineCodeSegEntry& aInfo );
    ~CMemSpyEngineHelperSysMemTrackerEntryCode();

private:
    CMemSpyEngineHelperSysMemTrackerEntryCode( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    void ConstructL( const CMemSpyEngineCodeSegEntry& aInfo );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );

private: // Data members
    HBufC* iCodeSegName;
    TInt iSize;
    };






/**
 * Change descriptor associated with code-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeCode ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeCode* NewLC( TUint8 aAttribs, const TDesC& aCodeSegName, TUint32 aSize, TUint32 aHandle );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeCode();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeCode( TUint8 aAttribs, TUint32 aSize, TUint32 aHandle );
    void ConstructL( const TDesC& aCodeSegName );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iCodeSegName;
    TUint32 iSize;
    TUint32 iHandle;
    };


#endif