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

#include "MemSpyDriverLogChanStack.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverSuspensionManager.h"

// Constants
const TUint32 KMemSpyStackFillPatternUser = 0x29292929;
const TUint32 KMemSpyStackFillPatternSupervisor = 0xeeeeeeee;


DMemSpyDriverLogChanStack::DMemSpyDriverLogChanStack( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::DMemSpyDriverLogChanStack() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanStack::~DMemSpyDriverLogChanStack()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanStack::~DMemSpyDriverLogChanStack() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanStack::~DMemSpyDriverLogChanStack() - END - this: 0x%08x", this ));
	}






TInt DMemSpyDriverLogChanStack::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeStackGetInfo:
            r = GetStackInfo( (TUint) a1, (TMemSpyDriverStackInfo*) a2 );
            break;
	    case EMemSpyDriverOpCodeStackGetData:
            r = GetStackData( (TMemSpyDriverInternalStackDataParams*) a1 );
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanStack::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeStackBase && aFunction < EMemSpyDriverOpCodeStackEnd );
    }









TInt DMemSpyDriverLogChanStack::GetStackInfo( TUint aTid, TMemSpyDriverStackInfo* aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackInfo() - START - thread id: %d", aTid));

    TMemSpyDriverStackInfo params;
	TInt r = OpenTempObject( aTid, EThread );
	if  ( r == KErrNone )
		{
        DThread* thread = (DThread*) TempObject();

        // Check the threads in the process are suspended
        if  ( SuspensionManager().IsSuspended( *thread ) )
            {
            DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();
            //
            GetStackPointers( thread, params.iSupervisorStackPointer, params.iUserStackPointer );
            //
            params.iUserStackBase = threadAdaption.GetUserStackBase( *thread );
            params.iUserStackSize = threadAdaption.GetUserStackSize( *thread );
            params.iSupervisorStackBase = threadAdaption.GetSupervisorStackBase( *thread );
            params.iSupervisorStackSize = threadAdaption.GetSupervisorStackSize( *thread );

            // Try to get watermarks
            GetStackHighWatermark( *thread, params.iUserStackHighWatermark, EMemSpyDriverDomainUser, KMemSpyStackFillPatternUser );
            GetStackHighWatermark( *thread, params.iSupervisorStackHighWatermark, EMemSpyDriverDomainKernel, KMemSpyStackFillPatternSupervisor ) ;
        
    #ifdef __WINS__
            params.iUserStackPointer = params.iUserStackBase;
            params.iUserStackHighWatermark = params.iUserStackBase;
            params.iSupervisorStackPointer = params.iSupervisorStackBase;
            params.iSupervisorStackHighWatermark = params.iSupervisorStackBase;
    #endif

            // Write data to user-side
            r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverStackInfo) );
            }
        else
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackInfo - parent process not suspended => KErrAccessDenied"));
            r = KErrAccessDenied;
            }

        CloseTempObject();
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackInfo() - END - ret: %d", r));
    return r;
    }


