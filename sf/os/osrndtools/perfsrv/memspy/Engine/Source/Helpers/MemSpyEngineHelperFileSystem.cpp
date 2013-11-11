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

#include <memspy/engine/memspyenginehelperfilesystem.h>

// System includes
#include <f32file.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include "MemSpyEngineOutputListItem.h"

// Remap of E32 constant since this value only exists in recent v9.3 builds and MemSpy needs to build on v9.1
const TUint KMemSpyDriveAttPageable = 0x100; // KDriveAttPageable
const TUint KMemSpyMediaAttPageable = 0x200; // KMediaAttPageable

// Literal constants
_LIT( KMemSpyVolumeLabelNotSet, "None" );


CMemSpyEngineHelperFileSystem::CMemSpyEngineHelperFileSystem( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperFileSystem::~CMemSpyEngineHelperFileSystem()
    {
    }


void CMemSpyEngineHelperFileSystem::ConstructL()
    {
    iFileServerProcessId = MemSpyEngineUtils::IdentifyFileServerProcessIdL();
    }


CMemSpyEngineHelperFileSystem* CMemSpyEngineHelperFileSystem::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperFileSystem* self = new(ELeave) CMemSpyEngineHelperFileSystem( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CMemSpyEngineDriveList* CMemSpyEngineHelperFileSystem::DriveListL()
    {
    CMemSpyEngineDriveList* list = CMemSpyEngineDriveList::NewLC( iEngine.FsSession() );
    CleanupStack::Pop( list );
    return list;
    }


EXPORT_C TDriveNumber CMemSpyEngineHelperFileSystem::GetSystemDrive()
    {
    TDriveNumber ret = EDriveC;
#ifndef __WINS__
    const TInt KRFsGetSystemDriveOrdinal = 336;
    typedef TDriveNumber(*TRFsGetSystemDriveFunc)();
    //
    RLibrary lib;
    const TInt loadErr = lib.Load( _L("EFSRV.DLL") );
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperFileSystem::GetSystemDrive() - dll load: %d", loadErr );
#endif
    if  ( loadErr == KErrNone )
        {
        TLibraryFunction ordinal = lib.Lookup( KRFsGetSystemDriveOrdinal );
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyEngineHelperFileSystem::GetSystemDrive() - ordinal: 0x%08x", ordinal );
#endif
        //
        if  ( ordinal )
            {
            TRFsGetSystemDriveFunc func = (TRFsGetSystemDriveFunc) ordinal;
            ret = (*func)();
#ifdef _DEBUG
            RDebug::Printf( "CMemSpyEngineHelperFileSystem::GetSystemDrive() - drive: %c:\\", 'A' + ret);
#endif
            }
        //
        lib.Close();
        }
#endif
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineHelperFileSystem::ListOpenFilesL()
    {
    TOpenFileScan scanner( iEngine.FsSession() );

    _LIT( KMemSpyContext, "Open File Listing" );
    _LIT( KMemSpyFolder, "OpenFiles" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

    _LIT(KHeading, "OPEN FILES");
    iEngine.Sink().OutputSectionHeadingL( KHeading, '=' );
    
    // Set prefix for overall listing
    _LIT(KOverallPrefix, "OpenFiles - ");
    iEngine.Sink().OutputPrefixSetLC( KOverallPrefix );

    CFileList* list = NULL;
    scanner.NextL( list );
    while( list != NULL )
        {
        CleanupStack::PushL( list );
        ListOpenFilesL( *list, scanner.ThreadId() );
        CleanupStack::PopAndDestroy( list );
        //
        list = NULL;
        scanner.NextL( list );
        }

    CleanupStack::PopAndDestroy(); // prefix
    iEngine.Sink().DataStreamEndL();
    }


EXPORT_C void CMemSpyEngineHelperFileSystem::ListOpenFilesL( const TThreadId& aThreadId )
    {
    TOpenFileScan scanner( iEngine.FsSession() );

    _LIT( KMemSpyContext, "Open File Listing" );
    _LIT( KMemSpyFolder, "OpenFiles" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

    _LIT(KHeading, "OPEN FILES");
    iEngine.Sink().OutputSectionHeadingL( KHeading, '=' );
    
    // Set prefix for overall listing
    _LIT(KOverallPrefix, "OpenFiles - ");
    iEngine.Sink().OutputPrefixSetLC( KOverallPrefix );

    CFileList* list = NULL;
    scanner.NextL( list );
    while( list != NULL )
        {
        if  ( scanner.ThreadId() == aThreadId )
            {
            CleanupStack::PushL( list );
            ListOpenFilesL( *list, scanner.ThreadId() );
            CleanupStack::Pop( list );
            }
        //
        delete list;
        list = NULL;
        scanner.NextL( list );
        }

    CleanupStack::PopAndDestroy(); // prefix
    iEngine.Sink().DataStreamEndL();
    }


void CMemSpyEngineHelperFileSystem::GetDriveNumberText( TDriveNumber aDriveNumber, TDes& aText )
    {
    const TInt offset = aDriveNumber - EDriveA;
    const TChar driveChar = 'A' + offset;
    aText.Copy( _L("EDrive") );
    aText.Append( driveChar );
    }


void CMemSpyEngineHelperFileSystem::ListOpenFilesL( const CFileList& aList, const TThreadId& aThreadId )
    {
    _LIT( KLineFormatSpec, "%d %S %S %S %S" );
    TBuf<64> timeBuf;
    TMemSpySizeText valueBuf;

    HBufC* itemBuf = HBufC::NewLC( 3 * KMaxFullName );
    TPtr item( itemBuf->Des() );

    TFullName threadName;
    RThread thread;
    if  ( iEngine.Driver().OpenThread( aThreadId, thread ) == KErrNone )
        {
        threadName = thread.FullName();
        thread.Close();
        }

    const TInt entryCount = aList.Count();
    for(TInt i=0; i<entryCount; i++)
        {
        const TEntry& entry = aList[ i ];

        // Get time and size format strings
        valueBuf = MemSpyEngineUtils::FormatSizeText( entry.iSize );
        MemSpyEngineUtils::FormatTimeL( timeBuf, entry.iModified );

        // Create item 
        item.Format( KLineFormatSpec, 0, &entry.iName, &valueBuf, &timeBuf, &threadName );
        iEngine.Sink().OutputLineL( item );
        }

    CleanupStack::PopAndDestroy( itemBuf );
    }


CMemSpyEngineOpenFileList* CMemSpyEngineHelperFileSystem::ListOpenFilesLC()
    {
    CMemSpyEngineOpenFileList* list = CMemSpyEngineOpenFileList::NewLC( iEngine );
    return list;
    }


TProcessId CMemSpyEngineHelperFileSystem::FileServerProcessId() const
    {
    return iFileServerProcessId;
    }


const CMemSpyEngineChunkEntry* CMemSpyEngineHelperFileSystem::IdentifyCacheChunk( const CMemSpyEngineChunkList& aList, TInt& aIndex ) const
    {
    aIndex = KErrNotFound;
    const CMemSpyEngineChunkEntry* ret = NULL;
    //
    const TInt count = aList.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineChunkEntry& entry = aList.At( i );
        const TMemSpyDriverChunkInfo& info = entry.Info();
        //
        if  ( info.iType == EMemSpyDriverChunkTypeLocal )
            {
#ifdef _DEBUG
            RDebug::Print( _L("CMemSpyEngineHelperFileSystem::IdentifyCacheChunk() - name: %S, pid: 0x%04x, f32Pid: 0x%04x, size: %d, max: %d, attribs: %d"), &info.iName, info.iOwnerId, (TUint) iFileServerProcessId, info.iSize, info.iMaxSize, info.iAttributes );
#endif

            if ( info.iOwnerId == FileServerProcessId() )
                {
                // Additionally, the chunk is disconnected.
                const TBool isDisconnected = ( info.iAttributes & EMemSpyChunkAttributesDisconnected );
#ifdef _DEBUG
                RDebug::Printf( "CMemSpyEngineHelperFileSystem::IdentifyCacheChunk() - it is an f32 local chunk... isDisconnected: %d", isDisconnected );
#endif

                if  ( info.iSize > 0 && info.iMaxSize > 0 && isDisconnected )
                    {
                    aIndex = i;
                    ret = &entry;
#ifdef _DEBUG
                    RDebug::Print( _L("CMemSpyEngineHelperFileSystem::IdentifyCacheChunk() - FOUND: %S, size: %d"), &info.iName, info.iSize );
#endif
                    break;
                    }
                }
            }
        }
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperFileSystem::IdentifyCacheChunk() - END - ret: 0x%08x", ret );
#endif
    return ret;
    }





















CMemSpyEngineDriveList::CMemSpyEngineDriveList( RFs& aFsSession )
:   CActive( EPriorityStandard ), iFsSession( aFsSession )
    {
    CActiveScheduler::Add( this );
    }


EXPORT_C CMemSpyEngineDriveList::~CMemSpyEngineDriveList()
    {
    Cancel();
    //
    iItems.ResetAndDestroy();
    iItems.Close();
    }


void CMemSpyEngineDriveList::ConstructL()
    {
    TDriveList drives;
    if  ( iFsSession.DriveList( drives ) == KErrNone )
        {
        for( TInt i=EDriveA; i<=EDriveZ; i++ )
            {
            const TDriveNumber drive = static_cast< TDriveNumber >( i );
            if  ( drives[ i ] != KDriveAbsent )
                {
                TRAP_IGNORE( 
                    CMemSpyEngineDriveEntry* driveEntry = CMemSpyEngineDriveEntry::NewLC( *this, drive );
                    iItems.AppendL( driveEntry );
                    CleanupStack::Pop( driveEntry );
                    );
                }
            }
        }

    iFsSession.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }


CMemSpyEngineDriveList* CMemSpyEngineDriveList::NewLC( RFs& aFsSession )
    {
    CMemSpyEngineDriveList* self = new(ELeave) CMemSpyEngineDriveList( aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CMemSpyEngineDriveList::RunL()
    {
    iFsSession.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    //
    NotifyListChangedL();
    }


void CMemSpyEngineDriveList::DoCancel()
    {
    iFsSession.NotifyChangeCancel();
    }


EXPORT_C void CMemSpyEngineDriveList::SetObserver( MMemSpyEngineDriveListObserver* aObserver )
    {
    iObserver = aObserver;
    }


EXPORT_C TInt CMemSpyEngineDriveList::Count() const
    {
    return iItems.Count();
    }


EXPORT_C CMemSpyEngineDriveEntry& CMemSpyEngineDriveList::At( TInt aIndex )
    {
    return *iItems[ aIndex ];
    }


EXPORT_C const CMemSpyEngineDriveEntry& CMemSpyEngineDriveList::At( TInt aIndex ) const
    {
    return *iItems[ aIndex ];
    }


EXPORT_C TInt CMemSpyEngineDriveList::ItemIndex( const CMemSpyEngineDriveEntry& aEntry ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineDriveEntry* item = iItems[ i ];
        //
        if  ( item == &aEntry )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


EXPORT_C CMemSpyEngineDriveEntry* CMemSpyEngineDriveList::EntryByDriveNumber( TDriveNumber aDrive )
    {
    CMemSpyEngineDriveEntry* ret = NULL;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyEngineDriveEntry* item = iItems[ i ];
        //
        if  ( item->DriveNumber() == aDrive )
            {
            ret = item;
            break;
            }
        }
    //
    return ret;
    }


RFs& CMemSpyEngineDriveList::FsSession()
    {
    return iFsSession;
    }


void CMemSpyEngineDriveList::NotifyListChangedL()
    {
    if  ( iObserver != NULL )
        {
        TRAP_IGNORE( iObserver->HandleDriveListChangedL( *this ) );
        }
    }


EXPORT_C TInt CMemSpyEngineDriveList::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineDriveList::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineDriveEntry& item = At( aIndex );
    return TPtrC( item.Caption() );
    }


















CMemSpyEngineDriveEntry::CMemSpyEngineDriveEntry( CMemSpyEngineDriveList& aList, TDriveNumber aDriveNumber )
:   CActive( CActive::EPriorityStandard), iList( aList ), iDriveNumber( aDriveNumber ), iDriveUnit( aDriveNumber )
    {
    CActiveScheduler::Add( this );
    }


EXPORT_C CMemSpyEngineDriveEntry::~CMemSpyEngineDriveEntry()
    {
    Cancel();
    //
    delete iCaption;
    delete iItems;
    }


void CMemSpyEngineDriveEntry::ConstructL()
    {
    RunL();
    }


EXPORT_C void CMemSpyEngineDriveEntry::SetObserver( MMemSpyEngineDriveEntryObserver* aObserver )
    {
    iObserver = aObserver;
    }


void CMemSpyEngineDriveEntry::RefreshL()
    {
    CMemSpyEngineOutputList* items = CMemSpyEngineOutputList::NewL();
    delete iItems;
    iItems = items;

    /*
	RFs13VolumeIOParamEiR18TVolumeIOParamInfo @ 344 NONAME
	RFs17FileSystemSubTypeEiR6TDes16 @ 345 NONAME
	RFs18QueryVolumeInfoExtEi22TQueryVolumeInfoExtCmdR5TDes8 @ 346 NONAME
    */
    const TInt err1 = FsSession().Drive( iDriveInfo, iDriveNumber );
    const TInt err2 = FsSession().Volume( iVolumeInfo, iDriveNumber );
    if  ( err1 || err2 )
        {
        _LIT( KUnavailable, "Unavailable");
        iItems->AddItemL( KUnavailable );

        // Mostly empty caption
        CreateCaptionL( KNullDesC, KNullDesC );
        }
    else
        {
        // Literal constants
        TBuf<KMaxFileName * 2> temp;
        TBuf<KMaxFileName * 2> item;

        AddStandardFieldsL();

        _LIT( KEntry2a, "Media Type");
        TBuf<20> mediaType;
        GetMediaTypeText( iDriveInfo.iType, mediaType );
        iItems->AddItemL( KEntry2a, mediaType );

        _LIT( KEntry2b, "Battery Status");
        GetBatteryState( iDriveInfo.iBattery, temp );
        iItems->AddItemL( KEntry2b, temp );

        // Now have enough data to create caption
        CreateCaptionL( iVolumeInfo.iName, mediaType );
        
        CreateDriveAttributesL( item );
        CreateMediaAttributesL( item );

        _LIT( KEntry3a, "Volume Name");
        if  ( iVolumeInfo.iName.Length() )
            {
            iItems->AddItemL( KEntry3a, iVolumeInfo.iName );
            }
        else
            {
            iItems->AddItemL( KEntry3a, KMemSpyVolumeLabelNotSet );
            }

        _LIT( KEntry3, "Volume UID");
        iItems->AddItemHexL( KEntry3, iVolumeInfo.iUniqueID );

        _LIT( KEntry4, "Volume Size");
        temp = MemSpyEngineUtils::FormatSizeText( iVolumeInfo.iSize );
        iItems->AddItemL( KEntry4, temp );

        _LIT( KEntry5, "Volume Free");
        temp = MemSpyEngineUtils::FormatSizeText( iVolumeInfo.iFree );
        iItems->AddItemL( KEntry5, temp );

        TMediaSerialNumber serialNum;
        if  ( FsSession().GetMediaSerialNumber( serialNum, iDriveNumber ) == KErrNone )
            {
            _LIT( KEntry6, "Serial Number");
            TBuf<KMaxSerialNumLength> serialNumUnicode;
            serialNumUnicode.Copy( serialNum );
            iItems->AddItemL( KEntry6, serialNumUnicode );
            }

        if  ( FsSession().FileSystemName( temp, iDriveNumber ) == KErrNone )
            {
            _LIT( KEntry7, "File System");
            iItems->AddItemL( KEntry7, temp );
            }
        }
    }


CMemSpyEngineDriveEntry* CMemSpyEngineDriveEntry::NewLC( CMemSpyEngineDriveList& aList, TDriveNumber aDriveNumber )
    {
    CMemSpyEngineDriveEntry* self = new(ELeave) CMemSpyEngineDriveEntry( aList, aDriveNumber );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CMemSpyEngineDriveEntry::CreateDriveAttributesL( TDes& aItemBuf )
    {
    TInt driveAttribIndex = 0;
    _LIT( KEntryText, "Drive Attribute %d");

    if  ( iDriveInfo.iDriveAtt & KDriveAttLocal )
        {
        _LIT( KAttribName, "Local" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttRom )
        {
        _LIT( KAttribName, "ROM" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttRedirected )
        {
        _LIT( KAttribName, "Redirected" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttSubsted )
        {
        _LIT( KAttribName, "Substed" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttInternal )
        {
        _LIT( KAttribName, "Internal" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttRemovable )
        {
        _LIT( KAttribName, "Removable" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttRemote )
        {
        _LIT( KAttribName, "Remote" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttTransaction )
        {
        _LIT( KAttribName, "Transaction" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KMemSpyDriveAttPageable )
        {
        _LIT( KAttribName, "Pageable" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iDriveAtt & KDriveAttRemovable )
        {
        _LIT( KAttribName, "Removable" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    }


void CMemSpyEngineDriveEntry::CreateMediaAttributesL( TDes& aItemBuf )
    {
    TInt driveAttribIndex = 0;
    _LIT( KEntryText, "Media Attribute %d");

    if  ( iDriveInfo.iMediaAtt & KMediaAttVariableSize )
        {
        _LIT( KAttribName, "Variable Size" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttDualDensity )
        {
        _LIT( KAttribName, "Dual Density" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttFormattable )
        {
        _LIT( KAttribName, "Formattable" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttWriteProtected )
        {
        _LIT( KAttribName, "Write Protected" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttLockable )
        {
        _LIT( KAttribName, "Lockable" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttLocked )
        {
        _LIT( KAttribName, "Locked" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttHasPassword )
        {
        _LIT( KAttribName, "Has Password" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttReadWhileWrite )
        {
        _LIT( KAttribName, "Read-while-Write" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMediaAttDeleteNotify )
        {
        _LIT( KAttribName, "Delete Notify" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    if  ( iDriveInfo.iMediaAtt & KMemSpyMediaAttPageable )
        {
        _LIT( KAttribName, "Pageable" );
        aItemBuf.Format( KEntryText, ++driveAttribIndex );
        iItems->AddItemL( aItemBuf, KAttribName );
        }
    }


void CMemSpyEngineDriveEntry::AddStandardFieldsL()
    {
    _LIT( KEntry1, "Unit Name");
    const TDriveName driveUnitName = iDriveUnit.Name();
    iItems->AddItemL( KEntry1, driveUnitName );

    _LIT( KEntry2, "Unit Number");
    TBuf<30> temp;
    CMemSpyEngineHelperFileSystem::GetDriveNumberText( iDriveNumber, temp );
    iItems->AddItemL( KEntry2, temp );

    const TDriveNumber sysDrive = GetSystemDrive();
    if  ( sysDrive == iDriveNumber )
        {
        _LIT( KEntry3, "System Drive");
        iItems->AddItemYesNoL( KEntry3, ETrue );
        }
    }


void CMemSpyEngineDriveEntry::CreateCaptionL( const TDesC& aVolumeName, const TDesC& aMediaType )
    {
    TBuf<256 * 2> caption;

    // Always include drive unit name
    const TDriveName driveUnitName = iDriveUnit.Name();
    caption.AppendFormat( _L("\t%S"), &driveUnitName );

    // If we have a volume name, then include that too
    if  ( aVolumeName.Length() )
        {
        caption.AppendFormat( _L(" [%S]"), &aVolumeName );
        }
    
    // New line
    caption.Append( _L("\t\t") );

    // Include media type if known, or unknown otherwise
    if  ( aMediaType.Length() )
        {
        caption.Append( aMediaType );
        }
    else
        {
        _LIT( KUnknownMediaType, "Unknown" );
        caption.Append( KUnknownMediaType );
        }

    HBufC* finalCaption = caption.AllocL();
    delete iCaption;
    iCaption = finalCaption;
    }

void CMemSpyEngineDriveEntry::GetMediaTypeText( TMediaType aType, TDes& aText )
    {
    switch( aType )
        {
    case EMediaNotPresent:
        aText.Copy( _L("Not Present") );
        break;
    case EMediaFloppy:
        aText.Copy( _L("Floppy") );
        break;
    case EMediaHardDisk:
        aText.Copy( _L("Hard Disk") );
        break;
    case EMediaCdRom:
        aText.Copy( _L("CD-ROM") );
        break;
    case EMediaRam:
        aText.Copy( _L("RAM") );
        break;
    case EMediaFlash:
        aText.Copy( _L("Flash") );
        break;
    case EMediaRom:
        aText.Copy( _L("ROM") );
        break;
    case EMediaRemote:
        aText.Copy( _L("Remote") );
        break;
    case EMediaNANDFlash:
        aText.Copy( _L("NAND Flash") );
        break;
    default:
    case EMediaUnknown:
        aText.Copy( _L("Unknown") );
        break;
        }
    }


void CMemSpyEngineDriveEntry::GetBatteryState( TBatteryState aBatteryState, TDes& aText )
    {
    switch( aBatteryState )
        {
    case EBatNotSupported:
        aText.Copy( _L("Not Supported") );
        break;
    case EBatGood:
        aText.Copy( _L("Good") );
        break;
    case EBatLow:
        aText.Copy( _L("Low") );
        break;
    default:
        aText.Copy( _L("Unknown") );
        break;
        }
    }


TDriveNumber CMemSpyEngineDriveEntry::GetSystemDrive()
    {
    return CMemSpyEngineHelperFileSystem::GetSystemDrive();
    }


EXPORT_C TInt CMemSpyEngineDriveEntry::MdcaCount() const
    {
    return iItems->Count();
    }


EXPORT_C TPtrC CMemSpyEngineDriveEntry::MdcaPoint( TInt aIndex ) const
    {
    return iItems->MdcaPoint( aIndex );
    }


void CMemSpyEngineDriveEntry::RunL()
    {
    FsSession().NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    //
    TRAP_IGNORE( RefreshL() );
    //
    if ( iObserver != NULL )
        {
        TRAP_IGNORE( iObserver->HandleDriveEntryChangedL( *this ) );
        }
    }


void CMemSpyEngineDriveEntry::DoCancel()
    {
    FsSession().NotifyChangeCancel();
    }


RFs& CMemSpyEngineDriveEntry::FsSession()
    {
    return iList.FsSession();
    }



































CMemSpyEngineOpenFileListEntry::CMemSpyEngineOpenFileListEntry()
    {
    }


CMemSpyEngineOpenFileListEntry::~CMemSpyEngineOpenFileListEntry()
    {
    delete iFileName;
    }


void CMemSpyEngineOpenFileListEntry::ConstructL( const TEntry& aEntry )
    {
    iFileName = aEntry.iName.AllocL();
    iAtt = aEntry.iAtt;
    iType = aEntry.iType;
    iSize = aEntry.iSize;
    iModified = aEntry.iModified;
    }


CMemSpyEngineOpenFileListEntry* CMemSpyEngineOpenFileListEntry::NewLC( const TEntry& aEntry )
    {
    CMemSpyEngineOpenFileListEntry* self = new(ELeave) CMemSpyEngineOpenFileListEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    return self;
    }


EXPORT_C const TDesC& CMemSpyEngineOpenFileListEntry::FileName() const
    {
    return *iFileName;
    }


EXPORT_C TInt CMemSpyEngineOpenFileListEntry::Size() const
    {
    return iSize;
    }


TInt CMemSpyEngineOpenFileListEntry::UniqueFileId() const
    {
    return iUniqueFileIdentifier;
    }


void CMemSpyEngineOpenFileListEntry::SetUniqueFileId( TInt aValue )
    {
    iUniqueFileIdentifier = aValue;
    }

























CMemSpyEngineOpenFileListForThread::CMemSpyEngineOpenFileListForThread( const TThreadId& aThreadId, const TProcessId& aProcessId )
:   iThreadId( aThreadId ), iProcessId( aProcessId )
    {
    }


CMemSpyEngineOpenFileListForThread::~CMemSpyEngineOpenFileListForThread()
    {
    iItems.ResetAndDestroy();
    iItems.Close();
    delete iThreadName;
    }


void CMemSpyEngineOpenFileListForThread::ConstructL( const TDesC& aThreadName, const CFileList& aList )
    {
    iThreadName = aThreadName.AllocL();

    // Create fs entries
    const TInt entryCount = aList.Count();
    for(TInt i=0; i<entryCount; i++)
        {
        const TEntry& fsEntry = (aList)[ i ];
        //
        CMemSpyEngineOpenFileListEntry* entry = CMemSpyEngineOpenFileListEntry::NewLC( fsEntry );
        AddL( entry );
        CleanupStack::Pop( entry );
        }
    }


CMemSpyEngineOpenFileListForThread* CMemSpyEngineOpenFileListForThread::NewLC( const TDesC& aThreadName, const TThreadId& aThreadId, const TProcessId& aProcessId, const CFileList& aList )
    {
    CMemSpyEngineOpenFileListForThread* self = new(ELeave) CMemSpyEngineOpenFileListForThread( aThreadId, aProcessId );
    CleanupStack::PushL( self );
    self->ConstructL( aThreadName, aList );
    return self;
    }


EXPORT_C const TDesC& CMemSpyEngineOpenFileListForThread::ThreadName() const
    {
    return *iThreadName;
    }


EXPORT_C const TThreadId& CMemSpyEngineOpenFileListForThread::ThreadId() const
    {
    return iThreadId;
    }


EXPORT_C const TProcessId& CMemSpyEngineOpenFileListForThread::ProcessId() const
    {
    return iProcessId;
    }


EXPORT_C TInt CMemSpyEngineOpenFileListForThread::Count() const
    {
    return iItems.Count();
    }


EXPORT_C const CMemSpyEngineOpenFileListEntry& CMemSpyEngineOpenFileListForThread::At( TInt aIndex ) const
    {
    return *iItems[ aIndex ];
    }
 

void CMemSpyEngineOpenFileListForThread::AddL( CMemSpyEngineOpenFileListEntry* aEntry )
    {
    // Ensure we increment the unique filename counter if adding an entry where the same filename
    // already is registered with the list.
    CMemSpyEngineOpenFileListEntry* existingEntry = EntryByFileName( aEntry->FileName() );
    if  ( existingEntry )
        {
        // EntryByFileName() always returns the most recent entry with the same name. The list
        // itself is not sorted as entries are always appended to it.
        const TInt lastUID = existingEntry->UniqueFileId();
#ifdef _DEBUG
        RDebug::Print( _L("CMemSpyEngineOpenFileListForThread::AddL() - searched for %S and found existing entry with UID: %d"), &aEntry->FileName(), lastUID );
#endif

        aEntry->SetUniqueFileId( lastUID + 1 );
        }

    iItems.AppendL( aEntry );
    }


CMemSpyEngineOpenFileListEntry* CMemSpyEngineOpenFileListForThread::EntryByFileName( const TDesC& aFileName )
    {
    CMemSpyEngineOpenFileListEntry* ret = NULL;
    //
    const TInt count = iItems.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        CMemSpyEngineOpenFileListEntry* item = iItems[ i ];
        if  ( item->FileName().CompareF( aFileName ) == 0 )
            {
            ret = item;
            break;
            }
        }
    //
    return ret;
    }










CMemSpyEngineOpenFileList::CMemSpyEngineOpenFileList()
    {
    }


EXPORT_C CMemSpyEngineOpenFileList::~CMemSpyEngineOpenFileList()
    {
    iItems.ResetAndDestroy();
    iItems.Close();
    }


void CMemSpyEngineOpenFileList::ConstructL( CMemSpyEngine& aEngine )
    {
    _LIT( KUnknownThread, "Unknown Thread" );
    TFullName threadName;
    TOpenFileScan scanner( aEngine.FsSession() );
    //
    CFileList* list = NULL;
    scanner.NextL( list );
    while( list != NULL )
        {
        CleanupStack::PushL( list );

        // Reset name
        threadName = KUnknownThread;
        
        // Try to get real thread name
        TProcessId procId;
        RThread thread;
        if  ( aEngine.Driver().OpenThread( scanner.ThreadId(), thread ) == KErrNone )
            {
            RProcess process;
            if ( thread.Process( process ) == KErrNone )
                {
                procId = process.Id();
                process.Close();
                }
            //
            thread.FullName( threadName );
            thread.Close();
            }

        // Create thread entry
        CMemSpyEngineOpenFileListForThread* threadEntry = CMemSpyEngineOpenFileListForThread::NewLC( threadName, scanner.ThreadId(), procId, *list );
        iItems.AppendL( threadEntry );
        CleanupStack::Pop( threadEntry );

        // Destroy list and move on to next thread.
        CleanupStack::PopAndDestroy( list );
        list = NULL;
        scanner.NextL( list );
        }
    }


CMemSpyEngineOpenFileList* CMemSpyEngineOpenFileList::NewLC( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineOpenFileList* self = new(ELeave) CMemSpyEngineOpenFileList();
    CleanupStack::PushL( self );
    self->ConstructL( aEngine );
    return self;
    }


EXPORT_C TInt CMemSpyEngineOpenFileList::Count() const
    {
    return iItems.Count();
    }


EXPORT_C const CMemSpyEngineOpenFileListForThread& CMemSpyEngineOpenFileList::At( TInt aIndex ) const
    {
    return *iItems[ aIndex ];
    }

