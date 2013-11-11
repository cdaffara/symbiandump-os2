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
 @file texmlutils.cpp
 @internalTechnology
*/

#include "texml2teststep.h"
#include "libxml2_globals_private.h"
#include <xml/utils/xmlengutils.h>
#include <utf.h>
/**
 * Class CXmlUtilStep Implementation
 */

/**
 * Constructor. Sets the test step name so that it can be 
 * differentiated from within doTestStepL()
 */
CXmlUtilStep::CXmlUtilStep(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

/**
 * TEF invokes doTestStepL interface along with the step name
 * to let appropriate action to be taken for the test step.
 */
TVerdict CXmlUtilStep::doTestStepL(void)
	{
	if (TestStepName() == KXmlEngXmlCharFromDes)
		{
		INFO_PRINTF1(KXmlEngXmlCharFromDes);
		SetTestStepResult(TestKXmlEngXmlCharFromDes());
		}
	else if (TestStepName() == KXmlEngEscapeForXmlValueStep2)
		{
		INFO_PRINTF1(KXmlEngEscapeForXmlValueStep2);
		SetTestStepResult(TestKXmlEngEscapeForXmlValueStep2());
		}
	else if (TestStepName() == KXmlEngEscapeForXmlValueStep1)
		{
		INFO_PRINTF1(KXmlEngEscapeForXmlValueStep2);
		SetTestStepResult(TestKXmlEngEscapeForXmlValueStep1());
		}
	return TestStepResult();
	}
/**
 * Improving on CLibxml2Tester::UtilsXmlCharFromDesL
 */
TVerdict CXmlUtilStep::TestKXmlEngXmlCharFromDes()
	{
	_LIT(KText, "Test Data");
	TBufC<10> buf(KText);
	TPtrC desc = buf.Des();
	TBuf8<10> desc8;
	char * tekst = XmlEngXmlCharFromDesL(desc);
	
	CnvUtfConverter::ConvertFromUnicodeToUtf8(desc8, desc);
	if (!CompareDescAndChar(tekst, desc8))
		{
		return EFail;
		}
	TBufC8<10> buf8(desc8);
	TPtrC8 inDesc8 = buf8.Des();
	const char * compareString = "Test Data";
	char * tekst2 = XmlEngXmlCharFromDes8L(inDesc8);
	if(!CompareChars(tekst2, compareString))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Improving on CLibxml2Tester::UtilsEscapeForXmlValueL
 */
TVerdict CXmlUtilStep::TestKXmlEngEscapeForXmlValueStep1()
	{
	const char * testString = "Test < & > text.";
	const char * compareString = "Test &lt; &amp; &gt; text.";
	//Test step 1
	char *resultString = XmlEngEscapeForXmlValueL(testString);
	if (!CompareChars(resultString, compareString))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests XmlEngEscapeForXmlValueL under different memory conditions
 * This test step should be run only on debug builds owing to the 
 * __UHEAP macros that are defined only in debug builds.
 */
TVerdict CXmlUtilStep::TestKXmlEngEscapeForXmlValueStep2()
	{
#ifdef _DEBUG
	TInt err = KErrNone;
	const char * testString = "Test < & > text.";
	xmlGlobalStatePtr gsPtr = xmlCreateAndInitializeGlobalState();
	if (!gsPtr)
		{
		INFO_PRINTF1(_L("Warning: Global state pointer is NULL\n"));
		}
	__UHEAP_MARK;
	__UHEAP_SETFAIL(RHeap::EDeterministic, 1);

	TRAP(err, XmlEngEscapeForXmlValueL(testString));
	if (err != KErrNoMemory)
		{
		return EFail;
		}
	char* result= NULL;
	TRAP(err, result = XmlEngEscapeForXmlValueL(NULL));
	if (err != KErrNone && !result)
		{
		return EFail;
		}
	__UHEAP_SETFAIL(RHeap::ENone, 0);
	__UHEAP_MARKEND;
#else
	INFO_PRINTF1(_L("This test is meant to be run only in DEBUG builds. Returning EPASS"));
#endif
	return EPass;
	}

/**
 * Utility function to compare the characters
 */
TBool CXmlUtilStep::CompareChars(const char* aString1, const char* aString2)
	{
	TInt strLen = strlen(aString1);
	TInt desLen = strlen(aString2);
	if (strLen != desLen)
		{
		return FALSE;
		}
	else
		{
		for (TInt i = 0; i < strLen; i++)
			{
			if (aString1[i] != aString2[i])
				return FALSE;
			}
		}
	return TRUE;
	}

/**
 * Utility function to compare C string with Desc8
 */
TBool CXmlUtilStep::CompareDescAndChar(const char* aString,TDesC8& aDesc)
{
	TInt strLen = strlen(aString);
	TInt desLen = aDesc.Length();
	if (strLen != desLen)
	{
		return FALSE;	
	}
	else
	{
		for(TInt i = 0; i < strLen; i++)
		{
			if (aString[i] != aDesc[i])
				return FALSE;
		}
	}	
	return TRUE;
}
