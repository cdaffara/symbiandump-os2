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

// MSVC++ up to 5.0 has problems with expanding inline functions
// This disables the mad warnings for the whole project
#if defined(NDEBUG) && defined(__VC32__) && _MSC_VER<=1100
#pragma warning(disable : 4710)			// function not expanded. MSVC 5.0 is stupid
#endif

#include <d32dbms.h>
#include <f32file.h>
#include <e32test.h>
#include <e32math.h>

// constructing literal TInt64 from compiler 64 bit integer rep

#ifndef I64LIT
#if defined __GCC32__ || defined __EABI__
#define _LINT64(val) MAKE_TINT64(TUint(val##LL>>32),TUint(val##LL&0xffffffffu))
#else
#define _LINT64(val) MAKE_TINT64(TUint(__int64(val)>>32),TUint(__int64(val)&0xffffffffu))
#endif
#else // I64LIT
#define _LINT64 I64LIT
#endif // I64LIT

LOCAL_D RTest TheTest(_L("t_dbsql: DBMS SQL parsing and execution tests"));

LOCAL_D RFs TheFs;
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RDbs TheDbs;
LOCAL_D RDbNamedDatabase TheDatabase;
LOCAL_D RDbTable TheTable;
LOCAL_D RDbView TheView;
LOCAL_D TBuf<1024> TheSql;

const TInt KTestCleanupStack=0x20;
const TPtrC KTestDatabase=_L("c:\\dbms-tst\\t_sql.db");

#define elementsof(array) (sizeof(array)/sizeof(array[0]))

LOCAL_C void TestCleanup()
	{
	TheTable.Close();
	TheView.Close();
	TheDatabase.Close();
	TheDbs.Close();
	TheFs.Close();
	/////////////
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(KTestDatabase, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &KTestDatabase);
			err = fsSession.SetAtt(KTestDatabase, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &KTestDatabase);
				}
			err = fsSession.Delete(KTestDatabase);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &KTestDatabase);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &KTestDatabase);
		}
	}

LOCAL_C void CloseDatabase()
	{
	TheDatabase.Close();
	}

