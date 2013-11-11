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
// Written by Martin, May 1996
// 
//


#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <s32mem.h>
#include <bacell.h>

LOCAL_D RTest test(_L("T_CELL"));

class Test
	{
public:
	static void Test1();
	static void Test2();
	static void Test3();
	static void Test4L();
	};

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0400
@SYMTestCaseDesc        Tests the functionality of TCellRef::operator ==() and !=()
@SYMTestPriority        High
@SYMTestActions         Test the equality of two cells defined by TCellRef's constructor
                        Test the equality after changing the offsets of the cells already created
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Test::Test1()
	{
	__UHEAP_MARK;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0400 Cells "));
	TCellRef c1(0,0);
	TCellRef c2(0,0);
	TCellRef c3(1,1);
	TCellRef c4 = c3;
	TCellRef c5(100,100);

	test(c1==c2);
	test(c3==c4);
	test(c1!=c3);

	c4 = c5;
	test(c4==c5);

	c2.Offset(1,1);
	test(c2==c3);
	c2.Offset(99,99);
	test(c2==c5);

	c2 = c2 - TCellRef(48,40);
	test(c2==TCellRef(52,60));
	c2 = c2 + TCellRef(100,100);
	test(c2==TCellRef(152,160));

	test.End();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0401
@SYMTestCaseDesc        Tests the functionality of TRangeRef::NoRows(),NoCols(),NoCells(),Contains() functions
@SYMTestPriority        Medium
@SYMTestActions         Test the equality of two objects defined by TRangeRef's
                        Test the no of rows & columns after a range is defined
						Test the existence of a cell after a range is defined.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Test::Test2()
	{
	__UHEAP_MARK;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0401 Ranges "));

	TRangeRef r1(TCellRef(2,3),TCellRef(3,5));
	TRangeRef r2(2,3,3,5);
	TRangeRef r3(2,4,3,6);
	TRangeRef r5(1,0,19,20);

	test(r1==r2);
	test(r2!=r3);
	r2 = r3;
	test(r2==r3);

	r2.SetRange(2,3,3,5);
	test(r2==r1);
	r2.SetRange(TCellRef(1,0),TCellRef(19,20));
	test(r2==r5);

	{test(TRangeRef(TCellRef(2,3),TCellRef(3,5)).NoRows()==2);}
	{test(TRangeRef(TCellRef(2,3),TCellRef(3,5)).NoCols()==3);}
	{test(TRangeRef(TCellRef(2,3),TCellRef(3,5)).NoCells()==6);}

	{test(TRangeRef(TCellRef(1,7),TCellRef(10,20)).NoRows()==10);}
	{test(TRangeRef(TCellRef(1,7),TCellRef(10,20)).NoCols()==14);}
	{test(TRangeRef(TCellRef(1,7),TCellRef(10,20)).NoCells()==140);}

	{test(TRangeRef(TCellRef(2,3),TCellRef(3,5)).Contains(TCellRef(3,3)));}
	{test(TRangeRef(TCellRef(2,3),TCellRef(3,5)).Contains(TCellRef(2,3)));}
	{test(TRangeRef(TCellRef(2,3),TCellRef(3,5)).Contains(TCellRef(3,5)));}
	{test(!TRangeRef(TCellRef(2,3),TCellRef(3,5)).Contains(TCellRef(1,3)));}
	{test(!TRangeRef(TCellRef(2,3),TCellRef(3,5)).Contains(TCellRef(3,6)));}

	test.End();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0402
@SYMTestCaseDesc        TRangeRef class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Test range of cells defined by TRangeRef by iteration and checking the count
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void Test::Test3()
	{
	__UHEAP_MARK;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0402 Range iterator "));

	{TRangeRef range(TCellRef(2,3),TCellRef(10,24));
	TRangeRef::TIter iter(range);
	test(iter.iCurrent==range.iFrom);
	TInt count = 0;
	while (iter.InRange())
		{
		++iter;
		++count;
		}
	test(count==range.NoCells());
	test(iter.iCurrent==TCellRef(range.iTo.iRow+1,range.iFrom.iCol));}

	{TRangeRef range(TCellRef(2,3),TCellRef(2,3));
	TRangeRef::TIter iter(range);
	test(iter.iCurrent==range.iFrom);
	TInt count = 0;
	while (iter.InRange())
		{
		++iter;
		++count;
		}
	test(count==1);}


	{TRangeRef range(TCellRef(2,3),TCellRef(2,-3));
	TRangeRef::TIter iter(range);
	test(iter.iCurrent==range.iFrom);
	TInt count = 0;
	while (iter.InRange())
		{
		++iter;
		++count;
		}
	test(count==0);}

	test.End();
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0403
@SYMTestCaseDesc        Tests for RDesWriteStream and RDesReadStream classes
@SYMTestPriority        Medium
@SYMTestActions         Test the cell's info by writing and reading back from stream.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void Test::Test4L()
	{
	__UHEAP_MARK;
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0403 Streaming "));

	TBuf8<64> buf;
	RDesWriteStream writeStream(buf);
	TCellRef cell1(2,5);
	TCellRef cell2(6,8);
	TRangeRef range1(12,13,2,5);
	writeStream << cell1;
	writeStream << cell2;
	writeStream.WriteInt32L(KMaxTInt32);
	writeStream.WriteInt32L(KMinTInt32);
	writeStream << range1;
	writeStream.Close();

	RDesReadStream readStream(buf);
	readStream >> range1;
	test(range1==TRangeRef(cell1,cell2));
	readStream >> range1;
	test(range1.iFrom==TCellRef(KMaxTInt32,KMinTInt32));
	test(range1.iTo.iRow==12);
	test(range1.iTo.iCol==13);
	readStream >> cell2;
	test(cell1==cell2);
	readStream.Close();

	test.End();
	__UHEAP_MARKEND;
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();

	Test::Test1();
	Test::Test2();
	Test::Test3();
	Test::Test4L();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return 0;
	}
