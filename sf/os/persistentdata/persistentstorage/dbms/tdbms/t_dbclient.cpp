// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <d32dbms.h>
#include <e32test.h>

LOCAL_D CTrapCleanup* TheTrapCleanup;

LOCAL_D RTest test(_L("t_dbclient"));

LOCAL_D const TPtrC KColName(_S("A_column_name"));
LOCAL_D TDbColName VarName(_S("Column_A"));

const TInt KTestCleanupStack=0x20;
const TInt KLeaveError=-4000;
const TInt KColCount=26;

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0586
@SYMTestCaseDesc        Tests for TDbCol class
@SYMTestPriority        Medium
@SYMTestActions        	Tests for column name and type after creating them.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTDbCol()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0586 Testing TDbCol "));
	TDbCol c1(KColName,EDbColText8);
	test(c1.iName==KColName);
	test(c1.iType==EDbColText8);
	test(c1.iMaxLength==KDbDefaultTextColLength);
	test(c1.iAttributes==0);
	TDbCol c2(KColName,EDbColText8,1234);
	test(c2.iName==KColName);
	test(c2.iType==EDbColText8);
	test(c2.iMaxLength==1234);
	test(c2.iAttributes==0);
	TDbCol c3(KColName,EDbColBit);
	test(c3.iName==KColName);
	test(c3.iType==EDbColBit);
	test(c3.iMaxLength==KDbUndefinedLength);
	test(c3.iAttributes==0);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0587
