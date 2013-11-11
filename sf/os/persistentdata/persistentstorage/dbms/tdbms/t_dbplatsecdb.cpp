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
// DBMS security policy - testing new APIs - database level.
// All tests assume that drive C is presented in the system and is not a ROM drive.
// 
//

#include "t_dbplatsecutl.h"
#include "t_dbplatsecdef.h"
#include "t_dbplatsecdb.h"

static TColDef const KColumns[]=
	{
	{_S("ID"), EDbColInt32, TDbCol::ENotNull | TDbCol::EAutoIncrement},
	{_S("DATA1"), EDbColInt32, TDbCol::ENotNull},
	{_S("DATA2"), EDbColInt32, TDbCol::ENotNull},
	{0}
	};


//Open database tests
static void OpenTestL()
	{
	RFs fileSess;
	CleanupClosePushL(fileSess);
	TEST2(fileSess.Connect(), KErrNone);
	
	TBuf<32> fmt;
	fmt.Copy(KSecure);
	fmt.Append(KSecureDbUid.Name());
	
	TheTest.Printf(_L("Open secure shared database\n"));
	TInt err = TheDb1.Open(TheDbs, KDb1Name, fmt);
	TheDb1.Close();
	TEST2(err, KErrNone);
	

	TheTest.Printf(_L("An attempt to open it as a secure shared database - no format string\n"));
	err = TheDb1.Open(TheDbs, KDb1Name);
	TheDb1.Close();
	TEST2(err, KErrNotFound);//Treated as a non-secure, not found database.

	TheTest.Printf(_L("An attempt to open it as a non-secure shared database\n"));
	_LIT(KNSDbName, "C:\\PRiVatE\\100012A5\\A.dB");
	err = TheDb1.Open(TheDbs, KNSDbName);
	TheDb1.Close();
	TEST2(err, KErrPermissionDenied);
    _LIT(KNSDbName2, "C:\\PRiVatE\\100012A5\\DBS_11335577_A.Db");
    if(PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) && 
       PlatSec::IsCapabilityEnforced(ECapabilityAllFiles))
        {
	    err = TheDb1.Open(fileSess, KNSDbName2);
	    TheDb1.Close();
	    TEST2(err, KErrPermissionDenied);

		TheTest.Printf(_L("An attempt to create it as a non-secure shared database\n"));
		err = TheDb1.Create(fileSess, KNSDbName2);
		TheDb1.Close();
		TEST2(err, KErrPermissionDenied);
        }

	TheTest.Printf(_L("--An attempt to open a database, which security policy cannot be satisfied by the current test application\n"));	
	fmt.Copy(KSecure);
	fmt.Append(KSecureDbUid2.Name());
	err = TheDb1.Open(TheDbs, KProtDb3CName, fmt);
	TheDb1.Close();
	TEST2(err, KErrPermissionDenied);
	
	CleanupStack::PopAndDestroy(&fileSess);
	}

