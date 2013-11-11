/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  osa interface declaration
*
*/

/*
* %version: 6 %
*/

#ifndef M_WLANOSA_H
#define M_WLANOSA_H

#include <wlanosaplatform.h>

class MWlanDfc;
class MWlanTimer;

/**
 *  osa interface declaration
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanOsa 
    {

public:

    /* basic trace levels masks */
    enum { KFatalLevel      = 0x01 };
    enum { KErrorLevel      = 0x02 };
    enum { KWarningLevel    = 0x04 };
    enum { KInfoLevel       = 0x08 };

    /* masks for Alloc method */
    enum { KAllocZeroStampMask = 0x01 };

    /* memory type identifier */
    enum TOsaMemoryType
        {
        /* for general purpose */
        ENormalMemory = 0,
        /* DMA suitable memory to be used for accessing the WLAN device */
        EInterconnectMemory 
        };

    /**
     * Memory allocation. 
     * Correct alignment guaranteed
     *
     * @since S60 v3.2
     * @param aOsaMemoryType memory type to be allocated
     * @param aSize size of the buffer to be allocated in bytes. Must be 
     *              positive otherwise the allocation fails
     * @param aFlags bitmask to fine-tune behavior of the allocation
     *               bit 0 - set: zero stamp allocated memory
     * @return begin of the allocated memory, NULL upon failure
     */
    virtual void* Alloc( TOsaMemoryType aOsaMemoryType, 
                         TInt aSize, 
                         TUint aFlags ) = 0;

    /**
     * Releases memory allocated by the Alloc method
     *
     * @since S60 v3.2
     * @param aPtr begin of the buffer to be freed
     */
    virtual void Free( void* aPtr ) = 0;

    /**
     * Creates DFC context object 
     *
     * @since S60 v3.2
     * @return DFC context object, NULL upon failure
     */
    virtual MWlanDfc* DfcCreate() = 0;

    /**
     * Destroys DFC context object 
     *
     * @since S60 v3.2
     * @param aWlanDfc DFC context object to be destroyed
     */
    virtual void DfcDestroy( MWlanDfc* aWlanDfc ) = 0;

    /**
     * Creates timer context object
     *
     * @since S60 v3.2
     * @return timer context object, NULL upon failure
     */
    virtual MWlanTimer* TimerCreate() = 0;

    /**
     * Destroys timer context object
     *
     * @since S60 v3.2
     * @param aWlanTimer timer context object to be destroyed
     */
    virtual void TimerDestroy( MWlanTimer* aWlanTimer ) = 0;

    /**
     * Prints a formatted string to the debug port
     * 
     * @since S60 v3.2
     * @param aLevelMask bitmask to define level of the trace
     * @param aFmt a null terminated ANSI 8 bit printf format specifier. 
     * The format specifier dictates what arguments follow 
     * @param A list of arguments as defined by the aFmt specifier. 
     * This must not be longer than 256 characters
     */
    static void FormattedPrint( TUint32 aLevelMask, 
                                const TInt8* aFmt, 
                                ... );

    /**
     * Dumps bytestream in hexadecimal format to the debug port
     * 
     * @since S60 v3.2
     * @param aLevelMask bitmask to define level of the trace
     * @param aData begin of the data
     * @param aDataSize size of the data
     */
    static void Dump( TUint32 aLevelMask, 
                      const TInt8* aData, 
                      TInt aDataSize );

    /**
     * Evaluates an boolean expression and executes an assertion, 
     * that stops the execution of the program, 
     * in case the evaluation yields to false 
     * 
     * @since S60 v3.2
     * @param aFile file name to be traced
     * @param aLine line number to be traced
     * @param aExpression expression to be evaluated
     */
    static void Assert( const TInt8* aFile, 
                        TInt aLine, 
                        TBool aExpression = EFalse );

    /**
     * compares two memory buffers for equality 
     * The two buffers are considered equal only if
     * the binary content of both buffers is the same.
     * 
     * @since S60 v3.2
     * @param aLhs start address of the first buffer in the comparison
     * @param aRhs start address of the second buffer in the comparison
     * @param aNumOfBytes number of bytes to compare
     * @return zero if both buffers are equal; non-zero, otherwise. 
     */
    static TInt MemCmp( const void* aLhs, const void* aRhs, TInt aNumOfBytes );

    /**
     * copies bytes in memory, target and source address can overlap
     * 
     * @since S60 v3.2
     * @param aDest target address
     * @param aSrc source address
     * @param aLengthinBytes number of bytes to be moved
     * @return target address
     */
    static void* MemCpy( void* aDest, const void* aSrc, TUint aLengthinBytes );

    /**
     * copies bytes in memory, target and source address can overlap 
     * Assumes that the addresses are aligned on TInt boundaries,
     * and that the aLengthinBytes value is a multiple of sizeof(TInt).
     * 
     * @since S60 v3.2
     * @param aDest target address
     * @param aSrc source address
     * @param aLengthinBytes number of bytes to be moved
     * @return target address
     */
    static void* WordCpy( TInt* aDest, 
                          const TInt* aSrc, 
                          TUint aLengthinBytes );

    /**
     * sets the specified number of bytes to binary zero
     * 
     * @since S60 v3.2
     * @param aDest target address
     * @param aSrc source address
     * @param aLengthinBytes number of bytes to be set
     * @return target address
     */
    static void* MemClr( void* aDest, TUint aLengthinBytes );

    /**
     * sets all of the specified number of bytes to the specified fill value
     * 
     * @since S60 v3.2
     * @param aDest start address
     * @param aValue fill value
     * @param aCount number of bytes to be set
     * @return start address
     */
    static void* MemSet( void* aDest, TInt aValue, TUint aCount );

    /**
     * gets the current system time in microseconds
     * 
     * @since S60 v3.2
     * @return current system time in microseconds
     */
    static TInt64 Time();

    /**
     * busy-wait for a length of time specified by 
     * the param aWaitTimeInMicroSeconds
     * 
     * @since S60 v3.2
     * @param aWaitTimeInMicroSeconds the 
     * length of time to wait in microseconds
     */
    static void BusyWait( TUint aWaitTimeInMicroSeconds );

    };

#endif // M_WLANOSA_H
