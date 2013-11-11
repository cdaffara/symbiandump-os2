// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <e32test.h>
#include <e32math.h>
#include <sqlite3.h>
#include "t_sqlitewsd.h"

///////////////////////////////////////////////////////////////////////////////////////
static RTest	TheTest(_L("t_sqlitewsd2 test"));

const char* KTestDb = "c:\\test\\t_sqlitewsd.db";

sqlite3* TheDb = 0;

_LIT(KPanicCategory, "t_sqlitewsd2");

const TInt KExprEvalFailed1 = -1001;
const TInt KExprEvalFailed2 = -1002;

///////////////////////////////////////////////////////////////////////////////////////

static void DestroyTestEnv()
	{
	if(TheDb)
		{
		(void)sqlite3_close(TheDb);
		TheDb = 0;
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

static void Panic(TInt aLine, TInt aPanicCode)
	{
	TBuf<64> buf;
	_LIT(KFormat,"*** Panic %d, at line t_sqlitewsd2.cpp(%d)\r\n");
	buf.Format(KFormat, aPanicCode, aLine);
	RDebug::Print(buf);
	User::Panic(KPanicCategory, aPanicCode);
	}

//Test macros and functions
void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DestroyTestEnv();
		RDebug::Print(_L("*** Expression evaluates to false.\r\n"));
		Panic(aLine, KExprEvalFailed1);
		}
	}

void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d.\r\n"), aExpected, aValue);
		const char* errMsg = sqlite3_errmsg(TheDb);
		if(errMsg)
			{
			TBuf<200> msgBuf;
			msgBuf.Copy(TPtrC8((const TUint8*)errMsg));
			RDebug::Print(_L("*** SQLITE error msg: \"%S\".\r\n"), &msgBuf);
			}
		DestroyTestEnv();
		Panic(aLine, KExprEvalFailed2);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

static void CreateTestEnv()
    {
	TInt err = sqlite3_open(KTestDb, &TheDb);
	TEST2(err, SQLITE_OK);
	TEST(TheDb != 0);
	}

///////////////////////////////////////////////////////////////////////////////////////

TInt E32Main()
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	if(!tc)
		{
		User::Panic(KPanicCategory, KErrNoMemory) ;
		}

	__UHEAP_MARK;

	TheTest.Title();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-SQL-LEGACY-T_SQLITEWSD2-0001 "));
	CreateTestEnv();
	DoInserts(KWsdProc2Id, KWsdProc2RecId1, KWsdProc2RecId2);
	DestroyTestEnv();
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
