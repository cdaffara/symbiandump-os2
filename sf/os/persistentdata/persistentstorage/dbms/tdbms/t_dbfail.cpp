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
#include <e32math.h>
#include <s32file.h>
#include <e32test.h>
#include "t_dbfail.h"

_LIT(KTestTitle,"t_dbfail: DBMS Failure mode test");
GLDEF_D RTest test(KTestTitle);
GLDEF_D RDbs TheDbs;
GLDEF_D RDbNamedDatabase TheDatabase;
GLDEF_D TClientHeap KClientHeap;
GLDEF_D TServerHeap KServerHeap;


LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
LOCAL_D RDbView TheView;
LOCAL_D RDbTable TheTable;
LOCAL_D RDbRowSet::TAccess Access;
LOCAL_D CDbColSet* TheColSet;
LOCAL_D CDbKey* TheKey;
LOCAL_D const TDesC* TheSql;
LOCAL_D TBuf<64> TheFormat;

const TInt KTestCleanupStack=0x20;
_LIT(KTestDatabase,"C:\\DBMS-TST\\t_fail.db");
_LIT(TableName,"Table1");
_LIT(TableName2,"Table_two");
_LIT(TableNameX,"Bad Table Name");
_LIT(IndexName,"Index1");
_LIT(IndexName2,"Index2");
_LIT(Column1,"column_one");
_LIT(Column1Fold,"COLUMN_ONE");
_LIT(Column2,"column_2");
_LIT(Column2X,"column_2%");
_LIT(SimpleSql,"select * from Table1");
_LIT(UpdateSql,"update Table1 SET column_2='hello'");

//const TPtrC ComplexSql(_S("select * from Table1 where column_one<0 and not column_one is null or column_2 not like '*fred*' and column_2>'m' order by column_one desc"));
const TPtrC ComplexSql[]=
	{
	_S("select * from Table1 where column_one<0 and column_one is null"),
	_S("select * from Table1 where column_one<0 and (column_one is null and column_2 like '')"),
	_S("select * from Table1 where (column_one<0 and column_one is null) and column_2 like ''"),
	_S("select * from Table1 where (column_one<0 and column_one is null) and (column_2 like '' and column_one>0)"),
	_S("select * from Table1 where (column_one<0 and column_one is null) and (column_2 like '' and column_one>0 and column_one is null)"),
	_S("select * from Table1 where (column_one<0 and column_one is null and column_one = 10) and (column_2 like '' and column_one>0 and column_one is null)"),
	_S("select * from Table1 where column_one<0 and column_one is null and column_one = 10 and column_2 like '' and column_one>0 and column_one is null")
	};

struct SSqlErrors
	{
	const TText* iSql;
	TInt iError;
	};

LOCAL_D SSqlErrors const BadSql[]=
	{
		{_S("sponge"),KErrArgument},
		{_S("select * from widget"),KErrNotFound},
		{_S("select * from Table1 where x = 0"),KErrNotFound},
		{_S("select * from Table1 where x 0 like"),KErrArgument},
		{_S("select * from Table1 where column_2>'a' and column_one<'z'"),KErrGeneral},
		{_S("select from Table1"),KErrArgument},
		{_S("select x, from Table1"),KErrArgument},
		{_S("select x from Table1"),KErrNotFound},
		{_S("select column_2 column_one from Table1"),KErrArgument},
		{_S("select * from Table1 order by x"),KErrNotFound},
		{_S("select * from Table1 order column_one"),KErrArgument},
		{_S("select * from Table1 order by column_one down"),KErrArgument}
	};

GLDEF_C void Connect()
	{
	TInt r=TheDbs.Connect();
	test (r==KErrNone);
	TheDbs.ResourceMark();
	}

GLDEF_C void Disconnect()
	{
	TheDbs.ResourceCheck();
	TheDbs.Close();
	}


//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version
LOCAL_C void DbCreateL()
	{
	User::LeaveIfError(TheDatabase.Replace(TheFs,KTestDatabase,TheFormat));
	}

//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version
LOCAL_C void DbOpenL()
	{
	User::LeaveIfError(TheDatabase.Open(TheFs,KTestDatabase,TheFormat));
	CleanupClosePushL(TheDatabase);
	delete TheDatabase.TableNamesL();	// force a schema load
	CleanupStack::Pop();
	}

