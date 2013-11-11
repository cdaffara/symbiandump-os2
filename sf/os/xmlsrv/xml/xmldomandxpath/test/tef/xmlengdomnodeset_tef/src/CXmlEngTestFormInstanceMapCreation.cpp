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
 @file CXmlEngTestFormInstanceMapCreation.cpp
 @internalTechnology
*/

#include "CXmlEngTestFormInstanceMapCreation.h"


#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengnodeset.h>
#include <xml/dom/xmlengnode.h>

#include <xml/dom/xmlengxpathevaluator.h>


/**
 * Class CXmlEngNodeSetTestContains Implementation
 */
CXmlEngTestFormInstanceMapCreation::CXmlEngTestFormInstanceMapCreation(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngTestFormInstanceMapCreation::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngCreateXFormInstanceMap)
		{
		INFO_PRINTF1(KXmlEngCreateXFormInstanceMap);
		SetTestStepResult(TestKXmlEngCreateXFormInstanceMap());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngTestFormInstanceMapCreation::TestKXmlEngCreateXFormInstanceMap()
	{
	//Create the dom utils object
	TInt err = KErrNone; 
		
	// register instance doc trees
    CXmlEngXFormsInstanceMap* map = NULL;
    
    TRAP(err, map = CXmlEngXFormsInstanceMap::NewL());
	       
    delete map;
    
	if (KErrNone != err)
	{
		return EFail;
	}
	
	return EPass;
	}
