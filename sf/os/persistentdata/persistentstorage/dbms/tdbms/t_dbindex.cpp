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
#include "D32TABLE.H"

// MSVC++ up to 5.0 has problems with expanding inline functions
// This disables the mad warnings for the whole project
#if defined(NDEBUG) && defined(__VC32__) && _MSC_VER<=1100
#pragma warning(disable : 4710)			// function not expanded. MSVC 5.0 is stupid
#endif

LOCAL_D RTest test(_L("t_dbindex - Test DBMS indexing and ordering"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbs TheDbs;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RDbView TheView;
LOCAL_D RFs TheFs;
LOCAL_D TBuf<0x200> TheBuf;

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDatabase=_L("C:\\DBMS-TST\\T_INDEX.DB");
const TPtrC KTableName(_S("Table"));
const TPtrC KIndexName(_S("index"));
const TPtrC KIndexTwo(_S("index_two"));
const TPtrC KColumnName(_S("column"));
const TPtrC KColumnTwo(_S("column2"));

const TPtrC KTableName1(_S("Table1"));
const TPtrC KColumnName1(_S("column1"));
const TPtrC KIndexName1(_S("index1"));

const TPtrC KSelectOrdered(_L("select column from table order by column"));

#define elementsof(array) (sizeof(array)/sizeof(array[0]))

void Check(TInt aValue,TInt aExpected,TInt aLine)
	{
	if (aValue!=aExpected)
		{
		test.Printf(_L("*** Expected %d: got %d\r\n"),aExpected,aValue);
		test.operator()(EFalse,aLine);
		}
	}
#define test2(a,b) Check(a,b,__LINE__)

//
// Open the database (shared access) (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C void OpenDatabase()
	{
	TInt r=TheDatabase.Open(TheDbs,KTestDatabase);
	test (r==KErrNone);
	}

//
// Create the database (SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version)
//
LOCAL_C void CreateDatabase()
	{
	TInt r=TheDatabase.Replace(TheFs,KTestDatabase);
	test (r==KErrNone);
	TheDatabase.Close();
	OpenDatabase();
	}


LOCAL_C void CloseDatabase()
	{
	TheDatabase.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0618
@SYMTestCaseDesc        Tests for RDbNamedDatabase functionality
@SYMTestPriority        Medium
@SYMTestActions         Tests for RDbNamedDatabase::CreateIndex()
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestIndexBuildL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0618 Create table "));
	TheDatabase.Begin();
	CDbColSet *cs=CDbColSet::NewLC();
	cs->AddL(TDbCol(KColumnName,EDbColInt32));
	cs->AddL(TDbCol(KColumnTwo,EDbColInt32));
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	test.Next(_L("create indices"));
	CDbKey *key=CDbKey::NewLC();
	key->AddL(TDbKeyCol(KColumnName));
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	key->Clear();
	key->AddL(TDbKeyCol(KColumnTwo));
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexTwo,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	test.Next(_L("Populate table"));
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheTable.InsertL();
	TheTable.SetColL(1,1);
	TheTable.SetColL(2,-1);
	TheTable.PutL();
	TheTable.InsertL();
	TheTable.SetColL(1,2);
	TheTable.SetColL(2,-2);
	TheTable.PutL();
	test(TheDatabase.Commit()==KErrNone);
	test.Next(_L("Check order"));
	test(TheTable.SetNoIndex()==KErrNone);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	test(TheTable.CountL()==2);
	test(TheTable.NextL());
	TheTable.GetL();
	test(TheTable.ColInt(1)==1);
	test(TheTable.NextL());
	TheTable.GetL();
	test(TheTable.ColInt(1)==2);
	test(!TheTable.NextL());
	test(TheTable.SetIndex(KIndexTwo)==KErrNone);
	test(TheTable.CountL()==2);
	test(TheTable.NextL());
	TheTable.GetL();
	test(TheTable.ColInt(1)==2);
	test(TheTable.NextL());
	TheTable.GetL();
	test(TheTable.ColInt(1)==1);
	test(!TheTable.NextL());
 	test(TheTable.SetNoIndex()==KErrNone);
	test(TheTable.CountL()==2);
	test(TheTable.NextL());
	test(TheTable.NextL());
	test(!TheTable.NextL());
	TheTable.Close();
	test.Next(_L("Drop indices"));
	test(TheDatabase.DropIndex(KIndexTwo,KTableName)==KErrNone);
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	test(TheTable.SetIndex(KIndexTwo)!=KErrNone);
	TheTable.Close();
	test(TheDatabase.DropIndex(KIndexName,KTableName)==KErrNone);
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	test(TheTable.SetIndex(KIndexName)!=KErrNone);
	test(TheTable.SetIndex(KIndexTwo)!=KErrNone);
	TheTable.Close();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

LOCAL_C TInt CountRowsL()
	{
	TInt count=0;
	while (TheTable.NextL())
		++count;
	return count;
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0619
@SYMTestCaseDesc        RDbNamedDatabase::Execute() function test
@SYMTestPriority        Medium
@SYMTestActions         Tests for CREATE TABLE,CREATE INDEX and CREATE UNIQUE INDEX query
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestPersistenceL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0619 Create table "));
	TheDatabase.Begin();
	test(TheDatabase.Execute(_L("CREATE TABLE Table (column CHAR(120) NOT NULL)"))==KErrNone);
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheTable.InsertL();
	TheTable.SetColL(1,_L("a"));
	TheTable.PutL();
	TheTable.InsertL();
	TheTable.SetColL(1,_L("b"));
	TheTable.PutL();
	TheTable.InsertL();
	TheTable.SetColL(1,_L("c"));
	TheTable.PutL();
	TheTable.InsertL();
	TheTable.SetColL(1,_L("d"));
	TheTable.PutL();
	TheTable.InsertL();
	TheTable.SetColL(1,_L("e"));
	TheTable.PutL();
	TheTable.Close();
	test(TheDatabase.Commit()==KErrNone);
	test.Next(_L("Build indices"));
	test (TheDatabase.Execute(_L("CREATE INDEX index ON table (column ASC)"))==KErrNone);
	test (TheDatabase.Execute(_L("CREATE UNIQUE INDEX index_two ON table (column DESC)"))==KErrNone);
	test.Next(_L("Close and re-open database"));
	CloseDatabase();
	OpenDatabase();
	test.Next(_L("Check indices"));
	CDbKey* key=TheDatabase.KeyL(KIndexName,KTableName);
	test (key->Count()==1);
	test (key->Comparison()==EDbCompareNormal);
	test (!key->IsUnique());
	test ((*key)[0].iName.CompareF(KColumnName)==0);
	test ((*key)[0].iOrder==TDbKeyCol::EAsc);
	delete key;
	key=TheDatabase.KeyL(KIndexTwo,KTableName);
	test (key->Count()==1);
	test (key->Comparison()==EDbCompareNormal);
	test (key->IsUnique());
	test ((*key)[0].iName.CompareF(KColumnName)==0);
	test ((*key)[0].iOrder==TDbKeyCol::EDesc);
	delete key;
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	test(TheTable.SetNoIndex()==KErrNone);
	test(CountRowsL()==5);
	test(TheTable.CountL()==5);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	test(TheTable.CountL()==5);
	test(CountRowsL()==5);
	test(TheTable.SetIndex(KIndexTwo)==KErrNone);
	test(TheTable.CountL()==5);
	test(CountRowsL()==5);
	TheTable.Close();
	test.Next(_L("Drop indices"));
	TheDatabase.Begin();
	test (TheDatabase.Execute(_L("DROP INDEX index_two FROM table"))==KErrNone);
	test (TheDatabase.Execute(_L("DROP INDEX index FROM table"))==KErrNone);
	test (TheDatabase.Execute(_L("DROP TABLE table"))==KErrNone);
	test(TheDatabase.Commit()==KErrNone);
	test.End();
	}

LOCAL_C void BuildTablePrologL(TDbColType aType,TInt aAttribs=0)
	{
	TheDatabase.Begin();
	CDbColSet *cs=CDbColSet::NewLC();
	TDbCol col(KColumnName,aType);
	col.iAttributes=aAttribs;
	cs->AddL(col);
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	}

inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TInt& aValue)
	{aSet.SetColL(col,aValue);}
inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TUint& aValue)
	{aSet.SetColL(col,aValue);}
inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TInt64& aValue)
	{aSet.SetColL(col,aValue);}
inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TReal32& aValue)
	{aSet.SetColL(col,aValue);}
inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TReal64& aValue)
	{aSet.SetColL(col,aValue);}
inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TTime& aValue)
	{aSet.SetColL(col,aValue);}
inline void SetColL(RDbRowSet& aSet,TDbColNo col,const TPtrC& aValue)
	{aSet.SetColL(col,aValue);}

template <class T>
void BuildTableL(TDbColType aType,TInt aAttribs,T aValues[],TInt aCount)
	{
	BuildTablePrologL(aType,aAttribs);
	for (TInt ii=0;ii<aCount;++ii)
		{
		TheTable.InsertL();
		SetColL(TheTable,1,aValues[ii]);
		TheTable.PutL();
		}
	test(TheDatabase.Commit()==KErrNone);
	test (TheTable.CountL()==aCount);
	TheTable.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0620
@SYMTestCaseDesc        Setting the specified index as the active index for a table test
@SYMTestPriority        Medium
@SYMTestActions         Create an index and set as active index for the table.
                        Tests for RDbNamedDatabase::CreateIndex(),RDbNamedDatabase::Commit()
						RDbTable::Open(),RDbTable::SetIndex() functions.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void BuildIndexL(TDbTextComparison aComparison=EDbCompareNormal,TInt aLength=KDbUndefinedLength)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0620 Test index order "));
	TheDatabase.Begin();
	CDbKey *key=CDbKey::NewLC();
	key->AddL(TDbKeyCol(KColumnName,aLength));
	key->MakeUnique();
	key->SetComparison(aComparison);
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	test (TheDatabase.Commit()==KErrNone);
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	}

inline void GetCol(RDbRowSet& aSet,TDbColNo col,TInt& aValue)
	{aValue=aSet.ColInt(col);}
inline void GetCol(RDbRowSet& aSet,TDbColNo col,TUint& aValue)
	{aValue=aSet.ColUint(col);}
inline void GetCol(RDbRowSet& aSet,TDbColNo col,TInt64& aValue)
	{aValue=aSet.ColInt64(col);}
inline void GetCol(RDbRowSet& aSet,TDbColNo col,TReal32& aValue)
	{aValue=aSet.ColReal32(col);}
inline void GetCol(RDbRowSet& aSet,TDbColNo col,TReal64& aValue)
	{aValue=aSet.ColReal64(col);}
inline void GetCol(RDbRowSet& aSet,TDbColNo col,TTime& aValue)
	{aValue=aSet.ColTime(col);}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0621