//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version
LOCAL_C void DbShareL()
	{
	User::LeaveIfError(TheDatabase.Open(TheDbs,KTestDatabase,TheFormat));
	CleanupClosePushL(TheDatabase);
	delete TheDatabase.TableNamesL();	// force a schema load
	CleanupStack::Pop();
	}


/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0612
@SYMTestCaseDesc        Database validity test
@SYMTestPriority        Medium
@SYMTestActions         Tests for opening and closing of database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestOpenL()
	{
	_LIT(KFileNotFound,"not a database");
	TInt r=TheDatabase.Open(TheFs,KFileNotFound);
	test (r==KErrNotFound || r==KErrPathNotFound);
//
	_LIT(KPathNotFound,"C:\\not a path\\database.db");
	r=TheDatabase.Open(TheFs,KPathNotFound);
	test (r==KErrPathNotFound);
//
	_LIT(KNotFormat,"not.a.dbx");
	r=TheDatabase.Open(TheFs,KFileNotFound,KNotFormat);
	test (r==KErrNotFound || r==KErrPathNotFound);
//
	DbCreateL();
	TheDatabase.Close();
	r=TheDatabase.Open(TheFs,KTestDatabase,TUid::Uid(0x01234567).Name());
	test (r==KErrNone); // New code has no loadable drivers, it is irrelevant to expect error here
	TheDatabase.Close(); // We have added it here because previous statement does not return error anymore
//
	RFile f;
	r=f.Replace(TheFs,KTestDatabase,EFileWrite);
	test (r==KErrNone);
	TCheckedUid type(KDirectFileStoreLayoutUid);
	r=f.Write(type.Des());
	test (r==KErrNone);
	f.Close();
	r=TheDatabase.Open(TheFs,KTestDatabase);
	test (r==KErrNotSupported);
//
	_LIT(KDefaultFormat,"epoc");
	r=TheDatabase.Open(TheFs,KTestDatabase,KDefaultFormat);
	test (r==KErrNotSupported); // We expect not supported db here
	}

class TClient : public TContext
	{
public:
	TClient() {}
private:
	void OpenDbL() const
		{DbOpenL();}
	};
class TServer : public TContext
	{
public:
	TServer() {}
private:
	void OpenDbL() const
		{DbShareL();}
	};

const TClient KClient;
const TServer KServer;

void TFail::Test(const THeapFail& aHeap,const TContext* aContext)
	{
	TInt ii;
	TInt errCode;
	for (ii=1;;++ii)
		{
		if (aContext)
			{
			TRAP(errCode, aContext->OpenDbL());
			if(errCode != KErrNone)
				return;
			}
		aHeap.Fail(ii);
		aHeap.Mark();
		TRAPD(r,RunL());
		aHeap.Reset();
		if (r==KErrNone)
			break;
		test(r==KErrNoMemory);
		if (aContext)
			TheDatabase.Close();
		aHeap.Check();
		}
	End();
	if (aContext)
		TheDatabase.Close();
	aHeap.Check();
	}

class TFailCreateDatabase : public TFail
	{
	void RunL()
		{DbCreateL();}
	void End()
		{TheDatabase.Close();}
	};

class TFailOpenDatabase : public TFail
	{
	void RunL()
		{DbOpenL();}
	void End()
		{TheDatabase.Close();}
	};

class TFailShareDatabase : public TFail
	{
	void RunL()
		{DbShareL();}
	void End()
		{TheDatabase.Close();}
	};

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0613
@SYMTestCaseDesc        Tests for allocation failures on creating a database
@SYMTestPriority        Medium
@SYMTestActions         Tests for allocation failure for differently sourced databases
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void OriginsL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0613 Allocation failures on Creating a database "));
	TFailCreateDatabase t1;
	t1.Test(KClientHeap);
//
	test.Next(_L("Fail to create existing database"));
	TUint att;
	TInt r=TheFs.Att(KTestDatabase,att);
	test (r==KErrNone);
	r=TheDatabase.Create(TheFs,KTestDatabase,TheFormat);
	test (r==KErrAlreadyExists);
	r=TheFs.Att(KTestDatabase,att);
	test (r==KErrNone);
