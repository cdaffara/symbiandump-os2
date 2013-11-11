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
 @file texml2teststep.h
 @internalTechnology
*/
#ifndef _XML2TESTSTEP_H_
#define _XML2TESTSTEP_H_

#include <test/testexecutestepbase.h>

/**
 * Test Steps:
 * KXmlEngLeaveOOM 			: tests xmlengmem:XmlEngLeaveOOML
 * KXmlEngXmlCharFromDes 	: tests xmlengutils:XmlEngXmlCharFromDesL
 * KXmlEngEscapeForXmlValue : tests xmlengutils:XmlEngEscapeForXmlValue
 * KXmlEnginePush			: tests xmlengxmlengine:XmlEnginePushL
 * KXmlEnginePopAndClose 	: tests xmlengxmlengine:XmlEnginePopAndClose
 * KXmlStringCopy			: tests xmlengxmlstring:CopyL
 * KXmlStringFree			: tests xmlengxmlstring:Free
 * KXmlStringLength			: tests xmlengxmlstring:Length
 * KXmlStringSet1			: tests xmlengxmlstring:Set
 * KXmlStringAlloc1			: tests xmlengxmlstring:AllocL
 * KXmlStringAlloc2			: tests xmlengxmlstring:AllocLC
 * KXmlStringSet2			: tests xmlengxmlstring:SetL
 * KXmlStringSet3			: tests xmlengxmlstring:SetL
 * KXmlStringAllocAndFree1	: tests xmlengxmlstring:AllocAndFreeL
 * KXmlStringAllocAndFree2	: tests xmlengxmlstring:AllocAndFreeLC
 * KXmlStringAppend   		: tests xmlengxmlstring:AppendL
 */
_LIT(KXmlEngLeaveOOMStep1, "XmlEngLeaveOOMStep1");
_LIT(KXmlEngLeaveOOMStep2, "XmlEngLeaveOOMStep2");
_LIT(KXmlEngXmlCharFromDes, "XmlEngXmlCharFromDes");
_LIT(KXmlEngEscapeForXmlValueStep1, "XmlEngEscapeForXmlValueStep1");
_LIT(KXmlEngEscapeForXmlValueStep2, "XmlEngEscapeForXmlValueStep2");
_LIT(KXmlEnginePushAndPop, "XmlEnginePushAndPop");
_LIT(KXmlStringCopy, "XmlStringCopy");
_LIT(KXmlStringSetStep1, "XmlStringSetStep1");
_LIT(KXmlStringSetStep2, "XmlStringSetStep2");
_LIT(KXmlStringAlloc1, "XmlStringAlloc1");
_LIT(KXmlStringAlloc2, "XmlStringAlloc2");
_LIT(KXmlStringCompare, "XmlStringCompare");
_LIT(KXmlStringAllocAndFree1, "XmlStringAllocAndFree1");
_LIT(KXmlStringAllocAndFree2, "XmlStringAllocAndFree2");
_LIT(KXmlStringAppend, "XmlStringAppend");

/**
 * For the test cases covering APIs in xmlengmem
 */
class CXmlEngMemStep : public CTestStep
	{
public:
	CXmlEngMemStep(const TDesC& aStepName);
	virtual TVerdict doTestStepL(void);
private:
	TVerdict TestKXmlEngLeaveOOMStep1();
	TVerdict TestKXmlEngLeaveOOMStep2();
	};

/**
 * For the test cases covering APIs in xmlengutils
 */
class CXmlUtilStep : public CTestStep
	{
public:
	CXmlUtilStep(const TDesC& aStepName);
	virtual TVerdict doTestStepL(void);
private:
	TVerdict TestKXmlEngXmlCharFromDes();
	TVerdict TestKXmlEngEscapeForXmlValueStep1();
	TVerdict TestKXmlEngEscapeForXmlValueStep2();
	TBool CompareChars(const char* aString1,const char* aString2);
	TBool CompareDescAndChar(const char* aString,TDesC8& aDesc);
	};

/**
 * For the test cases covering APIs in xmlengxmlengine
 */
class CXmlEngineStep : public CTestStep
	{
public:
	CXmlEngineStep(const TDesC& aStepName);
	virtual TVerdict doTestStepL(void);
private:
	TVerdict TestKXmlEnginePushAndPopL();
	};

/**
 * For the test cases covering APIs in xmlengxmlstring
 */
class CXmlStringStep : public CTestStep
	{
public:
	CXmlStringStep(const TDesC& aStepName);
	virtual TVerdict doTestStepL(void);
private:
	TVerdict TestKXmlStringCopy();
	TVerdict TestKXmlStringSetStep1();
	TVerdict TestKXmlStringSetStep2();
	TVerdict TestKXmlStringAppend();
	TVerdict TestKXmlStringCompare();
	TVerdict TestKXmlStringAlloc1();
	TVerdict TestKXmlStringAlloc2();
	TVerdict TestKXmlStringAllocAndFree1();
	TVerdict TestKXmlStringAllocAndFree2();

	};
#endif
