// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file texml2teststep.cpp
 @internalTechnology
*/
#include "texml2teststep.h"
#include "libxml2_globals_private.h"
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengutils.h>

/**
 * Class CXmlEngMemStep Implementation
 */

/**
 * Constructor. Sets the test step name so that it can be 
 * differentiated from within doTestStepL()
 */
CXmlEngMemStep::CXmlEngMemStep(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

/**
 * TEF invokes doTestStepL interface along with the step name
 * to let appropriate action to be taken for the test step.
 */
TVerdict CXmlEngMemStep::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngLeaveOOMStep1)
		{
		INFO_PRINTF1(KXmlEngLeaveOOMStep1);
		SetTestStepResult(TestKXmlEngLeaveOOMStep1());
		}
	else if (TestStepName() == KXmlEngLeaveOOMStep2)
		{
		INFO_PRINTF1(KXmlEngLeaveOOMStep2);
		SetTestStepResult(TestKXmlEngLeaveOOMStep2());
		}
	return TestStepResult();
	}
/**
 * Tests the API XmlEngOOMTestL without any OOM condition.
 */
TVerdict CXmlEngMemStep::TestKXmlEngLeaveOOMStep1()
	{
	TInt err = KErrNone;
	//Clean the global state pointer before calling XmlEngLeaveOOML()
	xmlGlobalStatePtr gsPtr = xmlCreateAndInitializeGlobalState();
	if (!gsPtr)
		{
		INFO_PRINTF1(_L("Warning: Global state pointer is NULL\n"));
		}
	//reset the OOM flag
	xmlResetOOM();
	//invoke the API and check for the error status returned
	TRAP(err, XmlEngOOMTestL());
	//should pass without any errors
	if (err != KErrNone)
		{
		return EFail;
		}
	return EPass;
	}
/**
 * Tests the API XmlEngOOMTestL without OOM condition.
 */
TVerdict CXmlEngMemStep::TestKXmlEngLeaveOOMStep2()
	{
	TInt err = KErrNone;
	//Set the OOM Flag so that the alternate path in XmlEngOOMTestL()
	//is taken
	xmlGlobalStatePtr gsPtr = xmlCreateAndInitializeGlobalState();
	if (!gsPtr)
		{
		INFO_PRINTF1(_L("Warning: Global state pointer is NULL\n"));
		}
	//set the OOM Flag
	xmlSetOOM();
	//invoke the API
	TRAP(err, XmlEngOOMTestL());
	//Test case done, reset the flag.
	xmlResetOOM();
	//check for the error status returned; should have failed with 
	//KErrNoMemory
	if (err != KErrNoMemory)
		{
		return EFail;
		}
	return EPass;
	}