//
	test.Next(_L("Allocation failures on Open"));
	TFailOpenDatabase t2;
	t2.Test(KClientHeap);
//
	test.Next(_L("Allocation failures on 1st Share"));
	Connect();
	TFailShareDatabase t3;
	t3.Test(KClientHeap);
	t3.Test(KServerHeap);
//
	test.Next(_L("Allocation failures on 2nd Share"));
	DbShareL();
	RDbNamedDatabase temp=TheDatabase;TheDatabase=RDbNamedDatabase();
	t3.Test(KClientHeap);
	t3.Test(KServerHeap);
	temp.Close();
	Disconnect();
	test.End();
	}


class TFailCreateTable : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheDatabase.CreateTable(TableName,*TheColSet));}
	};

class TFailAlterTable : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheDatabase.AlterTable(TableName,*TheColSet));}
	};

class TFailDropTable : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheDatabase.DropTable(TableName));}
	};

class TFailCreateIndex : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheDatabase.CreateIndex(IndexName,TableName,*TheKey));}
	};

class TFailDropIndex : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheDatabase.DropIndex(IndexName,TableName));}
	};

class TFailGetObject : public TFail
	{
protected:
	void End()
		{delete iObject;}
protected:
	CBase* iObject;
	};

class TFailDatabaseTables : public TFailGetObject
	{
	void RunL()
		{iObject=TheDatabase.TableNamesL();}
	};

class TFailDatabaseColSet : public TFailGetObject
	{
	void RunL()
		{iObject=TheDatabase.ColSetL(TableName);}
	};

class TFailDatabaseIndexes : public TFailGetObject
	{
	void RunL()
		{iObject=TheDatabase.IndexNamesL(TableName);}
	};

class TFailDatabaseKeys : public TFailGetObject
	{
	void RunL()
		{iObject=TheDatabase.KeyL(IndexName,TableName);}
	};

const TInt KRowCount=60;

LOCAL_C void WriteTableL()
	{
	DbOpenL();
	TInt r=TheTable.Open(TheDatabase,TableName);
	test (r==KErrNone);
	TheDatabase.Begin();
	for (TInt ii=0;ii<KRowCount;++ii)
		{
		TheTable.InsertL();
		TheTable.SetColL(1,TUint((ii*17)%KRowCount));
		TheTable.PutL();
		}
	r=TheDatabase.Commit();
	test (r==KErrNone);
	TheTable.Close();
	TheDatabase.Close();
	}

