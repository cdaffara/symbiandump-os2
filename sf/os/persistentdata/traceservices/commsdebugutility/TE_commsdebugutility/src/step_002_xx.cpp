// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains Flogger Unit Test Case 002.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_002_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest002_01
* Input parameters	: None
* Output parameters	: None
* Description		: This is the constructor
*/


CFloggerTest002_01::CFloggerTest002_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_002_01"));
	}


/**
* Function  Name	:~ CFloggerTest002_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest002_01::~CFloggerTest002_01()
	{	
			
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 002_01 has 
*					  passed or failed
*/


TVerdict CFloggerTest002_01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function connects to the file logger
						  and return the result
*/


TInt CFloggerTest002_01::executeStepL( )
	{
	INFO_PRINTF1(_L("Step 002.01 called "));

	RFileLogger theFlogger;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	TInt result = theFlogger.Connect();
	CleanupClosePushL(theFlogger);
	theFlogger.SetLogTags(ptrSubSystem, ptrComponent);
	CleanupStack::PopAndDestroy();	//logger
	return result;
	}
TInt CFloggerTest002_01::executeStepL(TBool)
	{
	return KErrGeneral;
	}


/*
* Function  Name	: CFloggerTest002_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest002_02::CFloggerTest002_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_002_02"));
	}


/**
* Function  Name	:~ CFloggerTest002_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/

CFloggerTest002_02::~CFloggerTest002_02()
	{
	}



/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest002_02::doTestStepL( )
	{
	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("Step 002.02 called "));

	CFloggerTest002_01* step002_01 = new CFloggerTest002_01;
	CleanupStack::PushL(step002_01);
	doTestStepWithHeapFailureL( *step002_01, 1, 10, KErrNone);

	if ( step002_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);


	CleanupStack::PopAndDestroy(step002_01);
	return TestStepResult();
	}

