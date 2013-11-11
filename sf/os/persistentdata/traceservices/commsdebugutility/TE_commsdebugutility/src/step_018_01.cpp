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
// This contains CommDb Unit Test Case 018.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_018_01.h"


#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest018_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest018_01::CFloggerTest018_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_01"));
	}


/**
* Function  Name	:~ CFloggerTest018_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest018_01::~CFloggerTest018_01()
	{
	}

/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 018_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest018_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.01 called "));
	
	RFileLogger theFlogger;
	if ( theFlogger.LastError() == KErrNone)
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	return TestStepResult();
	}

