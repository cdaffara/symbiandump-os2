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
//

#include <e32test.h>
#include <bautils.h>
#include <sqldb.h>

///////////////////////////////////////////////////////////////////////////////////////

#define UNUSED_VAR(a) (a) = (a)
#define IGNORE_ERR(a) (a) = (a)

RTest TheTest(_L("t_sqlpanic test"));

_LIT(KTestDir, "c:\\test\\");
_LIT(KTestDbName, "c:\\test\\t_sqlpanic.db");

_LIT(KCategory, "SqlDb");

///////////////////////////////////////////////////////////////////////////////////////

void DeleteTestFiles()
	{
	RSqlDatabase::Delete(KTestDbName);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteTestFiles();
		TheTest(EFalse, aLine);
		}
	}
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		DeleteTestFiles();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

void CreateTestDir()
    {
    RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);

	err = fs.MkDir(KTestDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	fs.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////

//Panic thread function. 
//It will cast aData parameter to a TFunctor pointer and call it.
//The expectation is that the called function will panic and kill the panic thread.
TInt ThreadFunc(void* aData)
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	User::SetJustInTime(EFalse);	// disable debugger panic handling
	
	TFunctor* obj = reinterpret_cast<TFunctor*> (aData);
	TEST(obj != NULL);
	(*obj)();//call the panic function
	
	delete tc;
	
	return KErrNone;		
	}

//Panic test.
//PanicTest function will create a new thread - panic thread, giving it a pointer to the function which has to
//be executed and the expectation is that the function will panic and kill the panic thread.
//PanicTest function will check the panic thread exit code, exit category and the panic code.
void PanicTest(TFunctor& aFunctor, TExitType aExpectedExitType, const TDesC& aExpectedCategory, TInt aExpectedPanicCode)
	{
	RThread thread;
	_LIT(KThreadName,"SqlDbPanicThread");
	TEST2(thread.Create(KThreadName, &ThreadFunc, 0x2000, 0x1000, 0x10000, (void*)&aFunctor, EOwnerThread), KErrNone);
	
	TRequestStatus status;
	thread.Logon(status);
	TEST2(status.Int(), KRequestPending);
	thread.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);	// enable debugger panic handling

	TEST2(thread.ExitType(), aExpectedExitType);
	TEST(thread.ExitCategory() == aExpectedCategory);
	TEST2(thread.ExitReason(), aExpectedPanicCode);
	
	CLOSE_AND_WAIT(thread);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////     Panic test functions    /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Panic when calling RSqlDatabase::Exec() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Exec8 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.Exec(_L8("CREATE TABLE A(f integer)"));//panic here
		}
	};
static TSqlDatabase_NotCreated_Exec8 TheSqlDatabase_NotCreated_Exec8;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::Exec() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Exec : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.Exec(_L("CREATE TABLE A(f integer)"));//panic here
		}
	};
static TSqlDatabase_NotCreated_Exec TheSqlDatabase_NotCreated_Exec;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::GetSecuritySettings() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_SecuritySettings : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		RSqlSecurityPolicy securityPolicy;
		(void)db.GetSecurityPolicy(securityPolicy);//panic here
		}
	};
static TSqlDatabase_NotCreated_SecuritySettings TheSqlDatabase_NotCreated_SecuritySettings;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::SetIsolationLevel() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_SetIsolationLevel : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.SetIsolationLevel(RSqlDatabase::EReadUncommitted);//panic here
		}
	};
static TSqlDatabase_NotCreated_SetIsolationLevel TheSqlDatabase_NotCreated_SetIsolationLevel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::LastErrorMessage() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_LastErrorMessage : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.LastErrorMessage();//panic here
		}
	};
static TSqlDatabase_NotCreated_LastErrorMessage TheSqlDatabase_NotCreated_LastErrorMessage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::LastInsertedRowId() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_LastInsertedRowId : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.LastInsertedRowId();//panic here
		}
	};
static TSqlDatabase_NotCreated_LastInsertedRowId TheSqlDatabase_NotCreated_LastInsertedRowId;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::Size() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Size : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		(void)db.Size();//panic here
		}
	};
static TSqlDatabase_NotCreated_Size TheSqlDatabase_NotCreated_Size;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::Size(TSize&) on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Size2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		RSqlDatabase::TSize size;
		(void)db.Size(size);//panic here
		}
	};
static TSqlDatabase_NotCreated_Size2 TheSqlDatabase_NotCreated_Size2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::InTransaction() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_InTransaction : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		(void)db.InTransaction();//panic here
		}
	};
static TSqlDatabase_NotCreated_InTransaction TheSqlDatabase_NotCreated_InTransaction;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::Attach() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Attach : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.Attach(_L("C:\\TEST\\A.DB"), _L("A"));//panic here
		}
	};
