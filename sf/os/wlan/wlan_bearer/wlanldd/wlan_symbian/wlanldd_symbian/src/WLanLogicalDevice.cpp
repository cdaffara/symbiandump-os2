/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the DWlanLogicalDevice class.
*
*/

/*
* %version: 19 %
*/

#include "WlLddWlanLddConfig.h"
#include "WlanLogicalDevice.h"
#include "WlanLogicalChannel.h"
#include "wlanlddcommon.h"
#include "osachunk.h"
#include <kern_priv.h>

const TUint KWlanParseMask = KDeviceAllowUnit | KDeviceAllowPhysicalDevice;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DECLARE_STANDARD_LDD()
    {
    TraceDump(INIT_LEVEL, ("WLANLDD: allocate logical device object"));    

    // alloc the device

#ifndef RD_WLAN_DDK
    return new DWlanLogicalDevice;
    
#else        
    DWlanLogicalDevice* logicalDevice( new DWlanLogicalDevice );

    if ( logicalDevice && !(logicalDevice->IsValid()) )
        {
        // something went wrong
        delete logicalDevice;
        logicalDevice = NULL;
        }

    return logicalDevice;
#endif
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanLogicalDevice::DWlanLogicalDevice()
    :
    iDfcQ( NULL ),
#ifndef RD_WLAN_DDK
    iMutex( NULL ),
#else
    iOsa( NULL ),    
#endif    
    iUseCachedMemory( EFalse ),
	iRxFrameMemoryPool( NULL ),
	iRxBufAlignmentPadding( 0 )
	{
    TraceDump(INIT_LEVEL, (("WLANLDD: DWlanLogicalDevice Ctor: 0x%08x"), this));

	iParseMask = KWlanParseMask;
    iUnitsMask = KWlanUnitsAllowedMask;

    TraceDump(INIT_LEVEL, (("iUnitsMask: 0x%08x"), iUnitsMask));
    TraceDump(INIT_LEVEL, (("iParseMask: 0x%08x"), iParseMask));

	iVersion = TVersion(KWlanDriverMajorVersion,KWlanDriverMinorVersion,KWlanDriverBuildVersion);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
DWlanLogicalDevice::~DWlanLogicalDevice()
	{
    TraceDump(INIT_LEVEL, ("WLANLDD: DWlanLogicalDevice dtor"));

    delete iRxFrameMemoryPool;
    iRxFrameMemoryPool = NULL;

#ifndef RD_WLAN_DDK

    if ( iMutex )
        {
        TraceDump(MUTEX, ("WLANLDD: DWlanLogicalDevice dtor: close the mutex"));
        iMutex->Close( NULL ); 
        iMutex = NULL;       
        }

#else

    if ( iOsa )
        {
        TraceDump(MEMORY, (("WLANLDD: delete WlanOsa: 0x%08x"), 
            reinterpret_cast<TUint32>(iOsa)));
            
        delete iOsa;
        iOsa = NULL;
        }

#endif // RD_WLAN_DDK      
    
    if( iDfcQ )
        {
        TraceDump(MEMORY, (("WLANLDD: delete DFC queue: 0x%08x"), 
            reinterpret_cast<TUint32>(iDfcQ)));
        
        iDfcQ->Destroy();
        }
    
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalDevice::Create( DLogicalChannelBase *&aChannel )
    {
    TraceDump(INIT_LEVEL, ("WLANLDD: DWlanLogicalDevice::Create"));

    TInt ret = KErrNoMemory;

    // UMAC exists we may proceed

    aChannel = 

#ifndef RD_WLAN_DDK
        new DWlanLogicalChannel( 
            *this,
            iUmac,
            *iDfcQ,
            *iMutex, 
            iSharedMemoryChunk, 
            iRxFrameMemoryPool );
#else                
        new DWlanLogicalChannel( 
            *this,
            iUmac,
            *iDfcQ,
            iOsa, 
            iSharedMemoryChunk, 
            iRxFrameMemoryPool );
#endif        

    if ( aChannel )
        {
        ret = KErrNone;
        }
    else
        {
        // allocation failed
        TraceDump(WARNING_LEVEL, 
            ("WLANLDD: DWlanLogicalDevice::Create: logical channel allocation failure"));
        }

    return ret;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalDevice::Install()
	{
    TraceDump(INIT_LEVEL, ("WLANLDD: DWlanLogicalDevice::Install"));

    _LIT(KWlanDdThreadName, "WlanDdThread");
    const TInt KWlanDdThreadPriority = 27;
    TInt ret( 0 );

    // create our own DFC queue (and thread)
    ret = Kern::DynamicDfcQCreate( 
        iDfcQ, 
        KWlanDdThreadPriority, 
        KWlanDdThreadName );

    if ( ret != KErrNone )
        {
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalDevice::Install: DFC queue creation failed -> aborting"));
        return ret;
        }
    else
        {
        TraceDump(MEMORY, (("WLANLDD: new DFC queue: 0x%08x"), 
            reinterpret_cast<TUint32>(iDfcQ)));        
        }

#ifndef RD_WLAN_DDK
    ret = Kern::MutexCreate( iMutex, KNullDesC, KMutexOrdGeneral7);
    TraceDump(MUTEX, 
        (("WLANLDD: DWlanLogicalDevice::Install: mutex create; status: %d"), 
        ret));
        
    if ( ret )
        {
        // mutex creation failed
        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalDevice::Install: mutex creation failed -> aborting"));
        ret = KErrGeneral;                                
        }
        
#else

    // create OSA
   
    iOsa = new WlanOsa();
    
    if ( iOsa )
        {
        TraceDump(MEMORY, (("WLANLDD: new WlanOsa: 0x%08x"), 
            reinterpret_cast<TUint32>(iOsa)));

        if ( !( iOsa->IsValid() ) )
            {
            // OSA is not valid. That's fatal
            // OSA object instance will be deallocated in the destructor

            TraceDump(ERROR_LEVEL, 
                ("WLANLDD: DWlanLogicalDevice::Install: osa not valid, abort"));

            ret = KErrGeneral;
            }                
        }
    else
        {
        // allocation failed

        TraceDump(ERROR_LEVEL, 
            ("WLANLDD: DWlanLogicalDevice::Install: osa alloc failed, abort"));
        ret = KErrNoMemory;
        }

#endif

    if ( !ret )
        {        
        // init the UMac
        if ( iUmac.Init() )
            {
            const TPtrC name= LDD_NAME;
            ret = SetName(&name);
            
            }
        else
            {
            TraceDump(WARNING_LEVEL, 
                ("WLANLDD: DWlanLogicalDevice::Install: UMAC init failed -> aborting "));

            ret = KErrGeneral;
            }
        }

    return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalDevice::GetCaps(TDes8& aDes) const
	{
    TraceDump(INIT_LEVEL, ("WLANLDD: DWlanLogicalDevice::GetCaps"));

    DWlanLogicalDevice::TCaps caps;
	caps.iVersion=iVersion;
    // write back to user mode
    Kern::InfoCopy( aDes, reinterpret_cast<TUint8*>(&caps), sizeof(caps) );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DWlanLogicalDevice::UseCachedMemory() const
    {
    return iUseCachedMemory;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalDevice::UseCachedMemory( TBool aValue )
    {
    iUseCachedMemory = aValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DWlanLogicalDevice::SetRxBufAlignmentPadding( 
    TInt aRxBufAlignmentPadding )
    {
    iRxBufAlignmentPadding = aRxBufAlignmentPadding;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DWlanLogicalDevice::RxBufAlignmentPadding() const
    {
    return iRxBufAlignmentPadding;
    }
