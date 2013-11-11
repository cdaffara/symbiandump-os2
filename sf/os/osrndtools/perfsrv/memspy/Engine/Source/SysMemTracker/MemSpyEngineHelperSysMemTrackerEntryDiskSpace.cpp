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

#include "MemSpyEngineHelperSysMemTrackerEntryDiskSpace.h"

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>


CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::CMemSpyEngineHelperSysMemTrackerEntryDiskSpace( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TDriveNumber aDrive )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeDiskSpace ), iDrive( aDrive )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::~CMemSpyEngineHelperSysMemTrackerEntryDiskSpace()
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::ConstructL()
    {
    UpdateVolumeInfoL( iCurrent );
    iLast = iCurrent;
    }


CMemSpyEngineHelperSysMemTrackerEntryDiskSpace* CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TDriveNumber aDrive )
    {
    CMemSpyEngineHelperSysMemTrackerEntryDiskSpace* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryDiskSpace( aTracker, aDrive );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::Key() const
    {
    const TUint32 val = ( Type() << 28 );
    TUint64 ret = val;
    ret <<= 32;
    ret += iDrive;
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::NewLC( Attributes(), iDrive, iCurrent, IsNew() ? NULL : &iLast );
    aCycle.AddAndPopL( changeDescriptor );
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrent.iFree != iLast.iFree ) || ( iCurrent.iSize != iLast.iSize );
    TRACE( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::HasChangedL() - drive: %C:\\, iCurrent.iSize: %Ld, iLast.iSize: %Ld, iCurrent.iFree: %Ld, iLast.iFree: %Ld"), iDrive + 'A', iCurrent.iSize, iLast.iSize, iCurrent.iFree, iLast.iFree ) ); 
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryDiskSpace& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryDiskSpace& >( aEntry );

    TRACE( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::UpdateFromL() - START - drive: %C:\\, iCurrent.iSize: %Ld, iLast.iSize: %Ld, iCurrent.iFree: %Ld, iLast.iFree: %Ld"), iDrive + 'A', iCurrent.iSize, iLast.iSize, iCurrent.iFree, iLast.iFree ) ); 
  
    // Update state
    iLast = iCurrent;
    iCurrent = entry.iCurrent;

    TRACE( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::UpdateFromL() - END - drive: %C:\\, iCurrent.iSize: %Ld, iLast.iSize: %Ld, iCurrent.iFree: %Ld, iLast.iFree: %Ld"), iDrive + 'A', iCurrent.iSize, iLast.iSize, iCurrent.iFree, iLast.iFree ) ); 
    }


void CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::UpdateVolumeInfoL( TVolumeInfo& aInfo )
    {
    RFs& fsSession = Engine().FsSession();
    User::LeaveIfError( fsSession.Volume( aInfo, iDrive ) );
    TRACE( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::UpdateVolumeInfoL() - drive: %C:\\, aInfo.iSize: %Ld, aInfo.iFree: %Ld"), iDrive + 'A', iCurrent.iSize, iLast.iSize, iCurrent.iFree, iLast.iFree ) ); 
    }



























CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace( TUint8 aAttribs, TDriveNumber aDrive )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iDrive( aDrive )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::~CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace()
    {
    delete iName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::ConstructL( const TVolumeInfo& aCurrent, const TVolumeInfo* /*aLast*/ )
    {
    BaseConstructL();

    // Format drive letter
    _LIT( KDriveFormatSpec, "EDrive%c" );
    TChar driveChar( 'A' + iDrive );
    iDriveName.Format( KDriveFormatSpec, (TInt) driveChar );

    iName = aCurrent.iName.AllocL();
    iUniqueID = aCurrent.iUniqueID;
    iSize = aCurrent.iSize;
    iFree = aCurrent.iFree;
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace* CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::NewLC( TUint8 aAttribs, TDriveNumber aDrive, const TVolumeInfo& aCurrent, const TVolumeInfo* aLast )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace( aAttribs, aDrive );
    CleanupStack::PushL( self );
    self->ConstructL( aCurrent, aLast );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeDiskSpace;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderLine, "Type, Drive Name, Drive Number, Size, Free, UID, Volume Name, Attribs");
    aSink.OutputLineL( KHeaderLine );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeDiskSpace::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,%1d,%ld,%ld,%8x,%S,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf( buf->Des() );

    // Now output line
    pBuf.Format( KFormat,
                 &type,
                 &iDriveName,
                 iDrive,
                 iSize,
                 iFree,
                 iUniqueID,
                 iName,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