static TSqlDatabase_NotCreated_Attach TheSqlDatabase_NotCreated_Attach;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::Detach() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Detach : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.Detach(_L("A"));//panic here
		}
	};
static TSqlDatabase_NotCreated_Detach TheSqlDatabase_NotCreated_Detach;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlDatabase::Compact() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Compact : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		db.Compact(2048);//panic here
		}
	};
static TSqlDatabase_NotCreated_Compact TheSqlDatabase_NotCreated_Compact;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling async RSqlDatabase::Compact() on an invalid RSqlDatabase object.
class TSqlDatabase_NotCreated_Compact2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TRequestStatus stat;
		db.Compact(2048, stat);//panic here
		}
	};
static TSqlDatabase_NotCreated_Compact2 TheSqlDatabase_NotCreated_Compact2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::Reset() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_Reset : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.Reset();//panic here
		}
	};
static TSqlStatement_NotCreated_Reset TheSqlStatement_NotCreated_Reset;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::Exec() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_Exec : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.Exec();//panic here
		}
	};
static TSqlStatement_NotCreated_Exec TheSqlStatement_NotCreated_Exec;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::Next() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_Next : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.Next();//panic here
		}
	};
static TSqlStatement_NotCreated_Next TheSqlStatement_NotCreated_Next;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ParameterIndex() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ParameterIndex : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ParameterIndex(_L("ABV"));//panic here
		}
	};
static TSqlStatement_NotCreated_ParameterIndex TheSqlStatement_NotCreated_ParameterIndex;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnIndex() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnIndex : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnIndex(_L("ABV"));//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnIndex TheSqlStatement_NotCreated_ColumnIndex;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnType() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnType : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnType(1);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnType TheSqlStatement_NotCreated_ColumnType;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnSize() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnSize : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnSize(1);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnSize TheSqlStatement_NotCreated_ColumnSize;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindNull() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindNull : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindNull(1);//panic here
		}
	};
static TSqlStatement_NotCreated_BindNull TheSqlStatement_NotCreated_BindNull;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindInt() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindInt : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindInt(1, 2);//panic here
		}
	};
static TSqlStatement_NotCreated_BindInt TheSqlStatement_NotCreated_BindInt;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindInt64() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindInt64 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindInt64(1, TInt64(2));//panic here
		}
	};
static TSqlStatement_NotCreated_BindInt64 TheSqlStatement_NotCreated_BindInt64;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindReal() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindReal : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindReal(1, 2.5);//panic here
		}
	};
static TSqlStatement_NotCreated_BindReal TheSqlStatement_NotCreated_BindReal;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindText() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindText : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindText(1, _L("ABV"));//panic here
		}
	};
static TSqlStatement_NotCreated_BindText TheSqlStatement_NotCreated_BindText;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindBinary() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindBinary : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindBinary(1, _L8("ABV"));//panic here
		}
	};
static TSqlStatement_NotCreated_BindBinary TheSqlStatement_NotCreated_BindBinary;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::BindZeroBlob() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_BindZeroBlob : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.BindZeroBlob(1, 100);//panic here
		}
	};
static TSqlStatement_NotCreated_BindZeroBlob TheSqlStatement_NotCreated_BindZeroBlob;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::BindZeroBlob() giving an invalid parameter index.
class TSqlStatement_OutOfBounds_BindZeroBlob : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST2(db.Exec(_L8("CREATE TABLE D(A INTEGER, B BLOB)")), 1);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("INSERT INTO D VALUES(1, :Prm1)")), KErrNone);
		stmt.BindZeroBlob(12, 100);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_OutOfBounds_BindZeroBlob TheSqlStatement_OutOfBounds_BindZeroBlob;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::IsNull() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_IsNull : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.IsNull(1);//panic here
		}
	};
static TSqlStatement_NotCreated_IsNull TheSqlStatement_NotCreated_IsNull;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnInt() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnInt : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnInt(1);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnInt TheSqlStatement_NotCreated_ColumnInt;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnInt64() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnInt64 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnInt64(1);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnInt64 TheSqlStatement_NotCreated_ColumnInt64;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnReal() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnReal : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnReal(1);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnReal TheSqlStatement_NotCreated_ColumnReal;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnText() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnText : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		TPtrC ptr;
		(void)stmt.ColumnText(1, ptr);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnText TheSqlStatement_NotCreated_ColumnText;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnText() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnText2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		TBuf<10> buf;
		stmt.ColumnText(1, buf);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnText2 TheSqlStatement_NotCreated_ColumnText2;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnBinary() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnBinary : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		TPtrC8 ptr;
		(void)stmt.ColumnBinary(1, ptr);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnBinary TheSqlStatement_NotCreated_ColumnBinary;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnBinary() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnBinary2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		TBuf8<10> buf;
		stmt.ColumnBinary(1, buf);//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnBinary2 TheSqlStatement_NotCreated_ColumnBinary2;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::Prepare() giving an invalid RSqlDatabase object.
