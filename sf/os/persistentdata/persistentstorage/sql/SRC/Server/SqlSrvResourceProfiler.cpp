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

#include <f32file.h>
#include "SqlAssert.h"
#include "SqlSrvResourceProfiler.h"
#include "SqlResourceProfiler.h"
#include "SqliteSymbian.h"
#include <hal.h>

#ifdef _SQLPROFILER

///////////////////////////////// Heap max alloc /////////////////////////////////////

/**
If true the max alloc profiling is enabled.
@internalComponent
*/
TBool TheSqlSrvProfilerMaxAllocEnabled = EFalse;
/**
The size of the biggest memory block ever allocated by the SQL server.
Set only if compiled with _SQLPROFILER macro.
@internalComponent
*/
TInt  TheSqlSrvProfilerMaxAllocSize = 0;

////////////////////////// IPC & SQL tracing related //////////////////////////////////

/**
If true the tracing is enabled (IPC calls & SQL statements).
@internalComponent
*/
static TBool TheSqlSrvProfilerTraceEnabled = EFalse;
/**
Trace level:
 - 0: no IPC calls traced (default);
 - 1: only the 10 most important IPC calls traced - SQL statement execution, ....
 - 2: all IPC calls traced; 
@internalComponent
*/
static TInt TheSqlSrvProfilerTraceLevel = 0;
/**
If true the SQL statement tracing is enabled.
@internalComponent
*/
static TBool TheSqlSrvProfilerSqlTraceEnabled = EFalse;

/**
When KSqlSrvProfilerDbName is with non-zero length, then only traces coming from database identified by
KSqlSrvProfilerDbName name are printed out. 
@internalComponent
*/
//_LIT(KSqlSrvProfilerDbName, "default_avacon.dbSQL");
_LIT(KSqlSrvProfilerDbName, "");
static TUint TheSqlSrvProfilerHandle = 0;

//File "read", "write", "sync" and "set size" counters, incremented inside the OS porting layer.
TInt TheSqlSrvProfilerFileRead = 0;
TInt TheSqlSrvProfilerFileWrite = 0;
TInt TheSqlSrvProfilerFileSync = 0;
TInt TheSqlSrvProfilerFileSetSize = 0;

//Set it to true if you want traces to be stored into a file.
static TBool TheSqlSrvProfilerTraceToFile = EFalse;

//"Prepared" and "Executed" SQL statement counters
static TInt TheSqlSrvProfilerPreparedCnt8 = 0;
static TInt TheSqlSrvProfilerPreparedCnt16 = 0;
static TInt TheSqlSrvProfilerExecutedCnt8 = 0;
static TInt TheSqlSrvProfilerExecutedCnt16 = 0;

///////////////////////////////// IPC counters ///////////////////////////////////////

/**
If true the IPC profiling is enabled.
@internalComponent
*/
TBool TheSqlSrvProfilerIpcEnabled = EFalse;
/**
IPC requests, read and write counters.
@internalComponent
*/
TInt  TheSqlSrvProfilerIpc[ESqlIpcLast] = {0};
/**
IPC read and write - bytes.
@internalComponent
*/
TInt64 TheSqlSrvProfilerIpcBytes[ESqlIpcLast] = {0};

//////////////////////////////////////////////////////////////////////////////////////

