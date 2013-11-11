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
// The Symbian OS porting layer - single-threaded implementation.
// SQLite never accesses the file system and the OS services directly.
// SQLite uses for that sqlite3_vfs and sqlite3_file objects.
// sqlite3_vfs and sqlite3_file functionality is implemented in this file - 
// TVfs and TFileIo classes.
// This file is also used for the COsLayerData implementation. A single COslayerData
// object is used by the OS porting layer for managing some global data.
// 
//

/**
 @file
 @see TVfs
 @see TFileIo
*/

#ifdef  SQLITE_OS_SYMBIAN

//#define _SQLPROFILER // Enable profiling //The same macro has to be enabled in SqlAssert.h file

extern "C" 
{
#include "sqliteInt.h"
#include "os.h"
}
#include "os_common.h"
#include "SqliteSymbian.h"
#include "FileBuf64.h"
#include <e32math.h>
#ifdef _SQLPROFILER
#include <hal.h>
#include "../INC/SqlResourceProfiler.h"
#endif
#include "SqliteUtil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "os_symbianTraces.h"
#endif
#include "SqliteTraceDef.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//The SQLite temp files willl be created in this subdir
_LIT(KTempFileDir, "temp");

//Bit-mask constant. If xOpen()'s "aFlag" parameter contains one of these bits set, then the the file top be
//opened or created is a journal file.
const TUint KJournalFileTypeBitMask = SQLITE_OPEN_MAIN_JOURNAL | SQLITE_OPEN_TEMP_JOURNAL | SQLITE_OPEN_SUBJOURNAL | SQLITE_OPEN_MASTER_JOURNAL; 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _SQLPROFILER

    //Profiling counters, defined in SqlSrvResourceprofiler.cpp
    extern TInt TheSqlSrvProfilerFileRead;
    extern TInt TheSqlSrvProfilerFileWrite;
    extern TInt TheSqlSrvProfilerFileSync;
    extern TInt TheSqlSrvProfilerFileSetSize;
    
#   define  __COUNTER_INCR(counter) ++counter

	//////////////////////////////////////////////////////////////////////////////
	////////////////////////// File I/O //////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	/**
	This enum is used only in _SQLPROFILER mode for tracing the file system calls.
	@internalComponent
	@see FsCallBrkPt()
	*/
	enum TFsOpType
		{
		EFsOpFileCreate,
		EFsOpFileOpen,
		EFsOpFileClose,
		EFsOpFileDelete,
		EFsOpFileRead,
		EFsOpFileWrite,
		EFsOpFileSeek,
		EFsOpFileSize,
		EFsOpFileSetSize,
		EFsOpFileSync,
		EFsOpFileDrive,
		EFsOpFileAdopt,
		EFsOpFsClose,
		EFsOpFsConnect,
		EFsOpFsGetSystemDrive,
		EFsOpFsCreatePrivatePath,
		EFsOpFsPrivatePath,
		EFsOpFsVolumeIoParam,
		EFsOpFsEntry,
		EFsOpFsAtt,
		EFsOpFileCreateTemp,
		EFsOpFileAttach,
		//
		EFsOpLast
		};

	TBool   TheFileIoProfileEnabled = EFalse;
	TUint32 TheFileOpCounter[EFsOpLast] = {0};
	TInt64	TheFileWriteAmount = 0;
	TInt64	TheFileReadAmount = 0;
			
	/**
	This function is used only in _SQLPROFILER mode as an appropriate place for:
	 	- setting breakpoints for tracing the file system calls;
	 	- collection information about the number of the file system calls;
	 	
	@param aFsOpType A TFsOpType enum item value, identifying the file system operation that will be executed;
	@param a1 If the operation is "file read" or "file write" - the amount of data read/written;
	
	@internalComponent
	
	@see TFsOpType
	*/
	void FsCallBrkPt(TInt aFsOpType, TInt a1)
		{
		__ASSERT_DEBUG(aFsOpType >= 0 && aFsOpType < EFsOpLast, User::Invariant());
		if(!TheFileIoProfileEnabled)
			{
			return;	
			}
		TFsOpType fsOpType = (TFsOpType)aFsOpType;
		++TheFileOpCounter[fsOpType];
		if(fsOpType == EFsOpFileWrite)
			{
			TheFileWriteAmount += a1;
			}
		else if(fsOpType == EFsOpFileRead)
			{
			TheFileReadAmount += a1;
			}
		}
		
#	define __FS_CALL(aFsOpType, a1) FsCallBrkPt(aFsOpType, a1)

	//////////////////////////////////////////////////////////////////////////////
	////////////////////////// Heap Stats ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	/**
	Memory operation type: alloc, realloc, free. Used during the profiling.
	
	@internalComponent
	*/
	enum TMemOpType
		{
		EMemOpAlloc,
		EMemOpRealloc,
		EMemOpFree,
		//
		EMemOpLast
		};

	TBool   TheMemProfileEnabled = EFalse;//Mem operation call counters and time counters enabled/disabled
	TBool   TheMaxAllocProfileEnabled = EFalse;//Max mem allocation enabled/disabled
	TUint32 TheMemOpCounter[EMemOpLast] = {0};
	TInt64 	TheMemOpTicks[EMemOpLast] = {0};
	TInt64 	TheAllocated = 0;
	TInt64 	TheFreed = 0;
	TInt	TheAllocMax = 0;
		
	/**
	This class is used only in _SQLPROFILER mode as an appropriate place for:
	 	- setting breakpoints for tracing the memory allocation/deallocation calls;
	 	- collection information about the number of the memory allocation/deallocation calls and the time spent in the calls;
	 	
	The constructor's parameters are:	 	
	 - aOpType A TMemOpType enum item value, identifying the operation that will be executed;
	 - aAmt1 The allocated/deallocated size;
	 - aAmt2 Used only if a block of memory is reallocated in which case a2 is the old size of the block;
	
	@internalComponent
	
	@see TMemOpType
	*/
	class TMemCallCounter
		{
	public:			
		TMemCallCounter(TMemOpType aOpType, TInt aAmt1, TInt aAmt2) :
			iOpType(aOpType),
			iStartTicks(0)
			{
			if(TheMaxAllocProfileEnabled && (iOpType == EMemOpAlloc || iOpType == EMemOpRealloc) && aAmt1 > TheAllocMax)
				{
				TheAllocMax = aAmt1;	
				}
			if(TheMemProfileEnabled)
				{
				++TheMemOpCounter[iOpType];
				switch(iOpType)
					{
					case EMemOpAlloc:
						TheAllocated += aAmt1;
						break;
					case EMemOpRealloc:
						TheAllocated += aAmt1;
						TheFreed += aAmt2;
						break;
					case EMemOpFree:
						TheFreed += aAmt1;
						break;
					default:
						__ASSERT_DEBUG(0, User::Invariant());
						break;
					}
				iStartTicks = User::FastCounter();
				}
			}
		~TMemCallCounter()
			{
			if(TheMemProfileEnabled)
				{
				TInt64 diffTicks = (TInt64)User::FastCounter() - (TInt64)iStartTicks;
				if(diffTicks < 0)
					{
					diffTicks = KMaxTUint + diffTicks + 1;
					}
				TheMemOpTicks[iOpType] += diffTicks;
				}
			}
	private:
		TMemOpType	iOpType;
		TUint32 	iStartTicks;
		};
		
#	define __MEM_CALL(aMemOpType, a1, a2) TMemCallCounter memCallCounter(aMemOpType, a1, a2)

	//////////////////////////////////////////////////////////////////////////////
	////////////////////////// OS layer calls ////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	
	//The OS porting layer call types
	enum TOsOpType
		{
		EOsFileClose,
		EOsFileRead,
		EOsFileWrite,
		EOsFileTruncate,
		EOsFileSync,
		EOsFileFileSize,
		EOsFileLock,
		EOsFileUnlock,
		EOsFileCheckReservedLock,
		EOsFileFileControl,
		EOsFileSectorSize,
		EOsFileDeviceCharacteristics,
		//
		EOsVfsOpen,
		EOsVfsDelete,
		EOsVfsAccess,
		EOsVfsFullPathName,
		EOsVfsRandomness,
		EOsVfsSleep,
		EOsVfsCurrentTime,
		EOsVfsGetLastError,
		//
		EOsOpLast
		};
	
	TBool   TheOsProfileEnabled = EFalse;
	TUint32 TheOsOpCounter[EOsOpLast] = {0};//Each entry is a counter - how many times specific OS porting layer function has been called
	
#	define __OS_CALL(aOsOpType, a1, a2) 		\
			do									\
				{								\
				if(TheOsProfileEnabled) 		\
					{							\
					++TheOsOpCounter[aOsOpType];\
					}							\
				}								\
			while(0)				
			
	//////////////////////////////////////////////////////////////////////////////
	////////////////////////// OS layer timings //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	TBool  TheOsCallTimeProfileEnabled = EFalse;//If true, the OS porting layer call timings are enabled.
	TInt64 TheOsCallTicks[EOsOpLast];//Each entry represents the time in ticks spent in a specific OS porting layer function,
									 //disregarding the file type (main or journal)
	
	TBool  TheOsCallTimeDetailedProfileEnabled = EFalse;//If true, the OS porting layer call details are enabled and for each call an entry will be added to the log file (epocwind.out).
	TInt   TheOpCounter = 0;//Operations counter. Each OS porting layer function call increments the counter.
	
	//Used for storing the OS call details:
	// - iType        - on which file the call has been made: main database file - 'M', or journal file - 'J';
	// - iIdentifier  - two letters identifying the monitored OS porting layer function;
	// - iCallCounter - how many times the monitored OS porting layer function has been called;	
	// - iTicksTotal  - the total amount of time in ticks spent in the monitored OS porting layer function;	
	// - iBytesTotal  - the total amount of bytes passed to the monitored OS porting layer function (if it is read or write);
	struct TOsCallProfile
		{
		TOsCallProfile(char aType, const TDesC& aIdentifier) :
			iType(aType),
			iIdentifier(aIdentifier),
			iCallCounter(0),
			iTicksTotal(0),
			iBytesTotal(0)
			{
			}
		void Zero()
			{
			iCallCounter = 0;	
			iTicksTotal = 0;	
			iBytesTotal = 0;
			}
		char	iType;
		TBuf<32> iIdentifier;
		TInt	iCallCounter;
		TInt64	iTicksTotal;
		TInt64	iBytesTotal;
		};

	//An array of TOsCallProfile entries, each entry keeps the profile of a specifc OS porting layer function, when
	//the function was used on the main database file
	TOsCallProfile TheOsCallMProfile[EOsOpLast] = 
		{
		TOsCallProfile('M', _L("Close")), TOsCallProfile('M', _L("Read")), TOsCallProfile('M', _L("Write")), TOsCallProfile('M', _L("Truncate")),
		TOsCallProfile('M', _L("Sync")), TOsCallProfile('M', _L("Size")), TOsCallProfile('M', _L("Lock")), TOsCallProfile('M', _L("Unlock")),
		TOsCallProfile('M', _L("CheckReservedLock")), TOsCallProfile('M', _L("FileControl")), TOsCallProfile('M', _L("SetSize")), TOsCallProfile('M', _L("DeviceCharacteristics")),
		TOsCallProfile('M', _L("Open")), TOsCallProfile('M', _L("Delete")), TOsCallProfile('M', _L("Access")), TOsCallProfile('M', _L("FullPath")),
		TOsCallProfile('M', _L("Randomness")), TOsCallProfile('M', _L("Sleep")), TOsCallProfile('M', _L("CurrentTime")), TOsCallProfile('M', _L("GetLastError"))
		};

	//An array of TOsCallProfile entries, each entry keeps the profile of a specifc OS porting layer function, when
	//the function was used on the journal file
	TOsCallProfile TheOsCallJProfile[EOsOpLast] = 
		{
		TOsCallProfile('J', _L("Close")), TOsCallProfile('J', _L("Read")), TOsCallProfile('J', _L("Write")), TOsCallProfile('J', _L("Truncate")),
		TOsCallProfile('J', _L("Sync")), TOsCallProfile('J', _L("Size")), TOsCallProfile('J', _L("Lock")), TOsCallProfile('J', _L("Unlock")),
		TOsCallProfile('J', _L("CheckReservedLock")), TOsCallProfile('J', _L("FileControl")), TOsCallProfile('J', _L("SetSize")), TOsCallProfile('J', _L("DeviceCharacteristics")),
		TOsCallProfile('J', _L("Open")), TOsCallProfile('J', _L("Delete")), TOsCallProfile('J', _L("Access")), TOsCallProfile('J', _L("FullPath")),
		TOsCallProfile('J', _L("Randomness")), TOsCallProfile('J', _L("Sleep")), TOsCallProfile('J', _L("CurrentTime")), TOsCallProfile('J', _L("GetLastError"))
		};
	
	//The main class for the OS porting layer call profiles.
	class TOsCallCounter
		{
	public:
		//aOsCallTicksEntryRef - a reference to the related TheOsCallTicks[] entry;
		//aProfileRef          - a reference to the related TOsCallProfile object - TheOsCallMProfile[] or TheOsCallJProfile[] entry;
		//aOffset              - file offset in bytes;
		//aBytes               - amount of bytes to be read/written;
		//aOptional			   - might be NULL. Used to print out the name of the file being processed.
		TOsCallCounter(TInt64& aOsCallTicksEntryRef, TOsCallProfile& aOsCallProfileRef, TInt64 aOffset, TInt aBytes, 
				       const sqlite3_file* aHandle, const char* aOptional) :
			iOsCallTicksEntryRef(aOsCallTicksEntryRef),
			iOsCallProfileRef(aOsCallProfileRef),
			iOffset(aOffset),
			iBytes(aBytes),
			iHandle((TUint)aHandle),
			iOptional((const TUint8*)aOptional)
			{
			if(TheOsCallTimeProfileEnabled)
				{
				iStartTicks = User::FastCounter();
				}
			}
		~TOsCallCounter()
			{
			if(TheOsCallTimeProfileEnabled)
				{
				TInt64 diffTicks = (TInt64)User::FastCounter() - (TInt64)iStartTicks;
				if(diffTicks < 0)
					{
					diffTicks = KMaxTUint + diffTicks + 1;
					}
				iOsCallTicksEntryRef += diffTicks;
				if(TheOsCallTimeDetailedProfileEnabled)
					{
					++TheOpCounter;
					++iOsCallProfileRef.iCallCounter;
					iOsCallProfileRef.iTicksTotal += diffTicks;
					iOsCallProfileRef.iBytesTotal += iBytes;
					TFileName fname;
					if(iOptional)
						{
						TPtrC8 fn8(iOptional);
						fname.Copy(fn8);
						}
					//                                           0    1  2  3  4  5   6  7   8   9   10
					RDebug::Print(_L("[SQL-OS]¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬\"%X\"¬%c¬%S¬%d¬%d¬%ld¬%d¬%ld¬%ld¬%ld¬%S\n"),
															//[SQL-OS]
															//Handle
															//Time from start, microseconds
															//Subtype
															//IPC sequence counter
															//IPC call name
					    iHandle,							//sqlite3_file*
						iOsCallProfileRef.iType, 			//1 - main or journal file
						&iOsCallProfileRef.iIdentifier, 	//2 - operation type
						TheOpCounter, 						//3 - Operation counter
						iOsCallProfileRef.iCallCounter, 	//4 - This call type counter
						iOffset, 							//5 - File offset
						iBytes, 							//6 - Data, bytes
						diffTicks, 							//7 - Ticks
						iOsCallProfileRef.iBytesTotal, 		//8 - Data total, bytes
						iOsCallProfileRef.iTicksTotal,		//9 - Ticks total
						&fname);							//10- File name
					}
				}
			}
	private:
		TInt64&			iOsCallTicksEntryRef;
		TOsCallProfile&	iOsCallProfileRef;
		TInt64			iOffset;
		TInt			iBytes;			
		TUint32 		iStartTicks;
		TUint			iHandle;
		const TUint8*	iOptional;
		};
		
	inline TOsCallProfile& OsCallProfile(TBool aType, TInt aIndex)
		{
		return aType ? TheOsCallJProfile[aIndex] : TheOsCallMProfile[aIndex];
		}
		
