/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWhaToDot11TypeConverter class.
*
*/

/*
* %version: 8 %
*/

#ifndef T_WLANWHATODOT11TYPECONVERTER_H
#define T_WLANWHATODOT11TYPECONVERTER_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

#include "umacoidmsg.h"

class WlanContextImpl;
struct SSupportedRatesIE;
struct SExtendedSupportedRatesIE;

/**
 *  Converts WHA types to dot11 types
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanWhaToDot11TypeConverter 
    {

public:

    /**
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    static void Convert( 
        WHA::TRate aRateMask, 
        SSupportedRatesIE& aRateIe,
        SExtendedSupportedRatesIE& aExtRateIe );

    /**
     * Converts scan parameters to WHA types
     *
     * @since S60 3.2
     * @param aCtxImpl global statemachine context
     * @param aSsid SSID as non-WHA type
     * @param aWhaSsid SSID as WHA type
     * @param aChannels channel information as non-WHA type
     * @param aMinChannelTime min. time to listen beacons/probe responses 
     *                        on a channel
     * @param aMaxChannelTime max. time to listen beacons/probe responses 
     *                        on a channel
     * @param aWhaChannels channel information as WHA type
     * @param aWhaChannelCount number of channels to be scanned
     * @return ETrue when successful
     *         EFalse when memory allocation fails
     */
    static TBool ConvertToWhaTypes( 
        WlanContextImpl& aCtxImpl,
        const TSSID* aSsid,                 
        WHA::SSSID& aWhaSsid,
        const SChannels* aChannels,
        TUint32 aMinChannelTime,            
        TUint32 aMaxChannelTime,        
        WHA::SChannels*& aWhaChannels,
        TUint8& aWhaChannelCount );

private:

    WlanWhaToDot11TypeConverter() {};

    // Prohibit copy constructor.
    WlanWhaToDot11TypeConverter( const WlanWhaToDot11TypeConverter& );
    // Prohibit assigment operator.
    WlanWhaToDot11TypeConverter& operator= 
        ( const WlanWhaToDot11TypeConverter& );

    };

#endif // T_WLANWHATODOT11TYPECONVERTER_H