LOCAL_C void DatabaseL()
	{
	test.Start(_L("Adding and dropping tables"));
	DbCreateL();
// ensure the database locking list has been allocated
	TheDatabase.Begin();
	TheDatabase.Commit();
//
	CDbColSet *col=CDbColSet::NewLC();
//
	test.Next(_L("Empty Column Set"));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Invalid table name"));
	col->AddL(TDbCol(Column1,EDbColInt32));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableNameX,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Invalid column name"));
	col->AddL(TDbCol(Column2X,EDbColBit));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Duplicate column name"));
	col->Remove(Column2X);
	col->AddL(TDbCol(Column1Fold,EDbColBit));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Invalid column type"));
	col->Remove(Column1);
	col->AddL(TDbCol(Column2,TDbColType(-1)));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Invalid maximum length"));
	col->Remove(Column2);
	col->AddL(TDbCol(Column2,EDbColInt32,0));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Invalid attributes"));
	col->Remove(Column2);
	TDbCol cc(Column2,EDbColInt32);
	cc.iAttributes=13;
	col->AddL(cc);
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Adding/dropping a table name twice"));
	col->Remove(Column2);
	col->AddL(TDbCol(Column2,EDbColText8));
	__UHEAP_MARK;
	test(TheDatabase.CreateTable(TableName,*col)==KErrNone);
	test(TheDatabase.CreateTable(TableName,*col)==KErrAlreadyExists);
	test(TheDatabase.DropTable(TableNameX)!=KErrNone);
	test(TheDatabase.DropTable(TableName)==KErrNone);
	test(TheDatabase.DropTable(TableName)==KErrNotFound);
	__UHEAP_MARKEND;
//
	test.Next(_L("Adding and dropping indexes"));
	test(TheDatabase.CreateTable(TableName,*col)==KErrNone);
	TheDatabase.Close();
	CDbKey *key=CDbKey::NewLC();
	__UHEAP_MARK;
	DbOpenL();
	test(TheDatabase.CreateIndex(IndexName,TableName,*key)!=KErrNone);
	TheDatabase.Close();
	__UHEAP_MARKEND;
	key->AddL(Column2X());
	__UHEAP_MARK;
	DbOpenL();
	test(TheDatabase.CreateIndex(IndexName,TableName,*key)!=KErrNone);
	TheDatabase.Close();
	__UHEAP_MARKEND;
	key->Clear();
	key->AddL(Column1());
	__UHEAP_MARK;
	DbOpenL();
	test(TheDatabase.CreateIndex(TableNameX,TableName,*key)!=KErrNone);
	TheDatabase.Close();
	__UHEAP_CHECK(0);
	DbOpenL();
	test(TheDatabase.CreateIndex(IndexName,TableNameX,*key)!=KErrNone);
	TheDatabase.Close();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	DbOpenL();
	test(TheDatabase.CreateIndex(IndexName,TableName,*key)==KErrNone);
	test(TheDatabase.CreateIndex(IndexName,TableName,*key)==KErrAlreadyExists);
	test(TheDatabase.DropIndex(TableNameX,TableName)!=KErrNone);
	test(TheDatabase.DropIndex(IndexName,TableNameX)!=KErrNone);
	test(TheDatabase.DropIndex(IndexName,TableName)==KErrNone);
	test(TheDatabase.DropIndex(IndexName,TableName)==KErrNotFound);
	test(TheDatabase.DropTable(TableName)==KErrNone);
	test(TheDatabase.DropIndex(IndexName,TableName)==KErrNotFound);
	TheDatabase.Close();
	__UHEAP_MARKEND;
//
	test.Next(_L("Allocation failure during DDL"));
	TFailCreateTable fct;
	TFailAlterTable fat;
	TFailDropTable fdt;
	TFailCreateIndex fci;
	TFailDropIndex fdi;
	TheColSet=CDbColSet::NewL();
	TheColSet->AddL(TDbCol(Column1,EDbColUint16));
	TheKey=CDbKey::NewL();
	TheKey->AddL(Column1());
	fct.Test(KClientHeap,KClient);
	WriteTableL();
	TheColSet->AddL(TDbCol(Column2,EDbColText));
	fat.Test(KClientHeap,KClient);
	fci.Test(KClientHeap,KClient);
	fdi.Test(KClientHeap,KClient);
	fdt.Test(KClientHeap,KClient);
//
	test.Next(_L("Allocation failure during server DDL"));
	Connect();
	TheColSet->Remove(Column2);
	fct.Test(KClientHeap,KServer);
	WriteTableL();
	TheColSet->AddL(TDbCol(Column2,EDbColText));
	fat.Test(KClientHeap,KServer);
	fci.Test(KClientHeap,KServer);
	fdi.Test(KClientHeap,KServer);
	fdt.Test(KClientHeap,KServer);
//
	TheColSet->Remove(Column2);
	fct.Test(KServerHeap,KServer);
	WriteTableL();
	TheColSet->AddL(TDbCol(Column2,EDbColText));
	fat.Test(KServerHeap,KServer);
	fci.Test(KServerHeap,KServer);
	fdi.Test(KServerHeap,KServer);
	fdt.Test(KServerHeap,KServer);
	Disconnect();
//
	delete TheColSet;
	delete TheKey;

//
	test.Next(_L("Allocation failure on schema enquiry"));
	DbCreateL();
	test(TheDatabase.CreateTable(TableName,*col)==KErrNone);
	test(TheDatabase.CreateIndex(IndexName,TableName,*key)==KErrNone);
	CleanupStack::PopAndDestroy(2);	// columns set and key
	TheDatabase.Close();
	TFailDatabaseTables t4;
	TFailDatabaseColSet t5;
	TFailDatabaseIndexes t6;
	TFailDatabaseKeys t7;
	t4.Test(KClientHeap,KClient);
	t5.Test(KClientHeap,KClient);
	t6.Test(KClientHeap,KClient);
	t7.Test(KClientHeap,KClient);
//
	test.Next(_L("Allocation failure on server schema enquiry"));
	Connect();
	t4.Test(KClientHeap,KServer);
	t4.Test(KServerHeap,KServer);
	t5.Test(KClientHeap,KServer);
	t5.Test(KServerHeap,KServer);
	t6.Test(KClientHeap,KServer);
	t6.Test(KServerHeap,KServer);
	t7.Test(KClientHeap,KServer);
	t7.Test(KServerHeap,KServer);
	Disconnect();
	test.End();
	}

