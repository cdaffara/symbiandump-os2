/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface used by WLAN LDD for 
*                attaching and extracting interfaces
*                and for getting WLAN PDD capabilities
*
*/

/*
* %version: 5 %
*/

#ifndef WLANPDDIFACE_H
#define WLANPDDIFACE_H

#include <wlanwha.h>
#include <klib.h>

class MWlanOsa;
class MWlanOsaExt;

/**
 *  Interface used by WLAN LDD for attaching and extracting interfaces 
 *  and for getting WLAN PDD capabilities.
 *
 *  @since S60 v5.0
 */
class MWlanPddIface : public DBase
    {

public:

    typedef TUint32 TCapab;

    struct SCapabilities
        {
        static const TCapab KCachedMemory = ( 1 << 0 ); 
        
        /** Capabilities bitmap */
        TCapab iCapabilities;
        /** 
         * Host processor's cache line length.
         * Relevant only when iCapabilities includes KCachedMemory
         */
        TInt iCacheLineLength;
        };

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v5.0
	 */
    virtual ~MWlanPddIface() {};
    
    /**
     * Attach osa and osa extension interface
     *
     * @since S60 v5.0
     * @param aWlanOsa osa interface object
     * @param aWlanOsaExt osa extension interface object
     * @return ETrue for success any other for failure
     */
    virtual TBool Attach( MWlanOsa& aWlanOsa, MWlanOsaExt& aWlanOsaExt ) = 0;

    /**
     * Extract wha interface
     *
     * @since S60 v5.0
     * @return wha interface object
     */
    virtual WHA::Wha& Extract() = 0;
    
    /**
     * Gets lower layer capabilities.
     *
     * Overriding method allocates dynamically an SCapabilities structure
     * providing information about the capabilities of the lower layer.
     * It is the caller's responsibility to deallocate the structure.
     * Note! This method has a default implementation returning just a 
     * NULL pointer in order to maintain backwards sw compatibility with
     * a WLAN PDD which doesn't implement (i.e. override) this method.
     * @since S60 v5.0
     * @param aCapabilities Pointer to capabilities structure
     */
    virtual void GetCapabilities( SCapabilities*& aCapabilities )
        {
        aCapabilities = NULL;
        };
    };


#endif // WLANPDDIFACE_H
