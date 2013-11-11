/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   WlanDfc implementation
*
*/

/*
* %version: 5 %
*/

#include "osa_includeme.h"

#include <wlandfcclient.h>

#include "osadfc.h"
#include "osa.h"

// mask value to check is dfc cancel event occurred
const TUint KDfcCancelledMask( 1 << 0 );

struct WlanDfcImpl : public DBase
    {

    /**
     * Ctor
     *
     * @since S60 v3.2
     * @param aWlanDfc wlandfc object
     * @param aOsa wlanosa object
     * @param aDfcQueue Pointer to the DFC queue to use
     */
    WlanDfcImpl( WlanDfc& aWlanDfc, WlanOsa& aOsa, TDfcQue* aDfcQueue ); 

    /**
     * Dtor
     *
     * @since S60 v3.2
     */
    virtual  ~WlanDfcImpl();
    
    /**
     * OS dfc callback method
     *
     * @since S60 v3.2
     * @param aPtr callback context
     */
    static void DfcDoToggle( TAny* aPtr );

    /**
     * osa object reference
     */
    WlanOsa&        iOsa;

    /**
     * dfc object
     */
    TDfc            iDfc;    

    /**
     * dfc client callback interface
     * Not Own.
     */
    MWlanDfcClient* iDfcClient;

    /**
     * callback context
     */
    TInt            iCtx;

    /**
     * internal state flags
     */
    TUint           iFlags;

private:

    // Prohibit copy constructor.
    WlanDfcImpl( const WlanDfcImpl& );
    // Prohibit assigment operator.
    WlanDfcImpl& operator= ( const WlanDfcImpl& );        

    };

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDfcImpl::WlanDfcImpl( 
    WlanDfc& aWlanDfc, 
    WlanOsa& aOsa,
    TDfcQue* aDfcQueue ) 
    : iOsa( aOsa ), iDfc( DfcDoToggle, &aWlanDfc, 0 ),
    iDfcClient( NULL ), iCtx( 0 ), iFlags( 0 ) 
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfcImpl ctor: + this 0x%08x"), this));

    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
        aDfcQueue != NULL );

    iDfc.SetDfcQ( aDfcQueue );

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfcImpl ctor: - this 0x%08x"), this));
    };

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDfcImpl::~WlanDfcImpl()
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfcImpl dtor: + this 0x%08x"), this));

    iDfc.Cancel();

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfcImpl dtor: - this 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDfcImpl::DfcDoToggle( TAny* aPtr )
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfcImpl::DfcDoToggle: + addr: 0x%08x"), aPtr));

    WlanDfc* ptr( static_cast<WlanDfc*>(aPtr) );
    ptr->RealDfc();

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfcImpl::DfcDoToggle: - addr: 0x%08x"), aPtr));
    }


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDfc::~WlanDfc()
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc dtor + addr: 0x%08x"), this));

    if ( IsValid() )
        {
        delete iPimpl;
        }
    else
        {
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc dtor - addr: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDfc::WlanDfc( 
    WlanOsa& aOsa,
    void* aDfcQueue ) : iPimpl( NULL )
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc ctor + addr: 0x%08x"), this));

    iPimpl = new WlanDfcImpl( 
        *this, 
        aOsa, 
        reinterpret_cast<TDfcQue*>(aDfcQueue) );

    if ( iPimpl )
        {
        // allocation success -> proceed
        Validate();
        }
    else
        {
        // allocation failed invalidate object
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

        InValidate();
        }

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc ctor - addr: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDfc::RealDfc()
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc::RealDfc: + addr: 0x%08x"), this));

    // acquire system lock
    Pimpl().iOsa.MutexAcquire();

    if ( !(Pimpl().iFlags & KDfcCancelledMask) )
        {
        // DFC has NOT been cancelled
        Pimpl().iDfcClient->OnDfc( Pimpl().iCtx );
        }
    else
        {
        TraceDump(DFC_LEVEL, 
            (("[WLAN] WlanDfc::RealDfc: DFC cancelled -> skip client call")));
        // left intentionally empty
        }

    // release system lock
    Pimpl().iOsa.MutexRelease();

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc::RealDfc: - addr: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// From class MWlanDfc.
// 
// ---------------------------------------------------------------------------
//
void WlanDfc::Enqueue( MWlanDfcClient& aDfcClient, TInt aCtx )
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc::Enqueue: + addr: 0x%08x"), this));

    Pimpl().iFlags &= ~KDfcCancelledMask;
    Pimpl().iDfcClient = &aDfcClient;
    Pimpl().iCtx = aCtx;

    if ( NKern::CurrentContext() == NKern::EInterrupt )
        {
        TraceDump(DFC_LEVEL, 
            ("[WLAN] WlanDfc::Enqueue: in ISR context"));
        Pimpl().iDfc.Add();
        }
    else
        {
        TraceDump(DFC_LEVEL, 
            ("[WLAN] WlanDfc::Enqueue: in other than ISR context"));
        Pimpl().iDfc.Enque();
        }

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc::Enqueue: - addr: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// From class MWlanDfc.
// 
// ---------------------------------------------------------------------------
//
void WlanDfc::Dequeue()
    {
    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc::Dequeue: + addr: 0x%08x"), this));

    Pimpl().iFlags |= KDfcCancelledMask;
    Pimpl().iDfc.Cancel();    

    TraceDump(INFO_LEVEL | DFC_LEVEL, 
        (("[WLAN] WlanDfc::Dequeue: - addr: 0x%08x"), this));
    }