LOCAL_C void Disconnect()
	{
	TheDbs.ResourceCheck();
	TheDbs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
        RDebug::Print(_L("*** Expression evaluated to false\r\n"));
		::TestCleanup();
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		::TestCleanup();
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

// Create the database
LOCAL_C void CreateDatabase()
	{
	TInt r=TheDatabase.Replace(TheFs,KTestDatabase);
	TEST2(r, KErrNone);
	}

LOCAL_C void Connect()
	{
	TInt r=TheDbs.Connect();
	TEST2(r, KErrNone);
	TheDbs.ResourceMark();
	}

// Open the database through the server
LOCAL_C void ShareDatabase()
	{
	TInt r=TheDatabase.Open(TheDbs,KTestDatabase);
	TEST2(r, KErrNone);
	}

struct TSelectTest
	{
	const TText* iSearchCondition;
	TUint iResultSet;
	};
#define ROWS(n,m) (((~0u)<<(n))&((2u<<(m))-1))
#define ROW(n) ROWS(n,n)
#define NO_ROWS 0
#define ALL_ROWS (~1u)		// all rows which are not NULL

class TestPredicateBase
	{
protected:
	static void Create(const TText* aType);
	static void Test(const TSelectTest* aTest,TInt aCount,TInt aRows, TBool aLog);
	static void TestViewL(const TSelectTest* aTest,TInt aCount,TInt aRows, TBool aLog=EFalse);
	};

// Create the table for the predicate tests
void TestPredicateBase::Create(const TText* aType)
	{
	TheTest.Next(TPtrC(aType));
	TheDatabase.Begin();
	TheSql.Format(_L("CREATE TABLE Compare (Id COUNTER,Test %s)"),aType);
	TInt r=TheDatabase.Execute(TheSql);
	TEST2(r, KErrNone);
	r=TheTable.Open(TheDatabase,_L("Compare"),TheTable.EInsertOnly);
	TEST2(r, KErrNone);
	}

// Test the predicate on the table, then on the indexed table
void TestPredicateBase::Test(const TSelectTest* aTest,TInt aCount,TInt aRows, TBool aLog)
	{
    if(aLog)
        {
        TheTest.Printf(_L("TestPredicateBase::Test\r\n"));
        }
	TheTable.Close();
	TInt r=TheDatabase.Commit();
	if(aLog)
        {
        TheTest.Printf(_L("Commit %d\r\n"), r);
        }
	TEST2(r, KErrNone);
	TRAPD(errCode, TestViewL(aTest,aCount,aRows, aLog));
	if(aLog)
        {
        TheTest.Printf(_L("TestViewL %d"), errCode);
        }

	TEST2(errCode, KErrNone);
	r=TheDatabase.Execute(_L("CREATE INDEX Key ON Compare (Test)"));
   if(aLog)
        {
        TheTest.Printf(_L("Execute %d"), r);
        }

	TEST2(r, KErrNone);
	TRAP(errCode,TestViewL(aTest,aCount,aRows, aLog));
	if(aLog)
        {
        TheTest.Printf(_L("TestViewL %d"), errCode);
	    }

	TEST2(errCode, KErrNone);
	r=TheDatabase.Execute(_L("DROP TABLE Compare"));
    if(aLog)
        {
        TheTest.Printf(_L("Execute %d"), r);
        }

	TEST2(r, KErrNone);
	}

// Test the predicate on the table
void TestPredicateBase::TestViewL(const TSelectTest* aTest,TInt aCount,TInt aRows, TBool /*aLog*/)
	{
	TUint rowMask=(2u<<aRows)-1;
	for (;--aCount>=0;++aTest)
		{
		TheSql.Format(_L("SELECT Id FROM Compare WHERE Test %s"),aTest->iSearchCondition);
		TInt r=TheView.Prepare(TheDatabase,TheSql,TheView.EReadOnly);
		if(r!=KErrNone)
		    {
            TheTest.Printf(_L("Prepare r= %d aCount= %d  statement %S\r\n"), r, aCount, &TheSql);
		    }
		TEST2(r, KErrNone);
		TBool ignoreRow0=TheView.Unevaluated();
		r=TheView.EvaluateAll();
 		TEST2(r, KErrNone);
		TUint rows=0;
		while (TheView.NextL())
			{
 			TheView.GetL();
			rows|=1u<<TheView.ColUint(1);
			}
		if (ignoreRow0)
		    {
  			TEST((rows&~ROW(0))==(aTest->iResultSet&rowMask&~ROW(0)));
		    }
		else
		    {
 			TEST(rows==(aTest->iResultSet&rowMask));
		    }
		TheView.Close();
		}
	}

typedef void (*FSetColL)(TDbColNo aCol,const TAny* aVal);

void WriteRowsL(const TAny* aValues,TInt aRows,TInt aSize,FSetColL aSetColL, TBool aLog)
	{
	for (TInt row=0;row<=aRows;++row)
		{
        if(aLog)
            {
            TheTest.Printf(_L("row = %d"), row);
            }
		TheTable.InsertL();
		TEST(TheTable.ColUint(1)==TUint(row));
		if (row>0)
			{	// first row is always null
			aSetColL(2,aValues);
			aValues=PtrAdd(aValues,aSize);
			}
		TheTable.PutL();
		}
	}

template <class T>
struct SetCol
	{
	static void SetColL(TDbColNo aCol,const TAny* aVal)
		{
		TheTable.SetColL(aCol,*(const T*)aVal);
		}
	};

template <class T>
inline void WriteRowsL(const T* aValues,TUint aRows, TBool aLog)
	{
	WriteRowsL(aValues,aRows,sizeof(T),&SetCol<T>::SetColL, aLog);
	}

template <class T>
class TestPredicate : public TestPredicateBase
	{
public:
	static void RunL();
	};

// Test the Predicate operators for all types
template <class T>
void TestPredicate<T>::RunL()
	{
	Create(T::KType);
	TBool log = EFalse;
	if((TPtrC(T::KType)).CompareF(_L("TIME"))==0)
	    {
        log = ETrue;
	    }
	WriteRowsL(T::KValues,elementsof(T::KValues), log);
	Test(T::KTests,elementsof(T::KTests),elementsof(T::KValues),log);
	}

struct TypeBit
	{
public:
	static const TText* const KType;
	static const TUint KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeBit::KType=_S("BIT");
const TUint TypeBit::KValues[]={0,1};
const TSelectTest TypeBit::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(1)},
	{_S("<>0"),ROW(2)},
	{_S(">0"),ROW(2)},
	{_S("<0"),NO_ROWS},
	{_S("<=0"),ROW(1)},
	{_S(">=0"),ALL_ROWS},
	{_S("=1"),ROW(2)},
	{_S("<>1"),ROW(1)},
	{_S(">1"),NO_ROWS},
	{_S("<1"),ROW(1)},
	{_S("<=1"),ALL_ROWS},
	{_S(">=1"),ROW(2)}
	};

struct TypeUint8
	{
public:
	static const TText* const KType;
	static const TUint KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeUint8::KType=_S("UNSIGNED TINYINT");
const TUint TypeUint8::KValues[]={0,1,127,128,254,255};
const TSelectTest TypeUint8::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(1)},
	{_S("=3"),NO_ROWS},
	{_S("<2"),ROWS(1,2)},
	{_S(">=0"),ALL_ROWS},
	{_S("<128"),ROWS(1,3)},
	{_S("<>1"),ALL_ROWS-ROW(2)},
	{_S(">255"),NO_ROWS}
	};

