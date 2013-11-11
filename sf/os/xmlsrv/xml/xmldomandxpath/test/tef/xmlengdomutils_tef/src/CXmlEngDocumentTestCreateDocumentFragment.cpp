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
 @file CXmlEngDocumentTestCreateDocumentFragment.cpp
 @internalTechnology
*/
#include "CXmlEngDocumentTestCreateDocumentFragment.h"

#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengdocumentfragment.h>


/**
 * Class CXmlEngDOMUtilStep1 Implementation
 */
CXmlEngDocumentTestCreateDocumentFragment::CXmlEngDocumentTestCreateDocumentFragment(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngDocumentTestCreateDocumentFragment::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngDocumentCreateDocumentFragment)
		{
		INFO_PRINTF1(KXmlEngDocumentCreateDocumentFragment);
		SetTestStepResult(TestKXmlEngDocumentCreateDocumentFragment());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngDocumentTestCreateDocumentFragment::TestKXmlEngDocumentCreateDocumentFragment()
	{
	//Create the dom utils object
	TInt err = KErrNone; 
	
	RXmlEngDOMImplementation domImpl;
	domImpl.OpenL(); 
		
	RXmlEngDocument doc;
	doc.OpenL(domImpl);

	TXmlEngDocumentFragment xmlengDocFragment; 
	
	//For code coverage
	TRAP(err,  xmlengDocFragment = doc.CreateDocumentFragmentL());
		
	doc.Close();               
	domImpl.Close();
		
	if (KErrNone != err)
	{
		return EFail;
	}
	
	return EPass;
	}