/**
Starts the specified profiling counter.

@leave KErrNotSupported, The requested profiling type is not supported;
                 		 The function may also leave with some other system-wide error codes.

Usage of the IPC call arguments:
 - Arg 0: [in]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
 - Arg 1: [in]  the length of the additional profiling parameters.
 - Arg 2: [in]  additional profiling parameters.
*/
void TSqlSrvResourceProfiler::StartL(const RMessage2& aMessage)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aMessage.Int0());
	TInt err = KErrNone;
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
		case TSqlResourceProfiler::ESqlCounterOsCall:
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			err = sqlite3SymbianProfilerStart(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterIpc:
			TheSqlSrvProfilerIpcEnabled = ETrue;
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			err = sqlite3SymbianProfilerStart(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			TheSqlSrvProfilerMaxAllocEnabled = ETrue;
			err = sqlite3SymbianProfilerStart(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterTrace:
		    {
			TheSqlSrvProfilerTraceEnabled = ETrue;
		    TInt len = aMessage.Int1();
		    __SQLPANIC_CLIENT2((TUint)len < 64, aMessage, ESqlPanicBadArgument);
		    if(len > 0)
		        {
                TBuf8<64> prmBuf;
                aMessage.ReadL(2, prmBuf);
                prmBuf.UpperCase();
                TPtrC8 ptr(prmBuf);
                _LIT8(KLevel0, "L0");
                _LIT8(KLevel1, "L1");
                _LIT8(KLevel2, "L2");
                _LIT8(KSqlStmtTraceOff, "S0");
                _LIT8(KSqlStmtTraceOn, "S1");
                while(ptr.Length() > 0)
                    {
                    TInt pos = ptr.Locate(TChar(';'));
                    TPtrC8 str = ptr;
                    if(pos >= 0)
                        {
                        str.Set(ptr.Left(pos));
                        }
                    if(str == KLevel0)
                        {
                        TheSqlSrvProfilerTraceLevel = 0;
                        }
                    else if(str == KLevel1)
                        {
                        TheSqlSrvProfilerTraceLevel = 1;
                        }
                    else if(str == KLevel2)
                        {
                        TheSqlSrvProfilerTraceLevel = 2;
                        }
                    else if(str == KSqlStmtTraceOff)
                        {
                        TheSqlSrvProfilerSqlTraceEnabled = EFalse;
                        }
                    else if(str == KSqlStmtTraceOn)
                        {
                        TheSqlSrvProfilerSqlTraceEnabled = ETrue;
                        }
                    if((TUint)pos > (ptr.Length() - 1))
                        {
                        break;
                        }
                    ptr.Set(ptr.Mid(pos + 1));
                    }
		        }
		    }
			break;
		default:
			err = KErrNotSupported;
			break;
		}
	__SQLLEAVE_IF_ERROR2(err);
	}
	
/**
Stops the specified profiling counter.

@leave KErrNotSupported, The requested profiling type is not supported;
                 		 The function may also leave with some other system-wide error codes.

Usage of the IPC call arguments:
 - Arg 0: [in]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
*/
void TSqlSrvResourceProfiler::StopL(const RMessage2& aMessage)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aMessage.Int0());
	TInt err = KErrNone;
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
		case TSqlResourceProfiler::ESqlCounterOsCall:
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			err = sqlite3SymbianProfilerStop(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterIpc:
			TheSqlSrvProfilerIpcEnabled = EFalse;
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			err = sqlite3SymbianProfilerStop(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			TheSqlSrvProfilerMaxAllocEnabled = EFalse;
			err = sqlite3SymbianProfilerStop(KCounterType);
			break;
        case TSqlResourceProfiler::ESqlCounterTrace:
            TheSqlSrvProfilerTraceEnabled = EFalse;
            TheSqlSrvProfilerSqlTraceEnabled = EFalse;
            TheSqlSrvProfilerTraceLevel = 0;
            TheSqlSrvProfilerHandle = 0;
            break;
		default:
			err = KErrNotSupported;
			break;
		}
	__SQLLEAVE_IF_ERROR2(err);
	}
	
/**
Resets the specified profiling counter.

@leave KErrNotSupported, The requested profiling type is not supported;
                 		 The function may also leave with some other system-wide error codes.

Usage of the IPC call arguments:
 - Arg 0: [in]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
*/
void TSqlSrvResourceProfiler::ResetL(const RMessage2& aMessage)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aMessage.Int0());
	TInt err = KErrNone;
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
		case TSqlResourceProfiler::ESqlCounterOsCall:
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			err = sqlite3SymbianProfilerReset(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterIpc:
			Mem::FillZ(TheSqlSrvProfilerIpc, sizeof(TheSqlSrvProfilerIpc));
			Mem::FillZ(TheSqlSrvProfilerIpcBytes, sizeof(TheSqlSrvProfilerIpcBytes));
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			err = sqlite3SymbianProfilerReset(KCounterType);
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			TheSqlSrvProfilerMaxAllocSize = 0;
			err = sqlite3SymbianProfilerReset(KCounterType);
			break;
        case TSqlResourceProfiler::ESqlCounterTrace:
            break;
		default:
			err = KErrNotSupported;
			break;
		}
	__SQLLEAVE_IF_ERROR2(err);
	}
	
/**
Retrieves the counter values for the specified profiling counter.

@leave KErrNotSupported, The requested profiling type is not supported;
                 		 The function may also leave with some other system-wide error codes.

@see TSqlResourceProfiler

Usage of the IPC call arguments:
 - Arg 0: [in]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
 - Arg 1: [in]  the size of the buffer for the profiling counter values.
 - Arg 2: [out] the buffer for the profiling counter values.
*/
void TSqlSrvResourceProfiler::QueryL(const RMessage2& aMessage)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aMessage.Int0());
	const TInt KIpcBufLen = 300;
	TBuf8<KIpcBufLen> ipcBuf;
	TInt err = KErrNone;
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
		case TSqlResourceProfiler::ESqlCounterOsCall:
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			err = sqlite3SymbianProfilerQuery(KCounterType, ipcBuf);
			break;
		case TSqlResourceProfiler::ESqlCounterIpc:
			ipcBuf.AppendNum(TheSqlSrvProfilerIpc[ESqlIpcRq]);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(TheSqlSrvProfilerIpc[ESqlIpcRead]);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(TheSqlSrvProfilerIpc[ESqlIpcWrite]);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(TheSqlSrvProfilerIpcBytes[ESqlIpcRead]);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(TheSqlSrvProfilerIpcBytes[ESqlIpcWrite]);
			ipcBuf.Append(TChar(';'));
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			{
			TInt totalAllocCells = 0;
			TInt totalAllocSize = 0;
			TInt totalFreeSpace = 0;
			TInt biggestBlockSize = 0;
			RHeap& heap = User::Heap();;
			totalAllocCells = heap.AllocSize(totalAllocSize);
			totalFreeSpace = heap.Available(biggestBlockSize);
			ipcBuf.AppendNum(totalAllocCells);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(totalAllocSize);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(totalFreeSpace);
			ipcBuf.Append(TChar(';'));
			ipcBuf.AppendNum(biggestBlockSize);
			ipcBuf.Append(TChar(';'));
			err = sqlite3SymbianProfilerQuery(KCounterType, ipcBuf);
			}
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			ipcBuf.AppendNum(TheSqlSrvProfilerMaxAllocSize);
			ipcBuf.Append(TChar(';'));
			err = sqlite3SymbianProfilerQuery(KCounterType, ipcBuf);
			break;
        case TSqlResourceProfiler::ESqlCounterTrace:
            break;
		default:
			err = KErrNotSupported;
			break;
		}
	__SQLLEAVE_IF_ERROR2(err);
	aMessage.WriteL(2, ipcBuf);
	}
	
