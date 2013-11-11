/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the interface that powersave plugins have to implement
*
*/

/*
* %version: 5 %
*/

#ifndef WLANPOWERSAVEINTERFACE_H
#define WLANPOWERSAVEINTERFACE_H

/** The possible WLAN power save modes. */
enum TWlanPowerSave
    {
    /** Allow the WLAN engine to decide the mode. */
    EWlanPowerSaveAutomatic, 
    /** Use a light power save mode. */
    EWlanPowerSaveLight,
    /** Use a deeper power save mode. */
    EWlanPowerSaveDeep,
    /** Disable power save completely. */
    EWlanPowerSaveNone
    };

/**
* Callback interface for adjusting the WLAN power save mode.
*
* This interface allows an ECom plugin implementing MWlanPowerSaveInterface
* to adjust the WLAN power save mode. These callbacks are implemented by the
* WLAN engine.
*
* @note The mode will stay until it is changed or the device is booted.
* @note If power save has been disabled from the WLAN settings, this
* setting has no effect.
*
* @lib wlmserversrv.dll
* @since Series 60 3.0
*/
class MWlanPowerSaveCallback
    {
    public: // New functions

        /**
        * Set the WLAN power save mode.
        * @since Series 60 3.0
        * @param aMode The power save mode to set.
        * @return A Symbian error code.
        */
        virtual TInt SetPowerSaveMode(
            TWlanPowerSave aMode ) = 0;
    };

#endif // WLANPOWERSAVEINTERFACE_H
