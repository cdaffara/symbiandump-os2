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

#include <memspy/engine/memspyengineutils.h>

// System includes
#include <e32svr.h>
#include <e32capability.h>
#ifdef __EPOC32__
#include <e32rom.h>
#endif

// User includes
#include <memspy/engine/memspyengineoutputsink.h>

// Typedefs
typedef TInt(*TSysUtilGetSWVersionFunction)(TDes&);

// Literal constants
_LIT( KMemSpyEngineDoubleColon, "::" );
_LIT( KMemSpyKernelProcessName, "ekern" );
_LIT( KMemSpyKernelProcessFullName, "ekern.exe[100041af]0001" );
_LIT( KMemSpyKernelThreadName, "Supervisor" );
_LIT( KMemSpyLogRootPath, "\\MemSpy\\" );
_LIT( KMemSpyLogDefaultName, "Log" );
_LIT( KMemSpyLogFileNameWithTimeStamp, "MemSpy - %4d%02d%02d - %02d.%02d.%02d.%06d - " );


EXPORT_C void MemSpyEngineUtils::FormatTimeL( TDes& aBuf, const TInt64& aTimeVal, TBool aLocalTime )
    {
    const TTime time( aTimeVal );
    FormatTimeL( aBuf, time, aLocalTime );
    }


