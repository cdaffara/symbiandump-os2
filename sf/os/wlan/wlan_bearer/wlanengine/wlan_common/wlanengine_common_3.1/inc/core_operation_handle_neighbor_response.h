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
* Description:  Statemachine for handling received neighbor response.
*
*/


#ifndef CORE_OPERATION_HANDLE_NEIGHBOR_RESPONSE_H
#define CORE_OPERATION_HANDLE_NEIGHBOR_RESPONSE_H

#include "core_operation_base.h"
#include "core_type_list.h"
#include "core_frame_nr_ie.h"
#include "core_frame_mgmt_ie.h"

// forward declarations
class core_frame_action_nr_c;
class core_frame_dot11_ie_c;
class core_frame_action_c;

/**
 * Statemachine for handling received neighbor response.
 *
 * @lib wlmserversrv.lib
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_operation_handle_neighbor_response_c ) : public core_operation_base_c
    {

public:

    /**
     * The possible states of the operation.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_ies_stored,
        core_state_return_erroneous_frame,
        core_state_get_next_ie,
        core_state_process_current_ie,
        core_state_operation_finished,
        core_base_state_parameters_set,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_handle_neighbor_response_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_frame_action_nr_c* neighbor_response );

    /**
     * Destructor.
     */
    virtual ~core_operation_handle_neighbor_response_c();
    
protected:

    /**
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @since S60 v5.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();
    
private:
    
    /**
     * This method parses the received neighbor response IE.
     *
     * @since S60 v5.2
     * @param ie Reference to the received neighbor response IE
     */
    void parse( core_frame_dot11_ie_c* ie );
    
private: // data
   
    /**
     * The received Neighbor Response frame.
     */
    core_frame_action_nr_c* neighbor_response_m;
    
    /**
     * List of IEs included in the received Neighbor Response
     */
    core_type_list_c<core_frame_dot11_ie_c> ie_list_m;
    
    /**
     * Reference to IE currently under processing.
     */
    core_frame_dot11_ie_c* current_ie_m;
    
    /** Parser for Neighbor Response IE. */
    core_frame_nr_ie_c* neighbor_resp_parser_m;
    
    };

#endif // CORE_OPERATION_HANDLE_NEIGHBOR_RESPONSE_H