class TSqlStatement_DbNotCreated_Prepare : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		RSqlStatement stmt;
		stmt.Prepare(db, _L("CREATE TABLE A(d INTEGER)"));//panic here
		}
	};
static TSqlStatement_DbNotCreated_Prepare TheSqlStatement_DbNotCreated_Prepare;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::Prepare() giving an invalid RSqlDatabase object.
class TSqlStatement_DbNotCreated_Prepare8 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		RSqlStatement stmt;
		stmt.Prepare(db, _L8("CREATE TABLE A(d INTEGER)"));//panic here
		}
	};
static TSqlStatement_DbNotCreated_Prepare8 TheSqlStatement_DbNotCreated_Prepare8;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
//Panic when an attempt is made to call RSqlStatement::Prepare() twice on the same RSqlStatement object.
class TSqlStatement_CreateTwice : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L("CREATE TABLE A(d INTEGER)")), KErrNone);
		stmt.Prepare(db, _L("CREATE TABLE A(d INTEGER)"));//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_CreateTwice TheSqlStatement_CreateTwice;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::Prepare() twice on the same RSqlStatement object.
class TSqlStatement_CreateTwice8 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("CREATE TABLE A(d INTEGER)")), KErrNone);
		stmt.Prepare(db, _L8("CREATE TABLE A(d INTEGER)"));//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_CreateTwice8 TheSqlStatement_CreateTwice8;
*/		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::ColumnType() giving an invalid column index.
class TSqlStatement_OutOfBounds_ColumnType : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A INTEGER, B INTEGER, C INTEGER)")) >= 0);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT A, B, C FROM D")), KErrNone);
		stmt.ColumnType(12);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_OutOfBounds_ColumnType TheSqlStatement_OutOfBounds_ColumnType;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::ColumnSize() giving an invalid column index.
class TSqlStatement_OutOfBounds_ColumnSize : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A INTEGER, B INTEGER, C INTEGER)")) >= 0);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT A, B, C FROM D")), KErrNone);
		stmt.ColumnSize(-25);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_OutOfBounds_ColumnSize TheSqlStatement_OutOfBounds_ColumnSize;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::BindNull() giving an invalid parameter index.
class TSqlStatement_OutOfBounds_Bind : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A INTEGER, B INTEGER, C INTEGER)")) >= 0);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT A, B, C FROM D WHERE A = :V")), KErrNone);
		stmt.BindNull(2);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_OutOfBounds_Bind TheSqlStatement_OutOfBounds_Bind;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::ColumnInt() giving an invalid column index.
class TSqlStatement_OutOfBounds_ColumnValue : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A INTEGER, B INTEGER, C INTEGER)")) >= 0);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT A, B, C FROM D")), KErrNone);
		stmt.ColumnInt(56);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_OutOfBounds_ColumnValue TheSqlStatement_OutOfBounds_ColumnValue;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::ColumnCount() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnCount : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		stmt.ColumnCount();//panic here
		}
	};
static TSqlStatement_NotCreated_ColumnCount TheSqlStatement_NotCreated_ColumnCount;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when calling RSqlStatement::DeclaredColumnType() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_DeclaredColumnType : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		TSqlColumnType colType;
		stmt.DeclaredColumnType(0, colType);//panic here
		}
	};
static TSqlStatement_NotCreated_DeclaredColumnType TheSqlStatement_NotCreated_DeclaredColumnType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::DeclaredColumnType() giving an invalid column index.
class TSqlStatement_OutOfBounds_DeclaredColumnType : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A INTEGER, B INTEGER, C INTEGER)")) >= 0);
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT A, B, C FROM D")), KErrNone);
		TSqlColumnType colType;
		stmt.DeclaredColumnType(8, colType);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_OutOfBounds_DeclaredColumnType TheSqlStatement_OutOfBounds_DeclaredColumnType;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::ColumnName() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ColumnName : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		RSqlStatement stmt;
		TPtrC columnName;
		(void)stmt.ColumnName(0, columnName);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_NotCreated_ColumnName TheSqlStatement_NotCreated_ColumnName;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::ParameterName() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ParameterName : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		RSqlStatement stmt;
		TPtrC parameterName;
		(void)stmt.ParameterName(0, parameterName);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_NotCreated_ParameterName TheSqlStatement_NotCreated_ParameterName;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlStatement::ParamName() on an invalid RSqlStatement object.
