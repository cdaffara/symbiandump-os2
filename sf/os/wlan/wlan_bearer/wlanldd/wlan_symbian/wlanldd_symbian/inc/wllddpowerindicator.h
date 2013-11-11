/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the MWlanPowerIndicator class.
*
*/

/*
* %version: 4 %
*/

#ifndef M_WLANPOWERINDICATOR_H
#define M_WLANPOWERINDICATOR_H

/**
*  Interface class for DWlanPowerHandler for indicating power events to wlanldd.
*
*  @since S60 v3.1
*/
class MWlanPowerIndicator
    {

public: 
    
    /**
    * Indicates that power up notification has been received from kernel 
    * side Power Manager.
    *
    * @since S60 3.1
    */
    virtual void OnPowerUp() = 0;

    /**
    * Indicates that power down request has been received from kernel 
    * side Power Manager.
    *
    * @since S60 3.1
    */
    virtual void OnPowerDown() = 0;
    };

#endif  // M_WLANPOWERINDICATOR_H   