////////////////////////// IPC tracing related ////////////////////////////////////////

//Max trace line length
const TInt KSqlTraceMaxLength = 220;
//Trace buffer.
static TBuf<KSqlTraceMaxLength> TheSqlTraceBuf;

//IPC calls names - begin
_LIT(KSqlSrvDbCreate, "DbCreate");
_LIT(KSqlSrvDbCreateSecure, "DbCreateSecure");
_LIT(KSqlSrvDbOpen, "DbOpen");
_LIT(KSqlSrvDbOpenFromHandle, "DbOpenFromHandle");
_LIT(KSqlSrvDbClose, "DbClose");
_LIT(KSqlSrvDbCopy, "DbCopy");
_LIT(KSqlSrvDbDelete, "DbDelete");
_LIT(KSqlSrvLastErrorMsg, "LastErrorMsg");
_LIT(KSqlSrvDbExec8, "DbExec8");
_LIT(KSqlSrvDbExec16, "DbExec16");
_LIT(KSqlSrvDbSetIsolationLevel, "DbSetIsolationLevel");
_LIT(KSqlSrvDbGetSecurityPolicy, "DbGetSecurityPolicy");
_LIT(KSqlSrvDbAttach, "DbAttach");
_LIT(KSqlSrvDbAttachFromHandle, "DbAttachFromHandle");
_LIT(KSqlSrvDbDetach, "DbDetach");
_LIT(KSqlSrvDbScalarFullSelect8, "DbScalarFullSelect8");
_LIT(KSqlSrvDbScalarFullSelect16, "DbScalarFullSelect16");
_LIT(KSqlSrvDbInTransaction, "DbInTransaction");
_LIT(KSqlSrvDbSize, "DbSize");
_LIT(KSqlSrvDbSize2, "DbSize2");
_LIT(KSqlSrvDbBlobSource, "DbBlobSource");
_LIT(KSqlSrvDbLastInsertedRowId, "DbLastInsertedRowId");
_LIT(KSqlSrvDbCompact, "DbCompact");
_LIT(KSqlSrvDbReserveDriveSpace, "DbReserveDriveSpace");
_LIT(KSqlSrvDbFreeReservedSpace, "DbFreeReservedSpace");
_LIT(KSqlSrvDbGetReserveAccess, "DbGetReserveAccess");
_LIT(KSqlSrvDbReleaseReserveAccess, "DbReleaseReserveAccess");
_LIT(KSqlSrvStmtPrepare8, "StmtPrepare8");
_LIT(KSqlSrvStmtPrepare16, "StmtPrepare16");
_LIT(KSqlSrvStmtClose, "StmtClose");
_LIT(KSqlSrvStmtReset, "StmtReset");
_LIT(KSqlSrvStmtExec, "StmtExec");
_LIT(KSqlSrvStmtAsyncExec, "StmtAsyncExec");
_LIT(KSqlSrvStmtBindExec, "StmtBindExec");
_LIT(KSqlSrvStmtAsyncBindExec, "StmtAsyncBindExec");
_LIT(KSqlSrvStmtNext, "StmtNext");
_LIT(KSqlSrvStmtBindNext, "StmtBindNext");
_LIT(KSqlSrvStmtColumnNames, "StmtColumnNames");
_LIT(KSqlSrvStmtParamNames, "StmtParamNames");
_LIT(KSqlSrvStmtColumnSource, "StmtColumnSource");
_LIT(KSqlSrvStmtBinParamSink, "StmtBinParamSink");
_LIT(KSqlSrvStmtTxtParamSink16, "StmtTxtParamSink16");
_LIT(KSqlSrvStmtBufFlat, "StmtBufFlat");
_LIT(KSqlSrvStmtColumnValue, "StmtColumnValue");
_LIT(KSqlSrvStmtDeclColumnTypes, "StmtDeclColumnTypes");
_LIT(KSqlSrvStreamRead, "StreamRead");
_LIT(KSqlSrvStreamWrite, "StreamWrite");
_LIT(KSqlSrvStreamSize, "StreamSize");
_LIT(KSqlSrvStreamSynch, "StreamSynch");
_LIT(KSqlSrvStreamClose, "StreamClose");
//IPC calls names - end

