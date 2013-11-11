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
* Description:   Declaration of the MWlanPrivacyModeFilter class
*
*/

/*
* %version: 8 %
*/

#ifndef MWLANPRIVACYMODEFILTER_H
#define MWLANPRIVACYMODEFILTER_H

struct SDataFrameHeader;

/**
* Class declares an interface for privacy mode rx filters
*/
class MWlanPrivacyModeFilter
    {
public:

    /** Dtor */
    virtual ~MWlanPrivacyModeFilter() {};

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
        TBool aAesOrTkipOrWapiEncrypted ) const = 0;                
    };

#endif  // MWLANPRIVACYMODEFILTER_H
