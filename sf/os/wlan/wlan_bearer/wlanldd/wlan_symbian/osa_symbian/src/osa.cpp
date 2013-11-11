/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   WlanOsa implementation
*
*/

/*
* %version: 9 %
*/

#include "osa_includeme.h"

#include <kern_priv.h>

#include "osa.h"
#include "osadfc.h"
#include "osatimer.h"

#ifdef __WLAN_ENABLE_DMA
    #include "osamemorypool.h"
#else
    // allocator for general purpose memory
    extern void* GpAlloc( TInt aSize, TBool aZeroStamp );
    // free for general purpose memory
    extern void GpFree( void* aPtr );
#endif // __WLAN_ENABLE_DMA

struct WlanOsaImpl : public DBase, public WlanObject
    {

    /**
     * Ctor
     *
     * @since S60 v3.2
     */
    inline WlanOsaImpl();

    /**
     * Dtor
     *
     * @since S60 v3.2
     */
    virtual ~WlanOsaImpl();

    /**
     * Initializes the object instance.
     * Note! Needs to be executed successfully after object instance
     * construction to be able to use the instance.
     *
     * @param aUseCachedMemory ETrue if cached interconnect memory shall be used
     *                         EFalse otherwise
     * @param aAllocationUnit allocation unit size for interconnect memory in
     *        bytes
     * @return ETrue when successful
     *         EFalse otherwise
     */
    TBool Initialize( TBool aUseCachedMemory, TInt aAllocationUnit );

    /**
     * mutex
     * Not Own.
     */
    DMutex*         iMutex;

#ifdef __WLAN_ENABLE_DMA
    /**
     * memorypool
     */
    WlanMemoryPool*  iMemoryPool;
#endif // __WLAN_ENABLE_DMA

private:

    // Prohibit copy constructor.
    WlanOsaImpl( const WlanOsaImpl& );
    // Prohibit assigment operator.
    WlanOsaImpl& operator= ( const WlanOsaImpl& );
    };

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
WlanOsaImpl::WlanOsaImpl(): iMutex( NULL ), iMemoryPool( NULL )
    {
    Validate();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
WlanOsaImpl::~WlanOsaImpl()
    {
#ifdef __WLAN_ENABLE_DMA
    if ( iMemoryPool )
        {
        delete iMemoryPool;
        iMemoryPool = NULL;
        }
#endif // __WLAN_ENABLE_DMA
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool WlanOsaImpl::Initialize( TBool aUseCachedMemory, TInt aAllocationUnit )
    {
    TBool status( ETrue );

#ifdef __WLAN_ENABLE_DMA
    iMemoryPool = new WlanMemoryPool( aUseCachedMemory, aAllocationUnit );
    if ( iMemoryPool )
        {
        if ( !iMemoryPool->IsValid() )
            {
            TraceDump(ERROR_LEVEL,
                ("[WLAN] WlanOsaImpl::Initialize: ERROR: WLAN mem pool invalid"));

            status = EFalse;
            // memory pool will be freed in the destructor
            }
        }
    else
        {
        TraceDump(ERROR_LEVEL,
            ("[WLAN] WlanOsaImpl::Initialize: ERROR: WLAN mem pool alloc failed"));

        status = EFalse;
        }
#endif // __WLAN_ENABLE_DMA

    return status;
    }


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
WlanOsa::~WlanOsa()
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa dtor"));

    iDfcQueue = NULL;
    
    if ( IsValid() )
        {
        // we are valid -> proceed
        // close the mutex
        TraceDump(MUTEX_LEVEL, ("[WLAN] WlanOsa close mutex"));

        if ( Pimpl().iMutex )
            {
            Pimpl().iMutex->Close( NULL );
            }

        if ( iPimpl )
            {
            delete iPimpl;
            iPimpl = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
WlanOsa::WlanOsa() : iPimpl( NULL ), iDfcQueue( NULL )
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa ctor"));

    iPimpl = new WlanOsaImpl();
    if ( iPimpl )
        {
        if ( iPimpl->IsValid() )
            {
            // allocation success & object valid -> proceed

            Validate();

            const TUint mutex_order( KMutexOrdGeneral7 );
            TraceDump(MUTEX_LEVEL,
                (("[WLAN] WlanOsa mutex create: order: %d"), mutex_order));

            const TInt ret( Kern::MutexCreate(
                Pimpl().iMutex, KNullDesC, mutex_order) );

            if ( ret != KErrNone )
                {
                delete iPimpl;
                // and invalidate
                InValidate();
                }
            }
        else
            {
            // osa impl is invalid. Deallocate it
            delete iPimpl;
            // we are invalid, too
            InValidate();
            }
        }
    else
        {
        // allocation failed invalidate object
        InValidate();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool WlanOsa::Initialize( 
    TBool aUseCachedMemory, 
    TInt aAllocationUnit,
    void* aDfcQueue )
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa::Initialize()"));

    TBool status( ETrue );
    
    iDfcQueue = aDfcQueue;

    if ( iPimpl )
        {
        status = Pimpl().Initialize( aUseCachedMemory, aAllocationUnit );
        }
    else
        {
        status = EFalse;
        }

    TraceDump(INFO_LEVEL, (("[WLAN] WlanOsa::Initialize(): status (bool): %d"),
        status));

    return status;
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
//
// ---------------------------------------------------------------------------
//
void* WlanOsa::Alloc( TOsaMemoryType aOsaMemoryType, TInt aSize, TUint aFlags )
    {
#ifdef __WLAN_ENABLE_DMA

    return Pimpl().iMemoryPool->Alloc(
        aOsaMemoryType,
        aSize,
        (aFlags & KAllocZeroStampMask) );

#else

    return GpAlloc( aSize, (aFlags & KAllocZeroStampMask) );

#endif // __WLAN_ENABLE_DMA
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
//
// ---------------------------------------------------------------------------
//
void WlanOsa::Free( void* aPtr )
    {
#ifdef __WLAN_ENABLE_DMA

    Pimpl().iMemoryPool->Free( aPtr );

#else

    GpFree( aPtr );

#endif // __WLAN_ENABLE_DMA
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
//
// ---------------------------------------------------------------------------
//
MWlanDfc* WlanOsa::DfcCreate()
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa::DfcCreate()"));

    WlanDfc* dfc( new WlanDfc( *this, iDfcQueue ) );
    if ( dfc )
        {
        if ( !(dfc->IsValid()) )
            {
            // construct failure -> destroy
            delete dfc;
            dfc = NULL;
            }
        }

    TraceDump(INFO_LEVEL, (("dfc addr: 0x%08x"), dfc));

    return dfc;
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
//
// ---------------------------------------------------------------------------
//
void WlanOsa::DfcDestroy( MWlanDfc* aWlanDfc )
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa::DfcDestroy()"));

    delete aWlanDfc;
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
//
// ---------------------------------------------------------------------------
//
MWlanTimer* WlanOsa::TimerCreate()
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa::TimerCreate()"));

    WlanTimer* timer( new WlanTimer( *this, iDfcQueue ) );
    if ( timer )
        {
        if ( !(timer->IsValid()) )
            {
            // construct failure -> destroy
            delete timer;
            timer = NULL;
            }
        }
    else
        {
        TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa::TimerCreate() no memory"));
        }

    return timer;
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
//
// ---------------------------------------------------------------------------
//
void WlanOsa::TimerDestroy( MWlanTimer* aWlanTimer )
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanOsa::TimerDestroy()"));

    WlanTimer* timer = static_cast< WlanTimer* >( aWlanTimer );

    // dequeue just in case
    timer->Dequeue();

    // remove the final reference to the timer instance - this will
    // cause the object to destroy itself when it is ready to go
    // (the timer DFC been completed.)
    timer->RefDel();

    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
TInt MWlanOsa::MemCmp( const void* aLhs,
                 const void* aRhs,
                 TInt aNumOfBytes )
    {
    return memcompare(
        static_cast<const TUint8*>(aLhs), aNumOfBytes,
        static_cast<const TUint8*>(aRhs), aNumOfBytes );
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
void* MWlanOsa::MemCpy( void* aDest,
                  const void* aSrc,
                  TUint aLengthinBytes )
    {
    return memcpy( aDest, aSrc, aLengthinBytes );
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
void* MWlanOsa::WordCpy( TInt* aDest, const TInt* aSrc, TUint aLengthinBytes )
    {
    return wordmove( aDest, aSrc, aLengthinBytes );
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
void* MWlanOsa::MemClr( void* aDest, TUint aLengthinBytes )
    {
    return memclr( aDest, aLengthinBytes );
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
void* MWlanOsa::MemSet( void* aDest,
                  TInt aValue,
                  TUint aCount )
    {
    return memset( aDest, aValue, aCount );
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
TInt64 MWlanOsa::Time()
    {
    return Kern::SystemTime();
    }

// ---------------------------------------------------------------------------
// From class MWlanOsa.
// ---------------------------------------------------------------------------
//
void MWlanOsa::BusyWait( TUint aWaitTimeInMicroSeconds )
    {
    const TInt KMicroSecsToNanoSecs( 1000 );
    Kern::NanoWait( aWaitTimeInMicroSeconds * KMicroSecsToNanoSecs );
    }

// ---------------------------------------------------------------------------
// From class MWlanOsaExt.
// ---------------------------------------------------------------------------
//
void WlanOsa::MutexAcquire()
    {
    TraceDump(MUTEX_LEVEL, ("[WLAN] WlanOsa::MutexAcquire() +"));
    TraceDump(MUTEX_LEVEL, (("[WLAN] current thread id: %d"),
        (Kern::CurrentThread()).iId));

    // acquire mutex
    Kern::MutexWait( *(Pimpl().iMutex) );

    TraceDump(MUTEX_LEVEL, ("[WLAN] WlanOsa::MutexAcquire() -"));
    TraceDump(MUTEX_LEVEL, (("[WLAN] current thread id: %d"),
        (Kern::CurrentThread()).iId));
    }

// ---------------------------------------------------------------------------
// From class MWlanOsaExt.
// ---------------------------------------------------------------------------
//
void WlanOsa::MutexRelease()
    {
    TraceDump(MUTEX_LEVEL, ("[WLAN] WlanOsa::MutexRelease() +"));
    TraceDump(MUTEX_LEVEL, (("[WLAN] current thread id: %d"),
        (Kern::CurrentThread()).iId));

    // release mutex
    Kern::MutexSignal( *(Pimpl().iMutex) );

    TraceDump(MUTEX_LEVEL, ("[WLAN] WlanOsa::MutexRelease() -"));
    TraceDump(MUTEX_LEVEL, (("[WLAN] current thread id: %d"),
        (Kern::CurrentThread()).iId));
    }

