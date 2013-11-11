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
// This contains Flogger Unit Test Case 019.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_019_01.h"


#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest019_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest019_01::CFloggerTest019_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_01"));
	}


/**
* Function  Name	:~ CFloggerTest019_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest019_01::~CFloggerTest019_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 019_01 has 
* 					  passed or failed

*/

TVerdict CFloggerTest019_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.01 called "));
	
	RFileLogger theFlogger;
	TRAPD(res, theFlogger.SetDateAndTime(ETrue, ETrue))

	if (res == KErrNone)
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

		
	return TestStepResult();
	}

