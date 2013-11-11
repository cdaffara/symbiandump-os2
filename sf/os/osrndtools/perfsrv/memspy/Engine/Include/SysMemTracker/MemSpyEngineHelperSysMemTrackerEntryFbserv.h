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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYFBSERV_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYFBSERV_H

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



class CMemSpyEngineHelperSysMemTrackerEntryFbserv : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryFbserv* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aCurrentLarge, const TMemSpyDriverChunkInfo& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount );
    ~CMemSpyEngineHelperSysMemTrackerEntryFbserv();

private:
    CMemSpyEngineHelperSysMemTrackerEntryFbserv( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aCurrentLarge, const TMemSpyDriverChunkInfo& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );

private: // Internal methods

private: // Data members
    TMemSpyDriverChunkInfo iCurrentLarge;
    TMemSpyDriverChunkInfo iCurrentShared;
    TMemSpyDriverChunkInfoWithoutName iLastShared;
    TMemSpyDriverChunkInfoWithoutName iLastLarge;
    TInt iCurrentBitmapConCount;
    TInt iCurrentFontConCount;
    TInt iLastBitmapConCount;
    TInt iLastFontConCount;
    TInt iCurrentAccessibleBitmapCount;
    TInt iLastAccessibleBitmapCount;
    };





/**
 * Change descriptor associated with bitmap changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv* NewLC( TUint8 aAttribs, const TMemSpyDriverChunkInfoWithoutName& aCurrentLarge, const TMemSpyDriverChunkInfoWithoutName& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv( TUint8 aAttrib, const TMemSpyDriverChunkInfoWithoutName& aCurrentLarge, const TMemSpyDriverChunkInfoWithoutName& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    const TMemSpyDriverChunkInfoWithoutName iSharedChunk;
    const TMemSpyDriverChunkInfoWithoutName iLargeChunk;
    const TInt iBitmapConCount;
    const TInt iFontConCount;
    const TInt iAccessibleBitmapCount;
    };





#endif