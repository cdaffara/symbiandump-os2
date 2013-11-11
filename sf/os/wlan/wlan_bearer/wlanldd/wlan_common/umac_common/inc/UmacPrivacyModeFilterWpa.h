/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanPrivacyModeFilterWpa class
*
*/

/*
* %version: 8 %
*/

#ifndef WLANPRIVACYMODEFILTERWPA_H
#define WLANPRIVACYMODEFILTERWPA_H

#include "UmacPrivacyModeFilter.h"

/**
* Class Implements privacy mode WPA rx filter
* which is used when WPA is the active privacy mode
*/
class WlanPrivacyModeFilterWpa : public MWlanPrivacyModeFilter
    {
public:

    /**
    * C++ default constructor.
    */
    WlanPrivacyModeFilterWpa() {};

    /** Dtor */
    virtual ~WlanPrivacyModeFilterWpa() {};

    /**
    * Filtering function for Rx-data frames
    * @param aFrameheader Header of the received frame
    * @param aUserDataEnabled is protocol stack side datapath 
    *        enabled or not
    * @param aEthernetType Ethernet Type of the received frame
    * @param aUnicastKeyExists AP <-> client unicast key 
    *        is configured or not
    * @param aAesOrTkipOrWapiEncrypted ETrue if the frame is encrypted with AES,
    *        TKIP or WAPI, EFalse otherwise
    * @return ETrue if frame can be accepted, EFalse otherwise
    */
    virtual TBool operator()( 
        const SDataFrameHeader& aFrameheader, 
        TBool aUserDataEnabled, 
        TUint16 aEthernetType,
        TBool aUnicastKeyExists, 
        TBool aAesOrTkipOrWapiEncrypted ) const;
    
private:

    // Prohibit copy constructor
    WlanPrivacyModeFilterWpa( const WlanPrivacyModeFilterWpa& );
    // Prohibit assigment operator
    WlanPrivacyModeFilterWpa& operator= ( 
        const WlanPrivacyModeFilterWpa& );
    };

#endif  // WLANPRIVACYMODEFILTERWPA_H
