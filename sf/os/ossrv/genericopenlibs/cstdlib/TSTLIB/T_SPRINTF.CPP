// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include <e32test.h>
#include <e32svr.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
// Globals

static RTest TheTest(_L("T_SPRINTF"));

//
//
//Test macro and function

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)

//
// Tests

/**
@SYMTestCaseID 			SYSLIB-STDLIB-UT-1670
@SYMTestCaseDesc	    Making sure that error codes are returned from sprintf when when out of memory. 
@SYMTestPriority 	    Critical
@SYMTestActions  	    Sets heap failures to occur at different stages and calls sprintf. The sprintf function should return with error codes
						when errors occur.
@SYMTestExpectedResults The test should not fail.
@SYMDEF 				DEF083988
*/	
void DEF083988()
	{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-UT-1670 DEF083988 - Prop: sprintf panics with ESTLIB-INIT 0 when out of memory "));
	
	int ret;
	char buf[30];
	char* hw = "How are you?";	
	
	#ifdef _DEBUG
	const int err = -1;
	
	__UHEAP_FAILNEXT(1);
    ret = sprintf(buf, "Hello");
	TEST(ret == err);  // Error returned as memory needed to be allocated, but none was spare.	
	__UHEAP_RESET;
	
	__UHEAP_FAILNEXT(2);
    ret = sprintf(buf, "Hello");
	TEST(ret == 5);  // No Error, as memory allocation failure did not affected sprintf.
	TEST(strcmp(buf, "Hello")==0);
	__UHEAP_RESET;
	// Releases all the STDLIB resources so memory for the globals will need to be allocated once more.	
	CloseSTDLIB(); 
	#endif

	
	#ifdef _DEBUG
	__UHEAP_FAILNEXT(3);
    ret = sprintf(buf, "Hello");
	TEST(ret == err);  // Error returned as memory needed to be allocated, but none was spare.
	__UHEAP_RESET;	
	CloseSTDLIB();
	#endif
		
				
    ret = sprintf(buf, "Hello");
	TEST(strcmp(buf, "Hello") == 0);
	TEST(ret == 5);  // Returns the number of characters printed.

	ret = sprintf(buf, hw);
	TEST(strcmp(buf, "How are you?") == 0);
	TEST(ret == 12); // Returns the number of characters printed, as the memory is already allocated.

	#ifdef _DEBUG
	__UHEAP_FAILNEXT(1);	
	ret = sprintf(buf, hw);
	TEST(strcmp(buf, "How are you?") == 0);
	TEST(ret == 12); // Returns the number of characters printed, as the memory is already allocated.
	__UHEAP_RESET;
	#endif
		
	CloseSTDLIB();
		
	ret = sprintf(buf, "How are you?");
	TEST(strcmp(buf, "How are you?") == 0);
	TEST(ret == 12); // Returns the number of characters printed, as the memory is able to be allocated.
	
	CloseSTDLIB();
		
	}



static void Main()
	{
	TheTest.Start(_L("Defect..."));
	DEF083988(); //DEF083988 - Prop: sprintf panics with ESTLIB-INIT 0 when out of memory
	}


TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* tc = CTrapCleanup::New();
    TEST(tc != NULL);
    
	CloseSTDLIB();
	TheTest.Title();
	
    ::Main();
	
	TheTest.End();
	TheTest.Close();

	delete tc;
	CloseSTDLIB();	

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