#	define __OSTIME_COUNTER(aOsCallTicksRef, aOsCallProfileRef, aOffset, aBytes, aHandle, aOpt)	TOsCallCounter osCallCounter(aOsCallTicksRef, aOsCallProfileRef, aOffset, aBytes, aHandle, aOpt)

#else //_SQLPROFILER

#   define __COUNTER_INCR(counter) void(0)
	
#	define __FS_CALL(aFsOpType, a1) void(0)

#	define __MEM_CALL(aMemOpType, a1, a2) void(0)

#	define __OS_CALL(aOpType, a1, a2) void(0)

#	define __OSTIME_COUNTER(aOsCallTicksRef, aOsCallProfileRef, aOffset, aBytes, aHandle, aOpt)	void(0)

#endif//_SQLPROFILER

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       Profiling                          ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _SQLPROFILER

const TInt KMicroSecIn1Sec = 1000000;

TInt FastCounterFrequency()
	{
	TInt ticksPerSec = 0;
	TInt err = HAL::Get(HAL::EFastCounterFrequency, ticksPerSec);
	__ASSERT_ALWAYS(err == KErrNone, __SQLITEPANIC2(ESqliteOsPanicFastCounterFreq));
	return ticksPerSec;
	}

TInt sqlite3SymbianProfilerStart(TInt aCounterType)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aCounterType);
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
			TheFileIoProfileEnabled = ETrue;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCall:
			TheOsProfileEnabled = ETrue;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
			TheOsCallTimeProfileEnabled = ETrue;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			TheOsCallTimeProfileEnabled = ETrue;
			TheOsCallTimeDetailedProfileEnabled = ETrue;
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			TheMemProfileEnabled = ETrue;
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			TheMaxAllocProfileEnabled = ETrue;
			break;
		default:
			return KErrNotSupported;
		}
	return KErrNone;
	}
	
TInt sqlite3SymbianProfilerStop(TInt aCounterType)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aCounterType);
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
			TheFileIoProfileEnabled = EFalse;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCall:
			TheOsProfileEnabled = EFalse;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
			TheOsCallTimeProfileEnabled = EFalse;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			TheOsCallTimeDetailedProfileEnabled = EFalse;
			TheOsCallTimeProfileEnabled = EFalse;
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			TheMemProfileEnabled = EFalse;
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			TheMaxAllocProfileEnabled = EFalse;
			break;
		default:
			return KErrNotSupported;
		}
	return KErrNone;
	}
	
TInt sqlite3SymbianProfilerReset(TInt aCounterType)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aCounterType);
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
			Mem::FillZ(TheFileOpCounter, sizeof(TheFileOpCounter));
			TheFileWriteAmount = TheFileReadAmount = 0;
			break;
		case TSqlResourceProfiler::ESqlCounterOsCall:
			Mem::FillZ(TheOsOpCounter, sizeof(TheOsOpCounter));
			break;
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			TheOpCounter = 0;
			Mem::FillZ(TheOsCallTicks, sizeof(TheOsCallTicks));
			for(TInt i=0;i<EOsOpLast;++i)
				{
				TheOsCallMProfile[i].Zero();
				TheOsCallJProfile[i].Zero();
				}
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			Mem::FillZ(TheMemOpCounter, sizeof(TheMemOpCounter));
			Mem::FillZ(TheMemOpTicks, sizeof(TheMemOpTicks));
			TheAllocated = TheFreed = 0;
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			TheAllocMax = 0;
			break;
		default:
			return KErrNotSupported;
		}
	return KErrNone;
	}
	
TInt sqlite3SymbianProfilerQuery(TInt aCounterType, TDes8& aResult)
	{
	const TSqlResourceProfiler::TSqlCounter KCounterType = static_cast <TSqlResourceProfiler::TSqlCounter> (aCounterType);
	switch(KCounterType)
		{
		case TSqlResourceProfiler::ESqlCounterFileIO:
			for(TInt i=0;i<EFsOpLast;++i)
				{
				aResult.AppendNum(TheFileOpCounter[i]);
				aResult.Append(TChar(';'));
				}
			aResult.AppendNum(TheFileWriteAmount);
			aResult.Append(TChar(';'));
			aResult.AppendNum(TheFileReadAmount);
			aResult.Append(TChar(';'));
			break;
		case TSqlResourceProfiler::ESqlCounterOsCall:
			for(TInt i=0;i<EOsOpLast;++i)
				{
				aResult.AppendNum(TheOsOpCounter[i]);
				aResult.Append(TChar(';'));
				}
			break;
		case TSqlResourceProfiler::ESqlCounterOsCallTime:
		case TSqlResourceProfiler::ESqlCounterOsCallDetails:
			{
			TInt ticksPerSec = FastCounterFrequency();
			for(TInt i=0;i<EOsOpLast;++i)
				{
				TInt64 osCallTimeUs = (TheOsCallTicks[i] * KMicroSecIn1Sec) / ticksPerSec;
				aResult.AppendNum(osCallTimeUs);
				aResult.Append(TChar(';'));
				}
			}
			break;
		case TSqlResourceProfiler::ESqlCounterMemory:
			aResult.AppendNum(TheMemOpCounter[EMemOpAlloc]);
			aResult.Append(TChar(';'));
			aResult.AppendNum(TheMemOpCounter[EMemOpRealloc]);
			aResult.Append(TChar(';'));
			aResult.AppendNum(TheMemOpCounter[EMemOpFree]);
			aResult.Append(TChar(';'));
			aResult.AppendNum(TheAllocated);
			aResult.Append(TChar(';'));
			aResult.AppendNum(TheFreed);
			aResult.Append(TChar(';'));
			{
			TInt ticksPerSec = FastCounterFrequency();
			TInt64 memOpCallTimeUs = (TheMemOpTicks[EMemOpAlloc] * KMicroSecIn1Sec) / ticksPerSec;
			aResult.AppendNum(memOpCallTimeUs);
			aResult.Append(TChar(';'));
			memOpCallTimeUs = (TheMemOpTicks[EMemOpRealloc] * KMicroSecIn1Sec) / ticksPerSec;
			aResult.AppendNum(memOpCallTimeUs);
			aResult.Append(TChar(';'));
			memOpCallTimeUs = (TheMemOpTicks[EMemOpFree] * KMicroSecIn1Sec) / ticksPerSec;
			aResult.AppendNum(memOpCallTimeUs);
			aResult.Append(TChar(';'));
			}
			break;
		case TSqlResourceProfiler::ESqlCounterMaxAlloc:
			aResult.AppendNum(TheAllocMax);
			aResult.Append(TChar(';'));
			break;
		default:
			return KErrNotSupported;
		}
	return KErrNone;
	}
	
#else//_SQLPROFILER	

TInt sqlite3SymbianProfilerStart(TInt)
	{
	return KErrNotSupported;	
	}

TInt sqlite3SymbianProfilerStop(TInt)
	{
	return KErrNotSupported;	
	}

TInt sqlite3SymbianProfilerReset(TInt)
	{
	return KErrNotSupported;	
	}

TInt sqlite3SymbianProfilerQuery(TInt, TDes8&)
	{
	return KErrNotSupported;	
	}

#endif//_SQLPROFILER

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Os2SqliteErr() is called at the end of many of the interface functions of the OS porting layer (wherever it is appropriate - 
TFileIo and TVfs interfaces). The purpose of this function is to identify the "out of memory" and "disk is full" errors
reported by the used Symbian OS APIs (aOsErr parameter) and report them to SQLite as SQLITE_FULL and SQLITE_NOMEM errors.
The KErrEof error (TFileIo::Read() can return KErrEof) is reported to SQLite as SQLITE_IOERR_SHORT_READ. The rest of failures
are reported as the error specified in aDefaultErr parameter.
 
@param aOsErr      Symbian OS error
@param aDefaultErr The default SQLite error that should be used if the aOsErr parameter is not one of:
                     KErrNone, KErrEof, KErrNoMemory, KErrDiskFull
@return SQLITE_OK,               The OS porting layer function call has completed successfully, 
          SQLITE_IOERR_SHORT_READ, The amount of the data read is less than the requested amount,
          SQLITE_IOERR_NOMEM,      Out of memory,
          SQLITE_FULL,             The disk is full,
          aDefaultErr,             The rest of failures will be reported as aDefaultErr.
*/
static TInt Os2SqliteErr(TInt aOsErr, TInt aDefaultErr)
	{
	switch(aOsErr)
		{
		case KErrNone:
			return SQLITE_OK;	
		case KErrEof:
			return SQLITE_IOERR_SHORT_READ;
		case KErrNoMemory:
			return SQLITE_IOERR_NOMEM;
		case KErrDiskFull:
			return SQLITE_FULL;
		default:
#ifdef _DEBUG		
			RDebug::Print(_L("SQLite OS porting layer, Os2SqliteErr(), err=%d\n"), aOsErr);
			SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, OS2SQLITEERR, "OS;0;Os2SqliteErr;aOsErr=%d", aOsErr));
#endif			
			break;
		}
	return aDefaultErr;
	}
	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       COsLayerData class declaration   //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
COsLayerData singleton class manages a single SQLite OS layer copy of the following data: 
@code
 - file session instance;
 - process's private data path, where the temporary file will be stored (on the system drive);
 - the last OS error code, every Symbian OS API call (file system calls, etc.) will set it;
 - the stored OS error code, initialized with the last OS error code only if stored OS error code is KErrNone.
	Each StoredOsErrorCode() call will reset it to KErrNone;
	The reason of having two data members for storing the OS error codes is that if there is just one variable
	and it gets initialized with the error value reported by a failed OS API call, the next successful OS API
	call will reset it and the COsLayerData client will miss the last "real" OS API error.
 - A pointer to the current RMessage2 object, if the current operation is "open a private secure database";
 - A boolean flag - iReadOnly - used only for private secure databases, indicating whether the database is read-only or not;
@endcode

@internalComponent
*/
NONSHARABLE_CLASS(COsLayerData)
	{
public:	
	static TInt Create();
	static inline void Destroy();
	static inline COsLayerData& Instance();
	
	inline TInt SetOsErrorCode(TInt aError);
	inline TInt StoredOsErrorCode();
	
	inline void StoreFhData(const RMessage2* aMsg, TBool aReadOnly);
	inline void RetrieveAndResetFhData(const RMessage2*& aMsg, TBool& aReadOnly);
	
private:
	inline COsLayerData();
	inline ~COsLayerData();
	TInt DoCreate();
	
public:
	RFs			iFs;		//File session instance.
	TFileName	iSysPrivDir;//"<system drive>:\" + process's private data path. Initialized in sqlite3SymbianFsOpen().
	TInt64		iSeed;

private:	
	static COsLayerData* 	iOsLayerData;
	TInt					iStoredOsErrorCode;	//Contains the last OS error code.
	const RMessage2* 		iMessage;			//Fh data
	TBool					iReadOnly;			//Fh data
	};

