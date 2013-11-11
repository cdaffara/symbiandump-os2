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
// The Symbian OS porting layer - multi-threaded implementation. 
// Platform dependend implementation of the static mutexes and the file session API.
// 
//

/**
 @file
*/
#include "os_symbian.h"
#include <pls.h>
#include <e32std.h>
#include "SqliteUtil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "os_symbian_emulTraces.h"
#endif
#include "SqliteTraceDef.h"

#ifdef SQLITE_OS_SYMBIAN

const TUid KSqliteUid = {0x10285A79};//See UID3 in SQLite3.mmp file - it should be the same

/**
This class describes an object that serves as a container for SQLite global variables
that are stored in an allocated WSD buffer.
The global variables are organised in a map, where the pointer to the initial global variable value
is used as a key.
A hash table is used to speed-up the key search.
A single TWsdMap object is created and stored in the process local storage - TPls.

@see TPls

@internalComponent
*/
NONSHARABLE_CLASS(TWsdMap)
	{
	/**
	Hash table entry.
	*/
	struct TPair
		{
		const TUint8* 	iKey;	//Global variable - key (a pointer the initial variable value)
		TUint8* 		iData;	//Global variable - data
		};
		
public:
	enum {KBufferSize = 4096};		//WSD buffer size in bytes
	enum {KMaxEntries = 37}; 		//Max No. of entries in the hash table - prime number
	
	TWsdMap() :
		iNext(iBuffer),
		iSize(0)
		{
		Mem::FillZ(iTable, sizeof(iTable));
		}	
	/**
	Performs a search in the map for a global variable with aKey key.
	If there is no such variable in the map, the variable will be added and the new variable will be initialized
	with the data of aLength length, pointed by aKey.
	@param aKey    Global variable key
	@param aLength Global variable data length in bytes
	@return Pointer to the global variable (located in the WSD buffer)
	@panic SqliteMt  9 The global variables map is full
	@panic SqliteMt 10 There is no space in the WSD buffer for the new variable
	*/		
	TUint8* Find(const TUint8* aKey, TInt aLength)
		{
		TInt idx = Hash((TUint)aKey);
		TInt cnt = 0;
		while(iTable[idx].iKey != aKey && iTable[idx].iKey != NULL && ++cnt < KMaxEntries)
			{
			idx = ++idx % KMaxEntries;
			}
		__ASSERT_ALWAYS(cnt < KMaxEntries, __SQLITEPANIC2(ESqliteOsPanicMaxKeysExceeded));
		if(!iTable[idx].iKey)
			{
			Add(idx, aKey, aLength);
			}
		return iTable[idx].iData;
		}
		
private:	
	/**
	Hash function. Calculates the index of the global variable key in the hash table.
	@param aKey Global variable key (casted "const TUint8*" to "TUint")
	@return Hash table index
	*/	
	TUint Hash(TUint aKey) const
		{
		return (aKey * (aKey + 3)) % KMaxEntries;
		}
	/**
	Adds a new global variable to the WSD buffer and initializes a new entry in the hash table for the key.
	@param aIdx    The entry index in the hash table
	@param aKey    Global variable key
	@param aLength Global variable data length in bytes
	@panic SqliteMt 10 There is no space in the WSD buffer for the new variable
	*/
	void Add(TInt aIdx, const TUint8* aKey, TInt aLength)
		{
		__ASSERT_ALWAYS((iSize + aLength) <= KBufferSize, __SQLITEPANIC2(ESqliteOsPanicBufferSizeExceeded));
		//Add new entry to the hash table and the intial value to the WSD buffer
		iTable[aIdx].iKey = aKey;
		iTable[aIdx].iData = iNext;
		iNext = Mem::Copy(iNext, aKey, aLength);
		iSize += aLength;
		//////////////// DEBUG prints   /////////////////
		//for(TInt i=0;i<KMaxEntries;++i)	
		//	{
		//	RDebug::Print(_L("%d %X %X %d\r\n"), i, (TUint)iTable[i].iKey, (TUint)iTable[i].iData, Hash((TUint)iTable[i].iKey));	
		//	}
		}
		
private:
	TUint8	iBuffer[KBufferSize]; 	//WSD buffer
	TUint8*	iNext; 					//Points to the next entry in the buffer	
	TInt	iSize;  				//Amount of buffer currently allocated in bytes
	TPair	iTable[KMaxEntries];	//Hash table matching the address of global varaibles to the address of the values in the WSD buffer	 
		
	};

/**
"Process local storage" structure, used for managing SQLite global variables.

@see TStaticFs 
@see TStaticMutex
@see TWsdMap
@see PlsInitialize

@internalComponent
*/
NONSHARABLE_STRUCT(TPls)
	{
	TStaticFs		iStaticFs;
	TStaticMutex 	iStaticMutex[KStaticMutexCount];
	TWsdMap			iWsdMap;
	sqlite3_vfs		iVfsApi;
	};

/**
This is a callback function used by the Pls() call to get the process local storage initialized.
The function initializes the TPls data members.

@param aPls A pointer to the process local storage.
@return KErrNone The process local storage has been successfully initialized.

@panic SqliteMt 1 Failed to connect the RFs
@panic SqliteMt 2 Failed to create the static mutexes
@panic SqliteMt 4 Null aPls parameter value

@internalComponent
*/
static TInt PlsInitialize(TPls* aPls)
	{
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, PLSINITIALIZE_ENTRY, "OS-Entry;0;PlsInitialize"));
	__ASSERT_ALWAYS(aPls != NULL, __SQLITEPANIC2(ESqliteOsPanicNullPls1));
	//Global RFs object
	TInt err = aPls->iStaticFs.Connect();
	SQLITE_TRACE_OS(OstTrace1(TRACE_INTERNALS, PLSINITIALIZE1, "OS;0;PlsInitialize;iStaticFs.Connect() err=%d", err));
	__ASSERT_ALWAYS(err == KErrNone, __SQLITEPANIC2(ESqliteOsPanicFsCreationError));
	//Static mutexes
	TInt idx = 0;
	for(;idx<(sizeof(aPls->iStaticMutex)/sizeof(aPls->iStaticMutex[0])) && err==KErrNone;++idx)
		{
		err = aPls->iStaticMutex[idx].Create();
		}
	SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, PLSINITIALIZE2, "OS;0;PlsInitialize;iStaticMutex[%d].Create() err=%d", idx, err));
	__ASSERT_ALWAYS(err == KErrNone , __SQLITEPANIC2(ESqliteOsPanicMutexCreationError));
	//WSD map
	//...already initialized by its constructor
	//sqlite3_vfs object	
	aPls->iVfsApi.iVersion 		= 1;
	aPls->iVfsApi.szOsFile 		= sizeof(TDbFile);
	aPls->iVfsApi.mxPathname 	= KMaxFileName;
	aPls->iVfsApi.pNext 		= NULL;
	aPls->iVfsApi.zName 		= "SymbianSqliteMt";
	aPls->iVfsApi.pAppData 		= NULL;
	aPls->iVfsApi.xOpen 		= &TVfs::Open;
	aPls->iVfsApi.xDelete 		= &TVfs::Delete;
	aPls->iVfsApi.xAccess 		= &TVfs::Access;
	aPls->iVfsApi.xFullPathname	= &TVfs::FullPathName;
	aPls->iVfsApi.xDlOpen 		= NULL;
	aPls->iVfsApi.xDlError 		= NULL;
	aPls->iVfsApi.xDlSym 		= NULL;
	aPls->iVfsApi.xDlClose 		= NULL;
	aPls->iVfsApi.xRandomness 	= &TVfs::Randomness;
	aPls->iVfsApi.xSleep 		= &TVfs::Sleep;
	aPls->iVfsApi.xCurrentTime 	= &TVfs::CurrentTime;
	aPls->iVfsApi.xGetLastError	= &TVfs::GetLastError;
	
	SQLITE_TRACE_OS(OstTrace0(TRACE_INTERNALS, PLSINITIALIZE_EXIT, "OS-Exit;0;PlsInitialize"));
	return KErrNone;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TStaticFs  /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TStaticFs::TStaticFs()
	{
	}

/**
Returns a reference to the already created RFs object that is located in the process local storage.

@return RFs reference

@panic SqliteMt 3 Invalid RFs handle
@panic SqliteMt 5 Process local storage initialization failure
*/
RFs& TStaticFs::Fs()
	{
	TPls* pls = ::Pls(KSqliteUid, &PlsInitialize);
	__ASSERT_ALWAYS(pls != 0, __SQLITEPANIC2(ESqliteOsPanicNullPls2));
	__ASSERT_DEBUG(pls->iStaticFs.iFs.Handle() != KNullHandle, __SQLITEPANIC2(ESqliteOsPanicInvalidFs));
	return pls->iStaticFs.iFs;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TStaticMutex  //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TStaticMutex::TStaticMutex()
	{
	}

sqlite3_mutex* StaticMutex(TInt aType)
	{
	TPls* pls = ::Pls(KSqliteUid, &PlsInitialize);
	__ASSERT_ALWAYS(pls != 0, __SQLITEPANIC2(ESqliteOsPanicNullPls3));
	__ASSERT_ALWAYS((TUint)aType < (sizeof(pls->iStaticMutex)/sizeof(pls->iStaticMutex[0])), __SQLITEPANIC2(ESqliteOsPanicInvalidMutexType));
	return &pls->iStaticMutex[aType];
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////    sqlite3_wsd    /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
The implementation of this function does not do anything special apart from checking that the requested WSD buffer size and
global variables count (WSD entries) are not bigger than the max TWsdMap buffer size and max TWsdMap entry count.

@param aWsdBufSize    SQLite expects the size of the WSD buffer to be "aWsdBufSize" bytes
@param aWsdEntryCount SQLite can place in the WSD buffer up to "aWsdEntryCount" global variables.

@panic SqliteMt 12 The requested WSD buffer is too big. Bigger than TWsdMap::KBufferSize.
@panic SqliteMt 13 The requested global variables count is too big. Bigger than TWsdMap::KMaxEntries.

@see TWsdMap
*/
int sqlite3_wsd_init(int aWsdBufSize, int aWsdEntryCount)
	{
	__ASSERT_ALWAYS(aWsdBufSize <= TWsdMap::KBufferSize, __SQLITEPANIC2(ESqliteOsPanicWsdBufSize));
	__ASSERT_ALWAYS(aWsdEntryCount <= TWsdMap::KMaxEntries, __SQLITEPANIC2(ESqliteOsPanicWsdEntryCount));
	return SQLITE_OK;	
	}

/**
Performs a search in the WSD map (in the process local storage) for a global variable identified by the aKey parameter.

@param aKey    Global variable key
@param aLength Global variable data length in bytes
@return        Pointer to the global variable data

@panic SqliteMt 11 Process local storage initialization failure

@see TWsdMap
@see TPls
*/
void* sqlite3_wsd_find(void* aKey, int aLength)
	{
	__ASSERT_ALWAYS(aKey != NULL, __SQLITEPANIC2(ESqliteOsPanicNullKey));
	return ::Pls(KSqliteUid, &PlsInitialize)->iWsdMap.Find(static_cast <const TUint8*> (aKey), aLength);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  sqlite3_vfs     ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sqlite3_vfs* VfsApi()
	{
	TPls* pls = ::Pls(KSqliteUid, &PlsInitialize);
	__ASSERT_ALWAYS(pls != 0, __SQLITEPANIC2(ESqliteOsPanicNullPls4));
	return &pls->iVfsApi;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif SQLITE_OS_SYMBIAN