class TSqlStatement_NotCreated_ParamName : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		RSqlStatement stmt;
		TPtrC paramName;
		(void)stmt.ParamName(0, paramName);//panic here
		stmt.Close();
		db.Close();
		}
	};
static TSqlStatement_NotCreated_ParamName TheSqlStatement_NotCreated_ParamName;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlColumnReadStream::ColumnText() with an invalid RSqlStatement object.
class TColumnReadStream_ColumnText_Statement : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		RSqlColumnReadStream strm;
		strm.ColumnText(stmt, 0);//panic here
		strm.Close();
		}
	};
static TColumnReadStream_ColumnText_Statement TheColumnReadStream_ColumnText_Statement;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlColumnReadStream::ColumnBinary() with an invalid RSqlStatement object.
class TColumnReadStream_ColumnBinary_Statement : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		RSqlColumnReadStream strm;
		strm.ColumnBinary(stmt, 0);//panic here
		strm.Close();
		}
	};
static TColumnReadStream_ColumnBinary_Statement TheColumnReadStream_ColumnBinary_Statement;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlColumnReadStream::ColumnText() with an invalid column index.
class TColumnReadStream_ColumnText_Column : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A TEXT)")) >= 0);
		
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT * FROM D")), KErrNone);
		
		RSqlColumnReadStream strm;
		strm.ColumnText(stmt, 8);//panic here
		
		strm.Close();
		stmt.Close();
		db.Close();
		}
	};
static TColumnReadStream_ColumnText_Column TheColumnReadStream_ColumnText_Column;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlColumnReadStream::ColumnBinary() with an invalid column index.
class TColumnReadStream_ColumnBinary_Column : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A BLOB)")) >= 0);
		
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT * FROM D")), KErrNone);
		
		RSqlColumnReadStream strm;
		strm.ColumnBinary(stmt, 3);//panic here
		
		strm.Close();
		stmt.Close();
		db.Close();
		}
	};
static TColumnReadStream_ColumnBinary_Column TheColumnReadStream_ColumnBinary_Column;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlColumnReadStream::ColumnText() when the statement object is not at row.
class TColumnReadStream_ColumnText_AtRow : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A TEXT)")) >= 0);
		
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT * FROM D")), KErrNone);
		
		RSqlColumnReadStream strm;
		strm.ColumnText(stmt, 0);//panic here
		
		strm.Close();
		stmt.Close();
		db.Close();
		}
	};
static TColumnReadStream_ColumnText_AtRow TheColumnReadStream_ColumnText_AtRow;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlColumnReadStream::ColumnBinary() when the statement object is not at row.
class TColumnReadStream_ColumnBinary_AtRow : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A BLOB)")) >= 0);
		
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT * FROM D")), KErrNone);
		
		RSqlColumnReadStream strm;
		strm.ColumnBinary(stmt, 0);//panic here
		
		strm.Close();
		stmt.Close();
		db.Close();
		}
	};
static TColumnReadStream_ColumnBinary_AtRow TheColumnReadStream_ColumnBinary_AtRow;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlParamWriteStream::BindText() with an invalid RSqlStatement object.
class TParamWriteStream_BindText_Statement : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		RSqlParamWriteStream strm;
		strm.BindText(stmt, 0);//panic here
		strm.Close();
		}
	};
static TParamWriteStream_BindText_Statement TheParamWriteStream_BindText_Statement;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlParamWriteStream::BindBinary() with an invalid RSqlStatement object.
class TParamWriteStream_BindBinary_Statement : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlStatement stmt;
		RSqlParamWriteStream strm;
		strm.BindBinary(stmt, 0);//panic here
		strm.Close();
		}
	};
static TParamWriteStream_BindBinary_Statement TheParamWriteStream_BindBinary_Statement;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlParamWriteStream::BindText() with an invalid parameter index.
class TParamWriteStream_BindText_Column : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A TEXT)")) >= 0);
		
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT * FROM D WHERE A = :Val")), KErrNone);
		
		RSqlParamWriteStream strm;
		strm.BindText(stmt, 8);//panic here
		
		strm.Close();
		stmt.Close();
		db.Close();
		}
	};
static TParamWriteStream_BindText_Column TheParamWriteStream_BindText_Column;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlParamWriteStream::BindBinary() with an invalid parameter index.
class TParamWriteStream_BindBinary_Column : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlDatabase db;
		TEST2(db.Create(KTestDbName), KErrNone);
		TEST(db.Exec(_L8("CREATE TABLE D(A BLOB)")) >= 0);
		
		RSqlStatement stmt;
		TEST2(stmt.Prepare(db, _L8("SELECT * FROM D WHERE A = :Val")), KErrNone);
		
		RSqlParamWriteStream strm;
		strm.BindBinary(stmt, 3);//panic here
		
		strm.Close();
		stmt.Close();
		db.Close();
		}
	};
