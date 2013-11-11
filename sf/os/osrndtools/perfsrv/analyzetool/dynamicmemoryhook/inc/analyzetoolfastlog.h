/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#ifndef __ANALYZETOOLFASTLOG_H__
#define __ANALYZETOOLFASTLOG_H__

// INCLUDES
#include    <e32base.h>
#include    <analyzetool/atcommon.h>
#include    <analyzetool/analyzetooltraceconstants.h>

// Function prototypes.

/**
 * TBD
 * @param aProcessName The name of the new process started. The length of this
 *   descriptor must not be greater than KMaxProcessName.
 * @param aProcessId The ID of the process started.
 * @param aIsDebug Determines whether a binary is UDEB or UREL
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogProcessStarted( const TDesC8& aProcessName,
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
GLREF_C TInt ATFastLogProcessEnded( TUint aProcessId, 
                                TUint aHandleLeakCount );

/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aDllName The name of the new DLL loaded. The length of this descriptor
 *   must not be greater than KMaxLibraryName.
 * @param aStartAddress The start address of the DLL loaded.
 * @param aEndAddress The end address of the DLL loaded.
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogDllLoaded( TUint aProcessId, const TDesC8& aDllName, TUint32 aStartAddress,
                                TUint32 aEndAddress );

/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aDllName The name of the DLL to be unloaded. The length of this
 *   descriptor must not be greater than KMaxLibraryName.
 * @param aStartAddress The start address of the DLL to be unloaded.
 * @param aEndAddress The end address of the DLL to be unloaded.
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogDllUnloaded( TUint aProcessId, const TDesC8& aDllName, TUint32 aStartAddress,
                                       TUint32 aEndAddress );

/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aMemAddress The memory location where memory has been allocated.
 * @param aCallstack An array including the current call stack.
 * @param aSize The size of the newly allocated memory chunk.
 * @param aThreadId ID of the thread
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogMemoryAllocated( TUint aProcessId, TUint32 aMemAddress,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize,  TUint aThreadId  );
        
/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aMemAddress The memory location where memory has been deallocated.
 * @param aFreeCallstack An array including the current call stack.
 * @param aThreadId ID of the thread
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogMemoryFreed( TUint aProcessId, TUint32 aMemAddress, 
                              TFixedArray<TUint32, KATMaxFreeCallstackLength>& aFreeCallstack,
                              TUint aThreadId );

/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aMemAddressFree The memory location where memory has been deallocated.
 * @param aMemAddressAlloc The memory location where memory has been allocated.
 * @param aFreeCallstack An array including the current call stack.
 * @param aThreadId ID of the thread
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogMemoryReallocated( TUint aProcessId, TUint32 aMemAddressFree,  TUint32 aMemAddressAlloc,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize,  TUint aThreadId  );

/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aThreadID An ID of the new thread started
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogThreadStarted( TUint aProcessId, TUint aThreadId );

/**
 * TBD
 * @param aProcessId The ID number of the process ended.
 * @param aThreadID An ID of the thread ended
 * @return KErrNone.
*/
GLREF_C TInt ATFastLogThreadEnded( TUint aProcessId, TUint64 aThreadId );

/**
 * TBD
 * @param aS60Version S60 version
 * @aChecksum ROM checksum
 * @return KErrNone.
*/
TInt ATFastLogDeviceInfo( const TDesC8& aS60Version, const TDesC8& aChecksum);

#endif
