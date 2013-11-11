// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_sqloom.h"

RTest TheTest(_L("t_sqloom2 test"));

const TInt KSqlBufMaxLen = 16 * 1024;
const TInt KBlobStrLen = KSqlBufMaxLen / 2 - 100;
const TInt KBlobLen = KBlobStrLen;

TBuf8<KSqlBufMaxLen> TheSqlBuf;
TBuf8<KBlobStrLen> TheSqlBuf2;

///////////////////////////////////////////////////////////////////////////////////////
///////////////         RSqlStatement OOM tests         ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

_LIT8(KSqlDbString, "BEGIN;\
					 CREATE TABLE BBB(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, Fld4 TEXT, Fld5 LONGBLOB);\
                     INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(4562, 123456789012345, 78612.0091, 'text data');\
                     COMMIT;");
_LIT8(KSqlDbString2,"INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(1, 34, 10.9897, 'data2');");

const TInt KLongColumnSize = 3013;
const TInt KLongParameterSize = 4501;

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static TUint16 TheTextColumnData[MAX(KLongColumnSize, KLongParameterSize)];
static TUint8 TheBinaryColumnData[MAX(KLongColumnSize, KLongParameterSize) * sizeof(TUint16)];

//"RSqlStatement::Prepare()" OOM test (8-bit SELECT SQL statement)
void PrepareStmt8L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString, "SELECT * FROM BBB WHERE Fld1=? AND Fld4<>?");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::PrepareL()" OOM test (8-bit SELECT SQL statement)
void PrepareStmt8_2L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString, "SELECT * FROM BBB WHERE Fld1=? AND Fld4<>?");
	aStmt.PrepareL(aDb, KSqlString);
	}

//"RSqlStatement::Prepare()" OOM test (8-bit SELECT SQL statement), syntax error
void PrepareBadStmt8L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString, "SELECT123 * FROM BBB WHERE Fld1=? AND Fld4<>?");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (8-bit SELECT SQL statement, move next)
void PrepareMoveStmt8L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString, "SELECT * FROM BBB WHERE Fld1=? AND Fld4<>?");
	TInt err = aStmt.Prepare(aDb, KSqlString);
    User::LeaveIfError(err);
    err = aStmt.BindInt(0, 1);
    User::LeaveIfError(err);
    err = aStmt.BindText(1, _L("data244weewfn43wr83224iu23ewkjfbrektug4i433b3k45b"));
    User::LeaveIfError(err);
    err = aStmt.Next();
    if(err == KSqlAtRow)
        {
        err = KErrNone;
        }
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (8-bit INSERT SQL statement)
void PrepareInsertStmt8L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString, "INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(2, 22, 22.2222, '2-2-2-2');");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare(), RSqlStatement::Exec(), RSqlStatement::Next()" OOM test (8-bit INSERT SQL statement, long column)
void ExecInsertNextStmt8L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString1, "INSERT INTO BBB(fld1, fld2, fld3, fld4, fld5) VALUES(10, 100, 100.001, :Prm1, :Prm2);");
	_LIT8(KSqlString2, "SELECT * FROM BBB WHERE fld1 = 10");
	TInt err = aStmt.Prepare(aDb, KSqlString1);
	if(err == KErrNone)
		{
		for(TInt i=0;i<KLongColumnSize;++i)
			{
			TheTextColumnData[i] = (TUint16)i;
			}
		const TPtrC textVal(TheTextColumnData, KLongColumnSize);
		const TPtrC8 binVal((TUint8*)TheTextColumnData, KLongColumnSize * sizeof(TUint16));
		TInt err = aStmt.BindText(0, textVal);
		if(err == KErrNone)
			{
			err = aStmt.BindBinary(1, binVal);
			}
		if(err == KErrNone)
			{
			err = aStmt.Exec();
			}
		}
	if(err >= 0)
		{
		aStmt.Close();
		err = aStmt.Prepare(aDb, KSqlString2);
		if(err == KErrNone)
			{
			err = aStmt.Next();
			}
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (8-bit INSERT SQL statement with parameters)
void PrepareInsertPrmStmt8L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT8(KSqlString, "INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(:Prm0, :Prm1, :Prm2, :Prm3);");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (16-bit SELECT SQL statement)
void PrepareStmt16L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString, "SELECT * FROM BBB");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::PrepareL()" OOM test (16-bit SELECT SQL statement)
void PrepareStmt16_2L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString, "SELECT * FROM BBB");
	aStmt.PrepareL(aDb, KSqlString);
	}

