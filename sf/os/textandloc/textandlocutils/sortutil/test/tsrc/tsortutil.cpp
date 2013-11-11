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
#include <SortUtil.h>
#include "CSortKeyArray.h"

///////////////////////////////////////////////////////////////////////////////////////
RTest TheTest(_L("TestSortUtil"));
	
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


LOCAL_C void TestSortKeyText()
    {
    _LIT( KText, "text" );
    TSortKey sortKey( KText, ESortKeyBasic );

    TDesC text = sortKey.Text();
    TEST(text.Length() != 0);
    }

LOCAL_C void TestSortKeyType()
    {
    _LIT( KText, "text" );
    TSortKey sortKey( KText, ESortKeyBasic );

    TSortKeyType type = sortKey.Type();
    TEST(ESortKeyBasic == type);
    }

LOCAL_C void TestSortUtilNewL()
    {
    CSortUtil* sortUtil = CSortUtil::NewL();
    CleanupStack::PushL( sortUtil );
    TEST(sortUtil != NULL);

    CleanupStack::PopAndDestroy( sortUtil );
    }

LOCAL_C void TestSortUtilCompareItemsL()
    {
    CSortUtil* sortUtil = CSortUtil::NewL();
    CleanupStack::PushL( sortUtil );
    CSortKeyArray* leftSortKeyArray = CSortKeyArray::NewL();
    CleanupStack::PushL( leftSortKeyArray );
    CSortKeyArray* rightSortKeyArray = CSortKeyArray::NewL();
    CleanupStack::PushL( rightSortKeyArray );

    TInt actualResult;    
    {
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("aaa"), 0 );

        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("bbb"), 0 );
        
        actualResult = sortUtil->Interface()->CompareItems( *leftSortKeyArray, *rightSortKeyArray );

        leftSortKeyArray->Reset();
        rightSortKeyArray->Reset();

        TEST(actualResult < 0);
    }

    {
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("aaa"), 0 );

        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("aaa"), 0 );
        
        actualResult = sortUtil->Interface()->CompareItems( *leftSortKeyArray, *rightSortKeyArray );

        leftSortKeyArray->Reset();
        rightSortKeyArray->Reset();

        TEST(actualResult == 0);
    }

    {
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("bbb"), 0 );

        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("aaa"), 0 );
        
        actualResult = sortUtil->Interface()->CompareItems( *leftSortKeyArray, *rightSortKeyArray );

        leftSortKeyArray->Reset();
        rightSortKeyArray->Reset();

        TEST(actualResult > 0);
    }

    {
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("aaa"), 0 );
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("bbb"), 1 );
        
        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("aaa"), 0 );
        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("bbb"), 1 );
        
        actualResult = sortUtil->Interface()->CompareItems( *leftSortKeyArray, *rightSortKeyArray );

        leftSortKeyArray->Reset();
        rightSortKeyArray->Reset();

        TEST(actualResult == 0);
    }

    {
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("aaa"), 0 );
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("bbb"), 1 );
        
        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("aaa"), 0 );
        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("aaa"), 1 );
        
        actualResult = sortUtil->Interface()->CompareItems( *leftSortKeyArray, *rightSortKeyArray );

        leftSortKeyArray->Reset();
        rightSortKeyArray->Reset();

        TEST(actualResult > 0);
    }

    {
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("aaa"), 0 );
        leftSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        leftSortKeyArray->SetText( _L("aaa"), 1 );
        
        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("aaa"), 0 );
        rightSortKeyArray->AppendL( TSortKey( KNullDesC, ESortKeyBasic ) );
        rightSortKeyArray->SetText( _L("bbb"), 1 );
        
        actualResult = sortUtil->Interface()->CompareItems( *leftSortKeyArray, *rightSortKeyArray );

        leftSortKeyArray->Reset();
        rightSortKeyArray->Reset();

        TEST(actualResult < 0);
    }

    CleanupStack::PopAndDestroy( rightSortKeyArray );
    CleanupStack::PopAndDestroy( leftSortKeyArray );
    CleanupStack::PopAndDestroy( sortUtil );
    }

LOCAL_C void DoE32MainL()
	{
	TheTest.Start(_L("TestSortUtil:\n"));

	TestSortKeyText();
	TestSortKeyType();
	TestSortUtilNewL();
	TestSortUtilCompareItemsL();
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
