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
* Description:  Class for generating WMM TSPEC elements.
*
*/

/*
* %version: 3 %
*/

#ifndef CORE_FRAME_WMM_IE_TSPEC_H
#define CORE_FRAME_WMM_IE_TSPEC_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for generating WMM TSPEC elements.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_frame_wmm_ie_tspec_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param ie Pointer to the IE data.
     * @return The A pointer to the created IE instance.
     */
    static core_frame_wmm_ie_tspec_c* instance(
        const core_frame_dot11_ie_c& ie );

    /**
     * Factory for creating a TSPEC IE with the given parameters.
     *
     * @since S60 v3.2
     * @param tid Traffic Identifier field of the frame.
     * @param user_priority User Priority field of the frame.
     * @param is_uapsd_enabled Whether U-APSD is enabled.
     * @param is_periodic_traffic Whether the traffic pattern is periodic or aperiodic.
     * @param direction Direction field of the frame.
     * @param nominal_msdu_size Nominal MSDU Size field of the frame.
     * @param maximum_msdu_size Maximum MSDU Size field of the frame.
     * @param minimum_service_interval Minimum Service Interval field of the frame.
     * @param maximum_service_interval Maximum Service Interval field of the frame.
     * @param inactivity_interval Inactivity Interval field of the frame.
     * @param suspension_interval Suspension Interval field of the frame.
     * @param service_start_time Service Start Time field of the frame.
     * @param minimum_data_rate Minimum Data Rate field of the frame.
     * @param mean_data_rate Mean Data Rate field of the frame.
     * @param peak_data_rate Peak Data Rate field of the frame.
     * @param maximum_burst_size Maximum Burst Size field of the frame.
     * @param delay_bound Delay Bound field of the frame.
     * @param minimum_phy_rate Minimum PHY Rate field of the frame.
     * @param surplus_bandwidth_allowance Surplus Bandwidth Allowance field of the frame.
     * @param medium_time Medium Time field of the frame.
     * @return A pointer to the created IE instance.
     */
    static core_frame_wmm_ie_tspec_c* instance(
        u8_t tid,
        u8_t user_priority,
        bool_t is_uapsd_enabled,
        bool_t is_periodic_traffic,
        core_traffic_stream_direction_e direction,
	    u16_t nominal_msdu_size,
	    u16_t maximum_msdu_size,
	    u32_t minimum_service_interval,
	    u32_t maximum_service_interval,
	    u32_t inactivity_interval,
	    u32_t suspension_interval,
	    u32_t service_start_time,
	    u32_t minimum_data_rate,
    	u32_t mean_data_rate,
    	u32_t peak_data_rate,
	    u32_t maximum_burst_size,
	    u32_t delay_bound,
	    u32_t minimum_phy_rate,
	    u16_t surplus_bandwidth_allowance,
	    u16_t medium_time );

    /** 
     * Destructor.
     */
    virtual ~core_frame_wmm_ie_tspec_c();

    /**
     * Return the Version field.
     * @return The Version field.
     */
    u8_t version() const;

    /**
     * Return the Traffic Identifier field of the frame.
     *
     * @since S60 v3.2
     * @return The Traffic Identifier field of the frame.
     */
    u8_t tid() const;

    /**
     * Return the User Priority field of the frame.
     *
     * @since S60 v3.2
     * @return The User Priority field of the frame.
     */
    u8_t user_priority() const;

    /**
     * Return the whether U-APSD is enabled.
     *
     * @since S60 v3.2
     * @return Whether U-APSD is enabled.
     */
    bool_t is_uapsd_enabled() const;

    /**
     * Return whether the traffic pattern is periodic or aperiodic.
     *
     * @since S60 v3.2
     * @return Whether the traffic pattern is periodic or aperiodic.
     */
    bool_t is_periodic_traffic() const;
    
    /**
     * Return the Direction field of the frame.
     * 
     * @since S60 v5.1
     * @return The Direction field of the frame.
     */
    core_traffic_stream_direction_e direction() const;
    
    /**
     * Return the Nominal MSDU Size field of the frame.
     *
     * @since S60 v3.2
     * @return The Nominal MSDU Size field of the frame.
     */
    u16_t nominal_msdu_size() const;

    /**
     * Return the Maximum MSDU Size field of the frame.
     *
     * @since S60 v3.2
     * @return The Maximum MSDU Size field of the frame.
     */
    u16_t maximum_msdu_size() const;

    /**
     * Return the Minimum Service Interval field of the frame.
     *
     * @since S60 v3.2
     * @return The Minimum Service Interval field of the frame.
     */
    u32_t minimum_service_interval() const;

    /**
     * Return the Maximum Service Interval field of the frame.
     *
     * @since S60 v3.2
     * @return The Maximum Service Interval field of the frame.
     */
    u32_t maximum_service_interval() const;

    /**
     * Return the Inactivity Interval field of the frame.
     *
     * @since S60 v3.2
     * @return The Inactivity Interval field of the frame.
     */
    u32_t inactivity_interval() const;

    /**
     * Return the Suspension Interval field of the frame.
     *
     * @since S60 v3.2
     * @return The Suspension Interval field of the frame.
     */
    u32_t suspension_interval() const;

    /**
     * Return the Service Start Time field of the frame.
     *
     * @since S60 v3.2
     * @return The Service Start Time field of the frame.
     */
    u32_t service_start_time() const;

    /**
     * Return the Minimum Data Rate field of the frame.
     *
     * @since S60 v3.2
     * @return The Minimum Data Rate field of the frame.
     */
    u32_t minimum_data_rate() const;

    /**
     * Return the Mean Data Rate field of the frame.
     *
     * @since S60 v3.2
     * @return The Mean Data Rate field of the frame.
     */
    u32_t mean_data_rate() const;

    /**
     * Return the Peak Data Rate field of the frame.
     *
     * @since S60 v3.2
     * @return The Peak Data Rate field of the frame.
     */
    u32_t peak_data_rate() const;

    /**
     * Return the Maximum Burst Size field of the frame.
     *
     * @since S60 v3.2
     * @return The Maximum Burst Size field of the frame.
     */
    u32_t maximum_burst_size() const;

    /**
     * Return the Delay Bound field of the frame.
     *
     * @since S60 v3.2
     * @return The Delay Bound field of the frame.
     */
    u32_t delay_bound() const;

    /**
     * Return the Minimum PHY Rate field of the frame.
     *
     * @since S60 v3.2
     * @return The Minimum PHY Rate field of the frame.
     */
    u32_t minimum_phy_rate() const;

    /**
     * Return the Surplus Bandwidth Allowance field of the frame.
     *
     * @since S60 v3.2
     * @return The Surplus Bandwidth Allowance field of the frame.
     */
    u16_t surplus_bandwidth_allowance() const;

    /**
     * Return the Medium Time field of the frame.
     *
     * @since S60 v3.2
     * @return The Medium Time field of the frame.
     */
    u16_t medium_time() const;

