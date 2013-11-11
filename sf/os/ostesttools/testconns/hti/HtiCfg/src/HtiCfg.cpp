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
* Description:  CHtiCfg implementation
*
*/

// INCLUDE FILES
#include "HtiCfg.h"
#include <badesca.h>
#include <f32file.h>

// CONSTANTS
const static TInt KCfgArrayGranularity = 5;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiCfg::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CHtiCfg* CHtiCfg::NewL()
    {
    CHtiCfg* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHtiCfg::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CHtiCfg* CHtiCfg::NewLC()
    {
    CHtiCfg* self = new ( ELeave ) CHtiCfg();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CHtiCfg::CHtiCfg
// Constructor
// -----------------------------------------------------------------------------
CHtiCfg::CHtiCfg():iCfgParameters( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CHtiCfg::CHtiCfg
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C CHtiCfg::~CHtiCfg()
    {
    if ( iCfgParameters )
        {
        iCfgParameters->Reset();
        delete iCfgParameters;
        iCfgParameters = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CHtiCfg::ConstructL
// Second phase constructor. Private.
// -----------------------------------------------------------------------------
void CHtiCfg::ConstructL()
    {
    iCfgParameters = new ( ELeave ) CDesC8ArrayFlat( KCfgArrayGranularity );
    }

// -----------------------------------------------------------------------------
// CHtiCfg::LoadCfgL
// Searches the file in all drives and if found reads all parameter lines to the
// iCfgParameters array.
// -----------------------------------------------------------------------------
EXPORT_C void CHtiCfg::LoadCfgL( const TDesC& aCfgFilePath,
                                 const TDesC& aCfgFileName )
    {
    // Reset parameters
    if ( iCfgParameters )
        {
        iCfgParameters->Reset();
        delete iCfgParameters;
        iCfgParameters = NULL;
        }
    iCfgParameters = new ( ELeave ) CDesC8ArrayFlat( KCfgArrayGranularity );

    // Open & read file
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );

    TFindFile finder( fsSession );
    TFileName path( aCfgFilePath );
    TInt err = finder.FindByPath( aCfgFileName, &path );
    if ( err != KErrNone )
        {
        User::Leave( err );
        }

    TFileName cfgFile = finder.File();

    RFile file;
    User::LeaveIfError( file.Open( fsSession, cfgFile, EFileRead ) );
    CleanupClosePushL( file );

    TInt fileLength;
    User::LeaveIfError( file.Size( fileLength ) );

    HBufC8* fileData = HBufC8::NewLC( fileLength );
    TPtr8 fileDes = fileData->Des();
    User::LeaveIfError( file.Read( fileDes ) );

    // Get parameters
    TBool eof = EFalse;
    while ( !eof )
        {
        TInt strEndIndex = fileDes.Locate( KCfgNewLine );
        if ( strEndIndex == KErrNotFound )
            {
                strEndIndex = fileDes.Length();
                eof = ETrue;
            }

        TPtrC8 line = fileDes.Left( strEndIndex );

        if ( line.Locate( KCfgComment ) != 0 )
            {
            if ( line.Locate( KCfgSeparator ) > 0 )
                {
                TBuf8<KMaxParameterLength> parameter;
                parameter.Copy( line );
                parameter.Trim();
                iCfgParameters->AppendL( parameter );
                }
            }

        if ( !eof )
            fileDes = fileDes.Right( fileDes.Length() - ( strEndIndex + 1 ) );

        }

    CleanupStack::PopAndDestroy( 3 ); // fsSession, fileData, file
    }

// -----------------------------------------------------------------------------
// CHtiCfg::SaveCfgL
// Writes all parameter lines from iCfgParameters array to the file.
// Existing file is searched from all drives and if found file will be replaced.
// If existing file is not found file will be created to c-drive.
// -----------------------------------------------------------------------------
EXPORT_C void CHtiCfg::SaveCfgL( const TDesC& aCfgFilePath,
                                 const TDesC& aCfgFileName )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    RFile file;
    CleanupClosePushL( file );

    // Find the file
    TFindFile finder( fs );
    TFileName path( aCfgFilePath );
    TFileName cfgFile;
    TInt err = finder.FindByPath( aCfgFileName, &path );
    if ( err != KErrNone )
        {
        cfgFile.Append( _L ( "c:" ) );
        cfgFile.Append( path );
        cfgFile.Append( aCfgFileName );
        }
    else
        {
        cfgFile = finder.File();
        // If the file was found from ROM, we must save to C-drive.
        if ( cfgFile[0] == 'z' || cfgFile[0] == 'Z' )
            {
            cfgFile[0] = 'c';
            }
        }

    // Replace or create the file
    User::LeaveIfError( file.Replace( fs, cfgFile, EFileWrite | EFileShareAny ) );

    // Write the file
    for ( TInt i = 0; i < iCfgParameters->Count(); i++ )
        {
        file.Write( iCfgParameters->MdcaPoint( i ) );
        file.Write( _L8( "\n" ) );
        }

    // Close
    CleanupStack::PopAndDestroy( 2 ); // fs, file
    }

// -----------------------------------------------------------------------------
// CHtiCfg::SetParameterL
// -----------------------------------------------------------------------------
EXPORT_C TInt CHtiCfg::SetParameterL( const TDesC8& aName,
                                      const TDesC8& aValue )
    {
    // Does the parameter exist?
    for ( TInt i = 0; i < iCfgParameters->Count(); i++ )
        {
        TInt sepIndex =
            ( iCfgParameters->MdcaPoint( i ) ).Locate( KCfgSeparator );
        if ( sepIndex <= 0 )
            User::Leave( KErrGeneral ); // should not happen

        TPtrC8 name = ( iCfgParameters->MdcaPoint( i ) ).Left( sepIndex );
        if ( name.Compare( aName ) == 0 )
            {
            iCfgParameters->Delete( i );
            TBuf8<KMaxParameterLength> parameter;
            parameter.Append( aName );
            parameter.Append( KCfgSeparator );
            parameter.Append( aValue );
            iCfgParameters->AppendL( parameter );
            return KErrNone;
            }
        }

    // Apparently not.. add it
    TBuf8<KMaxParameterLength> parameter;
    parameter.Append( aName );
    parameter.Append( KCfgSeparator );
    parameter.Append( aValue );
    iCfgParameters->AppendL( parameter );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHtiCfg::RemoveParameterL
// -----------------------------------------------------------------------------
EXPORT_C TInt CHtiCfg::RemoveParameterL( const TDesC8& aName )
    {
    for ( TInt i = 0; i < iCfgParameters->Count(); i++ )
        {
        TInt sepIndex =
            ( iCfgParameters->MdcaPoint( i ) ).Locate( KCfgSeparator );
        if ( sepIndex <= 0 )
            User::Leave( KErrGeneral ); // should not happen

        TPtrC8 name = ( iCfgParameters->MdcaPoint( i ) ).Left( sepIndex );
        if ( name.Compare( aName ) == 0 )
            {
            iCfgParameters->Delete( i );
            return KErrNone;
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CHtiCfg::GetParameterL
// -----------------------------------------------------------------------------
EXPORT_C TPtrC8 CHtiCfg::GetParameterL( const TDesC8& aName )
    {
    for ( TInt i = 0; i < iCfgParameters->Count(); i++ )
        {
        TInt sepIndex =
            ( iCfgParameters->MdcaPoint( i ) ).Locate( KCfgSeparator );
        if ( sepIndex <= 0 )
            User::Leave( KErrGeneral ); // should not happen

        TPtrC8 name = ( iCfgParameters->MdcaPoint( i ) ).Left( sepIndex );
        if ( name.Compare( aName ) == 0 )
            {
            TPtrC8 value = ( iCfgParameters->MdcaPoint( i ) ).Right(
                ( iCfgParameters->MdcaPoint( i ) ).Length() - ( sepIndex + 1 ) );
            return value;
            }
        }
    User::Leave( KErrNotFound );
    return 0;
    }

// -----------------------------------------------------------------------------
// CHtiCfg::GetParameterIntL
// -----------------------------------------------------------------------------
EXPORT_C TInt CHtiCfg::GetParameterIntL( const TDesC8& aName )
    {
    for ( TInt i = 0; i < iCfgParameters->Count(); i++ )
        {
        TInt sepIndex =
            ( iCfgParameters->MdcaPoint( i ) ).Locate( KCfgSeparator );
        if ( sepIndex <= 0 )
            User::Leave( KErrGeneral ); // should not happen

        TPtrC8 name = ( iCfgParameters->MdcaPoint( i ) ).Left( sepIndex );
        if ( name.Compare( aName ) == 0 )
            {
            TPtrC8 value = ( iCfgParameters->MdcaPoint( i ) ).Right(
                ( iCfgParameters->MdcaPoint( i ) ).Length() - ( sepIndex + 1 ) );

            TLex8 lex( value );
            TInt result;
            User::LeaveIfError( lex.Val( result ) );
            return result;
            }
        }
    User::Leave( KErrNotFound );
    return 0;
    }


// End of file
