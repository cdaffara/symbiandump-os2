// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Testing DBMS ordering and searching functionality when the key field is unicode string.
// The idea is to verify that collation level 0 is used when doing string based searching 
// and collation level > 0 is used when doing string based ordering.
// 
//

#include <e32test.h>
#include <f32file.h>
#include <d32dbms.h>

#include "crccheck.h"

#undef __UHEAP_MARK
#define __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_DBMSSTRCOMP.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_DBMSSTRCOMP.CRC");
#endif
#endif

/////////////////////////////////////////////////////////////////
//Globals

#ifdef __TOOLS2__
const TPtrC KTestDatabase = _L(".\\dbms-tst\\T_DbmsStrComp.db");
#else
const TPtrC KTestDatabase = _L("c:\\dbms-tst\\T_DbmsStrComp.db");
#endif

const TUint             KExitDelay = 6 * 1000000;//6 seconds

static RTest			TheTest(_L("T_DbmsStrComp test"));
static RFs				TheFs;
static RDbNamedDatabase TheDb;
static RDbs				TheDbSession;

//Test table defs
_LIT(KTestTableName1, "TABLE1");//EDbColText16 key field
_LIT(KTestTableName2, "TABLE2");//EDbColLongText16 key field

struct TColDef
	{
	const TText*	iName;
	TDbColType		iType;
	TInt			iAttributes;
	};
static TColDef const KColDefs1[]=
	{
		{_S("ID"), EDbColText16, 0},
		{_S("DATA"), EDbColUint32, 0},
		{0}
	};
static TColDef const KColDefs2[]=
	{
		{_S("ID"), EDbColLongText16, 0},
		{_S("DATA"), EDbColUint32, 0},
		{0}
	};

//Test strings
const TInt KTestStrLen = 3; //The length of test strings
typedef TBuf16<KTestStrLen> TNameBuf;
//Test strings array - using upper and lower case - which will force the DBMS server to make
//different decisions depending on what is the current case: ordering or searching.
const TNameBuf KTestStr[] = 
	{
	_L16("aaa"),
	_L16("aAa"),
	_L16("bbB"),
	_L16("BbB")
	};
const TInt KTestStrCnt = sizeof(KTestStr) / sizeof(KTestStr[0]);

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Destroy test environment - global functions

