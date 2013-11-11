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
 @file CXmlEngXPathUtilsToBooleanTest.cpp
 @internalTechnology
*/
#include "CXmlEngXPathUtilsToBooleanTest.h"


#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdocument.h>


#include <xml/dom/xmlengxpathutils.h>




/**
 * Class CXmlEngXPathUtilsToBooleanTest Implementation
 */
CXmlEngXPathUtilsToBooleanTest::CXmlEngXPathUtilsToBooleanTest(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 


TVerdict CXmlEngXPathUtilsToBooleanTest::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngXPathUtilsToBooleanTest)
		{
		INFO_PRINTF1(KXmlEngXPathUtilsToBooleanTest);
		SetTestStepResult(TestKXmlEngXPathUtilsToBoolean());
		}
			
	return TestStepResult();
	}


TVerdict CXmlEngXPathUtilsToBooleanTest::TestKXmlEngXPathUtilsToBoolean()
	{
	//Create the dom utils object
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

	doc.CreateDocumentElementL(aLocalName, aNamespaceUri, aPrefix);
	
	TXmlEngElement aElement;  
	
	//For code coverage
	TBool isEmpty = XmlEngXPathUtils::ToBoolean(aElement);
	
	doc.Close();               
	domImpl.Close();
		
	if (KErrNone != err)
	{
		return EFail;
	}
	
	return EPass;
	}

