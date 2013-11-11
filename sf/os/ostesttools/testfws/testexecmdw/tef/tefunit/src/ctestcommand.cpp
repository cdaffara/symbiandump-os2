/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file CTestCommand.cpp
*/

#include "ctestcommand.h"
#include "crunner.h"

CTestCommand::CTestCommand(const TDes& aStepPath, CTestSuite* aSuite )
/**
 * Constructor
 *
 * @param aStepPath - Path of the test to execute
 * @param aSuite - Suite of tests
 */
	: iSuite(aSuite), iStepPath(aStepPath)
	{
	}

CTestCommand::~CTestCommand()
/**
 * Destructor
 */
	{
	}

TVerdict CTestCommand::doTestStepL()
/**
 * dotestStepL
 */
	{
	// Create a test config from this test step
	CTestConfig testConfig(this);
	
	// Search for the test/suite of tests and run it/them
	CRunner* runner = CRunner::NewL( iStepPath, testConfig,	Logger() );
	CleanupStack::PushL(runner);
	TRAPD( err, iSuite->AcceptL( *runner ) );

	if( err != KErrTEFUnitPass )
		{
		// Map the TEFVerdict onto a TVerdict for TestExecute
		switch(err)
			{
			case KErrTEFUnitFail:
				{
				SetTestStepResult(EFail);
				}
				break;
			case KErrTEFUnitInconclusive:
				{
				SetTestStepResult(EInconclusive);
				}
				break;
			case KErrTEFUnitTestSuiteError:
				{
				SetTestStepResult(ETestSuiteError);
				}
				break;
			case KErrTEFUnitAbort:
				{
				SetTestStepResult(EAbort);
				}
				break;
			case KErrTEFUnitIgnore:
				{
				SetTestStepResult(EIgnore);
				}
				break;
			default:
				{
				// Else, if another error code is contained in the leave,
				//  leave again
				User::Leave(err);
				}
				break;
			}
		}

	// Cleanup
	CleanupStack::PopAndDestroy(runner);

	return TestStepResult();
}