//Gets as an argument the IPC call type in "aCode" parameter.
//Returns: 
// 0 or positive integer: the IPC call is one of the 10 most important IPC calls (trace level 0).
// KErrNotFound         : the IPC call is some of the other possible call types (trace level 1).
// KErrNotSupported     : unknown IPC call type.
//
// aIpcCallName will always be set to IPC call name descriptor.
static TInt SqlIpcTraceIdxAndName(TSqlSrvFunction aCode, TPtrC& aIpcCallName)
    {
    TInt rc = KErrNotFound;
    switch(aCode)
        {
        case ESqlSrvDbCreate:
            aIpcCallName.Set(KSqlSrvDbCreate);
            break;
        case ESqlSrvDbCreateSecure:
            aIpcCallName.Set(KSqlSrvDbCreateSecure);
            break;
        case ESqlSrvDbOpen:
            aIpcCallName.Set(KSqlSrvDbOpen);
            break;
        case ESqlSrvDbOpenFromHandle:
            aIpcCallName.Set(KSqlSrvDbOpenFromHandle);
            break;
        case ESqlSrvDbClose:
            aIpcCallName.Set(KSqlSrvDbClose);
            break;
        case ESqlSrvDbCopy:
            aIpcCallName.Set(KSqlSrvDbCopy);
            break;
        case ESqlSrvDbDelete:
            aIpcCallName.Set(KSqlSrvDbDelete);
            break;
        case ESqlSrvLastErrorMsg:
            aIpcCallName.Set(KSqlSrvLastErrorMsg);
            break;
        case ESqlSrvDbExec8:
            aIpcCallName.Set(KSqlSrvDbExec8);
            rc = 0;
            break;
        case ESqlSrvDbExec16:
            aIpcCallName.Set(KSqlSrvDbExec16);
            rc = 1;
            break;
        case ESqlSrvDbSetIsolationLevel:
            aIpcCallName.Set(KSqlSrvDbSetIsolationLevel);
            break;
        case ESqlSrvDbGetSecurityPolicy:
            aIpcCallName.Set(KSqlSrvDbGetSecurityPolicy);
            break;
        case ESqlSrvDbAttach:
            aIpcCallName.Set(KSqlSrvDbAttach);
            break;
        case ESqlSrvDbAttachFromHandle:
            aIpcCallName.Set(KSqlSrvDbAttachFromHandle);
            break;
        case ESqlSrvDbDetach:
            aIpcCallName.Set(KSqlSrvDbDetach);
            break;
        case ESqlSrvDbScalarFullSelect8:
            aIpcCallName.Set(KSqlSrvDbScalarFullSelect8);
            rc = 2;
            break;
        case ESqlSrvDbScalarFullSelect16:
            aIpcCallName.Set(KSqlSrvDbScalarFullSelect16);
            rc = 3;
            break;
        case ESqlSrvDbInTransaction:
            aIpcCallName.Set(KSqlSrvDbInTransaction);
            break;
        case ESqlSrvDbSize:
            aIpcCallName.Set(KSqlSrvDbSize);
            break;
        case ESqlSrvDbSize2:
            aIpcCallName.Set(KSqlSrvDbSize2);
            break;
        case ESqlSrvDbBlobSource:
            aIpcCallName.Set(KSqlSrvDbBlobSource);
            break;
        case ESqlSrvDbLastInsertedRowId:
            aIpcCallName.Set(KSqlSrvDbLastInsertedRowId);
            break;
        case ESqlSrvDbCompact:
            aIpcCallName.Set(KSqlSrvDbCompact);
            break;
        case ESqlSrvDbReserveDriveSpace:
            aIpcCallName.Set(KSqlSrvDbReserveDriveSpace);
            break;
        case ESqlSrvDbFreeReservedSpace:
            aIpcCallName.Set(KSqlSrvDbFreeReservedSpace);
            break;
        case ESqlSrvDbGetReserveAccess:
            aIpcCallName.Set(KSqlSrvDbGetReserveAccess);
            break;
        case ESqlSrvDbReleaseReserveAccess:
            aIpcCallName.Set(KSqlSrvDbReleaseReserveAccess);
            break;
        case ESqlSrvStmtPrepare8:
            aIpcCallName.Set(KSqlSrvStmtPrepare8);
            break;
        case ESqlSrvStmtPrepare16:
            aIpcCallName.Set(KSqlSrvStmtPrepare16);
            break;
        case ESqlSrvStmtClose:
            aIpcCallName.Set(KSqlSrvStmtClose);
            break;
        case ESqlSrvStmtReset:
            aIpcCallName.Set(KSqlSrvStmtReset);
            break;
        case ESqlSrvStmtExec:
            aIpcCallName.Set(KSqlSrvStmtExec);
            rc = 4;
            break;
        case ESqlSrvStmtAsyncExec:
            aIpcCallName.Set(KSqlSrvStmtAsyncExec);
            rc = 5;
            break;
        case ESqlSrvStmtBindExec:
            aIpcCallName.Set(KSqlSrvStmtBindExec);
            rc = 6;
            break;
        case ESqlSrvStmtAsyncBindExec:
            aIpcCallName.Set(KSqlSrvStmtAsyncBindExec);
            rc = 7;
            break;
        case ESqlSrvStmtNext:
            aIpcCallName.Set(KSqlSrvStmtNext);
            rc = 8;
            break;
        case ESqlSrvStmtBindNext:
            aIpcCallName.Set(KSqlSrvStmtBindNext);
            rc = 9;
            break;
        case ESqlSrvStmtColumnNames:
            aIpcCallName.Set(KSqlSrvStmtColumnNames);
            break;
        case ESqlSrvStmtParamNames:
            aIpcCallName.Set(KSqlSrvStmtParamNames);
            break;
        case ESqlSrvStmtColumnSource:
            aIpcCallName.Set(KSqlSrvStmtColumnSource);
            break;
        case ESqlSrvStmtBinParamSink:
            aIpcCallName.Set(KSqlSrvStmtBinParamSink);
            break;
        case ESqlSrvStmtTxtParamSink16:
            aIpcCallName.Set(KSqlSrvStmtTxtParamSink16);
            break;
        case ESqlSrvStmtBufFlat:
            aIpcCallName.Set(KSqlSrvStmtBufFlat);
            break;
        case ESqlSrvStmtColumnValue:
            aIpcCallName.Set(KSqlSrvStmtColumnValue);
            break;
        case ESqlSrvStmtDeclColumnTypes:
            aIpcCallName.Set(KSqlSrvStmtDeclColumnTypes);
            break;
        case ESqlSrvStreamRead:
            aIpcCallName.Set(KSqlSrvStreamRead);
            break;
        case ESqlSrvStreamWrite:
            aIpcCallName.Set(KSqlSrvStreamWrite);
            break;
        case ESqlSrvStreamSize:
            aIpcCallName.Set(KSqlSrvStreamSize);
            break;
        case ESqlSrvStreamSynch:
            aIpcCallName.Set(KSqlSrvStreamSynch);
            break;
        case ESqlSrvStreamClose:
            aIpcCallName.Set(KSqlSrvStreamClose);
            break;
        default:
            return KErrNotSupported;
        };
    __ASSERT_DEBUG((TUint)rc < KIpcTraceTypeCount || rc == KErrNotFound, __SQLPANIC2(ESqlPanicInternalError));
    return rc;
    }

