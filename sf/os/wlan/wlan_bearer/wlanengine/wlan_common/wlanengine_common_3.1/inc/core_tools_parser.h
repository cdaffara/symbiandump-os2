/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simple parser functions for core
*
*/

/*
* %version: 18 %
*/

#ifndef CORE_TOOLS_PARSER_H
#define CORE_TOOLS_PARSER_H

#include "core_types.h"
#include "core_ap_data.h"
#include "core_settings.h"
#include "core_connection_data.h"
#include "abs_core_wpx_adaptation.h"

class core_frame_dot11_c;

/**
 * Class for parsing various 802.11 information elements.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_tools_parser_c )
    {

public:

    /**
     * Check whether the given AP has compatible settings with the connection settings.
     * 
     * @since S60 v3.1
     * @param wpx_adaptation WPX adaptation.
     * @param ap_data AP data to compare.
     * @param iap_data IAP data to compare.
     * @param core_settings Core settings to compare.
     * @param is_cm_active Whether WPA counter measures are active.
     * @return core_connect_ok if AP is compatible, the failure reason otherwise.
     */
    static core_connect_status_e is_ap_compatible_with_iap(
        abs_core_wpx_adaptation_c& wpx_adaptation,
        core_ap_data_c& ap_data,
        core_iap_data_c& iap_data,
        core_settings_c& core_settings,
        bool_t is_cm_active = false_t,
        bool_t ignore_channels = false_t);

    /**
     * Check whether the given AP is suitable for roaming.
     *
     * @since S60 v3.2
     * @param wpx_adaptation WPX adaptation.
     * @param ap_data AP data to compare.
     * @param iap_data IAP data to compare.
     * @param core_settings Core settings to compare.
     * @param connection_data Connection data to compare.
     * @param min_rcpi Minimum RCPI required. RCPI_VALUE_NONE if none.
     * @param min_medium_time Minimum amount free medium time required. MEDIUM_TIME_NOT_DEFINED if none.
     * @param is_cm_active Whether WPA counter measures are active.
     * @param current_bssid BSSID currently connected to. ZERO_MAC_ADDR if none.
     * @return core_connect_ok if AP is compatible, the failure reason otherwise.
     */
    static core_connect_status_e is_ap_suitable(
        abs_core_wpx_adaptation_c& wpx_adaptation,
        core_ap_data_c& ap_data,
        core_iap_data_c& iap_data,
        core_settings_c& core_settings,
        core_connection_data_c& connection_data,
        u8_t min_rcpi,
        const medium_time_s& min_medium_time,
        bool_t is_cm_active,
        const core_mac_address_s& current_bssid );

    /**
     * Get parsed access point information structure.
     *
     * @since S60 v3.2
     * @param iap_data Information about the used IAP.
     * @param ap_data AP data to parse.
     * @return Parsed access point information structure.
     */
    static core_ap_information_s get_ap_info(
        const core_iap_data_c& iap_data,
        core_ap_data_c& ap_data );

    /**
     * Get the traffic stream parameters for the given TID.
     * 
     * @since S60 v3.2
     * @param frame Frame to be parsed.
     * @param tid TID of the traffic stream.
     * @param tspec Parameters are stored here.
     * @return true_t if parameters found, false_t otherwise.
     */
    static bool_t get_wmm_traffic_stream_params(
        core_frame_dot11_c& frame,
        u8_t tid,
        core_traffic_stream_params_s& tspec );

    /**
     * Parse the basic and supported rates from the given information element.
     * @param data Pointer to IE data.
     * @param data_length Length of IE data.
     * @param basic_rates Bitmask of basic rates.
     * @param supported_rates Bitmask of supported rates.
     * @see core_tx_rate_e for rates.
     */
    static void parse_rates(
        const u8_t* data,
        u8_t data_length,
        u32_t& basic_rates,
        u32_t& supported_rates );

private:

    /**
     * Constructor.
     */
    core_tools_parser_c();
    
    };

#endif // core_tools_parser_H