/**
This functon returns a reference to the current thread allocator object.
The static RAllocator& variable will be initialized once at the moment when the function is called for 
first time. 
*/
static RAllocator& Allocator()
    {
    static RAllocator& allocator = User::Allocator();
    return allocator;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////       TDbFile struct declaration      /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
TDbFile derives from the sqlite3_file structure, adding data members needed for processing the SQLite requests to the OS layer.
When SQLite needs an access to a file, SQLite allocates memory for a new TDbFile instance and passes a pointer to that 
instance to TVfs::Open(). TVfs::Open() creates/opens the file and initializes the TDbFile instance. 
SQLite uses the initialized TDbFile instance (actually SQLite knows and uses the sqlite3_file, the base structure) 
every time when needs to read or write from/to the file, using for that an appropriate TFileIo method.

Note: currently RFileBuf64 object is used instead of RFile64. That improves the read/write file performance.

No virtual methods here! sqlite3_file contains data members. If a virtual method is added, that will shift the offset of the
data members from the beginning of the sqlite3_file  object by 4 bytes. This is not what SQLite (C code) expects.

@internalComponent

@see TVfs
@see TFileIo
@see TVfs::Open()
*/
NONSHARABLE_STRUCT(TDbFile) : public sqlite3_file 
	{
	inline TDbFile();
	RFileBuf64	iFileBuf;
	TInt		iLockType;				//File lock type
	TBool		iReadOnly;				//True if the file is read-only
	TInt		iSectorSize;			//Media sector-size
	TInt		iDeviceCharacteristics;
	TSqlFreePageCallback iFreePageCallback;
	TBool       iIsFileCreated;          //If the file to be created is a temp file, 
                                       //it will not be created until the data is to be written to.
#ifdef _SQLPROFILER
	TBool		iIsJournal;
#endif	
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////       TFileIo class declaration      //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
TFileIo class offers static methods for performing operations on a file.
Every TFileIo method has a pointer to a sqlite3_file instance (so, a TDbFile instance) as its first argument.

SQLite never accesses the file system directly, only through function pointers, data members of the sqlite3_io_methods structure.
The OS porting layer defines a single instance of sqlite3_io_methods structure, TheFileIoApi, and uses the TFileIo to initialize the 
sqlite3_io_methods data members (function pointers).
Every time when SQLite creates/opens a file using TVfs::Open(), TVfs::Open() will pass back to SQLite a pointer to the single
initialized sqlite3_io_methods instance (TheFileIoApi) that will be used later by SQLite for accessing the file.

@internalComponent

@see TVfs
@see TVfs::Open()
@see TheFileIoApi
@see TDbFile
*/
NONSHARABLE_CLASS(TFileIo)
	{
public:	
	static int Close(sqlite3_file* aDbFile);
	static int Read(sqlite3_file* aDbFile, void* aBuf, int aAmt, sqlite3_int64 aOffset);
	static int Write(sqlite3_file* aDbFile, const void* aData, int aAmt, sqlite3_int64 aOffset);
	static int Truncate(sqlite3_file* aDbFile, sqlite3_int64 aLength);
	static int Sync(sqlite3_file* aDbFile, int aFlags);
	static int FileSize(sqlite3_file* aDbFile, sqlite3_int64* aSize);
	static int Lock(sqlite3_file* aDbFile, int aLockType);
	static int Unlock(sqlite3_file* aDbFile, int aLockType);
	static int CheckReservedLock(sqlite3_file* aDbFile, int *aResOut);
	static int FileControl(sqlite3_file* aDbFile, int aOp, void* aArg);
	static int SectorSize(sqlite3_file* aDbFile);
	static int DeviceCharacteristics(sqlite3_file* aDbFile);
private:
	static TInt DoCreateTempFile(TDbFile& aDbFile);
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////       TVfs class declaration      /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
TVfs ("VFS" - virtual file system) class offers methods for creating/openning a file, deleting a file,
a "sleep" method, a "time" method, a "rand" method, etc.
SQLite never accesses the OS API directly, only through the API offered by TVfs and TFileIo classes.

@internalComponent

@see TFileIo
@see TheVfsApi
@see COsLayerData
*/
NONSHARABLE_CLASS(TVfs)
	{
public:		
	static int Open(sqlite3_vfs* aVfs, const char* aFileName, sqlite3_file* aDbFile, int aFlags, int* aOutFlags);
	static int Delete(sqlite3_vfs* aVfs, const char* aFileName, int aSyncDir);	
	static int Access(sqlite3_vfs* aVfs, const char* aFileName, int aFlags, int* aResOut);
	static int FullPathName(sqlite3_vfs* aVfs, const char* aRelative, int aBufLen, char* aBuf);
	static int Randomness(sqlite3_vfs* aVfs, int aBufLen, char* aBuf);
	static int Sleep(sqlite3_vfs* aVfs, int aMicrosec);
	static int CurrentTime(sqlite3_vfs* aVfs, double* aNow);
	static int GetLastError(sqlite3_vfs *sVfs, int aBufLen, char* aBuf);
	static TInt DoGetDeviceCharacteristicsAndSectorSize(TDbFile& aDbFile, TInt& aRecReadBufSize);
private:
	static TInt DoOpenFromHandle(TDbFile& aDbFile, const RMessage2& aMsg, TBool aReadOnly);
	static inline TInt DoGetVolumeIoParamInfo(RFs& aFs, TInt aDriveNo, TVolumeIOParamInfo& aVolumeInfo);
	static TInt DoGetDeviceCharacteristics(const TDriveInfo& aDriveInfo, const TVolumeIOParamInfo& aVolumeInfo);
	static TInt DoGetSectorSize(const TDriveInfo& aDriveInfo, const TVolumeIOParamInfo& aVolumeInfo);
	static TInt DoFileSizeCorruptionCheck(TDbFile& aDbFile, const TDesC& aFname, TInt aFmode);
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       Global variables, constants    ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The following macro specifies the size of the RFileBuf64 file buffer in KB:
//    __SQLITE_OS_SYMBIAN_FILEBUF_KBSIZE__
// 
// If not set, a default value of 8 is used	
#if !defined(__SQLITE_OS_SYMBIAN_FILEBUF_KBSIZE__)
#define __SQLITE_OS_SYMBIAN_FILEBUF_KBSIZE__ 8
#endif	
const TInt KFileBufSize = __SQLITE_OS_SYMBIAN_FILEBUF_KBSIZE__ * 1024;	

/**
Pointer to the single COsLayerData instance.

@see COsLayerData

@internalComponent
*/
COsLayerData* COsLayerData::iOsLayerData = NULL;

/**
Single sqlite3_io_methods instance, which data members (function pointers) are initialized with the addresses of
TFileIo members. 
TheFileIoApi is used by SQLite for performing OS independent file I/O.

@see TFileIo
@see TVfs

@internalComponent
*/
static sqlite3_io_methods TheFileIoApi = 
	{
	1,						//Version
	&TFileIo::Close,
	&TFileIo::Read,
	&TFileIo::Write,
	&TFileIo::Truncate,
	&TFileIo::Sync,
	&TFileIo::FileSize,
	&TFileIo::Lock,
	&TFileIo::Unlock,
	&TFileIo::CheckReservedLock,
	&TFileIo::FileControl,
	&TFileIo::SectorSize,
	&TFileIo::DeviceCharacteristics
	};

/**
Single sqlite3_vfs instance, which data members (function pointers) are initialized with the addresses of
TVfs members. TheVfsApi also keeps information regarding some other OS dependend characteristics like 
the TDbFile size and max file name length.
TheVfsApi is used by SQLite for accessing needed OS API.

TheVfsApi can't be a constant definition. SQLite expects the "sqlite3_vfs" object to be a R/W one, because
SQLite may have and use a chain of sqlite3_vfs instances.

@see TVfs
@see TTFileIo
@see TDbFile

@internalComponent
*/
static sqlite3_vfs TheVfsApi = 
	{
	1,					//iVersion
	sizeof(TDbFile),	//szOsFile
	KMaxFileName,		//mxPathname
	0,					//pNext
	"SymbianSql",		//zName
	0,					//pAppData
	&TVfs::Open,
	&TVfs::Delete,
	&TVfs::Access,
	&TVfs::FullPathName,
	0,
	0,
	0,
	0,
	&TVfs::Randomness,
	&TVfs::Sleep,
	&TVfs::CurrentTime,
	&TVfs::GetLastError
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       COsLayerData class definition    //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Creates a single COsLayerData instance.

@return KErrNone,          The operation has completed succesfully;
	    KErrNoMemory,      Out of memory condition has occured;
                           Note that other system-wide error codes may also be returned.
                           
@panic Sqlite 16 In _DEBUG mode - the COsLayerData instance has been created already.
*/
/* static */ TInt COsLayerData::Create()
	{
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, COSLAYERDATA_CREATE_ENTRY, "OS-Entry;0;COsLayerData::Create"));
	__ASSERT_DEBUG(!COsLayerData::iOsLayerData, __SQLITEPANIC2(ESqliteOsPanicOsLayerDataExists));
	COsLayerData::iOsLayerData = new COsLayerData;
	if(!COsLayerData::iOsLayerData)
		{
		SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, COSLAYERDATA_CREATE_EXIT1, "OS-Exit;0;COsLayerData::Create;err=KErrNoMemory"));
		return KErrNoMemory;	
		}
	TInt err = COsLayerData::iOsLayerData->DoCreate();
	if(err != KErrNone)
		{
		delete COsLayerData::iOsLayerData;
		COsLayerData::iOsLayerData = NULL;
		SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, COSLAYERDATA_CREATE_EXIT2, "OS-Exit;0;COsLayerData::Create;err=%d", err));
		return err;
		}
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, COSLAYERDATA_CREATE_EXIT3, "OS-Exit;0;COsLayerData::Create;err=KErrNone"));
	return KErrNone;
	}

/**
Destroys the COsLayerData instance.
*/
/* static */ inline void COsLayerData::Destroy()
	{
	delete COsLayerData::iOsLayerData;
	COsLayerData::iOsLayerData = NULL;
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, COSLAYERDATA_DESTROY, "OS;0;COsLayerData::Destroy"));
	}

/**
Returns a reference to the single COsLayerData instance.

@panic Sqlite 1 In _DEBUG mode if the COsLayerData instance is NULL.
*/
/* static */ inline COsLayerData& COsLayerData::Instance()
	{
	__ASSERT_DEBUG(COsLayerData::iOsLayerData != NULL, __SQLITEPANIC2(ESqliteOsPanicNullOsLayerDataPtr));
	return *COsLayerData::iOsLayerData;
	}

/**
Sets the last OS error code data member. The stored OS error code data member will be set only if it is
KErrNone. (If it is not KErrNone it means that its value has not been accessed yet)
An exception from the rule described above is KErrDiskFull error which, if happens, will be set always, because
this error has a special meaning for the database clients - special actions may have to be taken if the
disk is full.

@param aError The OS error code
@return The OS error code
*/
inline TInt COsLayerData::SetOsErrorCode(TInt aError)
	{
	if(iStoredOsErrorCode == KErrNone || aError == KErrDiskFull)
		{
		iStoredOsErrorCode = aError;
		}
	return aError;
	}

/**
Returns the last stored OS error code, which was stored by SetOsErrorCode() call.
The function also resets the stored OS error code to KErrNone.

@return The last stored OS error code
*/
inline TInt COsLayerData::StoredOsErrorCode()
	{
	TInt err = iStoredOsErrorCode;
	iStoredOsErrorCode = KErrNone;
	return err;
	}

/**
Stores the RMessage2 object address, file and file session handles and the read-only flag for later use when SQLite issues a 
request for open the database file (private secure database). 

The aMsg argument of the function can be NULL, because this fucntion is also used to reset the stored "file handle" data.

How this function is used:
1) When the SQL server receives a request to establish a connection with private secure database, the SQL server
   will add additional information to the private secure database file name, such as: 
   	- the file handle (the private secure database is opened by the client side dll - sqldb.dll);
   	- a pointer to the RMessage2 object used in this request;
2) The passed additional information will be used for adopting the file handle by calling RFile64::AdoptFromClient().
3) Before calling TVfs::Open() to establish a connection with the database, SQLite will call TVfs::FullPathName()
   to retrieve the database file full path
4) TVfs::FullPathName() will detect that the file name contains an additional information and will extraxt the information
   calling COsLayerData::StoreFhData().
5) After TVfs::FullPathName() SQLite calls TVfs::Open() where the extracted information will be used for adopting the file handle

@param aMsg A pointer to the current RMessage2 object
@param aReadOnly True if the private secure database is read-only
*/
inline void COsLayerData::StoreFhData(const RMessage2* aMsg, TBool aReadOnly)
	{
	iMessage = aMsg;
	iReadOnly = aReadOnly;
	}

/**
Retrieves the RMessage2 object, file and file session handles. The stored data will be reset.
This function is used by TVfs::Open(), when a request for opening a secure private database is processed.

@param aMsg Output parameter. A reference to a RMessage2 pointer, which will be initialized with the stored RMessage2 pointer.
@param aReadOnly Output parameter. The store read-only flag value will be set there.

@panic Sqlite 13 In _DEBUG mode - aMsg is NULL.
*/
inline void COsLayerData::RetrieveAndResetFhData(const RMessage2*& aMsg, TBool& aReadOnly)
	{
	__ASSERT_DEBUG(iMessage != NULL, __SQLITEPANIC(ESqliteOsPanicInvalidFhData));
	aMsg = iMessage; 
	aReadOnly = iReadOnly;
	iMessage = NULL;
	}

/**
Initializes the COsLayerData data members with their default values.
*/
inline COsLayerData::COsLayerData() :
	iStoredOsErrorCode(KErrNone),
	iMessage(0),
	iReadOnly(EFalse)
	{
	TTime now;
	now.UniversalTime();
	iSeed = now.Int64();
	}