//RDbDatabase method calls test. All calls have to pass with KErrNone,
//because the test application has enough rights to do any R/W/S database operation.
static void DbCallTestL()
	{
	TBuf<32> fmt;
	fmt.Copy(KSecure);
	fmt.Append(KSecureDbUid.Name());
	
	TheTest.Printf(_L("Open()\n"));
	TInt err = TheDb1.Open(TheDbs, KDb1Name, fmt);
	
	TheTest.Printf(_L("IsDamaged()\n"));
	TBool res = TheDb1.IsDamaged();
	TEST(!res);
	
	TheTest.Printf(_L("Recover()\n"));
	err = TheDb1.Recover();
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("UpdateStats()\n"));
	err = TheDb1.UpdateStats();
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("Size()\n"));
	RDbDatabase::TSize size = TheDb1.Size();
	
	TheTest.Printf(_L("Compact()\n"));
	err = TheDb1.Compact();
	TEST2(err, KErrNone);

	
	TheTest.Printf(_L("Begin()/Commit()\n"));
	err = TheDb1.Begin();
	TEST2(err, KErrNone);
	res = TheDb1.InTransaction();
	TEST(res);
	err = TheDb1.Commit();
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("Begin()/Rollback()\n"));
	err = TheDb1.Begin();
	TEST2(err, KErrNone);
	res = TheDb1.InTransaction();
	TEST(res);
	TheDb1.Rollback();
	
	TheTest.Printf(_L("CreateTable()\n"));
	CDbColSet* colset = TDBSCUtils::CreateColSetLC(KColumns);
	_LIT(KTblName1, "A1");
	err = TheDb1.CreateTable(KTblName1, *colset);
	TEST2(err, KErrNone);
	_LIT(KTblName2, "A2");
	err = TheDb1.CreateTable(KTblName2, *colset);
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("DropTable()\n"));
	err = TheDb1.DropTable(KTblName2);
	TEST2(err, KErrNone);

	TheTest.Printf(_L("AlterTable()\n"));
	err = TheDb1.AlterTable(KTblName1, *colset);
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("CreateIndex()\n"));
	CDbKey* key = TDBSCUtils::CreateKeyLC(KCol2);
	err = TheDb1.CreateIndex(KCol2, KTblName1, *key);
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("DropIndex()\n"));
	err = TheDb1.DropIndex(KCol2, KTblName1);
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("TableNamesL()\n"));
	_LIT(KTblName3, "DdD");
	_LIT(KTblName4, "FFf");
	_LIT(KTblName5, "Ggg");
	TDBSCUtils::CreateTableL(TheDb1, KTblName3, KColumns);
	TDBSCUtils::CreateTableL(TheDb1, KTblName4, KColumns);
	TDBSCUtils::CreateTableL(TheDb1, KTblName5, KColumns);
	CDbTableNames* tblNames = TheDb1.TableNamesL();
	TInt cnt = tblNames->Count();
	TEST(cnt == 4);
	TInt i;
	for(i=0;i<cnt;++i)
		{
		const TDesC& name = (*tblNames)[i];
		TheTest.Printf(_L("%02d, %S\n"), i + 1, &name);
		}
	delete tblNames;
		
	TheTest.Printf(_L("ColSetL()\n"));
	CDbColSet* cs1 = TheDb1.ColSetL(KTblName3);
	cnt = cs1->Count();
	for(i=0;i<cnt;++i)
		{
		const TDbCol& dbc = (*cs1)[i + 1];
		TheTest.Printf(_L("%02d, %S\n"), i + 1, &dbc.iName);
		}
	delete cs1;
	
	TheTest.Printf(_L("IndexNamesL()\n"));
	CDbKey* tblKey1 = TDBSCUtils::CreateKeyLC(KCol2);
	err = TheDb1.CreateIndex(KCol2, KTblName1, *tblKey1);
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(tblKey1);
	CDbIndexNames* dbiNames = TheDb1.IndexNamesL(KTblName1);
	cnt = dbiNames->Count();
	for(i=0;i<cnt;++i)
		{
		const TDesC& name = (*dbiNames)[i];
		TheTest.Printf(_L("%02d, %S\n"), i + 1, &name);
		}
	delete dbiNames;
	
	TheTest.Printf(_L("KeyL()\n"));
	CDbKey* tblKey2 = TheDb1.KeyL(KCol2, KTblName1);
	delete tblKey2;
		
	CleanupStack::PopAndDestroy(key);
	CleanupStack::PopAndDestroy(colset);
	TheDb1.Close();
	}

