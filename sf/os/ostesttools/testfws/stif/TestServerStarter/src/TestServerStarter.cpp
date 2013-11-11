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
* Description: This file contains TestServerStarter implementation.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/TestServerClient.h>
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  


// ================= OTHER EXPORTED FUNCTIONS =================================


/*
-------------------------------------------------------------------------------

    Class: -

    Method: E32Main

    Description: 

    Parameters: None

    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/

GLDEF_C TInt E32Main()
    {

    RDebug::Print(_L("STIF: New process starting"));

    // Get module name from command line
	const TInt length = User::CommandLineLength();

    HBufC* cmdLine = HBufC::New( length );
    
    if ( cmdLine == NULL )
        {
        return KErrNoMemory;
        }

    TPtr moduleName = cmdLine->Des();

	User::CommandLine( moduleName );

    RDebug::Print(_L("STIF: Received data [%S]"), &moduleName);

    // Extract semaphore name passed in data
    TInt index = moduleName.Find(_L(" "));
    RDebug::Print(_L("STIF: Space separator found at position [%d]"), index);
    TPtrC semaphoreName = moduleName.Mid(index + 1);
    moduleName = moduleName.Left(index);

    RDebug::Print(_L("STIF: Extracted module name [%S] and sempahore name [%S]"), &moduleName, &semaphoreName);

    // Open start-up synchronization semaphore
    RSemaphore startup;
    RDebug::Print(_L(" Openingstart-up semaphore"));
    //TName semaphoreName = _L("startupSemaphore");
    //semaphoreName.Append( moduleName );
    
    TInt res = startup.OpenGlobal(semaphoreName);
    RDebug::Print(_L("Opening result %d"), res);    


    TFileName serverName;
    TInt r = StartNewServer ( moduleName, serverName, EFalse, startup);    

    if ( r ==   KErrAlreadyExists )
        {        
        // Ok, server was already started
        RDebug::Print(_L("Server already started, signaling semaphore and existing"));
        startup.Signal();        

        delete cmdLine;

        return KErrNone;
        }
    else
        {       
        RDebug::Print(_L("Server is finished, code %d"), r);
        }

    delete cmdLine;
    
    return r;

    }

// End of File
