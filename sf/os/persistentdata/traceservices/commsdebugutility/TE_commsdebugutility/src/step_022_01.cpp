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
// This contains Flogger Unit Test Case 022.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_022_01.h"


#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest022_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest022_01::CFloggerTest022_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_01"));
	}


/**
* Function  Name	:~ CFloggerTest022_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest022_01::~CFloggerTest022_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 022_01 has 
* 					  passed or failed

*/

TVerdict CFloggerTest022_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.01 called "));

	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem")); //Test system name
	ptrComponent.Set(_L8("Component")); //Test component name
	
	RFileLogger theFlogger;
	theFlogger.Connect(); //Connect to the file server
	theFlogger.SetLogTags(ptrSubSystem, ptrComponent); //To set the tags 
	TRAPD(res, theFlogger.Close()); //Close the connection 		

	if (res == KErrNone)
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