//"RSqlStatement::Prepare()" OOM test (16-bit SELECT SQL statement), syntax error
void PrepareBadStmt16L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString, "23478SELECT * FROM BBB");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (16-bit SELECT SQL statement, move next)
void PrepareMoveStmt16L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString, "SELECT * FROM BBB");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	if(err == KErrNone)
		{
		err = aStmt.Next();
		if(err == KSqlAtRow)
			{
			err = KErrNone;
			}
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (16-bit INSERT SQL statement)
void PrepareInsertStmt16L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString, "INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(2, 22, 22.2222, '2-2-2-2');");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare(), RSqlStatement::Exec(), RSqlStatement::Next()" OOM test (16-bit INSERT SQL statement, long column)
void ExecInsertNextStmt16L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString1, "INSERT INTO BBB(fld1, fld2, fld3, fld4, fld5) VALUES(10, 100, 100.001, :Prm1, :Prm2);");
	_LIT(KSqlString2, "SELECT * FROM BBB WHERE fld1 = 10");
	TInt err = aStmt.Prepare(aDb, KSqlString1);
	if(err == KErrNone)
		{
		for(TInt i=0;i<KLongColumnSize;++i)
			{
			TheTextColumnData[i] = (TUint16)i;
			}
		const TPtrC textVal(TheTextColumnData, KLongColumnSize);
		const TPtrC8 binVal((TUint8*)TheTextColumnData, KLongColumnSize * sizeof(TUint16));
		TInt err = aStmt.BindText(0, textVal);
		if(err == KErrNone)
			{
			err = aStmt.BindBinary(1, binVal);
			}
		if(err == KErrNone)
			{
			err = aStmt.Exec();
			}
		}
	if(err >= 0)
		{
		aStmt.Close();
		err = aStmt.Prepare(aDb, KSqlString2);
		if(err == KErrNone)
			{
			err = aStmt.Next();
			}
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::Prepare()" OOM test (16-bit INSERT SQL statement with parameters)
void PrepareInsertPrmStmt16L(RSqlDatabase& aDb, RSqlStatement& aStmt)
	{
	_LIT(KSqlString, "INSERT INTO BBB(fld1, fld2, fld3, fld4) VALUES(:Prm0, :Prm1, :Prm2, :Prm3);");
	TInt err = aStmt.Prepare(aDb, KSqlString);
	User::LeaveIfError(err);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1617
@SYMTestCaseDesc		RSqlStatement::Prepare() OOM test
						Precondition: the database exists, opened, some record(s) inserted.
						The test calls the given as an argument function while simulating OOM failures
						and checks that there are no memory and resource leaks.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement::Prepare() OOM test
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void DoStmtPrepareOomTestL(TStmtFuncPtrL aStmtFuncPtrL, const TDesC& aDbFileName, TDbType aDbType, TInt aExpectedError = KErrNone)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1617 RSqlStatement::Prepare() - OOM test"));
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		RSqlDatabase::Delete(aDbFileName);
		RSqlDatabase db;
		TInt err = aDbType == ESecureDb ? db.Create(aDbFileName, securityPolicy) : db.Create(aDbFileName);
		TEST2(err, KErrNone);
		err = db.Exec(KSqlDbString);
		TEST(err >= 0);
		//The next operation is executed to force the SQL server side session to make one memory allocation
		//for the statement handle in its RDbObjContainer contrainer data member. Otherwise the OOM test will fail,
		//because the server side session will do one additional allocation, which stays alive untill the
		//session gets closed.
		RSqlStatement dummyStmt;
		err = dummyStmt.Prepare(db, _L("SELECT * FROM BBB"));
		TEST2(err, KErrNone);
		dummyStmt.Close();

		err = KErrNoMemory;
		const TInt KMaxAllocation = TheOomTestType[i] == EServerSideTest ? KStmtOomTestAllocLimitServer : KStmtOomTestAllocLimitClient;
		TInt allocationNo = 0;
		TInt failingAllocationNo = 0;//the real exit point of the OOM test. allocationNo is set KMaxAllocation times.
		while(allocationNo < KMaxAllocation)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

			SetHeapFailure(TheOomTestType[i], ++allocationNo);

	    	RSqlStatement stmt;
			TRAP(err, (*aStmtFuncPtrL)(db, stmt));
			stmt.Close();
			if(err != KErrNoMemory)
				{
				TEST2(err, aExpectedError);
				}
			else
				{
				failingAllocationNo = allocationNo;	
				}

			ResetHeapFailure(TheOomTestType[i]);

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			}

		db.Close();
		TEST2(err, aExpectedError);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo);
		}
	//Delete the database file
	RSqlDatabase::Delete(aDbFileName);
	securityPolicy.Close();
	}

//"RSqlStatement::Reset()" OOM test
void ResetStmtL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.Next();
	if(err == KErrNone)
		{
		err = aStmt.Reset();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::Exec()" OOM test
void ExecStmtL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.Exec();
	User::LeaveIfError(err);
	}

