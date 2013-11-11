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

#ifndef MEMSPYCOMMANDLINE_H
#define MEMSPYCOMMANDLINE_H

// System includes
#include <e32base.h>
#include <f32file.h>
#include <badesca.h>
#include <e32cons.h>

#include "MemSpyCommands.h"

#ifdef _DEBUG
#   define TRACE( x ) x
#else
#   define TRACE( x )
#endif

// Constants
const TInt KMemSpyCommandLineMaxLength = 128;

// Classes referenced
class RFs;
class CCommandLineArguments;
class RMemSpyEngineClientInterface;
class CConsoleBase;
class RMemSpySession;

class CMemSpyCommandLine :  public CActive
    {
public:
    //static CMemSpyCommandLine* NewLC();
	static CMemSpyCommandLine* NewLC( CConsoleBase& aConsole );
    ~CMemSpyCommandLine();

private:
    //CMemSpyCommandLine();
    CMemSpyCommandLine( CConsoleBase& aConsole );
    void ConstructL();

public: // API
    //void PerformBatchL( const TDesC& aFileName ); 	//support of the batch files removed 
    void PerformOpL( const CCommandLineArguments& aCommandLine );
    void PerformSingleOpL( const TDesC& aCommand, const CDesCArray& aParameters );
    //
    //AO request method
    void WaitForInput();

private: // Internal methods
    void ConnectToMemSpyL();
    void LaunchMemSpyL();
    CDesCArray* ReadLinesL( RFile& aFile );
    void ParseSWMTParametersL( const CDesCArray& aParameters, TInt& aCategories, TDes& aFilter );
    TInt FindBatchFile( TDes &aFileName );
    TInt FindFile( TDes &aFileName, const TDesC &aDirPath );

private: // Console write methods
    void RedrawInputPrompt();
    void RedrawStatusMessage();
    void RedrawStatusMessage( const TDesC& aMessage );
    void ProcessCommandBufferL();
    void RunL(); // from CActive
    TInt RunError( TInt aError );
    void DoCancel();
    
private: // Data members
    RFs iFsSession;
    RMemSpyEngineClientInterface* iMemSpy;
    RMemSpySession* iMemSpySession;
    TBool iIsBatch; // For avoiding recursion
    
private: // Data members - console - write status messages
    CConsoleBase& iConsole;
    TPoint iCommandPromptPos;
    TPoint iStatusMessagePos;
    TBuf<KMemSpyMaxInputBufferLength> iCommandBuffer;
    };


#endif