struct TypeUint16
	{
public:
	static const TText* const KType;
	static const TUint KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeUint16::KType=_S("UNSIGNED SMALLINT");
const TUint TypeUint16::KValues[]={0,1,5000,32767,32768,65534,65535};
const TSelectTest TypeUint16::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(1)},
	{_S("=3"),NO_ROWS},
	{_S("<2"),ROWS(1,2)},
	{_S(">=32768"),ROWS(5,7)},
	{_S("<32767"),ROWS(1,3)},
	{_S("<>1"),ALL_ROWS-ROW(2)},
	{_S(">65535"),NO_ROWS}
	};

struct TypeUint32
	{
public:
	static const TText* const KType;
	static const TUint KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeUint32::KType=_S("UNSIGNED INTEGER");
const TUint TypeUint32::KValues[]={0,1,2147483647u,2147483648u,3000000000u,4294967294u,4294967295u};
const TSelectTest TypeUint32::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(1)},
	{_S("=3"),NO_ROWS},
	{_S("<2"),ROWS(1,2)},
	{_S(">=2147483648"),ROWS(4,7)},
	{_S("<2147483647"),ROWS(1,2)},
	{_S("<>3000000000"),ALL_ROWS-ROW(5)},
	{_S(">4294967295"),NO_ROWS}
	};

struct TypeInt8
	{
public:
	static const TText* const KType;
	static const TInt KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeInt8::KType=_S("TINYINT");
const TInt TypeInt8::KValues[]={-128,-1,0,1,127};
const TSelectTest TypeInt8::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(3)},
	{_S("=-1"),ROW(2)},
	{_S("<2"),ROWS(1,4)},
	{_S(">=-128"),ALL_ROWS},
	{_S("<128"),ALL_ROWS},
	{_S("<>1"),ALL_ROWS-ROW(4)}
	};

struct TypeInt16
	{
public:
	static const TText* const KType;
	static const TInt KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeInt16::KType=_S("SMALLINT");
const TInt TypeInt16::KValues[]={-32768,-32767,-1,0,1,32766,32767};
const TSelectTest TypeInt16::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(4)},
	{_S("<>0"),ALL_ROWS-ROW(4)},
	{_S("=-1"),ROW(3)},
	{_S("<2"),ROWS(1,5)},
	{_S(">=-40000"),ALL_ROWS},
	{_S("<32766"),ROWS(1,5)},
	{_S("=40"),NO_ROWS}
	};

struct TypeInt32
	{
public:
	static const TText* const KType;
	static const TInt KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeInt32::KType=_S("INTEGER");
const TInt TypeInt32::KValues[]={0x80000000/*-2147483648*/,-2147483647,-1,0,1,2147483646,2147483647};
const TSelectTest TypeInt32::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(4)},
	{_S("<>0"),ALL_ROWS-ROW(4)},
	{_S("=-1"),ROW(3)},
	{_S("<2"),ROWS(1,5)},
	{_S(">=-2147483648"),ALL_ROWS},
	{_S("<2147483646"),ROWS(1,5)},
	{_S(">2147483647"),NO_ROWS},
	{_S("=40"),NO_ROWS}
	};

struct TypeInt64
	{
public:
	static const TText* const KType;
	static const TInt64 KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeInt64::KType=_S("BIGINT");
const TInt64 TypeInt64::KValues[]=
	{
	MAKE_TINT64(0x80000000, 0x00000000), // min int64
	_LINT64(-4294967296),
	TInt(0x80000000),			// -2147483648!
	-1,
	0u,
	1u,
	2147483647u,
	_LINT64(2147483648),
	_LINT64(4294967295),
	_LINT64(4294967296),
	_LINT64(9223372036854775807)		// max int64
	};
const TSelectTest TypeInt64::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(5)},
	{_S("<>0"),ALL_ROWS-ROW(5)},
	{_S("=-1"),ROW(4)},
	{_S("<2"),ROWS(1,6)},
	{_S(">=-9223372036854775808"),ALL_ROWS},
	{_S("<4294967296"),ROWS(1,9)},
	{_S(">9223372036854775806"),ROW(11)},
	{_S("=40"),NO_ROWS}
	};

struct TypeReal32
	{
public:
	static const TText* const KType;
	static const TReal32 KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeReal32::KType=_S("REAL");
const TReal32 TypeReal32::KValues[]=
	{
	-KMaxTReal32,
	-1.0f,
	-KMinTReal32,
	0.0f,
	KMinTReal32,
	1.0f,
	KMaxTReal32
	};
const TSelectTest TypeReal32::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(4)},
	{_S("<>0.0"),ALL_ROWS-ROW(4)},
	{_S("=-1"),ROW(2)},
	{_S("<2e1"),ROWS(1,6)},
	{_S(">=-100000000000000"),ROWS(2,7)},
	{_S("<1e-36"),ROWS(1,5)},
	{_S(">1e15"),ROW(7)},
	{_S("=.5"),NO_ROWS},
	{_S("<=-.0"),ROWS(1,4)},
	{_S("<1e40"),ALL_ROWS}
	};