TInt DMemSpyDriverLogChanStack::GetStackData( TMemSpyDriverInternalStackDataParams* aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData() - START"));
	TMemSpyDriverInternalStackDataParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalStackDataParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData() - END - params read error: %d", r));
        return r;
        }

    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - thread id: %d, remaining: %8d", params.iTid, params.iRemaining));

    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();

    r = OpenTempObject( params.iTid, EThread );
	if  ( r == KErrNone )
		{
        // Find the correct thread...
        DThread* thread = (DThread*) TempObject();
        DProcess* process = threadAdaption.GetOwningProcess( *thread );
	    TFullName fullName;
        thread->FullName( fullName );
	    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - thread: %lS", &fullName));

        // Check the threads in the process are suspended
        const TBool isSuspended = SuspensionManager().IsSuspended( *process );
        TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData() - isSuspended: %d", isSuspended ));
        if  ( isSuspended )
            {
            TInt stackSize = 0;
            TUint32 stackBase = 0;
            TBool stackAvailable = EFalse;
            //
            if  ( params.iDomain == EMemSpyDriverDomainUser )
                {
                stackSize = threadAdaption.GetUserStackSize( *thread );
                stackBase = threadAdaption.GetUserStackBase( *thread );
                }
            else if ( params.iDomain == EMemSpyDriverDomainKernel )
                {
                stackSize = threadAdaption.GetSupervisorStackSize( *thread );
                stackBase = threadAdaption.GetSupervisorStackBase( *thread );
                }
            TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData() - stackAvailable: %d", stackAvailable ));
            //           
            if  ( stackAvailable )
		        {
                // Get user side descriptor length info
                TInt destLen = 0;
                TInt destMax = 0;
                TUint8* destPtr = NULL;
                r = Kern::ThreadGetDesInfo( &ClientThread(), params.iDes, destLen, destMax, destPtr, ETrue );
                TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - user side descriptor: 0x%08x (0x%08x), len: %8d, maxLen: %8d, r: %d", params.iDes, destPtr, destLen, destMax, r ));

                if  ( r == KErrNone )
                    {
                    // Calculate stack starting address. If we want to get the entire stack data
                    // then we use the information from DThread. If we only want the data from
                    // the current SP onwards, then we use GetStackPointerByDomain to return
                    // the current SP value. If we are only fetching a portion of the stack, then
                    // the amount of data we will have to read is obviously reduced.
                    TUint32 stackStartingAddress = 0;
                    if  ( params.iEntireStack )
                        {
                        stackStartingAddress = stackBase;
                        }
                    else
                        {
                        stackStartingAddress = GetStackPointerByDomain( thread, params.iDomain ); 
                        }
                    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - stackStartingAddress:  0x%08x", stackStartingAddress));
                    __ASSERT_ALWAYS( stackStartingAddress != 0, MemSpyDriverUtils::Fault( __LINE__ ) );

                    // Calculate how much data (maximum) there is to read, depending on whether the
                    // client asked for all or just the active part of the stack.
                    TUint stackDataSize = 0;
                    if  ( params.iEntireStack )
                        {
                        stackDataSize = stackSize;
                        }
                    else
                        {
                        stackDataSize = stackBase + stackSize - stackStartingAddress;
                        }
                    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - stackDataSize:           %8d", stackDataSize ));

                    // Deal with the initial case - i.e. whereby the client is requesting stack
                    // data for the first time. In this situation, the magic rune for "first
                    // request" is a remaining value of -1.
                    if  ( params.iRemaining < 0 )
                        {
                        params.iRemaining = stackDataSize;
                        }

                    // The remaining number of bytes should allow us to calculate the position
                    // to read from.
                    const TInt amountToRead = Min( params.iRemaining, destMax );
                    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - amountToRead:            %8d", amountToRead));
                    const TInt readOffset = ( stackDataSize - params.iRemaining );
                    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - readOffset:              %8d", readOffset));
                    const TAny* readAddress = (const TAny*) ( stackStartingAddress + readOffset );
                    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - readAddress:           0x%08x", readAddress));
        
                    // Do the read into user-space
            #ifndef __WINS__
                    if  ( params.iDomain == EMemSpyDriverDomainKernel )
                        {
                        const TPtrC8 pData( (TUint8*) readAddress, amountToRead );
                        r = Kern::ThreadDesWrite( &Kern::CurrentThread(), params.iDes, pData, 0 );
                        }
                    else
                        {
                        r = Kern::ThreadRawRead( thread, readAddress, destPtr, amountToRead );
                        }

                    // Update user-side
                    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - mem. operation result: %d", r));
                    if  ( r == KErrNone )
                        {
                        // Update remaining bytes and write back to user address space
                        params.iRemaining -= amountToRead;
                        r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalStackDataParams) );

                        if  ( r == KErrNone )
                            {
                            // Client takes care of updating descriptor length.
                            r = amountToRead;
                            }
                        }
                    else if ( r == KErrBadDescriptor )
                        {
                        MemSpyDriverUtils::PanicThread( ClientThread(), EPanicBadDescriptor );
                        }
            #else
                    (void) destPtr;
                    params.iRemaining -= amountToRead;
                    r = amountToRead;
                    Kern::Printf( "DMemSpyDriverLogChanStack::GetStackData - not reading data on WINS" );
            #endif
                    }
                else
                    {
                    Kern::Printf( "DMemSpyDriverLogChanStack::GetStackData - error getting client descriptor info" );
                    }
                }
	        else
                {
    	        Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - stack address or stack length is invalid");
                r = KErrArgument;
                }
            }
        else
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - parent process not suspended => KErrAccessDenied"));
            r = KErrAccessDenied;
            }

	    CloseTempObject();
        }
    else
        {
    	Kern::Printf("DMemSpyDriverLogChanStack::GetStackData - thread not found");
		}

	TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackData() - END - ret: %d", r));
    return r;
    }