static void SQLTestL()
	{
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1.Close();
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	
	TheTest.Printf(_L("Create table\n"));
	TInt err = TheDb1.Execute(_L("crEAte tabLe a (id couNter)"));
	TEST2(err, KErrNone);
	
	TheTest.Printf(_L("Alter table\n"));
	err = TheDb1.Execute(_L("ALter TablE a aDd DaTA1 CHaR(20)"));
	TEST2(err, KErrNone);
	
	err = TheDb1.Execute(_L("alter table A add DATA2 INTEGER"));
	TEST2(err, KErrNone);
	
	err = TheDb1.Execute(_L("ALTEr tABLe A dROp DaTa1"));
	TEST2(err, KErrNone);

	TheTest.Printf(_L("Drop table\n"));
	err = TheDb1.Execute(_L("DRop tABle a"));
	TEST2(err, KErrNone);

	err = TheDb1.Execute(_L("crEAte tabLe a (iD coUNteR)"));
	TEST2(err, KErrNone);
	err = TheDb1.Execute(_L("altER taBLe A add daTA2 InteGER"));
	TEST2(err, KErrNone);
	err = TheDb1.Execute(_L("creaTE tAblE b (Id counTEr)"));
	TEST2(err, KErrNone);
	err = TheDb1.Execute(_L("alter TAbLe B aDd DAtA2 INTEgER"));
	TEST2(err, KErrNone);

	TheTest.Printf(_L("Create index\n"));
	err = TheDb1.Execute(_L("CREAte INdeX idX oN A(dATA2)"));
	TEST2(err, KErrNone);

	TheTest.Printf(_L("Drop index\n"));
	err = TheDb1.Execute(_L("DroP IndEX idx frOm a"));
	TEST2(err, KErrNone);

	TheTest.Printf(_L("Insert\n"));
	TInt cnt = TheDb1.Execute(_L("INSerT IntO A (DATa2) VALuES(45)"));
	TEST(cnt > 0);

	TheTest.Printf(_L("Update\n"));
	cnt = TheDb1.Execute(_L("UPDATe A SeT DAtA2=56 WheRE iD = 0"));
	TEST(cnt > 0);

	TheTest.Printf(_L("Delete\n"));
	cnt = TheDb1.Execute(_L("INSErt INtO B (DATA2) VALUES(100)"));
	TEST(cnt > 0);
	cnt = TheDb1.Execute(_L("INSERT INTo b (DATA2) VALUES(200)"));
	TEST(cnt > 0);
	cnt = TheDb1.Execute(_L("delete FROM B where ID = 1"));
	TEST(cnt > 0);

	TheDb1.Close();
	}

static void TwoDatabasesTestL()
    {
	TBuf<32> fmt;
	fmt.Copy(KSecure);
	fmt.Append(KSecureDbUid.Name());
	
    RDbNamedDatabase db1;
	TheTest.Printf(_L("Open secure shared database-1\n"));
	TInt err = db1.Open(TheDbs, KDb1Name, fmt);
	TEST2(err, KErrNone);

    RDbNamedDatabase db2;
	TheTest.Printf(_L("Open secure shared database-2\n"));
	err = db2.Open(TheDbs, KDb1Name, fmt);
	TEST2(err, KErrNone);

    db2.Close();
    db1.Close();
    }

/**
@SYMTestCaseID SYSLIB-DBMS-CT-0019
@SYMTestCaseDesc RDbNamedDatabase method calls test for a secure shared database.
                 Every method of RDbNamedDatabase class and its base class too, is called
				 and the result - asserted.
@SYMTestPriority High
@SYMTestActions  RDbNamedDatabase method calls test.
@SYMTestExpectedResults The test must not fail.
@SYMREQ          REQ2429
                 DBMS shall provide an API to apply security policies to database tables.
*/
void DoDbTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0019 "));	
	TDBSCUtils::DeleteDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1 = TDBSCUtils::CreateDatabase(TheDbs, KSecureDbUid, KDb1Name);
	TheDb1.Close();

	TheTest.Next(_L("Open secure database test"));	
	::OpenTestL();
	
	TheTest.Next(_L("Secure Database calls test"));	
	::DbCallTestL();

	TheTest.Next(_L("SQL test"));	
	::SQLTestL();

	TheTest.Next(_L("Two databases test"));	
	::TwoDatabasesTestL();
	}
