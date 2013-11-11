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
* Description:   Declaration of the WlanPrivacyModeFilterWep class
*
*/

/*
* %version: 8 %
*/

#ifndef UMACPRIVACYMODEFILTERWEP_H
#define UMACPRIVACYMODEFILTERWEP_H

#include "UmacPrivacyModeFilter.h"

/**
* Class Implements privacy mode WEP rx filter
* which is used when 802.11 WEP is the active privacy mode
*/
class WlanPrivacyModeFilterWep : public MWlanPrivacyModeFilter
    {
public:

    /**
    * C++ default constructor.
    */
    WlanPrivacyModeFilterWep() {};

    /** Dtor */
    virtual ~WlanPrivacyModeFilterWep() {};

    /**
    * Filtering function for Rx-data frames.
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
    WlanPrivacyModeFilterWep( const WlanPrivacyModeFilterWep& );
    // Prohibit assigment operator
    WlanPrivacyModeFilterWep& operator= ( 
        const WlanPrivacyModeFilterWep& );
    };

#endif  // UMACPRIVACYMODEFILTERWEP_H