//"RSqlStatement::Next()" OOM test
void NextStmtL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.Next();
	if(err == KErrNone)
		{
		err = aStmt.Next();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::ParameterIndex()" OOM test
void StmtParameterIndexL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.ParameterIndex(_L(":Prm2"));
	if(err == 1) //":Prm2" index is 1
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::ParameterName()" OOM test
void StmtParameterNameL(RSqlStatement& aStmt)
	{
	// _LIT(KExcpected, ":Prm2");
	TPtrC paramName;
	TInt err = aStmt.ParameterName(1, paramName);
	User::LeaveIfError(err);
	}

//"RSqlStatement::ParamName()" OOM test
void StmtParamNameL(RSqlStatement& aStmt)
	{
	TPtrC paramName;
	TInt err = aStmt.ParamName(1, paramName);
	User::LeaveIfError(err);
	}

//"RSqlStatement::ColumnIndex()" OOM test
void StmtColumnIndexL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.ColumnIndex(_L("fLd3"));
	if(err == 2) //"fLd3" index is 2
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::ColumnName()" OOM test
void StmtColumnNameL(RSqlStatement& aStmt)
	{
	//_LIT(KExpected, "fLd3");
	TPtrC colName;
	TInt err = aStmt.ColumnName(2, colName);
	User::LeaveIfError(err);
	}


//"RSqlStatement::ColumnType()" OOM test
void StmtColumnTypeL(RSqlStatement& aStmt)
	{
	TSqlColumnType coltype = aStmt.ColumnType(2);
	TEST(coltype == ESqlReal);
	}

//"RSqlStatement::ColumnSize()" OOM test
void StmtColumnSizeL(RSqlStatement& aStmt)
	{
	TInt colsize = aStmt.ColumnSize(2);
	TEST(colsize == sizeof(TReal));
	}

//"RSqlStatement::BindNull()" OOM test
void StmtBindNullL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.BindNull(1);
	//The bindings will be transferred on the server side right before Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::BindInt()" OOM test
void StmtBindIntL(RSqlStatement& aStmt)
	{
	TInt val = 184;
	TInt err = aStmt.BindInt(0, val);
	//The bindings will be transferred on the server side right before Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::BindInt64()" OOM test
void StmtBindInt64L(RSqlStatement& aStmt)
	{
	TInt64 val = MAKE_TINT64(0x00FF00FF, 0x12345678);
	TInt err = aStmt.BindInt64(1, val);
	//The bindings will be transferred on the server side right before Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::BindReal()" OOM test
void StmtBindRealL(RSqlStatement& aStmt)
	{
	TReal val = 25.2423;
	TInt err = aStmt.BindReal(2, val);
	//The bindings will be transferred on the server side right before Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::BindText()" OOM test
void StmtBindTextL(RSqlStatement& aStmt)
	{
	for(TInt i=0;i<KLongParameterSize;++i)
		{
		TheTextColumnData[i] = (TUint16)i;
		}
	const TPtrC val(TheTextColumnData, KLongParameterSize);
	TInt err = aStmt.BindText(3, val);
	//The bindings will be transferred on the server side right before a Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::BindBinary()" OOM test
void StmtBindBinaryL(RSqlStatement& aStmt)
	{
	for(TInt i=0;i<KLongParameterSize;++i)
		{
		TheBinaryColumnData[i] = (TUint8)i;
		}
	const TPtrC8 val(TheBinaryColumnData, KLongParameterSize);
	TInt err = aStmt.BindBinary(3, val);
	//The bindings will be transferred on the server side right before a Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::BindZeroBlob()" OOM test
void StmtBindZeroBlobL(RSqlStatement& aStmt)
	{
	TInt err = aStmt.BindZeroBlob(3, KLongParameterSize);
	//The bindings will be transferred on the server side right before a Next() or Exec() call
	if(err == KErrNone)
		{
		err = aStmt.Exec();
		}
	User::LeaveIfError(err);
	}

//"RSqlStatement::IsNull()" OOM test
void StmtIsNullColumnL(RSqlStatement& aStmt)
	{
	TBool rc = aStmt.IsNull(0);
	TEST(!rc);
	}

//"RSqlStatement::ColumnInt()" OOM test
void StmtColumnIntL(RSqlStatement& aStmt)
	{
	TInt val = aStmt.ColumnInt(0);
	TEST(val == 10);
	}

//"RSqlStatement::ColumnInt64()" OOM test
void StmtColumnInt64L(RSqlStatement& aStmt)
	{
	TInt64 val = aStmt.ColumnInt64(1);
	TEST(val == 100);
	}

//"RSqlStatement::ColumnReal()" OOM test
void StmtColumnRealL(RSqlStatement& aStmt)
	{
	TReal val = aStmt.ColumnReal(2);
	TEST(Abs(val - 100.001) < 0.000001);
	}

//"RSqlStatement::ColumnTextL()" OOM test
void StmtColumnText1L(RSqlStatement& aStmt)
	{
	TPtrC val = aStmt.ColumnTextL(3);
	TEST(val.Length() == KLongColumnSize);
	for(TInt i=0;i<KLongColumnSize;++i)
		{
		TEST(val[i] == (TUint16)i);
		}
	}

//"RSqlStatement::ColumnText()" OOM test
void StmtColumnText2L(RSqlStatement& aStmt)
	{
	TPtrC val;
	TInt err = aStmt.ColumnText(3, val);
	User::LeaveIfError(err);
	TEST(val.Length() == KLongColumnSize);
	for(TInt i=0;i<KLongColumnSize;++i)
		{
		TEST(val[i] == (TUint16)i);
		}
	}

//"RSqlStatement::ColumnText()" OOM test
void StmtColumnText3L(RSqlStatement& aStmt)
	{
	TPtr val(TheTextColumnData, KLongColumnSize);
	TInt err = aStmt.ColumnText(3, val);
	User::LeaveIfError(err);
	TEST(val.Length() == KLongColumnSize);
	for(TInt i=0;i<KLongColumnSize;++i)
		{
		TEST(val[i] == (TUint16)i);
		}
	}

//"RSqlStatement::ColumnBinaryL()" OOM test
void StmtColumnBinary1L(RSqlStatement& aStmt)
	{
	TPtrC8 val = aStmt.ColumnBinaryL(4);
	TEST(val.Length() == KLongColumnSize * sizeof(TUint16));
	for(TInt i=0,j=0;i<KLongColumnSize;++i,j+=2)
		{
		TUint8 valLow = val[j];
		TUint8 valHigh = val[j + 1];
		TEST(valLow == (TUint8)(i & 0xFF));
		TEST(valHigh == (TUint8)(((TUint)i >> 8) & 0xFF));
		}
	}

//"RSqlStatement::ColumnBinary()" OOM test
void StmtColumnBinary2L(RSqlStatement& aStmt)
	{
	TPtrC8 val;
	TInt err = aStmt.ColumnBinary(4, val);
	User::LeaveIfError(err);
	TEST(val.Length() == KLongColumnSize * sizeof(TUint16));
	for(TInt i=0,j=0;i<KLongColumnSize;++i,j+=2)
		{
		TUint8 valLow = val[j];
		TUint8 valHigh = val[j + 1];
		TEST(valLow == (TUint8)(i & 0xFF));
		TEST(valHigh == (TUint8)(((TUint)i >> 8) & 0xFF));
		}
	}

//"RSqlStatement::ColumnBinary()" OOM test
void StmtColumnBinary3L(RSqlStatement& aStmt)
	{
	TPtr8 val(TheBinaryColumnData, KLongColumnSize * sizeof(TUint16));
	TInt err = aStmt.ColumnBinary(4, val);
	User::LeaveIfError(err);
	TEST(val.Length() == KLongColumnSize * sizeof(TUint16));
	for(TInt i=0,j=0;i<KLongColumnSize;++i,j+=2)
		{
		TUint8 valLow = val[j];
		TUint8 valHigh = val[j + 1];
		TEST(valLow == (TUint8)(i & 0xFF));
		TEST(valHigh == (TUint8)(((TUint)i >> 8) & 0xFF));
		}
	}

//"RSqlStatement::ColumnCount()" OOM test
void StmtColumnCount(RSqlStatement& aStmt)
	{
	TInt cnt = aStmt.ColumnCount();
	TEST2(cnt, 5);
	}

//"RSqlStatement::DeclaredColumnType()" OOM test
void StmtDeclaredColumnTypeL(RSqlStatement& aStmt)
	{
	TInt cnt = aStmt.ColumnCount();
	TEST2(cnt, 5);
	const TSqlColumnType KColTypes[] = {ESqlInt, ESqlInt, ESqlReal, ESqlText, ESqlBinary};
	for(TInt i=0;i<cnt;++i)
		{
		TSqlColumnType colType;
		TInt err = aStmt.DeclaredColumnType(i, colType);
		User::LeaveIfError(err);
		TEST2(colType, KColTypes[i]);
		}
	}

//"RSqlColumnReadStream::ColumnBinary()" OOM test
void StmtColumnBinaryStreamL(RSqlStatement& aStmt)
	{
	RSqlColumnReadStream strm;
	CleanupClosePushL(strm);
	TInt err = strm.ColumnBinary(aStmt, 4);
	User::LeaveIfError(err);
	strm.ReadL(TheBinaryColumnData, KLongColumnSize * sizeof(TUint16));
	CleanupStack::PopAndDestroy(&strm);
	for(TInt i=0,j=0;i<KLongColumnSize;++i,j+=2)
		{
		TUint8 valLow = TheBinaryColumnData[j];
		TUint8 valHigh = TheBinaryColumnData[j + 1];
		TEST(valLow == (TUint8)(i & 0xFF));
		TEST(valHigh == (TUint8)(((TUint)i >> 8) & 0xFF));
		}
	}

//"RSqlColumnReadStream::ColumnText()" OOM test
void StmtColumnTextStreamL(RSqlStatement& aStmt)
	{
	RSqlColumnReadStream strm;
	CleanupClosePushL(strm);
	TInt err = strm.ColumnText(aStmt, 3);
	User::LeaveIfError(err);
	strm.ReadL(TheTextColumnData, KLongColumnSize);
	CleanupStack::PopAndDestroy(&strm);
	for(TInt i=0;i<KLongColumnSize;++i)
		{
		TEST(TheTextColumnData[i] == (TUint16)i);
		}
	}

//"RSqlParamWriteStream::BindBinary()" OOM test
void StmtParamBinaryStreamL(RSqlStatement& aStmt)
	{
	for(TInt i=0;i<KLongParameterSize;++i)
		{
		TheTextColumnData[i] = (TUint16)i;
		}

	RSqlParamWriteStream strm;
	CleanupClosePushL(strm);
	TInt err = strm.BindBinary(aStmt, 3);
	User::LeaveIfError(err);
	strm.WriteL((TUint8*)TheTextColumnData, KLongColumnSize * sizeof(TUint16));
	CleanupStack::PopAndDestroy(&strm);
	}

//"RSqlParamWriteStream::BindText()" OOM test
void StmtParamTextStreamL(RSqlStatement& aStmt)
	{
	for(TInt i=0;i<KLongParameterSize;++i)
		{
		TheTextColumnData[i] = (TUint16)i;
		}

	RSqlParamWriteStream strm;
	CleanupClosePushL(strm);
	TInt err = strm.BindText(aStmt, 3);
	User::LeaveIfError(err);
	strm.WriteL(TheTextColumnData, KLongColumnSize);
	CleanupStack::PopAndDestroy(&strm);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1618
@SYMTestCaseDesc		RSqlStatement methods OOM tests
						Precondition: the database exists, opened, some record(s) inserted.
						The test calls the given as an argument function while simulating OOM failures
						and checks that there are no memory and resource leaks.
@SYMTestPriority		High
@SYMTestActions			RSqlStatement methods OOM tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void DoStmtOomTestL(TStmtFuncPtrL aStmtPrepareFuncPtrL, TStmtFuncPtr2L aStmtTestFuncPtrL, const TDesC& aDbFileName, TDbType aDbType)
	{
	//This is not really an OOM test, because:
	//(A) The test forces the server to simulate heap failure on determied allocation step
	//(B) SQL server counts only the number of active statement and stream objects, the allocated memory cells
	//    are not counted.
	//    The reason that the allocated memory cells are not counted and in case of OOM failure - checked is
	//    because the SQL server may make some per-connection persistent memory allocations (cache pages, etc.)
	//    which will not be deallocated when the statement object is closed.

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1618 RSqlStatement - OOM test"));
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		RSqlDatabase::Delete(aDbFileName);
		RSqlDatabase db;
		TInt err = aDbType == ESecureDb ? db.Create(aDbFileName, securityPolicy) : db.Create(aDbFileName);
		TEST2(err, KErrNone);
		err = db.Exec(KSqlDbString);
		TEST(err >= 0);
		err = db.Exec(KSqlDbString2);
		TEST(err >= 0);

		err = KErrNoMemory;
		const TInt KMaxAllocation = TheOomTestType[i] == EServerSideTest ? KStmtOomTestAllocLimitServer : KStmtOomTestAllocLimitClient;
		TInt allocationNo = 0;
		TInt failingAllocationNo = 0;//the real exit point of the OOM test. allocationNo is set KMaxAllocation times.
		while(allocationNo < KMaxAllocation)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

	    	RSqlStatement stmt;
			TRAP(err, (*aStmtPrepareFuncPtrL)(db, stmt));
			TEST2(err, KErrNone);

			SetHeapFailure(TheOomTestType[i], ++allocationNo);

			TRAP(err, (*aStmtTestFuncPtrL)(stmt));

			//ResetHeapFailure() has to be called before stmt.Close() because,
			//otherwise it will panic the server (the server resource count, marked by
			//SetHeapFailure() call, is 1 (there is one prepared statement))
			ResetHeapFailure(TheOomTestType[i]);

			stmt.Close();

			if(err != KErrNoMemory)
				{
				TEST2(err, KErrNone);
				}
			else
				{
				failingAllocationNo = allocationNo;	
				}

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			}

		db.Close();
		TEST2(err, KErrNone);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo);
		}
	//Delete the database file
	RSqlDatabase::Delete(aDbFileName);
	securityPolicy.Close();
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               RSqlBlobReadStream, RSqlBlobWriteStream - OOM tests          /////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//RSqlBlobReadStream, RSqlBlobWriteStream - OOM test preparation.
void ExecInsertBlobL(RSqlDatabase& aDb)
	{
	_LIT8(KSqlString1, "CREATE TABLE BBB(Fld1 INTEGER, Fld2 BIGINT, Fld3 DOUBLE, Fld4 TEXT, Fld5 LONGBLOB)");
	TInt err = aDb.Exec(KSqlString1);
	User::LeaveIfError(err);
	_LIT8(KSqlString2, "INSERT INTO BBB(fld1, fld2, fld3, fld4, fld5) VALUES(10, 100, 100.001, 'AAA', x'");
	TheSqlBuf.Copy(KSqlString2);
	for(TInt i=0;i<KBlobStrLen;++i)	
		{
		TheSqlBuf.Append(_L8("A5"));
		}
	TheSqlBuf.Append(_L8("')"));
	err = aDb.Exec(TheSqlBuf);
	User::LeaveIfError(err);
	}

//"RSqlBlobReadStream::OpenL()/RSqlBlobReadStream::ReadL()" OOM test
void BlobReadStreamOpenL(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	if(aAttachDbName.Length() > 0)
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1, aAttachDbName);
		}
	else
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1);
		}
	const TInt KReadOpCnt = 8;
	for(TInt j=0;j<KReadOpCnt;++j)
		{
		strm.ReadL(TheSqlBuf, KBlobLen / KReadOpCnt);
		TEST2(TheSqlBuf.Length(), (KBlobLen / KReadOpCnt));
		for(TInt i=0;i<(KBlobLen / KReadOpCnt);++i)
			{
			TUint8 val = TheSqlBuf[i];
			TEST2(val, 0xA5);
			}
		}
	CleanupStack::PopAndDestroy(&strm);
	}

//"RSqlBlobReadStream::OpenL()/RSqlBlobReadStream::SizeL()" OOM test
void BlobReadStreamSizeL(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	RSqlBlobReadStream strm;
	CleanupClosePushL(strm);
	if(aAttachDbName.Length() > 0)
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1, aAttachDbName);
		}
	else
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1);
		}
	TInt size = strm.SizeL();	
	TEST2(size, KBlobLen);
	CleanupStack::PopAndDestroy(&strm);
	}

