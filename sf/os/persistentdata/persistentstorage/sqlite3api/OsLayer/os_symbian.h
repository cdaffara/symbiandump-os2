/**
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declarations for the multi-threaded Symbian OS porting layer.
* 
*
*/



/**
 @file
*/
#ifndef OS_SYMBIAN_H
#define OS_SYMBIAN_H

#include <sqlite3.h>
#include "FileBuf64.h"

#ifdef SQLITE_OS_SYMBIAN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TStaticFs  /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
This class encapsulates a RFs object. 
Only one RFs object is created per process.
The class has a build dependent implementation:
 - a single global static TStaticFs variable is defined for the hardware builds;
 - the process local storage (TPls) is used for storing TStaticFs object for the emulator builds;

@see TPls

@internalComponent
*/
NONSHARABLE_CLASS(TStaticFs)
	{
public:	
	TStaticFs();		//Build dependent implementation
	TInt Connect();
	static RFs& Fs();	//Build dependent implementation
private:
	RFs	iFs;	
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  sqlite3_mutex  /////////////////////////////////////////////////////////////////////////////////////            
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const TInt KStaticMutexCount = SQLITE_MUTEX_STATIC_LRU2 - 1;//"-1": excluding fast and recoursive mutex types

/**
This class describes a mutex object. 
SQLite uses three mutex types: static, fast and recursive.

The static mutexes are expected to be created prior first SQLite function call.
The OS porting layer provides build dependent implementation for the static mutextes:
 - The static mutexes are defined as global variables for the hardware builds.
   The mutex creation is performed in the global variable's constructor. 
   If the mutex creation fails, the program will be terminated;
 - The static mutexes are stored in the process local storage for the emulator builds.
   If the mutex creation fails, the program will be terminated;

The difference between fast and recursive mutextes is that the recursive mutexes can be entered mutiple times
by the same thread. The OS porting layer makes no difference between fast and recursive mutexes at the moment.
Whether the SQLite requests fast or ecursive mutex, a recursive mutex will be created.
The recursive mutex creation can fail, in which case the error will be reported back to the caller.

Note that even though sqlite3_mutex has virtual methods, it is not declared as a standard Symbian OS "C" class 
because sqlite3_mutex is an abstract type, externally declared and used by SQLite (SQLite is a C library).
SQLite deals only with pointers to sqlite3_mutex objects. See the declaration in sqlite3.h file. 

@see TPls
@see CRecursiveMutex
@see TStaticMutex

@internalComponent
*/
NONSHARABLE_CLASS(sqlite3_mutex)
	{
public:
	sqlite3_mutex();
	TInt 	Create();
	virtual ~sqlite3_mutex();
	void 	Enter();
	void	Leave();
	TBool	IsHeld() const;
private:	
	TInt		iRefCount;
	TThreadId	iOwnerThreadId;
	RMutex		iMutex;
	};

/**
sqlite3_mutex derived class. Describes a recursive mutex.
The mutex creation can fail, the error will be reported back to the caller.

This is not a standard Symbian OS "C" class, not derived from CBase.
CRecursiveMutex is a specialization of the sqlite3_mutex class, used for recursive mutexes. 

@see sqlite3_mutex

@internalComponent
*/
NONSHARABLE_CLASS(CRecursiveMutex) : public sqlite3_mutex
	{
public:	
	static CRecursiveMutex* New();
	virtual ~CRecursiveMutex();
	
private:
	CRecursiveMutex()
		{
		}
	};

/**
sqlite3_mutex derived class. Describes a static mutex.
If the mutex creation fails, the program will be terminated.

@see sqlite3_mutex

@internalComponent
*/
NONSHARABLE_CLASS(TStaticMutex) : public sqlite3_mutex
	{
public:	
	TStaticMutex();						//Build dependent implementation
	};
	
/**
Returns a pointer to already created static mutex.
The function has build dependet implementation:
 - The static mutexes are defined as global objects for the hardware builds;
 - The static mutexes are stored in th eprocess local storage for the emulator builds;
 
@see TPls
@see sqlite3_mutex

@param  aType The static mutex type
@return sqlite3_mutex pointer

@panic SqliteMt  6 Process local storage initialization failure (the emulator builds only)
@panic SqliteMt 22 The mutex type is bigger than SQLITE_MUTEX_STATIC_LRU2

@internalComponent
*/
sqlite3_mutex* StaticMutex(TInt aType);	//Build dependent implementation

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TMutexApi  ////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Defines mutex API, that is needed by SQLite.

SQLite creates a mutex using the TMutexApi::Alloc() method.
Depending on aType parameter value, either existing static mutex will be used or a new recursive mutex will be created.

@see TheMutexMethods
@see sqlite3_mutex
@see CRecursiveMutex
@see TStaticMutex

@internalComponent
*/
NONSHARABLE_CLASS(TMutexApi)
	{
public:		
	static int Init();
	static int End();
	static sqlite3_mutex* Alloc(int aType);
	static void Free(sqlite3_mutex* aMutex);
	static void Enter(sqlite3_mutex* aMutex);
	static int Try(sqlite3_mutex* aMutex);
	static void Leave(sqlite3_mutex* aMutex);
	static int Held(sqlite3_mutex* aMutex);
	static int Notheld(sqlite3_mutex* aMutex);
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TDbFile  //////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
TDbFile derives from the sqlite3_file structure, adding data members needed for processing the SQLite requests to the OS layer.
When SQLite needs an access to a file, SQLite allocates memory for a new TDbFile instance and passes a pointer to that 
instance to TVfs::Open(). TVfs::Open() creates/opens the file and initializes the TDbFile instance. 
SQLite uses the initialized TDbFile instance (actually SQLite knows and uses the sqlite3_file, the base structure) 
every time when needs to read or write from/to the file, using for that an appropriate TFileIo method.

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
	HBufC*		iFullName;				//Used for the "delete file" operation
	TInt 		iSharedLockByte;
	TInt		iLockType;
	TBool		iReadOnly;
	TInt		iSectorSize;
	TInt		iDeviceCharacteristics;
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TFileIo  //////////////////////////////////////////////////////////////////////////////////////////
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
	static TInt GetReadLock(TDbFile& aDbFile);	
	static TInt UnlockReadLock(TDbFile& aDbFile);	
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  TVfs     //////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@return Pointer to the sqlite3_vfs interface

@see TVfs

@panic SqliteMt 7 Process local storage initialization failure (the emulator builds only)

@internalComponent
*/
sqlite3_vfs* VfsApi();//Platform dependend implementation

/**
TVfs ("VFS" - virtual file system) class offers methods for creating/openning a file, deleting a file,
a "sleep" method, a "time" method, a "rand" method, etc.
SQLite never accesses the OS API directly, only through the API offered by TVfs and TFileIo classes.

@internalComponent

@see TFileIo
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
private:
	static inline TInt DoGetVolumeIoParamInfo(RFs& aFs, TInt aDriveNo, TVolumeIOParamInfo& aVolumeInfo);
	static TInt DoGetDeviceCharacteristics(const TDriveInfo& aDriveInfo, const TVolumeIOParamInfo& aVolumeInfo);
	static TInt DoGetSectorSize(const TDriveInfo& aDriveInfo, const TVolumeIOParamInfo& aVolumeInfo);
	static TInt DoGetDeviceCharacteristicsAndSectorSize(TDbFile& aDbFile, TInt& aRecReadBufSize);
	static TInt DoFileSizeCorruptionCheck(TDbFile& aDbFile, const TDesC& aFname, TInt aFmode);
	};
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//SQLITE_OS_SYMBIAN

#endif//OS_SYMBIAN_H
