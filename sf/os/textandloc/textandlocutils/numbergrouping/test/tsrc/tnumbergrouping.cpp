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
#include <NumberGrouping.h>

///////////////////////////////////////////////////////////////////////////////////////
RTest TheTest(_L("TestNumberGrouping"));
	
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

_LIT( KNormalNumber, "13548130726" );
_LIT( KMoreLengthNumber, "1354813072688" );
_LIT( KLessLengthNumber, "135481307" );
//_LIT( KNormalNumberSpace, "         " );
//_LIT( KNormalNumberChar, "abcdefghijk" );
//_LIT( KNoStandardNumber, "12  43 32" );

LOCAL_C void TestNUMGRPINGInsertL()
    {
    TBool reserved( ETrue );
    TInt maxlength( 11 );
    CPNGNumberGrouping* numbergrouping = CPNGNumberGrouping::NewLC( maxlength,
        reserved );
    
    TDesC testlesslengthnumber( KLessLengthNumber );
    numbergrouping->Set( KLessLengthNumber );
    
    TInt einsertposition( 13 );
    TInt err( KErrNone );
    TText value( 1 );
    
    err = numbergrouping->Insert( einsertposition, value );
    TEST(KErrIndexOutOfRange == err);
    
    einsertposition = 9;
    err = numbergrouping->Insert( 9 , value );
    TEST(KErrNone == err);
    
    einsertposition = -8;
    err = numbergrouping->Insert( -8, value );
    TEST(KErrIndexOutOfRange == err);
    
    CleanupStack::PopAndDestroy( numbergrouping );
    }

LOCAL_C void TestNUMGRPINGDeleteL()
    {
    TBool reserved( ETrue );
    TInt maxlength( 11 );
    CPNGNumberGrouping* numbergrouping = CPNGNumberGrouping::NewLC( maxlength,
        reserved );
    
    TDesC testnormalnumber( KNormalNumber );
    numbergrouping->Set( testnormalnumber );
    TInt einsertposition( 13 );
    TInt err( KErrNone );
    
    err = numbergrouping->Delete( einsertposition );
    TEST(KErrIndexOutOfRange == err);
    
    einsertposition = 7;
    err = numbergrouping->Delete( einsertposition );
    TEST(KErrNone == err);
    
    CleanupStack::PopAndDestroy( numbergrouping );
    }

LOCAL_C void TestNUMGRPINGAppendL()
    {
    TBool reserved( ETrue );
    TInt maxlength( 11 );
    CPNGNumberGrouping* numbergrouping = CPNGNumberGrouping::NewLC( maxlength, 
        reserved );
    
    TDesC testnormalnumber( KNormalNumber );
    numbergrouping->Set( testnormalnumber );
    
    TText value( 1 );
    TInt err( KErrNone );
    
    err = numbergrouping->Append( value );
    TEST(KErrOverflow == err);
    CleanupStack::PopAndDestroy( numbergrouping );
    
    CPNGNumberGrouping* numbergroup = CPNGNumberGrouping::NewLC( maxlength, 
        reserved );
    
    TDesC testlesslengthnumber( KLessLengthNumber );
    numbergroup->Set( testlesslengthnumber );
    
    err = numbergrouping->Append( value );
    TEST(KErrNone == err);
    
    CleanupStack::PopAndDestroy( numbergroup );
    }
  
LOCAL_C void TestNUMGRPINGSetL()
    {
    TBool reserved( ETrue );
    TInt maxlength( 11 );
    CPNGNumberGrouping* numbergrouping = CPNGNumberGrouping::NewLC( maxlength,
        reserved );
    
    TDesC testmorelengthnumber( KMoreLengthNumber );
    TDesC testnormalnumber( KNormalNumber );
    TDesC testlesslengthnumber( KLessLengthNumber );
    TInt err( KErrNone );
    
    err = numbergrouping->Set( testmorelengthnumber );
    TEST(KErrOverflow == err);
    
    err = numbergrouping->Set( testnormalnumber );
    TEST(KErrNone == err);
    
    err = numbergrouping->Set( testlesslengthnumber );
    TEST(KErrNone == err);

    CleanupStack::PopAndDestroy( numbergrouping );
    }

LOCAL_C void DoE32MainL()
	{
	TheTest.Start(_L("TestNumberGrouping:\n"));

	TestNUMGRPINGInsertL();
	TestNUMGRPINGDeleteL();
	TestNUMGRPINGAppendL();
	TestNUMGRPINGSetL();
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