//"RSqlBlobWriteStream::OpenL()/RSqlBlobWriteStream::WriteL()" OOM test
void BlobWriteStreamOpenL(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	if(aAttachDbName.Length() > 0)
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1, aAttachDbName);
		}
	else
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1);
		}
	const TInt KWriteOpCnt = 8;
	TheSqlBuf.SetLength(KBlobLen / KWriteOpCnt);
	TheSqlBuf.Fill(TChar('Z'));
	for(TInt j=0;j<KWriteOpCnt;++j)
		{
		strm.WriteL(TheSqlBuf, KBlobLen / KWriteOpCnt);
		}
	strm.CommitL();		
	CleanupStack::PopAndDestroy(&strm);
	}

//"RSqlBlobWriteStream::OpenL()/RSqlBlobWriteStream::SizeL()" OOM test
void BlobWriteStreamSizeL(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	RSqlBlobWriteStream strm;
	CleanupClosePushL(strm);
	if(aAttachDbName.Length() > 0)
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1, aAttachDbName);
		}
	else
		{
		strm.OpenL(aDb, _L("BBB"), _L("fld5"), 1);
		}
	TInt size = strm.SizeL();	
	TEST2(size, KBlobLen);
	CleanupStack::PopAndDestroy(&strm);
	}
	
