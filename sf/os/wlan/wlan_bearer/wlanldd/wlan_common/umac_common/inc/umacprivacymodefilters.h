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
* Description:   Declaration of the WlanPrivacyModeFilters class.
*
*/

/*
* %version: 7 %
*/

#ifndef WLANPRIVACYMODEFILTERS_H
#define WLANPRIVACYMODEFILTERS_H

#include "umacoidmsg.h"
#include "UmacPrivacyModeFilter.h"
#include "UmacPrivacyModeFilterNone.h"
#include "UmacPrivacyModeFilterWep.h"
#include "UmacPrivacyModeFilterWpa.h"
#include "UmacPrivacyModeFilter1x.h"
#include "UmacPrivacyModeFilterMixedModeWep.h"
#include "UmacPrivacyModeFilterMixedMode1x.h"
#include "umacprivacymodefilterwapi.h"


struct SDataFrameHeader;

/**
 *  Privacy mode Rx filters context
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanPrivacyModeFilters
    {

public:

    /**
    * C++ default constructor.
    */
    inline WlanPrivacyModeFilters();

    /**
    * Destructor.
    */
    inline virtual ~WlanPrivacyModeFilters();

    /**
    * Sets desired privacy mode
    *
    * @since S60 3.1
    * @param aEncryptionStatus desired privacy mode
    * @return ETrue filter set success any other failure
    */
    virtual void SetActiveFilter( 
        TEncryptionStatus aEncryptionStatus );

    /**
    * Executes current privacy mode filter
    *
    * @since S60 3.1
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
    inline TBool ExecuteFilter( 
        const SDataFrameHeader& aFrameheader, 
        TBool aUserDataEnabled, 
        TUint16 aEthernetType,
        TBool aUnicastKeyExists, 
        TBool aAesOrTkipOrWapiEncrypted ) const;

private:

    // Prohibit copy constructor
    WlanPrivacyModeFilters( const WlanPrivacyModeFilters& );
    // Prohibit assigment operator
    WlanPrivacyModeFilters& operator= 
        ( const WlanPrivacyModeFilters& );

protected: // Data

    /** privacy mode none filter */
    WlanPrivacyModeFilterNone            iNone;
    /** privacy mode 802.11 WEP filter */
    WlanPrivacyModeFilterWep             iWep;
    /** privacy mode WPA filter */
    WlanPrivacyModeFilterWpa             iWpa;
    /** privacy mode 802.1x filter */
    WlanPrivacyModeFilter1x              i1x;
    /** privacy mode mixed cell filter */
    WlanPrivacyModeFilterMixedModeWep    iMixedModeWep;
    /** privacy mode 802.1x mixed cell filter */
    WlanPrivacyModeFilterMixedMode1x     iMixedMode1x;
    /** privacy mode WAPI filter */
    WlanPrivacyModeFilterWapi            iWapi;
    /** currently active privacy filter */
    MWlanPrivacyModeFilter*              iActiveFilter;
    };

#include "umacprivacymodefilters.inl"

#endif // WLANPRIVACYMODEFILTERS_H
