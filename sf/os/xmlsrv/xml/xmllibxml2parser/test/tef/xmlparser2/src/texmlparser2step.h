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
 @file texmlparser2step.h
 @internalTechnology
*/

#ifndef _TE_XMLPARSER2STEP_H_
#define _TE_XMLPARSER2STEP_H_

#include <test/testexecutestepbase.h>

/**
 * Test Steps 
 */
_LIT(KXmlParser2SetContentSink, "XmlParser2SetContentSink");
_LIT(KXmlParser2OOMTests, "XmlParser2OOMTests");
_LIT(KXmlParser2OOMTests2, "XmlParser2OOMTests2");
_LIT(KXmlParser2OOMTests3, "XmlParser2OOMTests3");
_LIT(KXmlParser2OOMTests4, "XmlParser2OOMTests4");
/**
 * Class to test XML SAX parser 
 */
class CXmlParser2Step : public CTestStep
	{
public:
	CXmlParser2Step(const TDesC& aStepName);
	virtual TVerdict doTestStepL(void);
	TVerdict TestKXmlParser2SetContentSink();
	TVerdict TestKXmlParser2OOMTests();
	TVerdict TestKXmlParser2OOMTests2();
	TVerdict TestKXmlParser2OOMTests3();
	TVerdict TestKXmlParser2OOMTests4();
	};
#endif /*TE_XMLPARSER2STEP_H_*/
