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
* Description:  HtiFileHlp implementation. This exe is used for file operations
*                to TCB folders (requiring capability ALL).
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <f32file.h>

// CONSTANTS
_LIT( KFileHlpName, "HtiFileHlp" );

_LIT( KCmdCopy,   "c" );
_LIT( KCmdDelete, "d" );
_LIT( KCmdMkd,    "m" );
_LIT( KCmdRmd,    "r" );
_LIT( KCmdMove,   "mv" );
_LIT( KCmdRename, "re" );
_LIT( KDelimiter, "|" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================

LOCAL_C void HandleCopyMoveRenameL( TDesC& aCmd,
		TDesC& aFromFile, TDesC& aToFile )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );

    if ( aCmd == KCmdCopy )
    	{
    	User::LeaveIfError( fileMan->Copy( aFromFile, aToFile,
    			CFileMan::ERecurse|CFileMan::EOverWrite ) );
    	}
    else if ( aCmd == KCmdMove )
    	{
    	User::LeaveIfError( fileMan->Move( aFromFile, aToFile,
    			CFileMan::ERecurse|CFileMan::EOverWrite ) );
    	}
    else
    	{
    	User::LeaveIfError( fileMan->Rename( aFromFile, aToFile ) );
    	}

    CleanupStack::PopAndDestroy();  // fileman
    CleanupStack::PopAndDestroy();  // fs
    }

LOCAL_C void HandleDeleteL( TDesC& aFilename )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    // check that filenames are valid
    if ( !fs.IsValidName( aFilename ) )
        {
        User::Leave( KErrBadName );
        }

    // delete file
    User::LeaveIfError( fs.Delete( aFilename ) );

    CleanupStack::PopAndDestroy();
    }

LOCAL_C void HandleMkdL( TDesC& aDirName )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    User::LeaveIfError( fs.MkDirAll( aDirName ) );

    CleanupStack::PopAndDestroy();
    }

LOCAL_C void HandleRmdL( TDesC& aDirName )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    CFileMan* fileMan = CFileMan::NewL( fs );
    CleanupStack::PushL( fileMan );

    User::LeaveIfError( fileMan->RmDir( aDirName ) );

    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    }

LOCAL_C TInt StartL()
    {
    TInt cmdLen = User::CommandLineLength();
    if ( cmdLen < 3 )
        {
        User::Leave( KErrArgument );
        }
    HBufC* cmdLine = HBufC::NewLC( cmdLen );
    TPtr ptCmdLine = cmdLine->Des();
    User::CommandLine( ptCmdLine );

    TInt paramStart = 0;
    TInt paramEnd = 0;

    // Take first parameter (the command)
    paramEnd = cmdLine->Find( KDelimiter );
    if ( paramEnd <= paramStart )
        {
        User::Leave( KErrArgument );
        }
    TPtrC cmd = cmdLine->Mid( paramStart, paramEnd - paramStart );

    // Take the next parameter either until next delimiter or
    // the rest of the command line.
    paramStart = paramEnd + 1;
    paramEnd = cmdLine->Mid( paramStart ).Find( KDelimiter ) + paramStart;
    if ( paramEnd < paramStart )
        {
        // No delimiter found - this is the last parameter
        paramEnd = cmdLen;
        }

    TPtrC param1 = cmdLine->Mid( paramStart, paramEnd - paramStart );

    paramStart = paramEnd + 1;

    if ( cmd == KCmdCopy || cmd == KCmdMove || cmd == KCmdRename )
        {
        // Copy, Move and Rename should have also the destination parameter
        // It is assumed to be the rest of the command line from previous delim
        if ( paramStart >= cmdLen )
            {
            User::Leave( KErrArgument );
            }
        TPtrC param2 = cmdLine->Mid( paramStart );
        HandleCopyMoveRenameL( cmd, param1, param2 );
        }
    else if ( cmd == KCmdDelete )
        {
        HandleDeleteL( param1 );
        }
    else if ( cmd == KCmdMkd )
        {
        HandleMkdL( param1 );
        }
    else if ( cmd == KCmdRmd )
        {
        HandleRmdL( param1 );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy(); // cmdLine

    return KErrNone;
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    CActiveScheduler::Install( scheduler );

    User::RenameThread( KFileHlpName );

    TRAPD( error, StartL() );

    delete scheduler;
    delete cleanup;
    __UHEAP_MARKEND;

    //__ASSERT_ALWAYS( !error, User::Panic( KFileHlpName, error ) );
    return error;
    }


// End of File