//Calculates and returns the time difference between aStartTicks and aEndTicks in microseconds.  
static TInt SqlConvertTicks2Us(TUint32 aStartTicks, TUint32 aEndTicks)
    {
    static TInt freq = 0;
    if(freq == 0)
        {
        TInt err = HAL::Get(HAL::EFastCounterFrequency, freq);
        if(err != KErrNone)
            {
            __SQLPANIC2((TSqlPanic)err);
            }
        }
    TInt64 diffTicks = (TInt64)aEndTicks - (TInt64)aStartTicks;
    if(diffTicks < 0)
        {
        diffTicks = KMaxTUint32 + diffTicks + 1;
        }
    const TInt KMicroSecIn1Sec = 1000000;
    TInt32 us = (diffTicks * KMicroSecIn1Sec) / freq;
    return us;
    }

//Calculates the time since the first time this function has been called.
static TInt64 SqlTimeFromStartUs()
    {
    struct TStartTime
        {
        inline TStartTime()
            {
            iTime.UniversalTime();
            }
        TTime iTime;
        };
    static TStartTime startTime;
    TTime time;
    time.UniversalTime();
    TTimeIntervalMicroSeconds us = time.MicroSecondsFrom(startTime.iTime);
    const TInt64 KMaxDiff = 999999999999LL;//999999999999 is the max number that can be printed out using %12ld format spec
    if(us.Int64() < 0 || us.Int64() >= KMaxDiff)
        {
        startTime.iTime = time;
        us = 0;
        }
    return us.Int64();
    }


//Tracing data buffer
const TInt KSqlSrvProfilePrnBufSize = 300;
static TBuf<KSqlSrvProfilePrnBufSize> TheSqlSrvProfileTraceBuf;
static TBuf8<KSqlSrvProfilePrnBufSize> TheSqlSrvProfileTraceBuf8;

static RFs TheSqlSrvTraceFs;
static RFile TheTheSqlSrvTraceFile;
_LIT(KSqlSrvTraceFileName, "C:\\SQLTRACE");

