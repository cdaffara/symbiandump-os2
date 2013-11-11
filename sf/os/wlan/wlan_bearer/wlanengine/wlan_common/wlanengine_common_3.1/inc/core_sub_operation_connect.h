/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for connecting to a network
*
*/

/*
* %version: 18 %
*/

#ifndef CORE_SUB_OPERATION_CONNECT_H
#define CORE_SUB_OPERATION_CONNECT_H

#include "core_operation_base.h"
#include "core_ap_data.h"
#include "core_type_list.h"
#include "abs_core_frame_handler.h"

class core_server_c;
class abs_core_driverif_c;
class core_frame_dot11_ie_c;
class core_frame_assoc_resp_c;

const u64_t RRM_CAPABILITY_BIT_MASK = 0xfc0070;

/**
 * Statemachine for connecting to a network
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_connect_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_set_tx_level,
        core_state_req_set_tx_rate_policies,
        core_state_connect,
        core_state_connect_frame,
        core_state_connect_complete,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_sub_operation_connect_c(
        u32_t request_id,        
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        bool_t& is_connected,
        core_management_status_e& connect_status,
        const core_ssid_s& ssid,
        core_ap_data_c& ap_data,
        u16_t auth_algorithm,
        core_encryption_mode_e encryption_level,
        core_cipher_key_type_e pairwise_key_type,
        core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list,
        core_frame_assoc_resp_c** assoc_resp,
        bool_t is_pairwise_key_invalidated,
        bool_t is_group_key_invalidated,
        const core_cipher_key_s* pairwise_key );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_connect_c();

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
     * This method is called when the operation needs to be canceled.     
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel();

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

private: // data

    /**
     * Whether we are still connected to an AP.
     */
    bool_t& is_connected_m;
   
    /** Status of connection attempt. */
    core_management_status_e& connect_status_m;

    /**
     * SSID to connect to.
     */
    const core_ssid_s& ssid_m;
    
    /**
     * AP to connect to.
     */
    core_ap_data_c& ap_data_m;
    
    /**
     * Authentication algorithm number to use.
     */
    u16_t auth_algorithm_m;
    
    /**
     * Encryption to use.
     */
    core_encryption_mode_e encryption_m;

    /**
     * Pairwise cipher key type to be used.
     */
    core_cipher_key_type_e pairwise_key_type_m;

    /**
     * The maximum allowed TX level.
     */
    u32_t tx_level_m;

    /** The list of IEs that will be added to the association/resassociation request */
    core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list_m;

    /** Buffer for storing all the IEs added to the association/reassociation request. */    
    u8_t* assoc_ie_data_m;

    /**
     * The (re-)association response frame is stored here if received.
     * Not owned by this pointer.
     */
    core_frame_assoc_resp_c** assoc_resp_m;

    /**
     * Whether the pairwise key should be invalidated.
     */
    bool_t is_pairwise_key_invalidated_m;
    
    /**
     * Whether the group key should be invalidated.
     */
    bool_t is_group_key_invalidated_m;

    /**
     * Pairwise key to be set before association. Not owned by this pointer.
     */
    const core_cipher_key_s* pairwise_key_m;

    };

#endif // CORE_SUB_OPERATION_CONNECT_H
