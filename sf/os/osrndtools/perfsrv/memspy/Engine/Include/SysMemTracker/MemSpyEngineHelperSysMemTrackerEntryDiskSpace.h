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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYDISKSPACE_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYDISKSPACE_H

// System includes
#include <e32base.h>
#include <badesca.h>
#include <f32file.h>

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





class CMemSpyEngineHelperSysMemTrackerEntryDiskSpace : public CMemSpyEngineHelperSysMemTrackerEntry
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryDiskSpace* NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TDriveNumber aDrive );
    ~CMemSpyEngineHelperSysMemTrackerEntryDiskSpace();

private:
    CMemSpyEngineHelperSysMemTrackerEntryDiskSpace( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TDriveNumber aDrive );
    void ConstructL();

public: // From CMemSpyEngineHelperSysMemTrackerEntry
    TUint64 Key() const;
    void UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry );
    void CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    TBool HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig ) const;

private: // Internal methods
    void UpdateVolumeInfoL( TVolumeInfo& aInfo );

private: // Data members
    const TDriveNumber iDrive;
    TVolumeInfo iLast;
    TVolumeInfo iCurrent;
    };









/**
 * Change descriptor associated with open file-related changes
 */
NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace ) : public CMemSpyEngineHelperSysMemTrackerCycleChange
    {
public:
    static CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace* NewLC( TUint8 aAttribs, TDriveNumber aDrive, const TVolumeInfo& aCurrent, const TVolumeInfo* aLast );
    ~CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace();

private:
    CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace( TUint8 aAttrib, TDriveNumber aDrive );
    void ConstructL( const TVolumeInfo& aCurrent, const TVolumeInfo* aLast );

public: // From CMemSpyEngineHelperSysMemTrackerCycleChange
    TMemSpyEngineSysMemTrackerType Type() const;
    void OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    const TDriveNumber iDrive;
    TBuf<7> iDriveName;
    //
    HBufC* iName;
    TUint iUniqueID;
    TInt64 iSize;
    TInt64 iFree;
    };


#endif