/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of RWlanCircularBuffer inline methods.
*
*/

/*
* %version: 2 %
*/

#include <kernel.h> // for Kern::SystemTime()

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline void RWlanCircularBuffer<size>::DoInit()
    {
    iSize = size;
    iPacketAmount = 0;
    iGetIndex = 0;
    iPutIndex = 0;
    iBecameEmptyAt = 0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline void RWlanCircularBuffer<size>::Release()
    {
    // Do nothing
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TBool RWlanCircularBuffer<size>::IsEmpty() const
    {
    return iPacketAmount == 0 ? ETrue : EFalse;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TBool RWlanCircularBuffer<size>::IsFull() const
    {
    return iPacketAmount == iSize ? ETrue : EFalse;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TDataBuffer* RWlanCircularBuffer<size>::GetPacket()
    {
    TDataBuffer* packet = NULL;
    
    if ( !IsEmpty() )
        {
        packet = iBuffer[iGetIndex];
        
        if ( ++iGetIndex == iSize )
            {
            iGetIndex = 0;
            }
        
        if ( --iPacketAmount == 0 )
            {
            iBecameEmptyAt = Kern::SystemTime();
            }
        }
    
    return packet;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TBool RWlanCircularBuffer<size>::PutPacket( TDataBuffer* aPacket )
    {
    TBool ret( EFalse );
    
    if ( !IsFull() )
        {
        iBuffer[iPutIndex] = aPacket;
        
        if ( ++iPutIndex == iSize )
            {
            iPutIndex = 0;
            }
        
        ++iPacketAmount;
        ret = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TDataBuffer* RWlanCircularBuffer<size>::PeekPacket()
    {
    TDataBuffer* packet = NULL;
    
    if ( !IsEmpty() )
        {
        packet = iBuffer[iGetIndex];
        }
    
    return packet;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TUint RWlanCircularBuffer<size>::GetLength() const
    {
    return iPacketAmount;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template <TUint size>
inline TBool RWlanCircularBuffer<size>::IsActive( TInt64 aTimeNow ) const
    {
    const TInt64 KActivityThreshold ( 25000 ); // microseconds
    
    if ( !IsEmpty() )
        {
        return ETrue;
        }
    else
        {
        if ( aTimeNow - iBecameEmptyAt > KActivityThreshold )
            {
            return EFalse;
            }
        else
            {
            return ETrue;
            }
        }
    }