//Prints out a time stamp
static void SqlSrvProfileTimePrintf()
    {
    static TInt64 prevTimeDiff = 0;;
    TInt64 timeDiff = SqlTimeFromStartUs();
    const TInt64 KTimeInterval = 1000000; 
    if((timeDiff - prevTimeDiff) >= KTimeInterval || timeDiff < prevTimeDiff)
        {
        prevTimeDiff = timeDiff;
        TTime time;
        time.UniversalTime();
        TDateTime dt = time.DateTime();
        TheSqlSrvProfileTraceBuf8.Format(_L8("[SQL]¬\"%X\"¬¬TME¬¬¬¬¬¬¬¬¬¬¬¬%02d:%02d:%02d:%06d¬Prep8¬%d¬Prep16¬%d¬Ex8¬%d¬Ex16¬%d"),
                timeDiff, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond(),
                TheSqlSrvProfilerPreparedCnt8, TheSqlSrvProfilerPreparedCnt16, 
                TheSqlSrvProfilerExecutedCnt8, TheSqlSrvProfilerExecutedCnt16);
        if(TheSqlSrvProfilerTraceToFile)
            {
            TheSqlSrvProfileTraceBuf8.Append(_L8("\r\n"));
            (void)TheTheSqlSrvTraceFile.Write(TheSqlSrvProfileTraceBuf8);
            }
        else
            {
            TheSqlSrvProfileTraceBuf8.Append(_L8("\n"));
            RDebug::RawPrint(TheSqlSrvProfileTraceBuf8);
            }
        }
    }

//Trace types
enum TSqlSrvProfilerTraceType
    {
    ESqlSrvProfilerNonSqlTrace,
    ESqlSrvProfilerMiddleLineSqlTrace,
    ESqlSrvProfilerLastLineSqlTrace
    };

//Prints out the data that is in TheSqlSrvProfileTraceBuf.
// aType = ESqlSrvProfilerNonSqlTrace       - non-SQL trace
// aType = ESqlSrvProfilerMiddleLineSqlTrace - not last line of an SQL trace
// aType = ESqlSrvProfilerLastLineSqlTrace  - last line of an SQL trace
static void SqlSrvProfilePrintf(TSqlSrvProfilerTraceType aType)
    {
    SqlSrvProfileTimePrintf();
    TheSqlSrvProfileTraceBuf8.Copy(TheSqlSrvProfileTraceBuf);
    if(TheSqlSrvProfilerTraceToFile)
        {
        if(aType == 0 || aType == 2)
            {
            TheSqlSrvProfileTraceBuf8.Append(_L8("\r\n"));
            }
        (void)TheTheSqlSrvTraceFile.Write(TheSqlSrvProfileTraceBuf8);
        }
    else
        {
        TheSqlSrvProfileTraceBuf8.Append(_L8("\n"));
        RDebug::RawPrint(TheSqlSrvProfileTraceBuf8);
        }
    }

//Called at the beginning of CSqlSrvSession::ServiceL().
void SqlIpcStart(TUint& aIpcCounter, TUint32& aStartTicks, TUint aDbHandle)
    {
    if(TheSqlSrvProfilerTraceEnabled)
        {
        TheSqlSrvProfilerFileRead = TheSqlSrvProfilerFileWrite = TheSqlSrvProfilerFileSync = TheSqlSrvProfilerFileSetSize = 0;
        if(TheSqlSrvProfilerTraceLevel == 0)
            {
            return;
            }
        if(KSqlSrvProfilerDbName().Length() > 0 && (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
            {
            return;
            }
        ++aIpcCounter;
        aStartTicks = User::FastCounter();
        }
    }

//Called at the end of CSqlSrvSession::ServiceL().
void SqlIpcEnd(TUint aIpcCounter, TUint32 aStartTicks, TSqlSrvFunction aFuncCode, 
               TUint aDbHandle, TSqlSrvIpcTraceData aIpcTraceData[], TInt aRetCode)
    {
    if(TheSqlSrvProfilerTraceEnabled)
        {
        if(TheSqlSrvProfilerTraceLevel == 0)
            {
            return;
            }
        if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
            {
            return;
            }
        TUint32 endTicks = User::FastCounter();
        TInt executionTime = SqlConvertTicks2Us(aStartTicks, endTicks);
        TPtrC ipcCallName;
        TInt ipcCallIdx = SqlIpcTraceIdxAndName(aFuncCode, ipcCallName); 
        TInt64 timeFromStart = SqlTimeFromStartUs();
        TInt64 ttlExecTime = 0;
        TInt count = 0;
        if(ipcCallIdx >= 0)
            {
            aIpcTraceData[ipcCallIdx].iExecutionTime += executionTime;
            ttlExecTime = aIpcTraceData[ipcCallIdx].iExecutionTime;
            count = ++aIpcTraceData[ipcCallIdx].iCount;
            }
        if(ipcCallIdx >= 0  || (ipcCallIdx == KErrNotFound && TheSqlSrvProfilerTraceLevel == 2))
            {
            TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬\"%X\"¬%ld¬IPC¬%u¬%S¬%ld¬%d¬%d¬%d¬%d¬%d¬%d¬rc¬%d"),  
														//[SQL]
                    aDbHandle,							//Database handle
                    timeFromStart,						//Time from start, microseconds
														//IPC
                    aIpcCounter,						//IPC sequence counter for this database (connection)
                    &ipcCallName,						//IPC call name
                    ttlExecTime,						//All time spent in this IPC call type for this database (connection)
                    executionTime,						//This IPC call execution time
                    count,								//This IPC call sequence counter for this database (connection)
                    TheSqlSrvProfilerFileRead,			//File read count, performed during this IPC
                    TheSqlSrvProfilerFileWrite,			//File write count, performed during this IPC
                    TheSqlSrvProfilerFileSync,			//File flush count, performed during this IPC
                    TheSqlSrvProfilerFileSetSize,		//File set size count, performed during this IPC
                    aRetCode);							//IPC call - return code
            SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
            }
        }
    }

