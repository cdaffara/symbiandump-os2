/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   inline implementation of Handle
*
*/

/*
* %version: 4 %
*/

#include "algorithm.h"  // for operator !=
#include <wlanosa.h>    // for assert

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline Handle<T>::Handle( T& aElem ) 
    : iData( &aElem ), iRefCount( new TInt( 1 ) )
    {
    TraceDump(INFO_LEVEL, (("[WLAN] Handle ctor +: 0x%08x"), this));
    TraceDump(INFO_LEVEL, (("[WLAN] data: 0x%08x"), iData));
    TraceDump(INFO_LEVEL, (("[WLAN] refcount: 0x%08x"), iRefCount));

    if ( iRefCount )
        {
        // allocation success
        TraceDump(INFO_LEVEL, 
            (("[WLAN] reference count: %d"), *iRefCount));
        Validate();
        }
    else
        {
        // allocation failure
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

        InValidate();
        }

    TraceDump(INFO_LEVEL, (("[WLAN] Handle ctor -: 0x%08x"), this));
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline Handle<T>::~Handle()
    {
    TraceDump(INFO_LEVEL, (("[WLAN] Handle dtor +: 0x%08x"), this));

    Dispose();

    TraceDump(INFO_LEVEL, (("[WLAN] Handle dtor -: 0x%08x"), this));
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline Handle<T>::Handle( const Self& aElem ) 
    : iData( aElem.iData ), iRefCount( aElem.iRefCount )
    {
    TraceDump(INFO_LEVEL, (("[WLAN] Handle copy ctor +: 0x%08x"), this));
    TraceDump(INFO_LEVEL, (("[WLAN] data: 0x%08x"), iData));
    TraceDump(INFO_LEVEL, (("[WLAN] refcount: 0x%08x"), iRefCount));


    MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), 
        __LINE__, (iRefCount != NULL) );

    ++(*iRefCount);
    TraceDump(INFO_LEVEL, (("[WLAN] reference count: %d"), *iRefCount));

    TraceDump(INFO_LEVEL, (("[WLAN] Handle copy ctor -: 0x%08x"), this));
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline Handle<T>& Handle<T>::operator=( const Self& aElem )
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] Handle assignement operator +: 0x%08x"), this));

    if ( *this != aElem )
        {
        Dispose();

        iData = aElem.iData;
        iRefCount = aElem.iRefCount;

        TraceDump(INFO_LEVEL, (("[WLAN] data: 0x%08x"), iData));
        TraceDump(INFO_LEVEL, (("[WLAN] refcount: 0x%08x"), iRefCount));

        MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), 
            __LINE__, (iRefCount != NULL) );

        ++(*iRefCount);
        }
    else
        {
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL, (("[WLAN] reference count: %d"), *iRefCount));
    TraceDump(INFO_LEVEL, 
        (("[WLAN] Handle assignement operator -: 0x%08x"), this));

    return *this;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline T* Handle<T>::operator->()
    {
    return iData;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline T* Handle<T>::Data()
    {
    MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), 
        __LINE__, (iData != NULL) );
    return iData;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline const T* Handle<T>::Data() const
    {
    MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), 
        __LINE__, (iData != NULL) );
    return iData;
    }

// -----------------------------------------------------------------------------
// only upon allocation failure return EFalse
// -----------------------------------------------------------------------------
//
template<class T>
inline TBool Handle<T>::Bind( T& aElem )
    {
    TraceDump(INFO_LEVEL, (("[WLAN] Handle::Bind +: 0x%08x"), this));

    TBool ret( ETrue );

    if ( aElem != *iData )
        {
        if ( --(*iRefCount) == 0 )
            {
            TraceDump(INFO_LEVEL, 
                ("[WLAN] Handle::Bind deallocate referenced data"));
            TraceDump(INFO_LEVEL, 
                (("[WLAN] old data delloac: 0x%08x"), iData));
            TraceDump(INFO_LEVEL, (("[WLAN] refcount: 0x%08x"), iRefCount));

            delete iData;
            ++(*iRefCount); // recycle counter
            }
        else
            {
            // references still exist after this critter is a goner
            // as we are doing a rebind we shall create a new counter
            iRefCount = new TInt( 1 );
            if ( !iRefCount )
                {
                TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
                Trace( ERROR_LEVEL, 
                    reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

                ret = EFalse;
                }
            else
                {
                // allocation success
                // left intentionally empty
                TraceDump(INFO_LEVEL, 
                    (("[WLAN] new refcount: 0x%08x"), iRefCount));
                }
            }

        iData = &aElem;
        TraceDump(INFO_LEVEL, (("[WLAN] new data: 0x%08x"), iData));
        }
    else
        {
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL, (("[WLAN] reference count: %d"), *iRefCount));
    TraceDump(INFO_LEVEL, (("[WLAN] Handle::Bind -: 0x%08x"), this));

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline void Handle<T>::Dispose()
    {
    TraceDump(INFO_LEVEL, (("[WLAN] Handle::Dispose +: 0x%08x"), this));

    if ( IsValid() )
        {
        // we are a valid object
        MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), 
            __LINE__, (iRefCount != NULL) );

        if ( --(*iRefCount) == 0 )
            {
            TraceDump(INFO_LEVEL, 
                ("[WLAN] Handle::Dispose deallocate referenced data"));

            TraceDump(INFO_LEVEL, (("[WLAN] data: 0x%08x"), iData));
            delete iData;
            TraceDump(INFO_LEVEL, (("[WLAN] refcount: 0x%08x"), iRefCount));
            delete iRefCount;
            }
        else
            {
            // references still exist after this critter is a goner
            // left intentionally empty
            TraceDump(INFO_LEVEL, 
                (("[WLAN] reference count: %d"), *iRefCount));
            }
        }
    else
        {
        // we are an invalid object
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL, (("[WLAN] Handle::Dispose -: 0x%08x"), this));
    }