struct TypeReal64
	{
public:
	static const TText* const KType;
	static const TReal64 KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeReal64::KType=_S("DOUBLE");
const TReal64 TypeReal64::KValues[]=
	{
	-KMaxTReal64,
	-1.0f,
	-KMinTReal64,
	0.0f,
	KMinTReal64,
	1.0f,
	KMaxTReal64
	};
const TSelectTest TypeReal64::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=0"),ROW(4)},
	{_S("<>0"),ALL_ROWS-ROW(4)},
	{_S("=-1"),ROW(2)},
	{_S("<2"),ROWS(1,6)},
	{_S(">=-100000000000000"),ROWS(2,7)},
	{_S("<1e-300"),ROWS(1,5)},
	{_S(">1e15"),ROW(7)},
	{_S("=.5"),NO_ROWS},
	{_S("<=0"),ROWS(1,4)},
	{_S("<1e40"),ROWS(1,6)}
	};

struct TypeTime
	{
public:
	static const TText* const KType;
	static const TTime KValues[];
	static const TSelectTest KTests[];
	};
const TText* const TypeTime::KType=_S("TIME");
const TTime TypeTime::KValues[]=
	{
	TInt64(0u),				// zero date/time
	_L(":085815"),			// 8:58:15am
	_L("19181010:110000"),	// 11am, 11 Nov 1918
	_L("19750226:"),		// midnight, 27 Mar 1975
	_L("19961130:235959"),	// 11:59:59pm, 31 Dec 1996
	_L("19970000:"),		// midnight, 1 Jan 1997
	_L("19970611:210000"),	// 9pm, 12 July 1997
	_L("19980309:214500"),	// 9:45pm, 10 April 1998
	_L("20700608:")			// midnight, 9 July 2070
	};
const TSelectTest TypeTime::KTests[]=
	{
	{_S("IS NULL"),ROW(0)},
	{_S("IS NOT NULL"),ALL_ROWS},
	{_S("=#12am#"),ROW(1)},
	{_S("<#Jan 1 2100#"),ALL_ROWS},
	{_S("<>#31/12/1996 23:59:59#"),ALL_ROWS-ROW(5)},
	{_S("<#9a#"),ROWS(1,2)},
	{_S(">=#11:59:59pm, 31 Dec 1996#"),ROWS(5,9)},
	{_S("=#9:45pm 10 April, 1998#"),ROW(8)},
	{_S("=#8:58:15#"),ROW(2)}
	};

struct TypeText
	{
public:
	static const TText* const KType;
	static const TText* const KValues[];
	static TSelectTest KTests[];
	};
const TText* const TypeText::KType=_S("VARCHAR(100)");
const TText* const TypeText::KValues[]=
	{
	_S(""),					// this should be equivalent to NULL
	_S("a"),
	_S("aa"),
	_S("aba"),
	_S("like"),
	_S("abcdefghijklmnopqrstuvwxyzlike"),
	_S("likeabcdefghijklmnopqrstuvwxyz"),
	_S("abcdefghijklmnopqrstuvwxyzlikeabcdefghijklmnopqrstuvwxyz"),
	_S("abcdefghijklmnopqrstuvwxyzliveabcdefghijklmnopqrstuvwxyz"),
	_S("l'ke"),
	_S("'Tis")
	};
