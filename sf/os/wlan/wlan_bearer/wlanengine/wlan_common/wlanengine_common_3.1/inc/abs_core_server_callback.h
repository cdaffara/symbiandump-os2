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
* Description:  Callback interface from core server
*
*/


#ifndef ABS_CORE_SERVER_CALLBACK_H
#define ABS_CORE_SERVER_CALLBACK_H

#include "am_platform_libraries.h"
#include "core_types.h"

class abs_wlan_eapol_callback_c;

/**
 * Abstract adaptation layer interface for core server callbacks.
 *
 * This interface is implemented by the adaptation layer and a reference
 * to the class implementing it is passed as a parameter to the core server
 * constructor.
 *
 * @since S60 v3.1
 * @see abs_core_server_c
 */
class abs_core_server_callback_c
    {
public:

    /**
     * Destructor.
     */
    virtual ~abs_core_server_callback_c() {};

    /**
     * Notify about country information received from APs.
     * @param request_id An unique identification for the request.
     * @param found_region WLAN region information obtained from the AP's country information.
     * @param inconsistent_info Information whether different APs gave inconsistent country information.
     */
    virtual void store_ap_country_info(
    		u32_t request_id,
    		const core_wlan_region_e& found_region,
    	    bool_t inconsistent_info ) = 0;        

    /**
     * Get current regulatory domain.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param region Current regulatory domain.
     * @param mcc_known Information whether the WLAN region and MCC is currently known.
     */
    virtual void get_regulatory_domain(
        u32_t request_id,
        core_wlan_region_e& region,
        bool_t& mcc_known ) = 0;

    /**
     * Send a notification to the adaptation layer.
     *
     * @since S60 v3.1
     * @param notification The notification to be sent.
     * @param data_length Length of optional data.
     * @param data Pointer to optional data.
     */
    virtual void notify(
        core_notification_e notification,
        int_t data_length,
        const u8_t* data ) = 0;

    /**
     * An asynchronous request from the adaptation layer has been completed.
     *
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param status Completion status of the request.
     */
    virtual void request_complete(
        u32_t request_id,
        core_error_e status ) = 0;

    /**
     * Cancel a pending asynchronous request.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     */
    virtual void cancel_request(
        u32_t request_id ) = 0;

    /**
     * Initialize interface to EAPOL.
     *
     * @since S60 v3.2
     * @param mode Operating mode to use.
     * @param partner Pointer to partner instance.
     * @return Status value of operation.
     */
    virtual u32_t load_eapol(
        core_eapol_operating_mode_e mode,
        abs_wlan_eapol_callback_c* const partner ) = 0;

    /**
     * Shutdown EAPOL.
     *
     * @since S60 v3.2
     * @return Status value of operation.
     */
    virtual u32_t shutdown() = 0;
    
    /**
     * Configure EAPOL.
     *
     * @since S60 v3.2
     * @param header_offset Offset of EAP-header in packet_send.
     * @param MTU Maximum transfer unit (MTU).
     * @param trailer_length Length of trailer needed by lower levels..
     * @return Status value of operation.
     */
    virtual u32_t configure(
        const u32_t header_offset,
        const u32_t MTU,
        const u32_t trailer_length ) = 0;

    /**
     * Send data to EAPOL.
     *
     * @since S60 v3.2
     * @param data Pointer to data to be send.
     * @param length Length of data to be send.
     * @return Status value of operation.
     */
    virtual u32_t process_data(
        const void* const data,
        const u32_t length ) = 0;

    };

#endif // ABS_CORE_SERVER_CALLBACK_H
