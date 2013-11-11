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
 @file CXmlEngTestLibXMLNodeCleanup.cpp
 @internalTechnology
*/
#include "CXmlEngTestLibXMLNodeCleanup.h"

#include <xml/dom/xmlengdomutils.h>
#include <xml/dom/xmlengdomimplementation.h>
#include <xml/dom/xmlengdocument.h>




/**
 * Class CXmlEngTestLibXMLNodeCleanup Implementation
 */
CXmlEngTestLibXMLNodeCleanup::CXmlEngTestLibXMLNodeCleanup(const TDesC& aStepName)
	{
	//Set the test step name in the base class
	SetTestStepName(aStepName);
	} 

TVerdict CXmlEngTestLibXMLNodeCleanup::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngLibXMLNodeCleanup)
		{
		INFO_PRINTF1(KXmlEngLibXMLNodeCleanup);
		SetTestStepResult(TestKXmlEngLibXMLNodeCleanup());
		}
			
	return TestStepResult();
	}



TVerdict CXmlEngTestLibXMLNodeCleanup::TestKXmlEngLibXMLNodeCleanup()
	{
#ifdef _DEBUG
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
	
	//To invoke the method 'LibxmlNodeCleanup() mrthod of file: XMLEngDomBase' for code coverage
	__UHEAP_SETFAIL(RHeap::EDeterministic, 4);
	TXmlEngElement aElement;
	TRAP(err , aElement  = doc.CreateElementL(aLocalName, aNamespaceUri, aPrefix));
	__UHEAP_SETFAIL(RHeap::ENone, 0); 
		
	doc.Close();               
	domImpl.Close();
		
	if (KErrNoMemory != err)
	{
		return EFail;
	}
#else
  INFO_PRINTF1(_L("This test is meant to be run only in DEBUG builds. Returning EPASS"));
#endif
	return EPass;
	}
