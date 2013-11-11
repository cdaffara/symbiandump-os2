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
 @file texmlstring.cpp
 @internalTechnology
*/

#include "texml2teststep.h"
#include <xml/utils/xmlengxestrings.h>
#include <string.h>
#include <utf.h>
#include <e32des16.h>

/**
 * Class CXmlStringStep Implementation
 */

/**
 * Constructor. Sets the test step name so that it can be 
 * differentiated from within doTestStepL()
 */
CXmlStringStep::CXmlStringStep(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

/**
 * TEF invokes doTestStepL interface along with the step name
 * to let appropriate action to be taken for the test step.
 */
TVerdict CXmlStringStep::doTestStepL(void)
	{
	if (TestStepName() == KXmlStringCopy)
		{
		INFO_PRINTF1(KXmlStringCopy);
		SetTestStepResult(TestKXmlStringCopy());
		}
	else if (TestStepName() == KXmlStringSetStep1)
			{
			INFO_PRINTF1(KXmlStringSetStep1);
			SetTestStepResult(TestKXmlStringSetStep1());
			}
	else if (TestStepName() == KXmlStringSetStep2)
			{
			INFO_PRINTF1(KXmlStringSetStep2);
			SetTestStepResult(TestKXmlStringSetStep2());
			}
	else if (TestStepName() == KXmlStringAlloc1)
			{
			INFO_PRINTF1(KXmlStringAlloc1);
			SetTestStepResult(TestKXmlStringAlloc1());
			}
	else if (TestStepName() == KXmlStringAlloc2)
			{
			INFO_PRINTF1(KXmlStringAlloc2);
			SetTestStepResult(TestKXmlStringAlloc2());
			}
	else if (TestStepName() == KXmlStringAllocAndFree1)
			{
			INFO_PRINTF1(KXmlStringAllocAndFree1);
			SetTestStepResult (TestKXmlStringAllocAndFree1());
			}
	else if (TestStepName() == KXmlStringAllocAndFree2)
			{
			INFO_PRINTF1(KXmlStringAllocAndFree2);
			SetTestStepResult(TestKXmlStringAllocAndFree2());
			}
	else if (TestStepName() == KXmlStringAppend)
			{
			INFO_PRINTF1(KXmlStringAppend);
			SetTestStepResult (TestKXmlStringAppend());
			}
	else if (TestStepName() == KXmlStringCompare)
			{
			INFO_PRINTF1(KXmlStringCompare);
			SetTestStepResult(TestKXmlStringCompare());
      }
	return TestStepResult();
	}
/**
 * Tests the APIs Copy(), Free(), PushL(), Set()
 */
TVerdict CXmlStringStep::TestKXmlStringCopy()
	{
	char *string1 = new char[34];
	const char *compareString2 = "<SampleTag>TestString</SampleTag>";
	strcpy(string1, "<SampleTag>TestString</SampleTag>");
	TXmlEngString *testString = new (ELeave) TXmlEngString(string1);
	testString->PushL();
	char* resultString = testString->CopyL();
	if (0 != strcmp(resultString, compareString2))
		{
		return EFail;
		}
	CleanupStack::Pop();
	User::Free(resultString);
	resultString = 0;

	//Test Set(TXmlEngString& src)
	TXmlEngString *testString2 = new (ELeave) TXmlEngString();
	testString2->Set(*testString);
	resultString = testString2->CopyL();
	char* resultString2 = testString->CopyL();
	//Check if the testString2 object has the same string 
	//and testString object's string is empty.
	if (0 != strcmp(resultString, compareString2) && 0 != resultString2)
		{
		return EFail;
		}
	User::Free(resultString);
	resultString = 0;
	//Test if the same object is passed to Set(), it does not delete the
	//existing string inside.
	testString2->Set(*testString2); //both have same string inside
	resultString = testString2->CopyL();
	if (0 != strcmp(resultString, compareString2))
		{
		return EFail;
		}

	char *singleChar = new char[2];
	strcpy(singleChar, "V\0");
	testString->Set(singleChar);
	testString2->Set(*testString);
	resultString = testString2->CopyL();
	if (0 != strcmp(resultString, "V"))
		{
		return EFail;
		}
	//Free the object
	testString->Free();
	return EPass;
	}
/**
 * Tests TXmlEngString::SetL(const TDesC8& aDes8)
 */
TVerdict CXmlStringStep::TestKXmlStringSetStep1()
	{
	_LIT(KText, "<TestData>SomeData</TestData>");
	const char *compareString = "<TestData>SomeData</TestData>";
	TBufC<30> buf(KText);
	TPtrC desc = buf.Des();
	TBuf8<30> buf8;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(buf8, desc);

	char *string1 = new char[34];
	strcpy(string1, "<SampleTag>TestString</SampleTag>");
	TXmlEngString *testString = new (ELeave) TXmlEngString(string1);

	//now set the new value for testString
	testString->SetL(buf8);
	char* resultString = testString->CopyL();
	if ( 0 != strcmp(resultString, compareString))
		{
		return EFail;
		}
	User::Free(resultString);
	resultString = 0;
	//now try setting value to empty TXmlEngString object
	TXmlEngString *testString2 = new (ELeave) TXmlEngString();
	testString2->SetL(buf8);
	resultString = testString2->CopyL();
	if ( 0 != strcmp(resultString, compareString))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests TXmlEngString::SetL(const TDesC8& aDes8)
 */
TVerdict CXmlStringStep::TestKXmlStringSetStep2()
	{
	_LIT(KText, "<TestData>SomeData</TestData>");
	const char *compareString = "<TestData>SomeData</TestData>";
	TBufC<30> buf(KText);
	TPtrC desc = buf.Des();
	char *string1 = new char[34];
	strcpy(string1, "<SampleTag>TestString</SampleTag>");
	TXmlEngString *testString = new (ELeave) TXmlEngString(string1);

	//now set the new value for testString
	testString->SetL(buf);
	char* resultString = testString->CopyL();
	if ( 0 == testString->Length() || 0 != strcmp(resultString, compareString))
		{
		return EFail;
		}
	User::Free(resultString);
	resultString = 0;
	//now try setting value to empty TXmlEngString object
	TXmlEngString *testString2 = new (ELeave) TXmlEngString();
	resultString = testString2->CopyL();
	if (testString2->Length() != 0 && resultString != 0)
		{
		return EFail;
		}
	testString2->SetL(buf);
	resultString = testString2->CopyL();
	if ( 0 != strcmp(resultString, compareString))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests AppendL(), Size(), Copy()
 */
TVerdict CXmlStringStep::TestKXmlStringAppend()
	{
	char *string2 = new char[34];
	const char *compareString = "<SampleTag>TestString</SampleTag>";
	const char *compareString2 =
			"<SampleTag>TestString</SampleTag><SampleTag>TestString</SampleTag>";
	strcpy(string2, "<SampleTag>TestString</SampleTag>");

	//empty testString to start with
	TXmlEngString *testString = new (ELeave) TXmlEngString();
	//without string
	TXmlEngString *appendString1 = new (ELeave) TXmlEngString();
	//with string
	TXmlEngString *appendString2 = new (ELeave) TXmlEngString(string2);

	testString->AppendL(*appendString1, *appendString2);
	char* resultString = testString->CopyL();
	TInt len = appendString1->Size();
	if (len)
		{
		return EFail;
		}
	if ( 0 != strcmp(resultString, compareString))
		{
		return EFail;
		}
	User::Free(resultString);
	resultString = 0;

	appendString2->AppendL(*testString);
	resultString = appendString2->CopyL();
	if ( 0 != strcmp(compareString2, resultString))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests TXmlEngString::AllocL
 */
TVerdict CXmlStringStep::TestKXmlStringAlloc1()
	{
	char *string = new char[34];
	strcpy(string, "<SampleTag>TestString</SampleTag>");
	//empty string
	TXmlEngString *testString = new (ELeave) TXmlEngString();
	HBufC* resBuf = testString->AllocL();

	if (resBuf->Size())
		{
		return EFail;
		}
	TXmlEngString *testString2 = new (ELeave) TXmlEngString(string);
	resBuf = testString2->AllocL();
	_LIT(KText, "<SampleTag>TestString</SampleTag>");
	TBufC<33> buf(KText);

	if (resBuf->Compare(buf.Des()))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests TXmlEngString::AllocLC
 */
TVerdict CXmlStringStep::TestKXmlStringAlloc2()
	{
	char *string = new char[34];
	strcpy(string, "<SampleTag>TestString</SampleTag>");
	//empty string
	TXmlEngString *testString = new (ELeave) TXmlEngString();
	HBufC* resBuf = testString->AllocLC();
	TInt compare = resBuf->Size();
	CleanupStack::PopAndDestroy();
	if (compare)
		{
		return EFail;
		}

	TXmlEngString *testString2 = new (ELeave) TXmlEngString(string);
	resBuf = testString2->AllocLC();
	_LIT(KText, "<SampleTag>TestString</SampleTag>");
	TBufC<33> buf(KText);
	compare = resBuf->Compare(buf.Des());
	CleanupStack::PopAndDestroy();
	if (compare)
		{
		return EFail;
		}
	return EPass;
	}
/**
 * Tests TXmlEngString::AllocAndFreeL
 */
TVerdict CXmlStringStep::TestKXmlStringAllocAndFree1()
	{
	char *string = new char[34];
	strcpy(string, "<SampleTag>TestString</SampleTag>");
	//empty string
	TXmlEngString *testString = new (ELeave) TXmlEngString();
	HBufC* resBuf = testString->AllocAndFreeL();

	if (resBuf->Size())
		{
		return EFail;
		}
	TXmlEngString *testString2 = new (ELeave) TXmlEngString(string);
	resBuf = testString2->AllocAndFreeL();
	_LIT(KText, "<SampleTag>TestString</SampleTag>");
	TBufC<33> buf(KText);

	if (resBuf->Compare(buf.Des()))
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests TXmlEngString::AllocAndFreeLC
 */
TVerdict CXmlStringStep::TestKXmlStringAllocAndFree2()
	{
	char *string = new char[34];
	strcpy(string, "<SampleTag>TestString</SampleTag>");
	//empty string
	TXmlEngString *testString = new (ELeave) TXmlEngString();
	HBufC* resBuf = testString->AllocAndFreeLC();
	TInt compare = resBuf->Size();
	CleanupStack::PopAndDestroy();
	if (compare)
		{
		return EFail;
		}

	TXmlEngString *testString2 = new (ELeave) TXmlEngString(string);
	resBuf = testString2->AllocAndFreeLC();
	_LIT(KText, "<SampleTag>TestString</SampleTag>");
	TBufC<33> buf(KText);
	compare = resBuf->Compare(buf.Des());
	CleanupStack::PopAndDestroy();
	if (compare)
		{
		return EFail;
		}
	return EPass;
	}

/**
 * Tests TXmlEngString::Compare
 */
TVerdict CXmlStringStep::TestKXmlStringCompare()
	{
	char *string = new char[12];
	strcpy(string, "<SampleTag>");
	TXmlEngString *testString1 = new (ELeave) TXmlEngString();
	TXmlEngString *testString2 = new (ELeave) TXmlEngString();

	//both strings empty
	if (testString1->Compare(*testString2))
		{
		return EFail;
		}
	_LIT(KText, "<TestData>SomeData</TestData>");
	TBufC<30> buf(KText);
	TPtrC desc = buf.Des();
	testString1->SetL(desc);
	//testString1 has <TestData>SomeData</TestData>, testString2 is empty
	if (!testString1->Compare(*testString2))
		{
		return EFail;
		}
	char *empty = new char[1];
	empty[0] = '\0';
	//testString1 is set to \0, testString2 is empty
	testString1->Set(empty);
	if (testString2->Compare(*testString1))
		{
		return EFail;
		}
	if (testString1->Compare(*testString2))
		{
		return EFail;
		}

	testString2->Free();
	char *singleChar = new char[2];
	strcpy(singleChar, "V\0");
	testString1->Set(singleChar);
	if (!testString2->Compare(*testString1))
		{
		return EFail;
		}

	char *string1 = new char[13];
	strcpy(string1, "<TestString>");
	char *string2 = new char[13];
	strcpy(string2, "<TestString>");
	testString1->Set(string1);
	testString2->Set(string2);
	if(testString1->Compare(*testString2))
		{
		return EFail;
		}
	return EPass;
	}