TSelectTest TypeText::KTests[]=
	{
	{_S("IS NULL"),ROWS(0,1)},
	{_S("IS NOT NULL"),ALL_ROWS-ROW(1)},
//
	{_S("=''"),ROWS(0,1)},					// equivalent to IS NULL
	{_S("<>''"),ALL_ROWS-ROW(1)},			// equivalent to IS NOT NULL
	{_S(">''"),ALL_ROWS-ROW(1)},			// equivalent to IS NOT NULL
	{_S("<''"),NO_ROWS},					// expression is trivially false
	{_S("<=''"),ROWS(0,1)},
	{_S(">=''"),ALL_ROWS+ROW(0)},			// expression is trivially true
//
	{_S("LIKE ''"),ROWS(0,1)},			// synonomous with IS NULL
	{_S("NOT LIKE ''"),ALL_ROWS-ROW(1)},
	{_S("LIKE '?*'"),ALL_ROWS-ROW(1)},	// synonomous with IS NOT NULL
	{_S("NOT LIKE '?*'"),ROWS(0,1)},
	{_S("LIKE '*'"),ALL_ROWS+ROW(0)},	// trivially true
	{_S("NOT LIKE '*'"),NO_ROWS},
//
	{_S("='a'"),ROW(2)},
	{_S("<'ab'"),ROWS(0,3)+ROW(11)},
	{_S("<'abc'"),ROWS(0,4)+ROW(11)},
	{_S("<'b'"),ROWS(0,4)+ROW(6)+ROWS(8,9)+ROW(11)},
	{_S(">'abc'"),ROWS(5,10)},
	{_S("='like'"),ROW(5)},
	{_S("='l''ke'"),ROW(10)},
	{_S("='''Tis'"),ROW(11)},
//
	{_S("LIKE 'a'"),ROW(2)},
	{_S("LIKE 'a*'"),ROWS(2,4)+ROW(6)+ROWS(8,9)},
	{_S("LIKE '*a'"),ROWS(2,4)},
	{_S("LIKE 'a*a'"),ROWS(3,4)},
	{_S("LIKE '*a*'"),ROWS(2,4)+ROWS(6,9)},
//
	{_S("LIKE 'like'"),ROW(5)},
	{_S("LIKE 'l?ke'"),ROW(5)+ROW(10)},
	{_S("LIKE 'like*'"),ROW(5)+ROW(7)},
	{_S("LIKE '*like'"),ROWS(5,6)},
	{_S("LIKE '*like*'"),ROWS(5,8)},
	{_S("LIKE '*likeit*'"),NO_ROWS},
	{_S("LIKE '*li?e*'"),ROWS(5,9)},
	{_S("LIKE '?*li?e*'"),ROW(6)+ROWS(8,9)},
	{_S("LIKE '*li?e*?'"),ROWS(7,9)},
	{_S("LIKE '?*li?e*?'"),ROWS(8,9)},
	{_S("LIKE '*?k?*'"),ROWS(5,10)},
	{_S("LIKE '*?i?e*'"),ROWS(5,9)},
//
	{_S("LIKE '*e*'"),ROWS(5,10)},
	{_S("LIKE '*z*k*e*'"),ROW(6)+ROW(8)},
	{_S("LIKE '\?\?k?'"),ROW(5)+ROW(10)},
	{_S("LIKE '\?\?k*'"),ROW(5)+ROW(7)+ROW(10)},
	{_S("LIKE '*''*'"),ROWS(10,11)},
	{_S("LIKE '?''\?\?'"),ROW(10)},
	{_S("LIKE '?'"),ROW(2)},
	{_S("LIKE '\?\?\?\?'"),ROW(5)+ROWS(10,11)},
	{_S("LIKE '\?\?*\?\?'"),ROWS(5,11)},
	{_S("LIKE '''*'"),ROW(11)}
	};

TEMPLATE_SPECIALIZATION struct SetCol<const TText* const>
	{
	static void SetColL(TDbColNo aCol,const TAny* aVal)
		{
		TheTable.SetColL(aCol,TPtrC(*(const TText* const*)aVal));
		}
	};
TEMPLATE_SPECIALIZATION struct SetCol<const TText*>
	{
	static void SetColL(TDbColNo aCol,const TAny* aVal)
		{
		TheTable.SetColL(aCol,TPtrC(*(const TText* const*)aVal));
		}
	};

struct TypeLongText
	{
public:
	static const TText* const KType;
	};
const TText* const TypeLongText::KType=_S("LONG VARCHAR");

class TestPredicate2 : public TestPredicateBase
	{
public:
	static void RunL();
	};

// write rows equivalent to TypeText and use its tests
void TestPredicate2::RunL()
	{
	Create(TypeLongText::KType);
	TBuf<1022> fill=_S("abcdefghijklmnopqrstuvqxyz");
	fill.AppendFill('.',fill.MaxLength()-fill.Length());

	for (TInt row=0;row<=TInt(elementsof(TypeText::KValues));++row)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,row);
		if (row>0)
			{
			RDbColWriteStream blob;
			blob.OpenLC(TheTable,2);
			switch (row)
				{
			case 0:
				break;
			case 1: case 2: case 3: case 4: case 5: case 10: case 11:
				blob.WriteL(TPtrC(TypeText::KValues[row-1]));
				break;
			case 6:
				blob.WriteL(fill);
				blob.WriteL(_L("like"));
				break;
			case 7:
				blob.WriteL(_L("like"));
				blob.WriteL(fill);
				break;
			case 8:
				blob.WriteL(fill);
				blob.WriteL(_L("like"));
				blob.WriteL(fill);
				break;
			case 9:
				blob.WriteL(fill);
				blob.WriteL(_L("live"));
				blob.WriteL(fill);
				break;
				}
			blob.CommitL();
			CleanupStack::PopAndDestroy();
			}
		TheTable.PutL();
		}
	TheTable.Close();
	TInt r=TheDatabase.Commit();
	TEST2(r, KErrNone);
	TestViewL(TypeText::KTests,elementsof(TypeText::KTests),elementsof(TypeText::KValues));
	CDbKey& key=*CDbKey::NewLC();
	key.AddL(TDbKeyCol(_L("Test"),120));
	r=TheDatabase.CreateIndex(_L("Key"),_L("Compare"),key);
	TEST2(r, KErrNone);
	CleanupStack::PopAndDestroy();
	TestViewL(TypeText::KTests,elementsof(TypeText::KTests),elementsof(TypeText::KValues));
	r=TheDatabase.Execute(_L("DROP TABLE Compare"));
	TEST2(r, KErrNone);
	}

