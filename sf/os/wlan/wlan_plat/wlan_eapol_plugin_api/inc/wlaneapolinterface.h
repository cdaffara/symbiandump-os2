/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECom interface definition for WLAN EAPOL interface API.
*
*/

/*
* %version: 3 %
*/

#ifndef WLANEAPOLINTERFACE_H
#define WLANEAPOLINTERFACE_H

// INCLUDES
#include <e32std.h>


/**
 * @brief ECom interface definition for WLAN EAPOL inteface API.
 *
 * This class defines the methods used for configuring plugin and sending data from WLAN Engine to EAPOL.
 *
 * @since S60 v3.2
 */
class MWlanEapolInterface
    {

public:

    /**
     * Configure plugin implementation.
     *
     * @since S60 v3.2
     * @param aHeaderOffset Offset of EAP-header in packet_send.
     * @param aMTU Maximum transfer unit (MTU).
     * @param aTrailerLength Length of trailer needed by lower levels..
     * @return Return value is specified in interface specification.
     */
    virtual TInt Configure(
        const TInt aHeaderOffset,
        const TInt aMTU,
        const TInt aTrailerLength
        ) = 0;

    /**
     * Shutdown plugin implementation.
     *
     * @since S60 v3.2
     * @return Return value is specified in interface specification.
     */        
    virtual TInt Shutdown() = 0;

    /**
     * Send data to EAPOL.
     *
     * @since S60 v3.2
     * @param aData Pointer to the data to be sent.
     * @param aLength Length of the data to be sent.
     * @return Return value is specified in interface specification.
     */
    virtual TInt ProcessData(
        const void * const aData, 
        const TInt aLength ) = 0;

    };



/**
 * @brief ECom interface definition for WLAN EAPOL inteface API.
 *
 * This class defines the method used for sending data from EAPOL to WLAN Engine.
 *
 * @since S60 v3.2
 */
class MWlanEapolCallbackInterface
    {

public:

    /**
     * Callback interface to partner.
     *
     * @since S60 v3.2
     * @param aData Pointer to the data to be sent.
     * @param aLength Length of the data to be sent.
     * @return Return value is specified in interface specification.
     */
    virtual TInt SendData(
        const void * const aData, 
        const TInt aLength ) = 0;

    };


#endif // WLANEAPOLINTERFACE_H