//"TSqlBlob::GetLC()" OOM test
void BlobWholeGet1L(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	HBufC8* buf = NULL;
	if(aAttachDbName.Length() > 0)
		{
		buf = TSqlBlob::GetLC(aDb, _L("BBB"), _L("fld5"), 1, aAttachDbName);
		}
	else
		{
		buf = TSqlBlob::GetLC(aDb, _L("BBB"), _L("fld5"), 1);
		}
	TEST(buf->Length() == KBlobStrLen);
	CleanupStack::PopAndDestroy(buf);
	}
	
//"TSqlBlob::Get()" OOM test
void BlobWholeGet2L(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	if(aAttachDbName.Length() > 0)
		{
		TSqlBlob::Get(aDb, _L("BBB"), _L("fld5"), TheSqlBuf2, 1, aAttachDbName);
		}
	else
		{
		TSqlBlob::Get(aDb, _L("BBB"), _L("fld5"), TheSqlBuf2, 1);
		}
	}

//"TSqlBlob::SetL()" OOM test
void BlobWholeSetL(RSqlDatabase& aDb, const TDesC& aAttachDbName)
	{
	if(aAttachDbName.Length() > 0)
		{
		TSqlBlob::SetL(aDb, _L("BBB"), _L("fld5"), TheSqlBuf, 1, aAttachDbName);
		}
	else
		{
		TSqlBlob::Get(aDb, _L("BBB"), _L("fld5"), TheSqlBuf, 1);
		}
	}

