// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32uid.h>
#include <f32file.h>
#include <e32math.h>
#include <sqlite3.h>
#include "t_sqlitewsd.h"

void DoInserts(TInt aProcId, TInt aRecId1, TInt aRecId2)
	{
	TEST(TheDb != 0);
	
	TTime now;
	now.UniversalTime();
	TInt64 seed = now.Int64();
	
	const TInt KMaxFailingAllocationNo = 20;
	TInt lockcnt = 0;
	
	for(TInt recno=0;recno<KTestRecordCnt;)
		{
		//Insert record 1 under OOM simulation
		TInt failingAllocationNo = Math::Rand(seed) % (KMaxFailingAllocationNo + 1);
		__UHEAP_SETFAIL(RHeap::EDeterministic, failingAllocationNo );
		TBuf8<100> sql;
		sql.Format(_L8("INSERT INTO A VALUES(%d)"), aRecId1);
		TInt err = sqlite3_exec(TheDb, (const char*)sql.PtrZ(), 0, 0, 0);
		__UHEAP_SETFAIL(RHeap::ENone, 0);	
		TEST(err == SQLITE_NOMEM || err == SQLITE_BUSY || err == SQLITE_OK);
		if(err == SQLITE_BUSY)
			{
			++lockcnt;
			User::After(1);
			continue;	
			}
		else if(err == SQLITE_OK)
			{
			++recno;
			if((recno % 100) == 0)
				{
				RDebug::Print(_L("Process %d: %d records inserted.\r\n"), aProcId, recno);	
				}
			continue;	
			}
		//Insert record 2
		sql.Format(_L8("INSERT INTO A VALUES(%d)"), aRecId2);
		err = sqlite3_exec(TheDb, (const char*)sql.PtrZ(), 0, 0, 0);
		TEST(err == SQLITE_BUSY || err == SQLITE_OK);
		if(err == SQLITE_BUSY)
			{
			++lockcnt;
			User::After(1);
			continue;	
			}
		//SQLITE_OK case
		++recno;
		if((recno % 100) == 0)
			{
			RDebug::Print(_L("Process %d: %d records inserted.\r\n"), aProcId, recno);	
			}
		}
	RDebug::Print(_L("Process %d inserted %d records. %d locks occured.\r\n"), aProcId, KTestRecordCnt, lockcnt);
	}

