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

#include "MemSpyDriverLogChanRawMemory.h"

// System includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"



DMemSpyDriverLogChanRawMemory::DMemSpyDriverLogChanRawMemory( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanRawMemory::DMemSpyDriverLogChanRawMemory() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanRawMemory::~DMemSpyDriverLogChanRawMemory()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanRawMemory::~DMemSpyDriverLogChanRawMemory() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanRawMemory::~DMemSpyDriverLogChanRawMemory() - END - this: 0x%08x", this ));
	}



TInt DMemSpyDriverLogChanRawMemory::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeRawMemoryRead:
		    r = ReadMem( (TMemSpyDriverInternalReadMemParams*) a1);
		    break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanRawMemory::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeRawMemoryBase && aFunction < EMemSpyDriverOpCodeRawMemoryEnd );
    }









TInt DMemSpyDriverLogChanRawMemory::ReadMem( TMemSpyDriverInternalReadMemParams* aParams )
	{
    TRACE( Kern::Printf("DMemSpyDriverLogChanRawMemory::ReadMem() - START"));
	TMemSpyDriverInternalReadMemParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof( TMemSpyDriverInternalReadMemParams ) );
    //
    if  ( r == KErrNone )
        {
        NKern::ThreadEnterCS();

        // Get user side descriptor length info
        TInt destLen = 0;
        TInt destMax = 0;
        TUint8* destPtr = NULL;
        r = Kern::ThreadGetDesInfo( &ClientThread(), params.iDes, destLen, destMax, destPtr, ETrue );
        TRACE( Kern::Printf("DMemSpyDriverLogChanRawMemory::ReadMem - user side descriptor: 0x%08x (0x%08x), len: %8d, maxLen: %8d, r: %d", params.iDes, destPtr, destLen, destMax, r ));
        //
        r = OpenTempObject( params.iTid, EThread );
        //
	    if  ( r == KErrNone )
		    {
		    r = Kern::ThreadRawRead( (DThread*) TempObject(), (TAny*) params.iAddr, destPtr, destMax );
            //
		    if  ( r == KErrNone )
                {
                r = destMax;
                }
            else
                {
        	    Kern::Printf( "DMemSpyDriverLogChanRawMemory::ReadMem - could not xfer memory" );
                }
            //
		    CloseTempObject();
		    }
        else
            {
        	Kern::Printf( "DMemSpyDriverLogChanRawMemory::ReadMem - could not open thread: %d", params.iTid );
            }
        //
	    NKern::ThreadLeaveCS();
        }
    else
        {
    	Kern::Printf( "DMemSpyDriverLogChanRawMemory::ReadMem - params read error" );
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanRawMemory::ReadMem() - END - r: %d", r));
	return r;
	}