static TParamWriteStream_BindBinary_Column TheParamWriteStream_BindBinary_Column;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::DefaultPolicy() on an invalid object.
class TSqlSecurity_DefaultPolicy : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlSecurityPolicy securityPolicy;
		TSecurityPolicy policy = securityPolicy.DefaultPolicy();//panic here
		UNUSED_VAR(policy);
		}
	};
static TSqlSecurity_DefaultPolicy TheSqlSecurity_DefaultPolicy;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::SetDbPolicy() with an invalid policy type.
class TSqlSecurity_Set1 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		RSqlSecurityPolicy::TPolicyType policyType = static_cast <RSqlSecurityPolicy::TPolicyType> (12345);
		TSecurityPolicy policy(TSecurityPolicy::EAlwaysFail);
		securityPolicy.SetDbPolicy(policyType, policy);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_Set1 TheSqlSecurity_Set1;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::SetPolicy() with an invalid database object type.
class TSqlSecurity_Set2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		RSqlSecurityPolicy::TObjectType objectType = static_cast <RSqlSecurityPolicy::TObjectType> (-113);
		TSecurityPolicy policy(TSecurityPolicy::EAlwaysFail);
		securityPolicy.SetPolicy(objectType, _L("ATbl"), RSqlSecurityPolicy::EWritePolicy, policy);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_Set2 TheSqlSecurity_Set2;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::SetPolicy() with an invalid database object name.
class TSqlSecurity_Set3 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		TSecurityPolicy policy(TSecurityPolicy::EAlwaysFail);
		securityPolicy.SetPolicy(RSqlSecurityPolicy::ETable, KNullDesC, RSqlSecurityPolicy::EReadPolicy, policy);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_Set3 TheSqlSecurity_Set3;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::DbPolicy() with an invalid policy type.
class TSqlSecurity_Get1 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		RSqlSecurityPolicy::TPolicyType policyType = static_cast <RSqlSecurityPolicy::TPolicyType> (12345);
		securityPolicy.DbPolicy(policyType);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_Get1 TheSqlSecurity_Get1;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::Policy() with an invalid database object type.
class TSqlSecurity_Get2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		RSqlSecurityPolicy::TObjectType objectType = static_cast <RSqlSecurityPolicy::TObjectType> (-113);
		securityPolicy.Policy(objectType, _L("BTbl"), RSqlSecurityPolicy::EReadPolicy);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_Get2 TheSqlSecurity_Get2;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::Policy() with an invalid database object name.
class TSqlSecurity_Get3 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		securityPolicy.Policy(RSqlSecurityPolicy::ETable, KNullDesC, RSqlSecurityPolicy::EWritePolicy);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_Get3 TheSqlSecurity_Get3;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
//Panic when an attempt is made to call RSqlSecurityPolicy::Create() on an already created object.
class TSqlSecurity_CreateTwice : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
		RSqlSecurityPolicy securityPolicy;
		TInt err = securityPolicy.Create(defaultPolicy);
		TEST2(err, KErrNone);
		securityPolicy.Create(defaultPolicy);//panic here
		securityPolicy.Close();
		}
	};
static TSqlSecurity_CreateTwice TheSqlSecurity_CreateTwice;
*/		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlSecurityPolicy::ExternalizeL() on an invalid object.
class TSqlSecurity_Externalize : public TFunctor
	{
private:		
	virtual void operator()()
		{
		RSqlSecurityPolicy securityPolicy;
		RWriteStream stream;
		TRAPD(err, securityPolicy.ExternalizeL(stream));//panic here
		IGNORE_ERR(err);
		}
	};
static TSqlSecurity_Externalize TheSqlSecurity_Externalize;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call TSqlScalarFullSelectQuery method and the database is not set.
class TSqlScalarFullSelectQuery_InvalidDatabase : public TFunctor
	{
private:		
	virtual void operator()()
		{
		TSqlScalarFullSelectQuery query;
		TRAP_IGNORE((void)query.SelectIntL(_L("SELECT Id FROM A WHERE Name = 'AAA'")));
		}
	};
static TSqlScalarFullSelectQuery_InvalidDatabase TheSqlScalarFullSelectQuery_InvalidDatabase;
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlBlobReadStream::OpenL() with an invalid RSqlDatabase object.
class TBlobReadStream_Open_Database1 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		CTrapCleanup* tc = CTrapCleanup::New();
		
		RSqlDatabase db;
		RSqlBlobReadStream strm;
		TRAP_IGNORE(strm.OpenL(db, _L("Tbl"),_L("Col"), 1));//panic here
		strm.Close();
		
		delete tc;
		}
	};
