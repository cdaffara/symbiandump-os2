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
 @file CXmlEngNodeTestSetValue.h
 @internalTechnology
*/
#ifndef CXMLENGNODETESTSETVALUE_H_
#define CXMLENGNODETESTSETVALUE_H_

#include <test/testexecutestepbase.h>

//Names of the sub steps of the test case
_LIT(KXmlEngNodeTestSetValueStep, "XmlEngNodeTestSetValue");


class CXmlEngNodeTestSetValue : public CTestStep
{
public:
	//The constructor is passed the step name 
	CXmlEngNodeTestSetValue(const TDesC& aStepName);

	//This method is impelemented and from here various sub steps of the tests are invoked
	virtual TVerdict doTestStepL(void);
	
	//These are the methods that are the sub steps for the testing
	TVerdict TestKXmlEngTestSetValue();
};

#endif /*CXMLENGNODETESTSETVALUE_H_*/