class TFailOpenTable : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheTable.Open(TheDatabase,TableName,Access));}
	void End()
		{TheTable.Close();}
	};

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0614
@SYMTestCaseDesc        Tests for allocation failure on opening and closing of database
@SYMTestPriority        Medium
@SYMTestActions         Tests for opening and closing of database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTableL(const THeapFail& aHeap,const TContext& aContext)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0614 Allocation failure on Open "));
	TFailOpenTable fot;
	Access=RDbRowSet::EUpdatable;
	fot.Test(aHeap,aContext);
	Access=RDbRowSet::EReadOnly;
	fot.Test(aHeap,aContext);
	Access=RDbRowSet::EInsertOnly;
	fot.Test(aHeap,aContext);
//
	test.Next(_L("Open invalid table"));
	aContext.OpenDbL();
	__UHEAP_MARK;
	TInt r=TheTable.Open(TheDatabase,TableNameX);
	test (r!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Set invalid index"));
	r=TheTable.Open(TheDatabase,TableName);
	test (r==KErrNone);
	__UHEAP_MARK;
	r=TheTable.SetIndex(IndexName2);
	test (r!=KErrNone);
	__UHEAP_MARKEND;
//
	test.Next(_L("Allocation failure on 2nd Open"));
	RDbTable table(TheTable);
	Access=RDbRowSet::EUpdatable;
	fot.Test(aHeap);
	Access=RDbRowSet::EReadOnly;
	fot.Test(aHeap);
	Access=RDbRowSet::EInsertOnly;
	fot.Test(aHeap);
	table.Close();
	TheDatabase.Close();
	test.End();
	}

LOCAL_C void TestTableDDL(const TContext& aContext)
	{
	test.Start(_L("DDL while open"));
	aContext.OpenDbL();
	TInt r=TheTable.Open(TheDatabase,TableName);
	test (r==KErrNone);
	CDbColSet* set=CDbColSet::NewLC();
	set->AddL(TDbCol(Column1,EDbColText));
	r=TheDatabase.CreateTable(TableName2,*set);
	test (r==KErrNone);
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrNone);
	TheTable.Close();
	r=TheTable.Open(TheDatabase,TableName2);
	test (r==KErrNone);
//
	set->AddL(TDbCol(Column2,EDbColUint32));
	r=TheDatabase.AlterTable(TableName2,*set);
	test (r==KErrNone);
	CleanupStack::PopAndDestroy();		// set
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrDisconnected);
	TheTable.Reset();
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrDisconnected);
	TheTable.Close();
	r=TheTable.Open(TheDatabase,TableName2);
	test (r==KErrNone);
//
	CDbKey* key=CDbKey::NewLC();
	key->AddL(Column2());
	r=TheDatabase.CreateIndex(IndexName2,TableName,*key);
	test (r==KErrNone);
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrNone);
	r=TheDatabase.DropIndex(IndexName2,TableName);
	test (r==KErrNone);
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrNone);
//
	r=TheDatabase.CreateIndex(IndexName,TableName2,*key);
	test (r==KErrNone);
	CleanupStack::PopAndDestroy();	// key
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrDisconnected);
	TheTable.Close();
	r=TheTable.Open(TheDatabase,TableName2);
	test (r==KErrNone);
//
	r=TheDatabase.DropIndex(IndexName,TableName2);
	test (r==KErrNone);
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrDisconnected);
	TheTable.Close();
	r=TheTable.Open(TheDatabase,TableName2);
	test (r==KErrNone);
