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

#ifndef __T_SQLITEWSD__H__
#define __T_SQLITEWSD__H__

const TInt KTestRecordCnt = 500;

const TInt KWsdProc1Id = 1;
const TInt KWsdProc1RecId1 = 1;
const TInt KWsdProc1RecId2 = 2;

const TInt KWsdProc2Id = 2;
const TInt KWsdProc2RecId1 = 11;
const TInt KWsdProc2RecId2 = 22;

extern sqlite3* TheDb;

extern void DoInserts(TInt aProcId, TInt aRecId1, TInt aRecId2);

extern void Check(TInt aValue, TInt aLine);
extern void Check(TInt aValue, TInt aExpected, TInt aLine);

#define TEST(arg) Check((arg), __LINE__)
#define TEST2(aValue, aExpected) Check(aValue, aExpected, __LINE__)

#endif//__T_SQLITEWSD__H__
