// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32debug.h>
#include <e32test.h>
#include "BaAssert.h"
#include "BaCompileAssert.h"

RTest TheTest(_L(" t_baassert.cpp"));

//
//
//Test macro and functions
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)


/**
@SYMTestCaseID          BASESRVCS-BAFL-UT-4068
@SYMTestCaseDesc        Tests the error code table (KErrorMapTbl) looks ups do not
                        overrun and return random data in memory.
@SYMTestPriority        Medium
@SYMTestActions         Passes in an invalid error code, to check the correct error is return.
                        Passes in valid error codes, at the end of the table to make sure the correct error
                        code is returned.
@SYMTestExpectedResults Correct system error codes are returned.
@SYMDEF                 DEF124481
*/
void DEF124481()
	{
	TBaAssert assertObj(TBaAssert::ELeave);

	TInt panicCode = 111; // Invalid error code. Valid range is 0-110. See KErrorMapTbl.
	// Should return 'KErrArgument' as the panicCode is outside the valid range.
	TRAPD (error, assertObj.AssertDebL(EFalse, static_cast <TBafPanic> (panicCode)));
	TEST2(error, KErrArgument);

	panicCode = EBafPanicRFsConnectArg; // Equates to 110, which maps to 'KErrArgument'.
	// Should return 'KErrArgument'.
	TRAP (error, assertObj.AssertDebL(EFalse, static_cast <TBafPanic> (panicCode)));
	TEST2(error, KErrArgument);

	panicCode = EBafPanicBufLength; // Equates to 109, which maps to 'KErrCorrupt'.
	// Should return 'KErrCorrupt'.
	TRAP (error, assertObj.AssertDebL(EFalse, static_cast <TBafPanic> (panicCode)));
	TEST2(error, KErrCorrupt);
	}


LOCAL_D void RunTestsL()
	{
	TheTest.Start(_L("@SYMTestCaseID:ASESRVCS-BAFL-UT-4068 BAFL error code table look up tests"));
	DEF124481();
	}


TInt E32Main()
	{
	__UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
    TheTest.Title();
    TRAPD(err,::RunTestsL());
	TEST2(err, KErrNone);
	TheTest.End();
	TheTest.Close();
    delete(cleanup);
	__UHEAP_MARKEND;
    return(0);
	}
