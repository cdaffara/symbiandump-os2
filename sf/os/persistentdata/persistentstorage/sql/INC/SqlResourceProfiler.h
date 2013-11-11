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
// Symbian SQL Resource Profiler header file
// 
//

/**
 @file
 @internalComponent
*/
#ifndef __SQLRESOURCEPROFILER_H__
#define __SQLRESOURCEPROFILER_H__

#include <sqldb.h>

/**
Symbian SQL profiling interface.
TSqlResourceProfiler class is used for:

@code

 -	Retrieving the current state of the SQL server heap and SQLite heap stats.
   	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		profiler.Start(TSqlResourceProfiler::ESqlCounterMemory);//Start the profiling
		profiler.Reset(TSqlResourceProfiler::ESqlCounterMemory);//Zero the porfiling counters
		....
		profiler.Stop(TSqlResourceProfiler::ESqlCounterMemory); //Stop the profiling
		TBuf8<128> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterMemory, res);
	The format of the retrieved counters is:   
		"<Allocated cells count>;<Allocated size>;<Free space>;<Largest block size>;
		 <SQLite: allocated cnt>;<SQLite: reallocated cnt>;<SQLite: freed cnt>;
		 <SQLite: allocated bytes>;<SQLite: freed bytes>;
		 <SQLite: time spent in alloc(), us>;<SQLite: time spent in realloc(), us>;<SQLite: time spent in free(), us>;";
	This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.
		
 - Retrieving the size of the biggest memory block ever allocated by the Symbian SQL server and SQLite.
   	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		.....
		profiler.Start(TSqlResourceProfiler::ESqlCounterMaxAlloc);//Start the profiling
		.....
		TBuf8<32> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterMaxAlloc, res);
		...
		profiler.Reset(TSqlResourceProfiler::ESqlCounterMaxAlloc);//Reset the profiling 
		....
		profiler.Stop(TSqlResourceProfiler::ESqlCounterMaxAlloc);
	The format of the retrieved counters is: 
		"<SQL server - max alloc size>;<SQLite - max alloc size>";
	This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.

 - Retrieving the number of the file system calls made by the OS porting layer and the amount of written/read data.
  	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		.....
		profiler.Start(TSqlResourceProfiler::ESqlCounterFileIO);//Start the profiling
		.....
		TBuf8<300> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterFileIO, res);
		.....
		profiler.Reset(TSqlResourceProfiler::ESqlCounterFileIO);//Reset the profiling 
		....
		profiler.Stop(TSqlResourceProfiler::ESqlCounterFileIO);
	The format of the retrieved data is: 
	"<RFileBuf64::Create() cnt>;<RFileBuf64::Open() cnt>;<RFileBuf64::Close() cnt>;<RFs::Delete() cnt>;
	 <RFileBuf64::Read() cnt>;<RFileBuf64::Write() cnt>;<RFileBuf64::Seek() cnt>;<RFileBuf64::Size() cnt>;<RFileBuf64::SetSize() cnt>;
	 <RFileBuf64::Flush() cnt>;<RFile64::Drive() cnt>;<RFileBuf64::AdoptFromClient() cnt>;<RFs::Close() cnt>;
	 <RFs::Connect() cnt>;<RFs::GetSystemDrive() cnt>;<RFs::CreatePrivatePath() cnt>;<RFs::PrivatePath() cnt>;
	 <RFs::VolumeIoParam() cnt>;<RFs::Entry() cnt>;<RFs::Att() cnt>;<RFileBuf64::Temp() cnt>;<0>;
	 <bytes written>;<bytes read>;"
	Note that currently the file operations are buffered, so the retrieved results will be about the operations executed on a
	RFileBuf64 objects.
	This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.

 - Retrieving information regading the current SQL/SQLite configuration.
  	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		TBuf8<128> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterConfig, res);
	The format of the retrieved numbers is:   
		"<Cache size in pages>;<Page size in bytes>;<Database encoding>;<Default soft heap limit>;<SQLite vacuum mode>";

 - Retrieving the number of the OS porting layer calls made by SQLite.
  	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		.....
		profiler.Start(TSqlResourceProfiler::ESqlCounterOsCall);//Start the profiling
		.....
		TBuf8<300> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterOsCall, res);
		.....
		profiler.Reset(TSqlResourceProfiler::ESqlCounterOsCall);//Reset the profiling 
		....
		profiler.Stop(TSqlResourceProfiler::ESqlCounterOsCall);
	The format of the retrieved data is: 
	"<file close cnt>;<file read cnt>;<file write cnt>;<file truncate cnt>;
	 <file sync cnt>;<file size cnt>;<file lock cnt>;<file unlock cnt>;<file check reserved lock cnt>;
	 <file i/o control cnt>;<file sector size cnt>;<file device characteristics cnt>;<vfs open cnt>;
	 <vfs delete cnt>;<vfs access cnt>;<vfs full path name cnt>;<vfs randomness cnt>;
	 <vfs sleep cnt>;<vfs current time cnt>;<vfs get last error cnt>;"
	This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.
   	
 - Retrieving the OS porting layer call times in microseconds per call type.
  	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		.....
		profiler.Start(TSqlResourceProfiler::ESqlCounterOsCallTime);//Start the profiling
		.....
		TBuf8<300> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterOsCallTime, res);
		.....
		profiler.Reset(TSqlResourceProfiler::ESqlCounterOsCallTime);//Reset the profiling 
		....
		profiler.Stop(TSqlResourceProfiler::ESqlCounterOsCallTime);
	The format of the retrieved data is: 
	"<file close time>;<file read time>;<file write time>;<file truncate time>;
	 <file sync time>;<file size time>;<file lock time>;<file unlock time>;<file check reserved lock time>;
	 <file i/o control time>;<file sector size time>;<file device characteristics time>;<vfs open time>;
	 <vfs delete time>;<vfs access time>;<vfs full path name time>;<vfs randomness time>;
	 <vfs sleep time>;<vfs current time time>;<vfs get last error time>;"
	This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.

 - Retrieving the OS porting layer call times in microseconds per call type plus adding a log entry 
   for each call. The returned information is the same as for the previous profiling type.
   But for each OS porting layer call an entry is added to the log file (epocwind.out).
   The format of the log entry is:
   	<'M'/'J'>,<'Call Id'>,<#Counter>,<Call Counter>,<File Offset>,<Bytes>,<Ticks>,<Bytes Total>,<Ticks Total>
   Note: ESqlCounterOsCallDetails and ESqlCounterOsCallTime profiles cannot be used together.

 - Retrieving the number of the IPC calls and IPC read/write operations.
  	Example:
		RSqlDatabase db;
		//initialize the db object....
		...
		TSqlResourceProfiler profiler(db);
		.....
		profiler.Start(TSqlResourceProfiler::ESqlCounterIpc);//Start the profiling
		.....
		TBuf8<300> res;
		profiler.Query(TSqlResourceProfiler::ESqlCounterIpc, res);
		.....
		profiler.Reset(TSqlResourceProfiler::ESqlCounterIpc);//Reset the profiling 
		....
		profiler.Stop(TSqlResourceProfiler::ESqlCounterIpc);
	The format of the retrieved data is: 
	"<IPC call cnt>;<IPC write cnt>;<IPC read cnt>;<IPC write bytes>;<IPC read bytes>;
	This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.
   	
 - Tracing the executed SQL statements and IPC calls.
   The folowing tracing configurations can be used (pased as a configuration parameter in TSqlResourceProfiler::Start()):
   - "L0" - [default] tracing level 0. IPC traces disabled;
   - "L1" - tracing level 1. Only the most important IPC calls are traced:
            ESqlSrvDbExec8, ESqlSrvDbExec16, ESqlSrvDbScalarFullSelect16, ESqlSrvStmtExec,
            ESqlSrvStmtAsyncExec, ESqlSrvStmtBindExec, ESqlSrvStmtAsyncBindExec, ESqlSrvStmtNext,
            ESqlSrvStmtBindNext;
   - "L2" - tracing level 2. All IPC calls traced;
   - "S0" - [default] SQL statement tracing is off; 
   - "S1" - SQL statement tracing is on;
    Example:
        RSqlDatabase db;
        //initialize the db object....
        ...
        TSqlResourceProfiler profiler(db);
        .....
        _LIT8(KConfig, "<config string>");
        profiler.Start(TSqlResourceProfiler::ESqlCounterTrace, &KConfig);//Start the profiling
        .....
        profiler.Stop(TSqlResourceProfiler::ESqlCounterTrace);
    The format of the traces is:
    - SQL statement trace:
      <DbHandle>¬<TimeFromStart>¬SQL¬Prepare/Exec¬<SQL statemment>
    - "Database create" trace:
      <DbHandle>¬<TimeFromStart>¬CRE¬<database file path>
    - "Database open" trace:
      <DbHandle>¬<TimeFromStart>¬OPN¬<database file path>
    - "Database close" trace:
      <DbHandle>¬<TimeFromStart>¬CSE
    - "IPC call error" trace:
      <DbHandle>¬<TimeFromStart>¬ERR¬<IPC counter>¬<IPC call name>¬err=<error code>
    - "IPC call level 1 or level 2" trace:
      <DbHandle>¬<TimeFromStart>¬IPC¬<IPC counter>¬<IPC call name>¬<TotalExecTime>¬<ExecTime>¬<ThisCallIpcCount>¬..¬rc=<ret code>
    This service can be used only if the SQL code is compiled with the _SQLPROFILER macro.
@endcode

@internalComponent
*/
class TSqlResourceProfiler
	{
public:
	enum TSqlCounter
		{
		ESqlCounterFileIO,
		ESqlCounterOsCall,
		ESqlCounterOsCallTime,
		ESqlCounterOsCallDetails,
		ESqlCounterIpc,
		ESqlCounterMemory,
		ESqlCounterMaxAlloc,
		ESqlCounterConfig,
		ESqlCounterTrace
		};
	IMPORT_C TSqlResourceProfiler(RSqlDatabase& aDatabase);
	IMPORT_C TInt Start(TSqlCounter aCounterType, const TDesC8* aParam = NULL);
	IMPORT_C TInt Stop(TSqlCounter aCounterType);
	IMPORT_C TInt Reset(TSqlCounter aCounterType);
	IMPORT_C TInt Query(TSqlCounter aCounterType, TDes8& aResult);
	
private:	
	RSqlDatabase iDatabase;
	
	};

#endif //__SQLRESOURCEPROFILER_H__