static TBlobReadStream_Open_Database1 TheBlobReadStream_Open_Database1;
		
class TBlobReadStream_Open_Database2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		CTrapCleanup* tc = CTrapCleanup::New();
		
		RSqlDatabase db;
		RSqlBlobReadStream strm;
		TRAP_IGNORE(strm.OpenL(db, _L("Tbl"),_L("Col"), 1, _L("Db")));//panic here
		strm.Close();
		
		delete tc;
		}
	};
static TBlobReadStream_Open_Database2 TheBlobReadStream_Open_Database2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlBlobReadStream::SizeL() on a unitialized RSqlBlobReadStream object.
class TBlobReadStream_Size_Stream : public TFunctor
	{
private:		
	virtual void operator()()
		{
		CTrapCleanup* tc = CTrapCleanup::New();
		
		RSqlBlobReadStream strm;
		TRAP_IGNORE(strm.SizeL());//panic here
		strm.Close();
		
		delete tc;
		}
	};
TBlobReadStream_Size_Stream TheBlobReadStream_Size_Stream;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlBlobWriteStream::OpenL() with an invalid RSqlDatabase object.
class TBlobWriteStream_Open_Database1 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		CTrapCleanup* tc = CTrapCleanup::New();
		
		RSqlDatabase db;
		RSqlBlobWriteStream strm;
		TRAP_IGNORE(strm.OpenL(db, _L("Tbl"),_L("Col"), 1));//panic here
		strm.Close();
		
		delete tc;
		}
	};
static TBlobWriteStream_Open_Database1 TheBlobWriteStream_Open_Database1;
		
class TBlobWriteStream_Open_Database2 : public TFunctor
	{
private:		
	virtual void operator()()
		{
		CTrapCleanup* tc = CTrapCleanup::New();
		
		RSqlDatabase db;
		RSqlBlobWriteStream strm;
		TRAP_IGNORE(strm.OpenL(db, _L("Tbl"),_L("Col"), 1, _L("Db")));//panic here
		strm.Close();
		
		delete tc;
		}
	};
static TBlobWriteStream_Open_Database2 TheBlobWriteStream_Open_Database2;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Panic when an attempt is made to call RSqlBlobWriteStream::SizeL() on a unitialized RSqlBlobWriteStream object.
class TBlobWriteStream_Size_Stream : public TFunctor
	{
private:		
	virtual void operator()()
		{
		CTrapCleanup* tc = CTrapCleanup::New();
		
		RSqlBlobWriteStream strm;
		TRAP_IGNORE(strm.SizeL());//panic here
		strm.Close();
		
		delete tc;
		}
	};
TBlobWriteStream_Size_Stream TheBlobWriteStream_Size_Stream;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1619
@SYMTestCaseDesc		RSqlDatabase panic tests
						Run a second thread. The second thread executes given RSqlDatabase method calling
						it with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlDatabase panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
                        REQ10405
                        REQ10407
