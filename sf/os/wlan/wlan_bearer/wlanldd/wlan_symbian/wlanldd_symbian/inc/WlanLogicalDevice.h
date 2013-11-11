/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the DWlanLogicalDevice class.
*
*/

/*
* %version: 14 %
*/

#ifndef WLANLOGICALDEVICE_H
#define WLANLOGICALDEVICE_H

#include "RWlanLogicalChannel.h"
#include "Umac.h"

#ifdef RD_WLAN_DDK
#include "osa.h"
#endif


class WlanChunk;

/**
 *  Device factory for logical channel
 *  @since S60 v3.1
 */
class DWlanLogicalDevice : public DLogicalDevice
#ifdef RD_WLAN_DDK
                         , public WlanObject
#endif
    {

public:

    // Interface integrity version check class
    class TCaps
        {
        public:
            TVersion iVersion;
        };
    
    DWlanLogicalDevice();

    virtual ~DWlanLogicalDevice();
    
    /**
    * Second stage constructor for derived objects.
    * This must at least set a name for the driver object.
    *
    * @since S60 3.1
    * @return KErrNone or standard error code.
    */
    virtual TInt Install();

    /**
    * Gets the driver's capabilities.
    * This is called in the response to an RDevice::GetCaps() request.
    *
    * @since S60 3.1
    * @param aDes A user-side descriptor into which capabilities information is to be wriiten.
    */
    virtual void GetCaps(TDes8& aDes) const;

    /**
    * Called by the kernel's device driver framework to create a Logical Channel.
    * This is called in the context of the user thread (client) which requested the creation of a Logical Channel
    * (e.g. through a call to RBusLogicalChannel::DoCreate).
    * The thread is in a critical section.
    *
    * @since S60 3.1
    * @param aChannel Set to point to the created Logical Channel
    * @return KErrNone or standard error code.
    */
    virtual TInt Create(DLogicalChannelBase*& aChannel);

    /**
    * Returns ETrue if cached frame transfer memory shall be used and
    * EFalse otherwise
    *
    * @return See above
    */
    TBool UseCachedMemory() const;
    
    /**
    * Sets the type of frame transfer memory (cached / non-cached) that shall
    * be used.
    *
    * @param aValue If ETrue, cached frame transfer memory shall be used
    *               If EFalse, cached frame transfer memory shall not be used
    */
    void UseCachedMemory( TBool aValue );

    /**
    * Sets the number of extra bytes required to align Rx buffer start
    * address, to be returned to WHA layer, to allocation unit boundary
    */
    void SetRxBufAlignmentPadding( TInt aRxBufAlignmentPadding );
    
    /**
    * Returns the number of extra bytes required to align Rx buffer start
    * address, to be returned to WHA layer, to allocation unit boundary
    * @return See above
    */
    TInt RxBufAlignmentPadding() const;    
    
private: // Methods

    // Prohibit copy constructor.
    DWlanLogicalDevice ( const DWlanLogicalDevice & );
    // Prohibit assigment operator.
    DWlanLogicalDevice & operator= ( const DWlanLogicalDevice & );
        
private: // Data

    Umac        iUmac;
    
    /** Our dynamic DFC queue. Own */
    TDynamicDfcQue* iDfcQ;

#ifndef RD_WLAN_DDK
    /**
    * mutex used to protect LDD from simultaneous execution by several 
    * different threads. Own
    */
    DMutex*     iMutex;
#else
    /** 
    * OS abstraction object
    */
    WlanOsa*    iOsa;
#endif    

    /** 
    * ETrue if cached frame transfer memory shall be used,
    * EFalse otherwise 
    */
    TBool iUseCachedMemory;

    /** 
    * shared memory chunk for frame transfer between user and kernel address
    * spaces. Own
    */
    DChunk*     iSharedMemoryChunk;

    /** Rx frame memory pool manager. Own */
    WlanChunk*  iRxFrameMemoryPool;   
    
    /** 
    * number of extra bytes required to align Rx buffer start address
    * to be returned to WHA layer to allocation unit boundary
    */
    TInt iRxBufAlignmentPadding;
   };

#endif // WLANLOGICALDEVICE_H
