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
* Description:  State machine for IAP availability.
*
*/

/*
* %version: 18 %
*/

#ifndef CORE_OPERATION_GET_AVAILABLE_IAPS_H
#define CORE_OPERATION_GET_AVAILABLE_IAPS_H

#include "genscanlist.h"
#include "core_operation_base.h"
#include "core_type_list.h"
#include "abs_core_frame_handler.h"
#include "core_ap_data.h"
#include "abs_core_event_handler.h"
#include "core_scan_channels.h"

class core_server_c;

/**
 * State machine for IAP availability.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_get_available_iaps_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c,
    public abs_core_event_handler_c
    {

public:

    /**
     * The possible states of the state machine.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_long_broadcast_scan_start,
        core_state_long_broadcast_scan_done,
        core_state_broadcast_scan_start,
        core_state_broadcast_scan_done,
        core_state_broadcast_scan_start_unknown_region,
        core_state_broadcast_scan_complete_unknown_region,
        core_state_store_country_info,
        core_state_broadcast_scan_done_handle_result,
        core_state_direct_scan_start,
        core_state_direct_scan_done,
        core_state_secondary_ssid_check,
        core_state_secondary_ssid_next,
        core_state_secondary_ssid_start,
        core_state_secondary_ssid_done,
        core_state_scanning_done,
        core_state_MAX
        };

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param request_id to corresponding service request
     * @param server core server
     * @param drivers pointer to lower adaptation
     * @param adaptation pointer to upper adaptation
     * @param is_active_scan_allowed specifies whether to use probes or just listen to beacons
     * @param iap_data_list contains list of IAP data structures
     * @param iap_availability_list List of available IAPs.
     * @param iap_ssid_list List of possible secondary SSIDs.
     * @param scan_data scan data of found networks on return
     */
    core_operation_get_available_iaps_c(
        u32_t request_id,
        core_server_c* server,        
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        bool_t is_active_scan_allowed,
        core_type_list_c<core_iap_data_s>& iap_data_list,
        core_type_list_c<core_iap_availability_data_s>& iap_availability_list,
        core_type_list_c<core_ssid_entry_s>* iap_ssid_list,
        ScanList& scan_data );

    /**
     * Destructor.
     */
    virtual ~core_operation_get_available_iaps_c();

protected:

    /**
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();

    /**
     * This method is called when the operation is cancelled from the outside.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

    /**
     * From abs_core_frame_handler_c Called by the core server when a dot11 frame has been received.
     *
     * @since S60 v3.1
     * @param frame Pointer to a dot11 frame parser.
     * @param rcpi RCPI value of the frame.
     * @return true_t if the frame was handled, false_t otherwise.
     */
    bool_t receive_frame(
        const core_frame_dot11_c* frame,
        u8_t rcpi );

    /**
     * From abs_core_event_handler_c Called by the core server when an indication has been received.
     *
     * @since S60 v3.1
     * @param indication Adaptation layer event.
     * @return true_t if the indication was handled, false_t if not handled.
     */
    bool_t notify(
        core_am_indication_e indication );