/**
@SYMTestCaseID			SYSLIB-SQL-UT-4091
@SYMTestCaseDesc		RSqlBlobReadStream, RSqlBlobWriteStream methods OOM tests
						Precondition: the database exists, opened, some record(s) inserted.
						The test calls the given as an argument function while simulating OOM failures
						and checks that there are no memory and resource leaks.
@SYMTestPriority		High
@SYMTestActions			RSqlBlobReadStream, RSqlBlobWriteStream methods OOM tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ10410
                        REQ10411
                        REQ10418
*/
void DoBlobOomTestL(TBlobPrepareFuncPtrL aBlobPrepareFuncPtrL, TBlobTestFuncPtrL aBlobTestFuncPtrL, 
					const TDesC& aDbFileName, TDbType aDbType, TBool aAttachDb = EFalse)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-UT-4091 RSqlBlobReadStream, RSqlBlobWriteStream - OOM test"));
	RSqlSecurityPolicy securityPolicy;
	CreateTestSecurityPolicy(securityPolicy);
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		RSqlDatabase::Delete(aDbFileName);
		RSqlDatabase db;
		TInt err = aDbType == ESecureDb ? db.Create(aDbFileName, securityPolicy) : db.Create(aDbFileName);
		TEST2(err, KErrNone);
		TRAP(err, (*aBlobPrepareFuncPtrL)(db));
		TEST2(err, KErrNone);
		_LIT(KAttachDbName, "AttachDb");
		if(aAttachDb)
			{
			err = db.Attach(aDbFileName, KAttachDbName);
			TEST2(err, KErrNone);
			}

		err = KErrNoMemory;
		const TInt KMaxAllocation = TheOomTestType[i] == EServerSideTest ? KBlobOomTestAllocLimitServer : KBlobOomTestAllocLimitClient;
		TInt allocationNo = 0;
		TInt failingAllocationNo = 0;//the real exit point of the OOM test. allocationNo is set KMaxAllocation times.
		while(allocationNo < KMaxAllocation)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

			SetHeapFailure(TheOomTestType[i], ++allocationNo);
			if(aAttachDb)
				{
				TRAP(err, (*aBlobTestFuncPtrL)(db, KAttachDbName));
				}
			else
				{
				TRAP(err, (*aBlobTestFuncPtrL)(db, KNullDesC));
				}

			ResetHeapFailure(TheOomTestType[i]);

			if(err != KErrNoMemory)
				{
				if(err != KErrNone)
					{
					TPtrC errmsg = db.LastErrorMessage();
					if(errmsg.Length() > 0)
						{
						TheTest.Printf(_L("\r\n@@@ error %d, error message: %S\r\n"), err, &errmsg);	
						}
					}
				TEST2(err, KErrNone);
				}
			else
				{
				failingAllocationNo = allocationNo;
				}				

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			}
		TEST2(err, KErrNone);

		if(aAttachDb)
			{
			(void)db.Detach(KAttachDbName);
			}

		db.Close();
		err = RSqlDatabase::Delete(aDbFileName);
		TEST2(err, KErrNone);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo);
		}
	securityPolicy.Close();
	}

void ScalarFullSelectInt_8L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TInt val = query.SelectIntL(_L8("SELECT F1 FROM A WHERE F3 < 0"));
	TEST2(val, 2);
	}

void ScalarFullSelectInt64_8L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TInt64 val = query.SelectInt64L(_L8("SELECT F2 FROM A WHERE F1 = 1"));
	TEST2(val, 10000000000LL);
	}

void ScalarFullSelectReal_8L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TReal val = query.SelectRealL(_L8("SELECT F3 FROM A WHERE F1 = 1"));
	TEST(val > 2.0 && val < 3.0);
	}

void ScalarFullSelectText_8L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TBuf<20> buf;
	TInt err = query.SelectTextL(_L8("SELECT F4 FROM A WHERE F1 = 1"), buf);
	TEST2(err, KErrNone);
	_LIT(KText, "NAME1234567890");
	TEST(buf == KText);
	}

void ScalarFullSelectBinary_8L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TBuf8<20> buf;
	TInt err = query.SelectBinaryL(_L8("SELECT F5 FROM A WHERE F1 = 1"), buf);
	TEST2(err, KErrNone);
	TEST(buf.Length() == 0);
	}

void ScalarFullSelectInt_16L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TInt val = query.SelectIntL(_L16("SELECT F1 FROM A WHERE F3 < 0"));
	TEST2(val, 2);
	}

void ScalarFullSelectInt64_16L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TInt64 val = query.SelectInt64L(_L16("SELECT F2 FROM A WHERE F1 = 1"));
	TEST2(val, 10000000000LL);
	}

void ScalarFullSelectReal_16L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TReal val = query.SelectRealL(_L16("SELECT F3 FROM A WHERE F1 = 1"));
	TEST(val > 2.0 && val < 3.0);
	}

void ScalarFullSelectText_16L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TBuf<20> buf;
	TInt err = query.SelectTextL(_L16("SELECT F4 FROM A WHERE F1 = 1"), buf);
	TEST2(err, KErrNone);
	_LIT(KText, "NAME1234567890");
	TEST(buf == KText);
	}

void ScalarFullSelectBinary_16L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	TBuf8<20> buf;
	TInt err = query.SelectBinaryL(_L16("SELECT F5 FROM A WHERE F1 = 1"), buf);
	TEST2(err, KErrNone);
	TEST(buf.Length() == 0);
	}

