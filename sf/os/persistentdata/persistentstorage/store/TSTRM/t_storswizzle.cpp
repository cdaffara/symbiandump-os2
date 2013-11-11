// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//
#include "S32STD.H"
#include "S32MEM.H"
#include <e32test.h>

RTest TheTest(_L("t_storswizzle"));

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		RDebug::Print(_L("*** Boolean expression evaluated to false.\r\n"));
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d.\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

class TRectangle
	{
public:
	TRectangle();
	TRectangle(TInt aWidth, TInt aHeight);

	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	
public:
	TInt	iWidth;
	TInt	iHeight;
	};

TBool operator==(const TRectangle& aLeft, const TRectangle& aRight)
	{
	return aLeft.iWidth == aRight.iWidth && aLeft.iHeight == aRight.iHeight;
	}

TRectangle::TRectangle() :
	iWidth(-1),
	iHeight(-1)
	{
	}

TRectangle::TRectangle(TInt aWidth, TInt aHeight) :
	iWidth(aWidth),
	iHeight(aHeight)
	{
	}

void TRectangle::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << (TInt32)iWidth;
	aStream << (TInt32)iHeight;
	}

void TRectangle::InternalizeL(RReadStream& aStream)
	{
	TInt32 a;
	aStream >> a;
	iWidth = a;
	aStream >> a;
	iHeight = a;
	}

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          PDS-STORE-CT-4060
@SYMTestCaseDesc        TSwizzleC<T> tests.
@SYMTestActions         TSwizzleC<T> functionality test. 
@SYMTestPriority        High
@SYMTestExpectedResults Test must not fail
*/
void TestSwizzleCL()
	{
	CBufStore* bufStore = CBufStore::NewLC(100);
	
	const TInt KWidth = 10;
	const TInt KHeight = 20;
	TRectangle r1(KWidth, KHeight);
	RStoreWriteStream wstrm1;
	TStreamId strmId1 = wstrm1.CreateLC(*bufStore);
	TSwizzleC<TRectangle> swizzle1(&r1);
	TEST((const void*)swizzle1 == (const void*)&r1);
	wstrm1 << *swizzle1;
	wstrm1.CommitL();
	CleanupStack::PopAndDestroy(&wstrm1);
	
	TRectangle r2;
	RStoreReadStream rstrm1;
	rstrm1.OpenLC(*bufStore, strmId1);
	rstrm1 >> r2;
	CleanupStack::PopAndDestroy(&rstrm1);
	
	TEST(r1 == r2);
	
	CleanupStack::PopAndDestroy(bufStore);
	
	TSwizzleC<TRectangle> swizzle2(swizzle1);
	TEST(swizzle1->iWidth == swizzle2->iWidth);
	TEST(swizzle1->iHeight == swizzle2->iHeight);
	TEST(swizzle1.AsPtr()->iHeight == swizzle2.AsPtr()->iHeight);

	
	TSwizzleC<TRectangle> swizzle3;
	swizzle3 = &r2;
	TEST(swizzle1->iWidth == swizzle3->iWidth);
	TEST(swizzle1->iHeight == swizzle3->iHeight);
	TEST(swizzle1.AsPtr()->iHeight == swizzle3.AsPtr()->iHeight);
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4061
@SYMTestCaseDesc        TSwizzleC<TAny> tests.
@SYMTestActions         TSwizzleC<TAny> functionality test. 
@SYMTestPriority        High
@SYMTestExpectedResults Test must not fail
*/
void TestSwizzleCAny()
	{
	const TInt KWidth = 10;
	const TInt KHeight = 20;
	TRectangle r1(KWidth, KHeight);
	
	TSwizzleC<TAny> swizzle1(&r1);
	TSwizzleC<TAny> swizzle2(&r1);
	TSwizzleC<TAny> swizzle3;
	swizzle3 = &r1;
	TEST((const void*)swizzle3 == (const void*)&r1);
	
	TSwizzleCBase b1 = swizzle1; 
	TSwizzleCBase b2 = swizzle2;
	TBool rc = b1 == b2;
	TEST(rc);
	rc = b1 != b2;
	TEST(!rc);
	
	TSwizzleC<TAny> swizzle4(b1);
	TEST(swizzle4.AsPtr() == swizzle1.AsPtr());

	const void* p1 = swizzle1.AsPtr();
	const void* p2 = swizzle2.AsPtr();
	const void* p3 = swizzle3.AsPtr();
	
	TEST(((const TRectangle*)p1)->iWidth == ((const TRectangle*)p2)->iWidth);
	TEST(((const TRectangle*)p1)->iHeight == ((const TRectangle*)p2)->iHeight);
	TEST(((const TRectangle*)p3)->iWidth == ((const TRectangle*)p2)->iWidth);
	TEST(((const TRectangle*)p3)->iHeight == ((const TRectangle*)p2)->iHeight);
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4062
@SYMTestCaseDesc        TSwizzle<TAny> tests.
@SYMTestActions         TSwizzle<TAny> functionality test. 
@SYMTestPriority        High
@SYMTestExpectedResults Test must not fail
*/
void TestSwizzleAny()
	{
	const TInt KWidth = 10;
	const TInt KHeight = 20;
	TRectangle r1(KWidth, KHeight);
		
	TSwizzle<TAny> swizzle1(&r1);
	TSwizzle<TAny> swizzle2(&r1);
	TSwizzle<TAny> swizzle3;
	swizzle3 = &r1;
	TEST((void*)swizzle3 == (void*)&r1);
	
	TSwizzleBase b1 = swizzle1; 
	TSwizzleBase b2 = swizzle2;
	TBool rc = b1 == b2;
	TEST(rc);
	rc = b1 != b2;
	TEST(!rc);
	
	TSwizzle<TAny> swizzle4(b1);
	TEST(swizzle4.AsPtr() == swizzle1.AsPtr());
	
	void* p1 = swizzle1.AsPtr();
	void* p2 = swizzle2.AsPtr();
	void* p3 = swizzle3.AsPtr();
	
	TEST(((TRectangle*)p1)->iWidth == ((TRectangle*)p2)->iWidth);
	TEST(((TRectangle*)p1)->iHeight == ((TRectangle*)p2)->iHeight);
	TEST(((TRectangle*)p3)->iWidth == ((TRectangle*)p2)->iWidth);
	TEST(((TRectangle*)p3)->iHeight == ((TRectangle*)p2)->iHeight);
	
	((TRectangle*)p3)->iWidth = 5;
	((TRectangle*)p3)->iHeight = 3;
	
	TEST2(r1.iWidth, 5);
	TEST2(r1.iHeight, 3);
	
	TSwizzle<TRectangle> swizzle5;
	swizzle5 = &r1;
	TEST2(swizzle5->iWidth, 5);
	TEST2(swizzle5->iHeight, 3);
	}

void DoTestsL()
	{
	TheTest.Start(_L("@SYMTestCaseID:PDS-STORE-CT-4060: TSwizzleC<T> test"));
	TestSwizzleCL();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-STORE-CT-4061: TSwizzleC<TAny> test"));
	TestSwizzleCAny();
	
	TheTest.Next(_L("@SYMTestCaseID:PDS-STORE-CT-4062: TSwizzle<TAny> test"));
	TestSwizzleAny();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);

	__UHEAP_MARK;

	TRAPD(err, DoTestsL());
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
