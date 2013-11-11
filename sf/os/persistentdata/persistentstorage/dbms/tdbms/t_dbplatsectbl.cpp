// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS security policy - testing new APIs - table level.
// All tests assume that drive C is presented in the system and is not a ROM drive.
// 
//

#include "t_dbplatsecutl.h"
#include "t_dbplatsecdef.h"
#include "t_dbplatsectbl.h"

static TColDef const KColumns[]=
	{
	{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
	{_S("DATA1"), EDbColInt32, TDbCol::ENotNull},
	{_S("DATA2"), EDbColInt32, TDbCol::ENotNull},
	{0}
	};

static TColDef const KColumns2[]=
	{
	{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
	{_S("DATA1"), EDbColLongBinary},
	{0}
	};

static void TblCallTestL()
	{
	TheTest.Next(_L("Open()"));	
	TInt err = TheTbl1.Open(TheDb1, KTableName1);
	TEST2(err, KErrNone);
	
	TheTest.Next(_L("ColSetL()"));	
	CDbColSet* colset = TheTbl1.ColSetL();
	TInt cnt = colset->Count();
	TInt i;
	for(i=0;i<cnt;++i)
		{
		const TDbCol& dbc = (*colset)[i + 1];
		TheTest.Printf(_L("%02d, %S\n"), i + 1, &dbc.iName);
		}
	delete colset;
	
	TheTest.Next(_L("ColDef()"));	
	TDbCol dbcol = TheTbl1.ColDef(1);
	TheTest.Printf(_L("%S\n"), &dbcol.iName);

	TheTest.Next(_L("InsertL()/SetColL()/PutL()"));	
	TheTbl1.InsertL();
	TheTbl1.SetColL(2, 1);
	TheTbl1.SetColL(3, 2);
	TheTbl1.PutL();
	TheTbl1.InsertL();
	TheTbl1.SetColL(2, 3);
	TheTbl1.SetColL(3, 4);
	TheTbl1.PutL();
	
	TheTest.Next(_L("AtRow()/AtBeginning()/AtEnd()"));	
	(void)TheTbl1.AtRow();
	(void)TheTbl1.AtBeginning();
	(void)TheTbl1.AtEnd();

	TheTest.Next(_L("BeginningL()/EndL()"));	
	TheTbl1.BeginningL();
	TheTbl1.EndL();

	TheTest.Next(_L("FirstL()/LastL()/PreviousL()/PreviousL()"));	
	TBool res = TheTbl1.FirstL();
	TEST(res);
	TheTbl1.GetL();
	TInt32 val1 = TheTbl1.ColInt32(1);
	TInt32 val2 = TheTbl1.ColInt32(2);
	TInt32 val3 = TheTbl1.ColInt32(3);
	TEST(val1 == 0);
	TEST(val2 == 1);
	TEST(val3 == 2);
	
	res = TheTbl1.LastL();
	TEST(res);
	TheTbl1.GetL();
	val1 = TheTbl1.ColInt32(1);
	val2 = TheTbl1.ColInt32(2);
	val3 = TheTbl1.ColInt32(3);
	TEST(val1 == 1);
	TEST(val2 == 3);
	TEST(val3 == 4);

	res = TheTbl1.PreviousL();
	TEST(res);
	TheTbl1.GetL();
	val1 = TheTbl1.ColInt32(1);
	val2 = TheTbl1.ColInt32(2);
	val3 = TheTbl1.ColInt32(3);
	TEST(val1 == 0);
	TEST(val2 == 1);
	TEST(val3 == 2);
	
	res = TheTbl1.NextL();
	TEST(res);
	TheTbl1.GetL();
	val1 = TheTbl1.ColInt32(1);
	val2 = TheTbl1.ColInt32(2);
	val3 = TheTbl1.ColInt32(3);
	TEST(val1 == 1);
	TEST(val2 == 3);
	TEST(val3 == 4);

	TheTest.Next(_L("UpdateL()"));	
	TheTbl1.UpdateL();
	TheTbl1.SetColL(2, 33);
	TheTbl1.SetColL(3, 44);
	TheTbl1.PutL();

	TheTest.Next(_L("Cancel()"));	
	TheTbl1.UpdateL();
	TheTbl1.SetColL(2, 36);
	TheTbl1.SetColL(3, 47);
	TheTbl1.Cancel();

	TheTest.Next(_L("DeleteL()"));	
	TheTbl1.DeleteL();

	TheTest.Next(_L("IsColNull()"));	
	CDbColSet* colset2 = TDBSCUtils::CreateColSetLC(KColumns2);
	_LIT(KTempTblName, "TempTbl");
	err = TheDb1.CreateTable(KTempTblName, *colset2);
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(colset2);

	err = TheTbl2.Open(TheDb1, KTempTblName);
	TEST2(err, KErrNone);

	TheTbl2.InsertL();
	_LIT8(KTestData, "");
	TheTbl2.SetColL(2, KTestData);
	TheTbl2.PutL();
	
	TheTbl2.Close();
	err = TheTbl2.Open(TheDb1, KTempTblName);
	TEST2(err, KErrNone);
	res = TheTbl2.FirstL();
	TEST(res);
	TheTbl2.GetL();
	res = TheTbl2.IsColNull(2);
	TEST(res);
	TheTbl2.Close();
	
	TheTest.Next(_L("ColSize()"));	
	res = TheTbl1.FirstL();
	TEST(res);
	TheTbl1.GetL();
	res = TheTbl1.ColSize(1);
	TEST(res);
		
	TheTest.Next(_L("ColLength()"));	
	TInt len = TheTbl1.ColLength(1);
	TEST(len > 0);

	TheTbl1.InsertL();
	TheTbl1.SetColL(2, 3);
	TheTbl1.SetColL(3, 4);
	TheTbl1.PutL();

	TheTest.Next(_L("GotoL(TPosition)"));	
	res = TheTbl1.GotoL(RDbRowSet::EFirst);
	TEST(res);
	
	TheTest.Next(_L("Bookmark()/GotoL(TDbBookmark)"));	
	TDbBookmark bkmk = TheTbl1.Bookmark();
	res = TheTbl1.NextL();
	TEST(res);
	TheTbl1.GotoL(bkmk);
	TheTbl1.GetL();
	val1 = TheTbl1.ColInt32(1);
	TEST(val1 == 0);
	
	TheTest.Next(_L("CountL()/IsEmptyL()"));	
	cnt = TheTbl1.CountL();
	TEST(cnt == 2);
	res = TheTbl1.IsEmptyL();
	TEST(!res);
	
	TheTest.Next(_L("MatchL()"));	
	RDbRowConstraint match;
	CleanupClosePushL(match);
	err = match.Open(TheTbl1, TDbQuery(_L("ID > 0")));
	TEST2(err, KErrNone);
	res = EFalse;
	TheTbl1.BeginningL();
	while(TheTbl1.NextL())
		{
		if(TheTbl1.MatchL(match))
			{
			res = ETrue;
			}
		}
	CleanupStack::PopAndDestroy(&match);
	TEST(res);

	TheTest.Next(_L("FindL()"));	
	res = TheTbl1.FirstL();
	TEST(res);
	err = TheTbl1.FindL(RDbRowSet::EForwards, TDbQuery(_L("ID <> 0")));
	TEST(err >= 0);
	TheTbl1.GetL();
	val1 = TheTbl1.ColInt32(1);
	TEST(val1 > 0);

	_LIT8(KTestData2, "0123456789");
	HBufC8* buf = HBufC8::NewLC(10000);
	TPtr8 ptr = buf->Des(); 
	for(i=0;i<1000;++i)
		{
		ptr += KTestData2();
		}
	
	TheTest.Next(_L("RDbColReadStream"));	
	err = TheTbl2.Open(TheDb1, KTempTblName);
	TEST2(err, KErrNone);

	TheTbl2.InsertL();
	TheTbl2.SetColL(2, *buf);
	TheTbl2.PutL();
	
	TheTbl2.InsertL();
	TheTbl2.SetColL(2, KTestData2);
	TheTbl2.PutL();
	
	res = TheTbl2.PreviousL();
	TEST(res);
	TheTbl2.GetL();
	RDbColReadStream rstream;
	rstream.OpenLC(TheTbl2, 2);
	ptr.Zero();
	rstream.ReadL(ptr, ptr.MaxLength());
	CleanupStack::PopAndDestroy(&rstream);
	TEST(ptr.Length() == ptr.MaxLength());

	TheTest.Next(_L("RDbColWriteStream"));	
	TheTbl2.InsertL();
	RDbColWriteStream wstream;
	wstream.OpenLC(TheTbl2, 2);
	wstream.WriteL(ptr, ptr.Length());
	wstream.CommitL();
	CleanupStack::PopAndDestroy(&wstream);
	TheTbl2.PutL();
	
	TheTbl2.Close();
	CleanupStack::PopAndDestroy(buf);
			
	TheTbl1.Close();
	}

static void ViewTestL()
	{
	RDbView view;
	CleanupClosePushL(view);
	
	TheTest.Next(_L("Prepare()"));	
	TInt err = view.Prepare(TheDb1, TDbQuery(_L("SELECT * FROM ATbl")));
	TEST2(err, KErrNone);
	
	TheTest.Next(_L("Update()"));	
	TBool res = view.FirstL();
	TEST(res);
	view.GetL();
	view.UpdateL();
	view.SetColL(2, 100);
	view.SetColL(3, 200);
	view.PutL();
	
	CleanupStack::PopAndDestroy(&view);

	TheTest.Next(_L("Prepare() with an update sql"));	
	CleanupClosePushL(view);
	err = view.Prepare(TheDb1, TDbQuery(_L("UPDATE ATbl SET DATA1 = 400 WHERE ID = 0")));
	TEST2(err, KErrArgument);
	CleanupStack::PopAndDestroy(&view);
	}

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0021
@SYMTestCaseDesc RDbTable method calls test for a secure shared database.
                 Every method of RDbTable class and its base class too, is called
				 and the result - asserted.
@SYMTestPriority High
@SYMTestActions  RDbTable method calls test.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
void DoTblTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID SYSLIB-DBMS-CT-0021 "));
	
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	
	TheDb1.Close();
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);

	CDbColSet* colset = TDBSCUtils::CreateColSetLC(KColumns);
	TInt err = TheDb1.CreateTable(KTableName1, *colset);
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(colset);

	TheTest.Next(_L("Table calls test"));	
	::TblCallTestL();
	
	TheTest.Next(_L("View test"));	
	::ViewTestL();
	
	TheDb1.Close();
	}
