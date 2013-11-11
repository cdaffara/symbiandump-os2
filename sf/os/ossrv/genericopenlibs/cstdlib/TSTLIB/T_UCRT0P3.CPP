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
// *IMPORTANT*: This should only be run when called from T_UCRT0P1.
// See TestCase SYSLIB-STDLIB-UT-4003 in T_UCRT0P1.CPP for more information.
// 
//

#include <stdlib.h>
#include <e32debug.h>
#include <e32test.h>
#include <estlib.h>


RTest TheTest(_L("T_UCRT0P3"));


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



//DEF124477: [coverity] RESOURCE_LEAK - stdlib.
void Defect_DEF124477_Part2()
	{
	int argc=0;
	char** char_argv=0;
	char** char_envp=0;

	TheTest.Printf(_L("Retrieving the arguments passed in from T_UCRT0P1\n"));
	
	// Set heap memory allocation failure for 'char_argv'.
	TheTest.Printf(_L("Set RHeap::EFailNext on the 4th memory allocation event.\n"));
	__UHEAP_SETFAIL(RHeap::EFailNext, 4);

	TheTest.Printf(_L("Try to get args and environment.\n"));
	__crt0(argc,char_argv,char_envp);			// get args & environment from somewhere
	
	// Memory allocation failure should result in the following variables being NULL.
	TheTest.Printf(_L("Check argc is NULL.\n"));
	TEST(argc==0);
	TheTest.Printf(_L("Check char_argv is NULL.\n"));
	TEST(char_argv==0);
	TheTest.Printf(_L("Check char_envp is NULL.\n"));
	TEST(char_envp==0);


	//Reset values.
	argc=0;
	char_argv=0;
	char_envp=0;
	
	// Set heap memory allocation failure for 'cmdbuf' in __crt0 to fail, resulting in routine
	// returning early, so 'char_argc' and 'char_envp' should still be NULL.
	TheTest.Printf(_L("Set RHeap::EFailNext on the 1st memory allocation event.\n"));
	__UHEAP_SETFAIL(RHeap::EFailNext, 1);

	TheTest.Printf(_L("Try to get args and environment.\n"));
	__crt0(argc,char_argv,char_envp);			// get args & environment from somewhere
	
	// Memory allocation failure should result in the following variables being NULL.
	TheTest.Printf(_L("Check argc is NULL.\n"));
	TEST(argc==0);
	TheTest.Printf(_L("Check char_envp is NULL.\n"));
	TEST(char_envp==0);


	wchar_t** wchar_t_argv=0;
	wchar_t** wchar_t_envp=0;
	
	TheTest.Printf(_L("Retrieving the arguments passed in from T_UCRT0P1\n"));
	
	// Will set the memory allocation for wargv in __crt0 to fail.
	TheTest.Printf(_L("Set RHeap::EFailNext on the 3rd memory allocation event.\n"));
	__UHEAP_SETFAIL(RHeap::EFailNext, 3);

	TheTest.Printf(_L("Now try to get args and environment.\n"));
	__crt0(argc,wchar_t_argv,wchar_t_envp);			// get args & environment from somewhere
	
	// The arguements passed into __crt0 should still be NULL.
	TheTest.Printf(_L("Check argc is NULL.\n"));
	TEST(argc==0);
	TheTest.Printf(_L("Check wchar_t_argv is NULL.\n"));
	TEST(wchar_t_argv==0);
	TheTest.Printf(_L("Check wchar_t_envp is NULL.\n"));
	TEST(wchar_t_envp==0);
	
	exit(0);
	}

/**
Invoke the tests
*/
LOCAL_C void RunTestsL ()
    {
	Defect_DEF124477_Part2();
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
	TheTest.Printf(_L("** Starting the tests in the child process T_UCRT0P3 ...\n"));
	TheTest.Start(_L(" @SYMTestCaseID: "));
	TRAPD(error,RunTestsL());
	TEST2(error,KErrNone);

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	delete tc;
	return 0;
	}

