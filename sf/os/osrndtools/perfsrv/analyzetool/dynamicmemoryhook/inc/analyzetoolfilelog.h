/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of the class TATDriveInfo.
*
*/

#ifndef ANALYZETOOLFILELOG_H_
#define ANALYZETOOLFILELOG_H_

#include <e32base.h>
#include <s32file.h> //RFileWriteStream 
#include <f32file.h> //RFs
#include <analyzetool/atcommon.h>

class RATFileLog : public CBase
	{
    enum EMessageType{
        EFileVersion = 0,
        		EProcessStart,
                EProcessEnd,
                EThreadStart,
                EThreadEnd,
                EDllLoad,
                EDllUnload,
                EAllocH,
                EAllocF,
                EFreeH,
                EFreeF,
                EReallocH,
                EReallocF,
                ETestStart,
                ETestEnd,
                EHandleLeak,
                EDeviceInfo,
                EError  
	    };
    
public:
    
    RATFileLog();
	virtual ~RATFileLog();

/**
 * TBD
 * @return KErrNone.
*/
	
TInt ATFileLogVersion();

/**
 * TBD
 * @param aProcessName The name of the new process started. The length of this
 *   descriptor must not be greater than KMaxProcessName.
 * @param aIsDebug Determines whether a binary is UDEB or UREL
 * @return KErrNone.
*/

TInt ATFileLogProcessStarted( const TDesC8& aProcessName,
                                      TUint aProcessId,
                                      TUint32 aIsDebug,
                                      const TDesC8& aAtoolVersion,
                                      const TDesC8& aApiVersion );

/**
 * TBD 
 * @param aProcessId The ID number of the process ended.
 * @param aHandleLeakCount Number of handles open.
 * @return KErrNone, if successful; otherwise one of the other
 *   system-wide error codes.
 */
TInt ATFileLogProcessEnded( TUint aHandleLeakCount );

/**
 * TBD
 * @param aDllName The name of the new DLL loaded. The length of this descriptor
 *   must not be greater than KMaxLibraryName.
 * @param aStartAddress The start address of the DLL loaded.
 * @param aEndAddress The end address of the DLL loaded.
 * @return KErrNone.
*/
TInt ATFileLogDllLoaded( const TDesC8& aDllName, TUint32 aStartAddress,
                                TUint32 aEndAddress );

/**
 * TBD
 * @param aDllName The name of the DLL to be unloaded. The length of this
 *   descriptor must not be greater than KMaxLibraryName.
 * @param aStartAddress The start address of the DLL to be unloaded.
 * @param aEndAddress The end address of the DLL to be unloaded.
 * @return KErrNone.
*/
TInt ATFileLogDllUnloaded( const TDesC8& aDllName, TUint32 aStartAddress,
                                       TUint32 aEndAddress );

/**
 * TBD
 * @param aMemAddress The memory location where memory has been allocated.
 * @param aCallstack An array including the current call stack.
 * @param aSize The size of the newly allocated memory chunk.
 * @param aThreadId ID of the thread
 * @return KErrNone.
*/
TInt ATFileLogMemoryAllocated( TUint32 aMemAddress,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize,  TUint aThreadId  );
        
/**
 * TBD
 * @param aMemAddress The memory location where memory has been deallocated.
 * @param aFreeCallstack An array including the current call stack.
 * @param aThreadId ID of the thread
 * @return KErrNone.
*/
TInt ATFileLogMemoryFreed( TUint32 aMemAddress, 
		                      TFixedArray<TUint32, KATMaxFreeCallstackLength>& aFreeCallstack,
                              TUint aThreadId );

/**
 * TBD
 * @param aMemAddressFree The memory location where memory has been deallocated.
 * @param aMemAddressAlloc The memory location where memory has been allocated.
 * @param aFreeCallstack An array including the current call stack.
 * @param aThreadId ID of the thread
 * @return KErrNone.
*/
TInt ATFileLogMemoryReallocated( TUint32 aMemAddressFree,  TUint32 aMemAddressAlloc,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize,  TUint aThreadId  );

/**
 * TBD
 * @param aThreadID An ID of the new thread started
 * @return KErrNone.
*/
TInt ATFileLogThreadStarted( TUint aThreadId );

/**
 * TBD
 * @param aThreadID An ID of the thread ended
 * @return KErrNone.
*/
TInt ATFileLogThreadEnded( TUint64 aThreadId );

/**
 * TBD
 * @param aS60Version S60 version
 * @aChecksum ROM checksum
 * @return KErrNone.
*/
TInt ATFileLogDeviceInfo( const TDesC8& aS60Version, const TDesC8& aChecksum);

/**
* Opens a file server session and a file with the name specified.
* @param aFileName The name of the file into which to log.
* @param aProcessName Current process name.
* @return KErrNone, if successful; otherwise one of the other system wide
*   error codes.
*/
TInt OpenFsAndFile( const TDesC& aFileName, const TDesC& aFilePath, const TDesC8& aProcessName );

/**
* Method is used to parse file name extension.
* @param aFileName The name of the file into which to log.
* @param aExtension Parsed file extension. 
*/
void ParseExtension( TDes& aFileName, TDes& aExtension );

/**
* Called internally when need generate new file name.
* @param aFileName The name of the file into which to log.
* @param aProcessName Current process name.
*/
void GenerateNewFileName( TDes& aFileName, const TDesC8& aProcessName );

/**
* Method is used to check that file exists and is valid.
* @param aFileName The name of the file into which to log.
*/
void CheckIfFileAlreadyExist( const TDes& aFileName );

/**
* Closes the handles to the file server session and to the file (if currently open).
*/
void CloseFsAndFile();

// Local time function.
TInt64 CurrentTime();



/**
* A variable telling the number of microseconds from January 1st, 0 AD
* nominal Gregorian to January 1st, 1970 AD nominal Gregorian. 
*/
TInt64 iMicroSecondsAt1970;

/** A handle to a file server */
RFs iFileServer;

/** A handle to a file write stream */
RFileWriteStream iFile;

/** The logging file of this process */
TBuf8<KMaxFileName> iLogFile;

/** A member variable for storing results of server function calls. */ 
TInt iError;


};

#endif /* ANALYZETOOLFILELOG_H_ */
