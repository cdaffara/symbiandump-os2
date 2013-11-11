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
* Description:  Statemachine for handling received measurement request.
*
*/


#ifndef CORE_OPERATION_HANDLE_MEASUREMENT_REQUEST_H
#define CORE_OPERATION_HANDLE_MEASUREMENT_REQUEST_H

#include "core_operation_base.h"
#include "abs_core_frame_handler.h"
#include "abs_core_event_handler.h"
#include "core_type_list.h"
#include "core_frame_rm_ie.h"
#include "core_frame_rm_ie_beacon_request.h"
#include "core_frame_rm_ie_beacon_report.h"
#include "core_frame_mgmt_ie.h"

// forward declarations
class core_frame_action_rm_c;
class core_frame_dot11_ie_c;
class core_frame_action_c;

NONSHARABLE_CLASS( core_measurement_status_c )
    {
public:
    
    core_measurement_status_c() :
            da_m ( ZERO_MAC_ADDR ),
            sa_m ( ZERO_MAC_ADDR ),
            error_in_request_m ( false_t ),
            refuse_request_m ( false_t ),
            too_short_interval_m ( false_t )
        {}
        
    ~core_measurement_status_c()
        {}
    
public:
    inline const core_mac_address_s& da() const { return da_m; }
    inline const core_mac_address_s& sa() const { return sa_m; }
    inline bool_t error_in_request() const { return error_in_request_m; }
    inline bool_t refuse_request() const { return refuse_request_m; }
    inline bool_t too_short_interval() const { return too_short_interval_m; }
    inline void set_error_in_request( bool_t state ){ error_in_request_m = state; }
    inline void set_refuse_request( bool_t state ){ refuse_request_m = state; }
    inline void set_too_short_interval( bool_t state ){ too_short_interval_m = state; }
    inline void set_sa( core_mac_address_s sa ){ sa_m = sa; }
    inline void set_da( core_mac_address_s da ){ da_m = da; }
    inline core_scan_channels_s get_scan_channels() const { return channels_m; }
    inline void set_scan_channels( core_scan_channels_s channels ){ channels_m = channels; }
    
private:
    core_mac_address_s da_m;
    core_mac_address_s sa_m;
    bool_t error_in_request_m;
    bool_t refuse_request_m;
    bool_t too_short_interval_m;
    core_scan_channels_s channels_m;
    };

