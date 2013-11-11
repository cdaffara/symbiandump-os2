// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Global sql statement related functions
// 
//

#ifndef __SQLSRVSTATEMENTUTIL_H__
#define __SQLSRVSTATEMENTUTIL_H__

#include <e32std.h>

//This header file contains useful utility function declarations.
//These functions are used for accessging directly the SQLITE interface functions.

//Forward declarations
struct sqlite3;
struct sqlite3_stmt;
struct sqlite3_context;
struct Mem;
typedef struct Mem sqlite3_value;

TInt CreateDbHandle16(const TDesC& aFileNameZ, sqlite3*& aDbHandle);
TInt CreateDbHandle8(const TDesC8& aFileNameZ, sqlite3*& aDbHandle);
inline void CloseDbHandle(sqlite3* aDbHandle);
inline void CloseDbCleanup(void* aDbHandle);
TPtrC GetFirstSqlStmt(TPtr& aString);
TInt DbExecStmt16(sqlite3 *aDbHandle, TDes16& aSqlStmt);
TInt DbExecStmt8(sqlite3 *aDbHandle, const TDesC8& aSqlStmt);

sqlite3_stmt* StmtPrepare16L(sqlite3* aDbHandle, const TDesC16& aSqlStmt);
TInt StmtPrepare8(sqlite3* aDbHandle, const TDesC8& aSqlStmt, sqlite3_stmt*& aStmtHandle);
sqlite3_stmt* StmtPrepare8L(sqlite3* aDbHandle, const TDesC8& aSqlStmt);
TInt FinalizeStmtHandle(sqlite3_stmt* aStmtHandle);
TInt StmtExec(sqlite3_stmt* aStmtHandle);
TInt StmtNext(sqlite3_stmt* aStmtHandle);
TInt StmtReset(sqlite3_stmt* aStmtHandle);

TInt DbPageCount(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aPageCount);
TInt DbPageSize(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aPageSize);
TInt DbCacheSize(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aCacheSize);
TInt DbEncoding(sqlite3* aDbHandle, const TDesC& aDbName, TDes8& aEncoding);
TInt DbFreePageCount(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aPageCount);
TInt DbVacuumMode(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aVacuumMode);
TInt DbCompact(sqlite3* aDbHandle, const TDesC& aDbName, TInt aPageCount, TInt& aProcessedPageCount, TInt aMaxTime = 0);

#include "SqlSrvStatementUtil.inl"

#endif//__SQLSRVSTATEMENTUTIL_H__
