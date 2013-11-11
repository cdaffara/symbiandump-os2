/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for Wi-Fi Protected setup
*
*/


#ifndef CORE_OPERATION_PROTECTED_SETUP_H
#define CORE_OPERATION_PROTECTED_SETUP_H

#include "core_operation_base.h"
#include "genscanlist.h"
#include "core_frame_dot11_ie.h"
#include "core_type_list.h"
#include "abs_core_protected_setup_handler.h"
#include "abs_core_timer.h"

class core_server_c;
class core_ap_data_c;
class abs_core_scan_list_iterator_c;

/**
 * Statemachine for Wi-Fi Protected setup
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_operation_protected_setup_c ) :
    public core_operation_base_c,
    public abs_core_protected_setup_handler_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next, // init
        core_state_prepare_scanning,            // initial actions before scanning
        core_state_start_scanning,              // scanning is started
        core_state_scan,                        // analyse scanlist
        core_state_connect_and_setup,           // make connect and invoke EAPOL to start Protected Setup
        core_state_setup_completed,             // setup is done
        core_state_disconnect,
        core_state_disconnect_on_error,         // this will return correct return value when EAPOL returns failure
        core_state_disconnect_on_cancel,        // this will return correct return value when user cancelled operation
        core_state_disconnect_before_second_round_trip,
        core_state_walktime_expiration,
        core_state_user_cancel,                 // the operation has been cancelled from the outside
        core_state_setup_failed,                // EAPOL has returned a failure code
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_protected_setup_c(
        u32_t request_id,
        core_server_c* server,        
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_iap_data_s& iap_data,
        core_type_list_c<core_iap_data_s>& iap_data_list,
        core_protected_setup_status_e& protected_setup_status );

    /**
     * Destructor.
     */
    virtual ~core_operation_protected_setup_c();

protected:

    /**
     * This method is called when a pending request has been completed
     * and no sub-operations are pending.
     *
     * @since S60 v3.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();
    
    /**
     * This method is called when the operation needs to be canceled.
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel();

    /**
     * This method is called when the operation is cancelled from the outside.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

    /**
     * Called by the timer framework when timer expires.
     */
    static void timer_expired( void* this_ptr );

    /**
     * Called by the core server when a network has been configured via
     * Protected Setup.
     *
     * @since S60 v3.2
     * @param iap_data Results of a successful Protected Setup operation. 
     */
    void handle_protected_setup_network(
        const core_iap_data_s& iap_data );

private:

    /**
     * This method is called to count selected registrars from scanlist
     *
     * @since S60 v3.2
     * @param iter                Iterator used for going through scan list.
     * @param copy_ap_data_always Whether ap_data should be copied even when 
     *                            selected_registrar is not found.
     * @return number of selected registrars
     */
    u32_t check_selected_registrars(
        abs_core_scan_list_iterator_c * iter,
        bool_t copy_ap_data_always );

    /**
     * Determine the status based on the given arguments.
     *
     * @param request_status The request completion status.
     * @param management_status The association response status from the AP.
     * @param protected_setup_status The status will be written here.
     */
    core_error_e protected_setup_status(
        core_error_e request_status,
        core_management_status_e management_status,
        core_protected_setup_status_e& protected_setup_status ) const;

    // Prohibit copy constructor
    core_operation_protected_setup_c(
        const core_operation_protected_setup_c& );
    // Prohibit assigment operator
    core_operation_protected_setup_c& operator=(
        const core_operation_protected_setup_c& );

private: // data

    /** IAP data. */
    core_iap_data_s core_iap_data_m;

    /** Storage for received credentials. */   
    core_type_list_c<core_iap_data_s>& iap_data_list_m;

    /** Protected Setup status that will be returned to adaptation */
    core_protected_setup_status_e& protected_setup_status_m;

    /** ScanList instance. */
    ScanList* scan_data_m;

    /** Walk time timer. */
    abs_core_timer_c* walktime_timer_m;

    /** AP that will be used Protected Setup. Not owned by this pointer. */
    core_ap_data_c* selected_ap_data_m;

    /** Tag used for scan results. */
    u8_t tag_m;

    /** Channels that will be used for scanning. */
	core_scan_channels_s all_valid_scan_channels_m;

    /** The list of IEs that will be added to the association/resassociation request */
    core_type_list_c<core_frame_dot11_ie_c> assoc_ie_list_m;

    /** Whether the connection attempt is a reassociation. */
    bool_t is_reassociation_m;

    /** Status of connection attempt. */
    core_management_status_e management_status_m;

    /** Whether we are still connected to an AP. */
    bool_t is_connected_m;
    
    /** Count how many times protected setup AP's are scanned. */
    u8_t wps_scan_count_m;
    
    /** Mark whether selected registrar flag was on.
     * This will prevent two round-trips when flag is set already in first round-trip.
     */
    bool_t is_selected_registrar_found_m;

    /** Mark whether pushbutton method is used.
     * This information is used for selected_registrar counting.
     */
    bool_t is_pushbutton_method_used_m;
    
};

#endif // CORE_OPERATION_PROTECTED_SETUP_H