void ScalarFullSelectText2_8L(RSqlDatabase& aDb)
	{
	TSqlScalarFullSelectQuery query(aDb);
	HBufC* buf = HBufC::NewLC(8);
	TPtr name = buf->Des();
	TInt rc = query.SelectTextL(_L8("SELECT F4 FROM A WHERE F1 = 1"), name);
	TEST(rc >= 0); //the function may return only non-negative values
	if(rc > 0)
		{
		buf = buf->ReAllocL(rc);
		CleanupStack::Pop();
		CleanupStack::PushL(buf);
		name.Set(buf->Des());
		rc = query.SelectTextL(_L8("SELECT F4 FROM A WHERE F1 = 1"), name);
		TEST(rc == 0);
		_LIT(KText, "NAME1234567890");
		TEST(name == KText);
		}
	CleanupStack::PopAndDestroy(buf);
	}

/**
@SYMTestCaseID			SYSLIB-SQL-CT-1811
@SYMTestCaseDesc		TSqlScalarFullSelectQuery functions OOM test
						Precondition: the database exists.
						The test simulates OOM failures while calling TSqlScalarFullSelectQuery functions
						and checks that there are no memory and resource leaks.
@SYMTestPriority		High
@SYMTestActions			TSqlScalarFullSelectQuery methods OOM tests
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ5792
                        REQ5793
*/
void DoFullSelectOomTest(TScalarFullSelectFuncPtrL aTestFunctionPtrL)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SQL-CT-1811 TSqlScalarFullSelectQuery - OOM test"));
	//Create test database
	RSqlDatabase db;
	TInt err = db.Create(KTestDb);
	TEST2(err, KErrNone);
	err = db.Exec(_L("CREATE TABLE A(F1 INTEGER, F2 INTEGER, F3 FLOAT, F4 TEXT, F5 BLOB)"));
	TEST(err >= 0);
	err = db.Exec(_L("INSERT INTO A(F1, F2, F3, F4, F5) VALUES(1, 10000000000, 2.54, 'NAME1234567890', NULL)"));
	TEST2(err, 1);
	err = db.Exec(_L("INSERT INTO A(F1, F2, F3, F4) VALUES(2, 200, -1.11, 'ADDRESS')"));
	TEST2(err, 1);
	db.Close();
	//OOM test loop
	for(TInt i=0;i<(TInt)(sizeof(TheOomTestType)/sizeof(TheOomTestType[0]));++i)
		{
		err = KErrNoMemory;
		const TInt KMaxAllocation = TheOomTestType[i] == EServerSideTest ? KStmtOomTestAllocLimitServer : KStmtOomTestAllocLimitClient;
		TInt allocationNo = 0;
		TInt failingAllocationNo = 0;//the real exit point of the OOM test. allocationNo is set KMaxAllocation times.
		while(allocationNo < KMaxAllocation)
			{
			MarkHandles();
			MarkAllocatedCells();

			__UHEAP_MARK;

			if(TheOomTestType[i] == EServerSideTest)
				{//We will delay the heap failure simulation, until the database is opened
				SetDbHeapFailure(TheOomTestType[i], ++allocationNo, ETrue);
				}

			err = db.Open(KTestDb);
			TEST2(err, KErrNone);

			if(TheOomTestType[i] == EClientSideTest)
				{
				SetDbHeapFailure(TheOomTestType[i], ++allocationNo);
				}

			TRAP(err, (*aTestFunctionPtrL)(db));
			db.Close();
			if(err != KErrNoMemory)
				{
				TEST2(err, KErrNone);
				}
			else
				{
				failingAllocationNo = allocationNo;	
				}

			ResetDbHeapFailure(TheOomTestType[i]);

			__UHEAP_MARKEND;

			CheckAllocatedCells();
			CheckHandles();
			}
		TEST2(err, KErrNone);
		PrintEndOfOomTest(TheOomTestType[i], failingAllocationNo);
		}
	//Delete the database file
	RSqlDatabase::Delete(KTestDb);
	}

