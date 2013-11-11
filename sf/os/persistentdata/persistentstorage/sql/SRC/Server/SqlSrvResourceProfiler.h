// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVRESOURCEPROFILER_H__
#define __SQLSRVRESOURCEPROFILER_H__

#include <e32cmn.h>
#include "SqlUtil.h"

#ifdef _SQLPROFILER

///////////////////////////////// Heap max alloc /////////////////////////////////////

extern TBool TheSqlSrvProfilerMaxAllocEnabled;
extern TInt  TheSqlSrvProfilerMaxAllocSize;

#define SQLPROFILER_REPORT_ALLOC(size) 													\
	do																					\
		{																				\
		if(TheSqlSrvProfilerMaxAllocEnabled && (size) > TheSqlSrvProfilerMaxAllocSize) 	\
			{																			\
			TheSqlSrvProfilerMaxAllocSize = (size);										\
			}																			\
		}																				\
	while(0)

///////////////////////////////// IPC counters ///////////////////////////////////////

enum TSqlIpcType
		{
		ESqlIpcRq,
		ESqlIpcRead,
		ESqlIpcWrite,
		//	
		ESqlIpcLast
		};

extern TBool TheSqlSrvProfilerIpcEnabled;
extern TInt  TheSqlSrvProfilerIpc[];
extern TInt64 TheSqlSrvProfilerIpcBytes[];

#define SQLPROFILER_REPORT_IPC(type, bytes) 				\
		do													\
			{												\
			if(TheSqlSrvProfilerIpcEnabled)					\
				{											\
				++TheSqlSrvProfilerIpc[type];				\
				TheSqlSrvProfilerIpcBytes[type] += bytes;	\
				}											\
			}												\
		while(0)

////////////////////////// IPC & SQL tracing related /////////////////////////////////

//Level 1 IPC calls count. IPC calls types included:
// ESqlSrvDbExec8, ESqlSrvDbExec16, ESqlSrvDbScalarFullSelect16, ESqlSrvStmtExec,
// ESqlSrvStmtAsyncExec, ESqlSrvStmtBindExec, ESqlSrvStmtAsyncBindExec, ESqlSrvStmtNext,
// ESqlSrvStmtBindNext;
const TInt KIpcTraceTypeCount = 10; 

//A TSqlIpcTraceData array of KIpcTraceTypeCount elements is a member of the CSqlSrvSession class.
//During the tracing, if level 1 or level 2 is enabled, all relevant information is summarized there.
struct TSqlSrvIpcTraceData
    {
    TInt64  iExecutionTime;     //The summary of the execution time spent in particular level 1 IPC call
    TInt    iCount;             //The IPC call count for particular level 1 call               
    };

///////////////////  IPC & SQL tracing functions and macros ///////////////////////////////

void   SqlIpcStart(TUint& aIpcCounter, TUint32& aStartTicks, TUint aDbHandle);
void   SqlIpcEnd(TUint aIpcCounter, TUint32 aStartTicks, TSqlSrvFunction aFuncCode, 
                 TUint aDbHandle, TSqlSrvIpcTraceData aIpcTraceData[], TInt aRetCode);
void   SqlIpcError(TUint aIpcCounter, TSqlSrvFunction aFuncCode, TUint aDbHandle, TInt aError);
void   SqlPrintSql16(TUint aDbHandle, const TDesC& aSql, TBool aPrepare);
void   SqlPrintSql8(TUint aDbHandle, const TDesC8& aSql, TBool aPrepare);

void   SqlPrintDbCreate(TUint aDbHandle, const TDesC& aDbName);
void   SqlPrintDbOpen(TUint aDbHandle, const TDesC& aDbName);
void   SqlPrintDbClose(TUint aDbHandle);

void   SqlPrintServerStart();
void   SqlPrintServerStop();

#define SQLPROFILER_IPC_START(aIpcCounter, aDbHandle) \
    TUint32 startTicks = 0;\
    SqlIpcStart(aIpcCounter, startTicks, aDbHandle)

#define SQLPROFILER_IPC_END(aIpcCounter, aFuncCode, aDbHandle, aIpcTraceData, aRetCode) \
    SqlIpcEnd(aIpcCounter, startTicks, aFuncCode, aDbHandle, aIpcTraceData, aRetCode)

#define SQLPROFILER_IPC_ERROR(aIpcCounter, aFuncCode, aDbHandle, aError) \
    SqlIpcError(aIpcCounter, aFuncCode, aDbHandle, aError)

#define SQLPROFILER_SQL16_PRINT(aDbHandle, aSql, aPrepare) \
    SqlPrintSql16(aDbHandle, aSql, aPrepare)

#define SQLPROFILER_SQL8_PRINT(aDbHandle, aSql, aPrepare) \
    SqlPrintSql8(aDbHandle, aSql, aPrepare)

#define SQLPROFILER_DB_CREATE(aDbHandle, aDbName) \
    SqlPrintDbCreate(aDbHandle, aDbName)

#define SQLPROFILER_DB_OPEN(aDbHandle, aDbName) \
    SqlPrintDbOpen(aDbHandle, aDbName)

#define SQLPROFILER_DB_CLOSE(aDbHandle) \
    SqlPrintDbClose(aDbHandle)

#define SQLPROFILER_SERVER_START() \
    SqlPrintServerStart()

#define SQLPROFILER_SERVER_STOP() \
    SqlPrintServerStop()

///////////////////////////////////////////////////////////////////////////////////////

#else//_SQLPROFILER

#define SQLPROFILER_REPORT_ALLOC(size) do {} while(0)
#define SQLPROFILER_REPORT_IPC(type, bytes) do {} while(0)

#define SQLPROFILER_IPC_START(aIpcCounter, aDbHandle) do {} while(0)
#define SQLPROFILER_IPC_END(aIpcCounter, aFuncCode, aDbHandle, aIpcTraceData, aRetCode) do {} while(0)
#define SQLPROFILER_IPC_ERROR(aIpcCounter, aFuncCode, aDbHandle, aError) do {} while(0)
#define SQLPROFILER_SQL16_PRINT(aDbHandle, aSql, aPrepare) do {} while(0)
#define SQLPROFILER_SQL8_PRINT(aDbHandle, aSql, aPrepare) do {} while(0)

#define SQLPROFILER_DB_CREATE(aDbHandle, aDbName) do {} while(0)
#define SQLPROFILER_DB_OPEN(aDbHandle, aDbName) do {} while(0)
#define SQLPROFILER_DB_CLOSE(aDbHandle) do {} while(0)

#define SQLPROFILER_SERVER_START() do {} while(0) 
#define SQLPROFILER_SERVER_STOP() do {} while(0)

#endif//_SQLPROFILER

/**
This class can be used for collecting information regarding the SQL server resources usage.

@see TSqlResourceProfiler
@internalComponent
*/
NONSHARABLE_CLASS(TSqlSrvResourceProfiler)
	{
public:
	static void StartL(const RMessage2&);
	static void StopL(const RMessage2&);
	static void ResetL(const RMessage2&);
	static void QueryL(const RMessage2&);
	
	};
	
#endif//__SQLSRVRESOURCEPROFILER_H__