/**
Destroys the COsLayerData instance.

Note: No SQLite functions should be called inside the destructor, because SQLite is already shutdown-ed!
*/
inline COsLayerData::~COsLayerData()
	{
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, COSLAYERDATA_COSLAYERDATA2, "OS;0;COsLayerData::~COsLayerData;iFs.Handle()=0x%X", (TUint)iFs.Handle()));
	__FS_CALL(EFsOpFsClose, 0);
	iFs.Close();	
	}

/**
Creates a file session instance.	

Creates the private path, where the temporary files will be stored (on the system drive).

In a case of a failure COsLayerData::DoCreate() does not close the file session.
This will be made in the calling function - COsLayerData::Create().

Note: No SQLite functions should be called inside the DoCreate() implementation, because SQLite is not initialized yet!

@return KErrNone,          The operation has completed succesfully;
		KErrGeneral		   The registration of TheVfsApi has failed;
	    KErrNoMemory,      Out of memory condition has occured;
                           Note that other system-wide error codes may also be returned.

@see TVfs
@see TheVfsApi
*/
TInt COsLayerData::DoCreate()
	{
	__FS_CALL(EFsOpFsConnect, 0);
	TInt err = iFs.Connect();
	if(err != KErrNone)
		{
		return err;	
		}
	//Get the system drive
	__FS_CALL(EFsOpFsGetSystemDrive, 0);
	TInt sysDrive = static_cast<TInt>(RFs::GetSystemDrive());
	__FS_CALL(EFsOpFsCreatePrivatePath, 0);
	if((err = iFs.CreatePrivatePath(sysDrive)) != KErrNone && err != KErrAlreadyExists)
		{
		return err;	
		}
	TFileName privateDir;
	__FS_CALL(EFsOpFsPrivatePath, 0);
	if((err = iFs.PrivatePath(privateDir)) != KErrNone)
		{
		return err;	
		}
	TDriveUnit drive(sysDrive);
	TDriveName driveName = drive.Name();
	TParse parse;
	(void)parse.Set(driveName, &privateDir, 0);//this call can't fail
	iSysPrivDir.Copy(parse.DriveAndPath());
	//Create the temp files directory
	(void)parse.AddDir(KTempFileDir);//this call can't fail
	err = iFs.MkDir(parse.DriveAndPath());
    if(err != KErrNone && err != KErrAlreadyExists)
    	{
		return err;
    	}
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, COSLAYERDATA_DOCREATE, "OS;0;COsLayerData::DoCreate;iFs.Handle()=0x%X;sysDrive=%d;iSysPrivDir=%S", (TUint)iFs.Handle(), sysDrive, __SQLITEPRNSTR(iSysPrivDir)));
	return KErrNone;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////     Symbian OS specific functions (called by the SQL server)        ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Returns the last OS error which occured durring the operations with the database files.
The per-thread variable, where the last OS error is hold, will be set to KErrNone.

This function is part of Symbian OS specific SQLITE API.

@return The last OS error.
@internalComponent
*/
TInt sqlite3SymbianLastOsError(void)
	{
	return COsLayerData::Instance().StoredOsErrorCode();
	}

/**
This function must be called once before any other SQLITE API call. 
The function:
@code
 - Initializes the OS poting layer;
 - Initializes the SQLite library;
@endcode

This function is part of the Symbian OS specific SQLITE API.

@return Symbian OS specific error code, including KErrNoMemory.

@internalComponent
*/
TInt sqlite3SymbianLibInit(void)
	{
	TInt osErr = COsLayerData::Create();
	if(osErr != KErrNone)
		{
		return osErr;
		}
	osErr = KErrNone;
	TInt sqliteErr = sqlite3_initialize();
	if(sqliteErr != SQLITE_OK)
		{
		osErr = sqliteErr == SQLITE_NOMEM ? KErrNoMemory : KErrGeneral;
		COsLayerData::Destroy();
		}
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, SQLITE3SYMBIANLIBINIT, "OS;0;sqlite3SymbianLibInit;osErr=%d", osErr));
	return osErr;
	}

/**
This function must be called once after finishing working with sqlite.
The function:
@code
 - Shuts down the SQLite library;
 - Releases the allocated by the OS porting layer resources;
@endcode

This function is part of the Symbian OS specific SQLITE API.

@internalComponent
*/
void sqlite3SymbianLibFinalize(void)
	{
	(void)sqlite3_shutdown();
	COsLayerData::Destroy();
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, SQLITE3SYMBIANLIBFINALIZE, "OS;0;sqlite3SymbianLibFinalize"));
	}

/**
This function is part of Symbian OS specific SQLITE API.

@return A reference to RFs instance used for sqlite file I/O operations.
@internalComponent
*/
RFs& sqlite3SymbianFs(void)
	{
	return COsLayerData::Instance().iFs;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////          UTF16<-->UTF8, conversion functions    ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The function converts aFileName to UTF16 encoded file name, and stores the UTF16 encoded file name
to the place pointed by aFileNameDestBuf argument.
If the UTF16 conversion of the file name failed because the file name is too long or NULL, 
the function returns EFalse. 

@param aFileName Expected to point to UTF8 encoded, zero terminated string.
				 Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
@param aFileNameDestBuf Output parameter. Will hold UTF16, non-zero-terminated string.
						The max length must be at least KMaxFileName characters.
                         
@return True if the conversion has been completed successfully						 
*/
static TBool ConvertToUnicode(const char *aFileName, TDes& aFileNameDestBuf)
	{
	if(aFileName)
		{
		wchar_t* dest = reinterpret_cast <wchar_t*> (const_cast <TUint16*> (aFileNameDestBuf.Ptr()));
		TInt len = mbstowcs(dest, aFileName, aFileNameDestBuf.MaxLength());
		__ASSERT_DEBUG(len >= 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
		//If len == aFileNameDestBuf.MaxLength(), then the output buffer is too small.
		if(len < aFileNameDestBuf.MaxLength())
			{
			aFileNameDestBuf.SetLength(len);
			return ETrue;
			}
		}
	return EFalse;
	}

/**
The function converts aFileName to UTF8 encoded file name, and stores the UTF8 encoded file name
to the place pointed by aFileNameDestBuf argument.
If the UTF8 conversion of the file name failed because the file name is too long or NULL, 
the function returns EFalse. 

@param aFileName Expected to point to UTF16 encoded, zero terminated string.
				 Max allowed aFileName length is KMaxFileName (excluding terminating 0 character).
@param aFileNameDestBuf Output parameter. Will hold UTF8, non-zero-terminated string.
						The max length must be at least KMaxFileName characters.
                         
@return True if the conversion has been completed successfully						 
*/
static TBool ConvertFromUnicode(const TDesC& aFileName, TDes8& aFileNameDestBuf)
	{
	char* dest = reinterpret_cast <char*> (const_cast <TUint8*> (aFileNameDestBuf.Ptr()));
	const wchar_t* src = reinterpret_cast <const wchar_t*> (aFileName.Ptr());
	TInt len = wcstombs(dest, src, aFileNameDestBuf.MaxLength());
	__ASSERT_DEBUG(len >= 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
	//If len == aFileNameDestBuf.MaxLength(), then the output buffer is too small.
	if(len < aFileNameDestBuf.MaxLength())
		{
		aFileNameDestBuf.SetLength(len);
		return ETrue;
		}
	return EFalse;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////          File name, containing handles, functions   ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char KFhSeparator = '|';	//The symbol, which when used in the file name means that the string does not contain a real file name but file handles
const TInt KFhSessHandleIdx = 2;//The index of the file session handle in RMessage2 object
const TInt KFhFileHandleIdx = 3;//The index of the file handle in RMessage2 object
const TInt KFhMarkPos = 0;		//if the symbol in this position is KFhSeparator, then the string contains file handles
const TInt KFhRoPos = 1;		//read-only flag position in the string
const TInt KFhMsgAddrPos = 2;	//RMessage2 address position in the string
const TInt KFhMsgAddrLen = 8;	//RMessage2 address length
//const TInt KFhDrivePos = 1;	//Drive position in the string (after removing the read-only flag and RMessage2 object's address)

/**
File name string types:
@code
	- ENotFhStr 	- The string does not contain file handles;
	- EFhStr		- The string contain file handles, but is not main db file;
	- EFhMainDbStr	- The string contain file handles and is the main db file;
@endcode

Every file name passed to the OS porting layer's TVfs::Open() method falls into one of the following three categories:
ENotFhStr - the file name does not contain handles, EFhStr - the file name contains handles but is not a name of a private
secure database, EFhMainDbStr - the file name contains handles and is a name of a private secure database.

@see TVfs::Open()
@see FhStringProps()

@internalComponent
*/
enum TFhStrType
	{
	ENotFhStr,						//The string does not contain file handles
	EFhStr,							//The string contain file handles, but is not main db file
	EFhMainDbStr					//The string contain file handles and is the main db file
	};

/**
The TVfs::Open() implementation uses this function to determine the type of the file name, which can be 
one of the TFhStrType enum item values.

@param aFileName Zero-terminated, UTF8 encoded file name.
@return The file name type, one of the TFhStrType enum item values.

@see TVfs::Open()
@see TFhStrType

@internalComponent
*/
static TFhStrType FhStringProps(const char* aFileName)
	{
	char* first = strchr(aFileName, KFhSeparator);
	if(!first)
		{
		return ENotFhStr;
		}
	char* last = strchr(first + 1, KFhSeparator);
	if(!last)
		{
		return ENotFhStr;
		}
	return *(last + 1) == 0 ? EFhMainDbStr : EFhStr;
	}

/**
Removes all invalid characters in aFileName.
If the file name contains handles (so that's a private secure database related name), the additional
information (handles, flags, object addresses) has to be excluded from the name in order to make it usable 
by the file system. 

The private file name format is (see FhExtractAndStore() comments):

"|<R/O flag><RMessage2 pointer><drive><app SID><file_name><file_ext>|"

Before opening or creating a file, SQLite will call TVfs::FullPathName() passing to the function the name of
the file and expecting the full file name (including path) as an output from the function.
After the TVfs::FullPathName() call, the full file name, returned to SQLite, will have the following format:

"|<drive><path><file_name><file_ext>|"

FhConvertToFileName() is called from TVfs::Open() and will remove the leading and trialing '|' characters from
the file name.

@param aFileName Input/Output parameter. The cleaned file name will be copied there.
@param aPrivateDir The SQL server private data cage.

@see TVfs::Open()
@see TFhStrType
@see FhStringProps()

@internalComponent
*/
static void FhConvertToFileName(TDes& aFileName, const TDesC& aPrivateDir)
	{
	TInt firstPos = aFileName.Locate(TChar(KFhSeparator));
	if(firstPos >= 0)
		{
		aFileName.Delete(firstPos, 1);
		TInt lastPos = aFileName.LocateReverse(TChar(KFhSeparator));
		if(lastPos >= 0)
			{
			aFileName.Delete(lastPos, 1);
			TParse parse;
			(void)parse.Set(aFileName, &aPrivateDir, 0);//the file name should be verified by the server
			aFileName.Copy(parse.FullName());
			}
		}
	}

/**
Extracts the read-only flag and RMessage address from aDbFileName and stores them in the single COsLayerData instance.

@param aDbFileName Input/output parameter. The file name. 
				   It will be reformatted and won't contain the already extracted data.
				   The aDbFileName format is:
@code
      				|<R/O flag><RMessage2 pointer><drive><app SID><file_name><file_ext>|
@endcode

@see TVfs::Open()
@see TFhStrType
@see FhStringProps()

@internalComponent

@panic Sqlite 12 In _DEBUG mode - invalid position of the "|" character in the file name.
@panic Sqlite 12 In _DEBUG mode - no RMessage2 pointer can be extracted from the file name.
@panic Sqlite 12 In _DEBUG mode - the extracted RMessage2 pointer is NULL.
*/
static void FhExtractAndStore(TDes& aDbFileName)
	{
	TInt fhStartPos = aDbFileName.Locate(TChar(KFhSeparator));
	__ASSERT_DEBUG(fhStartPos == KFhMarkPos, __SQLITEPANIC2(ESqliteOsPanicInvalidFhStr));
	//If this file name string contains file handles
	if(fhStartPos == KFhMarkPos)
		{
		//Extract from aDbFileName string RMessage2 object's address
		TLex lex(aDbFileName.Mid(fhStartPos + KFhMsgAddrPos, KFhMsgAddrLen));
		TUint32 addr;
		TInt err = lex.Val(addr, EHex);
		__ASSERT_DEBUG(err == KErrNone, __SQLITEPANIC2(ESqliteOsPanicInvalidFhStr));
		if(err == KErrNone)
			{
			//Cast the address to RMessage2 pointer.
			const RMessage2* msg = reinterpret_cast <const RMessage2*> (addr);
			__ASSERT_DEBUG(msg != NULL, __SQLITEPANIC2(ESqliteOsPanicInvalidFhStr));
			if(msg)
				{
				//Store the data from aDbFileName in the single COsLayerData instance.
				TBool readOnly = aDbFileName[fhStartPos + KFhRoPos] > '0';
				COsLayerData::Instance().StoreFhData(msg, readOnly);
				//Remove: read-only flag and RMessage2 object's address
				aDbFileName.Delete(fhStartPos + KFhRoPos, 1 + KFhMsgAddrLen);
				}
			}
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       TDbFile class definition    ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes TDbFile data members with their default values.
*/
inline TDbFile::TDbFile() :
	iFileBuf(KFileBufSize),
	iLockType(SQLITE_LOCK_NONE),
	iReadOnly(EFalse),
	iSectorSize(0),
	iDeviceCharacteristics(-1),
	iIsFileCreated(ETrue)
	{
#ifdef _SQLPROFILER
	iIsJournal = EFalse;
#endif
	pMethods = 0;
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, TDBFILE_TDBFILE, "OS;0x%X;TDbFile::TDbFile", (TUint)this));
	}

/**
Casts the passed sqlite3_file pointer to a reference to the derived class - TDbFile&.
All sqlite3_file pointers passed to TFileIo methods are actually pointers to TDbFile instances. 
So the cast is safe.

@param aDbFile A pointer to a sqlite3_file instance

@return A TDbFile reference. 
@see TFileIo
@see TVfs
@see TDbFile

@panic Sqlite 20 In _DEBUG mode if aDbFile is NULL.

@internalComponent
*/
static inline TDbFile& DbFile(sqlite3_file* aDbFile)
	{
	__ASSERT_DEBUG(aDbFile != 0, __SQLITEPANIC2(ESqliteOsPanicNullDbFilePtr));
	return *(static_cast <TDbFile*> (aDbFile));
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////       TFileIo class definition    ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQLite OS porting layer API.

Closes the file referred by aDbFile parameter.

@param aDbFile A pointer to a TDbFile instance, than contains the file handle to be closed.

@return SQLITE_OK

@see TDbFile
*/
/* static */ int TFileIo::Close(sqlite3_file* aDbFile)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, TFILEIO_CLOSE1, "OS;0x%X;TFileIo::Close", (TUint)&dbFile));
	__OS_CALL(EOsFileClose, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileClose], ::OsCallProfile(dbFile.iIsJournal, EOsFileClose), 0, 0, aDbFile, 0);
	__FS_CALL(EFsOpFileClose, 0);
	dbFile.iFileBuf.Close();
	return SQLITE_OK;
	}

/**
SQLite OS porting layer API.

Reads from the file referred by the aDbFile parameter.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle to be read from.
@param aBuf Output parameter. The data read from the file will be copied there.
			The buffer size must be at least aAmt bytes.
@param aAmt The amount of data to be read form the file.
@param aOffset The offset in the file where the read operation should start.

@return SQLITE_IOERR_READ, 			The file read or seek operation has failed;
	    SQLITE_IOERR_SHORT_READ, 	The amount of the data read is less than aAmt;
	    SQLITE_IOERR_NOMEM,			An out of memory condition has occured;
	    SQLITE_OK,					The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.

@see COsLayerData::SetOsErrorCode()
@see TDbFile
*/
/* static */ int TFileIo::Read(sqlite3_file* aDbFile, void* aBuf, int aAmt, sqlite3_int64 aOffset)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_READ_ENTRY, "OS-Entry;0x%X;TFileIo::Read;aAmt=%d;aOffset=%lld", (TUint)&dbFile, aAmt, aOffset));
	__OS_CALL(EOsFileRead, 0, 0);
	__COUNTER_INCR(TheSqlSrvProfilerFileRead);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileRead], ::OsCallProfile(dbFile.iIsJournal, EOsFileRead), aOffset, aAmt, aDbFile, 0);
	TPtr8 ptr((TUint8*)aBuf, 0, aAmt);
	TInt err = dbFile.iFileBuf.Read(aOffset, ptr);
	TInt cnt = ptr.Length();
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_IOERR_READ);
	if(cnt != aAmt && (sqliteErr == SQLITE_OK || sqliteErr == SQLITE_IOERR_SHORT_READ))
		{
		Mem::FillZ(static_cast <TUint8*> (aBuf) + cnt, aAmt - cnt);
		err = KErrEof;
		sqliteErr = SQLITE_IOERR_SHORT_READ;
		}
	COsLayerData::Instance().SetOsErrorCode(err);
	SQLITE_TRACE_OS(OstTraceExt4(TRACE_INTERNALS, TFILEIO_READ_EXIT, "OS-Exit;0x%X;TFileIo::Read;cnt=%d;err=%d;sqliteErr=%d", (TUint)&dbFile, cnt, err, sqliteErr));
	return sqliteErr;
	}