//RSqlStatement OOM tests
void StmtOomTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===RSqlStatement::Prepare(), 8-bit SQL\r\n"));
		DoStmtPrepareOomTestL(&PrepareStmt8L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::PrepareL(), 8-bit SQL\r\n"));
		DoStmtPrepareOomTestL(&PrepareStmt8_2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Prepare(), 16-bit SQL\r\n"));
		DoStmtPrepareOomTestL(&PrepareStmt16L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::PrepareL(), 16-bit SQL\r\n"));
		DoStmtPrepareOomTestL(&PrepareStmt16_2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Reset(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareStmt8L, &ResetStmtL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Reset(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareStmt16L, &ResetStmtL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Exec(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertStmt8L, &ExecStmtL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Exec(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertStmt16L, &ExecStmtL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Next(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareStmt8L, &NextStmtL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::Next(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareStmt16L, &NextStmtL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ParameterIndex(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtParameterIndexL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ParameterIndex(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtParameterIndexL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ParameterName(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtParameterNameL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ParameterName(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtParameterNameL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ParamName(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtParamNameL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ParamName(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtParamNameL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnIndex(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareStmt8L, &StmtColumnIndexL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnIndex(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareStmt16L, &StmtColumnIndexL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnType(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareMoveStmt8L, &StmtColumnTypeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnType(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareMoveStmt16L, &StmtColumnTypeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnSize(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareMoveStmt8L, &StmtColumnSizeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnSize(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareMoveStmt16L, &StmtColumnSizeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnName(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareMoveStmt8L, &StmtColumnNameL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnName(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareMoveStmt16L, &StmtColumnNameL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindNull(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindNullL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindNull(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindNullL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindInt(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindIntL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindInt(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindIntL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindInt64(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindInt64L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindInt64(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindInt64L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindReal(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindRealL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindReal(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindRealL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindText(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindTextL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindText(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindTextL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindBinary(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindBinaryL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindBinary(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindBinaryL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindZeroBlob(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtBindZeroBlobL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::BindZeroBlob(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtBindZeroBlobL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::IsNull(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtIsNullColumnL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::IsNull(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtIsNullColumnL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnInt(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnIntL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnInt(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnIntL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnInt64(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnInt64L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnInt64(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnInt64L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnReal(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnRealL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnReal(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnRealL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnTextL(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnText1L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnTextL(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnText1L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnText()-1, 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnText2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnText()-1, 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnText2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnText()-2, 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnText3L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnText()-2, 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnText3L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnBinaryL(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnBinary1L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnBinaryL(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnBinary1L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnBinary()-1, 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnBinary2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnBinary()-1, 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnBinary2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnBinary()-2, 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnBinary3L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnBinary()-2, 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnBinary3L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnCount(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnCount, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::ColumnCount(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnCount, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::DeclaredColumnType(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtDeclaredColumnTypeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlStatement::DeclaredColumnType(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtDeclaredColumnTypeL, dbFileName[i], dbType[i]);
		}
	}

//RSqlStatement - negative OOM tests
void StmtOomNegativeTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===RSqlStatement::Prepare(), 8-bit SQL\r\n"));
		DoStmtPrepareOomTestL(&PrepareBadStmt8L, dbFileName[i], dbType[i], KSqlErrGeneral);

		TheTest.Printf(_L("===RSqlStatement::Prepare(), 16-bit SQL\r\n"));
		DoStmtPrepareOomTestL(&PrepareBadStmt16L, dbFileName[i], dbType[i], KSqlErrGeneral);
		}
	}

//RSqlColumnReadStream OOM tests
void ColumnReadStreamOomTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===RSqlColumnReadStream::ColumnBinary(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnBinaryStreamL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlColumnReadStream::ColumnBinary(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnBinaryStreamL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlColumnReadStream::ColumnText(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt8L, &StmtColumnTextStreamL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlColumnReadStream::ColumnText(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&ExecInsertNextStmt16L, &StmtColumnTextStreamL, dbFileName[i], dbType[i]);
		}
	}

//RSqlParamWriteStream OOM tests
void ParamWriteStreamOomTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===RSqlParamWriteStream::BindBinary(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtParamBinaryStreamL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlParamWriteStream::BindBinary(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtParamBinaryStreamL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlParamWriteStream::BindText(), 8-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt8L, &StmtParamTextStreamL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlParamWriteStream::BindText(), 16-bit SQL\r\n"));
		DoStmtOomTestL(&PrepareInsertPrmStmt16L, &StmtParamTextStreamL, dbFileName[i], dbType[i]);
		}
	}

//TSqlScalarFullSelectQuery OOM tests
void ScalarFullSelectQueryOomTestsL()
	{
	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectIntL(), 8-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectInt_8L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectInt64L(), 8-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectInt64_8L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectRealL(), 8-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectReal_8L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectTextL(), 8-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectText_8L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectBinaryL(), 8-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectBinary_8L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectIntL(), 16-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectInt_16L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectInt64L(), 16-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectInt64_16L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectRealL(), 16-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectReal_16L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectTextL(), 16-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectText_16L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectBinaryL(), 16-bit SQL\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectBinary_16L);

	TheTest.Printf(_L("===TSqlScalarFullSelectQuery::SelectTextL(), 8-bit SQL, small buffer\r\n"));
	DoFullSelectOomTest(&ScalarFullSelectText2_8L);
	}

void BlobReadStreamOomTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===RSqlBlobReadStream::OpenL()/RSqlBlobReadStream::ReadL()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobReadStreamOpenL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlBlobReadStream::OpenL()/RSqlBlobReadStream::ReadL() + attached database\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobReadStreamOpenL, dbFileName[i], dbType[i], ETrue);

		TheTest.Printf(_L("===RSqlBlobReadStream::OpenL()/RSqlBlobReadStream::SizeL()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobReadStreamSizeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlBlobReadStream::OpenL()/RSqlBlobReadStream::SizeL() + attached database\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobReadStreamSizeL, dbFileName[i], dbType[i], ETrue);
		}
	}

void BlobWriteStreamOomTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===RSqlBlobWriteStream::OpenL()/RSqlBlobWriteStream::WriteL()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWriteStreamOpenL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlBlobWriteStream::OpenL()/RSqlBlobWriteStream::WriteL() + attached database\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWriteStreamOpenL, dbFileName[i], dbType[i], ETrue);

		TheTest.Printf(_L("===RSqlBlobWriteStream::OpenL()/RSqlBlobWriteStream::SizeL()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWriteStreamSizeL, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===RSqlBlobWriteStream::OpenL()/RSqlBlobWriteStream::SizeL() + attached database\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWriteStreamSizeL, dbFileName[i], dbType[i], ETrue);
		}
	}
	
void BlobWholeOomTestsL()
	{
	const TInt KTestCnt = 2;
	const TPtrC dbFileName[KTestCnt] = {KTestDb(), 	KSecureTestDb()};
	TDbType dbType[KTestCnt] = {ENonSecureDb, ESecureDb};

	for(TInt i=0;i<KTestCnt;++i)
		{
		TheTest.Printf(_L("===TSqlBlob::GetLC()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWholeGet1L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===TSqlBlob::Get()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWholeGet2L, dbFileName[i], dbType[i]);

		TheTest.Printf(_L("===TSqlBlob::SetL()\r\n"));
		DoBlobOomTestL(&ExecInsertBlobL, &BlobWholeSetL, dbFileName[i], dbType[i]);
		}
	}

void DoTestsL()
	{
	TheTest.Start(_L("SQL OOM tests - 2"));
	
	StmtOomTestsL();

	StmtOomNegativeTestsL();

	ColumnReadStreamOomTestsL();

	ParamWriteStreamOomTestsL();

	ScalarFullSelectQueryOomTestsL();

	BlobReadStreamOomTestsL();

	BlobWriteStreamOomTestsL();
	
	BlobWholeOomTestsL();
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateTestDir();
	DeleteTestFiles();

	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
