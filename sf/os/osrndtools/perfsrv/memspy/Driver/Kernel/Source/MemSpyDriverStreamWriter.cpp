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

#include "MemSpyDriverStreamWriter.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverXferBuffer.h"

#if defined(__GCC32__)
#define __RETURN_ADDRESS() __builtin_return_address(0)
#elif defined (__ARMCC__)
#define __RETURN_ADDRESS() ((TAny*)__return_address())
#else
#define __RETURN_ADDRESS() 0 // not supported
#endif 


RMemSpyMemStreamWriter::RMemSpyMemStreamWriter()
:   iBuffer( NULL ), iCurrent( NULL ), iMax( 0 ), iMark( NULL ), iCommitted( 0 )
    {
    }


RMemSpyMemStreamWriter::RMemSpyMemStreamWriter( DMemSpyDriverXferBuffer& aBuffer )
:   iBuffer( &aBuffer ), iMark( NULL ), iCommitted( 0 )
    {
    iCurrent = iBuffer->Ptr();
    iMax = iCurrent + iBuffer->Size();
    TRACE( Kern::Printf("RMemSpyMemStreamWriter::RMemSpyMemStreamWriter() - current: 0x%08x, max: 0x%08x, size: %d", iCurrent, iMax, iBuffer->Size() ) );
    }


void RMemSpyMemStreamWriter::Close()
    {
    iBuffer = NULL;
    iMark = NULL;
    }


TInt RMemSpyMemStreamWriter::WriteAndClose( TDes8* aDestinationPointer )
    {
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    //
    const TInt ret = iBuffer->WriteToClient( aDestinationPointer, iCommitted );
    Close();
    //
    return ret;
    }


TInt32* RMemSpyMemStreamWriter::WriteInt32( TInt32 aValue )
    {
    TInt32* ret = NULL;
    //
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    if  ( !IsFull() )
        {
        ret = (TInt32*) iCurrent;
        *ret = aValue;
        IncrementPos( sizeof( TInt32 ) );
        }
    else
        {
        TRACE( Kern::Printf( "RMemSpyMemStreamWriter::WriteInt32() - asked to write: 0x%08x from fn: 0x%08x BUT AM FULL", aValue, __RETURN_ADDRESS() ) );
        }
    //
    return ret;
    }


TUint32* RMemSpyMemStreamWriter::WriteUint32( TUint32 aValue )
    {
    TUint32* ret = NULL;
    //
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    if  ( !IsFull() )
        {
        ret = (TUint32*) iCurrent;
        *ret = aValue;
        IncrementPos( sizeof( TUint32 ) );
        }
    else
        {
        TRACE( Kern::Printf( "RMemSpyMemStreamWriter::WriteUint32() - asked to write: 0x%08x from fn: 0x%08x BUT AM FULL", aValue, __RETURN_ADDRESS() ) );
        }
    //
    return ret;
    }


TUint8* RMemSpyMemStreamWriter::Current() const
    {
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    return iCurrent;
    }


void RMemSpyMemStreamWriter::Seek( TInt aPosition )
    {
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    TUint8* pos = iBuffer->Ptr() + aPosition;
    __ASSERT_ALWAYS( pos < iMax, MemSpyDriverUtils::Fault( __LINE__ ) );
    iCurrent = pos;
    }


TUint32 RMemSpyMemStreamWriter::Remaining() const
    {
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    const TUint32 max = reinterpret_cast< TUint32 >( iMax );
    const TUint32 cur = reinterpret_cast< TUint32 >( iCurrent );
    TRACE( Kern::Printf("RMemSpyMemStreamWriter::Remaining() - current: 0x%08x, max: 0x%08x, ret: %d", cur, max, ( max - cur ) ) );
    return ( max - cur );
    }


void RMemSpyMemStreamWriter::MarkSet()
    {
    __ASSERT_ALWAYS( iMark == NULL, MemSpyDriverUtils::Fault( __LINE__ ) );
    iMark = iCurrent;
    }


void RMemSpyMemStreamWriter::MarkResume()
    {
    __ASSERT_ALWAYS( iMark != NULL, MemSpyDriverUtils::Fault( __LINE__ ) );
    iCurrent = iMark;
    iMark = NULL;
    }


TBool RMemSpyMemStreamWriter::IsFull() const
    {
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    const TBool ret = ( iCurrent >= iMax );
    return ret;
    }


TBool RMemSpyMemStreamWriter::IsOpen() const
    {
    const TBool ret = ( iBuffer && iCurrent && iMax );
    return ret;
    }


RMemSpyMemStreamWriter& RMemSpyMemStreamWriter::operator=( const RMemSpyMemStreamWriter& aCopy )
    {
    iBuffer = aCopy.iBuffer;
    iMark = aCopy.iMark;
    iCommitted = aCopy.iCommitted;
    iCurrent = aCopy.iCurrent;
    iMax = aCopy.iMax;
    //
    return *this;
    }


void RMemSpyMemStreamWriter::IncrementPos( TInt aAmount )
    {
    __ASSERT_ALWAYS( IsOpen(), MemSpyDriverUtils::Fault( __LINE__ ) );
    if  ( iCurrent + aAmount > iMax )
        {
        Kern::Printf("RMemSpyMemStreamWriter::IncrementPos() - OVERFLOW - current: 0x%08x, max: 0x%08x, aAmount: %d", iCurrent, iMax, aAmount );
        }

    iCurrent += aAmount;
    iCommitted += aAmount;
    }