//Creates a temporary file in "\temp" subdirectory of osLayerData.iSysPrivDir directory.
//If the function fails, the temp file will be closed and deleted, 
//the related Symbian OS error will be returned to the caller.
/* static */TInt TFileIo::DoCreateTempFile(TDbFile& aDbFile)
    {
    COsLayerData& osLayerData = COsLayerData::Instance();
    //TParse2 is used in order to avoid the need of another TFileName stack based variable
	class TParse2 : public TParse
		{
	public:
		inline TFileName& FileName()
			{
			return static_cast <TFileName&> (NameBuf());
			}
		};
	TParse2 parse;
	(void)parse.Set(osLayerData.iSysPrivDir, 0, 0);//this call can't fail
	(void)parse.AddDir(KTempFileDir);//this call can't fail
    __FS_CALL(EFsOpFileCreateTemp, 0);
    TInt err = aDbFile.iFileBuf.Temp(osLayerData.iFs, parse.FullName(), parse.FileName(), EFileRead|EFileWrite|EDeleteOnClose);        
    if(err == KErrPathNotFound)
        {
        err = osLayerData.iFs.MkDirAll(parse.DriveAndPath());
        if(err == KErrNone)
            {
            err = aDbFile.iFileBuf.Temp(osLayerData.iFs, parse.FullName(), parse.FileName(), EFileRead|EFileWrite|EDeleteOnClose);
            }
        }
    if(err == KErrNone)
        {
        TInt recReadBufSize = -1;
        err = TVfs::DoGetDeviceCharacteristicsAndSectorSize(aDbFile, recReadBufSize);
        if(err != KErrNone)
            {
            aDbFile.iFileBuf.Close();//With EDeleteOnClose flag set, the file will be deleted
            }
        else
            {
            (void)aDbFile.iFileBuf.SetReadAheadSize(aDbFile.iSectorSize, recReadBufSize);
            aDbFile.iIsFileCreated = ETrue;
            }
        }
    return err;
    }

/**
SQLite OS porting layer API.

Writes to the file referred by the aDbFile parameter.
"Write beyond the end of the file" operations are allowed.

If the write operation is in the 1st db file page and there is a registered "free pages" callback 
(TDbFile::iFreePageCallback) and the free pages count is above the defined value,
then the callback will be called.

If the file to be written to is a temp file, which is not created yet, then the file will be created.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle to be written to.
@param aData The data to be written to the file. The buffer size must be at least aAmt bytes.
@param aAmt The amount of data to be written to the file.
@param aOffset The offset in the file where the write operation should start.

@return SQLITE_FULL,       	The file write or seek operation has failed.
							The disk is full;
	    SQLITE_IOERR_NOMEM,	An out of memory condition has occured;
	    SQLITE_OK,			The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.
	    
@see COsLayerData::SetOsErrorCode()
@see TDbFile
*/
/* static */ int TFileIo::Write(sqlite3_file* aDbFile, const void* aData, int aAmt, sqlite3_int64 aOffset)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_WRITE_ENTRY, "OS-Entry;0x%X;TFileIo::Write;aAmt=%d;aOffset=%lld", (TUint)&dbFile, aAmt, aOffset));
	__OS_CALL(EOsFileWrite, 0, 0);
    __COUNTER_INCR(TheSqlSrvProfilerFileWrite);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileWrite], ::OsCallProfile(dbFile.iIsJournal, EOsFileWrite), aOffset, aAmt, aDbFile, 0);
	TInt err = KErrNone;
	if(!dbFile.iIsFileCreated)
	    {//Create a temp file if it has not been created. 
	    err = TFileIo::DoCreateTempFile(dbFile);
	    }
	if(err != KErrNone)
	    {
        COsLayerData::Instance().SetOsErrorCode(err);
		return ::Os2SqliteErr(err, SQLITE_FULL);
	    }
	err = KErrAccessDenied;
	if(!dbFile.iReadOnly)
		{
		TPtrC8 ptr((const TUint8*)aData, aAmt);
		err = dbFile.iFileBuf.Write(aOffset, ptr);
		}
	COsLayerData::Instance().SetOsErrorCode(err);
	
	const TInt KFreePageCountOffset = 36;//hard-coded constant. SQLite does not offer anything - a constant or #define.
	//The checks in the "if" bellow do:
	// - "err == KErrNone" - check the free page count only after a successful "write";
	// - "aOffset == 0"    - check the free page count only if the write operation affects the system page (at aOffset = 0);
	// - "aAmt >= (KFreePageCountOffset + sizeof(int))" - check the free page count only if the amount of bytes to be written
	//						 is more than the offset of the free page counter (othewrise the free page counter is not affected
	//						 by this write operation);
	// - "dbFile.iFreePageCallback.IsValid()" - check the free page count only if there is a valid callback;
	if(err == KErrNone  && aOffset == 0 && aAmt >= (KFreePageCountOffset + sizeof(int)) && dbFile.iFreePageCallback.IsValid())
		{
		const TUint8* ptr = static_cast <const TUint8*> (aData) + KFreePageCountOffset;
		TInt freePageCount = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
		dbFile.iFreePageCallback.CheckAndCallback(freePageCount);
		}
		
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_FULL);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_WRITE_EXIT, "OS-Exit;0x%X;TFileIo::Write;err=%d;sqliteErr=%d", (TUint)&dbFile, err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Truncates the file referred by the aDbFile parameter.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aLength The new file size in bytes.

@return SQLITE_FULL,       	The disk is full;
		SQLITE_IOERR,		This is a read-only file.
	    					The file truncate operation has failed;
	    SQLITE_IOERR_NOMEM,	An out of memory condition has occured;
	    SQLITE_OK,			The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.

@see COsLayerData::SetOsErrorCode()
@see TDbFile
*/
/* static */ int TFileIo::Truncate(sqlite3_file* aDbFile, sqlite3_int64 aLength)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TFILEIO_TRUNCATE_ENTRY, "OS-Entry;0x%X;TFileIo::Truncate;aLength=%lld", (TUint)&dbFile, aLength));
	__OS_CALL(EOsFileTruncate, 0, 0);
    __COUNTER_INCR(TheSqlSrvProfilerFileSetSize);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileTruncate], ::OsCallProfile(dbFile.iIsJournal, EOsFileTruncate), aLength, 0, aDbFile, 0);
	if(dbFile.iReadOnly)
		{
		COsLayerData::Instance().SetOsErrorCode(KErrAccessDenied);
  		return SQLITE_IOERR;	
		}
	__FS_CALL(EFsOpFileSetSize, 0);
	TInt err = dbFile.iFileBuf.SetSize(aLength);
	COsLayerData::Instance().SetOsErrorCode(err);
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_IOERR);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_TRUNCATE_EXIT, "OS-Exit;0x%X;TFileIo::Truncate;err=%d;sqliteErr=%d", (TUint)&dbFile, err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Flushes the file referred by the aDbFile parameter.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.

@return SQLITE_IOERR,		This is a read-only file.
	    					The file flush operation has failed;
	    SQLITE_IOERR_NOMEM,	An out of memory condition has occured;
	    SQLITE_OK,			The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.

@see COsLayerData::SetOsErrorCode()
@see TDbFile
*/
/* static */int TFileIo::Sync(sqlite3_file* aDbFile, int /* aFlags */)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, TFILEIO_SYNC_ENTRY, "OS-Entry;0x%X;TFileIo::Sync", (TUint)&dbFile));
	__OS_CALL(EOsFileSync, 0, 0);
    __COUNTER_INCR(TheSqlSrvProfilerFileSync);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileSync], ::OsCallProfile(dbFile.iIsJournal, EOsFileSync), 0, 0, aDbFile, 0);
	if(dbFile.iReadOnly)
		{
		COsLayerData::Instance().SetOsErrorCode(KErrAccessDenied);
		return SQLITE_IOERR;
		}
	__FS_CALL(EFsOpFileSync, 0);
	TInt err = dbFile.iFileBuf.Flush();
	COsLayerData::Instance().SetOsErrorCode(err);
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_IOERR);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_SYNC_EXIT, "OS-Exit;0x%X;TFileIo::Sync;err=%d;sqliteErr=%d", (TUint)&dbFile, err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Returns the size of the file referred by the aDbFile parameter.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aSize Output parameter. If the function completes successfully, the file size will be stored there.

@return SQLITE_IOERR,			The file size operation has failed;
	    SQLITE_IOERR_NOMEM,		An out of memory condition has occured;
	    SQLITE_OK,				The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.
	    
@see COsLayerData::SetOsErrorCode()
@see TDbFile
*/
/* static */ int TFileIo::FileSize(sqlite3_file* aDbFile, sqlite3_int64* aSize)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, TFILEIO_FILESIZE_ENTRY, "OS-Entry;0x%X;TFileIo::FileSize", (TUint)&dbFile));
	__OS_CALL(EOsFileFileSize, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileFileSize], ::OsCallProfile(dbFile.iIsJournal, EOsFileFileSize), 0, 0, aDbFile, 0);
	__FS_CALL(EFsOpFileSize, 0);
	TInt err =  dbFile.iFileBuf.Size(*aSize);
	COsLayerData::Instance().SetOsErrorCode(err);
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_IOERR);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_FILESIZE_EXIT, "OS-Exit;0x%X;TFileIo::FileSize;err=%d;sqliteErr=%d", (TUint)&dbFile, err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Locks the file, referred by the aDbFile parameter, with the specified lock type.
Since this is a single-threaded OS porting layer implementation, the file is not actually locked - small
performance optimisation. The file lock type is stored for later use by the CheckReservedLock() call.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aLockType Lock type: SQLITE_LOCK_NONE, SQLITE_LOCK_SHARED, SQLITE_LOCK_RESERVED, SQLITE_LOCK_PENDING or
				 SQLITE_LOCK_EXCLUSIVE.

@return SQLITE_OK,	The operation has completed successfully.

@see TFileIo::CheckReservedLock()
@see TFileIo::Unlock()
	    
@see TDbFile
*/
/* static */ int TFileIo::Lock(sqlite3_file* aDbFile, int aLockType)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	__OS_CALL(EOsFileLock, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileLock], ::OsCallProfile(dbFile.iIsJournal, EOsFileLock), aLockType, 0, aDbFile, 0);
	//If there is already a lock of this type or more restrictive on the database file, do nothing.
	if(dbFile.iLockType >= aLockType)
		{
		return SQLITE_OK;
		}
	dbFile.iLockType = aLockType;
	return SQLITE_OK;
	}

/**
SQLite OS porting layer API.

Unlocks the file, referred by the aDbFile parameter.
Since this is a single-threaded OS porting layer implementation, the file never gets locked - small
performance optimisation. The Unlock() call only sets the stored file lock type with the aLockType value.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aLockType Lock type: SQLITE_LOCK_NONE, SQLITE_LOCK_SHARED, SQLITE_LOCK_RESERVED, SQLITE_LOCK_PENDING or
				 SQLITE_LOCK_EXCLUSIVE.

@return SQLITE_OK,	The operation has completed successfully.

@see TFileIo::CheckReservedLock()
@see TFileIo::Lock()
	    
@see TDbFile
*/
/* static */ int TFileIo::Unlock(sqlite3_file* aDbFile, int aLockType)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	__OS_CALL(EOsFileUnlock, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileUnlock], ::OsCallProfile(dbFile.iIsJournal, EOsFileUnlock), aLockType, 0, aDbFile, 0);
	dbFile.iLockType = aLockType;
	return SQLITE_OK;
	}

/**
SQLite OS porting layer API.

Checks if the file lock type is SQLITE_LOCK_RESERVED or bigger.
Since this is a single-threaded OS porting layer implementation, the file never gets locked - small
performance optimisation. The CheckReservedLock() call only checks if the stored file lock type 
is bigger or equal than SQLITE_LOCK_RESERVED.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aResOut Output parameter. It should be set to 1 if the stored lock type is bigger or equal 
							     than SQLITE_LOCK_RESERVED.

@return SQLITE_OK.

@see TFileIo::Lock()
@see TFileIo::Unlock()
	    
@see TDbFile
*/
/* static */ int TFileIo::CheckReservedLock(sqlite3_file* aDbFile, int *aResOut)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	__OS_CALL(EOsFileCheckReservedLock, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileCheckReservedLock], ::OsCallProfile(dbFile.iIsJournal, EOsFileCheckReservedLock), 0, 0, aDbFile, 0);
	*aResOut = dbFile.iLockType >= SQLITE_LOCK_RESERVED ? 1 : 0;
  	return SQLITE_OK;
	}

/**
SQLite OS porting layer API.

Performs an aOp operation on the file referred by the aDbFile parameter.
Since the only supported operation at the moment is SQLITE_FCNTL_LOCKSTATE, and the current lock type is stored as
a data memebr of TDbFile, the function implementation has been optimised - no file I/O calls. The stored file lock type
is retured if the operation is SQLITE_FCNTL_LOCKSTATE.

Note: The range of supported operations includes KSqlFcntlRegisterFreePageCallback now.
      When the function is called with aOp = KSqlFcntlRegisterFreePageCallback, then a callback will be registered
      and called when the number of the free pages goes above certain threshold.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aOp File operation type. Currently only SQLITE_FCNTL_LOCKSTATE is supported.
@param aArg An additional input/output parameter which purpose depends on the type of the current file operation.
			If the file operation is SQLITE_FCNTL_LOCKSTATE, then aArg is used as an output parameter, where
			the file lock type is stored.
			If the operation type is KSqlFcntlRegisterFreePageCallback, then aArg points to a TSqlFreePageCallback object,
			that contains the free page threshold and the callback.

@return SQLITE_ERROR,	Non-supported operation;
		SQLITE_OK,		The operation has completed successfully.
	    
@see TDbFile
*/
/* static */ int TFileIo::FileControl(sqlite3_file* aDbFile, int aOp, void* aArg)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TFILEIO_FILECONTROL_ENTRY, "OS-Entry;0x%X;TFileIo::FileControl;aOp=%d", (TUint)&dbFile, aOp));
	__OS_CALL(EOsFileFileControl, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileFileControl], ::OsCallProfile(dbFile.iIsJournal, EOsFileFileControl), aOp, 0, aDbFile, 0);
	TInt err = KErrNone;
	switch(aOp)
		{
		case SQLITE_FCNTL_LOCKSTATE:
			*(int*)aArg = dbFile.iLockType;
			break;
		case KSqlFcntlRegisterFreePageCallback:
			{
			err = KErrArgument;
			if(aArg)
				{
				TSqlFreePageCallback* rq = static_cast <TSqlFreePageCallback*> (aArg);
				if(rq->IsValid())
					{
					dbFile.iFreePageCallback = *rq;
					err = KErrNone;
					}
				}
			}
			break;
		default:
			err = KErrArgument;
			break;
		}
	COsLayerData::Instance().SetOsErrorCode(err);
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_ERROR);
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TFILEIO_FILECONTROL_EXIT, "OS-Exit;0x%X;TFileIo::FileControl;err=%d;sqliteErr=%d", (TUint)&dbFile, err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Retrieves the sector size of the media of the file referred by the aDbFile parameter.
Since the sector size never changes till the file is open, the function has been optimised - no file I/O calls.
The sector size is retrieved during the TVfs::Open() call and stored in TDbFile::iSectorSize. The SectorSize()
call returns the value of TDbFile::iSectorSize.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.

@return The sector size.

@panic Sqlite 19 In _DEBUG mode - TDbFile::iSectorSize is negative or 0 .
	    
@see TDbFile
@see TVfs::Open()
*/
/* static */ int TFileIo::SectorSize(sqlite3_file* aDbFile)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	__OS_CALL(EOsFileSectorSize, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileSectorSize], ::OsCallProfile(dbFile.iIsJournal, EOsFileSectorSize), 0, 0, aDbFile, 0);
	__ASSERT_DEBUG(dbFile.iSectorSize > 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
	return dbFile.iSectorSize;
	}

/**
SQLite OS porting layer API.

Retrieves the device characteristics of the device of the file referred by the aDbFile parameter.
Since the device characteristics never change till the file is open, the function has been optimised - no file I/O calls.
The device characteristics are retrieved during the TVfs::Open() call and stored in TDbFile::iDeviceCharacteristics. 
The DeviceCharacteristics() call returns the value of TDbFile::iDeviceCharacteristics.

@param aDbFile A pointer to a TDbFile instance, that contains the file handle.

@return A bit set containing the device characteristics.
	    
@panic Sqlite 19 In _DEBUG mode - TDbFile::iDeviceCharacteristics is negative or 0 .

@see TDbFile
@see TVfs::Open()
*/
/* static */ int TFileIo::DeviceCharacteristics(sqlite3_file* aDbFile)
	{
	TDbFile& dbFile = ::DbFile(aDbFile);
	__OS_CALL(EOsFileDeviceCharacteristics, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsFileDeviceCharacteristics], ::OsCallProfile(dbFile.iIsJournal, EOsFileDeviceCharacteristics), 0, 0, aDbFile, 0);
	__ASSERT_DEBUG(dbFile.iDeviceCharacteristics >= 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
	return dbFile.iDeviceCharacteristics;	
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////       TVfs class definition     ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
Opens a private secure database.
Actually the database file has been created or opened by the client's process. 
This function only adopts the passed in aMsg parameter file handle.

@param aDbFile 		Output parameter, where the initialized file handle will be stored.
@param aMsg    		A reference to the current RMessage2 instance. Contains the file handle of the database created
					or opened by the client's process.
@param aReadOnly	True if the file is read-only.

@return KErrNone,          The operation has completed succesfully;
	    KErrNoMemory,      Out of memory condition has occured;
                           Note that other system-wide error codes may also be returned.
	    
@see TDbFile
@see TVfs::Open()
*/
/* static */ TInt TVfs::DoOpenFromHandle(TDbFile& aDbFile, const RMessage2& aMsg, TBool aReadOnly)
	{
	__FS_CALL(EFsOpFileAdopt, 0);
	TInt err = aDbFile.iFileBuf.AdoptFromClient(aMsg, KFhSessHandleIdx, KFhFileHandleIdx);
	if(err == KErrNone)
		{
		aDbFile.iReadOnly = aReadOnly;
		}
	return err;
	};

/**
Collects information about the drive referred by the aDriveNo parameter.

@param aFs			RFs instance.
@param aDriveNo     The drive about which an information will be collected.
@param aVolumeInfo	Output parameter. A reference to a TVolumeIOParamInfo object where the collected information will be stored.

@return KErrNone,          The operation has completed succesfully;
	    KErrNoMemory,      Out of memory condition has occured;
                           Note that other system-wide error codes may also be returned.
	    
@see TVfs::Open()
*/
/* static */ inline TInt TVfs::DoGetVolumeIoParamInfo(RFs& aFs, TInt aDriveNo, TVolumeIOParamInfo& aVolumeInfo)
	{
	__FS_CALL(EFsOpFsVolumeIoParam, 0);
	TInt err = aFs.VolumeIOParam(aDriveNo, aVolumeInfo); 
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TVFS_DOGETVOLUMEIOPARAMINFO, "OS;0;TVfs::DoGetVolumeIoParamInfo;aDriveNo=%d;err=%d", aDriveNo, err));
	return err;		
	}

//Maps disk sector sizes to SQLITE_IOCAP_ATOMIC<n> constants

struct TSqliteSectorSizeMap
	{
	TInt	iSectorSize;
	TInt	iSqliteSectorSizeConstant;
	};

//Used in TVfs::DoGetDeviceCharacteristics() to find which SQLITE_IOCAP_ATOMIC<n> constant should be used
//for the specified sector size
const TSqliteSectorSizeMap KSqliteSectorSizeMap[] = 
	{
		{  512, SQLITE_IOCAP_ATOMIC512},		
		{ 1024, SQLITE_IOCAP_ATOMIC1K},			
		{ 2048, SQLITE_IOCAP_ATOMIC2K},			
		{ 4096, SQLITE_IOCAP_ATOMIC4K},			
		{ 8192, SQLITE_IOCAP_ATOMIC8K},		
		{16384, SQLITE_IOCAP_ATOMIC16K},			
		{32768, SQLITE_IOCAP_ATOMIC32K},			
		{65536, SQLITE_IOCAP_ATOMIC64K}			
	};

/**
Retrieves and returns in a bit set the device characteristics.

@param aDriveInfo	A TDriveInfo reference from which the device characteristics will be extracted.
@param aVolumeInfo	A TVolumeIOParamInfo reference from which the device characteristics will be extracted.

@return A bit set containing the device characteristics: 
			SQLITE_IOCAP_SAFE_APPEND, SQLITE_IOCAP_ATOMIC, the atomic block size.
	    
@see TVfs::DoGetVolumeIoParamInfo();
@see TVfs::Open()
*/
/* static */ TInt TVfs::DoGetDeviceCharacteristics(const TDriveInfo& aDriveInfo, const TVolumeIOParamInfo& aVolumeInfo)
	{
	TInt deviceCharacteristics = 0;	
	if(aDriveInfo.iDriveAtt & (KDriveAttLocal | KDriveAttInternal))
		{
		deviceCharacteristics |= SQLITE_IOCAP_SAFE_APPEND;//Data written first, file size updated second
		}
	if(aDriveInfo.iDriveAtt & KDriveAttTransaction)
		{
		deviceCharacteristics |= SQLITE_IOCAP_ATOMIC;	
		}
	for(TInt i=0;i<(sizeof(KSqliteSectorSizeMap)/sizeof(KSqliteSectorSizeMap[0]));++i)
		{
		if(KSqliteSectorSizeMap[i].iSectorSize == aVolumeInfo.iBlockSize)
			{
			deviceCharacteristics |= KSqliteSectorSizeMap[i].iSqliteSectorSizeConstant;
			break;
			}
		}
	return deviceCharacteristics;
	}

