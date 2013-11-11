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
* Description:   Declaration of the DWlanPowerHandler class.
*
*/

/*
* %version: 6 %
*/

#ifndef D_WLANPOWERHANDLER_H
#define D_WLANPOWERHANDLER_H

#include <kpower.h> 

class MWlanPowerIndicator;

/**
*  Power handler which listens to Power Manager instructions
*
*  @since S60 v3.1
*/
class DWlanPowerHandler : public DPowerHandler
    {
    
public:

    /**
    * Constructor.
    *
    * @since S60 3.1
    * @param aWlanLdd The indicator interface class
    */
    explicit DWlanPowerHandler( 
        MWlanPowerIndicator& aWlanLdd,
        TDynamicDfcQue& aDfcQ );

    /**
    * Destructor.
    *
    * @since S60 3.1
    */
    virtual ~DWlanPowerHandler();

    /**
    * Power Down DFC function
    */
    static void PowerDownDfcFn( TAny *aPtr );        

    /**
    * Power Up DFC function
    */
    static void PowerUpDfcFn( TAny *aPtr );

    /**
    * Called by power manager. Requests peripheral power down.
    *
    * @since S60 3.1
    */
    virtual void PowerDown( TPowerState );

    /**
    * Called by power manager. Notifies of system power up.
    *
    * @since S60 3.1
    */
    virtual void PowerUp();

    /**
    * Called when the device has been powered down. Informs the power
    * manager about that.
    *
    * @since S60 3.1
    */
    virtual void OnPowerDownDone();

    /**
    * Called when power up handling for the device has been done.
    * Informs the power manager about that.
    *
    * @since S60 3.1
    */
    virtual void OnPowerUpDone();
    
private:

    /** Default C++ Ctor */
    DWlanPowerHandler();

    /**
    * Called by the Power Down DFC function. Triggers the actual
    * power down handling
    *
    * @since S60 3.1
    */
    void PowerDownOperation();

    /**
    * Called by the Power Up DFC function. Triggers the actual
    * power down handling
    *
    * @since S60 3.1
    */
    void PowerUpOperation();

    // Prohibit copy constructor
    DWlanPowerHandler( const DWlanPowerHandler& );
    // Prohibit assigment operator
    DWlanPowerHandler& operator= ( const DWlanPowerHandler& );

private:    // Data

    /** Pointer to the indicator interface class. Not own */
    MWlanPowerIndicator* iWlanLdd;
    /** Power Up DFC object */
    TDfc iPowerUpDfc;
    /** Power Down DFC object */
    TDfc iPowerDownDfc;
    };

#endif // D_WLANPOWERHANDLER_H
