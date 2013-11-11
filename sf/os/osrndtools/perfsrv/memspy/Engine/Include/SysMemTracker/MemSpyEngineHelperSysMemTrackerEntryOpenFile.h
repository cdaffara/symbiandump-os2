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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYOPENFILE_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYOPENFILE_H

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
class CMemSpyEngineOpenFileListEntry;
class CMemSpyEngineOpenFileListForThread;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class TMemSpyEngineHelperSysMemTrackerConfig;




class CMemSpyEngineHelperSysMemTrackerEntryOpenFile : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryOpenFile* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const CMemSpyEngineOpenFileListForThread& aThreadInfo, const CMemSpyEngineOpenFileListEntry& aEntryInfo );
    ~CMemSpyEngineHelperSysMemTrackerEntryOpenFile();

private:
    CMemSpyEngineHelperSysMemTrackerEntryOpenFile( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    void ConstructL( const CMemSpyEngineOpenFileListForThread& aThreadInfo, const CMemSpyEngineOpenFileListEntry& aEntryInfo );

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aInfo );

private: // Internal methods

private: // Data members
    TInt iSize;
    HBufC* iFileName;
    HBufC* iThreadName;
    TInt iUniqueFileId;
    };












/**
 * Change descriptor associated with open file-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile* NewLC( TUint8 aAttribs, const TDesC& aThreadName, const TDesC& aFileName, TInt aSize, TInt aUniqueFileId );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile( TUint8 aAttrib, TInt aSize, TInt aUniqueFileId );
    void ConstructL( const TDesC& aThreadName, const TDesC& aFileName );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    HBufC* iFileName;
    HBufC* iThreadName;
    TInt iSize;
    TInt iUniqueFileId;
    };


#endif