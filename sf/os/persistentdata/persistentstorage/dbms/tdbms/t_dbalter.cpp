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

LOCAL_D RTest test(_L("t_dbalter : Test AlterTable"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CFileStore* TheStore;
LOCAL_D RDbStoreDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RFs TheFs;

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDir=_L("C:\\DBMS-TST\\");
const TPtrC KTestFile=_L("T_ALTER.DB");
const TPtrC KTableName(_S("Table"));
const TPtrC KTableName2(_S("Table2"));
const TPtrC KIndexName(_S("Index"));

TInt KRecords=100;

const TUint KCol1Data=0;
const TInt KCol2Data=2;
const TPtrC KCol3Data=_L("three");
const TUint8 _Col4Data[80]={4,4,4,4,0,0xff,2,2,1};
const TPtrC8 KCol4Data(_Col4Data,sizeof(_Col4Data));
const TUint KCol5Data=1;
const TInt KCol6Data=5;
const TPtrC KCol7Data=_L("six");
const TPtrC KCol8Data=_L("column number eight = #8");
const TUint8 _Col9Data[400]={1,2,3,4,5,6,7,8,9,10};
const TPtrC8 KCol9Data(_Col9Data,sizeof(_Col9Data));

const TText* const KColumn1=_S("c1");
const TText* const KColumn2=_S("c2");
const TText* const KColumn3=_S("c3");
const TText* const KColumn4=_S("c4");
const TText* const KColumn5=_S("c5");
const TText* const KColumn6=_S("c6");
const TText* const KColumn7=_S("c7");
const TText* const KColumn8=_S("c8");
const TText* const KColumn9=_S("c9");
const TText* const KColumn10=_S("c10");
const TText* const KColumn11=_S("c11");
const TPtrC KColumns[]=
	{
	KColumn1,
	KColumn2,
	KColumn3,
	KColumn4,
	KColumn5,
	KColumn6,
	KColumn7,
	KColumn8,
	KColumn9
	};

class Set
	{
public:
	struct SColDef
		{
		const TText* iName;
		TDbColType iType;
		TInt iAttributes;
		TInt iMaxLength;
		};
	static SColDef const Basic[];
	static SColDef const Bad[];
	static SColDef const Incompatible1[];
	static SColDef const Incompatible2[];
	static SColDef const Incompatible3[];
	static SColDef const Different[];
	static SColDef const Extended[];
	static SColDef const LongerText[];
	static SColDef const TextToLongText[];
	static SColDef const Column3[];
	static SColDef const DropSome[];
	static SColDef const DropAndAdd[];
public:
	static CDbColSet* CreateL(const SColDef* aDef);
	};
// the basic column definition
enum TCol {EBit,EInt,EText,ELong,EBitNull,EIntNull,ETextNull,ELongNull,EExtra};
Set::SColDef const Set::Basic[]=
	{
	{KColumn1,EDbColBit,TDbCol::ENotNull,-1},
	{KColumn2,EDbColInt32,TDbCol::ENotNull,-1},
	{KColumn3,EDbColText,TDbCol::ENotNull,-1},
	{KColumn4,EDbColLongBinary,TDbCol::ENotNull,-1},
	{KColumn5,EDbColBit,0,-1},
	{KColumn6,EDbColInt32,0,-1},
	{KColumn7,EDbColText,0,-1},
	{KColumn8,EDbColText,0,50},
	{0}
	};
// a basically invalid set
Set::SColDef const Set::Bad[]=
	{
	{KColumn9,EDbColInt32,0,-1},
	{KColumn9,EDbColInt32,0,-1},
	{0}
	};
// an incompatible set with Basic
Set::SColDef const Set::Incompatible1[]=
	{
	{KColumn1,EDbColInt32,TDbCol::ENotNull,-1},	// retype a column
	{0}
	};
Set::SColDef const Set::Incompatible2[]=
	{
	{KColumn5,EDbColBit,TDbCol::ENotNull,-1},	// change attributes
	{0}
	};
Set::SColDef const Set::Incompatible3[]=
	{
	{KColumn8,EDbColText,0,49},	// shrink a text column
	{0}
	};
// a wildly different set
Set::SColDef const Set::Different[]=
	{
	{KColumn11,EDbColInt32,0,-1},
	{KColumn4,EDbColLongBinary,TDbCol::ENotNull,-1},
	{KColumn10,EDbColBit,TDbCol::ENotNull,-1},
	{KColumn3,EDbColText,TDbCol::ENotNull,-1},
	{0}
	};
// basic + 1 column
Set::SColDef const Set::Extended[]=
	{
	{KColumn1,EDbColBit,TDbCol::ENotNull,-1},
	{KColumn2,EDbColInt32,TDbCol::ENotNull,-1},
	{KColumn3,EDbColText,TDbCol::ENotNull,-1},
	{KColumn4,EDbColLongBinary,TDbCol::ENotNull,-1},
	{KColumn5,EDbColBit,0,-1},
	{KColumn6,EDbColInt32,0,-1},
	{KColumn7,EDbColText,0,-1},
	{KColumn8,EDbColText,0,50},
	{KColumn9,EDbColLongBinary,0,-1},		// add this column
	{0}
	};
// Extended with a longer text column
Set::SColDef const Set::LongerText[]=
	{
	{KColumn1,EDbColBit,TDbCol::ENotNull,-1},
	{KColumn2,EDbColInt32,TDbCol::ENotNull,-1},
	{KColumn3,EDbColText,TDbCol::ENotNull,-1},
	{KColumn4,EDbColLongBinary,TDbCol::ENotNull,-1},
	{KColumn5,EDbColBit,0,-1},
	{KColumn6,EDbColInt32,0,-1},
	{KColumn7,EDbColText,0,-1},
	{KColumn8,EDbColText,0,51},			// longer definition
	{KColumn9,EDbColLongBinary,0,-1},
	{0}
	};
// Extended with a text->LongText column
Set::SColDef const Set::TextToLongText[]=
	{
	{KColumn1,EDbColBit,TDbCol::ENotNull,-1},
	{KColumn2,EDbColInt32,TDbCol::ENotNull,-1},
	{KColumn3,EDbColText,TDbCol::ENotNull,-1},
	{KColumn4,EDbColLongBinary,TDbCol::ENotNull,-1},
	{KColumn5,EDbColBit,0,-1},
	{KColumn6,EDbColInt32,0,-1},
	{KColumn7,EDbColText,0,-1},
	{KColumn8,EDbColLongText,0,-1},		// longer still
	{KColumn9,EDbColLongBinary,0,-1},
	{0}
	};
Set::SColDef const Set::Column3[]=
	{
	{KColumn3,EDbColText,TDbCol::ENotNull,-1},
	{0}
	};
Set::SColDef const Set::DropSome[]=
	{
	{KColumn1,EDbColBit,TDbCol::ENotNull,-1},
	{KColumn2,EDbColInt32,TDbCol::ENotNull,-1},
	{KColumn6,EDbColInt32,0,-1},
	{KColumn7,EDbColText,0,-1},
	{0}
	};
Set::SColDef const Set::DropAndAdd[]=
	{
	{KColumn2,EDbColInt32,TDbCol::ENotNull,-1},
	{KColumn7,EDbColText,0,-1},
	{KColumn10,EDbColBinary,0,-1},
	{0}
	};

CDbColSet* Set::CreateL(const SColDef* aDef)
	{
	CDbColSet *set=CDbColSet::NewLC();
	for (;aDef->iName;++aDef)
		{
		TDbCol col(TPtrC(aDef->iName),aDef->iType);
		col.iAttributes=aDef->iAttributes;
		if (aDef->iMaxLength>=0)
			col.iMaxLength=aDef->iMaxLength;
		set->AddL(col);
		}
	CleanupStack::Pop();
	return set;
	}

//
// Create the database-in-a-store
//
LOCAL_C void CreateDatabaseL()
	{
	CFileStore* store=CPermanentFileStore::ReplaceLC(TheFs,KTestFile,EFileRead|EFileWrite);
	store->SetTypeL(KPermanentFileStoreLayoutUid);
	TStreamId id;
	    id=TheDatabase.CreateL(store);
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::Pop();
	TheStore=store;
	}

//
// Open the database-in-a-store
//
LOCAL_C void OpenDatabaseL()
	{
	CFileStore* store=CFileStore::OpenLC(TheFs,KTestFile,EFileRead|EFileWrite);
	TStreamId id=store->Root();
	    TheDatabase.OpenL(store,id);
	CleanupStack::Pop();
	TheStore=store;
	}

LOCAL_C void CloseDatabaseL()
	{
	TheDatabase.Close();
	delete TheStore;
	}

LOCAL_C void DestroyDatabaseL()
	{
	TheDatabase.Destroy();
	TheStore->CommitL();
	delete TheStore;
	}

LOCAL_C CDbColSet* TableDefinitionL(const TDesC& aTable)
	{
	RDbTable table;
	test(table.Open(TheDatabase,aTable,table.EReadOnly)==KErrNone);
	CDbColSet* cs=table.ColSetL();
	table.Close();
	return cs;
	}

//
// Compare two column sets
//
LOCAL_C void Compare(const CDbColSet& aLeft,const CDbColSet& aRight)
	{
	test(aLeft.Count()==aRight.Count());
	for (TDbColSetIter iter(aLeft);iter;++iter)
		{
		const TDbCol* pRight=aRight.Col(iter->iName);
		test(pRight!=NULL);
		test(iter->iType==pRight->iType);
		test(iter->iMaxLength==KDbUndefinedLength || pRight->iMaxLength==KDbUndefinedLength || iter->iMaxLength==pRight->iMaxLength);
		test((iter->iAttributes&pRight->iAttributes)==iter->iAttributes);
		}
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0575
@SYMTestCaseDesc        Store database test
                        Test for altering the table with different column definitions
@SYMTestPriority        Medium
@SYMTestActions        	Test for RDbStoreDatabase::AlterTable(),RDbStoreDatabase::DropIndex()
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEmptyTableL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0575 Create table "));
	CreateDatabaseL();
	CDbColSet* set=Set::CreateL(Set::Basic);
	test(TheDatabase.CreateTable(KTableName,*set)==KErrNone);
	test.Next(_L("Alter non existant table"));
	test(TheDatabase.AlterTable(KTableName2,*set)==KErrNotFound);
	delete set;
//
	test.Next(_L("Alter to bad definitions"));
	set=Set::CreateL(Set::Bad);
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
	delete set;
	set=Set::CreateL(Set::Incompatible1);
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
	delete set;
	set=Set::CreateL(Set::Incompatible2);
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
	delete set;
	set=Set::CreateL(Set::Incompatible3);
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
	delete set;
//
	test.Next(_L("Drop an indexed column"));
	CDbKey* key=CDbKey::NewLC();
	key->AddL(TPtrC(KColumn2));
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	set=TableDefinitionL(KTableName);
	set->Remove(TPtrC(KColumn2));
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
	test(TheDatabase.DropIndex(KIndexName,KTableName)==KErrNone);
	delete set;
//
	test.Next(_L("Extend an indexed text column"));
	set=Set::CreateL(Set::Extended);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
	key=CDbKey::NewLC();
	key->AddL(TPtrC(KColumn8));
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	set=Set::CreateL(Set::LongerText);
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
	test(TheDatabase.DropIndex(KIndexName,KTableName)==KErrNone);
//
	test.Next(_L("Extend a text column"));
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
//
	test.Next(_L("Extend a text column to a LongText column"));
	set=Set::CreateL(Set::TextToLongText);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
//
	test.Next(_L("Alter to a very different set"));
	set=Set::CreateL(Set::Different);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	CloseDatabaseL();
	OpenDatabaseL();
	CDbColSet* def=TableDefinitionL(KTableName);
	Compare(*set,*def);
	delete def;
	delete set;
	test.End();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	DestroyDatabaseL();
	}

class Map
	{
public:
	Map();
	void Init(RDbRowSet& aSet);
	inline TDbColNo operator[](TInt aCol) const
		{return iMap[aCol];}
private:
	TDbColNo iMap[EExtra+1];
	};

Map::Map()
	{
	}

void Map::Init(RDbRowSet& aSet)
	{
	CDbColSet* set=NULL;
	TRAPD(errCode, set=aSet.ColSetL());
	if(errCode != KErrNone)
		{
		return;
		}
	for (TInt ii=EBit;ii<=EExtra;++ii)
		iMap[ii]=set->ColNo(KColumns[ii]);
	if(set)
		delete set;
	}

//
// Build the table for Altering
//
LOCAL_C void BuildTableL(const Set::SColDef* aDef=Set::Basic)
	{
	CDbColSet* set=Set::CreateL(aDef);
	test(TheDatabase.CreateTable(KTableName,*set)==KErrNone);
	delete set;
	TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName,TheTable.EInsertOnly)==KErrNone);
	Map map;
	map.Init(TheTable);
	for (TInt ii=0;ii<KRecords;++ii)
		{
		TheTable.InsertL();
		TheTable.SetColL(map[EBit],KCol1Data);
		TheTable.SetColL(map[EInt],KCol2Data);
		TheTable.SetColL(map[EText],KCol3Data);
		TheTable.SetColL(map[ELong],KCol4Data);
		if ((ii%EBitNull)==0)
			TheTable.SetColL(map[EBitNull],KCol5Data);
		if ((ii%EIntNull)==0)
			TheTable.SetColL(map[EIntNull],KCol6Data);
		if ((ii%ETextNull)==0)
			TheTable.SetColL(map[ETextNull],KCol7Data);
		if ((ii%ELongNull)==0)
			TheTable.SetColL(map[ELongNull],KCol8Data);
		if (map[EExtra] && (ii%EExtra)==0)
			TheTable.SetColL(map[EExtra],KCol9Data);
		TheTable.PutL();
		}
	TheTable.Close();
	test(TheDatabase.Commit()==KErrNone);
	}

