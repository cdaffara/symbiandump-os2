// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by DWW, April 1996
// Tests descriptor arrays
// 
//

#include <e32test.h>
#include <badesca.h>

LOCAL_D RTest test(_L("T_DESCA"));

class TestDesca
	{
public:
	void MainTestL(CDesCArray* aDescA);
	void ComparisonTest(CDesCArray* aDescA1,CDesCArray* aDescA2);
	void FoldTestL(CDesCArray* aDescA);
	void MixableTestL();
	void DoMixableTest(MDesCArray* aDescA);
	void InSequenceTestL(CDesCArray* aDescA);
    };

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0408
@SYMTestCaseDesc        Tests for folded and non-folded find
@SYMTestPriority        Medium
@SYMTestActions         Tests by appending few descriptors and find the position for the target.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestDesca::FoldTestL(CDesCArray* aDescA)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0408 Test folded and non-folded find "));
	TPtrC target=_L("abcdefg");
	aDescA->AppendL(target);
	aDescA->AppendL(_L("aBcDeFg"));
	aDescA->InsertL(0,_L("ABCDEFG"));
	test(aDescA->Count()==3);
	TInt pos=(-1);
    aDescA->Find(target,pos);
    test(pos==0);
    aDescA->Find(target,pos,ECmpNormal);
    test(pos==1);
	test.Next(_L("Test reset"));
	aDescA->Reset();
	test(!aDescA->Count());
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0409
@SYMTestCaseDesc        CDesCArray class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Add month names to the descriptor array and tests with the find.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestDesca::MainTestL(CDesCArray* aDescA)
	{
    TMonthName name;
    test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0409 Add 12 month names "));
    for (TInt ii=0; ii<12; ii++)
        {
        name.Set((TMonth)ii);
        aDescA->AppendL(name);
        }
    test.Next(_L("Check 12 names exist"));
    test(aDescA->Count()==12);
    test.Next(_L("Print the 12 names"));
    for (TInt jj=0; jj<12; jj++)
        test.Printf((*aDescA)[jj]);
    test.Printf(_L("\r\n"));
    test.Next(_L("Find"));
    for (TInt kk=3; kk<12; kk+=3)
        {
        name.Set((TMonth)kk);
        TInt pos=0;
        aDescA->Find(name,pos);
        test(pos==kk);
        }
    test.Next(_L("Delete"));
    aDescA->Delete(2);
    aDescA->Delete(4);
    aDescA->Delete(6);
    test(aDescA->Count()==9);
    test((*aDescA)[6]==name);
    test.Next(_L("Sort"));
    aDescA->Sort();
    test(aDescA->Count()==9);
    for (TInt ll=0; ll<9; ll++)
        test.Printf((*aDescA)[ll]);
    test.Printf(_L("\r\n"));
	test.End();
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0410
@SYMTestCaseDesc        Comparision of two CDesCArray objects test
@SYMTestPriority        Medium
@SYMTestActions         Check if two descriptor arrays are equal.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestDesca::ComparisonTest(CDesCArray* aDescA1,CDesCArray* aDescA2)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0410 Test comparisons "));
    TInt count=aDescA1->Count();
    test(aDescA2->Count()==count);
    for (TInt ii=0; ii<count; ii++)
        test((*aDescA1)[ii]==(*aDescA2)[ii]);
    test.End();
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0411
@SYMTestCaseDesc        MDesCArray class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Checks for the existence of 12 names
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestDesca::DoMixableTest(MDesCArray* aDescA)
	{
    test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0411 Check 12 names exist "));
    test(aDescA->MdcaCount()==12);
    test.Next(_L("Print the 12 names"));
    for (TInt jj=0; jj<12; jj++)
        test.Printf(aDescA->MdcaPoint(jj));
    test.Printf(_L("\r\n"));
	delete(aDescA);
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0412
@SYMTestCaseDesc        Tests for CDesCArray sequences
@SYMTestPriority        Medium
@SYMTestActions         Checks for the sequence of the literals.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestDesca::InSequenceTestL(CDesCArray* aDescA)
	{
    test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0412 In Sequence Tests "));
	aDescA->Reset();
	TPtrC hello(_S("Hello"));
	TPtrC bye(_S("Bye"));
	TPtrC world(_S("World"));
	TPtrC middle(_S("middle"));
	TPtrC extra(_S("extra"));
	TBuf<32> buffer;
	TInt pos;
	aDescA->AppendL(hello);
	aDescA->AppendL(bye);
	aDescA->AppendL(world);
	aDescA->AppendL(middle);
    test(aDescA->MdcaPoint(0)==hello);
    test(aDescA->MdcaPoint(2)==world);
	//
	aDescA->Sort(ECmpFolded);
	buffer=aDescA->MdcaPoint(2);
    test(buffer==middle);
	test(aDescA->Find(middle,pos,ECmpFolded)==KErrNone);
	test(pos==2);
	test(aDescA->FindIsq(middle,pos,ECmpFolded)==KErrNone);
	test(pos==2);
	test(aDescA->FindIsq(middle,pos,ECmpNormal)==KErrNone); // !! should this fail for non-sorted array??
//	test(pos==??);
	pos=aDescA->InsertIsqL(extra,ECmpFolded);
	test(pos==1);
	TRAP(pos,aDescA->InsertIsqL(extra,ECmpFolded));
	test(pos==KErrAlreadyExists);
	pos=aDescA->InsertIsqAllowDuplicatesL(extra,ECmpFolded);
	test(pos==2);
	aDescA->Delete(pos--);
	aDescA->Delete(pos);
	test(aDescA->MdcaCount()==4);
	//
	//	Different sort order because of case
	aDescA->Sort(ECmpNormal);
	buffer=aDescA->MdcaPoint(2);
    test(buffer==world);
	test(aDescA->Find(world,pos,ECmpNormal)==KErrNone);
	test(pos==2);
	test(aDescA->FindIsq(world,pos,ECmpNormal)==KErrNone);
	test(pos==2);
    }

class CMonthNameArray : public CArrayFixFlat<TMonthName>, public MDesCArray
	{
public:
	CMonthNameArray() :CArrayFixFlat<TMonthName>(4) { }
private:
	TInt MdcaCount() const { return(Count()); }
	TPtrC MdcaPoint(TInt aIndex) const { return((*this)[aIndex]); }
	};

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0413
@SYMTestCaseDesc        Tests for mixability
@SYMTestPriority        Medium
@SYMTestActions         Tests the mix of setting and appending of month names to the descriptor array
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestDesca::MixableTestL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0413 Test mixability "));
	CMonthNameArray* array=new(ELeave) CMonthNameArray;
    TMonthName name;
    test.Next(_L("Add 12 month names"));
    for (TInt ii=0; ii<12; ii++)
        {
        name.Set((TMonth)ii);
        array->AppendL(name);
        }
	DoMixableTest(array);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0414
@SYMTestCaseDesc        Descriptor arrays tests
@SYMTestPriority        Medium
@SYMTestActions         Executes all the CDesCArray tests
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void DoTestsL()
    {
	TestDesca t;
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0414 Test CDesCArrayFlat "));
    CDesCArrayFlat* descFlatArray=new(ELeave) CDesCArrayFlat(4);
	TRAPD(errCode, t.FoldTestL(descFlatArray));
	test(errCode==KErrNone);
	TRAP(errCode, t.MainTestL(descFlatArray));
	test(errCode==KErrNone);
	test.Start(_L("Test CDesCArraySeg"));
    CDesCArraySeg* descSegArray=new(ELeave) CDesCArraySeg(3);
	TRAP(errCode, t.FoldTestL(descSegArray));
	test(errCode==KErrNone);
	TRAP(errCode, t.MainTestL(descSegArray));
	test(errCode==KErrNone);
    t.ComparisonTest(descFlatArray,descSegArray);
	TRAP(errCode, t.InSequenceTestL(descSegArray));
	test(errCode==KErrNone);
	TRAP(errCode, t.InSequenceTestL(descFlatArray));
	test(errCode==KErrNone);
    delete(descFlatArray);
    delete(descSegArray);
	t.MixableTestL();
    }

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	test.Title();
	test.Start(_L(" Testing descriptor arrays "));
    TRAPD(err,DoTestsL());
    test(err==KErrNone);
	test.End();
    test.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }
