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
#include "SqliteUtil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "os_symbian_hrdwTraces.h"
#endif
#include "SqliteTraceDef.h"

#ifdef SQLITE_OS_SYMBIAN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TStaticFs  /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Single global RFs instance

@see TStaticFs

@internalComponent
*/
static TStaticFs TheFs;

/**
Connects the RFs.
If the operation fails, the program will be terminated.

@see TStaticFs
*/
TStaticFs::TStaticFs()
	{
	TInt err = Connect();
	if(err != KErrNone)
		{
		SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TSTATICFS_TSTATICFS, "OS;0x%X;TStaticFs::TStaticFs;err=%d", (TUint)this, err));
		User::Exit(err);	
		}
	}

/**
Returns a reference to the already created global RFs object.

@return RFs reference

@panic SqliteMt 3 Invalid RFs handle

@see TStaticFs
*/
RFs& TStaticFs::Fs()
	{
	__ASSERT_DEBUG(TheFs.iFs.Handle() != KNullHandle, __SQLITEPANIC2(ESqliteOsPanicInvalidFs));
	return TheFs.iFs;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TStaticMutex  //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Global array of static mutexes.

@see TStaticMutex

@internalComponent
*/
static TStaticMutex	TheStaticMutex[KStaticMutexCount];

/**
Creates the static mutexes.
If the creation fails, the program will be terminated.

@see TStaticMutex
*/
TStaticMutex::TStaticMutex()
	{
	TInt err = Create();
	if(err != KErrNone)
		{
		SQLITE_TRACE_OS(OstTraceExt2(TRACE_INTERNALS, TSTATICMUTEX_TSTATICMUTEX, "OS;0x%X;TStaticMutex::TStaticMutex;err=%d", (TUint)this, err));
		User::Exit(err);	
		}
	}

sqlite3_mutex* StaticMutex(TInt aType)
	{
	__ASSERT_ALWAYS((TUint)aType < (sizeof(TheStaticMutex)/sizeof(TheStaticMutex[0])), __SQLITEPANIC2(ESqliteOsPanicInvalidMutexType));
	return &TheStaticMutex[aType];
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  sqlite3_vfs     ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Global sqlite3_vfs object.

@see VfsApi
@see TVfs

@internalComponent
*/
static sqlite3_vfs TheVfsApi = 
	{
	/*iVersion =*/		1,
	/*szOsFile =*/ 		sizeof(TDbFile),
	/*mxPathname =*/ 	KMaxFileName,
	/*pNext =*/ 		NULL,
	/*zName =*/ 		"SymbianSqliteMt",
	/*pAppData =*/ 		NULL,
	/*xOpen =*/ 		&TVfs::Open,
	/*xDelete =*/ 		&TVfs::Delete,
	/*xAccess =*/ 		&TVfs::Access,
	/*xFullPathname =*/ &TVfs::FullPathName,
	/*xDlOpen =*/ 		NULL,
	/*xDlError =*/ 		NULL,
	/*xDlSym =*/ 		NULL,
	/*xDlClose =*/ 		NULL,
	/*xRandomness =*/ 	&TVfs::Randomness,
	/*xSleep =*/ 		&TVfs::Sleep,
	/*xCurrentTime =*/ 	&TVfs::CurrentTime,
	/*xGetLastError =*/ &TVfs::GetLastError
	};

sqlite3_vfs* VfsApi()
	{
	return &TheVfsApi;		
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//SQLITE_OS_SYMBIAN