@SYMTestCaseDesc        CDbColSet class test
@SYMTestPriority        Medium
@SYMTestActions        	Tests for creation of a CDbColSet column set object
                        Tests for adding and removing columns to the column set
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestCDbColSetL()
	{
	TInt r;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0587 ctor and dtor "));
	CDbColSet* c=CDbColSet::NewL();		// assume it will succeed
	test(c!=NULL);
	test (c->Count()==0);
	delete c;
	__UHEAP_CHECK(0);
#if defined(_DEBUG)
	__UHEAP_FAILNEXT(1);
	TRAP(r,c=CDbColSet::NewL());
	test(r!=KErrNone);
	__UHEAP_CHECK(0);
#endif
	c=CDbColSet::NewLC();
	test(c!=NULL);
	CleanupStack::PopAndDestroy();
	__UHEAP_CHECK(0);
	c=CDbColSet::NewL();
	TDbCol col(KColName,EDbColText,20);
	c->AddL(col);
	delete c;
	__UHEAP_CHECK(0);
	TRAP(r,c=CDbColSet::NewLC();c->AddL(col);User::Leave(KLeaveError););
	test(r==KLeaveError);
	__UHEAP_CHECK(0);
//
	test.Next(_L("Add columns"));
	c=CDbColSet::NewLC();
	TInt ii;
	for (ii=1;ii<=KColCount;ii++)
		{
		VarName[7]=(TUint8)('A'-1+ii);
		test(c->Col(VarName)==NULL);
		TDbCol column(VarName,EDbColInt32);
		c->AddL(column);
		test(c->Count()==ii);
		test(c->ColNo(VarName)!=KDbNullColNo);
		test(c->Col(VarName)!=NULL);
		}
	test.Next(_L("Check columns: operator[] and ColNo()"));
	for (ii=1;ii<=KColCount;ii++)
		{
		const TDbCol& col=(*c)[ii];
		test(c->ColNo(col.iName)==ii);
		}
	test.Next(_L("Remove columns"));
	for (ii=1;ii<=KColCount;ii+=2)
		{
		VarName[7]=(TUint8)('A'-1+ii);
		c->Remove(VarName);
		}
	test(c->Count()==13);
	test.Next(_L("Clear"));
	c->Clear();
	test(c->Count()==0);
	CleanupStack::PopAndDestroy();
	__UHEAP_CHECK(0);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0588
@SYMTestCaseDesc        Tests for TDbColSetIter class
@SYMTestPriority        Medium
@SYMTestActions        	Tests for iterate over the contents of a column set
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTDbColSetIterL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0588 Testing TDbColSetIter "));
	CDbColSet* c=CDbColSet::NewLC();
	TInt ii;
	for (ii=0;ii<KColCount;++ii)
		{
		VarName[7]=(TUint8)('A'-1+ii);
		c->AddL(TDbCol(VarName,EDbColInt32));
		}
	TDbColSetIter iter1(*c);
	TDbColSetIter iter2(iter1);
	ii=0;
	for (;;)
		{
		test(iter1.Col()==++ii);
		test(iter1.operator->()==&iter1.operator*());
		test(&*iter1++==&*iter2);
		if (!iter1)
			break;
		test(&*iter1==&*++iter2);
		}
	test(!++iter2);
	test(ii==KColCount);
	CleanupStack::PopAndDestroy();
	__UHEAP_CHECK(0);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0589
@SYMTestCaseDesc        Tests for TDbCol,CDbColSet,TDbColSetIter classes
@SYMTestPriority        Medium
@SYMTestActions        	Executes the tests of TDbCol,CDbColSet,TDbColSetIter
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestColSetL()
	{
	test.Next(_L("@SYMTestCaseID:SYSLIB-DBMS-CT-0589 "));
	TestTDbCol();
	test.Next(_L("Testing CDbColSet"));
	TestCDbColSetL();
	TestTDbColSetIterL();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0590
@SYMTestCaseDesc        Tests for TDbKeyCol class
@SYMTestPriority        Medium
@SYMTestActions         Attempts to test for attributes of the key column together.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTDbKeyCol()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0590 Testing TDbKeyCol "));
	TDbKeyCol c1(KColName);
	test(c1.iName==KColName);
	test(c1.iOrder==TDbKeyCol::EAsc);
	TDbKeyCol c2(KColName,TDbKeyCol::EDesc);
	test(c2.iName==KColName);
	test(c2.iOrder==TDbKeyCol::EDesc);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0591
@SYMTestCaseDesc        Tests the CDbKey class
@SYMTestPriority        Medium
@SYMTestActions         Tests for the new CDbKey creation using NewL and NewLC functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestCDbKeyL()
	{
	TInt r;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0591 ctor and dtor "));
	CDbKey* k=CDbKey::NewL();		// assume it will succeed
	test(k!=NULL);
	delete k;
	__UHEAP_CHECK(0);
#if defined(_DEBUG)
	__UHEAP_FAILNEXT(1);
	TRAP(r,k=CDbKey::NewL());
	test(r!=KErrNone);
	__UHEAP_CHECK(0);
#endif
	k=CDbKey::NewLC();
	test(k!=NULL);
	CleanupStack::PopAndDestroy();
	__UHEAP_CHECK(0);
	k=CDbKey::NewL();
	k->AddL(VarName);
	delete k;
	__UHEAP_CHECK(0);
	TRAP(r,k=CDbKey::NewLC();k->AddL(KColName);User::Leave(KLeaveError););
	test(r==KLeaveError);
	__UHEAP_CHECK(0);
//
	test.Next(_L("Add key columns"));
	k=CDbKey::NewLC();
	TInt ii;
	for (ii=1;ii<=KColCount;++ii)
		{
		VarName[7]=(TUint8)('A'-1+ii);
		k->AddL(VarName);
		test(k->Count()==ii);
		test((*k)[ii-1].iName==VarName);
		}
	test.Next(_L("Remove key columns"));
	for (ii=1;ii<=KColCount;ii+=2)
		{
		VarName[7]=TUint8('A'-1+ii);
		k->Remove(VarName);
		}
	test(k->Count()==KColCount/2);
	test.Next(_L("Clear"));
	k->Clear();
	test(k->Count()==0);
	test.Next(_L("Unique flag"));
	test(!k->IsUnique());
	k->MakeUnique();
	test(k->IsUnique());
	k->MakeUnique();
	test(k->IsUnique());
	CleanupStack::PopAndDestroy();
	__UHEAP_CHECK(0);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0592
@SYMTestCaseDesc        Tests the TDbKeyCol,CDbKey classes
@SYMTestPriority        Medium
@SYMTestActions         Executes the TDbKeyCol,CDbKey tests
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestKeyL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0592 "));
	TestTDbKeyCol();
	test.Next(_L("Testing CDbKey"));
	TestCDbKeyL();
	test.End();
	}


LOCAL_C void setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
	}

GLDEF_C TInt E32Main()
//
// Test Client-side objects
//
    {
	test.Title();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L("Test the Column Set"));
	TRAPD(r,TestColSetL();)
	test(r==KErrNone);
	__UHEAP_CHECK(0);
	test.Next(_L("Test the Key"));
	TRAP(r,TestKeyL();)
	test(r==KErrNone);
	test.End();
//
	__UHEAP_MARKEND;
	delete TheTrapCleanup;
	test.Close();
	return 0;
    }
