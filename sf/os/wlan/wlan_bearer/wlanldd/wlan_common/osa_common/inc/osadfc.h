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
* Description:   WlanDfc declaration
*
*/

/*
* %version: 4 %
*/

#ifndef WLANDFC_H
#define WLANDFC_H

#include <wlandfc.h>
#include "wlanobject.h"

class WlanOsa;
struct WlanDfcImpl;

/**
 *  concrete DFC context object
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class WlanDfc : public MWlanDfc, public WlanObject
    {

    friend class WlanDfcImpl;

public:

    /**
     * Destructor.
     *
     * @since S60 v3.2
     */
    virtual ~WlanDfc();

	/**
	 * Constructor.
	 *
	 * @since S60 v3.2
     * @param aOsa wlanosa object
     * @param aDfcQueue Pointer to the DFC queue to use
	 */
    explicit WlanDfc( 
        WlanOsa& aOsa, 
        void* aDfcQueue );

protected:

// from base class MWlanDfc

    /**
     * From MWlanDfc.
     * Enqueue a dfc for execution. 
     * Either succeeds or has no effect.
     * Calling this method for allready queued dfc has no effect
     *
     * @since S60 v3.2
     * @param aDfcClient client interface
     * @param aCtx context passed back in client callback
     */
     virtual void Enqueue( MWlanDfcClient& aDfcClient, TInt aCtx );

    /**
     * From MWlanDfc.
     * Dequeue a queued dfc. 
     * Either succeeds or has no effect.
     * Calling this method for non queued dfc has no effect 
     *
     * @since S60 v3.2
     */
     virtual void Dequeue();

private:

    // Prohibit copy constructor.
    WlanDfc( const WlanDfc& );
    // Prohibit assigment operator.
    WlanDfc& operator= ( const WlanDfc& );

    /**
     * DFC execution entry
     *
     * @since S60 v3.2
     */
    void RealDfc();

    /**
     * Extracts implementation details
     *
     * @since S60 v3.2
     * @return implementation details
     */
    inline WlanDfcImpl& Pimpl();

private: // data

    /**
     * implementation
     * Own.  
     */
    WlanDfcImpl* iPimpl;
    };

#include "osadfc.inl"

#endif // WLANDFC_H