/**
* Utility for DEF063276 fix.
*/

_LIT(KTypeTextKTests44, "Z:\\test\\TypeTextKTests44.dat");
_LIT(KTypeTextKTests46, "Z:\\test\\TypeTextKTests46.dat");
_LIT(KTypeTextKTests47, "Z:\\test\\TypeTextKTests47.dat");

static void ReadDesc(TDes& aDes, const TDesC& aFilename, RFs& aFs)
	{
    TheTest.Printf(_L("---ReadDesc(), aFilename=%S\r\n"), &aFilename);
	RFile file;
	TInt err = file.Open(aFs, aFilename, EFileRead);
	TheTest.Printf(_L("Open file aFilename=%S err = %d\r\n"), &aFilename, err);
	TEST2(err, KErrNone);

	TPtr8 ptr(reinterpret_cast<TUint8*>(const_cast<TUint16*>(aDes.Ptr())), aDes.MaxSize());
	err = file.Read(ptr);
	TheTest.Printf(_L("Read file aFilename=%S err = %d\r\n"), &aFilename, err);
	TEST2(err, KErrNone);
	aDes.SetLength(ptr.Length() / sizeof(TText));
	file.Close();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0633
@SYMTestCaseDesc        Tests the Predicate operators for all types
@SYMTestPriority        Medium
@SYMTestActions         Attempt to check with different types
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestPredicatesL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0633 init "));
	CreateDatabase();
	TestPredicate<TypeBit>::RunL();
	TestPredicate<TypeUint8>::RunL();
	TestPredicate<TypeUint16>::RunL();
	TestPredicate<TypeUint32>::RunL();
	TestPredicate<TypeInt8>::RunL();
	TestPredicate<TypeInt16>::RunL();
	TestPredicate<TypeInt32>::RunL();
	TestPredicate<TypeInt64>::RunL();
	TestPredicate<TypeReal32>::RunL();
	TestPredicate<TypeReal64>::RunL();
	TestPredicate<TypeTime>::RunL();

	/**
	* Work around for DEF063276.
	* These literals are now loaded from Z:\test\TypeTextKTests44.dat,
	* Z:\test\data\TypeTextKTests44.dat and Z:\test\TypeTextKTests44.dat respectively.
	* Bullseye Coverage corrupts these literals to avoid this they are stored in files as to not be touched by Bullseye Coverage.
	*/

	TBuf<16> buf44;
	ReadDesc(buf44, KTypeTextKTests44, TheFs);
	TypeText::KTests[44].iSearchCondition = const_cast<TText*>(buf44.PtrZ());

	TBuf<32> buf46(TypeText::KTests[46].iSearchCondition);
	ReadDesc(buf46, KTypeTextKTests46, TheFs);
	TypeText::KTests[46].iSearchCondition = const_cast<TText*>(buf46.PtrZ());

	TBuf<32> buf47(TypeText::KTests[47].iSearchCondition);
	ReadDesc(buf47, KTypeTextKTests47, TheFs);
	TypeText::KTests[47].iSearchCondition = const_cast<TText*>(buf47.PtrZ());

	// End fix.

	TestPredicate<TypeText>::RunL();
	TestPredicate2::RunL();
	CloseDatabase();
	TheTest.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0634
@SYMTestCaseDesc        DML Query test
                        Test for RDbNamedDatabase::Execute() function
@SYMTestPriority        Medium
@SYMTestActions         Tests for CREATE TABLE,CREATE UNIQUE INDEX,INSET INTO,UPDATE,DELETE,DROP queries
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDataModificationlanguage()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0634 init "));
	Connect();
	ShareDatabase();
	TInt r=TheDatabase.Execute(_L("CREATE TABLE test (ID INTEGER NOT NULL,SALARY DOUBLE)"));
	TEST2(r, KErrNone);
	r=TheDatabase.Execute(_L("CREATE UNIQUE INDEX key ON test (ID)"));
	TEST2(r, KErrNone);

	TheTest.Next(_L("insert-statements"));
	r=TheDatabase.Execute(_L("INSERT INTO test VALUES (0,0)"));
	TEST(r==1);
	r=TheDatabase.Execute(_L("INSERT INTO test (ID) VALUES (1)"));
	TEST(r==1);
	r=TheDatabase.Execute(_L("INSERT INTO test (SALARY,ID) VALUES (20.4,2)"));
	TEST(r==1);

	TheTest.Next(_L("update-statements"));
	r=TheDatabase.Execute(_L("UPDATE test SET SALARY=30000 WHERE ID=1"));
	TEST(r==1);

	TheTest.Next(_L("delete-statements"));
	r=TheDatabase.Execute(_L("DELETE FROM test WHERE SALARY<40"));
	TEST(r==2);
	r=TheDatabase.Execute(_L("DELETE FROM test"));
	TEST(r==1);
	r=TheDatabase.Execute(_L("DROP TABLE test"));
	TEST2(r, KErrNone);
//
	TheTest.Next(_L("larger table"));
	r=TheDatabase.Execute(_L("CREATE TABLE test (ID COUNTER,DATA INTEGER)"));
	TEST2(r, KErrNone);

	TheTest.Next(_L("insert"));
	r=TheDatabase.Begin();
	TEST2(r, KErrNone);
	TBuf<256> sql;
	for (TInt ii=0;ii<100;++ii)
		{
		sql.Format(_L("INSERT INTO test (DATA) VALUES (%D)"),100-ii);
		r=TheDatabase.Execute(sql);
		TEST(r==1);
		}
	r=TheDatabase.Commit();
	TEST2(r, KErrNone);

	TheTest.Next(_L("update"));
	r=TheDatabase.Execute(_L("UPDATE test SET DATA=200 WHERE ID>=40 AND ID<60"));
	TEST(r==20);

	TheTest.Next(_L("delete"));
	r=TheDatabase.Execute(_L("DELETE FROM test WHERE DATA>90"));
	TEST(r==30);

	TheTest.Next(_L("update"));
	r=TheDatabase.Execute(_L("UPDATE test SET DATA=-1"));
	TEST(r==70);

	TheTest.Next(_L("delete"));
	r=TheDatabase.Execute(_L("DELETE FROM test"));
	TEST(r==70);
	r=TheDatabase.Execute(_L("DROP TABLE test"));
	TEST2(r, KErrNone);
	CloseDatabase();
	Disconnect();
	TheTest.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0635
@SYMTestCaseDesc        DBMS SQL parsing and execution tests.Tests for database index order
						Test for RDbNamedDatabase::Execute() function
@SYMTestPriority        Medium
@SYMTestActions         Tests for order by index
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOrderByL()
	{
	static const TReal TestDataSalary[]={10,34000,15000,53200,17800,240000};
	static const TText* const TestDataNames[]={_S("gopher"),_S("james '007' bond"),_S("moneypenny"),_S("Q"),_S("james '007' bond"),_S("M")};
//
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0635 init "));
	Connect();
	ShareDatabase();
	TheDatabase.Begin();
	TInt r=TheDatabase.Execute(_L("CREATE TABLE test (ID INTEGER NOT NULL,SALARY DOUBLE,NAME VARCHAR)"));
	TEST2(r, KErrNone);
	r=TheDatabase.Execute(_L("CREATE UNIQUE INDEX key ON test (ID)"));
	TEST2(r, KErrNone);

	TheTest.Next(_L("insert data"));
	r=TheView.Prepare(TheDatabase,_L("SELECT ID,SALARY,NAME FROM test"),TheView.EInsertOnly);
	TEST2(r, KErrNone);
	TInt ii;
	for (ii=0;ii<6;++ii)
		{
		TheView.InsertL();
		TheView.SetColL(1,6-ii);
		TheView.SetColL(2,TestDataSalary[ii]);
		TheView.SetColL(3,TPtrC(TestDataNames[ii]));
		TheView.PutL();
		}
	r=TheDatabase.Commit();
	TEST2(r, KErrNone);
	TheView.Close();

	TheTest.Next(_L("Order by <index>"));
	// test the index is used here
	r=TheView.Prepare(TheDatabase,_L("SELECT ID FROM test ORDER BY ID"));
	TEST2(r, KErrNone);
	TEST(!TheView.Unevaluated());
	TInt c=0;
	if (TheView.FirstL())
		{
		++c;
		TheView.GetL();
		TInt last=TheView.ColInt(1);
		while (TheView.NextL())
			{
			++c;
			TheView.GetL();
			TInt v=TheView.ColInt(1);
			TEST(v>last);
			last=v;
			}
		}
	TEST(c==6);
	TEST(c==TheView.CountL());
	TheView.Close();

	TheTest.Next(_L("Order by <no-index> 1"));
	// test that no index is used here
	r=TheView.Prepare(TheDatabase,_L("SELECT SALARY FROM test ORDER BY SALARY"));
	TEST2(r, KErrNone);
	TEST(TheView.Unevaluated());
	r=TheView.EvaluateAll();
	TEST2(r, KErrNone);
	c=0;
	if (TheView.FirstL())
		{
		++c;
		TheView.GetL();
		TReal last=TheView.ColReal(1);
		while (TheView.NextL())
			{
			++c;
			TheView.GetL();
			TReal v=TheView.ColReal(1);
			TEST(v>=last);
			last=v;
			}
		}
	TEST(c==6);
	TEST(c==TheView.CountL());
	TheView.Close();

	TheTest.Next(_L("Order by <no-index> 2"));
	// test that no index is used here
	r=TheView.Prepare(TheDatabase,_L("SELECT SALARY FROM test ORDER BY SALARY,NAME"));
	TEST2(r, KErrNone);
	TEST(TheView.Unevaluated());
	r=TheView.EvaluateAll();
	TEST2(r, KErrNone);
	c=0;
	if (TheView.FirstL())
		{
		++c;
		TheView.GetL();
		TReal last=TheView.ColReal(1);
		while (TheView.NextL())
			{
			++c;
			TheView.GetL();
			TReal v=TheView.ColReal(1);
			TEST(v>=last);
			last=v;
			}
		}
	TEST(c==6);
	TEST(c==TheView.CountL());
	TheView.Close();

	TheTest.Next(_L("Order by <no-index> 3"));
	// test that no index is used here
	r=TheView.Prepare(TheDatabase,_L("SELECT NAME FROM test ORDER BY NAME"));
	TEST2(r, KErrNone);
	TEST(TheView.Unevaluated());
	r=TheView.EvaluateAll();
	TEST2(r, KErrNone);
	c=0;
	if (TheView.FirstL())
		{
		++c;
		TheView.GetL();
		TBuf<50> last=TheView.ColDes(1);
		while (TheView.NextL())
			{
			++c;
			TheView.GetL();
			TPtrC v=TheView.ColDes(1);
			TEST(v>=last);
			last=v;
			}
		}
	TEST(c==6);
	TEST(c==TheView.CountL());
	TheView.Close();

	TheTest.Next(_L("Order by <no-index> 4"));
	// test that no index is used here
	r=TheView.Prepare(TheDatabase,_L("SELECT NAME FROM test ORDER BY NAME,SALARY"));
	TEST2(r, KErrNone);
	TEST(TheView.Unevaluated());
	r=TheView.EvaluateAll();
	TEST2(r, KErrNone);
	c=0;
	if (TheView.FirstL())
		{
		++c;
		TheView.GetL();
		TBuf<50> last=TheView.ColDes(1);
		while (TheView.NextL())
			{
			++c;
			TheView.GetL();
			TPtrC v=TheView.ColDes(1);
			TEST(v>=last);
			last=v;
			}
		}
	TEST(c==6);
	TEST(c==TheView.CountL());
	TheView.Close();

	TheTest.Next(_L("Order by + search <no-index>"));
	// test that no index is used here
	r=TheView.Prepare(TheDatabase,_L("SELECT ID,SALARY FROM test WHERE SALARY>30000 ORDER BY SALARY DESC"));
	TEST2(r, KErrNone);
	TEST(TheView.Unevaluated());
	r=TheView.EvaluateAll();
	TEST2(r, KErrNone);
	c=0;
	if (TheView.FirstL())
		{
		++c;
		TheView.GetL();
		TReal last=TheView.ColReal(2);
		while (TheView.NextL())
			{
			++c;
			TheView.GetL();
			TReal v=TheView.ColReal(2);
			TEST(v<=last);
			last=v;
			}
		}
	TEST(c==3);
	TEST(c==TheView.CountL());
	TheView.Close();
//
	CloseDatabase();
	Disconnect();

	TheTest.Next(_L("Order by <index> Finished"));
	TheTest.End();
	}

LOCAL_C void doMain()
	{
	TheTest.Start(_L("Predicate tests"));
	TRAPD(errCode, TestPredicatesL());
	TEST2(errCode, KErrNone);

	TheTest.Next(_L("DML execution"));
	TestDataModificationlanguage();

	TheTest.Next(_L("ORDER BY clause"));
	TRAP(errCode, TestOrderByL());
	TEST2(errCode, KErrNone);
	TheTest.End();
	}

// Prepare the test directory.
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	TEST2(r, KErrNone);
//
	r=TheFs.MkDir(KTestDatabase);
	TEST(r==KErrNone || r==KErrAlreadyExists);
	}

// Initialise the cleanup stack.
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	TEST(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TEST2(r, KErrNone);
	}

// Test streaming conversions.
GLDEF_C TInt E32Main()
    {
	TheTest.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	TheTest.Start(_L("Standard database"));
	doMain();

	// clean up data file used by this test - must be done before call to End() - DEF047652
	::TestCleanup();
	TheTest.End();
//
	__UHEAP_MARKEND;


	delete TheTrapCleanup;
	TheTest.Close();
	return 0;
    }
