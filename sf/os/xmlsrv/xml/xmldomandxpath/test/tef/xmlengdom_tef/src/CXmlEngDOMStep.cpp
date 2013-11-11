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
 @file CXmlEngDOMStep.cpp
 @internalTechnology
*/

#include "CXmlEngDOMStep.h"
#include <xml/dom/xmlengdomimplementation.h>


/**
 * Class CXmlEngDOMStep Implementation
 */
CXmlEngDOMStep::CXmlEngDOMStep(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngDOMStep::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngLeaveOOMStep1)
		{
		INFO_PRINTF1(KXmlEngLeaveOOMStep1);
		SetTestStepResult(TestKXmlEngLeaveOOMStep1());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngDOMStep::TestKXmlEngLeaveOOMStep1()
	{
	//Open 
	RHeap* rHeap = NULL;
	RXmlEngDOMImplementation rxmlDomObject;
	
	TInt err = KErrNone; 
	 
	TRAP(err, rxmlDomObject.OpenL(rHeap));
	
	if (KErrNone != err)
	{
		return EFail;
	}
				
	//For coverity coverage
	//Close 
	TBool tBool = 1 ;
	rxmlDomObject.Close(tBool);
		
	return EPass;
	}
