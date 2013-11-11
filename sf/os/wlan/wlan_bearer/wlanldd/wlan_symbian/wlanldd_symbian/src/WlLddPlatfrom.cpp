/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements OS platform specific functions declared in some 
*                librarys
*
*/

/*
* %version: 18 %
*/

#include "WlLddWlanLddConfig.h"
#include "wlanlddcommon.h"

// ============================= LOCAL FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 random()
    {
    return ( static_cast<TUint32>(Kern::SystemTime() / 1000) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* os_alloc( const TUint32 aLengthInBytes )
    {        
    TAny* ptr = new TUint8[aLengthInBytes];
    
    TraceDump(MEMORY, (("WLANLDD: os_alloc: 0x%08x"), 
        reinterpret_cast<TUint32>(ptr)));
    
    return ptr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_free( const TAny* aPtr )
    {
    TraceDump(MEMORY, (("WLANLDD: os_free: 0x%08x"), 
    reinterpret_cast<TUint32>(aPtr)));    

    delete [] aPtr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* os_memset( void* aDest, TInt aValue, TUint aCount )
    {
    return memset( aDest, aValue, aCount );
    }

// ---------------------------------------------------------------------------
// returns aDest
// ---------------------------------------------------------------------------
//
TAny* os_memcpy( TAny* aDest, const TAny* aSrc, TUint32 aLengthinBytes )
    {
    return memcpy( aDest, aSrc, aLengthinBytes );
    }

// ---------------------------------------------------------------------------
// return 0 if equal
// ---------------------------------------------------------------------------
// 
TInt os_memcmp( 
    const TAny* aLhs, 
    const TAny* aRhs, 
    TUint aNumOfBytes )
    {
    return memcompare( 
        static_cast<const TUint8*>(aLhs), aNumOfBytes, 
        static_cast<const TUint8*>(aRhs), aNumOfBytes );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void os_assert( const TUint8* aError, const TUint8* aFile, TUint32 aLine )
    {
    TraceDump( ERROR_LEVEL, (reinterpret_cast<const char*>(aError)) );
    TraceDump( ERROR_LEVEL, (reinterpret_cast<const char*>(aFile)) );
    TraceDump( ERROR_LEVEL, (("Line: %d"), aLine));

    TExitCategory category;

    _LIT( KWlanLdd, "WLANLDD " );
    const TUint KWlanLddLength ( 8 );

    category.Append( KWlanLdd );

    TPtrC8 fileName( aFile );
    const TUint KFileNameSuffixLen = 4; // length of ".cpp" or ".inl"

    const TInt KSpaceLeft = KMaxExitCategoryName - KWlanLddLength;

    const TUint8* KCopyFrom = 
        aFile + 
        fileName.Length() -
        KFileNameSuffixLen -
        KSpaceLeft;
    
    category.Append( KCopyFrom, KSpaceLeft );

    WlanPanic( category, aLine );
    }

// ---------------------------------------------------------------------------
// Returns the current time as the number of microseconds since midnight, 
// January 1st, 0 AD nominal Gregorian
// ---------------------------------------------------------------------------
//
TInt64 os_systemTime()
    {
    return Kern::SystemTime();
    }

// ---------------------------------------------------------------------------
// In our architecture also host byte order is LSB so no byte
// re-ordering is necessary
// ---------------------------------------------------------------------------
//
TUint16 os_Hton( TUint16 aHost )
    {
    return aHost;
    }

// ---------------------------------------------------------------------------
// In our architecture also host byte order is LSB so no byte
// re-ordering is necessary
// ---------------------------------------------------------------------------
//
TUint16 os_Ntoh( TUint16 aNw )
    {
    return aNw;
    }    

// ---------------------------------------------------------------------------
// In our architecture also host byte order is LSB so no byte
// re-ordering is necessary
// ---------------------------------------------------------------------------
//
TUint32 os_H32ton( TUint32 aHost )
    {
    return aHost;
    }

// ---------------------------------------------------------------------------
// In our architecture also host byte order is LSB so no byte
// re-ordering is necessary
// ---------------------------------------------------------------------------
//
TUint32 os_N32toh( TUint32 aNw )
    {
    return aNw;
    }    
