/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Test code for CTmCode class
*
*/


#include "TAGMA.H"
#include "TMSTD.H"

#include <e32test.h>
#include <bitdev.h>
#include "ttmcode.h"

TVerdict CTTmCodeStep::doTestStepL()
	{
    SetTestStepResult(EPass);
    INFO_PRINTF1(_L("TTmCode - Tests CTmCode class"));
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-TTMCODE-0001 CTmCode tests "));
	CTmCode* code = new(ELeave) CTmCode;
	CleanupStack::PushL(code);

	// Testcase 1 - Append a number, then check retrived number is the same as the one we appended
	code->CreateBufferL();
	TInt num = 42;
	code->AppendNumberL(num);
	TTmCodeReader reader(*code, 0, 0x7FFFFFFF);
	TEST(reader.ReadNumber() == num);

	// Testcase 2 - Append largest positive number occupying 1 byte (where a byte is 7 bits in the case of CTmCode), check correct value read back
	TInt size;
	TInt sizeDelta;
	size = code->Size();
	num = 63;
	TInt pos = code->AppendNumberL(num);
	sizeDelta = code->Size() - size;
	TEST(sizeDelta == 1 && (pos - size) == 1 && reader.ReadNumber() == num);

	// Testcase 3 - Append smallest positive number occupying 2 bytes, check correct value read back
	size = code->Size();
	num = 64;
	pos = code->AppendNumberL(num);
	sizeDelta = code->Size() - size;
	TEST(sizeDelta == 2 && (pos - size) == 2 && reader.ReadNumber() == num);

	// Testcase 4 - Append largest negative number occupying 1 byte, check correct value read back
	size = code->Size();
	num = -64;
	pos = code->AppendNumberL(num);
	sizeDelta = code->Size() - size;
	TEST(sizeDelta == 1 && (pos - size) == 1 && reader.ReadNumber() == num);

	// Testcase 5 - Append smallest negative number occupying 2 bytes, check correct value read back
	size = code->Size();
	num = -65;
	pos = code->AppendNumberL(num);
	sizeDelta = code->Size() - size;
	TEST(sizeDelta == 2 && (pos - size) == 2 && reader.ReadNumber() == num);

	// Testcase 6 - Append rect, check same rect returned
	size = code->Size();
	TRect rect(1, 1, 2, 2);
	pos = code->AppendRectL(rect);
	sizeDelta = code->Size() - size;
	TEST(sizeDelta == 4 && (pos - size) == 4 && reader.ReadRect() == rect);

	// Testcase 7 - Replace first 2 bytes with 3 different bytes
	CTmCode* code2 = new(ELeave) CTmCode;
	CleanupStack::PushL(code2);
	code2->CreateBufferL();
	code2->AppendNumberL(1);
	code2->AppendNumberL(1);
	code2->AppendNumberL(2);
	code->ChangeL(0, 1, *code2);
	reader.SetCodePos(0);
	TEST(reader.ReadNumber() == 1 && reader.ReadNumber() == 1 && reader.ReadNumber() == 2 && reader.ReadNumber() == 63);

	// Testcase 8 - Insert 1000 numbers, then read them
	size = code->Size();
	int ii;
	for (ii = 0; ii < 1000; ii++)
		code->AppendNumberL(ii);
	reader.SetCodePos(size);
	for (ii = 0; ii < 1000; ii++)
	    TEST(reader.ReadNumber() == ii);

	// Testcase 9 - Insert number at position 42 (1st segment), then reader to 42 and read number
	code->InsertNumberL(4242, 42);
	reader.SetCodePos(42);
	TEST(reader.ReadNumber() == 4242);

	// Testcase 10 - Insert number so it spans a segment boundary, check it reads back OK
	num = -1234567;
	code->InsertNumberL(num, 511);
	reader.SetCodePos(511);
	TEST(reader.ReadNumber() == num);

	// Testcase 11 - ChangeL using a range that spans a segment boundary
	code->InsertByteL(0x11, 515);
	code2->Reset();
	code2->CreateBufferL();
	code2->AppendNumberL(0xabababab);
	code->ChangeL(510, 515, *code2);
	TTmCodeReader reader2(*code, 0, 0x7FFFFFFF);
	reader2.SetCodePos(510);
	TEST(reader2.ReadNumber() == static_cast<TInt32>(0xabababab));
	TEST(reader2.ReadByte() == static_cast<TUint8>(0x11));

	CleanupStack::PopAndDestroy(code2);
	CleanupStack::PopAndDestroy(code);
	
	return TestStepResult();
	}

