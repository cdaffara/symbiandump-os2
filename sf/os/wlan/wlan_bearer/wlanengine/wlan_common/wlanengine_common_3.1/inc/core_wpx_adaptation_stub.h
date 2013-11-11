/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub version of WPX adaptation.
*
*/

/*
* %version: 10 %
*/

#ifndef CORE_WPX_ADAPTATION_STUB_H
#define CORE_WPX_ADAPTATION_STUB_H

#include "abs_core_wpx_adaptation.h"

/**
 * This class hides WPX specific functionality. 
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_wpx_adaptation_stub_c ) : public abs_core_wpx_adaptation_c
    {

public:

    /**
     * Constructor.
     */
    core_wpx_adaptation_stub_c();

    /**
     * Destructor.
     */
    virtual ~core_wpx_adaptation_stub_c();

    /**
     * Returns the WPX key management suite from the given OUI.
     *
     * @since S60 v3.1
     * @param oui OUI to check for key management suite.
     * @return core_key_management_none if no supported WPX key management suite found,
     *         otherwise the defined suite
     */
    virtual core_key_management_e wpx_key_management_suite(
        const u8_t* oui );

    /**
     * Convert the given key management suite to WPX specific OUI.
     *
     * @since S60 v3.1
     * @param key_management Key management suite to convert.
     * @param data_length Length of the OUI element.
     * @param data Pointer to the buffer for storing the OUI element.
     * @return code_error_ok on success, an error code otherwise.
     * @note On successful execution data_length parameter is changed
     *       to the size of data stored.     
     */
    virtual core_error_e get_wpx_key_management_oui(
        core_key_management_e key_management,
        u8_t& data_length,
        u8_t* data );

    /**
     * Check whether the given AP has compatible WPX settings with the connection settings.
     *
     * @since S60 v3.1
     * @param reason The reason AP failed the check.
     * @param ap_data AP data to compare.
     * @param iap_data IAP data to compare.
     * @return Whether further checking is allowed.
     */
    virtual bool_t is_ap_wpx_compatible_with_iap(
        core_connect_status_e& reason,
        const core_ap_data_c& ap_data,
        const core_iap_data_c& iap_data );

    /**
     * Process the given WPX frame.
     *
     * @since S60 v3.1
     * @param frame_type Type of the frame.
     * @param frame_length Length of frame data.
     * @param frame_data Pointer to the frame data.
     * @return Whether the frame was handled.
     */
    virtual bool_t handle_wpx_frame(
        core_frame_type_e frame_type,
        u16_t frame_length,
        const u8_t* frame_data );

    /**
     * Handle a successful roam to WPX AP.
     *
     * @since S60 v3.1
     * @param ap_data AP information.
     */
    virtual void handle_wpx_roam_success(
        const core_ap_data_c& ap_data );

    /**
     * Handle a connection stop.
     *
     * @since S60 v3.2
     */
    virtual void handle_wpx_connection_stop();

    /**
     * Handle the starting of WPX fast-roam reassociation.
     *
     * @since S60 v3.1
     * @param ap_data AP information.
     * @param assoc_ie_list IEs added to be added to reassociation request.
     * @return code_error_ok on success, an error code otherwise.     
     */
    virtual core_error_e handle_fast_roam_start_reassociation(
        core_ap_data_c& ap_data,
        core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list );

    /**
     * Process the WPX fast-roam reassociation response frame.
     *
     * @since S60 v3.1
     * @param server Pointer to the core_server instance.
     * @param frame Received reassociation response.
     */
    virtual void handle_fast_roam_reassoc_resp(
        core_frame_assoc_resp_c* frame );

    /**
     * Return the WPX specific IEs for the association request.
     * 
     * @since S60 v3.1
     * @param ap_data AP being connected to.
     * @param assoc_ie_list WPX IEs are added to this list.
     */
    virtual void get_association_request_wpx_ie(
        core_ap_data_c& ap_data,
        core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list );

    /**
     * Return the WPX version number.
     *
     * @since S60 v3.1
     * @param frame Frame to search for WPX version number. 
     * @return The WPX version number. Zero if none.
     */
     virtual u8_t wpx_version(
        core_frame_beacon_c* frame );

    /**
     * Return the maximum tx power level.
     *
     * @since S60 v3.1
     * @param frame Frame to search for maximum tx power level.
     * @return Maximum tx power level, MAX_TX_POWER_LEVEL_NOT_DEFINED if none.
     */
     virtual u8_t max_tx_power_level(
        core_frame_beacon_c* frame );

    /**
     * Handle the WPX-specific features in traffic stream creation.
     *
     * @since S60 v3.2
     * @param ap_data AP being connected to.
     * @param frame Frame containing the TSPEC parameters.
     * @param tid TID of the traffic stream.
     * @param tspec TSPEC parameters of the traffic stream.
     */
     virtual void handle_ts_create_request(
        const core_ap_data_c& ap_data,
        core_frame_dot11_c* frame,
        u8_t tid,
        const core_traffic_stream_params_s& tspec );

    /**
     * Get the WPX-specific traffic stream parameters.
     *
     * @since S60 v3.2
     * @param frame Frame to be parsed.
     * @param tid TID of the traffic stream.
     * @param tspec Parameters are stored here.
     */
     virtual void get_wpx_traffic_stream_params(
        core_frame_dot11_c* frame,
        u8_t tid,
        core_traffic_stream_params_s& tspec );

     /**
      * Get the WPX-specific traffic stream status.
      *
      * @since S60 v3.2
      * @param status Status Code field from the ADDTS Response.
      * @return The WPX-specific traffic stream status.
      */
     virtual core_traffic_stream_status_e get_wpx_traffic_stream_status(
         u8_t status );

     /**
      * Handle the WPX-specific features in association response.
      *
      * @since S60 v3.2
      * @param ap_data AP being connected to.
      * @param frame Received reassociation response.
      */
     virtual void handle_association_response(
        core_ap_data_c& ap_data,
        core_frame_assoc_resp_c* frame );

    /**
     * Handle the WPX-specific operations after successful traffic stream creation.
     *
     * @since S60 v3.2
     * @param frame Frame containing the TSPEC parameters.
     * @param tid TID of the traffic stream.
     * @param user_priority User Priority of the traffic stream.
     */
    virtual void handle_ts_create_success(
        core_frame_dot11_c* frame,
        u8_t tid,
        u8_t user_priority );

    /**
     * Handle the WPX-specific operations after traffic stream deletion.
     *
     * @since S60 v3.2
     * @param tid TID of the traffic stream.
     * @param user_priority User Priority of the traffic stream.
     */
    virtual void handle_ts_delete(
        u8_t tid,
        u8_t user_priority );

    /**
     * Check whether the given management status code is WPX-specific.
     *
     * @since S60 v3.2
     * @param management_status Management status code to check.
     * @return Whether the given status code is WPX-specific.
     */
    virtual bool_t is_wpx_management_status(
        u32_t management_status );

    /**
     * Check whether the given WPX-specific management status code is considered fatal.
     *
     * @since S60 v3.2
     * @param management_status Management status code to check.
     * @return Whether the given status code is considered fatal.
     */
    virtual bool_t is_fatal_wpx_management_status(
        u32_t management_status );

    /**
     * Get the encryption mode used for connection.
     *
     * @since S60 v3.2
     * @param ap_data AP being connected to.
     * @param mode Suggested encryption mode.
     * @return The encryption mode used for connection.
     */ 
    virtual core_encryption_mode_e encryption_mode(
        core_ap_data_c& ap_data,
        core_encryption_mode_e mode );

    /**
     * Get the authentication algorithm number used for connection.
     *
     * @since S60 v3.2
     * @param eapol_auth_type The type of authentication.
     * @param eapol_auth_mode The authentication mode requested by EAPOL.
     * @return The authentication algorithm number used for connection.
     */
    virtual u16_t authentication_algorithm(
        wlan_eapol_if_eapol_key_authentication_type_e eapol_auth_type,
        wlan_eapol_if_eapol_key_authentication_mode_e eapol_auth_mode );

    /**
     * Return a WPX specific operation to be run during loading of drivers.
     *
     * @since S60 v3.2
     * @return Pointer to the operation instance, NULL if none.
     */
    virtual core_operation_base_c* get_wpx_load_drivers_operation();

    };

#endif // CORE_WPX_ADAPTATION_STUB_H
