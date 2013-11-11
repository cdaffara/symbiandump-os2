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
 @file texmlengine.cpp
 @internalTechnology
*/
#include "texml2teststep.h"
#include <xml/utils/xmlengxestd.h>

/**
 * Class CXmlEngineStep Implementation
 */

/**
 * Constructor. Sets the test step name so that it can be 
 * differentiated from within doTestStepL()
 */
CXmlEngineStep::CXmlEngineStep(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

/**
 * TEF invokes doTestStepL interface along with the step name
 * to let appropriate action to be taken for the test step.
 */
TVerdict CXmlEngineStep::doTestStepL(void)
	{
	if (TestStepName() == KXmlEnginePushAndPop)
		{
		INFO_PRINTF1(KXmlEnginePushAndPop);
		SetTestStepResult(EPass);
		TRAPD(err, TestKXmlEnginePushAndPopL());
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}
/**
 * Tests XmlEnginePushL, XmlEnginePopAndClose
 */
TVerdict CXmlEngineStep::TestKXmlEnginePushAndPopL()
	{
	XmlEnginePushL();
	XmlEnginePopAndClose();
	return EPass;
	}
