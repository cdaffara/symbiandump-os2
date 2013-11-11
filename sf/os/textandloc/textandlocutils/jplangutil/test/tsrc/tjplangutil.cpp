/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <jplangutil.h>

///////////////////////////////////////////////////////////////////////////////////////
RTest TheTest(_L("TestJPLangUtil"));
	
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macroses and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

const TInt KDesLength = 24;
_LIT( KText, "Text" );

LOCAL_C void TestJLUConvertHalfToFullWidth()
    {
    TBuf<KDesLength> unisrc( KText );
    TBuf<KDesLength> unides; 
    TInt status = JPLangUtil::ConvertHalfToFullWidth( unisrc, unides );
    TEST(status == 4);
    }

LOCAL_C void TestJLUConvertFullToHalfWidth()
    {
    TBuf<KDesLength> unisrc( KText );
    TBuf<KDesLength> unides; 
    TInt status = JPLangUtil::ConvertFullToHalfWidth( unisrc, unides );
    TEST(status == 0);
    }

LOCAL_C void DoE32MainL()
	{
	TheTest.Start(_L("TestJPLangUtil:\n"));

	TestJLUConvertHalfToFullWidth();
	TestJLUConvertFullToHalfWidth();
    }

//  Global Functions

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TheTest.Title();

	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	TEST(trapCleanup != NULL);

	TRAPD(error, DoE32MainL());
	TEST2(error, KErrNone);

	delete trapCleanup;

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
	}
