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

#include "SqlResourceProfiler.h"
#include "SqlAssert.h"
#include "SqlDatabaseImpl.h"


#pragma BullseyeCoverage off

/**
Initializes TSqlResourceProfiler data members with their default values.

@param aDatabase The RSqlDatabase object that will be profiled.
*/
EXPORT_C TSqlResourceProfiler::TSqlResourceProfiler(RSqlDatabase& aDatabase) :
	iDatabase(aDatabase)
	{
	}

#ifdef _SQLPROFILER
	
/**
Starts the specified profiling counter.

@param aCounterType Profiling counter type.
@param aPrm Additional profiling parameters.

@capability None

@return KErrNone, the operation completed successfully;
                  One of the other system-wide error codes may also be returned.

Usage of the IPC call arguments:
 - Arg 0: [out]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
 
When the counter type is ESqlCounterTrace, then the optional aPrm argument can contain the follwing 
configuration parameters, split with ";":
   - "L0" - [default] tracing level 0. IPC traces disabled;
   - "L1" - tracing level 1. Only the most important IPC calls are traced:
            ESqlSrvDbExec8, ESqlSrvDbExec16, ESqlSrvDbScalarFullSelect16, ESqlSrvStmtExec,
            ESqlSrvStmtAsyncExec, ESqlSrvStmtBindExec, ESqlSrvStmtAsyncBindExec, ESqlSrvStmtNext,
            ESqlSrvStmtBindNext;
   - "L2" - tracing level 2. All IPC calls traced;
   - "S0" - [default] SQL statement tracing is off; 
   - "S1" - SQL statement tracing is on;
*/
EXPORT_C TInt TSqlResourceProfiler::Start(TSqlResourceProfiler::TSqlCounter aCounterType, const TDesC8* aPrm)
	{
	TIpcArgs ipcArgs;
	ipcArgs.Set(0, aCounterType);
    ipcArgs.Set(1, 0);
	if(aPrm)
	    {
        ipcArgs.Set(1, aPrm->Length());
        ipcArgs.Set(2, aPrm);
	    }
	return iDatabase.Impl().Session().SendReceive(ESqlSrvProfilerStart, ipcArgs);
	}
	
/**
Stops the specified profiling counter.

@param aCounterType Profiling counter type.

@capability None

@return KErrNone, the operation completed successfully;
                  One of the other system-wide error codes may also be returned.

Usage of the IPC call arguments:
 - Arg 0: [out]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
*/
EXPORT_C TInt TSqlResourceProfiler::Stop(TSqlResourceProfiler::TSqlCounter aCounterType)
	{
	return iDatabase.Impl().Session().SendReceive(ESqlSrvProfilerStop, TIpcArgs(aCounterType));
	}
	
/**
Sets to zero the specified profiling counter.

@param aCounterType Profiling counter type.

@capability None

@return KErrNone, the operation completed successfully;
                  One of the other system-wide error codes may also be returned.

Usage of the IPC call arguments:
 - Arg 0: [out]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
*/
EXPORT_C TInt TSqlResourceProfiler::Reset(TSqlResourceProfiler::TSqlCounter aCounterType)
	{
	return iDatabase.Impl().Session().SendReceive(ESqlSrvProfilerReset, TIpcArgs(aCounterType));
	}
	
/**
Retrieves the profiling counter(s) values for the aCounterType.

@param aCounterType Profiling counter type.
@param aResult Buffer, where the results will be copied.

@capability None

@return KErrNone, the operation completed successfully;
                  One of the other system-wide error codes may also be returned.
                  
@see TSqlResourceProfiler

Usage of the IPC call arguments:
 - Arg 0: [out]  profiling counter type, one of the TSqlResourceProfiler::TSqlCounter enum item values.
 - Arg 1: [out]  the size of the buffer for the profiling counters
 - Arg 2: [in]   the buffer for the profiling counters
*/
EXPORT_C TInt TSqlResourceProfiler::Query(TSqlResourceProfiler::TSqlCounter aCounterType, TDes8& aResult)
	{
	return iDatabase.Impl().Session().SendReceive(ESqlSrvProfilerQuery, TIpcArgs(aCounterType, aResult.MaxLength(), &aResult));
	}

#else //_SQLPROFILER

EXPORT_C TInt TSqlResourceProfiler::Start(TSqlResourceProfiler::TSqlCounter, const TDesC8*)
	{
	return KErrNotSupported;
	}
	
EXPORT_C TInt TSqlResourceProfiler::Stop(TSqlResourceProfiler::TSqlCounter)
	{
	return KErrNotSupported;
	}
	
EXPORT_C TInt TSqlResourceProfiler::Reset(TSqlResourceProfiler::TSqlCounter)
	{
	return KErrNotSupported;
	}
	
EXPORT_C TInt TSqlResourceProfiler::Query(TSqlResourceProfiler::TSqlCounter aCounterType, TDes8& aResult)
	{
	if(aCounterType == TSqlResourceProfiler::ESqlCounterConfig)	
		{
		return iDatabase.Impl().Session().SendReceive(ESqlSrvProfilerQuery, TIpcArgs(aCounterType, aResult.MaxLength(), &aResult));
		}
	return KErrNotSupported;
	}

#endif//_SQLPROFILER

#pragma BullseyeCoverage on