private: // functions

    /**
     * Process the scan results using the given tag.
     *
     * @param tag Tag to use for iterating through the scan results.
     */
    void process_scan_results(
        u8_t tag );

    /**
     * Remove IAPs that match the given AP.
     */
    void remove_matching_iaps(
        core_ap_data_c& ap_data );

    /**
     * Calculate the amount IAPs marked as hidden from the given IAP list.
     *
     * @param iap_data_list List of IAPs to check
     * @return The amount IAPs marked as hidden.
     */
    u8_t hidden_iap_count(
        core_type_list_c<core_iap_data_s>& iap_data_list );

    /**
     * If region information is not known before the scan, also the channels 12 and 13 can be 
     * scanned in passive mode. After the scan if region is still not known or region
     * limits the allowed channels, this method removes the those APs from scan list that 
     * are not on valid channels.
     */
    void remove_disallowed_aps();

    /**
     * Check whether the given IAP ID has any secondary SSIDs defined.
     *
     * @param id IAP ID to match.
     * @return true_t if secondary SSIDs found, false_t otherwise.
     */
    bool_t is_id_in_secondary_ssid_list(
        u32_t id );

    /**
     * Remove all secondary SSID entries with a matching IAP ID.
     *
     * @param id IAP ID to match.
     */
    void remove_secondary_ssid_entries_by_id(
        u32_t id );

    /**
     * Check whether the given SSID is present in the scan results.
     *
     * @param ssid SSID to match.
     * @param scan_data Scan list to search.
     * @return true_t if matching SSID is found, false_t otherwise.
     */
    bool_t is_ssid_in_scanlist(
        const core_ssid_s& ssid,
        const ScanList& scan_data );

    /**
     * Check whether the given SSID is present in the SSID list.
     *
     * @param ssid SSID to match.
     * @param ssid_list SSID list to search.
     * @return true_t if matching SSID is found, false_t otherwise.
     */
    bool_t is_ssid_in_list(
        const core_ssid_s& ssid,
        core_type_list_c<core_ssid_s>& ssid_list ) const;

    /**
     * Check whether the given IAP is present in the IAP availability list.
     *
     * @param iap_id IAP ID to match.
     * @param iap_list IAP availability list to search.
     * @return true_t if matching IAP is found, false_t otherwise.
     */
    bool_t is_iap_in_availability_list(
        u32_t iap_id,
        core_type_list_c<core_iap_availability_data_s>& iap_list ) const;

    /**
     * Add the given IAP to the IAP availability list. If an entry already exists,
     * only signal strength is updated if needed.
     *
     * @param iap_id IAP ID to add.
     * @param iap_rcpi Signal strength of the IAP.
     */
    void update_iap_availability(
        u32_t iap_id,
        u8_t iap_rcpi );

private: // data

    /**
     * Whether active scanning is allowed.
     */    
    bool_t is_active_scan_allowed_m;

    /**
     * Whether the amount of channels scanned should be limited.
     */    
    bool_t is_limiting_algorithm_used_m;    

    /**
     * Number of IAPs.
     */
    u32_t iap_data_count_m;

    /**
     * List of IAPs.
     */
    core_type_list_c<core_iap_data_s>& iap_data_list_m;

    /**
     * List of available IAPs.
     */
    core_type_list_c<core_iap_availability_data_s>& iap_availability_list_m;

    /**
     * List of possible secondary SSIDs. Not owned by this pointer.
     */
    core_type_list_c<core_ssid_entry_s>* iap_ssid_list_m;

    /**
     * Handle to the ScanList instance.
     */
    ScanList& client_scan_data_m;

    /**
     * List of channels that have WLAN activity.
     */
    core_scan_channels_c active_channels_m;

    /**
     * List of channels to scan in a broadcast scan.
     */
    core_scan_channels_c broadcast_channels_m;

    /**
     * Number of channels to scan with a long broadcast scan
     */
    u8_t long_broadcast_count_m;

    /**
     * List of IAPs that are not available.
     */
    core_type_list_c<core_iap_data_s> non_found_iaps_list_m;

    /**
     * Defines whether a split-scan should be used.
     */   
    bool_t is_split_scan_m;

    /**
     * Defines the number of BSSs found during scanning.
     */
    u8_t bss_count_m;

    /**
     * List of SSIDs to direct scan.
     */
    core_type_list_c<core_ssid_s> direct_scan_list_m;
    
    /**
     * Iterator for direct scan list. 
     */
    core_type_list_iterator_c<core_ssid_s> direct_scan_iter_m;

    /**
     * Region information that is selected based on the APs country information.
     */
    core_wlan_region_e region_from_ap_m;

    };

#endif // CORE_OPERATION_GET_AVAILABLE_IAPS_H
