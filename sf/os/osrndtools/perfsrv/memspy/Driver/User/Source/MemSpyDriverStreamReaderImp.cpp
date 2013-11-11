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

#include "MemSpyDriverStreamReaderImp.h"




CMemSpyMemStreamReader::CMemSpyMemStreamReader( const TDesC8& aData )
    {
    iCurrent = aData.Ptr();
    iMax = iCurrent + aData.Length();
    }


void CMemSpyMemStreamReader::ConstructL()
    {
    }


TInt32 CMemSpyMemStreamReader::ReadInt32L()
    {
    TInt32 ret = 0;
    //
    const TInt size = sizeof( TInt32 );
    IsAvailableL( size );
    Mem::Copy( (TAny*) &ret, iCurrent, size );
    IncrementPos( size );
    //
    return ret;
    }


TUint32 CMemSpyMemStreamReader::ReadUint32L()
    {
    TUint32 ret = 0;
    //
    const TInt size = sizeof( TUint32 );
    IsAvailableL( size );
    Mem::Copy( (TAny*) &ret, iCurrent, size );
    IncrementPos( size );
    //
    return ret;
    }


void CMemSpyMemStreamReader::ReadL( TDes8& aDes )
    {
    const TInt length = ReadInt32L();

    // Need to check the remaining text is actually present...
    IsAvailableL( length );

    // Now read into client descriptor
    aDes.SetLength( length );
    if  ( length > 0 ) 
        {
        TUint8* dest = const_cast< TUint8* >( aDes.Ptr() );
        Mem::Copy( dest, iCurrent, length );
        }
    //
    IncrementPos( length );
    }


void CMemSpyMemStreamReader::ReadL( TDes16& aDes )
    {
    // The kernel driver only ever writes narrow descriptors.
    // However, we can expand them to be UCS2
    const TInt length = ReadInt32L();

    // Need to check the remaining text is actually present...
    IsAvailableL( length );

    // Set final length in descriptor
    aDes.SetLength( length );

    // Read each char
    TUint16* dest = const_cast< TUint16* >( aDes.Ptr() );
    for( TInt i=0; i<length; i++ )
        {
        *dest++ = *iCurrent++;
        }
    }


void CMemSpyMemStreamReader::IsAvailableL( TInt aRequired ) const
    {
    const TUint8* endPos = iCurrent + aRequired;
    if  ( endPos > iMax )
        {
        User::Leave( KErrOverflow );
        }
    }


void CMemSpyMemStreamReader::IncrementPos( TInt aAmount )
    {
    iCurrent += aAmount;
    }




