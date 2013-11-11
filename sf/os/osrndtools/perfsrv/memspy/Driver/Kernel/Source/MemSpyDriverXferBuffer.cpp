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

#include "MemSpyDriverXferBuffer.h"

// System includes
#include <u32hal.h>

// Shared includes
#include <memspy/driver/memspydriverobjectsshared.h>
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"



DMemSpyDriverXferBuffer::DMemSpyDriverXferBuffer( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   iDevice( aDevice ), iClientThread( aThread )
    {
	TRACE( Kern::Printf("DMemSpyDriverXferBuffer::DMemSpyDriverXferBuffer() - this: 0x%08x", this ));
    }


DMemSpyDriverXferBuffer::~DMemSpyDriverXferBuffer()
	{
	TRACE( Kern::Printf("DMemSpyDriverXferBuffer::~DMemSpyDriverXferBuffer() - START - this: 0x%08x", this ));

    NKern::ThreadEnterCS();
    delete iBuffer;
    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::~DMemSpyDriverXferBuffer() - END - this: 0x%08x", this ));
	}


TInt DMemSpyDriverXferBuffer::Construct( TInt aSize )
    {
    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::Construct() - START - this: 0x%08x, newSize: %d, iSize: %d", this, aSize, iSize ));
    TInt ret = KErrNone;
    //
    iBuffer = Kern::AllocZ( aSize );
    if  ( iBuffer == NULL )
        {
        ret = KErrNoMemory;
        }
    else
        {
        iSize = aSize;
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::Construct() - END - this: 0x%08x, newSize: %d, iSize: %d", this, aSize, iSize ));
    return ret;
    }



TInt DMemSpyDriverXferBuffer::Size() const
    {
    return iSize;
    }


TInt DMemSpyDriverXferBuffer::GrowBy( TInt aSize )
    {
    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::GrowBy() - START - this: 0x%08x, newSize: %d, iSize: %d", this, iSize + aSize, iSize ));
    //
    TInt ret = KErrNone;
    const TInt newSize = iSize + aSize;
    if  ( newSize > iSize )
        {
        NKern::ThreadEnterCS();
        ret = Kern::SafeReAlloc( iBuffer, iSize, newSize );
        if  ( ret == KErrNone )
            {
            iSize = newSize;
            }
	    NKern::ThreadLeaveCS();
        }
    else
        {
        TRACE( Kern::Printf("DMemSpyDriverXferBuffer::GrowBy() - WARNING - new size would shrink buffer (or have no effect)!" ));
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::GrowBy() - END - this: 0x%08x, err: %d, iSize: %d", this, ret, iSize ));
    return ret;
    }


TInt DMemSpyDriverXferBuffer::EnsureCapacity( TInt aSize )
    {
    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::EnsureCapacity() - START - this: 0x%08x, aSize: %d, iSize: %d", this, aSize, iSize ));
    //
    TInt ret = KErrNone;
    if  ( aSize > iSize )
        {
        const TInt newSize = aSize;
        //
        NKern::ThreadEnterCS();
        ret = Kern::SafeReAlloc( iBuffer, iSize, newSize );
        if  ( ret == KErrNone )
            {
            iSize = newSize;
            }
	    NKern::ThreadLeaveCS();
        }
    else
        {
        TRACE( Kern::Printf("DMemSpyDriverXferBuffer::EnsureCapacity() - WARNING - new size would shrink buffer (or have no effect)!" ));
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverXferBuffer::EnsureCapacity() - END - this: 0x%08x, err: %d, iSize: %d", this, ret, iSize ));
    return ret;
    }


void DMemSpyDriverXferBuffer::Reset()
    {
    memclr( iBuffer, iSize );
    }










TInt DMemSpyDriverXferBuffer::WriteToClient( TDes8* aDestinationPointer, TInt aLength )
    {
    // Check the client has enough space
    TInt err = Kern::ThreadGetDesMaxLength( &iClientThread, aDestinationPointer );
    if  ( err >= 0 )
        {
        if  ( aLength > err )
            {
            Kern::Printf("DMemSpyDriverXferBuffer::WriteToClient() - client descriptor max len: %d, amount to write: %d => KErrOverflow", err, aLength );
            err = KErrOverflow;
            }
        else
            {
            const TPtrC8 pData( (const TUint8*) iBuffer, aLength );
            err = Kern::ThreadDesWrite( &iClientThread, aDestinationPointer, pData, 0, KChunkShiftBy0 | KTruncateToMaxLength, &iClientThread );
            }
        }

    // Check for bad descriptor and panic the client
    if  ( err == KErrBadDescriptor )
        {
        MemSpyDriverUtils::PanicThread( iClientThread, EPanicBadDescriptor );
        }
    //
    return err;
    }


DThread& DMemSpyDriverXferBuffer::ClientThread()
    {
    return iClientThread;
    }


TUint8* DMemSpyDriverXferBuffer::Ptr()
    {
    return (TUint8*) iBuffer;
    }






