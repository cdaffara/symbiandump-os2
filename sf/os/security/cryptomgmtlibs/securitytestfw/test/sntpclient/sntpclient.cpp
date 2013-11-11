/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <e32base.h>
#include <bacline.h>

#include "commandlineargs.h"
#include "sntpclientengine.h"
#include "util.h"

LOCAL_C void RunSNTPClientL();
LOCAL_C void ParseCommandLineL(TCommandLineArgs& aArgs);


GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(err, RunSNTPClientL()); 
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return err; // and return
    }
    

    
LOCAL_C void RunSNTPClientL()
	{
	TRAP_IGNORE(Util::SetAppropriateTimezoneL());
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	// Parse the command line
	
	TCommandLineArgs args;
	ParseCommandLineL(args);
	
	// Create the client object, and allow it to run to completion
	
	CSNTPClient* engine = CSNTPClient::NewLC(args);
	engine->Start();
	CActiveScheduler::Start();
	
	args.iServers.ResetAndDestroy();
	
	CActiveScheduler::Install(NULL);
	
	if (engine->State() == EStateFailed)
		{
		User::Leave(KErrGeneral);
		}
	else if (engine->State() == EStateAborted)
		{
		User::Leave(KErrTimedOut);
		}
		
	CleanupStack::PopAndDestroy(2, scheduler);
	
	}
	
LOCAL_C void ParseCommandLineL(TCommandLineArgs& aArgs)
	{
	
	/* Get and parse the command line arguments */
	
	TBool parsingOffset(EFalse);
	TBool enoughArguments(EFalse);
	
	aArgs.iOffset = 0;
	aArgs.iApplyDaylightSavings = EFalse;
	
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	TInt argCount = args->Count();
	
	/* Ignore the first argument, this is the executable name */
	
	for (TInt i = 1; i < argCount; ++i)
		{
		
		TPtrC arg = args->Arg(i);
		
		
		if (parsingOffset)
			{
			
			if (arg.Length() != 3)
				{
				User::Leave(KErrArgument);
				}
			
						
			TInt temp(((arg[1] - '0') * 10) + (arg[2] - '0'));
			
			if (arg[0] == '-')
				{
				aArgs.iOffset = -temp;
				}
			else
				{
				aArgs.iOffset = temp;
				}
			
			parsingOffset = EFalse;
			
			}
		else if (arg.CompareF(KCommandLineDaylightSavings) == 0)
			{
			aArgs.iApplyDaylightSavings = ETrue;
			}
		else if (arg.CompareF(KCommandLineOffset) == 0)
			{
			parsingOffset = ETrue;
			}
		else
			{
			// This argument must be the IP address/hostname
			// sanity check the name
			
			if (arg.Length() > 256)
				{
				User::Leave(KErrArgument);
				}
			
			HBufC* server = arg.AllocLC();
			User::LeaveIfError(aArgs.iServers.Append(server));
			CleanupStack::Pop(server);
			
			enoughArguments = ETrue;
			}
		
		}
		
	// Sanity check the arguments
	if ((!enoughArguments) || aArgs.iOffset < -12 || aArgs.iOffset > 13)
		{
		User::Leave(KErrArgument);
		}
		
	CleanupStack::PopAndDestroy(args);
	
	}
