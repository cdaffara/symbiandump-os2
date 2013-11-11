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
#ifndef T_DBSTRESS_H
#define T_DBSTRESS_H

#include <d32dbms.h>
#include <s32file.h>
#include <e32test.h>
#include <e32math.h>

enum TAccount {ECash=0,EJohn,ESam,EBen,EPenny};
const TInt KAccountIDs=EPenny-ECash+1;

GLREF_C TInt StartThread(RThread& aThread,TRequestStatus& aStat);
GLREF_C TInt Random(TInt aRange);

GLREF_D TPtrC KTestDatabase,KTestDir,KLogFile;
GLREF_D RTest TheTest;
GLREF_D TInt NewCount;
GLREF_D TInt OldCount;
GLREF_D TInt TransId;

class Timer
	{
public:
	void Start();
	TInt64 Stop();
	void Print();
private:
	TTime iTime;
	};

GLREF_D Timer RunTimer;

#undef  TEST_STRING
#define TEST_STRING(s) _S(s)

void Check1(TInt aValue, const TText* aFile, TInt aLine);
void Check2(TInt aValue, TInt aExpected, const TText* aFile, TInt aLine);

#define TEST(arg) ::Check1((arg), TEST_STRING(__FILE__), __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, TEST_STRING(__FILE__), __LINE__)

#endif// T_DBSTRESS_H