//Called at the end of CSqlSrvSession::ServiceError().
void SqlIpcError(TUint aIpcCounter, TSqlSrvFunction aFuncCode, TUint aDbHandle, TInt aError)
    {
    if(TheSqlSrvProfilerTraceEnabled)
        {
        if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
            {
            return;
            }
        TPtrC ipcCallName;
        (void)SqlIpcTraceIdxAndName(aFuncCode, ipcCallName); 
        TInt64 timeFromStart = SqlTimeFromStartUs();
        TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬\"%X\"¬%ld¬ERR¬%u¬%S¬¬¬¬¬¬¬¬err¬%d"), 
									//[SQL]
                aDbHandle,			//Database (connection) handle
                timeFromStart,		//Time from start, microseconds
									//ERR
                aIpcCounter,		//IPC sequence counter for this database (connection)
                &ipcCallName,		//IPC call name
                aError);			//IPC call - return code
        SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
        }
    }

//Prints the passed as a parameter 16-bit SQL statement.
void SqlPrintSql16(TUint aDbHandle, const TDesC& aSql, TBool aPrepare)
    {
    if(!TheSqlSrvProfilerTraceEnabled || !TheSqlSrvProfilerSqlTraceEnabled)
        {
        return;
        }
    if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
        {
        return;
        }
    
    aPrepare ? ++TheSqlSrvProfilerPreparedCnt16 : ++TheSqlSrvProfilerExecutedCnt16;
    
    TInt64 timeFromStart = SqlTimeFromStartUs();
    TPtr line((TUint16*)TheSqlTraceBuf.Ptr(), 0, TheSqlTraceBuf.MaxLength());
    TInt len = aSql.Length();
    TInt pos = 0;
    do
        {
        _LIT(KPrepare,  "Prepare16");
        _LIT(KExec,     "Exec16");
        _LIT(KEmptyStr,  "");
        if(pos == 0)
            {
            line.Format(_L("[SQL]¬\"%X\"¬%ld¬SQL¬¬%S¬¬¬¬¬¬¬¬¬¬"), aDbHandle, timeFromStart, aPrepare ? &KPrepare : &KExec);
            }
        else
            {
            if(!TheSqlSrvProfilerTraceToFile)
                {
                line.Format(_L("[SQL]¬\"%X\"¬%ld¬SQL¬¬%S¬¬¬¬¬¬¬¬¬¬"), aDbHandle, timeFromStart, &KEmptyStr);
                }
            }
        TInt l = Min(len, (line.MaxLength() - line.Length()));
        TPtrC ptr(aSql.Ptr() + pos, l);
        pos += l;
        len -= l;
        line.Append(ptr);
        TheSqlSrvProfileTraceBuf.Format(_L("%S"), &line); 
        SqlSrvProfilePrintf(len > 0 ? ESqlSrvProfilerMiddleLineSqlTrace : ESqlSrvProfilerLastLineSqlTrace);
        line.Zero();
        } while(len > 0);
    }
 
//Prints the passed as a parameter 8-bit SQL statement.
void SqlPrintSql8(TUint aDbHandle, const TDesC8& aSql, TBool aPrepare)
    {
    if(!TheSqlSrvProfilerTraceEnabled || !TheSqlSrvProfilerSqlTraceEnabled)
        {
        return;
        }
    if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
        {
        return;
        }
    
    aPrepare ? ++TheSqlSrvProfilerPreparedCnt8 : ++TheSqlSrvProfilerExecutedCnt8;
    
    TInt64 timeFromStart = SqlTimeFromStartUs();
    TPtr line((TUint16*)TheSqlTraceBuf.Ptr(), 0, TheSqlTraceBuf.MaxLength());
    TInt len = aSql.Length();
    TInt pos = 0;
    do
        {
        _LIT(KPrepare,   "Prepare8");
        _LIT(KExec,      "Exec8");
        _LIT(KEmptyStr,  "");
        if(pos == 0)
            {
            line.Format(_L("[SQL]¬\"%X\"¬%ld¬SQL¬¬%S¬¬¬¬¬¬¬¬¬¬"), aDbHandle, timeFromStart, aPrepare ? &KPrepare : &KExec);
            }
        else
            {
            if(!TheSqlSrvProfilerTraceToFile)
                {
                line.Format(_L("[SQL]¬\"%X\"¬%ld¬SQL¬¬%S¬¬¬¬¬¬¬¬¬¬"), aDbHandle, timeFromStart, &KEmptyStr);
                }
            }
        TInt l = Min(len, (line.MaxLength() - line.Length()));
        TPtrC8 ptr(aSql.Ptr() + pos, l);
        pos += l;
        len -= l;
        TPtr p2((TUint16*)line.Ptr() + line.Length(), 0, l);  
        p2.Copy(ptr);
        line.SetLength(line.Length() + p2.Length());
        TheSqlSrvProfileTraceBuf.Format(_L("%S"), &line); 
        SqlSrvProfilePrintf(len > 0 ? ESqlSrvProfilerMiddleLineSqlTrace : ESqlSrvProfilerLastLineSqlTrace);
        line.Zero();
        } while(len > 0);
    }