//
	r=TheDatabase.DropTable(TableName2);
	test (r==KErrNone);
	TRAP(r,TheTable.CountL(TheTable.EQuick));
	test (r==KErrDisconnected);
	TheTable.Close();
	TheDatabase.Close();
	test.End();
	}

LOCAL_C void TableL()
	{
	test.Start(_L("Testing Client-side"));
	TestTableL(KClientHeap,KClient);
	TestTableDDL(KClient);
	test.Next(_L("Testing Client-Server"));
	Connect();
	TestTableL(KClientHeap,KServer);
	TestTableL(KServerHeap,KServer);
	TestTableDDL(KServer);
	Disconnect();
	test.End();
	}

class TFailExecuteSQL : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheDatabase.Execute(*TheSql));}
	void End()
		{}
	};

class TFailPrepareView : public TFail
	{
	void RunL()
		{User::LeaveIfError(TheView.Prepare(TheDatabase,*TheSql,Access));}
	void End()
		{TheView.Close();}
	};

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0615
@SYMTestCaseDesc        Tests for allocation failure on prepare
@SYMTestPriority        Medium
@SYMTestActions         Tests for error on updating a row set data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestView(const THeapFail& aHeap,const TContext& aContext)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0615 Allocation failure on Prepare "));
	TFailPrepareView fpv;
	TheSql=&SimpleSql;
	Access=RDbRowSet::EUpdatable;
	fpv.Test(aHeap,aContext);
	Access=RDbRowSet::EReadOnly;
	fpv.Test(aHeap,aContext);
	Access=RDbRowSet::EInsertOnly;
	fpv.Test(aHeap,aContext);
//
	test.Next(_L("Allocation failure on Prepare (complex SQL)"));
	for (TUint ii=0;ii<sizeof(ComplexSql)/sizeof(ComplexSql[0]);++ii)
		{
		TheSql=&ComplexSql[ii];
		Access=RDbRowSet::EUpdatable;
		fpv.Test(aHeap,aContext);
		}
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0616
@SYMTestCaseDesc        Bad SQL query test
@SYMTestPriority        Medium
@SYMTestActions         Test for bad query
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestSQLL(const TContext& aContext)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0616 Bad SQL "));
	aContext.OpenDbL();
	for (TUint ii=0;ii<sizeof(BadSql)/sizeof(BadSql[0]);++ii)
		test(TheView.Prepare(TheDatabase,TPtrC(BadSql[ii].iSql))==BadSql[ii].iError);
	TheDatabase.Close();
	test.End();
	}

