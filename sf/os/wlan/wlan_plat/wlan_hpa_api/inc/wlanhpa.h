/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  hpa and its client interface declarations
*
*/ 

/*
* %version: 7 %
*/

#ifndef WLANHPA_H
#define WLANHPA_H

#include <wlanosaplatform.h>
#include <wlanosa.h>
/**
 *  hpa client interface declaration
 *
 *
 *  @lib wlanpdd.pdd
 *  @since S60 v3.2
 */
class MWlanHpaCb 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanHpaCb() {};

    /**
     * Called in ISR DFC context to signal that peripheral 
     * device interrupt has occurred. 
     * NOTE: on systems implementing level sensitive interrupt handling
     * host interrupt is disabled prior calling this method
     *
     * @since S60 v3.2
     */    
    virtual void OnInterrupt() = 0;

    };

class MWlanOsaExt;


/**
 *  hpa interface declaration
 *
 *
 *  @lib wlanpdd.pdd
 *  @since S60 v3.2
 */
class WlanHpa 
    {

public:
	 /* interrupt polarity */
	enum TIsrPolarity
    	{
    	/** active high */
    	EIsrPolarityHigh = 0,
    	/** active low */
    	EIsrPolarityLow
    	};

	struct TConfig
    	{
    	/* interrupt polarity */
    	TIsrPolarity iIsrPolarity;
    	};

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~WlanHpa() {};

    /**
     * Create hpa layer object
     *
     * @since S60 v3.2
     * @param aOsaExt osa extension object
     * @return hpa layer object, NULL upon failure
     */
    IMPORT_C static WlanHpa* Create( MWlanOsaExt& aOsaExt, MWlanOsa& aOsa );

    /**
     * Destroy hpa layer object
     *
     * @since S60 v3.2
     * @param aWlanHpa hpa layer object
     */
    IMPORT_C static void Destroy( WlanHpa* aWlanHpa );

    /**
     * Attach hpa layer object client
     *
     * @since S60 v3.2
     * @param aHpaCb hpa layer object client
     */
    inline void Attach( MWlanHpaCb& aHpaCb );
    
    /**
     * Turns WLAN devices power on
     *
     * @since S60 v3.2
     */
    virtual void PowerOnDevice() = 0;

    /**
     * Turns WLAN devices power off
     *
     * @since S60 v3.2
     */
    virtual void PowerOffDevice() = 0;

    /**
     * Called by hpa layer object client when peripheral interrupt
     * informed by OnInterrupt method has been serviced.
     * NOTE: on systems implementing level sensitive interrupt handling
     * host interrupt is enabled at this state
     *
     * @since S60 v3.2
     */
    virtual void InterruptServiced() = 0;
    
    /**
     * Configures the HPA layer
     *
     * NOTE: usage limited to bootup sequence only
     *
     * @since S60 v3.2
     * @param aConfig HPA layer configuration data
     */
    virtual void Configure( const TConfig& aConfig ) = 0;

	/**
	 * Enables Irq
	 *
	 * NOTE: usage limited to bootup sequence only. 
	 * Untill this call the host side interrupts are disabled
	 *
	 * @since S60 v3.2
	 */
	 virtual void EnableIrq() = 0;
	 
	 /**
	 * Toggles debug GPIO
	 * Usage limited to R&D (debug) purpose only
	 *
	 * @since S60 v3.2
	 * @param aHigh state to set (ETrue equals high state otherwise low state)
	 * @return ETrue equals functionality is supported in any other case not supported
	 */
	 virtual TBool DebugGpioToggle( TBool aHigh ) = 0;

protected:

    /**
     * Constructor 
     *
     * @since S60 v3.2
     * @param aOsaExt osa extension object
     */
    explicit WlanHpa( MWlanOsaExt& aOsaExt, MWlanOsa& aOsa ) 
        : iHpaCb( NULL ), iOsaExt( aOsaExt ), iOsa( aOsa ) {};

    /**
     * Extract hpa layer object client
     *
     * @since S60 v3.2
     * @return hpa layer object client
     */
    inline MWlanHpaCb& HpaCb();

    /**
     * Extract osa extension
     *
     * @since S60 v3.2
     * @return osa extension
     */
    inline MWlanOsaExt& OsaExtCb();

    /**
     * Extract osa
     *
     * @since 
     * @return osa
     */
    
    inline MWlanOsa& OsaCb();

private:

    /**
     * hpa layer object client
     * Not own.  
     */
    MWlanHpaCb*     iHpaCb;

    /**
     * osa extension
     */
    MWlanOsaExt&    iOsaExt;
    
     /**
     * osa
     */
    MWlanOsa&    iOsa;
    
    };

#include <wlanhpa.inl>

#endif // WLANHPA_H