/**
Retrieves and returns the sector size of the drive referred by the aDriveInfo parameter.
The sector size must be a power of two.
The sector size is extracted only if aDriveInfo refers to a removable device, otherwise the
SQLITE_DEFAULT_SECTOR_SIZE value (512 bytes) will be used as a sector size.

@param aDriveInfo	A TDriveInfo reference.
@param aVolumeInfo	A TVolumeIOParamInfo reference.

@return The sector size of the drive referred by the aDriveInfo parameter.

@panic Sqlite 19 In _DEBUG mode - The sector size is negative, zero or is not a power of two.
	    
@see TVfs::Open()
*/
/* static */ TInt TVfs::DoGetSectorSize(const TDriveInfo& aDriveInfo, const TVolumeIOParamInfo& aVolumeInfo)
	{
	//Initialize the sectorSize variable only if: 
	// - aDriveInfo refers to a removable drive
	// - aVolumeInfo.iBlockSize > SQLITE_DEFAULT_SECTOR_SIZE;
	// - aVolumeInfo.iBlockSize is power of 2;
	TInt sectorSize = SQLITE_DEFAULT_SECTOR_SIZE;
	if(aDriveInfo.iDriveAtt & KDriveAttRemovable)
		{
		if(aVolumeInfo.iBlockSize > SQLITE_DEFAULT_SECTOR_SIZE && (aVolumeInfo.iBlockSize & (aVolumeInfo.iBlockSize - 1)) == 0)
			{
			sectorSize = aVolumeInfo.iBlockSize;
			}
		}
	__ASSERT_DEBUG(sectorSize > 0 && (sectorSize & (sectorSize - 1)) == 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
	return sectorSize;
	}

/**
Retrieves in a bit set the device characteristics of the device of the file referred by the aDbFile parameter.
Retrieves the sector size of the drive of the file referred by the aDbFile parameter. 
The sector size and the device characteristics will be stored in iSectorSize and iDeviceCharacteristics TDbFile data members.
The stored values will be used later by TFileIo::DeviceCharacteristics() and TFileIo::SectorSize().

@param aDbFile	Input/Output parameter. A TDriveInfo reference. The collected information will be stored in TDbDrive
				data members.
@param aRecReadBufSize Output parameter. The recommended buffer size for optimised reading performance.

@return KErrNone,          The operation has completed succesfully;
                           Note that other system-wide error codes may also be returned.

@panic Sqlite 19 In _DEBUG mode - TDbFile::iSectorSize has been already initialized.
@panic Sqlite 19 In _DEBUG mode - TDbFile::iDeviceCharacteristics has been already initialized.

@see TVfs::DoGetDeviceCharacteristics();
@see TVfs::DoGetSectorSize();
@see TVfs::Open()
@see TDbFile
@see TFileIo::DeviceCharacteristics()
@see TFileIo::SectorSize()
*/
/* static */ TInt TVfs::DoGetDeviceCharacteristicsAndSectorSize(TDbFile& aDbFile, TInt& aRecReadBufSize)
	{
	__ASSERT_DEBUG(aDbFile.iDeviceCharacteristics < 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
	__ASSERT_DEBUG(aDbFile.iSectorSize <= 0, __SQLITEPANIC2(ESqliteOsPanicInternalError));
	TInt driveNo;
	TDriveInfo driveInfo;
	__FS_CALL(EFsOpFileDrive, 0);
	TInt err = aDbFile.iFileBuf.Drive(driveNo, driveInfo);
	if(err != KErrNone)
		{
		return err;	
		}
	TVolumeIOParamInfo volumeInfo;
	err = TVfs::DoGetVolumeIoParamInfo(COsLayerData::Instance().iFs, driveNo, volumeInfo);
	if(err != KErrNone)
		{
		return err;	
		}
	aDbFile.iDeviceCharacteristics = TVfs::DoGetDeviceCharacteristics(driveInfo, volumeInfo);
	aDbFile.iSectorSize = TVfs::DoGetSectorSize(driveInfo, volumeInfo);
	aRecReadBufSize = volumeInfo.iRecReadBufSize;
	SQLITE_TRACE_OS(OstTraceExt5(TRACE_INTERNALS, TVFS_DOGETGETDEVICECHARACTERISTICSANDSECTORSIZE, "OS;0x%X;TVfs::DoGetDeviceCharacteristicsAndSectorSize;driveNo=%d;sectorSize=%d;devCharact=0x%X;readBufSize=%d", (TUint)&aDbFile, driveNo, aDbFile.iSectorSize, (TUint)aDbFile.iDeviceCharacteristics, volumeInfo.iRecReadBufSize));
	return KErrNone;
	}

/**
SQLite OS porting layer API.

The behaviour of the RFile/RFile64::SetSize operation is not atomic for non-rugged drives. 
When RFile/RFile64::SetSize() is called 2 operations occurs:-

1)The cluster chain of the file is updated.
2)The new file size is added to the file cache.

If a power loss occurs after a SetSize there is a chance that the cluster chain was updated 
but the new file size is not yet flushed to the file. This puts the file into an inconsistent state.
This is most likely to occur in the journal file where the time between a SetSize and Flush can 
be long. 

For this reason this check is added when the file is opened to see if the end of the file can 
be read straight away, if an error is returned then it is assumed that the SetSize has not be 
completed previously. In this case the file is deleted and re-created.
 
@param aDbFile A pointer to a TDbFile instance, that contains the file handle.
@param aFname A string of 16-bit wide characters containing name of the file to be checked.
@param aFmode The mode in which the file is opened. These mode are documented in TFileMode.

@return KErrNone,          The operation has completed succesfully;
                           Note that other system-wide error codes may also be returned.
@see TFileMode
@see TVfs::Open()
@see TDbFile
*/
/* static */ TInt TVfs::DoFileSizeCorruptionCheck(TDbFile& aDbFile, const TDesC& aFname, TInt aFmode)
    {
    const TInt KMinSize = 16;
    TInt64 size;
    TInt err = KErrNone ;
    TBuf8<KMinSize> buf;

    err = aDbFile.iFileBuf.Size(size);
    if (err != KErrNone)
        {
        return err;
        }
    TBool IsMinFileSize = (size >= KMinSize);
    
    if (IsMinFileSize)
        {
        err = aDbFile.iFileBuf.Read(size - KMinSize, buf);
        }
    
    if (err == KErrCorrupt || err == KErrEof || !IsMinFileSize)
        {
        COsLayerData& osLayerData = COsLayerData::Instance();
    
        aDbFile.iFileBuf.Close();
        __SQLITETRACE_OSEXPR(TInt err2 =) osLayerData.iFs.Delete(aFname);
		SQLITE_TRACE_OS(OstTraceExt4(TRACE_INTERNALS, TVFS_DOFILESIZECORRUPTIONCHECK1, "OS;0x%X;TVfs::DoFileSizeCorruptionCheck;size=%lld;err=%d;deleteErr=%d", (TUint)&aDbFile, size, err, err2));
        err = aDbFile.iFileBuf.Create(osLayerData.iFs, aFname, aFmode);
		SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TVFS_DOFILESIZECORRUPTIONCHECK2, "OS;0x%X;TVfs::DoFileSizeCorruptionCheck;createErr=%d", (TUint)&aDbFile, err));
        }
    return err;
    }

/**
SQLite OS porting layer API.

Opens or creates a file which name is in the aFileName parameter.
If the function succeeds, the file handle and other related information will be stored in the place pointed by the 
aDbFile parameter, a memory block of sizeof(TDbFile) size for which is allocated by the caller.
The function will also retrieve the sector size and the device characteristics and store them in aDbFile,
which is actually a TDbFile pointer, for later use.

@param aFileName Zero-terminated, UTF8 encoded file name.
				 If aFileName is NULL then a temporary file is created.
@param aDbFile Output parameter. The file handle and other related information will be stored there.
@param aFlags  "Open/Create" input flags: 
					SQLITE_OPEN_DELETEONCLOSE,
					SQLITE_OPEN_READWRITE,
					SQLITE_OPEN_EXCLUSIVE,
					SQLITE_OPEN_CREATE
@param aOutFlags  "Open/Create" output flags:
					SQLITE_OPEN_READWRITE,
					SQLITE_OPEN_READONLY

@return SQLITE_CANTOPEN,    The aFileName parameter cannot be converted to UTF16.
							Any other file I/O error will also be reported as SQLITE_CANTOPEN;
	    SQLITE_IOERR_NOMEM,	An out of memory condition has occured;
	    SQLITE_OK,			The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.

@see COsLayerData::SetOsErrorCode()
@see TDbFile
*/
/* static */ int TVfs::Open(sqlite3_vfs* aVfs, const char* aFileName, sqlite3_file* aDbFile, int aFlags, int* aOutFlags)
	{
	__OS_CALL(EOsVfsOpen, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsOpen], ::OsCallProfile(EFalse, EOsVfsOpen), 0, 0, aDbFile, aFileName);
	COsLayerData& osLayerData = COsLayerData::Instance();
	new (aDbFile) TDbFile;
	TDbFile& dbFile = ::DbFile(aDbFile);
    if(!aFileName)
        {  
        //It is to create and open a temp file if aFileName is NULL. In this case,
        //we will defer the file creation util it is needed.  
        dbFile.pMethods = &TheFileIoApi;
        dbFile.iIsFileCreated = EFalse;
        if(aOutFlags)
            {
            *aOutFlags = SQLITE_OPEN_READWRITE;
            }
		osLayerData.SetOsErrorCode(KErrNone);
        return SQLITE_OK;
        }  
	TFileName fname;
    if(!::ConvertToUnicode(aFileName, fname))
		{
        osLayerData.SetOsErrorCode(KErrBadName);
        return SQLITE_CANTOPEN;	
    	}
	SQLITE_TRACE_OS(OstTraceExt3(TRACE_INTERNALS, TVFS_OPEN_ENTRY, "OS-Entry;0x%X;TVfs::Open;fname=%S;aFlags=0x%X", (TUint)&aDbFile, __SQLITEPRNSTR(fname), (TUint)aFlags));
	TFhStrType fhStrType = aFileName ? ::FhStringProps(aFileName) : ENotFhStr;
	TInt err = KErrNone;
	if(fhStrType == EFhMainDbStr)
		{//Main db file, open from handle
		const RMessage2* msg;
		TBool readOnly;
		osLayerData.RetrieveAndResetFhData(msg, readOnly);
		err = msg != NULL ? TVfs::DoOpenFromHandle(dbFile, *msg, readOnly) : KErrGeneral;
		}
	else
		{
		if(fhStrType == EFhStr)
			{//Not the main db file. Remove invalid characters in the file name
			::FhConvertToFileName(fname, osLayerData.iSysPrivDir);//If fname does not have a path, iSysPrivDir will be used
			}
		TInt fmode = EFileRead;
		if(aFlags & SQLITE_OPEN_READWRITE)
			{
			fmode |= EFileWrite;
			}
		if(aFlags & SQLITE_OPEN_EXCLUSIVE)
			{
			fmode |= EFileShareExclusive;
			}
		if(aFlags & SQLITE_OPEN_DELETEONCLOSE)
			{
			fmode |= EDeleteOnClose;
			}
		err = KErrAccessDenied;
		TInt prevErr = KErrNone;
		if(aFlags & SQLITE_OPEN_CREATE)
			{
			__FS_CALL(EFsOpFileCreate, 0);
			prevErr = err = dbFile.iFileBuf.Create(osLayerData.iFs, fname, fmode);
			}
		if(err != KErrNone && err != KErrNoMemory && err != KErrDiskFull)
			{
			__FS_CALL(EFsOpFileOpen, 0);
			err = dbFile.iFileBuf.Open(osLayerData.iFs, fname, fmode);

			if(err == KErrNone && (aFlags & KJournalFileTypeBitMask))
				{
				err = TVfs::DoFileSizeCorruptionCheck(dbFile, fname, fmode);
				}
			}
		if((err != KErrNone && err != KErrNoMemory && err != KErrDiskFull) && (aFlags & SQLITE_OPEN_READWRITE))
			{
			aFlags &= ~SQLITE_OPEN_READWRITE;
			aFlags |= SQLITE_OPEN_READONLY;
			fmode &= ~EFileWrite;
			__FS_CALL(EFsOpFileOpen, 0);
			err = dbFile.iFileBuf.Open(osLayerData.iFs, fname, fmode);
			}
		if(err != KErrNone && prevErr == KErrAccessDenied)
			{
			err = KErrAccessDenied;
			}
		}
	TInt recReadBufSize = -1;
	if(err == KErrNone)
		{
		err = TVfs::DoGetDeviceCharacteristicsAndSectorSize(dbFile, recReadBufSize);
		}
	osLayerData.SetOsErrorCode(err);
	if(err != KErrNone)
		{
		__FS_CALL(EFsOpFileClose, 0);
		dbFile.iFileBuf.Close();	
		}
	else
		{
		dbFile.pMethods = &TheFileIoApi;
		if(fhStrType != EFhMainDbStr)
			{
			dbFile.iReadOnly = !(aFlags & SQLITE_OPEN_READWRITE);
			}
		if(aOutFlags)
			{
			*aOutFlags = dbFile.iReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
			}
		(void)dbFile.iFileBuf.SetReadAheadSize(dbFile.iSectorSize, recReadBufSize);
		}
#ifdef _SQLPROFILER
	dbFile.iIsJournal = aFlags & KJournalFileTypeBitMask; 
#endif
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_CANTOPEN);
	SQLITE_TRACE_OS(OstTraceExt4(TRACE_INTERNALS, TVFS_OPEN_EXIT, "OS-Exit;0x%X;TVfs::Open;outFlags=0x%X;err=%d;sqliteErr=%d", (TUint)&aDbFile, aOutFlags ? (TUint)*aOutFlags : 0, err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Deletes a file which name is in the aFileName parameter.

@param aFileName Zero-terminated, UTF8 encoded file name.

@return SQLITE_ERROR,    	The aFileName parameter cannot be converted to UTF16.
							The file name refers to a private secure database;
	    SQLITE_IOERR_NOMEM,	An out of memory condition has occured;
	    SQLITE_IOERR_DELETE,The delete file operation has failed;
	    SQLITE_OK,			The operation has completed successfully.
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.

@see COsLayerData::SetOsErrorCode()
*/
/* static */ int TVfs::Delete(sqlite3_vfs* aVfs, const char* aFileName, int /*aSyncDir*/)
	{
	__OS_CALL(EOsVfsDelete, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsDelete], ::OsCallProfile(EFalse, EOsVfsDelete), 0, 0, 0, aFileName);
	COsLayerData& osLayerData = COsLayerData::Instance();
	TBuf<KMaxFileName + 1> fname;
	if(!::ConvertToUnicode(aFileName, fname))
		{
		osLayerData.SetOsErrorCode(KErrBadName);
		SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, TVFS_DELETE1, "OS;0;TVfs::Delete;err=KErrBadName"));
		return SQLITE_ERROR;	
		}
	SQLITE_TRACE_OS(OstTraceExt1(TRACE_INTERNALS, TVFS_DELETE_ENTRY, "OS-Entry;0;TVfs::Delete;fname=%S", __SQLITEPRNSTR(fname)));
	TFhStrType fhStrType = FhStringProps(aFileName);
	if(fhStrType == EFhMainDbStr)
		{//Deleting files not in your own private data cage - not allowed!
		osLayerData.SetOsErrorCode(KErrPermissionDenied);
		SQLITE_TRACE_OS(OstTraceExt1(TRACE_INTERNALS, TVFS_DELETE2, "OS;0;TVfs::Delete;err=KErrPermissionDenied;fname=%S", __SQLITEPRNSTR(fname)));
		return SQLITE_ERROR;	
		}
	if(fhStrType == EFhStr)
		{
		::FhConvertToFileName(fname, osLayerData.iSysPrivDir);//If fname does not have a path, iSysPrivDir will be used
		}
	__FS_CALL(EFsOpFileDelete, 0);
	TInt err = osLayerData.iFs.Delete(fname);
	osLayerData.SetOsErrorCode(err);
	TInt sqliteErr = ::Os2SqliteErr(err, SQLITE_IOERR_DELETE);	
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TVFS_DELETE_EXIT, "OS-Exit;0;TVfs::Delete;err=%d;sqliteErr=%d", err, sqliteErr));
	return sqliteErr;
	}

