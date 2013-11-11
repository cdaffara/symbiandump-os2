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
 @file CXmlEngTestLibXMLNodeCleanup.h
 @internalTechnology
*/
#ifndef CXMLENGTESTLIBXMLNODECLEANUP_H_
#define CXMLENGTESTLIBXMLNODECLEANUP_H_

#include <test/testexecutestepbase.h>
#include "libxml2_xmlstring.h"


//Step name 
_LIT(KXmlEngLibXMLNodeCleanup, "XmlEngLibXMLNodeCleanup");


class CXmlEngTestLibXMLNodeCleanup : public CTestStep
{
public:
	//The constructor is passed the step name 
	CXmlEngTestLibXMLNodeCleanup(const TDesC& aStepName);

	//This method is impelemented and from here various sub steps of the tests are invoked
	virtual TVerdict doTestStepL(void);
	
	//These are the methods that are the sub steps for the testing
	TVerdict TestKXmlEngLibXMLNodeCleanup();
};

#endif /*CXMLENGTESTLIBXMLNODECLEANUP_H_*/
