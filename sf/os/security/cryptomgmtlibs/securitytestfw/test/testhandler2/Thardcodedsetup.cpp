/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tHardcodedTests.h"
#include "Thardcodedsetup.h"

#include "t_input.h"
#include "t_certstoreactionmemfail.h"
#include "tcancel.h"
#include "t_message.h"
#include "t_output.h"
#include "tTestSpec.h"
#include "t_testactionspec.h"
#include "t_logger.h"


EXPORT_C CHardcodedSetup* CHardcodedSetup::NewLC(void)
	{
	CHardcodedSetup* self = new(ELeave) CHardcodedSetup();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CHardcodedSetup::SetupTestsL(RFs& aFs,
										   CTestSpec& aTestSpec, 
										   THardcodedTests theHardcodedTests[], 
										   const CTestHandlerSettings& aCommandLineSettings)
	{
	TInt err = KErrNone;
	for (TInt i = 0; theHardcodedTests[i].action!=NULL; i++)
		{
		
		TInt groupings= theHardcodedTests[i].groupings;
		TTestActionSpec actionSpec;
		actionSpec.HardcodedInit(theHardcodedTests[i].name);
		//Start off checking Exhaustive and Smoke flags, then calls groupings
		if (!CheckAllFlags(aCommandLineSettings, groupings))
			//The current test should not be run
			continue;


		CTestAction* newAction = 0;
		TRAP(err, 
			newAction = theHardcodedTests[i].action(aFs, *iConsole, *iLogFile, actionSpec));
			
		if (err != KErrNone)
			{
			// We warn the user that an error occured
			// IMPROVEMENT : we should give more details because this won't be enough
			PRINTANDLOG(_L("void CHardcoded::SetupTestsL(RPointerArray<CTestAction>& aTestActions)\n"));
			PRINTANDLOG1(_L("An error occurred while creating action %D\n"), i);
			User::Leave(err);
			}
		CleanupStack::PushL(newAction);
		User::LeaveIfError(aTestSpec.AddNextTest(newAction));
		CleanupStack::Pop(newAction);
		}
	}


CHardcodedSetup::~CHardcodedSetup()
	{
	}

CHardcodedSetup::CHardcodedSetup(void)
: CTestSetup(NULL)
	{
	}

EXPORT_C TBool CHardcodedSetup::InitialiseL(RFs &aFs, const TDesC& aDefaultLog)
	{
	TRAPD(err, OpenLogFileL(aFs, 0, aDefaultLog));

	if (err != KErrNone)
		{
		// We warn the user that an error occured
		// IMPROVEMENT : we should give more details because this won't be enough
		PRINTANDLOG1(_L("void CHardcodedSetup::InitialiseL: Failed to open log file: %D\n"), err);
		User::Leave(err);
		}	
	
	return(ETrue);
	}