#ifdef __MARM__

#define __INCLUDE_REG_OFFSETS__  // for SP_R13U in nk_plat.h
#include "nk_plat.h"
#include "arm.h"

void DMemSpyDriverLogChanStack::GetStackPointers( DThread* aThread, TUint& aSupSP, TUint& aUsrSP )
	{
    __ASSERT_ALWAYS( aThread != &Kern::CurrentThread(), MemSpyDriverUtils::Fault( __LINE__ ) );

    // Get NThread associated with DThread
    NThread* nThread = OSAdaption().DThread().GetNThread( *aThread );

    TMemSpyDriverRegSet regs;
    MemSpyDriverUtils::GetThreadRegisters( nThread, regs );
    //
    aSupSP = aThread->iNThread.iSavedSP;
    aUsrSP = regs.iRn[13];
    //
	TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackPointers() - usr: 0x%08x [0x%08x-0x%08x], svc: 0x%08x [0x%08x-0x%08x]", aUsrSP, aThread->iUserStackRunAddress, (TUint) aThread->iUserStackRunAddress + (TUint) aThread->iUserStackSize, aSupSP, aThread->iSupervisorStack, (TUint) aThread->iSupervisorStack + (TUint) aThread->iSupervisorStackSize ));
	}


TLinAddr DMemSpyDriverLogChanStack::GetStackPointerByDomain( DThread* aThread, TMemSpyDriverDomainType aDomainType )
    {
    TUint stackPointer = 0;
    //
    if  ( aDomainType == EMemSpyDriverDomainUser )
        {
        // Get NThread associated with DThread
        NThread* nThread = OSAdaption().DThread().GetNThread( *aThread );
 
        TMemSpyDriverRegSet regSet;
        MemSpyDriverUtils::GetThreadRegisters( nThread, regSet );
        stackPointer = regSet.iRn[13];
        }
    else if ( aDomainType == EMemSpyDriverDomainKernel )
        {
        TUint userSPNotUsed = 0;
        GetStackPointers( aThread, stackPointer, userSPNotUsed );
        }
    //
    return TLinAddr( stackPointer );
    }

#else

void DMemSpyDriverLogChanStack::GetStackPointers( DThread* /*aThread*/, TUint& aSupSP, TUint& aUsrSP )
	{
    aSupSP = 0;
    aUsrSP = 0;
	}


TLinAddr DMemSpyDriverLogChanStack::GetStackPointerByDomain( DThread* aThread, TMemSpyDriverDomainType aDomainType )
    {
    // Just return the base address in WINS
    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();
    TUint32 stackPointer = 0;
    //
    if  ( aDomainType == EMemSpyDriverDomainUser )
        {
        stackPointer = threadAdaption.GetUserStackBase( *aThread );
        }
    else if ( aDomainType == EMemSpyDriverDomainKernel )
        {
        stackPointer = threadAdaption.GetSupervisorStackBase( *aThread );
        }
    //
    return TLinAddr( stackPointer );
    }

#endif