EXPORT_C void MemSpyEngineUtils::FormatTimeL( TDes& aBuf, const TTime& aTime, TBool aLocalTime )
    {
    if  ( aLocalTime )
        {
        _LIT( KFormatSpecLocal, "%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
        // 
        TTime time( aTime );
        time += User::UTCOffset();
        time.FormatL( aBuf, KFormatSpecLocal );
        }
    else
        {
        _LIT( KFormatSpecUTC, "%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B UTC" );
        aTime.FormatL( aBuf, KFormatSpecUTC );
        }
    }


void MemSpyEngineUtils::FormatTimeSimple( TDes& aBuf, const TTime& aTime )
    {
    const TDateTime dt = aTime.DateTime();
    //
    _LIT( KTimeFormatSpec, "%04d%02d%02d %02d:%02d:%02d" );
    aBuf.Format( KTimeFormatSpec, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second() );
    }


EXPORT_C void MemSpyEngineUtils::FormatHex( TDes& aBuf, TInt aValue )
    {
    _LIT(KMemSpyNumericHexFormat, "0x%08x");
    aBuf.Format( KMemSpyNumericHexFormat, aValue );
    }


EXPORT_C TMemSpySizeText MemSpyEngineUtils::FormatSizeText( const TInt64& aValue, TInt aDecimalPlaces, TBool aExtraRounding )
    {
    _LIT(KFormatKilo, "%dK");
    _LIT(KFormatMega, "%SM");
    _LIT(KFormatGiga, "%SG");

	TMemSpySizeText buf;
	if  ( aValue < 1024000 )					// If < 1000K
		{
		TInt sizeInK = 0;

        if  ( aValue != 0 )
			{
			sizeInK = I64INT( (aValue + 512) >> 10 );
			if  (sizeInK < 1)
                {
				sizeInK = 1;
                }
			if  (sizeInK > 999)
                {
				sizeInK = 999;
                }
			}

        buf.Format( KFormatKilo, sizeInK );
		}
	else
		{
		TReal sizeInM = I64INT( aValue );
		sizeInM /= 1048576;
		if  ( sizeInM < 1 )
            {
			sizeInM = 1;
            }

        TPtrC pFormat( KFormatMega );
		if  ( sizeInM >= 1000 )
			{
			sizeInM /= 1024;				// Size in G
			if  (sizeInM < 1)
                {
				sizeInM = 1;
                }
			
            pFormat.Set( KFormatGiga );
			}

        if  ( sizeInM > 999.9)
            {
            sizeInM = 999.9;
            }

        if  ( aExtraRounding )
            {
			sizeInM += 0.499999;
            }

		TBuf<16> size;
		size.Num( sizeInM, TRealFormat( 14, aDecimalPlaces ) );	// Allow for "17179869184.0"G which is 2^64
        buf.Format( pFormat, &size );
		}

    return buf;
    }


EXPORT_C TMemSpySizeText MemSpyEngineUtils::FormatSizeText( TInt aValue )
    {
    const TInt64 value( aValue );
	return FormatSizeText( value );
    }


EXPORT_C TMemSpySizeText MemSpyEngineUtils::FormatSizeTextPrecise( TInt aValue )
    {
    /*
    0123456789
    ==========
    1234567890

    1,234,567,890

    len = 10;
    */

	TMemSpySizeText buf;
    buf.Num( aValue );
    TInt index = buf.Length() - 3;
    while( index > 0 )
        {
        buf.Insert( index, _L(",") );
        index -= 3;
        }

    return buf;
    }


EXPORT_C TBool MemSpyEngineUtils::IsRomAddress( TAny* aAddress )
    {
    TBool inRom = EFalse;
    //
#ifdef __EPOC32__
    const TInt err = User::IsRomAddress( inRom, aAddress );
    if  ( err != KErrNone )
        {
        inRom = EFalse;
        }
#else
    (void) aAddress;
    inRom = ETrue;
#endif
    //
    return inRom;
    }


EXPORT_C void MemSpyEngineUtils::GetCapabilityName( TDes& aBuf, TCapability aCapability )
    {
    const TPtrC8 pName( (const TUint8*) CapabilityNames[ aCapability ] );
    aBuf.Copy( pName );
    }


EXPORT_C TBool MemSpyEngineUtils::StripText( TDes& aText, const TDesC& aStrip )
    {
    TBool stripped = EFalse;
    const TInt stripTextLength = aStrip.Length();
    //
    if  ( aText.Length() > stripTextLength )
        {
        const TPtrC leftText( aText.Left( stripTextLength ) );
        if  ( leftText.CompareF( aStrip ) == 0)
            {
            // Try to find the first double colon
            const TInt doubleColonPos = aText.Find( KMemSpyEngineDoubleColon );
            if  ( doubleColonPos >= stripTextLength )
                {
                aText.Delete( 0, doubleColonPos + 2 );
                }
            else
                {
                aText.Delete( 0, stripTextLength );
                }

            stripped = ETrue;
            }
        }
    //
    return stripped;
    }


EXPORT_C void MemSpyEngineUtils::TextBeforeDoubleColon( TDes& aText )
    {
    const TInt doubleColonPos = aText.Find( KMemSpyEngineDoubleColon );
    if  ( doubleColonPos >= 0 )
        {
        aText.SetLength( doubleColonPos );
        }
    }


EXPORT_C void MemSpyEngineUtils::TextAfterDoubleColon( TDes& aText )
    {
    const TInt doubleColonPos = aText.Find( KMemSpyEngineDoubleColon );
    if  ( doubleColonPos >= 0 )
        {
        aText.Delete( 0, doubleColonPos + 2 );
        }
    }


TPtrC MemSpyEngineUtils::TextAfterLastDoubleColon( const TDesC& aText )
    {
    TPtrC ret( aText );

    // ABCD::123
    const TInt doubleColonPos = aText.LocateReverseF( ':' );
    if  ( doubleColonPos > 0 )
        {
        if ( aText[ doubleColonPos ] == ':' )
            {
            ret.Set( aText.Mid( doubleColonPos + 1 ) );
            }
        }

    return ret;
    }


EXPORT_C TMemSpyPercentText MemSpyEngineUtils::FormatPercentage( TReal aOneHundredPercentValue, TReal aValue )
    {
    const TReal value = (( aValue / aOneHundredPercentValue) * 100.0);
    
    _LIT(KPercentFormat, "%3.2f %%");

    TMemSpyPercentText val;
    val.Format( KPercentFormat, value );
    
    return val;
    }


EXPORT_C HBufC* MemSpyEngineUtils::CleanupTextLC( const TDesC& aText )
    {
    _LIT( KMemSpyTabChar, "\t" );
    _LIT( KMemSpyTabReplacementChar, " " );
    _LIT( KMemSpyNewLineChar, "\n" );
    _LIT( KMemSpyNewLineReplacementChar, " " );

    TInt pos = KErrNotFound;

    // Create replacement
    const TInt originalLength = aText.Length();
    HBufC* text = HBufC::NewLC( originalLength );
    TPtr pText( text->Des() );
    pText.Copy( aText );

    // Replace tabs
    pos = pText.Find( KMemSpyTabChar );
    while( pos >= 0 )
        {
        pText.Replace( pos, KMemSpyTabChar().Length(), KMemSpyTabReplacementChar );
        pos = pText.Find( KMemSpyTabChar );
        }

    // Replace tabs
    pos = pText.Find( KMemSpyNewLineChar );
    while( pos >= 0 )
        {
        pText.Replace( pos, KMemSpyNewLineChar().Length(), KMemSpyNewLineReplacementChar );
        pos = pText.Find( KMemSpyNewLineChar );
        }

    __ASSERT_ALWAYS( pText.Length() == aText.Length(), User::Invariant() );
    return text;
    }


EXPORT_C void MemSpyEngineUtils::GetRomInfoL( TDes& aPlatform, TDes& aChecksum )
    {
    aPlatform.Zero();
    aChecksum.Zero();

#ifdef __EPOC32__
    // Get checksum 
    TRomHeader* romHeader = (TRomHeader*) UserSvr::RomHeaderAddress();
    if  ( romHeader )
        {
        aChecksum.Format( _L("0x%08x"), romHeader->iCheckSum );
        }
#endif

    // Platform version
    _LIT( KS60VersionDllName, "SysUtil.dll" );
    RLibrary lib;
    if  ( lib.Load( KS60VersionDllName ) == KErrNone )
        {
        // Get exported version function
#ifdef __EPOC32__
        const TInt KSysUtilOrdinal = 9;
#else
        const TInt KSysUtilOrdinal = 6;
#endif
        TLibraryFunction fn = lib.Lookup( KSysUtilOrdinal );
        if ( fn != NULL )
            {
            TSysUtilGetSWVersionFunction sysUtilGetSWVersion = (TSysUtilGetSWVersionFunction) fn;
            TInt err = (*sysUtilGetSWVersion)( aPlatform );
            err = err;
#ifdef _DEBUG
            RDebug::Printf( "MemSpyEngineUtils::GetRomInfoL() - SysUtil::GetSWVersion() returned: %d", err );
#endif
            }

        lib.Close();
        }
    }


EXPORT_C void MemSpyEngineUtils::GetFolderL( RFs& aFsSession, TDes& aFolder, const CMemSpyEngineSinkMetaData& aMetaData, const TDriveNumber* aForceDrive )
    {
    const TChar KMemSpyDirectorySeparator = '\\';
    const TChar KMemSpyDriveSeparator = ':';

    TDriveList drives;
    User::LeaveIfError( aFsSession.DriveList( drives ) );
    
    // We prefer to log to MMC if its available. If not, we'll log to C: instead. On
    // WINS we prefer to log to C: because its easier to find via Windows Explorer.
    TDriveNumber logDrive = EDriveC;
    if  ( aForceDrive )
        {
        logDrive = *aForceDrive;
        TRACE( RDebug::Printf( "MemSpyEngineUtils::GetFolderL() - FORCED TO DRIVE: %c:\\", *aForceDrive + 'A' ) );
        }
    else
        {
		// check if drive is specified in root path
		if ( aMetaData.Root().Length() > 2 && aMetaData.Root()[1] == KMemSpyDriveSeparator )
			{
			TChar drive = aMetaData.Root()[0];
			
			// check if drive is valid
			if (drives.Locate(drive) != KErrNone)
				{
				TDriveUnit driveUnit( aMetaData.Root().Left(1) );
				logDrive = static_cast<TDriveNumber>(static_cast<TInt>(driveUnit));
				}
			else
				{
				logDrive = MemSpyEngineUtils::LocateSuitableDrive( aFsSession );
				}
			}
		else
			{
			logDrive = MemSpyEngineUtils::LocateSuitableDrive( aFsSession );
			}
        }

    // Prepare the drive buffer
    HBufC* fileName = HBufC::NewLC( KMaxFileName * 2 );
    TPtr pFileName( fileName->Des() );

    // Prepare the drive name
    TDriveUnit driveUnit( logDrive );
    pFileName.Append( driveUnit.Name() );
    
    if ( aMetaData.Root().Length() == 0 )
    	{
		pFileName.Append( KMemSpyLogRootPath );
    	}
    else
    	{
		TPtrC root( aMetaData.Root() );
		// check if root path contains drive (e.g. c:) and remove it
		if ( root.Length() > 2 && root[1] == KMemSpyDriveSeparator )
			{
			root.Set( root.Mid( 2 ) );
			}
		// check if root starts with \ and remove it
		if ( root.Length() > 1 && root[0] == KMemSpyDirectorySeparator )
			{
			root.Set( root.Mid( 1 ) );
			}
		
		// append root path
		pFileName.Append( KMemSpyDirectorySeparator );
		pFileName.Append( root );
		
		// add trailing slash if necessary
		if ( root[root.Length() - 1] != KMemSpyDirectorySeparator )
			{
			pFileName.Append( KMemSpyDirectorySeparator );
			}
    	}

    // Add any custom folder information
    if  ( aMetaData.Folder().Length() > 0 )
        {
        pFileName.Append( aMetaData.Folder() );
        TRACE( RDebug::Print( _L("MemSpyEngineUtils::GetFolderL() - client folder: %S" ), &pFileName ) );

        TChar lastChar = pFileName[ pFileName.Length() - 1 ];

        // Take into account any "group" timestamp appendix
        if  ( aMetaData.FolderTimeStamp().Length() )
            {
            if  ( lastChar != KMemSpyDirectorySeparator )
                {
                // Doesn't end with a backslash, so we must
                // add separator info before the timestamp
                pFileName.Append( ' ' );
                pFileName.Append( '-' );
                pFileName.Append( ' ' );
                }

            pFileName.Append( aMetaData.FolderTimeStamp() );
            TRACE( RDebug::Print( _L("MemSpyEngineUtils::GetFolderL() - timestamp folder: %S" ), &pFileName ) );
            }

        // Ensure post-fixed by '\\' character
        lastChar = ( pFileName[ pFileName.Length() - 1 ] );
        if  ( lastChar != KMemSpyDirectorySeparator )
            {
            pFileName.Append( KMemSpyDirectorySeparator );
            }
        }

    // Generate the timestamp file name
    if  ( aMetaData.UseFileTimeStamp() )
        {
        TTime now;
        now.HomeTime();
        const TDateTime dateTime( now.DateTime() );
        pFileName.AppendFormat( KMemSpyLogFileNameWithTimeStamp, dateTime.Year(), dateTime.Month() + 1, dateTime.Day() + 1,
                                                                 dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond() );
        TRACE( RDebug::Print( _L("MemSpyEngineUtils::GetFolderL() - timestamp file: %S" ), &pFileName ) );
        }
        
    // Do we have some context information? If so, make sure its printable
    HBufC* cleanedContext = NULL;
    if  ( aMetaData.Context().Length() )
        {
        cleanedContext = CleanContextInfoLC( aMetaData.Context() );
        }
    else
        {
        // This must be the standard log then...
        cleanedContext = KMemSpyLogDefaultName().AllocLC();
        }

    TRACE( RDebug::Print( _L("MemSpyEngineUtils::GetFolderL() - cleaned context: %S" ), cleanedContext ) );

    // Build final part of file name
    pFileName.Append( *cleanedContext );
    CleanupStack::PopAndDestroy( cleanedContext );

    // and finally, add the extension
    if  ( aMetaData.Extension().Length() )
        {
        pFileName.Append( aMetaData.Extension() );
        }
    else
        {
        pFileName.Append( KMemSpyLogDefaultExtension );
        }

    TRACE( RDebug::Print( _L("MemSpyEngineUtils::GetFolderL() - END - fileName: %S"), fileName ) );
    if  ( pFileName.Length() > aFolder.MaxLength() )
        {
        User::Leave( KErrOverflow );
        }

    aFolder.Copy( pFileName );
    CleanupStack::PopAndDestroy( fileName );
    }


TDriveNumber MemSpyEngineUtils::LocateSuitableDrive( RFs& aFsSession )
    {
#ifndef __WINS__
    TDriveInfo driveInfo;
    TDriveList drives;
    if  ( aFsSession.DriveList( drives ) == KErrNone )
        {
        for( TInt i=EDriveY; i>=EDriveD; i-- )
            {
            const TDriveNumber drive = static_cast< TDriveNumber >( i );
            TRACE( RDebug::Print( _L("MemSpyEngineUtils::LocateSuitableDrive() - drive: %c:\\, available: %d"), drive + 'A', drives[ drive ] ) );

            // Is drive available from an OS perspective?
            if  ( drives[ drive ] != 0 )
                {
                // Check whether there is a disk present or not.
                const TInt driveInfoErr = aFsSession.Drive( driveInfo, drive );
                TRACE( RDebug::Print( _L("MemSpyEngineUtils::LocateSuitableDrive() - drive: %c:\\, driveInfoErr: %d"), drive + 'A', driveInfoErr ) );
                if  ( driveInfoErr == KErrNone )
                    {
                    // Check if the drive is removable. We'll try to 
                    // save data here in preference to the system drive if at all
                    // possible...
                    const TBool removable = ( driveInfo.iDriveAtt & KDriveAttRemovable );
                    const TBool ram = ( driveInfo.iType == EMediaRam );
                    const TBool rom = ( driveInfo.iType == EMediaRom );
                    TRACE( RDebug::Printf( "MemSpyEngineUtils::LocateSuitableDrive() - drive: %c:\\, removable: %d, ram: %d, rom: %d", drive + 'A', removable, ram, rom ) );
                    //
                    if  ( removable && !( ram || rom ) )
                        {
                        // Check free space etc
                        TVolumeInfo volInfo;
                        const TInt volInfoErr = aFsSession.Volume( volInfo, drive );
                        TRACE( RDebug::Printf( "MemSpyEngineUtils::LocateSuitableDrive() - drive: %c:\\, volInfoErr: %d", drive + 'A', volInfoErr ) );
                        if  ( volInfoErr == KErrNone )
                            {
                            TRACE( RDebug::Printf( "MemSpyEngineUtils::LocateSuitableDrive() - END - using drive: %c:\\", drive + 'A', removable ) );
                            return drive;
                            }
                        }
                    }
                }
            }

        }
#else
	(void) aFsSession;
#endif

    // Don't use RFs::GetSystemDrive() as it isn't available on v9.1
    TRACE( RDebug::Printf( "MemSpyEngineUtils::LocateSuitableDrive() - END - fallback to EDriveC" ) );
    return EDriveC;
    }


void MemSpyEngineUtils::FormatTimeNowL( TDes& aBuf, TBool aLocalTime )
    {
    TTime time;
    if  ( aLocalTime )
        {
        time.HomeTime();
        }
    else
        {
        time.UniversalTime();
        }

    FormatTimeL( aBuf, time, aLocalTime );
    }


HBufC* MemSpyEngineUtils::DataStreamFolderNameWithTimeStampLC( const TDesC& aFolderName )
    {
    TMemSpyTimeText time;
    FormatTimeNowL( time, ETrue );
    //
    HBufC* folder = HBufC::NewLC( aFolderName.Length() + time.Length() + 10 );
    TPtr pFolder( folder->Des() );
    //
    pFolder.Append( aFolderName );
    pFolder.Append( time );
    //
    return folder;
    }


void MemSpyEngineUtils::GetKernelHeapThreadName( TDes& aBuf, TBool aFullName )
    {
    if  ( !aFullName )
        {
        aBuf.Copy( KMemSpyKernelProcessName );
        }
    else
        {
        aBuf.Copy( KMemSpyKernelProcessFullName );
        }

    aBuf.Append( KMemSpyEngineDoubleColon );
    aBuf.Append( KMemSpyKernelThreadName );
    }


void MemSpyEngineUtils::GetKernelHeapThreadAndProcessNames( TDes& aThreadName, TDes& aProcessName )
    {
    aThreadName.Copy( KMemSpyKernelThreadName );
    aProcessName.Copy( KMemSpyKernelProcessName );
    }


TUint32 MemSpyEngineUtils::Hash( const TDesC& aText )
    {
    // DJB Hash Function
    TUint32 hash = 5381;
    //
    const TInt length = aText.Length();
    for( TInt i = 0; i < length; i++)
        {
        hash = ((hash << 5) + hash) + aText[ i ];
        }
    //
    return hash;
    }


void MemSpyEngineUtils::Panic( TMemSpyEnginePanic aPanic )
    {
    _LIT( KMemSpyEnginePanicCategory, "MemSpyEngine" );
    User::Panic( KMemSpyEnginePanicCategory, aPanic );
    }


TProcessId MemSpyEngineUtils::IdentifyFileServerProcessIdL()
    {
    TRACE( RDebug::Printf( "MemSpyEngineUtils::IdentifyFileServerProcessIdL() - START" ) );

    TProcessId ret = KNullProcessId;

    _LIT(KFindMask, "efile*");
    TFullName name( KFindMask );
    TFindProcess finder( name );

    if  ( finder.Next( name ) == KErrNone )
        {
        RProcess process;
        const TInt error = process.Open( name );
        TRACE( RDebug::Print( _L("MemSpyEngineUtils::IdentifyFileServerProcessIdL() - process open err: %d, name: %S"), error, &name ) );
        if  ( error == KErrNone )
            {
            ret = process.Id();
            }

        process.Close();
        }

    TRACE( RDebug::Printf( "MemSpyEngineUtils::IdentifyFileServerProcessIdL() - almost done - ret: %d", (TUint) ret ) );

    if  ( static_cast< TUint >( ret ) == KNullProcessId )
        {
        User::Leave( KErrNotFound );
        }

    TRACE( RDebug::Printf( "MemSpyEngineUtils::IdentifyFileServerProcessIdL() - END - ret: %d", (TUint) ret ) );
    return ret;
    }


HBufC* MemSpyEngineUtils::CleanContextInfoLC( const TDesC& aContext )
    {
    TRACE( RDebug::Print( _L("MemSpyEngineUtils::CleanContextInfoLC() - START - %S"), &aContext ) );
    TFileName fileName;

    TBool seenDoubleColon = EFalse;
    const TInt length = aContext.Length();
    for( TInt i=0; i<length; i++ )
        {
        const TChar c( aContext[ i ] );
        const TBool haveNextChar = ( i+1 < length );
        //
        //TRACE( RDebug::Print( _L("MemSpyEngineUtils::CleanContextInfoLC() - c[%03d]: \'%c\', haveNextChar: %d, seenDoubleColon: %d"), i, (TUint32) c, haveNextChar, seenDoubleColon ) );
        //
        if  ( c == ':' && haveNextChar && aContext[ i + 1 ] == ':' )
            {
            // Skip double colon
            i++;
            fileName.Append( '-' );
            seenDoubleColon = ETrue;
            }
        else if ( c == '.' )
            {
            if  ( seenDoubleColon )
                {
                break;
                }
            else
                {
                fileName.Append( '-' );
                }
            }
        else
            {
            fileName.Append( c );
            }
        }

    TRACE( RDebug::Print( _L("MemSpyEngineUtils::CleanContextInfoLC() - END - %S"), &fileName ) );
    return fileName.AllocLC();
    }


