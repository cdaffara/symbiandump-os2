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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYBITMAP_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYBITMAP_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include "MemSpyEngineHelperSysMemTrackerEntries.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>

// Classes referenced
class CFbsBitmap;
class CMemSpyEngine;
class CMemSpyThread;
class CMemSpyProcess;
class CMemSpyEngineOutputSink;
class CMemSpyEngineOpenFileListEntry;
class CMemSpyEngineOpenFileListForThread;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;
class TMemSpyEngineFBServBitmapInfo;



class CMemSpyEngineHelperSysMemTrackerEntryBitmap : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryBitmap* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TInt aHandle );
    ~CMemSpyEngineHelperSysMemTrackerEntryBitmap();

private:
    CMemSpyEngineHelperSysMemTrackerEntryBitmap( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    void ConstructL( TInt aHandle );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );

private: // Internal methods

private: // Data members
    TMemSpyEngineFBServBitmapInfo* iInfo;
    };





/**
 * Change descriptor associated with bitmap changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap* NewLC( TUint8 aAttribs, const TMemSpyEngineFBServBitmapInfo& aInfo );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap( TUint8 aAttrib );
    void ConstructL( const TMemSpyEngineFBServBitmapInfo& aInfo );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    TMemSpyEngineFBServBitmapInfo* iInfo;
    CFbsBitmap* iBitmap;
    };





#endif