//Deletes "aFullName" file.
static TInt DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		err = fsSession.Entry(aFullName, entry);
		if(err == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	return err;
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteDataFile(KTestDatabase);
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		::DeleteDataFile(KTestDatabase);
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Global functions

//Prepares the test directory.
//TheFs.Connect() has to be called already.
static void SetupTestDirectory()
    {
	TInt err = TheFs.MkDir(KTestDatabase);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	}

//Leaves with info message printed out
static void Leave(TInt aError, TInt aLine)
	{
	RDebug::Print(_L("*** Leave. Error: %d, Line: %d\r\n"), aError, aLine);
	User::Leave(aError);
	}

//Leaves if aError < 0 with info message printed out
static void LeaveIfError(TInt aError, TInt aLine)
	{
	if(aError < KErrNone)
		{
		::Leave(aError, aLine);
		}
	}

//Use LEAVE() macro instead of User::Leave() and LEAVE_IF_ERROR() macro instead of
//User::LeaveIfError(). They will print the line number, where the "leave" was called.
#define LEAVE(aError) ::Leave(aError, __LINE__)
#define LEAVE_IF_ERROR(aError) ::LeaveIfError(aError, __LINE__)

//Creates the test DBMS session
static void CreateTestDbSession()
	{
#ifndef __TOOLS2__
	RDebug::Print(_L("Create DBMS session\n"));
	TInt err = TheDbSession.Connect();
	TEST2(err, KErrNone);
#endif
	}


//Creates the test database
//TheDbSession instance has to be connected already.
//TheFs.Connect() has to be called already.
static void CreateTestDatabase(RDbs& aDbs, RDbNamedDatabase& aDb)
	{
	RDebug::Print(_L("Create test database\n"));
	TInt err = aDb.Replace(TheFs, KTestDatabase);
	TEST2(err, KErrNone);
	TheDb.Close();
	err = aDb.Open(aDbs, KTestDatabase);
	TEST2(err, KErrNone);
	}

//Creates test table
static void DoCreateTestTableL(RDbNamedDatabase& aDb, const TDesC& aTblName, const TColDef aColDefs[])
	{
	CDbColSet* colSet = CDbColSet::NewLC();
	for(const TColDef* colDef=aColDefs;colDef->iName;++colDef)
		{
		TDbCol col(TPtrC(colDef->iName), colDef->iType);
		col.iAttributes = colDef->iAttributes;
		colSet->AddL(col);
		}
	TEST2(aDb.CreateTable(aTblName, *colSet), KErrNone);
	CleanupStack::PopAndDestroy(colSet);
	}

//Creates test tables
static void CreateTestTablesL(RDbNamedDatabase& aDb)
	{
	RDebug::Print(_L("Create test tables\n"));
	::DoCreateTestTableL(aDb, KTestTableName1, KColDefs1);
	::DoCreateTestTableL(aDb, KTestTableName2, KColDefs2);
	}

//Gets the value of the string field, which type may be EDbColText16 or EDbColLongText16
void GetStrFieldValue(RDbRowSet& aTbl, const TDesC& aTblName, TDes& aStrFldVal)
	{
	if(aTblName.CompareF(KTestTableName1) == 0)
		{
		aStrFldVal = aTbl.ColDes16(1);
		}
	else
		{
		RDbColReadStream blob;
		blob.OpenLC(aTbl, 1);
		blob.ReadL(aStrFldVal, aTbl.ColLength(1));
		CleanupStack::PopAndDestroy();
		}
	}

//Prints all table records
static TInt PrintRecordsL(RDbRowSet& aTbl, const TDesC& aTblName)
	{
	RDebug::Print(_L("Table: %S\n"), &aTblName);
    aTbl.FirstL();
	TInt rec = 0;
    while(aTbl.AtRow())
        {
        aTbl.GetL();
		TNameBuf strFldVal;
		::GetStrFieldValue(aTbl, aTblName, strFldVal);
		TUint32 v = aTbl.ColUint32(2);
		RDebug::Print(_L("   Record %d, Str: %S, Val: %d\n"), ++rec, &strFldVal, v);
        aTbl.NextL(); 
        }
	return rec;
	}

//Checks if the records order (based on a string key field comparison) matches the order of the
//strings in aTestStrArray
static void AssertRecordsOrder(RDbRowSet& aTbl, const TDesC& aTblName, const RArray<TNameBuf>& aTestStrArray)
	{
    aTbl.FirstL();
	TInt rec = 0;
    while(aTbl.AtRow())
        {
        aTbl.GetL();
		TNameBuf strFldVal;
		::GetStrFieldValue(aTbl, aTblName, strFldVal);
		TEST(aTestStrArray[rec] == strFldVal);
		++rec;
        aTbl.NextL(); 
        }
	}

//Adds test data to the specified table. Make sure that the records are not in
//order (assuming that the first field will be the key).
static void AddTestDataL(RDbNamedDatabase& aDb, const TDesC& aTblName)
	{
	RDbTable tbl;
	CleanupClosePushL(tbl);
	TEST2(tbl.Open(aDb, aTblName, RDbRowSet::EUpdatable), KErrNone);
	for(TInt i=0;i<KTestStrCnt;++i)
		{
		tbl.InsertL();
		tbl.SetColL(1, KTestStr[KTestStrCnt - i - 1]);
		tbl.SetColL(2, i + 1);
		tbl.PutL();
		}
	TEST(tbl.CountL() == KTestStrCnt);
	(void)::PrintRecordsL(tbl, aTblName);
	CleanupStack::PopAndDestroy(&tbl);
	}

//Adds the test data to test tables
static void AddTestDataL(RDbNamedDatabase& aDb)
	{
	RDebug::Print(_L("Add data to test tables\n"));
	::AddTestDataL(aDb, KTestTableName1);
	::AddTestDataL(aDb, KTestTableName2);
	}

//Init test environment
static void InitEnvL()
	{
	::CreateTestDbSession();
    //Create test database and tables. Add some test data to them.
	::CreateTestDatabase(TheDbSession, TheDb);
	::CreateTestTablesL(TheDb);
	::AddTestDataL(TheDb);
	}

//String comparison function, used in FillStrArraySorted() function.
static TInt CompareC(const TNameBuf& aName1, const TNameBuf& aName2)
    {
    return aName1.CompareC(aName2);
    }

//Inserts all test string into an ordered array - aTestStrArray
static void FillStrArraySortedL(RArray<TNameBuf>& aTestStrArray)
	{
	for(TInt i=0;i<KTestStrCnt;++i)
		{
    	User::LeaveIfError(aTestStrArray.InsertInOrder(KTestStr[i], TLinearOrder<TNameBuf>(CompareC)));
		}
	}

static void CreateIndexL(RDbNamedDatabase& aDb, const TDesC& aTblName, const TDesC& aColumnName)
	{
	RDebug::Print(_L("Create index. Table: %S, column: %S\n"), &aTblName, &aColumnName);
	CDbKey* key = CDbKey::NewLC();
	key->AddL(aColumnName);
	key->MakeUnique();
	key->SetComparison(EDbCompareCollated);
	LEAVE_IF_ERROR(aDb.CreateIndex(aColumnName, aTblName, *key));
	CleanupStack::PopAndDestroy(key);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test cases

//Test case 1. Check SELECT statement with ORDER BY clause when the key field is a string.
static void OrderByTestL(RDbNamedDatabase& aDb, const TDesC& aTblName, const RArray<TNameBuf>& aTestStrArray)
	{
    RDbView view;
    CleanupClosePushL(view);
    
    TBuf<128> sqlStmt;
    sqlStmt.Append(_L("SELECT ID, DATA FROM "));
    sqlStmt.Append(aTblName);
    sqlStmt.Append(_L(" ORDER BY ID"));
    User::LeaveIfError(view.Prepare(aDb, TDbQuery(sqlStmt, EDbCompareCollated), TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());

	(void)::PrintRecordsL(view, aTblName);
	::AssertRecordsOrder(view, aTblName, aTestStrArray);

    CleanupStack::PopAndDestroy(&view);
	}

//Test case 2. Check SELECT statement with LIKE keyword when the key field is a string.
static void LikeTestL(RDbNamedDatabase& aDb, const TDesC& aTblName)
	{
    RDbView view;
    CleanupClosePushL(view);
    
    TBuf<128> sqlStmt;
    sqlStmt.Append(_L("SELECT ID, DATA FROM "));
    sqlStmt.Append(aTblName);
    sqlStmt.Append(_L(" WHERE ID LIKE 'B*'"));
    User::LeaveIfError(view.Prepare(aDb, TDbQuery(sqlStmt, EDbCompareCollated), TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());

	TInt cnt = ::PrintRecordsL(view, aTblName);
	TEST(cnt == 2);

    CleanupStack::PopAndDestroy(&view);
	}

//Test case 3. Check SELECT statement with LIKE & ORDER BY keywords when the key field is a string.
static void LikeOrderTestL(RDbNamedDatabase& aDb, const TDesC& aTblName, const RArray<TNameBuf>& aTestStrArray)
	{
    RDbView view;
    CleanupClosePushL(view);
    
    TBuf<128> sqlStmt;
    sqlStmt.Append(_L("SELECT ID, DATA FROM "));
    sqlStmt.Append(aTblName);
    sqlStmt.Append(_L(" WHERE ID LIKE 'B*' ORDER BY ID"));
    User::LeaveIfError(view.Prepare(aDb, TDbQuery(sqlStmt, EDbCompareCollated), TDbWindow::EUnlimited));
    User::LeaveIfError(view.EvaluateAll());

	TInt cnt = ::PrintRecordsL(view, aTblName);
	TEST(cnt == 2);
	::AssertRecordsOrder(view, aTblName, aTestStrArray);

    CleanupStack::PopAndDestroy(&view);
	}

//Test case 4. Indexed table. The index is a string field.
static void IndexTestL(RDbNamedDatabase& aDb, const TDesC& aTblName, const RArray<TNameBuf>& aTestStrArray)
	{
	_LIT(KIdxName, "ID");
	::CreateIndexL(aDb, aTblName, KIdxName);
	
	RDbTable tbl;
	CleanupClosePushL(tbl);
	TEST2(tbl.Open(aDb, aTblName, RDbRowSet::EReadOnly), KErrNone);
	TEST2(tbl.SetIndex(KIdxName), KErrNone);

	(void)::PrintRecordsL(tbl, aTblName);
	::AssertRecordsOrder(tbl, aTblName, aTestStrArray);

    CleanupStack::PopAndDestroy(&tbl);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//The main test function.
//Call your new test functions from here
static void RunTestsL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-LEGACY-DBMSSTRCOMP-0001 Init test environment "));
	::InitEnvL();

    RArray<TNameBuf> testStrArray;
    CleanupClosePushL(testStrArray);
	::FillStrArraySortedL(testStrArray);

	TheTest.Next(_L("SELECT, ORDER BY, EDbColText16"));
	::OrderByTestL(TheDb, KTestTableName1, testStrArray);

	TheTest.Next(_L("SELECT, ORDER BY, EDbColLongText16"));
	::OrderByTestL(TheDb, KTestTableName2, testStrArray);
	
	TheTest.Next(_L("SELECT, LIKE, EDbColText16"));
	::LikeTestL(TheDb, KTestTableName1);

	TheTest.Next(_L("SELECT, LIKE, EDbColLongText16"));
	::LikeTestL(TheDb, KTestTableName2);
	
    RArray<TNameBuf> testStrArray2;
    CleanupClosePushL(testStrArray2);
    testStrArray2.AppendL(testStrArray[2]);//"bbB"
    testStrArray2.AppendL(testStrArray[3]);//"BbB"

	TheTest.Next(_L("SELECT, LIKE, ORDER BY, EDbColText16"));
	::LikeOrderTestL(TheDb, KTestTableName1, testStrArray2);

	TheTest.Next(_L("SELECT, LIKE, ORDER BY, EDbColLongText16"));
	::LikeOrderTestL(TheDb, KTestTableName2, testStrArray2);
	
	TheTest.Next(_L("Index, EDbColText16"));
	::IndexTestL(TheDb, KTestTableName1, testStrArray);

// Not possible to create a key with EDbColLongText16
//	TheTest.Next(_L("Index, EDbColLongText16"));
//	::IndexTestL(TheDb, KTestTableName2, testStrArray);
	
	//Add tests here!
	
    CleanupStack::PopAndDestroy(&testStrArray2);
    CleanupStack::PopAndDestroy(&testStrArray);
	}

TInt E32Main()
	{
	TheTest.Title();

	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	TEST(trapCleanup != NULL);

	TInt err = TheFs.Connect();
	TEST2(err, KErrNone);
	::SetupTestDirectory();

	::DeleteDataFile(KTestDatabase);

	TRAP(err, ::RunTestsL());
	TheDb.Close();
#ifndef __TOOLS2__
	TheDbSession.Close();
#endif
	TheFs.Close();
	TEST2(err, KErrNone);

#ifndef __linux__
	TRAP(err, TheCrcChecker.GenerateCrcL(KTestDatabase));
	TEST(err==KErrNone);
#ifndef __TOOLS2__
	TRAPD(lc, err = TheCrcChecker.DumpCrcRecordsL(KCrcRecord));
	TEST(err==KErrNone);
	TEST(lc==KErrNone);
#else
	TRAPD(lc, err = TheCrcChecker.ValidateCrcRecordsL(KCrcRecord));
	TPtrC errmsg;
	TheCrcChecker.ErrorReportL(err, errmsg);
	RDebug::Print(errmsg);
	TheTest(err==KErrNone || err==TDBMS_CRCChecks::ECrcCheckOk);
#endif
#endif

	::DeleteDataFile(KTestDatabase);

	TheTest.Printf(_L("Wait DBMS server shutdown...\n"));
	User::After(KExitDelay);

	TheTest.End();
	TheTest.Close();

	delete trapCleanup;

	__UHEAP_MARKEND;

	return 0;
	}