TInt DMemSpyDriverLogChanStack::GetStackHighWatermark( DThread& aThread, TLinAddr& aHighWatermark, TMemSpyDriverDomainType aDomain, TUint aRune )
    {
    aHighWatermark = 0;
    const TInt KStackPageSize = 512;
    
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark() - START - domain: %d, rune: 0x%08x", aDomain, aRune ));

    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();
   
    const TUint32 baseAddress = aDomain == EMemSpyDriverDomainUser ? threadAdaption.GetUserStackBase( aThread ) : threadAdaption.GetSupervisorStackBase( aThread );
    const TInt size           = aDomain == EMemSpyDriverDomainUser ? threadAdaption.GetUserStackSize( aThread ) : threadAdaption.GetSupervisorStackSize( aThread );
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark - baseAddress: 0x%08x, size: %8d", baseAddress, size));

    TInt r = KErrNone;
    //
	if  ( baseAddress && size )
		{
        aHighWatermark = baseAddress;
        TBuf8<KStackPageSize> stackBuf;
        TUint8* readAddress = NULL;
        //
        while( r == KErrNone )
            {
            // Read a chunk of data
            r = ReadStackData( aThread, stackBuf, readAddress, aDomain );

            // Process the data, looking for the first bytes that aren't 0x29292929
            if  ( r == KErrNone )
                {
                const TInt readLength = stackBuf.Length();
                TRACE_DATA( MemSpyDriverUtils::DataDump("stackdata - %lS", stackBuf.Ptr(), readLength, readLength ) );

                TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark - readLength: %d", readLength));

                for( TInt readPos = 0; readPos < readLength && ((readLength - readPos) >= 4); readPos += 4, aHighWatermark += 4 )
                    {
                    const TUint dword =  stackBuf[ readPos ] +
                                        (stackBuf[ readPos + 1 ] <<  8) + 
                                        (stackBuf[ readPos + 2 ] << 16) + 
                                        (stackBuf[ readPos + 3 ] << 24);
                    //
                    if  ( dword != aRune )
                        {
                        TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark - found end of uninit. stack!"));
                        TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark - dword:          0x%08x", dword));
                        TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark - readPos:          %8d", readPos));
                        TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark - aHighWatermark: 0x%08x", aHighWatermark));
                        TRACE( Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark() - END"));
                        return KErrNone;
                        }
                    }
                }
            }
        }
    //
    aHighWatermark = 0;
    Kern::Printf("DMemSpyDriverLogChanStack::GetStackHighWatermark() - END - error: %d", r);
    return r;
    }


TInt DMemSpyDriverLogChanStack::ReadStackData( DThread& aThread, TDes8& aDestination, TUint8*& aReadAddress, TMemSpyDriverDomainType aDomain )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData() - START - domain: %d", aDomain ));

    DMemSpyDriverOSAdaptionDThread& threadAdaption = OSAdaption().DThread();

    const TUint32 baseAddress = aDomain == EMemSpyDriverDomainUser ? threadAdaption.GetUserStackBase( aThread ) : threadAdaption.GetSupervisorStackBase( aThread );
    const TInt size           = aDomain == EMemSpyDriverDomainUser ? threadAdaption.GetUserStackSize( aThread ) : threadAdaption.GetSupervisorStackSize( aThread );
    const TUint32 topAddress  = ( baseAddress + size );
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - baseAddress:   0x%08x", baseAddress));
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - size:          0x%08x", size));
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - topAddress:    0x%08x", topAddress));
    //
    if  ( aReadAddress == NULL )
        {
        aReadAddress = (TUint8*) baseAddress; 
        }

    // Work out how much we should read
    TInt readLen = Min( aDestination.MaxLength(), topAddress - (TLinAddr) aReadAddress );
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - aReadAddress:  0x%08x", aReadAddress));
    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - readLen:         %8d", readLen));

    TInt r = KErrNotSupported;
    aDestination.Zero();

#ifndef __WINS__
    if  ( aDomain == EMemSpyDriverDomainKernel )
        {
        const TPtrC8 pData( (TUint8*) aReadAddress, readLen );
        aDestination.Copy( pData );
        r = KErrNone;
        readLen = aDestination.Length();
        }
    else
        {
        r = Kern::ThreadRawRead( &aThread, aReadAddress, (TAny*) aDestination.Ptr(), readLen );
        }

    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - read result: %d", r));
    if  (r == KErrNone)
        {
        aDestination.SetLength( readLen );
        aReadAddress += aDestination.Length();
        }
#else
    Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData - not reading data on WINS");
#endif

    TRACE( Kern::Printf("DMemSpyDriverLogChanStack::ReadStackData() - END - ret: %d", r));
    return r;
    }




