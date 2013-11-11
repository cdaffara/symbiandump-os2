// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// *IMPORTANT*: This should only be run when called from T_UCRT0P1.
// See TestCase SYSLIB-0001 in T_UCRT0P1.CPP for more information.
// 
//

#include <stdlib.h>
#include <e32debug.h>
#include <e32test.h>
#include <estlib.h>


RTest TheTest(_L("T_UCRT0P2"));


//Test macros and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		TheTest(EFalse, aLine);
	}

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)



//DEF074278: Java VM requires more command line testArguments.
void Defect_DEF074278_Part2()
	{
   	TBuf<180> testArguments(_L("one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty twenty-one twenty-two twenty-three twenty-four "));
	TBuf<210> inputArguments;

	TUint testArgNo = 25;

	int argc=0;
	char** argv=0;
	char** envp=0;

	TheTest.Printf(_L("Retrieving the arguments passed in from T_UCRT0P1\n"));
	__crt0(argc,argv,envp);			// get args & environment from somewhere

	//Get all the arguments into a descriptor, beginning at pos 1, as pos 0 contains
	//the T_UCRT0P2 exe path and name.
	for (TInt i=1; i < argc; i++)
		{
		char *str=argv[i];
		while(*str)
			inputArguments.Append((TChar)*str++);
		inputArguments.Append(' ');
		}

	//tests all the arguments were passed to the new process correctly.
	TheTest.Printf(_L("Checking all arguements were passed to the new process correctly... "));
	TEST(testArguments == inputArguments );
	TEST2(testArgNo, argc);

	exit(0);
	}

/**
Invoke the tests
*/
LOCAL_C void RunTestsL ()
    {
	Defect_DEF074278_Part2();
	}

/**
/This should only be called from T_UCRT0P1, as it is part of the same test.
*/
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Printf(_L("Starting the tests in the child process T_UCRT0P2 ...\n"));
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STDLIB-LEGACY-T_UCRT0P2-0001 "));
	TRAPD(error,RunTestsL());
	TEST2(error,KErrNone);

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	delete tc;
	return 0;
	}

