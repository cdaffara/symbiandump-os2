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
* Description:   WlanOsa declaration
*
*/

/*
* %version: 7 %
*/

#ifndef WLANOSA_H
#define WLANOSA_H

#include <wlanosa.h>
#include <wlanosaext.h>
#include "wlanobject.h"

struct WlanOsaImpl;

/**
 *  concrete osa implementation
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class WlanOsa : 
    public MWlanOsa, 
    public MWlanOsaExt, 
    public WlanObject 
    {
public:
    
    /**
     * dtor
     *
     * @since S60 v3.2
     */
	virtual ~WlanOsa();

    /**
     * ctor
     *
     * @since S60 v3.2
     */
	WlanOsa();
	
    /**
     * Initializes the object instance. 
     * Note! Needs to be executed successfully after object instance 
     * construction to be able to use the instance.
     * 
     * @param aUseCachedMemory ETrue if cached interconnect memory shall be used
     *                         EFalse otherwise
     * @param aAllocationUnit allocation unit size for interconnect memory in
     *        bytes
     * @param aDfcQueue Pointer to the DFC queue to use
     * @return ETrue when successful
     *         EFalse otherwise
     */
    TBool Initialize( 
        TBool aUseCachedMemory, 
        TInt aAllocationUnit,
        void* aDfcQueue );

// from base class MWlanOsa

    /**
     * From MWlanOsa.
     * Memory allocation. Correct alignment guaranteed
     *
     * @since S60 v3.2
     * @param aOsaMemoryType memory type to be allocated
     * @param aSize size of the buffer to be allocated in bytes. Must be 
     *              positive otherwise the allocation fails
     * @param aFlags bitmask to fine-tune behavior of the allocation
     *               bit 0 - set: zero stamp allocated memory
     * @return begin of the allocated memory, NULL upon failure
     */
    virtual void* Alloc( TOsaMemoryType aOsaMemoryType, 
                         TInt aSize, 
                         TUint aFlags );

    /**
     * From MWlanOsa.
     * Releases memory allocated by the Alloc method
     *
     * @since S60 v3.2
     * @param aPtr begin of the buffer to be freed
     */
    virtual void Free( void* aPtr );

    /**
     * From MWlanOsa.
     * Creates DFC context object 
     *
     * @since S60 v3.2
     * @return DFC context object, NULL upon failure
     */
    virtual MWlanDfc* DfcCreate();

    /**
     * From MWlanOsa.
     * Destroys DFC context object 
     *
     * @since S60 v3.2
     * @param aWlanDfc DFC context object to be destroyed
     */
    virtual void DfcDestroy( MWlanDfc* aWlanDfc );

    /**
     * From MWlanOsa.
     * Creates timer context object
     *
     * @since S60 v3.2
     * @return timer context object, NULL upon failure
     */
    virtual MWlanTimer* TimerCreate();

    /**
     * From MWlanOsa.
     * Destroys timer context object
     *
     * @since S60 v3.2
     * @param aWlanTimer timer context object to be destroyed
     */
    virtual void TimerDestroy( MWlanTimer* aWlanTimer );

// from base class MWlanOsaExt

    /**
     * From MWlanOsaExt.
     * Acquires the software system lock
     *
     * @since S60 v3.2
     */
    virtual void MutexAcquire();

    /**
     * From MWlanOsaExt.
     * Releases the software system lock
     *
     * @since S60 v3.2
     */
    virtual void MutexRelease();

protected:

private:

    // Prohibit copy constructor.
    WlanOsa( const WlanOsa& );
    // Prohibit assigment operator.
    WlanOsa& operator= ( const WlanOsa& );

    inline WlanOsaImpl& Pimpl();

private: // data

    /**
     * implementation
     * Own.  
     */
    WlanOsaImpl* iPimpl;

    /** pointer to DFC queue. Not own */
    void* iDfcQueue;
    };

#include "osa.inl"

#endif // WLANOSA_H
