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
 @file CXmlEngNodeTestReplaceWith.cpp
 @internalTechnology
*/

#include "CXmlEngNodeTestReplaceWith.h"

#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdocument.h>

/**
 * Class CXmlEngDOMUtilStep1 Implementation
 */
CXmlEngNodeTestReplaceWith::CXmlEngNodeTestReplaceWith(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngNodeTestReplaceWith::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngNodeTestReplaceWithStep)
		{
		INFO_PRINTF1(KXmlEngNodeTestReplaceWithStep);
		SetTestStepResult(TestKXmlEngTestReplace());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngNodeTestReplaceWith::TestKXmlEngTestReplace()
	{
	 TInt err = KErrNone; 
	_LIT8(KLit1, "asdfds");
	_LIT8(KLit2, "xzcvcv");
	_LIT8(KLit3, "lklklk");
	TBufC8<30> aLocalName(KLit1);
	TBufC8<30> aNamespaceUri(KLit2);
	TBufC8<30> aPrefix(KLit3);
	
	RXmlEngDOMImplementation domImpl;
	domImpl.OpenL(); 
		
	RXmlEngDocument doc;
	doc.OpenL(domImpl);

	TXmlEngElement aElement = doc.CreateElementL(aLocalName, aNamespaceUri, aPrefix);
	TXmlEngElement bElement = doc.CreateElementL(aNamespaceUri, aLocalName, aPrefix);
	
	//For code coverage
	TRAP(err, aElement.ReplaceWithL(bElement));
	
	doc.Close();               
	domImpl.Close();
		
	if (KErrNone != err)
	{
		return EFail;
	}
	
	return EPass;
	}