@SYMTestCaseDesc        RDbRowSet ordering test
@SYMTestPriority        Medium
@SYMTestActions         Reorder the row set data with RDbRowSet::GetL(),SetL() functions.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
template <class T>
void TestOrderingL(RDbRowSet& aSet,T,TInt aCount)
	{
	test.Next( _L( " @SYMTestCaseID:SYSLIB-DBMS-CT-0621 " ) );
	test(aSet.CountL()==aCount);
	TInt count=0;
	if (aSet.FirstL())
		{
		++count;
		aSet.GetL();
		T last;
		GetCol(aSet,1,last);
		while (aSet.NextL())
			{
			++count;
			aSet.GetL();
			T current;
			GetCol(aSet,1,current);
			test(last<current);
			last=current;
			}
		}
	test(count==aCount);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0622
@SYMTestCaseDesc        RDbTable::SeekL() function test
@SYMTestPriority        Medium
@SYMTestActions         Tests for the retrieved column value
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
template <class T>
void TestSeekL(RDbTable& aTable,T,const T aValues[],TInt aCount)
	{
	test.Next( _L( " @SYMTestCaseID:SYSLIB-DBMS-CT-0622 Test index seeking " ) );
	for (TInt ii=0;ii<aCount;++ii)
		{
		test(aTable.SeekL(aValues[ii]));
		aTable.GetL();
		T val;
		GetCol(aTable,1,val);
		test(aValues[ii]==val);
		}
	}

/**
aVal argument is used in the test functions (TestType & TestOrdering) and has no meaning outside them.
It is used only to avoid some compiler varnings and to determine the correct template type

@SYMTestCaseID          SYSLIB-DBMS-CT-0623
@SYMTestCaseDesc        Tests for RDbTable,RDbRowSet classes
@SYMTestPriority        Medium
@SYMTestActions         Call up Test for table ordering and index seeking functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
template <class T>
void TestTypeL( TDbColType aType, TInt aAttribs, const T aValues[], TInt aCount )
	{
	test.Start( _L( " @SYMTestCaseID:SYSLIB-DBMS-CT-0623 Build table " ) );
	BuildTableL( aType, aAttribs, aValues, aCount );
	T t(0);
//
	test.Next( _L( "Test ORDER BY ordering" ) );
	test( TheView.Prepare( TheDatabase, KSelectOrdered ) == KErrNone );
	test( TheView.EvaluateAll() == KErrNone );
	TestOrderingL( TheView, t, aCount );
	TheView.Close();
//
	BuildIndexL();
	TestOrderingL( TheTable, t, aCount );
//
	TestSeekL( TheTable, t, aValues, aCount );
	TheTable.Close();
//
	test( TheDatabase.DropTable( KTableName ) == KErrNone );
	test.End();
	}
/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1322
@SYMTestCaseDesc        Text ordering test
@SYMTestPriority        Medium
@SYMTestActions         Tests for RDbRowSet::Next(),RDbRowSet::GetL(),RDbRowSet::ColDes() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestTextOrderingL(RDbRowSet& aSet,TInt aCount,const TTextOps& aComp)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1322 "));
	test(aSet.CountL()==aCount);
	TInt count=0;
	while (aSet.NextL())
		{
		aSet.GetL();
		TPtrC current=aSet.ColDes(1);
		if (count++>0)
			test(aComp.Compare(TheBuf,current)<0);
		TheBuf=current;
		}
	test(count==aCount);
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-1323
@SYMTestCaseDesc        Tests for RDbView,RDbTable classes
@SYMTestPriority        Medium
@SYMTestActions         Wrapper function to call up for text ordering and text indexing tests
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestTextL(const TPtrC aValues[],TInt aCount,TDbTextComparison aComparison,TInt aLength=KDbUndefinedLength)
	{
	const TTextOps& comp=TTextOps::Ops(aComparison);
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-1323 Build table "));
	BuildTableL(EDbColText,0,aValues,aCount);
//
	test.Next(_L("Test ORDER BY ordering"));
	test (TheView.Prepare(TheDatabase,TDbQuery(KSelectOrdered,aComparison))==KErrNone);
	test (TheView.EvaluateAll()==KErrNone);
	TestTextOrderingL(TheView,aCount,comp);
	TheView.Close();
//
	BuildIndexL(aComparison,aLength);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	TestTextOrderingL(TheTable,aCount,comp);
//
	test.Next(_L("Test index seeking"));
	for (TInt ii=0;ii<aCount;++ii)
		{
		test(TheTable.SeekL(aValues[ii]));
		TheTable.GetL();
		test(comp.Compare(aValues[ii],TheTable.ColDes(1))==0);
		}
//
	TheTable.Close();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

void TestLongTextOrderingL(RDbRowSet& aSet,TInt aCount,const TTextOps& aComp)
	{
	test(aSet.CountL()==aCount);
	TInt count=0;
	HBufC* prev=0;
	while (aSet.NextL())
		{
		aSet.GetL();
		TInt len=aSet.ColLength(1);
		HBufC* buf=HBufC::NewL(len);
		RDbColReadStream s;
		s.OpenLC(aSet,1);
		TPtr des(buf->Des());
		s.ReadL(des,len);
		CleanupStack::PopAndDestroy();
		if (count++)
			test (aComp.Compare(*buf,*prev)>=0);
		delete prev;
		prev=buf;
		}
	delete prev;
	test(count==aCount);
	}

LOCAL_C void OrderByLongTextL(TInt aCount,TDbTextComparison aComparison)
	{
	test (TheView.Prepare(TheDatabase,TDbQuery(KSelectOrdered,aComparison))==KErrNone);
	test (TheView.EvaluateAll()==KErrNone);
	TestLongTextOrderingL(TheView,aCount,TTextOps::Ops(aComparison));
	TheView.Close();
	}

LOCAL_C void TestLongTextL(const TPtrC aValues[],TInt aCount,TDbTextComparison aComparison,TInt aLength=KDbUndefinedLength)
	{
	const TTextOps& comp=TTextOps::Ops(aComparison);
	test.Start(_L("Build table"));
	BuildTableL(EDbColLongText,0,aValues,aCount);
//
	test.Next(_L("Test ORDER BY ordering"));
	OrderByLongTextL(aCount,aComparison);
//
	BuildIndexL(aComparison,aLength);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	TestLongTextOrderingL(TheTable,aCount,comp);
//
	test.Next(_L("Test index seeking"));
	for (TInt ii=0;ii<aCount;++ii)
		{
		test(TheTable.SeekL(aValues[ii]));
		TheTable.GetL();
		RDbColReadStream strm;
		strm.OpenLC(TheTable,1);
		strm.ReadL(TheBuf,TheTable.ColLength(1));
		CleanupStack::PopAndDestroy();
		test(comp.Compare(aValues[ii],TheBuf)==0);
		}
	TheTable.Close();
//
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

TUint const KBitValues[]={0,1};
TInt const KInt8Values[]={0,KMinTInt8+1,1,KMaxTInt8,2,-3,-1,KMaxTInt8-1,KMinTInt8,-40};
TInt const KInt16Values[]={0,KMinTInt16+1,1,KMaxTInt16,2,-3,-1,KMaxTInt16-1,KMinTInt16,-4000};
TInt const KInt32Values[]={0,KMinTInt32+1,1,KMaxTInt32,2,-3,-1,KMaxTInt32-1,KMinTInt32,-40000000};
TInt const KInt32Count=sizeof(KInt32Values)/sizeof(KInt32Values[0]);
TUint const KUint8Values[]={0,1,KMaxTUint8,2,(KMaxTUint8+1)/2,(KMaxTUint8-1)/2,KMaxTUint8-1,40};
TUint const KUint16Values[]={0,1,KMaxTUint16,2,(KMaxTUint16+1)/2,(KMaxTUint16-1)/2,KMaxTUint16-1,4000};
TUint const KUint32Values[]={0,1,KMaxTUint32,2,KMaxTUint32/2+1,KMaxTUint32/2,KMaxTUint32-1,40000000};
//TReal32 const KReal32Values[]={0.0f,1.0f,KMaxTReal32,KMinTReal32,-1.0f,-KMaxTReal32,-4e20f,-KMinTReal32};
//TReal64 const KReal64Values[]={0.0,1.0,KMaxTReal64,KMinTReal64,-1.0,-KMaxTReal64,-4e200,-KMinTReal64};
TReal32 const KReal32Values[]={0.0f,1.0f,1e37f,1e-37f,-1.0f,-1e37f,-4e20f,-1e-37f};
TReal64 const KReal64Values[]={0.0,1.0,KMaxTReal64,KMinTReal64,-1.0,-KMaxTReal64,-4e200,-KMinTReal64};
TInt64 const KInt64Values[]=
	{
	0,
	MAKE_TINT64(0x80000000u,0x1u),
	1,
	MAKE_TINT64(0x7fffffffu,0xffffffffu),
	2,
	-3,
	-1,
	MAKE_TINT64(0x7fffffffu,0xfffffffeu),
	MAKE_TINT64(0x80000000u,0x0u),
	-40
	};
TTime const KTimeValues[]=
	{
	Time::MaxTTime(),
	TInt64(0),
	TDateTime(1970,EJanuary,0,0,0,0,0),
	Time::MinTTime(),
	TDateTime(2049,EDecember,30,23,59,59,999999),
	TDateTime(1996,EJuly,8,17,45,0,0)
	};
TPtrC const KTextValues[]=
	{
	_S(""),
	_S("10"),
	_S("zyx"),
	_S("0"),
	_S("abcd"),
	_S("abcdefg"),
	_S("ABCDE"),
	_S("Z")
	};
TPtrC const KLongTextValues[]=
	{
	_S("this blob will be inlined"),
	_S(""),
	_S("that blob was null"),
	_S("An example of an out-of-line blob in an index! LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8-LongText8")
	};
const TInt KLongTextLimit=30;

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0624
@SYMTestCaseDesc        Wrapper function testing for Indexing with different integer sizes and Text .
@SYMTestPriority        Medium
@SYMTestActions         Tests for indexing
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTypesL()
	{
	#define ARRAY_SIZE(a) TInt(sizeof(a)/sizeof(a[0]))
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0624 Indexing Bit "));
	TestTypeL(EDbColBit,TDbCol::ENotNull,KBitValues,ARRAY_SIZE(KBitValues));
	test.Next(_L("Indexing Int8"));
	TestTypeL(EDbColInt8,TDbCol::ENotNull,KInt8Values,ARRAY_SIZE(KInt8Values));
	test.Next(_L("Indexing Int16"));
	TestTypeL(EDbColInt16,TDbCol::ENotNull,KInt16Values,ARRAY_SIZE(KInt16Values));
	test.Next(_L("Indexing Int32"));
	TestTypeL(EDbColInt32,TDbCol::ENotNull,KInt32Values,ARRAY_SIZE(KInt32Values));
	test.Next(_L("Indexing Uint8"));
	TestTypeL(EDbColUint8,TDbCol::ENotNull,KUint8Values,ARRAY_SIZE(KUint8Values));
	test.Next(_L("Indexing Uint16"));
	TestTypeL(EDbColUint16,TDbCol::ENotNull,KUint16Values,ARRAY_SIZE(KUint16Values));
	test.Next(_L("Indexing Uint32"));
	TestTypeL(EDbColUint32,TDbCol::ENotNull,KUint32Values,ARRAY_SIZE(KUint32Values));
	test.Next(_L("Indexing Real32"));
	TestTypeL(EDbColReal32,TDbCol::ENotNull,KReal32Values,ARRAY_SIZE(KReal32Values));
	test.Next(_L("Indexing Real64"));
	TestTypeL(EDbColReal64,TDbCol::ENotNull,KReal64Values,ARRAY_SIZE(KReal64Values));
	test.Next(_L("Indexing Int64"));
	TestTypeL(EDbColInt64,TDbCol::ENotNull,KInt64Values,ARRAY_SIZE(KInt64Values));
	test.Next(_L("Indexing Time"));
	TestTypeL(EDbColDateTime,TDbCol::ENotNull,KTimeValues,ARRAY_SIZE(KTimeValues));
	test.Next(_L("Indexing Text (Normal)"));
	TestTextL(KTextValues,ARRAY_SIZE(KTextValues),EDbCompareNormal);
	test.Next(_L("Indexing Text (Folded)"));
	TestTextL(KTextValues,ARRAY_SIZE(KTextValues),EDbCompareFolded);
	test.Next(_L("Indexing Text (Collated)"));
	TestTextL(KTextValues,ARRAY_SIZE(KTextValues),EDbCompareCollated);
	test.Next(_L("Indexing Text (Normal, truncated)"));
	TestTextL(KTextValues,ARRAY_SIZE(KTextValues),EDbCompareNormal,5);
	test.Next(_L("Indexing LongText (Normal)"));
	TestLongTextL(KLongTextValues,ARRAY_SIZE(KLongTextValues),EDbCompareNormal,KLongTextLimit);
	test.Next(_L("Indexing LongText (Folded)"));
	TestLongTextL(KLongTextValues,ARRAY_SIZE(KLongTextValues),EDbCompareFolded,KLongTextLimit);
	test.Next(_L("Indexing LongText (Collated)"));
	TestLongTextL(KLongTextValues,ARRAY_SIZE(KLongTextValues),EDbCompareCollated,KLongTextLimit);
	test.End();
	}

const TInt KBlobKeyTruncated=32/sizeof(TText);
const TInt KBlobKeyMaxInline=255/sizeof(TText);
const TInt KBlobKeyCompare=512/sizeof(TText);

const TInt KMemoTestLengths[]=
	{
	0,
	1,
	KBlobKeyTruncated-1,
	KBlobKeyTruncated,
	KBlobKeyTruncated+1,
	KBlobKeyMaxInline,
	KBlobKeyMaxInline+1,
	KBlobKeyCompare-1,
	KBlobKeyCompare,
	KBlobKeyCompare+1,
	5000
	};

void TestMemoTableL(CDbColSet& aBaseSet)
	{
	test.Start(_L("create the table"));
	aBaseSet.AddL(TDbCol(KColumnName,EDbColLongText));
	TheDatabase.Begin();
	TInt r=TheDatabase.CreateTable(KTableName,aBaseSet);
	test (r==KErrNone);
//
	test.Next(_L("add the rows"));
	r=TheView.Prepare(TheDatabase,KSelectOrdered,RDbView::EInsertOnly);
	test (r==KErrNone);
	r=TheView.EvaluateAll();
	test (r==KErrNone);
	HBufC* prev=0;
	TInt count=0;
	for (TUint ii=0;ii<elementsof(KMemoTestLengths);++ii)
		{
		TInt size=KMemoTestLengths[ii];
		HBufC* buf=HBufC::NewL(size);
		if (size>0)
			{
			TPtr des(buf->Des());
			des.Copy(*prev);
			des.AppendFill('b',size-prev->Length());
			delete prev;
			TheView.InsertL();
			TheView.SetColL(1,*buf);
			TheView.PutL();
			++count;
			des[size-1]='a';
			}
		TheView.InsertL();
		TheView.SetColL(1,*buf);
		TheView.PutL();
		++count;
		prev=buf;
		}
	delete prev;
	TheView.Close();
	r=TheDatabase.Commit();
	test (r==KErrNone);
//
	test.Next(_L("Normal order"));
	OrderByLongTextL(count,EDbCompareNormal);
	test.Next(_L("Folded order"));
	OrderByLongTextL(count,EDbCompareFolded);
	test.Next(_L("Collated order"));
	OrderByLongTextL(count,EDbCompareCollated);
//
	r=TheDatabase.DropTable(KTableName);
	test (r==KErrNone);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0625
@SYMTestCaseDesc        Tests for ordering by longtext
@SYMTestPriority        Medium
@SYMTestActions         Tests for CDbColSet
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOrderByLongTextL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0625 Maximum Inline Limit "));
	CDbColSet* set=CDbColSet::NewLC();
	TestMemoTableL(*set);
//
	test.Next(_L("Reduced Inline limit [<32]"));
	set->Clear();
	TDbCol col;
	col.iType=EDbColText8;
	col.iMaxLength=255;
	col.iAttributes=0;
	TDbColName name;
	for (TInt ii=0;ii<32;++ii)
		{
		name.Format(_L("col%d"),ii);
		col.iName=name;
		if (ii==31)
			col.iMaxLength=255-20;
		set->AddL(col);
		}
	TestMemoTableL(*set);
	CleanupStack::PopAndDestroy();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0626
@SYMTestCaseDesc        Tests for reverse ordering in indexes
@SYMTestPriority        Medium
@SYMTestActions         Tests for reversing the indexes
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestReverseL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0626 Create table and index "));
	TheDatabase.Begin();
	CDbColSet *cs=CDbColSet::NewLC();
	cs->AddL(TDbCol(KColumnName,EDbColInt32));
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	CDbKey *key=CDbKey::NewLC();
	key->AddL(TDbKeyCol(KColumnName,TDbKeyCol::EDesc));
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	test(TheDatabase.Commit()==KErrNone);
	test.Next(_L("Add records"));
	TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	for (TInt ii=0;ii<KInt32Count;++ii)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,KInt32Values[ii]);
		TheTable.PutL();
		}
	test(TheDatabase.Commit()==KErrNone);
	test(TheTable.CountL()==KInt32Count);
	test.Next(_L("Check order"));
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	test(TheTable.CountL()==KInt32Count);
	TInt count=0;
	if (TheTable.FirstL())
		{
		++count;
		TheTable.GetL();
		TInt32 last=TheTable.ColInt(1);
		while (TheTable.NextL())
			{
			++count;
			TheTable.GetL();
			TInt32 current=TheTable.ColInt(1);
			test(last>current);
			last=current;
			}
		}
	test(count==KInt32Count);
	TheTable.Close();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0627
@SYMTestCaseDesc        Tests for multi-column keys
@SYMTestPriority        Medium
@SYMTestActions         Tests for mutliple column keys
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultiL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0627 Create table and index "));
	TheDatabase.Begin();
	CDbColSet *cs=CDbColSet::NewLC();
	TDbCol col(KColumnName,EDbColInt32);
	col.iAttributes=TDbCol::ENotNull;
	cs->AddL(col);
	col.iName=KColumnTwo;
	cs->AddL(col);
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	CDbKey *key=CDbKey::NewLC();
	key->AddL(TDbKeyCol(KColumnTwo));
	key->AddL(TDbKeyCol(KColumnName,TDbKeyCol::EDesc));
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	test(TheDatabase.Commit()==KErrNone);
	test.Next(_L("Add records"));
	TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	for (TInt ii=0;ii<KInt32Count;++ii)
		{
		for (TInt jj=0;jj<KInt32Count;++jj)
			{
			TheTable.InsertL();
			TheTable.SetColL(1,KInt32Values[ii]);
			TheTable.SetColL(2,KInt32Values[jj]);
			TheTable.PutL();
			}
		}
	test(TheDatabase.Commit()==KErrNone);
	test.Next(_L("Check order"));
	test(TheTable.CountL()==KInt32Count*KInt32Count);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	test(TheTable.CountL()==KInt32Count*KInt32Count);
	TInt count=0;
	if (TheTable.FirstL())
		{
		++count;
		TheTable.GetL();
		TInt32 lastOne=TheTable.ColInt(1);
		TInt32 lastTwo=TheTable.ColInt(2);
		while (TheTable.NextL())
			{
			++count;
			TheTable.GetL();
			TInt32 currentOne=TheTable.ColInt(1);
			TInt32 currentTwo=TheTable.ColInt(2);
			test(lastTwo<currentTwo||(lastTwo==currentTwo&&lastOne>currentOne));
			lastOne=currentOne;
			lastTwo=currentTwo;
			}
		}
	test(count==KInt32Count*KInt32Count);
	TheTable.Close();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0628
@SYMTestCaseDesc        Tests duplicates/unique constraints
@SYMTestPriority        Medium
@SYMTestActions         Tests for adding duplicate entries
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicatesL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0628 Create table and indices "));
	BuildTableL(EDbColInt32,TDbCol::ENotNull,KInt32Values,KInt32Count);
	BuildIndexL();
	TheTable.Close();
	CDbKey* key=CDbKey::NewLC();
	key->AddL(KColumnName);
	test(TheDatabase.CreateIndex(KIndexTwo,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	test.Next(_L("Attempt to add/update duplicate entry"));
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheTable.InsertL();
	TheTable.SetColL(1,0);
	TRAPD(r,TheTable.PutL());
	test(r!=KErrNone);
	TheTable.Cancel();
	TheTable.LastL();
	TheTable.UpdateL();
	TheTable.SetColL(1,0);
	TRAP(r,TheTable.PutL());
	test(r!=KErrNone);
	TheTable.Cancel();
	test.Next(_L("Remove unique index"));
	TheTable.Close();
	test(TheDatabase.DropIndex(KIndexName,KTableName)==KErrNone);
	test.Next(_L("Attempt to update/add duplicate entry"));
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	TheTable.LastL();
	TheTable.UpdateL();
	TheTable.SetColL(1,0);
	TRAP(r,TheTable.PutL());
	test(r==KErrNone);
	TheTable.InsertL();
	TheTable.SetColL(1,0);
	TRAP(r,TheTable.PutL());
	test(r==KErrNone);
	test.Next(_L("Check order"));
	test(TheTable.CountL()==KInt32Count+1);
	test(TheTable.SetIndex(KIndexTwo)==KErrNone);
	test(TheTable.CountL()==KInt32Count+1);
	TInt count=0;
	if (TheTable.FirstL())
		{
		++count;
		TheTable.GetL();
		TInt32 last=TheTable.ColInt(1);
		while (TheTable.NextL())
			{
			++count;
			TheTable.GetL();
			TInt32 current=TheTable.ColInt(1);
			test(last<=current);	// duplicates present
			last=current;
			}
		}
	test(count==KInt32Count+1);
	test.Next(_L("Try to create unique index"));
	TheTable.Close();
	key=CDbKey::NewLC();
	key->AddL(KColumnName);
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)!=KErrNone);
	CleanupStack::PopAndDestroy();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

struct Pair { TInt i1,i2; };
struct TSeekingTest
	{
	Pair iSeek;
	Pair iResults[5];
	};

// seek pair: results for <,<=,=,>=,>
static TSeekingTest const SeekingTests[]=
	{
	{{0,0},{{-1,-1},{-1,-1},{-1,-1},{100,10},{100,10}}},
	{{0,-1},{{-1,-1},{-1,-1},{-1,-1},{100,10},{100,10}}},
	{{100,0},{{-1,-1},{-1,-1},{-1,-1},{100,10},{100,10}}},
	{{100,10},{{-1,-1},{100,10},{100,10},{100,10},{100,20}}},
	{{100,55},{{100,50},{100,50},{-1,-1},{100,60},{100,60}}},
	{{100,60},{{100,50},{100,60},{100,60},{100,60},{100,70}}},
	{{100,100},{{100,90},{100,100},{100,100},{100,100},{200,10}}},
	{{100,110},{{100,100},{100,100},{-1,-1},{200,10},{200,10}}},
	{{100,-1},{{-1,-1},{100,100},{100,10},{100,10},{200,10}}},
	{{500,-1},{{400,100},{500,100},{500,10},{500,10},{600,10}}},
	{{550,50},{{500,100},{500,100},{-1,-1},{600,10},{600,10}}},
	{{550,-1},{{500,100},{500,100},{-1,-1},{600,10},{600,10}}},
	{{1000,0},{{900,100},{900,100},{-1,-1},{1000,10},{1000,10}}},
	{{1000,10},{{900,100},{1000,10},{1000,10},{1000,10},{1000,20}}},
	{{1000,100},{{1000,90},{1000,100},{1000,100},{1000,100},{-1,-1}}},
	{{1000,110},{{1000,100},{1000,100},{-1,-1},{-1,-1},{-1,-1}}},
	{{1000,-1},{{900,100},{1000,100},{1000,10},{1000,10},{-1,-1}}},
	{{1100,0},{{1000,100},{1000,100},{-1,-1},{-1,-1},{-1,-1}}},
	{{1100,-1},{{1000,100},{1000,100},{-1,-1},{-1,-1},{-1,-1}}}
	};

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0629
@SYMTestCaseDesc        Tests for seeking on indexes
@SYMTestPriority        Medium
@SYMTestActions         Tests for SeekL and GetL functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestSeekingL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0629 Create table and index "));
	TheDatabase.Begin();
	CDbColSet *cs=CDbColSet::NewLC();
	TDbCol col(KColumnName,EDbColInt32);
	col.iAttributes=TDbCol::ENotNull;
	cs->AddL(col);
	col.iName=KColumnTwo;
	cs->AddL(col);
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
	CDbKey *key=CDbKey::NewLC();
	key->AddL(KColumnName).AddL(KColumnTwo);
	key->MakeUnique();
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
	test(TheDatabase.Commit()==KErrNone);
	test.Next(_L("Add records"));
	TheDatabase.Begin();
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	for (TInt ii=100;ii<=1000;ii+=100)
		{
		for (TInt jj=10;jj<=100;jj+=10)
			{
			TheTable.InsertL();
			TheTable.SetColL(1,ii);
			TheTable.SetColL(2,jj);
			TheTable.PutL();
			}
		}
	test(TheDatabase.Commit()==KErrNone);
	test(TheTable.CountL()==100);
	test(TheTable.SetIndex(KIndexName)==KErrNone);
	test(TheTable.CountL()==100);
	test.Next(_L("Seeking"));
	const TSeekingTest* stest=SeekingTests;
	const TSeekingTest* const end=stest+sizeof(SeekingTests)/sizeof(SeekingTests[0])-1;
	for (;stest<=end;++stest)
		{
		TDbSeekMultiKey<2> key;
		key.Add(stest->iSeek.i1);
		if (stest->iSeek.i2>=0)
			key.Add(stest->iSeek.i2);
		const Pair* results=stest->iResults;
		for (TInt ii=RDbTable::ELessThan;ii<=RDbTable::EGreaterThan;++results,++ii)
			{
			if (TheTable.SeekL(key,RDbTable::TComparison(ii)))
				{
				test(results->i1>=0);
				TRAPD(errCode, TheTable.GetL());
				test(errCode==KErrNone);
				test(TheTable.ColInt(1)==results->i1);
				test(TheTable.ColInt(2)==results->i2);
				}
			else
				test(results->i1<0);
			}
		}
	TheTable.Close();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0630
@SYMTestCaseDesc        Tests for defect,index creation and set index operations
@SYMTestPriority        Medium
@SYMTestActions         Tests for creating and setting index operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestInequalityErrorL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0630 Create table "));
	TheDatabase.Begin();
	CDbColSet *cs=CDbColSet::NewLC();
	cs->AddL(TDbCol(KColumnName,EDbColInt32));
	test(TheDatabase.CreateTable(KTableName,*cs)==KErrNone);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("create indices"));
	CDbKey *key=CDbKey::NewLC();
	key->AddL(TDbKeyCol(KColumnName));
	test(TheDatabase.CreateIndex(KIndexName,KTableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy();
//
	test.Next(_L("Populate table"));
	test(TheTable.Open(TheDatabase,KTableName)==KErrNone);
	for(TInt ii=0; ii<=130;++ii)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,ii);
		TheTable.PutL();
		}
//
	test.Next(_L("Checks"));
	test(TheTable.SetIndex(KIndexName)==KErrNone);
//
// We need to delete a row in this node to get to the correct inequality condition
	test.Next(_L("Delete row 90"));
	test(TheTable.SeekL(TDbSeekKey(90)));
	TheTable.GetL();
	TheTable.DeleteL();
// Now delete last row on node which should be 94
	test.Next(_L("Delete row 94"));
	test(TheTable.SeekL(TDbSeekKey(94)));
	TheTable.GetL();
	TheTable.DeleteL();
	TheTable.EndL();
	test(!TheTable.SeekL(TDbSeekKey(94)));
//
	test.Next(_L("Insert row 94"));
	TheTable.InsertL();
	TheTable.SetColL(1,94);
	TheTable.PutL();
//
	test.Next(_L("now try and find it"));
	test(TheTable.SeekL(TDbSeekKey(94))); //prior to defect fix this line failed.
//
	TheDatabase.Commit();
	TheTable.Close();
	test(TheDatabase.DropTable(KTableName)==KErrNone);
	test.End();
	}

/**
@SYMTestCaseID          PDS-DBMS-CT-4006
@SYMTestCaseDesc        Tests for add dbseekkey operations
@SYMTestPriority        High
@SYMTestActions         Create a DbSeekKey then add an index to it
@SYMTestExpectedResults Verify that seeking with key works
@SYMDEF                 DEF135710
*/
LOCAL_C void TestAddSeekKeyL()
	{
	test.Start(_L(" @SYMTestCaseID:PDS-DBMS-CT-4006 Tests for add dbseekkey operations"));
	
	CreateDatabase();
	TInt err = TheDatabase.Begin();
	test2(err, KErrNone);
	
	CDbColSet *cs=CDbColSet::NewLC();
	cs->AddL(TDbCol(KColumnName1,EDbColText8));
	err = TheDatabase.CreateTable(KTableName1,*cs);
	test2(err , KErrNone);
	CleanupStack::PopAndDestroy();

	test.Next(_L("Create Indices"));
	CDbKey *key=CDbKey::NewLC();
	key->AddL(TDbKeyCol(KColumnName1));
	err = TheDatabase.CreateIndex(KIndexName1,KTableName1,*key);
	test2(err, KErrNone);
	CleanupStack::PopAndDestroy();

	test.Next(_L("Populate Table"));
	TBuf8<7> buf(_L8("testKey"));
	err = TheTable.Open(TheDatabase,KTableName1);
	test2(err, KErrNone);
	for(TInt i = 0; i <= 10; ++i)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,buf);
		TheTable.PutL();
		}
	
	test.Next(_L("Set Index"));
	err = TheTable.SetIndex(KIndexName1);
	test2(err, KErrNone);

	test.Next(_L("Testing Add Function"));
	TDbSeekKey testkey;
	testkey.Add(buf);
	TBool r = TheTable.SeekL(testkey);
	test2(r, ETrue);
	
	err = TheDatabase.Commit();
	test2(err, KErrNone);
	
	TheTable.Close();
	test.End();
	}

