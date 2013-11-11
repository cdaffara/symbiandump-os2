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
* Description:  Class for storing traffic stream parameters.
*
*/

/*
* %version: 6 %
*/

#ifndef CORE_TRAFFIC_STREAM_H
#define CORE_TRAFFIC_STREAM_H

#include "core_types.h"

/**
 * Class for storing traffic stream parameters.
 *
 * @lib wlmserversrv.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_traffic_stream_c )
    {

public:

    /**
     * Constructor.
     * @param tid TID of the traffic stream.
     * @param user_priority User Priority of the traffic stream.
     */
    core_traffic_stream_c(
	    u8_t tid,
	    u8_t user_priority );

    /**
     * Destructor.
     */
    virtual ~core_traffic_stream_c();

    /**
     * Get the TID value of the traffic stream.
     *
     * @since S60 v3.2
     * @return The TID value of the traffic stream.
     */
    u8_t tid() const;
    
    /**
     * Get the User Priority value of the traffic stream.
     *
     * @since S60 v3.2
     * @return The User Priority value of the traffic stream.
     */
    u8_t user_priority() const;
    
    /**
     * Get the access class of the traffic stream.
     * 
     * @since S60 v5.1
     * @return The access class of the traffic stream.
     */
    core_access_class_e access_class() const;
    
    /**
     * Set the default traffic values.
     *
     * @since S60 v3.2
     * @param params Traffic stream parameters.
     */
    void set_default_traffic_values(
        const core_traffic_stream_params_s& params );

    /**
     * Override the default traffic values with different values.
     *
     * @since S60 v3.2
     * @param params Traffic stream parameters.
     */
    void set_traffic_values(
        const core_traffic_stream_params_s& params );

    /**
     * Reset the traffic stream back to default values.
     *
     * @since S60 v3.2
     */
    void reset_to_default_values();

    /**
     * Reset the traffic stream back to previous values.
     *     
     * @since S60 v3.2
     */
    void reset_to_previous_values();

    /**
     * Get the current status of the traffic stream.
     *
     * @since S60 v3.2
     * @return The current status of the traffic stream.
     */
    core_traffic_stream_status_e status() const;

    /**
     * Set the current status of the traffic stream.
     *
     * @since S60 v3.2
     * @param status The current status of the traffic stream.
     */
    void set_status(    
        core_traffic_stream_status_e status );

    /**
     * Return the current amount of virtual traffic streams
     * referencing this traffic stream.
     * 
     * @since S60 v3.2
     * @return The current reference count. 
     */
    u8_t reference_count();
    
    /**
     * Increase the reference count by one.
     * 
     * @since S60 v3.2
     */
    void inc_reference_count();
    
    /**
     * Decrease the reference count by one.
     * 
     * @since S60 v3.2
     */
    void dec_reference_count();
    
    /**
     * Get the traffic pattern.
     *
     * @since S60 v3.2
     * @return Whether the traffic pattern is periodic or aperiodic.     
     */
    bool_t is_periodic_traffic() const;

    /**
     * Return the Direction field of the traffic stream.
     * 
     * @since S60 v5.1
     * @return The Direction field of the frame.
     */
    core_traffic_stream_direction_e direction() const; 
    
    /**
     * Get the Nominal MSDU Size of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Nominal MSDU Size of the traffic stream.
     */
    u16_t nominal_msdu_size() const;
    
    /**
     * Get the Maximum MSDU Size of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Maximum MSDU Size of the traffic stream.
     */    
    u16_t maximum_msdu_size() const;

    /**
     * Get the Minimum Service Interval of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Minimum Service Interval of the traffic stream.
     */    
    u32_t minimum_service_interval() const;

    /**
     * Get the Maximum Service Interval of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Maximum Service Interval of the traffic stream.
     */
    u32_t maximum_service_interval() const;

    /**
     * Get the Inactivity Interval of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Inactivity Interval of the traffic stream.
     */
    u32_t inactivity_interval() const;

    /**
     * Get the Suspension Interval of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Suspension Interval of the traffic stream.
     */   
    u32_t suspension_interval() const;

    /**
     * Get the Service Start Time of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Service Start Time of the traffic stream.
     */
    u32_t service_start_time() const;

    /**
     * Get the Minimum Data Rate of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Minimum Data Rate of the traffic stream.
     */
    u32_t minimum_data_rate() const;

    /**
     * Get the Mean Data Rate of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Mean Data Rate of the traffic stream.
     */   
    u32_t mean_data_rate() const;

    /**
     * Get the Peak Data Rate of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Peak Data Rate of the traffic stream.
     */    
    u32_t peak_data_rate() const;

    /**
     * Get the Maximum Burst Size of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Maximum Burst Size of the traffic stream.
     */
    u32_t maximum_burst_size() const;

    /**
     * Get the Delay Bound of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Delay Bound of the traffic stream.
     */
    u32_t delay_bound() const;

    /**
     * Get the Minimum PHY Rate of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Minimum PHY Rate of the traffic stream.
     */
    u32_t minimum_phy_rate() const;

    /**
     * Get the Surplus Bandwidth Allowance of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Surplus Bandwidth Allowance of the traffic stream.
     */
    u16_t surplus_bandwidth_allowance() const;

    /**
     * Get the Medium Time of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Medium Time of the traffic stream.
     */    
    u16_t medium_time() const;

    /**
     * Get the Nominal PHY Rate of the traffic stream.
     *
     * @since S60 v3.2
     * @return The Nominal PHY Rate of the traffic stream.
     */
    core_tx_rate_e nominal_phy_rate() const;

    /**
     * Get the rates for overriding the tx rate policy.
     * @since S60 v3.2
     * @return The Nominal PHY Rate of the traffic stream, 0 if defaults are used
     */
    u32_t override_rates() const;

    /**
     * Get the Maximum Transmit MSDU Lifetime.
     * @since S60 v3.2
     * @return The Maximum Transmit MSDU Lifetime of the traffic stream.
     */
    u32_t max_tx_msdu_lifetime() const;

    /**
     * Assignment operator.
     */
    core_traffic_stream_c& operator=(
        const core_traffic_stream_c& src );

private: // data

    /**
     * The TID of the traffic stream.
     */
    u8_t tid_m;
    
    /**
     * The User Priority of the traffic stream.
     */    
    u8_t user_priority_m;

    /**
     * The default traffic stream parameters.
     */
    core_traffic_stream_params_s default_params_m;

    /**
     * The current traffic stream parameters.
     */
    core_traffic_stream_params_s params_m;
    
    /**
     * The previous traffic stream parameters.
     */
    core_traffic_stream_params_s previous_params_m;

    /**
     * The current status of the traffic stream.
     */
    core_traffic_stream_status_e status_m;

    /**
     * The current amount of virtual traffic streams
     * referencing this traffic stream.
     */
    u8_t reference_count_m;
    
    };

#endif // CORE_TRAFFIC_STREAM_H
