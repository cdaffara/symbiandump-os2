/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11InfrastructureNormalMode class.
*
*/

/*
* %version: 11 %
*/

#ifndef C_WLANDOT11INFRASTRUCTURENORMALMODE_H
#define C_WLANDOT11INFRASTRUCTURENORMALMODE_H

#include "UmacDot11InfrastructureMode.h"

/**
 *  Normal infrastructure mode operation state
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11InfrastructureNormalMode : public WlanDot11InfrastructureMode
    {
public:  

    /**
    * C++ default constructor.
    */
    WlanDot11InfrastructureNormalMode() {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11InfrastructureNormalMode() {};
    
    /**
    * Changes dot11 power management mode between active and PS mode
    * 
    * @param aCtxImpl statemachine context
    * @return ETrue if state change occurred, EFalse otherwise
    */
    virtual TBool ChangePowerMgmtMode( 
        WlanContextImpl& aCtxImpl );  
        
    /**
    * Indicates that the WLAN device has detected problems in the power
    * save mode operation of the AP
    * 
    * @param aCtxImpl global statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual void DoPsModeErrorIndication( WlanContextImpl& aCtxImpl );
    
    /**
    * Active to Light PS timer timeout function
    *
    * @since S60 v5.1
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnActiveToLightPsTimerTimeout( WlanContextImpl& aCtxImpl );

    /**
    * Light PS to Active timer timeout function
    *
    * @since S60 v5.1
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnLightPsToActiveTimerTimeout( WlanContextImpl& aCtxImpl );

    /**
    * Light PS to Deep PS timer timeout function
    *
    * @since S60 v5.1
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnLightPsToDeepPsTimerTimeout( WlanContextImpl& aCtxImpl );
    
private:

    // from base class WlanDot11Associated
        
    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Exit( WlanContextImpl& aCtxImpl);

    /**
     * From ?base_class1.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    virtual TBool OnDot11PwrMgmtTransitRequired( WlanContextImpl& aCtxImpl );
                
    // Prohibit copy constructor.
    WlanDot11InfrastructureNormalMode( const WlanDot11InfrastructureNormalMode& );
    // Prohibit assigment operator.
    WlanDot11InfrastructureNormalMode& operator= ( const WlanDot11InfrastructureNormalMode& );

private: // data
    
#ifndef NDEBUG 
    /**
     * name of the state
     */
    static const TInt8  iName[];
#endif // !NDEBUG 
    };

#endif      // C_WLANDOT11INFRASTRUCTURENORMALMODE_H