*/	
void DatabaseTests()
	{
	TheTest.Printf(_L("'RSqlDatabase object not created - Exec 8' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Exec8, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - Exec' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Exec, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - GetSecuritySettings' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_SecuritySettings, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - Attach' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Attach, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - Detach' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Detach, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - SetIsolationLevel' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_SetIsolationLevel, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - LastErrorMessage' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_LastErrorMessage, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - LastInsertedRowId' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_LastInsertedRowId, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - Size' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Size, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - Size(TSize&)' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Size2, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - InTransaction' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_InTransaction, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - Compact' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Compact, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlDatabase object not created - async Compact' panic\r\n"));
	PanicTest(TheSqlDatabase_NotCreated_Compact2, EExitPanic, KCategory, 2);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1620
@SYMTestCaseDesc		RSqlStatement panic tests
						Run a second thread. The second thread executes given RSqlStatement method calling
						it with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void StatementTests()
	{
	TheTest.Printf(_L("'RSqlStatement object not created - Reset' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_Reset, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - Exec' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_Exec, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - Next' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_Next, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - ParameterIndex' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ParameterIndex, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - ColumnIndex' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnIndex, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - ColumnType' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnType, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - ColumnSize' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnSize, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - BindNull' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindNull, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - BindInt' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindInt, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - BindInt64' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindInt64, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - BindReal' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindReal, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - BindText' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindText, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - BindBinary' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindBinary, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement object not created - BindZeroBlob' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_BindZeroBlob, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement::BindZeroBlob() - invalid parameter index' panic\r\n"));
	PanicTest(TheSqlStatement_OutOfBounds_BindZeroBlob, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);

	TheTest.Printf(_L("'RSqlStatement object not created - ColumnInt' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnInt, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - IsNull' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnInt, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnInt64' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnInt64, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnReal' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnReal, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnText' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnText, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnText2' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnText2, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnBinary' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnBinary, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnBinary2' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnBinary2, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement - database not created - Prepare' panic\r\n"));
	PanicTest(TheSqlStatement_DbNotCreated_Prepare, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement - database not created - Prepare 8' panic\r\n"));
	PanicTest(TheSqlStatement_DbNotCreated_Prepare8, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement - ColumnType - Column index out of bounds' panic\r\n"));
	PanicTest(TheSqlStatement_OutOfBounds_ColumnType, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlStatement - ColumnSize - Column index out of bounds' panic\r\n"));
	PanicTest(TheSqlStatement_OutOfBounds_ColumnSize, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlStatement - Bind - Parameter index out of bounds' panic\r\n"));
	PanicTest(TheSqlStatement_OutOfBounds_Bind, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlStatement - Column value - Parameter index out of bounds' panic\r\n"));
	PanicTest(TheSqlStatement_OutOfBounds_ColumnValue, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlStatement object not created - ColumnCount' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnCount, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlStatement object not created - DeclaredColumnType' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_DeclaredColumnType, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlStatement - DeclaredColumnType - Column index out of bounds' panic\r\n"));
	PanicTest(TheSqlStatement_OutOfBounds_DeclaredColumnType, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);

	TheTest.Printf(_L("'RSqlStatement - ColumnName' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ColumnName, EExitPanic, KCategory, 2);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);

	TheTest.Printf(_L("'RSqlStatement - ParameterName' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ParameterName, EExitPanic, KCategory, 2);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlStatement - ParamName' panic\r\n"));
	PanicTest(TheSqlStatement_NotCreated_ParamName, EExitPanic, KCategory, 2);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1625
@SYMTestCaseDesc		RSqlColumnReadStream panic tests
						Run a second thread. The second thread executes given RSqlColumnReadStream method calling
						it with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlColumnReadStream panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void ColumnStreamTests()
	{
	TheTest.Printf(_L("'RSqlColumnReadStream - ColumnText - invalid statement' panic\r\n"));
	PanicTest(TheColumnReadStream_ColumnText_Statement, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlColumnReadStream - ColumnBinary - invalid statement' panic\r\n"));
	PanicTest(TheColumnReadStream_ColumnBinary_Statement, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlColumnReadStream - ColumnText - invalid column index' panic\r\n"));
	PanicTest(TheColumnReadStream_ColumnText_Column, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlColumnReadStream - ColumnBinary - invalid column index' panic\r\n"));
	PanicTest(TheColumnReadStream_ColumnBinary_Column, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);

	TheTest.Printf(_L("'RSqlColumnReadStream - ColumnText - not at row' panic\r\n"));
	PanicTest(TheColumnReadStream_ColumnText_AtRow, EExitPanic, KCategory, 11);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);

	TheTest.Printf(_L("'RSqlColumnReadStream - ColumnBinary - not at row' panic\r\n"));
	PanicTest(TheColumnReadStream_ColumnBinary_AtRow, EExitPanic, KCategory, 11);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1626
@SYMTestCaseDesc		RSqlParamWriteStream panic tests
						Run a second thread. The second thread executes given RSqlParamWriteStream method calling
						it with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlParamWriteStream panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void ParameterStreamTests()
	{
	TheTest.Printf(_L("'RSqlParamWriteStream - BindText - invalid statement' panic\r\n"));
	PanicTest(TheParamWriteStream_BindText_Statement, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlParamWriteStream - BindBinary - invalid statement' panic\r\n"));
	PanicTest(TheParamWriteStream_BindBinary_Statement, EExitPanic, KCategory, 2);
	
	TheTest.Printf(_L("'RSqlParamWriteStream - BindText - invalid parameter index' panic\r\n"));
	PanicTest(TheParamWriteStream_BindText_Column, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	
	TheTest.Printf(_L("'RSqlParamWriteStream - BindBinary - invalid parameter index' panic\r\n"));
	PanicTest(TheParamWriteStream_BindBinary_Column, EExitPanic, KCategory, 5);
	TEST2(RSqlDatabase::Delete(KTestDbName), KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1638
@SYMTestCaseDesc		RSqlSecurityPolicy panic tests
						Run a second thread. The second thread executes given RSqlSecurityPolicy method calling
						it with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlSecurityPolicy panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/	
void SecuritySettingsTests()
	{
	TheTest.Printf(_L("'RSqlSecurityPolicy::SetDbPolicy - invalid policy type' panic\r\n"));
	PanicTest(TheSqlSecurity_Set1, EExitPanic, KCategory, 4);
	
	TheTest.Printf(_L("'RSqlSecurityPolicy::SetPolicy - invalid database object type' panic\r\n"));
	PanicTest(TheSqlSecurity_Set2, EExitPanic, KCategory, 4);
	
	TheTest.Printf(_L("'RSqlSecurityPolicy::SetPolicy - invalid database object name' panic\r\n"));
	PanicTest(TheSqlSecurity_Set3, EExitPanic, KCategory, 4);

	TheTest.Printf(_L("'RSqlSecurityPolicy::DbPolicy - invalid policy type' panic\r\n"));
	PanicTest(TheSqlSecurity_Get1, EExitPanic, KCategory, 4);
	
	TheTest.Printf(_L("'RSqlSecurityPolicy::Policy - invalid database object type' panic\r\n"));
	PanicTest(TheSqlSecurity_Get2, EExitPanic, KCategory, 4);
	
	TheTest.Printf(_L("'RSqlSecurityPolicy::Policy - invalid database object name' panic\r\n"));
	PanicTest(TheSqlSecurity_Get3, EExitPanic, KCategory, 4);

	TheTest.Printf(_L("'RSqlSecurityPolicy::DefaultPolicy - invalid object' panic\r\n"));
	PanicTest(TheSqlSecurity_DefaultPolicy, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlSecurityPolicy::Externalize - panic\r\n"));
	PanicTest(TheSqlSecurity_Externalize, EExitPanic, KCategory, 2);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1812
@SYMTestCaseDesc		TSqlScalarFullSelectQuery panic tests
						Run a second thread. The second thread executes given TSqlScalarFullSelectQuery method calling
						it with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			TSqlScalarFullSelectQuery panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void ScalarFullSelectTests()
	{
	TheTest.Printf(_L("'TheSqlScalarFullSelectQuery, invalid database' - panic\r\n"));
	PanicTest(TheSqlScalarFullSelectQuery_InvalidDatabase, EExitPanic, KCategory, 2);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4092
@SYMTestCaseDesc		RSqlBlobReadStream panic tests
						Run a second thread. The second thread executes a given RSqlBlobReadStream method calling
						the method with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlBlobReadStream panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
                        REQ10410
                        REQ10411
*/
void BlobReadStreamTests()
	{
	TheTest.Printf(_L("'RSqlBlobReadStream::OpenL(), invalid database' - panic test 1\r\n"));
	PanicTest(TheBlobReadStream_Open_Database1, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlBlobReadStream::OpenL(), invalid database' - panic test 2\r\n"));
	PanicTest(TheBlobReadStream_Open_Database2, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlBlobReadStream::SizeL(), invalid stream' - panic test\r\n"));
	PanicTest(TheBlobReadStream_Size_Stream, EExitPanic, KCategory, 2);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4093
@SYMTestCaseDesc		RSqlBlobWriteStream panic tests
						Run a second thread. The second thread executes a given RSqlBlobWriteStream method calling
						the method with wrong arguments, or in a bad context,...The method panics the second thread. 
						The main thread captures and checks the panic code.
@SYMTestPriority		High
@SYMTestActions			RSqlBlobWriteStream panic tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
                        REQ10418
*/
void BlobWriteStreamTests()
	{
	TheTest.Printf(_L("'RSqlBlobWriteStream::OpenL(), invalid database' - panic test 1\r\n"));
	PanicTest(TheBlobWriteStream_Open_Database1, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlBlobWriteStream::OpenL(), invalid database' - panic test 2\r\n"));
	PanicTest(TheBlobWriteStream_Open_Database2, EExitPanic, KCategory, 2);

	TheTest.Printf(_L("'RSqlBlobWriteStream::SizeL(), invalid stream' - panic test\r\n"));
	PanicTest(TheBlobWriteStream_Size_Stream, EExitPanic, KCategory, 2);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1619 RSqlDatabase - panic tests"));
	DatabaseTests();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1620 RSqlStatement - panic tests"));
	StatementTests();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1625 RSqlColumnReadStream - panic tests"));
	ColumnStreamTests();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1626 RSqlParamWriteStream - panic tests"));
	ParameterStreamTests();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1638 RSqlSecurityPolicy - panic tests"));
	SecuritySettingsTests();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1812 TheSqlScalarFullSelectQuery - panic tests"));
	ScalarFullSelectTests();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4092 RSqlBlobReadStream - panic tests"));
	BlobReadStreamTests();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4093 RSqlBlobWriteStream - panic tests"));
	BlobWriteStreamTests();
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	CreateTestDir();
	DeleteTestFiles();
	DoTests();
	DeleteTestFiles();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