//Prints the name of the just created database.
void SqlPrintDbCreate(TUint aDbHandle, const TDesC& aDbName)
    {
    if(TheSqlSrvProfilerTraceEnabled)
        {
        if(KSqlSrvProfilerDbName().Length() > 0 && aDbName.FindF(KSqlSrvProfilerDbName) >= 0)
            {
            TheSqlSrvProfilerHandle = aDbHandle;
            }
        if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
            {
            return;
            }
        TInt64 timeFromStart = SqlTimeFromStartUs();
        TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬\"%X\"¬%ld¬CRE¬¬¬¬¬¬¬¬¬¬¬¬%S"), 
                aDbHandle, 
                timeFromStart,
                &aDbName);
        SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
        }
    }

//Prints the name of the just opened database.
void SqlPrintDbOpen(TUint aDbHandle, const TDesC& aDbName)
    {
    if(TheSqlSrvProfilerTraceEnabled)
        {
        if(KSqlSrvProfilerDbName().Length() > 0 && aDbName.FindF(KSqlSrvProfilerDbName) >= 0)
            {
            TheSqlSrvProfilerHandle = aDbHandle;
            }
        if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
            {
            return;
            }
        TInt64 timeFromStart = SqlTimeFromStartUs();
        TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬\"%X\"¬%ld¬OPN¬¬¬¬¬¬¬¬¬¬¬¬%S"), 
                aDbHandle, 
                timeFromStart,
                &aDbName);
        SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
        }
    }

//Prints the handle of the just closed database.
void SqlPrintDbClose(TUint aDbHandle)
    {
    if(TheSqlSrvProfilerTraceEnabled)
        {
        if(KSqlSrvProfilerDbName().Length() > 0 &&  (aDbHandle == 0 || TheSqlSrvProfilerHandle != aDbHandle))
            {
            return;
            }
        TInt64 timeFromStart = SqlTimeFromStartUs();
        TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬\"%X\"¬%ld¬CSE"),  
                aDbHandle, 
                timeFromStart);
        SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
        if(aDbHandle == TheSqlSrvProfilerHandle)
            {
            TheSqlSrvProfilerHandle = 0;
            }
        }
    }

//Prints a trace when the SQL server starts
void SqlPrintServerStart()
    {
    TInt64 timeFromStart = SqlTimeFromStartUs();
    if(TheSqlSrvProfilerTraceToFile)
        {
        TInt err = TheSqlSrvTraceFs.Connect();
        if(err == KErrNone)
            {
            TInt fileNum = 0;
            err = KErrGeneral;
            while(++fileNum < 1000 && err != KErrNone)
                {
                TBuf<80> ftrname;
                ftrname.Copy(KSqlSrvTraceFileName);
                ftrname.AppendNum(fileNum);
                ftrname.Append(_L(".txt"));
                err = TheTheSqlSrvTraceFile.Create(TheSqlSrvTraceFs, ftrname, EFileRead | EFileWrite);
                if(err == KErrNone)
                    {
                    break;
                    }
                }
            }
        if(err != KErrNone)
            {
            TheSqlSrvTraceFs.Close();
            TheSqlSrvProfilerTraceToFile = EFalse;
            RDebug::Print(_L("SQL trace file creation failed with err=%d"), err);
            }
        }
    TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬¬%ld¬SRV¬¬Start"), timeFromStart); 
    SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
    }

//Prints a trace when the SQL server stops
void SqlPrintServerStop()
    {
    TInt64 timeFromStart = SqlTimeFromStartUs();
    TheSqlSrvProfileTraceBuf.Format(_L("[SQL]¬¬%ld¬SRV¬¬Stop"), timeFromStart); 
    SqlSrvProfilePrintf(ESqlSrvProfilerNonSqlTrace);
    if(TheSqlSrvProfilerTraceToFile)
        {
        TheTheSqlSrvTraceFile.Close();
        TheSqlSrvTraceFs.Close();
        }
    }

#else //_SQLPROFILER

void TSqlSrvResourceProfiler::StartL(const RMessage2&)
	{
	__SQLLEAVE2(KErrNotSupported);
	}
	
void TSqlSrvResourceProfiler::StopL(const RMessage2&)
	{
	__SQLLEAVE2(KErrNotSupported);
	}
	
void TSqlSrvResourceProfiler::ResetL(const RMessage2&)
	{
	__SQLLEAVE2(KErrNotSupported);
	}
	
void TSqlSrvResourceProfiler::QueryL(const RMessage2&)
	{
	__SQLLEAVE2(KErrNotSupported);
	}
	
#endif//_SQLPROFILER