/**
SQLite OS porting layer API.

Retrieves an information about a file which name is in the aFileName parameter.
The requested information type can be: does the file exist, is the file read-only or read/write.

@param aFileName Zero-terminated, UTF8 encoded file name.
@param aFlags This parameter can be one of: SQLITE_ACCESS_READ, SQLITE_ACCESS_EXISTS or SQLITE_ACCESS_READWRITE.
@param aResOut Output parameter, set to 1 if the tested condition is true, 0 otherwise.

@return SQLITE_OK, 			The call has completed successfully,
		SQLITE_IOERR_NOMEM, An out of memory conditon has occured,
		SQLITE_IOERR_ACCESS,File I/O error;  
	    
If the function succeeds, COsLayerData::SetOsErrorCode() is called with KErrNone, otherwise COsLayerData::SetOsErrorCode() is called
with the reported by the OS API error. The stored error code will be used later by the SQLite API caller.

@see COsLayerData::SetOsErrorCode()
*/
/* static */ int TVfs::Access(sqlite3_vfs* aVfs, const char* aFileName, int aFlags, int* aResOut)
	{
	__OS_CALL(EOsVfsAccess, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsAccess], ::OsCallProfile(EFalse, EOsVfsAccess), aFlags, 0, 0, aFileName);
	COsLayerData& osLayerData = COsLayerData::Instance();
	TBuf<KMaxFileName + 1> fname;
	if(!::ConvertToUnicode(aFileName, fname))
		{
		osLayerData.SetOsErrorCode(KErrGeneral);
		SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, TVFS_ACCESS1, "OS;0;TVfs::Access;err=KErrGeneral"));
		return SQLITE_IOERR_ACCESS;
		}
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TVFS_ACCESS_ENTRY, "OS-Entry;0;TVfs::Access;fname=%S;aFlags=0x%X", __SQLITEPRNSTR(fname), (TUint)aFlags));
	TFhStrType fhStrType = ::FhStringProps(aFileName);
	if(fhStrType == EFhStr)
		{
		::FhConvertToFileName(fname, osLayerData.iSysPrivDir);//If fname does not have a path, iSysPrivDir will be used
		}
	TEntry entry;
	__FS_CALL(EFsOpFsEntry, 0);
	TInt err = osLayerData.iFs.Entry(fname, entry);
	if(aFlags == SQLITE_ACCESS_EXISTS && err == KErrNotFound)
		{
		osLayerData.SetOsErrorCode(KErrNone);
		*aResOut = 0;
		SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, TVFS_ACCESS_EXIT1, "OS-Exit;0;TVfs::Access;Exists-NoFound"));
		return SQLITE_OK;
		}
	if(err != KErrNone)
		{
		osLayerData.SetOsErrorCode(err);
		SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, TVFS_ACCESS_EXIT2, "OS-Exit;0;TVfs::Access;err=%d", err));
		return err == KErrNoMemory ? SQLITE_IOERR_NOMEM : SQLITE_IOERR_ACCESS;
		}
	*aResOut = 0;
	switch(aFlags)
		{
		case SQLITE_ACCESS_READ:
			*aResOut =  entry.IsReadOnly();
			break;
		case SQLITE_ACCESS_EXISTS:
			*aResOut = 1;
			break;
		case SQLITE_ACCESS_READWRITE:
			*aResOut = !entry.IsReadOnly();
			break;
		default:
			break;			
		}
	osLayerData.SetOsErrorCode(KErrNone);
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, TVFS_ACCESS_EXIT3, "OS-Exit;0;TVfs::Access;aResOut=%d", *aResOut));
	return SQLITE_OK;
	}

/**
SQLite OS porting layer API.

Accepts UTF8 encoded, zero-terminated file as an input argument in the aRelative parameter
and constructs the full file path in the aBuf output parameter.

If the format of aRelative argument is <[SID]FileName.[EXT]>, then the database file name will be 
treated as a name of a secure database file which has to be created/opened in the server's private 
directory on the system drive.

If the format of aRelative argument is <Drive:[SID]FileName.[EXT]>, then the database file name 
will be treated as a name of a secure database file which has to be created/opened in the server's 
private directory on <Drive:> drive. 

If the format of aRelative argument is <Drive:\Path\FileName.[EXT]>, then the database file name
will be treated as a name of a non-secure database file in <Drive:\Path\> directory.
If aRelative contains file handles, then it will be treated as a name of a file belonging to server's
private data cage. 

@param aRelative The input file name, zero-terminated, UTF8 encoded.
@param aBufLen The output buffer length.
@param aBuf Output buffer for the constructed full file name path. The allocated buffer length must be at least aBufLen bytes.

@return SQLITE_ERROR, The aRelative parameter is NULL or cannot be converted to UTF16;
		SQLITE_OK The operation has completed successfully.
*/
/* static */ int TVfs::FullPathName(sqlite3_vfs* aVfs, const char* aRelative, int aBufLen, char* aBuf)
	{
	__OS_CALL(EOsVfsFullPathName, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsFullPathName], ::OsCallProfile(EFalse, EOsVfsFullPathName), aBufLen, 0, 0, aRelative);
	COsLayerData& osLayerData = COsLayerData::Instance();
	osLayerData.StoreFhData(NULL, EFalse);
	//Convert the received file name to UTF16
	TBuf<KMaxFileName + 1> fname;
	if(!::ConvertToUnicode(aRelative, fname))
		{
		SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, TVFS_FULLPATHNAME1, "OS;0;TVfs::FullPathName;err=SQLITE_ERROR"));
		return SQLITE_ERROR;
		}
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TVFS_FULLPATHNAME_ENTRY, "OS-Entry;0;TVfs::FullPathName;fname=%S;aBufLen=%d", __SQLITEPRNSTR(fname), aBufLen));
	//Zero-terminate the converted file name
	fname.Append(TChar(0));
	TParse parse;
	TFhStrType strType = ::FhStringProps(aRelative);//Detect string type - it may not be a real file name
	if(strType == EFhMainDbStr)
		{//The additonal information has to be extracted and fname reformatted, because SQLITE will
		 //use the returned full file name when making a decission to share the cache.
		::FhExtractAndStore(fname);
		(void)parse.Set(fname, 0, 0);//the file name has to be verified by the file server
		}
	else
		{
		(void)parse.Set(fname, &osLayerData.iSysPrivDir, 0);//If fname does not have a path, iSysPrivDir will be used
		}
	TPtr8 dest8(reinterpret_cast <TUint8*> (aBuf), aBufLen);	
	if(!::ConvertFromUnicode(parse.FullName(), dest8))
		{//Zero the stored fh data, because it has been initialized by the FhExtractAndStore(fname) call (couple of lines above)
		osLayerData.StoreFhData(NULL, EFalse);
		SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, TVFS_FULLPATHNAME_EXIT1, "OS-Exit;0;TVfs::FullPathName;err=SQLITE_ERROR"));
		return SQLITE_ERROR;	
		}
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, TVFS_FULLPATHNAME_EXIT2, "OS-Exit;0;TVfs::FullPathName;err=SQLITE_OK"));
	return SQLITE_OK;
	}

/**
SQLite OS porting layer API.

Generates a set of random numbers and stores them in the aBuf output parameter.

@param aBufLen The output buffer length.
@param aBuf Output buffer for the generated random numbers. The allocated buffer length must be at least aBufLen bytes.

@return The length of the used part of the output buffer.
*/
/* static */ int TVfs::Randomness(sqlite3_vfs* aVfs, int aBufLen, char* aBuf)
	{
	__OS_CALL(EOsVfsRandomness, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsRandomness], ::OsCallProfile(EFalse, EOsVfsRandomness), aBufLen, 0, 0, 0);
	COsLayerData& osLayerData = COsLayerData::Instance();
	const TInt KRandIterations = aBufLen / sizeof(int);
	for(TInt i=0;i<KRandIterations;++i)
		{
		TInt val = Math::Rand(osLayerData.iSeed);
		Mem::Copy(&aBuf[i * sizeof(int)], &val, sizeof(val));
		}
	return KRandIterations * sizeof(int);
	}

/**
SQLite OS porting layer API.

Sleeps for aMicrosec microseconds.

@param aMicrosec The sleep interval in microseconds.

@return The aMicrosec value.
*/
/* static */ int TVfs::Sleep(sqlite3_vfs* aVfs, int aMicrosec)
	{
	__OS_CALL(EOsVfsSleep, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsSleep], ::OsCallProfile(EFalse, EOsVfsSleep), aMicrosec, 0, 0, 0);
	User::AfterHighRes(TTimeIntervalMicroSeconds32(aMicrosec));
	return aMicrosec;
	}

/**
SQLite OS porting layer API.

Retrieves the current date and time.

@param aNow Output parameter, where the data and time value will be stored.
			SQLite processes all times and dates as Julian Day numbers and
			aNow parameter will contain the julian date and time.

@return 0.
*/
/* static */ int TVfs::CurrentTime(sqlite3_vfs* aVfs, double* aNow)
	{
	__OS_CALL(EOsVfsCurrentTime, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsCurrentTime], ::OsCallProfile(EFalse, EOsVfsCurrentTime), 0, 0, 0, 0);
	TTime now;
	now.UniversalTime();
	TDateTime date = now.DateTime();
	TInt year = date.Year();
	TInt month = date.Month() + 1;
	TInt day = date.Day() + 1;
	
    //Calculate the Julian days
	TInt jd = day - 32076 +
	    1461*(year + 4800 + (month - 14)/12)/4 +
	    367*(month - 2 - (month - 14)/12*12)/12 -
	    3*((year + 4900 + (month - 14)/12)/100)/4;
          	
	*aNow = jd;

    // Add the fractional hours, mins and seconds
	*aNow += (date.Hour() + 12.0) / 24.0;
	*aNow += date.Minute() / 1440.0;
	*aNow += date.Second() / 86400.0;
	
	return 0;
	}

/**
SQLite OS porting layer API.

Retrieves a text description of the last OS error.
Note: the method has a default "no-op" implementation at the moment. 

@return 0.
*/
/* static */int TVfs::GetLastError(sqlite3_vfs* aVfs, int /*aBufLen*/, char* /*aBuf*/)
	{
	__OS_CALL(EOsVfsGetLastError, 0, 0);
	__OSTIME_COUNTER(TheOsCallTicks[EOsVfsGetLastError], ::OsCallProfile(EFalse, EOsVfsGetLastError), 0, 0, 0, 0);
	return 0;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////       Memory allocation functions     /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQLite OS porting layer API.

Memory allocation routine.

EXPORT_C required to match IMPORT_C in stdlib.h and avoid compiler warning

@internalComponent
*/
extern "C" EXPORT_C void* sqlite3SymbianMalloc(size_t aSize)
	{
	__MEM_CALL(EMemOpAlloc, aSize, 0);
	return Allocator().Alloc(aSize);
	}

/**
SQLite OS porting layer API.

Memory reallocation routine.

EXPORT_C required to match IMPORT_C in stdlib.h and avoid compiler warning

@internalComponent
*/
extern "C" EXPORT_C void* sqlite3SymbianRealloc(void* aPtr, size_t aSize)
	{
#ifdef _SQLPROFILER
	TInt size = Allocator().AllocLen(aPtr);
	__MEM_CALL(EMemOpRealloc, aSize, size);
#endif
	return Allocator().ReAlloc(aPtr, aSize);
	}

/**
SQLite OS porting layer API.

Memory free routine.

EXPORT_C required to match IMPORT_C in stdlib.h and avoid compiler warning

@internalComponent
*/
extern "C" EXPORT_C void sqlite3SymbianFree(void* aPtr)
	{
#ifdef _SQLPROFILER
	TInt size = Allocator().AllocLen(aPtr);
	__MEM_CALL(EMemOpFree, size, 0);
#endif
	Allocator().Free(aPtr);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////       SQLite init/release functions     ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Registers the single sqlite3_vfs instance ("TheVfsApi" global variable) by calling sqlite3_vfs_register()
*/
extern "C" int sqlite3_os_init(void)
	{
	return sqlite3_vfs_register(&TheVfsApi, 1);//"1" means - make TheVfsApi to be the default VFS object
	}

/**
Unregisters the single sqlite3_vfs instance ("TheVfsApi" global variable) by calling sqlite3_vfs_unregister()
*/
extern "C" int sqlite3_os_end(void)
	{
	return sqlite3_vfs_unregister(&TheVfsApi);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//SQLITE_OS_SYMBIAN