//
// Test the database definition and enquiry functions
//
LOCAL_C void TestIndexesL()
	{
	test.Start(_L("Create Database"));
	CreateDatabase();
	test.Next(_L("Test index build and drop"));
	TestIndexBuildL();
	test.Next(_L("Test index persistence"));
	TestPersistenceL();
	test.Next(_L("Testing index key types"));
	TestTypesL();
	test.Next(_L("Testing LongText ORDER BY"));
	TestOrderByLongTextL();
	test.Next(_L("Testing reverse ordering"));
	TestReverseL();
	test.Next(_L("Testing multi column keys"));
	TestMultiL();
	test.Next(_L("Testing duplicate/unqiue"));
	TestDuplicatesL();
	test.Next(_L("Testing seeking"));
	TestSeekingL();
	test.Next(_L("Testing incorrect inequaltiy condition in store btree"));
	TestInequalityErrorL();
	CloseDatabase();
	test.End();
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
	TInt r=TheDbs.Connect();
	test (r==KErrNone);
	TheDbs.ResourceMark();
	test.Start(_L("Standard database"));
	TRAP(r,TestIndexesL();)
	test(r==KErrNone);
	test.Next(_L("Secure database"));
	TRAP(r,TestIndexesL();)
	test(r==KErrNone);
	TRAP(r, TestAddSeekKeyL());
	test(r==KErrNone);
	CloseDatabase();

	::DeleteDataFile(KTestDatabase);	//deletion of data files must be before call to end - DEF047652

	test.End();
	TheDbs.ResourceCheck();
	TheDbs.Close();

	__UHEAP_MARKEND;
	delete TheTrapCleanup;

	TheFs.Close();
	test.Close();
	return 0;
    }