/**
 * Statemachine for handling received measurement request.
 *
 * @lib wlmserversrv.lib
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_operation_handle_measurement_request_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c,
    public abs_core_event_handler_c
    {

public:

    /**
     * The possible states of the operation.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_get_next_ie,
        core_state_process_current_ie,
        core_state_make_measurement,
        core_state_create_response,
        core_state_scan_start,
        core_state_create_negative_response,
        core_state_send_response,
        core_state_operation_finished,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_handle_measurement_request_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_frame_action_rm_c* measurement_request );

    virtual ~core_operation_handle_measurement_request_c();
    
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
    
    /**
     * From abs_core_frame_handler_c Called by the core server when a dot11 frame has been received.
     *
     * @since S60 v5.2
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
     * @since S60 v5.2
     * @param indication Adaptation layer event.
     * @return true_t if the indication was handled, false_t if not handled.
     */
    bool_t notify(
        core_am_indication_e indication );
    
    /**
     * This method is called when the operation is cancelled from the outside.
     *
     * @since S60 v5.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );
    
private:
    
    /**
     * This method parses the received measurement request IE and
     * fills in the parameters of the measurement request structure.
     *
     * @since S60 v5.2
     * @param ie Reference to the received measurement IE
     * @param parsed_mask Mask defining what information was parsed from the IE
     */
    void parse( core_frame_dot11_ie_c* ie, u16_t& parsed_mask );
    
    /**
     * This method creates a negative response to received measurement request.
     *
     * @since S60 v5.2
     * @return true_t if response created, false_t otherwise
     */
    bool_t create_negative_response();

    /**
     * This method creates a negative response to the received measurement request
     * refusing to perform the requested measurement or creates an empty response
     * to the received measurement request or creates an incapable response to a
     * measurement request.
     *
     * @since S60 v5.2
     * @param measurement_report_mode Measurement report mode
     * @return true_t if response created, false_t otherwise
     */
    bool_t create_negative_response_based_on_meas_report_mode( u8_t measurement_report_mode );

    /**
     * This method creates a response to received invalid measurement request.
     *
     * @since S60 v5.2
     * @return true_t if response created, false_t otherwise
     */
    bool_t create_response_to_invalid_request();
    
    /**
     * This method validates the received beacon request.
     *
     * @since S60 v5.2
     */
    void validate_measurement_request();
    
    /**
     * This method validates the received beacon request.
     *
     * @since S60 v5.2
     */
    void validate_beacon_request();
    
    /**
     * This method validates the Beacon Reporting Information IE.
     *
     * @since S60 v5.2
     */
    void validate_beacon_reporting_information();
    
    /**
     * This method validates the Beacon Reporting Detail IE.
     *
     * @since S60 v5.2
     */
    void validate_beacon_reporting_detail();
    
    /**
     * This method validates the interval that the measurement
     * requests are coming is not too small.
     *
     * @since S60 v5.2
     * @return true_t if interval long enough, false_t otherwise
     */
    bool_t validate_measurement_interval();
    
    /**
     * Add IE to Measurement Response, creates Measurement Response if
     * it doesn't already exist.
     *
     * @since S60 v5.2
     * @param ie Reference to IE to be added to Measurement Response
     * @return true_t if successful, false_t otherwise
     */
    bool_t add_to_report( const core_frame_rm_ie_c* ie );
    
    /**
     * This method validates the requested scan channels.
     *
     * @since S60 v5.2
     * @return true_t if scan can be done, false_t otherwise
     */
    bool_t validate_scan_channels();
    
    /**
     * Return channel time to use in scan.
     *
     * @since S60 v5.2
     * @return channel times to use in scan
     * @param measurement_mode Request's Measurement Mode
     */
    u16_t get_channel_time( core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_e measurement_mode );
    
    /**
     * Return SSID to use in scan.
     *
     * @since S60 v5.2
     * @return SSID to use in scan
     */
    core_ssid_s get_ssid();
    
    /**
     * Return channels to use in scan.
     *
     * @since S60 v5.2
     * @return Channels to use in scan.
     */
    core_scan_channels_c get_channels();

    /**
     * Check if Beacon/Probe matches to requested data.
     *
     * @since S60 v5.2
     * @return true_t if match, false_t otherwise
     */
    bool_t match_found( core_ap_data_c& ap_data );
    
    /**
     * Create response frame to a measurement request.
     *
     * @since S60 v5.2
     * @return true_t if success, false_t otherwise
     */
    bool_t create_response(
        core_ap_data_c& ap_data );
    
    /**
     * Initialise internal data members for processing next beacon request IE.
     *
     * @since S60 v5.2
     */
    void initialise_beacon_request_data();
    
    /**
     * Get random delay utilising Randomization Interval.
     *
     * @since S60 v5.2
     * @return random delay
     */
    u32_t get_random_delay();
    
private: // data
   
    /** The received Measurement Request frame. */
    core_frame_action_rm_c* measurement_request_m;
    
    /** List of IEs included in the received Measurement Request */
    core_type_list_c<core_frame_dot11_ie_c> ie_list_m;
    
    /** Structure containing status data for the operation logic */
    core_measurement_status_c status_m;

    /** Bitmask about which IEs are successfully parsed. */
    u16_t parsed_mask_m;
    
    /** Reference to IE currently under processing. */
    core_frame_dot11_ie_c* current_ie_m;
    
    /** The Measurement Report frame to be sent. */
    core_frame_action_c* report_m;
    
    /** Parser for Measurement Request IE. */
    core_frame_rm_ie_c* meas_req_parser_m;
    
    /** Parser for Beacon Request IE. */
    core_frame_rm_ie_beacon_request_c* beacon_req_parser_m;
    
    /** Parser for SSID Sub-Element. */
    core_frame_mgmt_ie_ssid_c* ssid_parser_m;
    
    /** Parser for Beacon Reporting Information Sub-Element. */
    core_frame_rm_ie_beacon_request_ie_c* bri_parser_m;
    
    /** Parser for Beacon Reporting Detail Sub-Element. */
    core_frame_rm_ie_beacon_request_detail_ie_c* brd_parser_m;
    
    /** Parser for AP Channel Report Sub-Element. */
    core_frame_mgmt_ie_ap_channel_report_c* ap_ch_rep_parser_m;

    /** Parser for Request IE Sub-Element. */
    core_frame_mgmt_ie_request_ie_c* request_ie_parser_m;
    
    /** Measurement Duration used in the actual measurement. */
    u16_t actual_measurement_duration_m;
    
    };

#endif // CORE_OPERATION_HANDLE_MEASUREMENT_REQUEST_H