private:

    /**
     * Generate a TSPEC IE.
     *
     * @param tid Traffic Identifier field of the frame.
     * @param user_priority User Priority field of the frame.
     * @param is_uapsd_enabled Whether U-APSD is enabled.
     * @param is_periodic_traffic Whether the traffic pattern is periodic or aperiodic.
     * @param direction Direction field of the frame.
     * @param nominal_msdu_size Nominal MSDU Size field of the frame.
     * @param maximum_msdu_size Maximum MSDU Size field of the frame.
     * @param minimum_service_interval Minimum Service Interval field of the frame.
     * @param maximum_service_interval Maximum Service Interval field of the frame.
     * @param inactivity_interval Inactivity Interval field of the frame.
     * @param suspension_interval Suspension Interval field of the frame.
     * @param service_start_time Service Start Time field of the frame.
     * @param minimum_data_rate Minimum Data Rate field of the frame.
     * @param mean_data_rate Mean Data Rate field of the frame.
     * @param peak_data_rate Peak Data Rate field of the frame.
     * @param maximum_burst_size Maximum Burst Size field of the frame.
     * @param delay_bound Delay Bound field of the frame.
     * @param minimum_phy_rate Minimum PHY Rate field of the frame.
     * @param surplus_bandwidth_allowance Surplus Bandwidth Allowance field of the frame.
     * @param medium_time Medium Time field of the frame.     
     */
    void generate(
        u8_t tid,
        u8_t user_priority,
        bool_t is_uapsd_enabled,
        bool_t is_periodic_traffic,
        core_traffic_stream_direction_e direction,
	    u16_t nominal_msdu_size,
	    u16_t maximum_msdu_size,
	    u32_t minimum_service_interval,
	    u32_t maximum_service_interval,
	    u32_t inactivity_interval,
	    u32_t suspension_interval,
	    u32_t service_start_time,
	    u32_t minimum_data_rate,
    	u32_t mean_data_rate,
    	u32_t peak_data_rate,
	    u32_t maximum_burst_size,
	    u32_t delay_bound,
	    u32_t minimum_phy_rate,
	    u16_t surplus_bandwidth_allowance,
	    u16_t medium_time );

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_wmm_ie_tspec_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

#endif // CORE_FRAME_WMM_IE_TSPEC_H
