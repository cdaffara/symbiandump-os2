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

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>

#include "crccheck.h"

#undef __UHEAP_MARK
#define __UHEAP_MARK
#undef __UHEAP_MARKEND
#define __UHEAP_MARKEND

LOCAL_D TDBMS_CRCChecks TheCrcChecker;

#ifndef __linux__ //No CRC test on LINUX
#ifdef __TOOLS2__
const TPtrC	KCrcRecord=_L("\\epoc32\\winscw\\c\\dbms-tst\\T_DBMS.CRC");
#else
const TPtrC	KCrcRecord=_L("C:\\dbms-tst\\T_DBMS.CRC");
#endif
#endif

#if defined(__VC32__) && !defined(_DEBUG)
#pragma warning(disable : 4710)			// function not expanded. MSVC 4.0 is stupid
#endif

LOCAL_D RTest test(_L("T_DBMS : Test DBMS API"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbs TheDbs;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RDbView TheView;
LOCAL_D RFs TheFs;
LOCAL_D TBuf8<0x200> TheBuf8;
LOCAL_D TBuf16<0x100> TheBuf16;

const TInt KTestCleanupStack=0x40;

#ifdef __TOOLS2__
const TPtrC KTestDatabase=_L(".\\dbms-tst\\T_DBMS.DB");
#else
const TPtrC KTestDatabase=_L("C:\\dbms-tst\\T_DBMS.DB");
#endif

const TPtrC8 KTestText8(_S8("Text used for test columns when testing"));
const TPtrC8 KTestShortText8(_S8("a"));
const TPtrC8 KTestLongText8(_S8("Text which is used to set Long ASCII Text columns and ensure they are not inlined, and therefore needs to be very long indeed............." \
							"Text which is used to set Long ASCII Text columns and ensure they are not inlined, and therefore needs to be very long indeed............."));
const TPtrC16 KTestText16(_S16("Text used for test columns when testing"));
const TPtrC16 KTestShortText16(_S16("a"));
const TPtrC16 KTestLongText16(_S16("Text which is used to set Long UNICODE Text columns and ensure they are not inlined, and therefore needs to be very long indeed............."));
const TPtrC KTableName(_S("TestTable"));
const TPtrC KTableName2(_S("TestTable2"));
const TPtrC KTestViewTable(_L("TestViewTable"));
const TPtrC KTestTypesTable(_L("TestTypesTable"));
const TPtrC KTestFindTable(_L("TestFindTable"));
const TPtrC KTestMultiTable(_L("TestMultiTable"));
const TPtrC KIndexName(_S("Index1"));
const TPtrC KIndexName2(_S("Index2"));
const TPtrC KIndexName3(_S("Index3"));
const TInt KBlobItems=1000;

const TPtrC KNull=TPtrC();
const TPtrC KPass1(_S("wibble"));
const TPtrC KPass2(_S("wobble"));

const TTime KMinTTime=Time::MinTTime();
const TTime KMaxTTime=Time::MaxTTime();

struct SColDef
	{
	const TText* iName;
	TDbColType iType;
	TInt iAttributes;
	};
LOCAL_D SColDef const SingleColumn[]=
	{
	{_S("Column1"),EDbColInt32,0},
	{0}
	};
LOCAL_D SColDef const DoubleColumn[]=
	{
	{_S("Column1"),EDbColInt32,0},
	{_S("c2"),EDbColInt64,0},
	{0}
	};
LOCAL_D SColDef const ViewTestColumns[]=
	{
	{_S("Int"),EDbColInt32,0},
	{_S("Long1"),EDbColLongText8,0},
	{_S("Long2"),EDbColLongText8,0},
	{_S("Long3"),EDbColLongText8,0},
	{_S("Long4"),EDbColLongText8,0},
	{_S("Long5"),EDbColLongText8,0},
	{0}
	};
LOCAL_D SColDef const AllColumns[]=
	{
	{_S("counter"),EDbColInt32,TDbCol::ENotNull|TDbCol::EAutoIncrement},
	{_S("bit"),EDbColBit,TDbCol::ENotNull},
	{_S("int8"),EDbColInt8,TDbCol::ENotNull},
	{_S("uint8"),EDbColUint8,TDbCol::ENotNull},
	{_S("int16"),EDbColInt16,TDbCol::ENotNull},
	{_S("uint16"),EDbColUint16,TDbCol::ENotNull},
	{_S("int32"),EDbColInt32,TDbCol::ENotNull},
	{_S("uint32"),EDbColUint32,TDbCol::ENotNull},
	{_S("int64"),EDbColInt64,TDbCol::ENotNull},
	{_S("real32"),EDbColReal32,TDbCol::ENotNull},
	{_S("real64"),EDbColReal64,TDbCol::ENotNull},
	{_S("date_time"),EDbColDateTime,TDbCol::ENotNull},
	{_S("text8"),EDbColText8,TDbCol::ENotNull},
	{_S("text16"),EDbColText16,TDbCol::ENotNull},
	{_S("binary"),EDbColBinary,TDbCol::ENotNull},
	{_S("longtext8"),EDbColLongText8,TDbCol::ENotNull},
	{_S("longtext16"),EDbColLongText16,TDbCol::ENotNull},
	{_S("longbinary"),EDbColLongBinary,TDbCol::ENotNull},
	{_S("bit_null"),EDbColBit,0},
	{_S("int8_null"),EDbColInt8,0},
	{_S("uint8_null"),EDbColUint8,0},
	{_S("int16_null"),EDbColInt16,0},
	{_S("uint16_null"),EDbColUint16,0},
	{_S("int32_null"),EDbColInt32,0},
	{_S("uint32_null"),EDbColUint32,0},
	{_S("int64_null"),EDbColInt64,0},
	{_S("real32_null"),EDbColReal32,0},
	{_S("real64_null"),EDbColReal64,0},
	{_S("date_time_null"),EDbColDateTime,0},
	{_S("text8_null"),EDbColText8,0},
	{_S("text16_null"),EDbColText16,0},
	{_S("binary_null"),EDbColBinary,0},
	{_S("longtext8_null"),EDbColLongText8,0},
	{_S("longtext16_null"),EDbColLongText16,0},
	{_S("longbinary_null"),EDbColLongBinary,0},
	{_S("force_null"),EDbColInt32,0},
	{0}
	};
const TInt KColumns=sizeof(AllColumns)/sizeof(AllColumns[0])-1;
enum TColumns
	{
	ECounter,
	EBit,EInt8,EUint8,EInt16,EUint16,EInt32,EUint32,EInt64,
	EReal32,EReal64,ETime,EText8,EText16,ETBinary,
	ELongText8,ELongText16,ELongBinary,
	EBitNull,EInt8Null,EUint8Null,EInt16Null,EUint16Null,EInt32Null,EUint32Null,EInt64Null,
	EReal32Null,EReal64Null,ETimeNull,EText8Null,EText16Null,ETBinaryNull,
	ELongText8Null,ELongText16Null,ELongBinaryNull,EForceNull
	};
LOCAL_D TDbColNo TheMap[KColumns];

CDbColSet* ColumnSetL(const SColDef* aDef)
	{
	CDbColSet *set=CDbColSet::NewLC();
	for (;aDef->iName;++aDef)
		{
		TDbCol col(TPtrC(aDef->iName),aDef->iType);
		col.iAttributes=aDef->iAttributes;
		set->AddL(col);
		}
	CleanupStack::Pop();
	return set;
	}

enum TColSet {EAllTypes,ESingleColumn,EDoubleColumn,EViewTestTable};
LOCAL_D const SColDef* const ColSetDef[]=
	{
	AllColumns,
	SingleColumn,
	DoubleColumn,
	ViewTestColumns
	};
LOCAL_C CDbColSet* CreateColSetL(TColSet aType)
	{
	return ColumnSetL(ColSetDef[aType]);
	}

inline TPtrC ColName(const SColDef* aSet,TInt aColumn)
	{return TPtrC(aSet[aColumn].iName);}

enum TKeySet {EKeyAsc,EKeyDesc,EKeyMulti,EKeySingle};
LOCAL_C CDbKey* CreateKeyL(TKeySet aType)
	{
	CDbKey *k=CDbKey::NewLC();
	switch(aType)
		{
	case EKeyAsc:
		k->AddL(ColName(AllColumns,EInt32));
		break;
	case EKeyDesc:
		k->AddL(TDbKeyCol(ColName(AllColumns,EInt32),TDbKeyCol::EDesc));
		k->MakeUnique();
		break;
	case EKeyMulti:
		k->AddL(ColName(AllColumns,EInt32)).AddL(ColName(AllColumns,EText8));
		k->SetComparison(EDbCompareFolded);
		break;
	case EKeySingle:
		k->AddL(ColName(SingleColumn,0));
		break;
		}
	CleanupStack::Pop();
	return k;
	}

LOCAL_C void CloseDatabase()
	{
	TheDatabase.Close();
	TheCrcChecker.GenerateCrcL(KTestDatabase);
	}


//
// Open a shared database (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C TInt OpenSharedDatabase()
	{
	return TheDatabase.Open(TheDbs,KTestDatabase);
	}

//
// Create a local database (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C void CreateClientDatabase()
	{
	TInt r=TheDatabase.Replace(TheFs,KTestDatabase);
	test (r==KErrNone);
	}

//
// Create a shared database (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C void CreateSharedDatabase()
	{
	CreateClientDatabase();
	CloseDatabase();
	TInt r=OpenSharedDatabase();
	test (r==KErrNone);
	}

//
// Open a shared database (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C void OpenDatabase()
	{
	TInt r=OpenSharedDatabase();
	test (r==KErrNone);
	}

//
// Create a shared database (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C void CreateDatabase()
	{
	CreateSharedDatabase();
	}


LOCAL_C void DestroyDatabase()
	{
	TInt r=TheDatabase.Destroy();
	test (r==KErrNone);
	}

//
// Check the database has this many tables
//
LOCAL_C void CountTables(TInt aCount)
	{
	CDbNames* names=TheDatabase.TableNamesL();
	test (names->Count()==aCount);
	delete names;
	}

//
// Compare two column sets
//
LOCAL_C void Compare(const TDesC& aTable,const CDbColSet& aSet)
	{
	CDbColSet* set=TheDatabase.ColSetL(aTable);
	test(set->Count()==aSet.Count());
	for (TDbColSetIter iter(*set);iter;++iter)
		{
		const TDbCol* pRight=aSet.Col(iter->iName);
		test(pRight!=NULL);
		test(iter->iType==pRight->iType);
		test(iter->iMaxLength==KDbUndefinedLength || pRight->iMaxLength==KDbUndefinedLength || iter->iMaxLength==pRight->iMaxLength);
		test((iter->iAttributes&pRight->iAttributes)==iter->iAttributes);
		}
	delete set;
	}

//
// Check the database has this many tables
//
LOCAL_C void CountIndexes(const TDesC& aTable,TInt aCount)
	{
	CDbNames* names=TheDatabase.IndexNamesL(aTable);
	test (names->Count()==aCount);
	delete names;
	}

//
// Compare index definition
//
LOCAL_C void Compare(const TDesC& aIndex,const TDesC& aTable,const CDbKey& aKey)
	{
	CDbKey* key=TheDatabase.KeyL(aIndex,aTable);
	test(key->Count()==aKey.Count());
	test(key->Comparison()==aKey.Comparison());
	test(key->IsUnique()==aKey.IsUnique());
	for (TInt ii=aKey.Count();--ii>=0;)
		{
		const TDbKeyCol& left=(*key)[ii];
		const TDbKeyCol& right=aKey[ii];
		test (left.iName==right.iName);
		test (left.iOrder==right.iOrder);
		test (left.iLength==right.iLength || right.iLength==KDbUndefinedLength);
		}
	delete key;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0594
@SYMTestCaseDesc        Tests the database definition and enquiry functions
@SYMTestPriority        Medium
@SYMTestActions         Tests for creation and opening of a database,creation of a table 
                        Tests for comparing column and table column sets
						Tests for altering a table and creation of an index.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDDL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0594 Create Database "));
	CreateDatabase();
	CountTables(0);
	CloseDatabase();
	test.Next(_L("Open Database"));
	OpenDatabase();
	CountTables(0);
	test.Next(_L("CreateTable"));
	CDbColSet* cs=CreateColSetL(EAllTypes);
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CountTables(1);
	test.Next(_L("Compare column sets"));
	Compare(KTableName,*cs);
	CloseDatabase();
	test.Next(_L("Re-open database"));
	OpenDatabase();
	CountTables(1);
	test.Next(_L("Compare table column sets"));
	Compare(KTableName,*cs);
	delete cs;
	test.Next(_L("Add another table"));
	cs=CreateColSetL(ESingleColumn);
	test(TheDatabase.CreateTable(KTableName2,*cs)==KErrNone);
	Compare(KTableName2,*cs);
	delete cs;
	CountTables(2);
//
	test.Next(_L("Alter table"));
	cs=CreateColSetL(EDoubleColumn);
	test(TheDatabase.AlterTable(KTableName2,*cs)==KErrNone);
	Compare(KTableName2,*cs);
	delete cs;
	CountTables(2);
//
	test.Next(_L("Test index creation"));
	CountIndexes(KTableName,0);
	CountIndexes(KTableName2,0);
	CDbKey* key=CreateKeyL(EKeyAsc);
	test (TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	Compare(KIndexName,KTableName,*key);
	delete key;
	key=CreateKeyL(EKeyDesc);
	test (TheDatabase.CreateIndex(KIndexName2,KTableName,*key)==KErrNone);
	Compare(KIndexName2,KTableName,*key);
	delete key;
	key=CreateKeyL(EKeySingle);
	test (TheDatabase.CreateIndex(KIndexName,KTableName2,*key)==KErrNone);
	Compare(KIndexName,KTableName2,*key);
	delete key;
	key=CreateKeyL(EKeyMulti);
	test (TheDatabase.CreateIndex(KIndexName3,KTableName,*key)==KErrNone);
	Compare(KIndexName3,KTableName,*key);
	CountIndexes(KTableName,3);
	CountIndexes(KTableName2,1);
	CloseDatabase();
	OpenDatabase();
	CountIndexes(KTableName,3);
	CountIndexes(KTableName2,1);
	Compare(KIndexName3,KTableName,*key);
	delete key;
	test.Next(_L("Drop indexes"));
	test (TheDatabase.DropIndex(KIndexName,KTableName)==KErrNone);
	CountIndexes(KTableName,2);
	test (TheDatabase.DropIndex(KIndexName2,KTableName)==KErrNone);
	CountIndexes(KTableName,1);
	test (TheDatabase.DropIndex(KIndexName,KTableName2)==KErrNone);
	CountIndexes(KTableName2,0);
	test.Next(_L("Drop table"));
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	CountTables(1);
	test(TheDatabase.DropTable(KTableName2)==KErrNone);
	CountTables(0);
	CloseDatabase();
	OpenDatabase();
	CountTables(0);
	DestroyDatabase();
	test.End();
	}


LOCAL_C void AddRow()
	{
	test(TheView.Prepare(TheDatabase,_L("select * from testviewtable"))==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TheView.InsertL();
	TheView.SetColL(1,(TInt32)12345);
	TheView.PutL();
	TheView.InsertL();
	TheView.Cancel();
	TheView.FirstL();
	TheView.NextL();
	test(TheView.AtEnd());		// should be only 1 row as second insert was cancelled
	TheView.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1318
@SYMTestCaseDesc        Tests for RDbView class
@SYMTestPriority        Medium
@SYMTestActions         Tests for navigation of the cursor in the generated row sets.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEmptyNavigation(const TDesC& aQuery)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1318 Test navigation states "));
	test(TheView.Prepare(TheDatabase,aQuery,TheView.EReadOnly)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TheView.FirstL();
	test(!TheView.AtBeginning());
	test(!TheView.AtRow());
	test(TheView.AtEnd());
	TheView.PreviousL();
	test(TheView.AtBeginning());
	test(!TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.NextL();
	test(!TheView.AtBeginning());
	test(!TheView.AtRow());
	test(TheView.AtEnd());
	TheView.LastL();
	test(TheView.AtBeginning());
	test(!TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.NextL();
	test(TheView.AtEnd());
	TheView.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1319
@SYMTestCaseDesc        Tests for RDbView class
@SYMTestPriority        Medium
@SYMTestActions         Tests for navigation of the cursor in the generated row sets.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNavigation(const TDesC& aQuery)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1319 Test navigation states "));
	test(TheView.Prepare(TheDatabase,aQuery,TheView.EReadOnly)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TheView.FirstL();
	test(!TheView.AtBeginning());
	test(TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.GetL();
	test(TheView.ColInt32(1)==12345);
	TheView.PreviousL();
	test(TheView.AtBeginning());
	test(!TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.NextL();
	test(!TheView.AtBeginning());
	test(TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.NextL();
	test(!TheView.AtBeginning());
	test(!TheView.AtRow());
	test(TheView.AtEnd());
	TheView.PreviousL();
	test(!TheView.AtBeginning());
	test(TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.LastL();
	test(!TheView.AtBeginning());
	test(TheView.AtRow());
	test(!TheView.AtEnd());
	TheView.NextL();
	test(TheView.AtEnd());
	TheView.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1320
@SYMTestCaseDesc        RDbView updation test
@SYMTestPriority        Medium
@SYMTestActions         Tests for updation of row sets 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TestUpdate()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1320 Update and delete the row "));
	test(TheView.Prepare(TheDatabase,_L("select * from TestViewTable"))==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TheView.FirstL();
	TheView.UpdateL();
	TheView.SetColL(1,(TInt32)54321);
	TheView.PutL();
	TheView.FirstL();
	TheView.GetL();
	test(TheView.ColInt32(1)==54321);
	TheView.UpdateL();
	TheView.SetColL(1,(TInt32)12345);
	TheView.Cancel();
	TheView.FirstL();
	TheView.GetL();
	test(TheView.ColInt32(1)==54321);
	TheView.DeleteL();
	TheView.PreviousL();
	test(TheView.AtBeginning());
	TheView.NextL();
	test(TheView.AtEnd());
	TheView.Close();
	}

#include <s32mem.h>

void writeBLOB(TDbColNo aCol,const TDesC8& aDes)
	{
	CBufSeg* buf=CBufSeg::NewL(128);
	CleanupStack::PushL(buf);
	buf->InsertL(0,aDes);
	RBufReadStream read(*buf);
	RDbColWriteStream blob;
	blob.OpenLC(TheView,aCol);
//	blob.WriteL(aDes);
	blob.WriteL(read,aDes.Length());
	blob.CommitL();
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();	// buf
	test (TheView.ColLength(aCol)==aDes.Length());
	test (TheView.ColSize(aCol)==aDes.Size());
	}

void checkBLOB(TDbColNo aCol,const TDesC8& aDes)
	{
	RDbColReadStream blob;
	blob.OpenLC(TheView,aCol);
	blob.ReadL(TheBuf8,TheView.ColLength(aCol));
	CleanupStack::PopAndDestroy();
	test(TheBuf8==aDes);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1321
@SYMTestCaseDesc        BLOB tests
@SYMTestPriority        Medium
@SYMTestActions         Test long column tracking through insert/put/update/cancel
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestBLOB()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1321 Long column tracking "));
	test(TheView.Prepare(TheDatabase,_L("select * from TestViewTable"))==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TheView.InsertL();
	writeBLOB(2,KTestShortText8);
	writeBLOB(3,KTestShortText8);
	TheView.PutL();
	TheView.InsertL();
	writeBLOB(4,KTestShortText8);
	writeBLOB(5,KTestShortText8);
	TheView.Cancel();
// change in behaviour--writing BLOBs forces a true rollback here. So reset
	TheView.Reset();
	test(TheView.EvaluateAll()==KErrNone);
// check the one row we should have
	TheView.FirstL();
	TheView.GetL();
	checkBLOB(2,KTestShortText8);
	checkBLOB(3,KTestShortText8);
	test(TheView.IsColNull(4));
	test(TheView.IsColNull(5));
	test(TheView.IsColNull(6));
	TheView.NextL();
	test(TheView.AtEnd());
// update/cancel the row
	TheView.FirstL();
	TheView.UpdateL();
	TheView.SetColNullL(3);
	writeBLOB(4,KTestLongText8);
	writeBLOB(5,KTestLongText8);
// check pre-cancel
	checkBLOB(2,KTestShortText8);
	test(TheView.IsColNull(3));
	checkBLOB(4,KTestLongText8);
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
// cancel and check
	TheView.Cancel();
// change in behaviour--writing BLOBs forces a true rollback here. So reset
	TheView.Reset();
	test(TheView.EvaluateAll()==KErrNone);
	TheView.FirstL();
	TheView.GetL();
	checkBLOB(2,KTestShortText8);
	checkBLOB(3,KTestShortText8);
	test(TheView.IsColNull(4));
	test(TheView.IsColNull(5));
	test(TheView.IsColNull(6));
	TheView.NextL();
	test(TheView.AtEnd());
// update/put the row
	TheView.FirstL();
	TheView.UpdateL();
	TheView.SetColNullL(3);
	writeBLOB(4,KTestLongText8);
	writeBLOB(5,KTestLongText8);
// check pre-put
	checkBLOB(2,KTestShortText8);
	test(TheView.IsColNull(3));
	checkBLOB(4,KTestLongText8);
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
// put and check
	TheView.PutL();
	TheView.FirstL();
	TheView.GetL();
	checkBLOB(2,KTestShortText8);
	test(TheView.IsColNull(3));
	checkBLOB(4,KTestLongText8);
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
	TheView.NextL();
	test(TheView.AtEnd());
// update/put the row again
	TheView.FirstL();
	TheView.UpdateL();
	TheView.SetColNullL(2);
	writeBLOB(3,KTestShortText8);
	writeBLOB(4,KTestShortText8);
	writeBLOB(5,KTestShortText8);
// check
	test(TheView.IsColNull(2));
	checkBLOB(3,KTestShortText8);
	checkBLOB(4,KTestShortText8);
	checkBLOB(5,KTestShortText8);
	test(TheView.IsColNull(6));
// modify again
	writeBLOB(2,KTestLongText8);
	TheView.SetColNullL(3);
	TheView.SetColNullL(4);
	writeBLOB(5,KTestLongText8);
// check pre-put
	checkBLOB(2,KTestLongText8);
	test(TheView.IsColNull(3));
	test(TheView.IsColNull(4));
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
// put and check
	TheView.PutL();
	TheView.FirstL();
	TheView.GetL();
	checkBLOB(2,KTestLongText8);
	test(TheView.IsColNull(3));
	test(TheView.IsColNull(4));
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
	TheView.NextL();
	test(TheView.AtEnd());
// insert copy
	TheView.LastL();
	TheView.InsertCopyL();
	TheView.SetColNullL(2);
	writeBLOB(3,KTestLongText8);
// check pre-put
	test(TheView.IsColNull(2));
	checkBLOB(3,KTestLongText8);
	test(TheView.IsColNull(4));
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
// put and check
	TheView.PutL();
	TheView.LastL();
	TheView.GetL();
	test(TheView.IsColNull(2));
	checkBLOB(3,KTestLongText8);
	test(TheView.IsColNull(4));
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
// delete previous one and check
	TheView.PreviousL();
	TheView.DeleteL();
	TheView.NextL();
	TheView.GetL();
	test(TheView.IsColNull(2));
	checkBLOB(3,KTestLongText8);
	test(TheView.IsColNull(4));
	checkBLOB(5,KTestLongText8);
	test(TheView.IsColNull(6));
	TheView.NextL();
	test(TheView.AtEnd());
// delete
	TheView.FirstL();
	TheView.DeleteL();
// finish
	TheView.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0596
@SYMTestCaseDesc        Tests for navigation states
@SYMTestPriority        Medium
@SYMTestActions         Tests for view state cycle, navigation, Insert/Update/Delete 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestView()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0596 Create the Test table "));
	CreateDatabase();
	CDbColSet* cs=CreateColSetL(EViewTestTable);
	test(TheDatabase.CreateTable(KTestViewTable,*cs)==KErrNone);
	delete cs;
	test.Next(_L("Test navigation states"));
	TestEmptyNavigation(_L("select * from TestViewTable"));
	test.Next(_L("Insert and Put/Cancel"));
	AddRow();
	test.Next(_L("Test navigation states"));
	TestNavigation(_L("select * from TestViewTable"));
	TestNavigation(_L("select * from TestViewTable where Int is not null"));
	TestEmptyNavigation(_L("select * from TestViewTable where Int is null"));
	test.Next(_L("Update and delete the row"));
	TestUpdate();
	test.Next(_L("Long column tracking"));
	TestBLOB();
	test.Next(_L("Close"));
	CloseDatabase();
	test.End();
	}

//
// Check the integral set/get functions for different width types
//
LOCAL_C void CheckIntAccessors()
	{
	TheView.InsertL();
// bit column
	TDbColNo ord=TheMap[EBit];
	TheView.SetColL(ord,TUint32(1));
	test (TheView.ColUint(ord)==1);
	TheView.SetColL(ord,TInt32(0));
	test (TheView.ColUint(ord)==0);
	TheView.SetColL(ord,TInt64(1));
	test (TheView.ColUint(ord)==1);
	test (TheView.ColUint8(ord)==1);
	test (TheView.ColUint16(ord)==1);
	test (TheView.ColUint32(ord)==1);
	test (TheView.ColInt8(ord)==1);
	test (TheView.ColInt16(ord)==1);
	test (TheView.ColInt32(ord)==1);
	test (TheView.ColInt64(ord)==1);
// uint8 column
	ord=TheMap[EUint8];
	TheView.SetColL(ord,TUint32(255));
	test (TheView.ColUint(ord)==255);
	TheView.SetColL(ord,TInt32(0));
	test (TheView.ColUint(ord)==0);
	TheView.SetColL(ord,TInt64(1));
	test (TheView.ColUint(ord)==1);
	test (TheView.ColUint8(ord)==1);
	test (TheView.ColUint16(ord)==1);
	test (TheView.ColUint32(ord)==1);
	test (TheView.ColInt16(ord)==1);
	test (TheView.ColInt32(ord)==1);
	test (TheView.ColInt64(ord)==1);
// uint16 column
	ord=TheMap[EUint16];
	TheView.SetColL(ord,TUint32(65535));
	test (TheView.ColUint(ord)==65535);
	TheView.SetColL(ord,TInt32(0));
	test (TheView.ColUint(ord)==0);
	TheView.SetColL(ord,TInt64(1));
	test (TheView.ColUint(ord)==1);
	test (TheView.ColUint16(ord)==1);
	test (TheView.ColUint32(ord)==1);
	test (TheView.ColInt32(ord)==1);
	test (TheView.ColInt64(ord)==1);
// uint32 column
	ord=TheMap[EUint32];
	TheView.SetColL(ord,TUint32(0));
	test (TheView.ColUint(ord)==0);
	TheView.SetColL(ord,TInt32(KMaxTInt));
	test (TInt(TheView.ColUint(ord))==KMaxTInt);
	TheView.SetColL(ord,TInt64(KMaxTUint));
	test (TheView.ColUint(ord)==KMaxTUint);
	test (TheView.ColUint32(ord)==KMaxTUint);
	test (TheView.ColInt64(ord)==KMaxTUint);
// int8 column
	ord=TheMap[EInt8];
	TheView.SetColL(ord,TUint32(127));
	test (TheView.ColInt(ord)==127);
	TheView.SetColL(ord,TInt32(0));
	test (TheView.ColInt(ord)==0);
	TheView.SetColL(ord,TInt64(-128));
	test (TheView.ColInt(ord)==-128);
	test (TheView.ColInt8(ord)==-128);
	test (TheView.ColInt16(ord)==-128);
	test (TheView.ColInt32(ord)==-128);
	test (TheView.ColInt64(ord)==-128);
// int16 column
	ord=TheMap[EInt16];
	TheView.SetColL(ord,TUint32(32767));
	test (TheView.ColInt(ord)==32767);
	TheView.SetColL(ord,TInt32(0));
	test (TheView.ColInt(ord)==0);
	TheView.SetColL(ord,TInt64(-32768));
	test (TheView.ColInt(ord)==-32768);
	test (TheView.ColInt16(ord)==-32768);
	test (TheView.ColInt32(ord)==-32768);
	test (TheView.ColInt64(ord)==-32768);
// int32 column
	ord=TheMap[EInt32];
	TheView.SetColL(ord,TUint32(KMaxTInt));
	test (TheView.ColInt(ord)==KMaxTInt);
	TheView.SetColL(ord,TInt32(0));
	test (TheView.ColInt(ord)==0);
	TheView.SetColL(ord,TInt64(KMinTInt));
	test (TheView.ColInt(ord)==KMinTInt);
	test (TheView.ColInt32(ord)==KMinTInt);
	test (TheView.ColInt64(ord)==KMinTInt);
//
	TheView.Cancel();
	}

//
// Write the values to nullable column types
//
LOCAL_C void WriteMinValues(TInt anOffset)
	{
	TheView.SetColL(TheMap[anOffset+EBit],TUint(0));
	TheView.SetColL(TheMap[anOffset+EInt8],KMinTInt8);
	TheView.SetColL(TheMap[anOffset+EUint8],TUint(0));
	TheView.SetColL(TheMap[anOffset+EInt16],KMinTInt16);
	TheView.SetColL(TheMap[anOffset+EUint16],TUint(0));
	TheView.SetColL(TheMap[anOffset+EInt32],KMinTInt32);
	TheView.SetColL(TheMap[anOffset+EUint32],TUint(0));
	TheView.SetColL(TheMap[anOffset+EInt64],KMinTInt64);
	TheView.SetColL(TheMap[anOffset+EReal32],KMinTReal32);
	TheView.SetColL(TheMap[anOffset+EReal64],KMinTReal64);
	TheView.SetColL(TheMap[anOffset+ETime],KMinTTime);
	TheView.SetColL(TheMap[anOffset+EText8],KTestShortText8);
	TheView.SetColL(TheMap[anOffset+EText16],KTestShortText16);
	{
	TUint8 val=0;
	TheView.SetColL(TheMap[anOffset+ETBinary],TPtrC8(&val,sizeof(val)));
	}
	TheView.SetColL(TheMap[anOffset+ELongText8],KTestShortText8);
	TheView.SetColL(TheMap[anOffset+ELongText16],KTestShortText16);
	RDbColWriteStream out;
	out.OpenLC(TheView,TheMap[anOffset+ELongBinary]);
	out.WriteUint8L(0);
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

//
// Write a row with the minimum (or small) column values
// return the value in the counter column
//
LOCAL_C TInt WriteMinimum()
	{
	TheView.InsertL();
	WriteMinValues(0);
	WriteMinValues(EBitNull-EBit);
	test(!TheView.IsColNull(TheMap[ECounter]));
	TInt cc=TheView.ColInt(TheMap[ECounter]);
	TheView.PutL();
	return cc;
	}

LOCAL_C void CheckMinValues(TInt anOffset)
	{
	test(TheView.ColUint8(TheMap[anOffset+EBit])==0);
	test(TheView.ColUint16(TheMap[anOffset+EBit])==0);
	test(TheView.ColUint32(TheMap[anOffset+EBit])==0);
	test(TheView.ColUint(TheMap[anOffset+EBit])==0);
	test(TheView.ColInt8(TheMap[anOffset+EInt8])==KMinTInt8);
	test(TheView.ColInt16(TheMap[anOffset+EInt8])==KMinTInt8);
	test(TheView.ColInt32(TheMap[anOffset+EInt8])==KMinTInt8);
	test(TheView.ColInt(TheMap[anOffset+EInt8])==KMinTInt8);
	test(TheView.ColUint8(TheMap[anOffset+EUint8])==0);
	test(TheView.ColUint16(TheMap[anOffset+EUint8])==0);
	test(TheView.ColUint32(TheMap[anOffset+EUint8])==0);
	test(TheView.ColUint(TheMap[anOffset+EUint8])==0);
	test(TheView.ColInt16(TheMap[anOffset+EInt16])==KMinTInt16);
	test(TheView.ColInt32(TheMap[anOffset+EInt16])==KMinTInt16);
	test(TheView.ColInt(TheMap[anOffset+EInt16])==KMinTInt16);
	test(TheView.ColUint16(TheMap[anOffset+EUint16])==0);
	test(TheView.ColUint32(TheMap[anOffset+EUint16])==0);
	test(TheView.ColUint(TheMap[anOffset+EUint16])==0);
	test(TheView.ColInt32(TheMap[anOffset+EInt32])==KMinTInt32);
	test(TheView.ColInt(TheMap[anOffset+EInt32])==KMinTInt32);
	test(TheView.ColUint32(TheMap[anOffset+EUint32])==0);
	test(TheView.ColUint(TheMap[anOffset+EUint32])==0);
	test(TheView.ColInt64(TheMap[anOffset+EInt64])==KMinTInt64);
	test(TheView.ColReal32(TheMap[anOffset+EReal32])==KMinTReal32);
	test(TheView.ColReal64(TheMap[anOffset+EReal64])==KMinTReal64);
	test(TheView.ColReal(TheMap[anOffset+EReal64])==KMinTReal64);
	test(TheView.ColTime(TheMap[anOffset+ETime])==KMinTTime);
	test(TheView.ColDes8(TheMap[anOffset+EText8])==KTestShortText8);
	test(TheView.ColSize(TheMap[anOffset+EText8])==KTestShortText8.Size());
	test(TheView.ColLength(TheMap[anOffset+EText8])==KTestShortText8.Length());
	test(TheView.ColDes16(TheMap[anOffset+EText16])==KTestShortText16);
	test(TheView.ColSize(TheMap[anOffset+EText16])==KTestShortText16.Size());
	test(TheView.ColLength(TheMap[anOffset+EText16])==KTestShortText16.Length());
	{
	TUint8 val=0;
	test(TheView.ColDes8(TheMap[anOffset+ETBinary])==TPtrC8(&val,sizeof(val)));
	}
	test(TheView.ColSize(TheMap[anOffset+ETBinary])==1);
	test(TheView.ColLength(TheMap[anOffset+ETBinary])==1);
//
	test(TheView.ColSize(TheMap[anOffset+ELongText8])==KTestShortText8.Size());
	test(TheView.ColLength(TheMap[anOffset+ELongText8])==KTestShortText8.Length());
	RDbColReadStream in;
	in.OpenLC(TheView,TheMap[anOffset+ELongText8]);
	in.ReadL(TheBuf8,TheView.ColLength(TheMap[anOffset+ELongText8]));
	CleanupStack::PopAndDestroy();
	test(TheBuf8==KTestShortText8);
//
	test(TheView.ColSize(TheMap[anOffset+ELongText16])==KTestShortText16.Size());
	test(TheView.ColLength(TheMap[anOffset+ELongText16])==KTestShortText16.Length());
	in.OpenLC(TheView,TheMap[anOffset+ELongText16]);
	in.ReadL(TheBuf16,TheView.ColLength(TheMap[anOffset+ELongText16]));
	CleanupStack::PopAndDestroy();
	test(TheBuf16==KTestShortText16);
//
	test(TheView.ColSize(TheMap[anOffset+ELongBinary])==1);
	test(TheView.ColLength(TheMap[anOffset+ELongBinary])==1);
	in.OpenLC(TheView,TheMap[anOffset+ELongBinary]);
	test(in.ReadUint8L()==0);
	CleanupStack::PopAndDestroy();
	}

//
// Check the row is the same as was written
//
LOCAL_C void CheckMinimum(TInt aCounter)
	{
	TheView.GetL();
	test(TheView.ColInt32(TheMap[ECounter])==aCounter);
	CheckMinValues(0);
	CheckMinValues(EBitNull-EBit);
	}

LOCAL_C void WriteMaxValues(TInt anOffset)
	{
	TheView.SetColL(TheMap[anOffset+EBit],TUint(1));
	TheView.SetColL(TheMap[anOffset+EInt8],KMaxTInt8);
	TheView.SetColL(TheMap[anOffset+EUint8],TUint(KMaxTUint8));
	TheView.SetColL(TheMap[anOffset+EInt16],KMaxTInt16);
	TheView.SetColL(TheMap[anOffset+EUint16],TUint(KMaxTUint16));
	TheView.SetColL(TheMap[anOffset+EInt32],KMaxTInt32);
	TheView.SetColL(TheMap[anOffset+EUint32],KMaxTUint32);
	TheView.SetColL(TheMap[anOffset+EInt64],KMaxTInt64);
	TheView.SetColL(TheMap[anOffset+EReal32],KMaxTReal32);
	TheView.SetColL(TheMap[anOffset+EReal64],KMaxTReal64);
	TheView.SetColL(TheMap[anOffset+ETime],KMaxTTime);
	TheView.SetColL(TheMap[anOffset+EText8],KTestText8);
	TheView.SetColL(TheMap[anOffset+EText16],KTestText16);
	TheView.SetColL(TheMap[anOffset+ETBinary],TPtrC8((TUint8*)&TheMap[0],sizeof(TheMap)));
	TheView.SetColL(TheMap[anOffset+ELongText8],KTestLongText8);
	TheView.SetColL(TheMap[anOffset+ELongText16],KTestLongText16);
	RDbColWriteStream out;
	out.OpenLC(TheView,TheMap[anOffset+ELongBinary]);
	for (TInt ii=0;ii<KBlobItems;++ii)
		out.WriteInt32L(ii);
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

//
// Write a row with the maximum (or longer) column values
// return the value in the counter column
//
LOCAL_C TInt WriteMaximum()
	{
	TheView.InsertL();
	WriteMaxValues(0);
	WriteMaxValues(EBitNull-EBit);
	test(!TheView.IsColNull(TheMap[ECounter]));
	TInt cc=TheView.ColInt(TheMap[ECounter]);
	TheView.PutL();
	return cc;
	}

LOCAL_C void CheckMaxValues(TInt anOffset)
	{
	test(TheView.ColUint8(TheMap[anOffset+EBit])==1);
	test(TheView.ColUint16(TheMap[anOffset+EBit])==1);
	test(TheView.ColUint32(TheMap[anOffset+EBit])==1);
	test(TheView.ColUint(TheMap[anOffset+EBit])==1);
	test(TheView.ColInt8(TheMap[anOffset+EInt8])==KMaxTInt8);
	test(TheView.ColInt16(TheMap[anOffset+EInt8])==KMaxTInt8);
	test(TheView.ColInt32(TheMap[anOffset+EInt8])==KMaxTInt8);
	test(TheView.ColInt(TheMap[anOffset+EInt8])==KMaxTInt8);
	test(TheView.ColUint8(TheMap[anOffset+EUint8])==KMaxTUint8);
	test(TheView.ColUint16(TheMap[anOffset+EUint8])==KMaxTUint8);
	test(TheView.ColUint32(TheMap[anOffset+EUint8])==KMaxTUint8);
	test(TheView.ColUint(TheMap[anOffset+EUint8])==KMaxTUint8);
	test(TheView.ColInt16(TheMap[anOffset+EInt16])==KMaxTInt16);
	test(TheView.ColInt32(TheMap[anOffset+EInt16])==KMaxTInt16);
	test(TheView.ColInt(TheMap[anOffset+EInt16])==KMaxTInt16);
	test(TheView.ColUint16(TheMap[anOffset+EUint16])==KMaxTUint16);
	test(TheView.ColUint32(TheMap[anOffset+EUint16])==KMaxTUint16);
	test(TheView.ColUint(TheMap[anOffset+EUint16])==KMaxTUint16);
	test(TheView.ColInt32(TheMap[anOffset+EInt32])==KMaxTInt32);
	test(TheView.ColInt(TheMap[anOffset+EInt32])==KMaxTInt32);
	test(TheView.ColUint32(TheMap[anOffset+EUint32])==KMaxTUint32);
	test(TheView.ColUint(TheMap[anOffset+EUint32])==KMaxTUint32);
	test(TheView.ColInt64(TheMap[anOffset+EInt64])==KMaxTInt64);
	test(TheView.ColReal32(TheMap[anOffset+EReal32])==KMaxTReal32);
	test(TheView.ColReal64(TheMap[anOffset+EReal64])==KMaxTReal64);
	test(TheView.ColReal(TheMap[anOffset+EReal64])==KMaxTReal64);
	test(TheView.ColTime(TheMap[anOffset+ETime])==KMaxTTime);
	test(TheView.ColDes8(TheMap[anOffset+EText8])==KTestText8);
	test(TheView.ColSize(TheMap[anOffset+EText8])==KTestText8.Size());
	test(TheView.ColLength(TheMap[anOffset+EText8])==KTestText8.Length());
	test(TheView.ColDes16(TheMap[anOffset+EText16])==KTestText16);
	test(TheView.ColSize(TheMap[anOffset+EText16])==KTestText16.Size());
	test(TheView.ColLength(TheMap[anOffset+EText16])==KTestText16.Length());
	test(TheView.ColDes8(TheMap[anOffset+ETBinary])==TPtrC8((TUint8*)&TheMap[0],sizeof(TheMap)));
	test(TheView.ColSize(TheMap[anOffset+ETBinary])==sizeof(TheMap));
	test(TheView.ColLength(TheMap[anOffset+ETBinary])==sizeof(TheMap));
//
	test(TheView.ColSize(TheMap[anOffset+ELongText8])==KTestLongText8.Size());
	test(TheView.ColLength(TheMap[anOffset+ELongText8])==KTestLongText8.Length());
	RDbColReadStream in;
	in.OpenLC(TheView,TheMap[anOffset+ELongText8]);
	in.ReadL(TheBuf8,TheView.ColLength(TheMap[anOffset+ELongText8]));
	CleanupStack::PopAndDestroy();
	test(TheBuf8==KTestLongText8);
//
	test(TheView.ColSize(TheMap[anOffset+ELongText16])==KTestLongText16.Size());
	test(TheView.ColLength(TheMap[anOffset+ELongText16])==KTestLongText16.Length());
	in.OpenLC(TheView,TheMap[anOffset+ELongText16]);
	in.ReadL(TheBuf16,TheView.ColLength(TheMap[anOffset+ELongText16]));
	CleanupStack::PopAndDestroy();
	test(TheBuf16==KTestLongText16);
//
	test(TheView.ColSize(TheMap[anOffset+ELongBinary])==KBlobItems*sizeof(TUint32));
	test(TheView.ColLength(TheMap[anOffset+ELongBinary])==KBlobItems*sizeof(TUint32));
	in.OpenLC(TheView,TheMap[anOffset+ELongBinary]);
	for (TInt ii=0;ii<KBlobItems;++ii)
		test(in.ReadInt32L()==ii);
	CleanupStack::PopAndDestroy();
	}

//
// Check the row is the same as was written
//
LOCAL_C void CheckMaximum(TInt aCounter)
	{
	TheView.GetL();
	test(TheView.ColInt32(TheMap[ECounter])==aCounter);
	CheckMaxValues(0);
	CheckMaxValues(EBitNull-EBit);
	}

//
// Write a row with null column values in the nullable columns
// return the value in the counter column
//
LOCAL_C TInt WriteNull()
	{
	TheView.InsertL();
	WriteMinValues(0);
	test(!TheView.IsColNull(TheMap[ECounter]));
	TInt cc=TheView.ColInt(TheMap[ECounter]);
	TheView.PutL();
	return cc;
	}

//
// Check the row is the same as was written
//
LOCAL_C void CheckNull(TInt aCounter)
	{
	TheView.GetL();
	test(TheView.ColInt32(TheMap[ECounter])==aCounter);
	TInt ii;
	for (ii=ECounter;ii<EBitNull;ii++)
		{	// not null columns
		TDbColNo col=TheMap[ii];
		test(!TheView.IsColNull(col));
		test(TheView.ColSize(col)!=0);
		test(TheView.ColLength(col)!=0);
		if (ii<ELongText8)
			test(TheView.ColDes8(col).Length()!=0);
		}
	CheckMinValues(0);
	for (;ii<EForceNull-1;++ii)
		{	// null columns
		TDbColNo col=TheMap[ii];
		test(TheView.IsColNull(col));
		test(TheView.ColSize(col)==0);
		test(TheView.ColLength(col)==0);
		if (ii<ELongText8Null)
			test(TheView.ColDes8(col).Length()==0);
		}
	test(TheView.ColUint(TheMap[EBitNull])==0);
	test(TheView.ColInt(TheMap[EInt8Null])==0);
	test(TheView.ColUint(TheMap[EUint8Null])==0);
	test(TheView.ColInt(TheMap[EInt16Null])==0);
	test(TheView.ColUint(TheMap[EUint16Null])==0);
	test(TheView.ColInt(TheMap[EInt32Null])==0);
	test(TheView.ColUint(TheMap[EUint32Null])==0);
	test(TheView.ColInt64(TheMap[EInt64Null])==0);
	test(TheView.ColReal32(TheMap[EReal32Null])==0);
	test(TheView.ColReal64(TheMap[EReal64Null])==0);
	test(TheView.ColTime(TheMap[ETimeNull])==TTime(0));
	}

//
// Copy the last row (should have a different auto-inc value)
//
LOCAL_C TInt WriteCopy()
	{
	TheView.LastL();
	TheView.InsertCopyL();
	TheView.SetColL(TheMap[EForceNull],1234567);
	test(!TheView.IsColNull(TheMap[ECounter]));
	TInt cc=TheView.ColInt(TheMap[ECounter]);
	TheView.PutL();
	return cc;
	}

template <class T>
void TestOverflow(TDbColNo aCol,const T& aValue)
	{
	TheView.UpdateL();
	TheView.SetColL(aCol,aValue);
	TRAPD(r,TheView.PutL());
	test(r==KErrOverflow);
	TheView.Cancel();
	}

LOCAL_C void TestWriteNull(TDbColNo aCol)
	{
	TheView.UpdateL();
	TheView.SetColNullL(aCol);
	TRAPD(r,TheView.PutL());
	test(r==KErrNotFound);
	TheView.Cancel();
	}

LOCAL_C void TestValidation()
	{
	TheView.InsertL();
	WriteMinValues(0);
	TheView.PutL();
	TestOverflow(TheMap[EBit],TUint(2));
	TestOverflow(TheMap[EBit],TUint(0xffffffffu));
	TestOverflow(TheMap[EInt8],TInt(-129));
	TestOverflow(TheMap[EInt8],TInt(128));
	TestOverflow(TheMap[EUint8],TUint(0xffffffffu));
	TestOverflow(TheMap[EUint8],TUint(256));
	TestOverflow(TheMap[EInt16],TInt(-32769));
	TestOverflow(TheMap[EInt16],TInt(32768));
	TestOverflow(TheMap[EUint16],TUint(0xffffffffu));
	TestOverflow(TheMap[EUint16],TUint(65536));
	{
	TBuf8<KDbDefaultTextColLength+1> buf;
	buf.SetMax();
	buf.Fill('x');
	TestOverflow(TheMap[EText8],buf);
	}
	{
	TBuf16<KDbDefaultTextColLength+1> buf;
	buf.SetMax();
	buf.Fill('x');
	TestOverflow(TheMap[EText16],buf);
	}
	for (TInt ii=EBit;ii<EBitNull;++ii)
		TestWriteNull(TheMap[ii]);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0597
@SYMTestCaseDesc        Tests the range and values of all columns types
@SYMTestPriority        Medium
@SYMTestActions         Tests for integral accessors 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTypes()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0597 Create table of all types "));
	OpenDatabase();
	CDbColSet* cs=CreateColSetL(EAllTypes);
	test(TheDatabase.CreateTable(KTestTypesTable,*cs)==KErrNone);
	delete cs;
	test(TheView.Prepare(TheDatabase,_L("select * from TestTypesTable"))==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	cs=TheView.ColSetL();
	for (TInt ii=0;ii<KColumns;ii++)
		TheMap[ii]=cs->ColNo(TPtrC(AllColumns[ii].iName));
	delete cs;
	test.Next(_L("Test integral accessors"));
	CheckIntAccessors();
	test.Next(_L("Add row of minimum values"));
	TInt c1=WriteMinimum();
	test.Next(_L("Add row of maximum values"));
	TInt c2=WriteMaximum();
	test(c2>c1);
	test.Next(_L("Add row of Null values"));
	TInt c3=WriteNull();
	test(c3>c2);
	test.Next(_L("Add a copy of the last row"));
	TInt c4=WriteCopy();
	test(c4>c3);
	test.Next(_L("Check minimum values"));
	TheView.FirstL();
	CheckMinimum(c1);
	test.Next(_L("Check maximum values"));
	TheView.NextL();
	CheckMaximum(c2);
	test.Next(_L("Check Null values"));
	TheView.NextL();
	CheckNull(c3);
	TheView.NextL();
	CheckNull(c4);
	test.Next(_L("Test column value Validation"));
	TestValidation();
	TheView.Close();
	CloseDatabase();
	test.End();
	}

const TPtrC KColumnID(_S("id"));
const TPtrC KColumnText(_S("txt"));
const TPtrC KColumnOther(_S("other"));
const TPtrC KBlobText(_S("text"));
const TPtrC KFind1(_S("id=1"));
const TPtrC KFind2(_S("id is null"));

// records will be:
// 0: "text"
// 1: "aaa...aaatext"
// 2: "textaaa...aaa"
// 3: "aaa...aaa"
// 4: "aaa...aaatextaaa...aaa"
// 5: ""

struct STest
	{
	const TText* iText;
	TUint iRecs;
	};

LOCAL_D STest const Tests[]=
	{
		{_S("txt like 'a*'"),0x010110},
		{_S("txt like 'a*' or other like 'a*'"),0x111111},
		{_S("txt like 'a*' and other like 'a*'"),0x010110},
		{_S("txt < 'text'"),0x010111},
		{_S("txt > 'text'"),0x001000},
		{_S("txt > ''"),0x111110},
		{_S("txt like 'text'"),0x100000},
		{_S("txt like '*TEXT'"),0x110000},
		{_S("txt like 'text*'"),0x101000},
		{_S("txt like '*text*'"),0x111010},
		{_S("txt like '*'"),0x111111},
		{_S("txt like '?*'"),0x111110},
		{_S("txt like '*t*t*'"),0x111010},
		{_S("txt like '*a??t*'"),0x010010},
		{_S("txt like 'aA*aa'"),0x000110},
		{_S("txt like 'teXT'"),0x100000},
		{_S("txt like '*text'"),0x110000},
		{_S("txt like '*tExt*'"),0x111010},
		{_S("txt like ''"),0x000001},
		{_S("txt is null"),0x000001}
	};

LOCAL_C void CreateFindTable()
	{
	CDbColSet *pC=CDbColSet::NewL();
	CleanupStack::PushL(pC);
	pC->AddL(TDbCol(KColumnID,EDbColUint32));
	pC->AddL(TDbCol(KColumnText,EDbColLongText,200));
	pC->AddL(TDbCol(KColumnOther,EDbColText,200));
	test(TheDatabase.CreateTable(KTestFindTable,*pC)==KErrNone);
	CleanupStack::PopAndDestroy();
	test(TheTable.Open(TheDatabase,KTestFindTable,TheTable.EInsertOnly)==KErrNone);
	TBuf<0x40> garbage;
	garbage.SetMax();
	garbage.Fill('a');
	TBuf<0xff> col;
	for (TUint ii=0;ii<6;++ii)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,ii);
		TheTable.SetColL(3,_L("abcdef"));

		switch (ii)
			{
		case 0:
			TheTable.SetColL(2,KBlobText);
			break;
		case 1:
			col=garbage;
			col+=KBlobText;
			TheTable.SetColL(2,col);
			break;
		case 2:
			col=KBlobText;
			col+=garbage;
			TheTable.SetColL(2,col);
			break;
		case 3:
			TheTable.SetColL(2,garbage);
			break;
		case 4:
			col=garbage;
			col+=KBlobText;
			col+=garbage;
			TheTable.SetColL(2,col);
			break;
		case 5:
			break;
			}
		TheTable.PutL();
		}
	TheTable.Close();
	CDbKey* key=CDbKey::NewLC();
	key->AddL(KColumnID);
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTestFindTable,*key)==KErrNone);
	key->Clear();
	key->AddL(TDbKeyCol(KColumnText,100));
	key->MakeUnique();
	key->SetComparison(EDbCompareFolded);
	test(TheDatabase.CreateIndex(KIndexName2,KTestFindTable,*key)==KErrNone);
	CleanupStack::PopAndDestroy(); //key
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0598
@SYMTestCaseDesc        Tests for RDbRowConstraint::Open() function
@SYMTestPriority        Medium
@SYMTestActions         Tests for the specified SQL search-condition for matching against rows in the specified rowset
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMatch()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0598 Match "));
	test(TheTable.Open(TheDatabase,KTestFindTable,TheTable.EReadOnly)==KErrNone);
	for (TInt ii=0;ii<TInt(sizeof(Tests)/sizeof(Tests[0]));++ii)
		{
		RDbRowConstraint match;
		test(match.Open(TheTable,TDbQuery(TPtrC(Tests[ii].iText),EDbCompareFolded))==KErrNone);
		TUint mask=0;
		TheTable.BeginningL();
		while (TheTable.NextL())
			{
			if (TheTable.MatchL(match))
				{
				TheTable.GetL();
				TUint bit=0x100000>>(TheTable.ColUint32(1)*4);
				test((bit&mask)==0);
				mask|=bit;
				}
			}
		match.Close();
		test(mask==Tests[ii].iRecs);
		}
	TheTable.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0599
@SYMTestCaseDesc        Tests for RDbRowSet::FindL(),RDbRowSet::GetL() functions
@SYMTestPriority        Medium
@SYMTestActions         Tests for finding a match through a rowset 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFind(RDbRowSet& aSet)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0599 "));
	CDbColSet* cs=aSet.ColSetL();
	test(cs!=0);
	TDbColNo col=cs->ColNo(KColumnID);
	test(col!=KDbNullColNo);
	delete cs;
	aSet.FirstL();
	TInt p1=aSet.FindL(aSet.EForwards,KFind1);
	test(p1>=0);
	aSet.GetL();
	test(aSet.ColUint(col)==1);
	test(aSet.FindL(aSet.EForwards,KFind1)==0);
	aSet.GetL();
	test(aSet.ColUint(col)==1);
	if (aSet.NextL())
		test(aSet.FindL(aSet.EForwards,KFind1)==KErrNotFound);
	aSet.FirstL();
	test(aSet.FindL(aSet.EForwards,KFind2)==KErrNotFound);
	aSet.LastL();
	TInt p2=aSet.FindL(aSet.EBackwards,KFind1);
	test(p2>=0);
	aSet.GetL();
	test(aSet.ColUint(col)==1);
	test(aSet.FindL(aSet.EBackwards,KFind1)==0);
	aSet.GetL();
	test(aSet.ColUint(col)==1);
	if (aSet.PreviousL())
		test(aSet.FindL(aSet.EBackwards,KFind1)==KErrNotFound);
	aSet.LastL();
	test(aSet.FindL(aSet.EBackwards,KFind2)==KErrNotFound);
	test(p1+p2+1==aSet.CountL());
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0600
@SYMTestCaseDesc        Tests for SQL find
@SYMTestPriority        Medium
@SYMTestActions         Tests for SQL querying a table
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestSQLFind(const TText* aSql)
	{
    test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0600 "));
	test(TheView.Prepare(TheDatabase,TDbQuery(TPtrC(aSql),EDbCompareFolded),TheView.EReadOnly)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TestFind(TheView);
	TheView.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0601
@SYMTestCaseDesc        Tests for DBMS limits
@SYMTestPriority        Medium
@SYMTestActions         Tests for maximum length boundaries
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDbmsBoundaries()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0601 "));
	// Test KMaxSegmentLength boundary

	_LIT(KMaxSegmentLengthMinusOne,"txt like \
		'*01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567@0.com*' \
		  or other like \
		'*01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567@0.com*'");
	_LIT(KMaxSegmentLengthExact,"txt like \
		'*012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678@0.com*' \
		  or other like \
		'*012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678@0.com*'");
	_LIT(KMaxSegmentLengthPlusOne,"txt like \
		'*0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789@0.com*' \
		  or other like \
		'*0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789@0.com*'");

	TInt ret=KErrNone;
	RDbRowConstraint match;

	test(TheTable.Open(TheDatabase,KTestFindTable,TheTable.EReadOnly)==KErrNone);

	// Test one less than the boundary
	ret = match.Open(TheTable,TDbQuery(KMaxSegmentLengthMinusOne,EDbCompareFolded));
	match.Close();
	test(ret==KErrNone);

	// Test the boundary
	ret = match.Open(TheTable,TDbQuery(KMaxSegmentLengthExact,EDbCompareFolded));
	match.Close();
	test(ret==KErrNone);

	// Test one more than the boundary
	ret = match.Open(TheTable,TDbQuery(KMaxSegmentLengthPlusOne,EDbCompareFolded));
	match.Close();
	test(ret==KErrArgument);

	TheTable.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0602
@SYMTestCaseDesc        Tests for table order,SQL query,
@SYMTestPriority        Medium
@SYMTestActions         Tests for finding a row in a rowset,dbms boundaries,SQL querying
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFind()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0602 Table order "));
	test(TheTable.Open(TheDatabase,KTestFindTable,TheTable.EReadOnly)==KErrNone);
	TestFind(TheTable);
	test.Next(_L("Index order"));
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	TestFind(TheTable);
	test(TheTable.SetIndex(KIndexName2)==KErrNone);
	TestFind(TheTable);
	TheTable.Close();

	test.Next(_L("SQL query test boundary"));
	TestDbmsBoundaries();

	test.Next(_L("SQL query"));
	TestSQLFind(_S("select id from TestFindTable where id=1"));
	TestSQLFind(_S("select * from TestFindTable where id<=2"));
	TestSQLFind(_S("select txt,id from TestFindTable order by id desc"));
	TestSQLFind(_S("select id,txt from TestFindTable where id=1 or txt like '*text*'"));
	TestSQLFind(_S("select * from TestFindTable where id=1 or txt is not null order by id"));
	TestSQLFind(_S("select id from TestFindTable where id<>3 order by txt"));
	test.End();
	}

LOCAL_C void TestBookmark(RDbRowSet& aSet)
	{
	aSet.BeginningL();
	while (aSet.NextL())
		{
		aSet.GetL();
		TUint id=aSet.ColUint(1);
		TDbBookmark mark=aSet.Bookmark();
		aSet.BeginningL();
		aSet.GotoL(mark);
		aSet.GetL();
		test(aSet.ColUint(1)==id);
		aSet.NextL();
		aSet.PreviousL();
		aSet.PreviousL();
		aSet.NextL();
		aSet.GetL();
		test(aSet.ColUint(1)==id);
		aSet.GotoL(mark);
		};
	}

LOCAL_C void TestSQLBookmark(const TText* aSql)
	{
	test(TheView.Prepare(TheDatabase,TDbQuery(TPtrC(aSql),EDbCompareFolded),TheView.EReadOnly)==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	TestBookmark(TheView);
	TheView.Close();
	}

LOCAL_C void TestBookmark()
	{
	test.Start(_L("Table order"));
	test(TheTable.Open(TheDatabase,KTestFindTable,TheTable.EReadOnly)==KErrNone);
	TestBookmark(TheTable);
	test.Next(_L("Index order"));
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	TestBookmark(TheTable);
	test(TheTable.SetIndex(KIndexName2)==KErrNone);
	TestBookmark(TheTable);
	TheTable.Close();
	test.Next(_L("SQL query"));
	TestSQLBookmark(_S("select * from TestFindTable where id=1"));
	TestSQLBookmark(_S("select * from TestFindTable where id>2"));
	TestSQLBookmark(_S("select * from TestFindTable order by id desc"));
	TestSQLBookmark(_S("select * from TestFindTable where txt like '*text*'"));
	TestSQLBookmark(_S("select * from TestFindTable where txt is not null order by id"));
	TestSQLBookmark(_S("select * from TestFindTable where id <> 3 order by txt"));
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0603
@SYMTestCaseDesc        Tests for find and bookmark
@SYMTestPriority        Medium
@SYMTestActions         Executes Match,Find,Bookmarks tests 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFindAndBookmark()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0603 Create Table "));
	OpenDatabase();
	CreateFindTable();
	test.Next(_L("Match"));
	TestMatch();
	test.Next(_L("Find"));
	TestFind();
	test.Next(_L("Bookmarks"));
	TestBookmark();
	test.Next(_L("Close"));
	CloseDatabase();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0604
@SYMTestCaseDesc        Tests for multi view
@SYMTestPriority        Medium
@SYMTestActions         Tests for adding and deleting rows 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultiView()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0604 Create Table "));
	OpenDatabase();
	CDbColSet* set=CreateColSetL(ESingleColumn);
	test(TheDatabase.CreateTable(KTestMultiTable,*set)==KErrNone);
	delete set;
	test.Next(_L("Add and modify rows"));
	test(TheTable.Open(TheDatabase,KTestMultiTable)==KErrNone);
	test(TheView.Prepare(TheDatabase,_L("select * from TestMultiTable"))==KErrNone);
	test(TheView.EvaluateAll()==KErrNone);
	for (TInt ii=1;ii<=10;++ii)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,ii);
		TheTable.PutL();
		TheView.NextL();
		TheView.GetL();
		test(TheView.ColInt(1)==ii);
		TheView.UpdateL();
		TheView.SetColL(1,ii+100);
		TheView.PutL();
		test(TheTable.ColInt(1)==ii);
		TheTable.GetL();
		test(TheTable.ColInt(1)==ii+100);
		}
	test.Next(_L("delete rows"));
	TheView.FirstL();
	TheTable.FirstL();
	TheView.GetL();
	test (TheView.ColInt(1)==1+100);
	TheTable.DeleteL();
	TRAPD(r,TheView.GetL());
	test(r!=KErrNone);
	TheView.NextL();
	TheView.GetL();
	test (TheView.ColInt(1)==2+100);
	TheView.DeleteL();
	test(!TheView.PreviousL());
	TheTable.NextL();
	test(!TheTable.PreviousL());
	test.End();
	TheView.Close();
	TheTable.Close();
	CloseDatabase();
	}
	
LOCAL_C void ValidateUID3(const TDesC& aDbName, const TUid& aUid)
	{
	test.Next(_L("Read DB header"));
	RFile dbFile;
	TInt err = dbFile.Open(TheFs, aDbName, EFileRead);
	test(err==KErrNone);
	TBuf8<100> contents; // header of a new DB is approx 77 bytes
	err = dbFile.Read(contents);
	dbFile.Close();
	test(err==KErrNone);
	
	test.Next(_L("Check UID3"));
	const TPtrC8 ptr((const TUint8*) &aUid.iUid, sizeof(aUid.iUid));
	TInt pos = contents.Find(ptr);
	test(pos==8); // UID3 begins at byte pos 8
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1372
@SYMTestCaseDesc        Tests for UID3 being set in DB header
@SYMTestPriority        Medium
@SYMTestActions         Creates a new DB and checks UID3
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDbUID3()
	{
	const TUid KOrigUid = {0x19768253}; // random UID
	const TUid KReplaceUid = {0x24731264}; // different random UID
#ifndef __TOOLS2__
	const TPtrC KTempDbName(_S("C:\\dbms-tst\\T_DBMS_UID3.DB"));
#else
	const TPtrC KTempDbName(_S(".\\dbms-tst\\T_DBMS_UID3.DB"));
#endif
	
	(void)TheFs.Delete(KTempDbName);
	
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1372 Create Database "));
	RDbNamedDatabase tempDb;
	TInt err = tempDb.Create(TheFs, KTempDbName, KOrigUid.Name());
	test(err==KErrNone);
	tempDb.Close();
	
	ValidateUID3(KTempDbName, KOrigUid);
	
	test.Next(_L("Replace Database"));
	err = tempDb.Replace(TheFs, KTempDbName, KReplaceUid.Name());
	test(err==KErrNone);
	tempDb.Close();
	
	ValidateUID3(KTempDbName, KReplaceUid);
	
	test.Next(_L("Replace Database using default UID3"));
	err = tempDb.Replace(TheFs, KTempDbName);
	test(err==KErrNone);
	tempDb.Close();
	
	ValidateUID3(KTempDbName, KNullUid);
	
	(void)TheFs.Delete(KTempDbName);
	test.End();
	}

LOCAL_C void Test()
	{
	__UHEAP_MARK;
	test.Start(_L("DDL"));
	TRAPD(r,TestDDL();)
	test(r==KErrNone);
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Navigation/Edit"));
	TRAP(r,TestView();)
	test(r==KErrNone);
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Column Types"));
	TRAP(r,TestTypes();)
	test(r==KErrNone);
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Find, Match & Bookmarks"));
	TRAP(r,TestFindAndBookmark();)
	test(r==KErrNone);
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Multiple view updates"));
	TRAP(r,TestMultiView();)
	test(r==KErrNone);
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Database UID3"));
	TRAP(r,TestDbUID3();)
	test(r==KErrNone);
	test.End();
	__UHEAP_MARKEND;
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	test(r==KErrNone);
//
	r=TheFs.MkDir(KTestDatabase);
	test(r==KErrNone || r==KErrAlreadyExists);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
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

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
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
	}

//
// Test streaming conversions.
//
GLDEF_C TInt E32Main()
    {
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
#ifndef __TOOLS2__
	TInt r=TheDbs.Connect();
	test (r==KErrNone);
#endif
	test.Start(_L("Standard database"));
	Test();
	test.Next(_L("Secure database"));
	Test();
#ifndef __TOOLS2__
	TheDbs.Close();
#endif

#ifndef __linux__
	TInt err;
#ifndef __TOOLS2__
	TRAPD(lc, err = TheCrcChecker.DumpCrcRecordsL(KCrcRecord));
	test(err==KErrNone);
	test(lc==KErrNone);
#else
	TRAPD(lc, err = TheCrcChecker.ValidateCrcRecordsL(KCrcRecord));
	TPtrC errmsg;
	TheCrcChecker.ErrorReportL(err, errmsg);
	RDebug::Print(errmsg);
	test(err==KErrNone || err==TDBMS_CRCChecks::ECrcCheckOk);
#endif // TOOLS2
#endif // linux

	test.Next(_L("Waiting for server exit"));
	const TUint KExitDelay=6*0x100000;	// ~6 seconds
	User::After(KExitDelay);
	test.End();
//
	__UHEAP_MARKEND;
	delete TheTrapCleanup;

	::DeleteDataFile(KTestDatabase);

	TheFs.Close();
	test.Close();
	return 0;
    }