/**
@SYMTestCaseID          SYSLIB-DBMS-CT-0617
@SYMTestCaseDesc        Tests for updation of an SQL statement
@SYMTestPriority        Medium
@SYMTestActions         Tests for update SQL statement
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestUpdateSQL(const THeapFail& aHeap,const TContext& aContext)
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0617 Test for UPDATE SQL statement "));
	TFailExecuteSQL fsql;
	TheSql=&UpdateSql;
	fsql.Test(aHeap,aContext);
	test.End();
	}

LOCAL_C void ViewL()
	{
	test.Start(_L("Client side"));
	TestView(KClientHeap,KClient);
	TestSQLL(KClient);
	test.Next(_L("Client-Server"));
	Connect();
	TestView(KClientHeap,KServer);
	TestView(KServerHeap,KServer);
	TestSQLL(KServer);
	TestUpdateSQL(KClientHeap,KClient);
	TestUpdateSQL(KServerHeap,KClient);
	Disconnect();
	test.End();
	}

//TFailIncrementalUpdate implements the base class' virtual methods - RunL() and End().
//TFailIncrementalUpdate::RunL() is called by the base class' Test() method, which simulates
//OOM failures and checks the behaviour of the "incremental update" statement used by RunL().
class TFailIncrementalUpdate : public TFail
	{
	virtual void RunL()
		{
		RDbUpdate dbUpdate;
		CleanupClosePushL(dbUpdate);
		User::LeaveIfError(dbUpdate.Execute(TheDatabase, _L("UPDATE A SET Name = 'ModifiedNameString' WHERE Id2 > 10")));
		TInt step = 0;
		for(TInt err=1;err>0;++step)
			{
			err = dbUpdate.Next();
			User::LeaveIfError(err);
			}
		test(step > 1);//just to be sure that the test executes dbUpdate.Next() more than once
		CleanupStack::PopAndDestroy(&dbUpdate);
		}
	virtual void End()
		{
		TheDatabase.Close();
		}
	};

/**
@SYMTestCaseID          SYSLIB-DBMS-UT-3414
@SYMTestCaseDesc        "Incremental update" operations - OOM test.
@SYMTestPriority        High
@SYMTestActions         Create a test database with one table and insert some records there (> 100).
						Run an "incremental update" operation in OOM loop.
@SYMTestExpectedResults The test should not fail or panic.
@SYMDEF INC101720
*/
LOCAL_C void IncrementalUpdateTest(const THeapFail& aHeap)
	{
	//Create a test shared database with a table
	TheDatabase.Close();
	TheDbs.Close();
	TInt err = TheDbs.Connect();
	test(err == KErrNone);
	err = TheDatabase.Replace(TheFs, KTestDatabase);
	test(err == KErrNone);
	TheDatabase.Close();
	err = TheDatabase.Open(TheDbs, KTestDatabase);
	test(err == KErrNone);
	//Create a test table and fill the table with enough test records (> 100)
	err = TheDatabase.Execute(_L("CREATE TABLE A(Id COUNTER, Id2 INTEGER, Name LONG VARCHAR)"));
	test(err == KErrNone);
	const TInt KTestRecCount = 110;
	err = TheDatabase.Begin();
	test(err == KErrNone);
	for(TInt i=0;i<KTestRecCount;++i)
		{
		_LIT(KSqlFmtStr, "INSERT INTO A(Id2, Name) VALUES(%d, 'TestNameString')");
		TBuf<100> sql;
		TUint32 id = Math::Random() % KTestRecCount;
		sql.Format(KSqlFmtStr, id + 1);
		err = TheDatabase.Execute(sql);
		test(err == 1);
		}
	err = TheDatabase.Commit();
	test(err == KErrNone);
	//The OOM test
	TFailIncrementalUpdate testObj;
	testObj.Test(aHeap);
	//Cleanup
	TheDatabase.Close();
	TheDbs.Close();
	}

//
// Testing the DBMS for failure modes
//
LOCAL_C void doMainL()
	{
	test.Start(_L("Class RDbNamedDatabase"));
	__UHEAP_MARK;
	OriginsL();
	__UHEAP_CHECK(0);
	__UHEAP_MARK;
	Origins2();
	__UHEAP_CHECK(0);
	test.Next(_L("Class RDbDatabase"));
	DatabaseL();
	__UHEAP_CHECK(0);
	test.Next(_L("Class RDbTable"));
	TableL();
	__UHEAP_CHECK(0);
	test.Next(_L("Class RDbView"));
	ViewL();
	__UHEAP_MARKEND;
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

GLDEF_C TInt E32Main()
	{
	test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-CT-0612 Locating a database "));
	TRAPD(r,TestOpenL());
	test(r==KErrNone);
	PrepareDbFmtString();
	TRAP(r,TestOpen2());
	test(r==KErrNone);
	test.Next(_L("Standard database"));
	TRAP(r,doMainL());
	test(r==KErrNone);
	test.Next(_L("Secure database"));
	TRAP(r,doMainL());
	test(r==KErrNone);
	test.Next(_L("ISAM database"));
	TheFormat=_S("epoc[12345678]");
	TRAP(r,OriginsL());
	test(r==KErrNone);
	TRAP(r,Origins2());
	test(r==KErrNone);
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3414 \"Incremental update\" - client test "));
	IncrementalUpdateTest(KClientHeap);
	test.End();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-DBMS-UT-3414 \"Incremental update\" - client-server test "));
	IncrementalUpdateTest(KServerHeap);
	test.End();

	::DeleteDataFile(KTestDatabase);		// clean up data file used by this test - must be done before call to End() - DEF047652
	test.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	test.Close();
	return 0;
	}