LOCAL_C void CheckBlobL(TDbColNo aCol,const TDesC8& aData)
	{
	test(TheTable.ColSize(aCol)==aData.Size());
	TBuf8<500> buf;
	__ASSERT_DEBUG(buf.MaxLength()>=aData.Length(),User::Invariant());
	RDbColReadStream str;
	str.OpenLC(TheTable,aCol);
	str.ReadL(buf,aData.Length());
	CleanupStack::PopAndDestroy();
	test(buf==aData);
	}

#if defined(UNICODE)
LOCAL_C void CheckBlobL(TDbColNo aCol,const TDesC16& aData)
	{
	test(TheTable.ColSize(aCol)==aData.Size());
	TBuf16<500> buf;
	__ASSERT_DEBUG(buf.MaxLength()>=aData.Length(),User::Invariant());
	RDbColReadStream str;
	str.OpenLC(TheTable,aCol);
	str.ReadL(buf,aData.Length());
	CleanupStack::PopAndDestroy();
	test(buf==aData);
	}
#endif

//
// Check that the columns which still exist, still contain the same stuff
// New columns should be Null
//
LOCAL_C void CheckTableL()
	{
	test(TheTable.Open(TheDatabase,KTableName,TheTable.EReadOnly)==KErrNone);
	Map map;
	map.Init(TheTable);

	for (TInt ii=0;ii<KRecords;++ii)
		{
		test(TheTable.NextL());
		TheTable.GetL();
		if (map[EBit])
			test(TheTable.ColUint(map[EBit])==KCol1Data);
		if (map[EInt])
			test(TheTable.ColInt(map[EInt])==KCol2Data);
		if (map[EText])
			test(TheTable.ColDes(map[EText])==KCol3Data);
		if (map[ELong])
			CheckBlobL(map[ELong],KCol4Data);
		for (TInt jj=EBitNull;jj<=EExtra;++jj)
			{
			if (!map[jj])
				continue;
			if (ii%jj)
				test(TheTable.IsColNull(map[jj]));
			else
				{
				switch (jj)
					{
				case EBitNull:
					test(TheTable.ColUint(map[EBitNull])==KCol5Data);
					break;
				case EIntNull:
					test(TheTable.ColInt(map[EIntNull])==KCol6Data);
					break;
				case ETextNull:
					test(TheTable.ColDes(map[ETextNull])==KCol7Data);
					break;
				case ELongNull:
					CheckBlobL(map[ELongNull],KCol8Data);
					break;
				case EExtra:
					CheckBlobL(map[EExtra],KCol9Data);
					break;
					}
				}
			}
		}
	TheTable.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0576
@SYMTestCaseDesc        Test a full table
@SYMTestPriority        Medium
@SYMTestActions        	Tests for altering the table
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFullTableL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0576 Create database "));
	CreateDatabaseL();
//
	test.Next(_L("Add non-null column"));
	BuildTableL();
	CDbColSet* set=TableDefinitionL(KTableName);
	TDbCol col10=TDbCol(TPtrC(KColumn10),EDbColInt32);
	col10.iAttributes=TDbCol::ENotNull;
	set->AddL(col10);
	test(TheDatabase.AlterTable(KTableName,*set)!=KErrNone);
//
	test.Next(_L("Add nullable column"));
	set->Remove(col10.iName);
	col10.iAttributes=0;
	set->AddL(col10);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	CheckTableL();
//
	test.Next(_L("Drop columns one by one"));
	while (set->Count()>1)
		{
		set->Remove((*set)[1].iName);
		test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
		CheckTableL();
		}
	delete set;
	test(TheDatabase.DropTable(KTableName)==KErrNone);
//
	test.Next(_L("Extend a text column"));
	BuildTableL(Set::Extended);
	set=Set::CreateL(Set::LongerText);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
	CheckTableL();
//
	test.Next(_L("Extend it to a LongText column"));
	set=Set::CreateL(Set::TextToLongText);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
	CheckTableL();
//
	test.Next(_L("Drop all except one"));
	set=Set::CreateL(Set::Column3);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
	CheckTableL();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.Next(_L("Drop single column"));
	for (TInt ii=EBit;ii<=EExtra;++ii)
		{
		BuildTableL(Set::Extended);
		CDbColSet* set=TableDefinitionL(KTableName);
		set->Remove(KColumns[ii]);
		test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
		delete set;
		CheckTableL();
		test(TheDatabase.DropTable(KTableName)==KErrNone);
		}
	test.Next(_L("Drop multiple columns"));
	BuildTableL();
	set=Set::CreateL(Set::DropSome);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
	CheckTableL();
	test.Next(_L("Drop and add together"));
	set=Set::CreateL(Set::DropAndAdd);
	test(TheDatabase.AlterTable(KTableName,*set)==KErrNone);
	delete set;
	CheckTableL();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	DestroyDatabaseL();
	}

LOCAL_C void Test()
	{
	__UHEAP_MARK;
//
	test.Start(_L("Alter empty table"));
	TRAPD(r,TestEmptyTableL();)
	test(r==KErrNone);
	__UHEAP_CHECK(0);
	test.Next(_L("Alter full table"));
	TRAP(r,TestFullTableL();)
	test(r==KErrNone);
	test.End();
//
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
	r=TheFs.MkDir(KTestDir);
	test(r==KErrNone || r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(KTestDir);
	test(r==KErrNone);
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
	test.Start(_L("Standard database"));
	Test();
	test.Next(_L("Secure database"));
	Test();

	// clean up data files used by this test - must be done before call to End() - DEF047652
	_LIT(KTestDbName, "C:\\DBMS-TST\\T_ALTER.DB");
	::DeleteDataFile(KTestDbName);

	test.End();
//
	__UHEAP_MARKEND;
	delete TheTrapCleanup;

	TheFs.Close();
	test.Close();
	return 0;
    }
