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
// This contains Flogger Unit Test Case 001.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_001_01.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest001_01
* Input parameters	: None
* Output parameters	: None
* Description		: This is the constructor
*/


CFloggerTest001_01::CFloggerTest001_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_001_01"));
	}


/**
* Function  Name	:~ CFloggerTest001_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest001_01::~CFloggerTest001_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 001_01 has 
*						passed or failed

*/


TVerdict CFloggerTest001_01::doTestStepL( )
	{

	INFO_PRINTF1(_L("Step 001.01 called "));
	RFileLogger theFlogger;
	TVersion LogVersion = theFlogger.Version();
	if ( LogVersion.iMinor == 0 && LogVersion.iMajor == 2 && LogVersion.iBuild == 0 ) // Check the version
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);

	// we haven't opened connection, but calling Close should still work and not
	// cause a panic since it should just be ignored.
	theFlogger.Close();

	return TestStepResult();
	}

