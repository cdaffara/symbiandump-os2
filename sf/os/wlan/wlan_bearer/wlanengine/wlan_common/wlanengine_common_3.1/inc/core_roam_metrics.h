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
* Description:  Class for storing roaming metrics.
*
*/


#ifndef CORE_ROAM_METRICS_H
#define CORE_ROAM_METRICS_H

#include "core_types.h"

/**
 * Class for storing traffic stream parameters.
 *
 * @lib wlmserversrv.dll
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_roam_metrics_c )
    {

public:

    /**
     * Constructor.
     */
    core_roam_metrics_c();
    
    /**
     * Destructor.
     */
    ~core_roam_metrics_c();

    /**
     * Set all the metrics to initial values.
     *
     * @since S60 v3.2
     */
    void clear_metrics();

    /**
     * Return the timestamp when user data was disabled.
     */
    u64_t roam_ts_userdata_disabled() const;

    /**
     * Update the timestamp when user data was disabled.
     *
     * @since S60 v3.2
     */    
    void set_roam_ts_userdata_disabled();

    /**
     * Return the timestamp when user data was enabled.
     */
    u64_t roam_ts_userdata_enabled() const;

    /**
     * Update the timestamp when user data was enabled.
     *
     * @since S60 v3.2
     */    
    void set_roam_ts_userdata_enabled();

    /**
     * Return the timestamp when connect request to drivers was issued.
     */
    u64_t roam_ts_connect_started() const;

    /**
     * Update the timestamp when connect request to drivers was issued.
     *
     * @since S60 v3.2
     */    
    void set_roam_ts_connect_started();

    /**
     * Return the timestamp when connect request to drivers was completed.
     */
    u64_t roam_ts_connect_completed() const;

    /**
     * Update the timestamp when connect request to drivers was completed.
     *
     * @since S60 v3.2
     */    
    void set_roam_ts_connect_completed();

    /**
     * Get the amount of microseconds user data was disabled during last roam.
     *
     * @since S60 v3.2
     * @return The amount of microseconds user data was disabled during last roam.
     */
    u32_t roam_total_delay() const;

    /**
     * Get the amount of microseconds taken to connect the network.
     *
     * @since S60 v3.2
     * @return The amount of microseconds taken to connect the network.
     */
    u32_t roam_connect_delay() const;

    /**
     * Get the total number of successful roams.
     *
     * @since S60 v3.2
     * @return The total number of successful roams.
     */    
    u16_t roam_success_count() const;

    /**
     * Increase the total number of successful roams.
     *
     * @since S60 v3.2
     * @param count Parameter to be set.
     */    
    void inc_roam_success_count();

    /**
     * Get the amount of roam attempts.
     *
     * @since S60 v3.2
     * @param reason Type of count to return.
     * @return The amount of roam attempts.
     */
    u16_t roam_attempt_count(
        core_roam_reason_e reason ) const;

    /**
     * Increase the amount of roam attempts.
     *
     * @since S60 v3.2
     * @param reason Type of count to increase.
     */
    void inc_roam_attempt_count(
        core_roam_reason_e reason );

    /**
     * Return the amount of failed roam attempts.
     *
     * @since S60 v3.2
     * @param reason Type of count to return.
     * @return The amount of failed roam attempts.
     */
    u16_t roam_attempt_failed_count(
        core_roam_failed_reason_e reason ) const;

    /**
     * Increase the amount of roam failed attempts.
     *
     * @since S60 v3.2
     * @param reason Type of count to increase.
     */
    void inc_roam_attempt_failed_count(
        core_roam_failed_reason_e reason );

    /**
     * Get the reason for the last roam.
     *
     * @since S60 v3.2
     * @return The reason for the last roam.
     */    
    core_roam_reason_e last_roam_reason() const;

    /**
     * Set the reason for the last roam.
     *
     * @since S60 v3.2
     * @param reason The reason for the last roam.
     */    
    void set_last_roam_reason(
        core_roam_reason_e reason );

    /**
     * Get the reason for the last roam failure.
     *
     * @since S60 v3.2
     * @return The reason for the last roam failure.
     */    
    core_roam_failed_reason_e last_roam_failed_reason() const;

    /**
     * Set the reason for the last roam failure.
     *
     * @since S60 v3.2
     * @param reason The reason for the last roam failure.
     */    
    void set_last_roam_failed_reason(
        core_roam_failed_reason_e reason );

    /**
     * Trace the current roam metrics.
     * 
     * @since S60 v3.2
     */
    void trace_current_roam_metrics() const;

private:

    /**
     * Assignment operator.
     */
    core_roam_metrics_c& operator=(
        const core_roam_metrics_c& src );

private: // data

    /** The timestamp when userdata was disabled. */
    u64_t roam_ts_userdata_disabled_m;

    /** The timestamp when userdata was enabled. */
    u64_t roam_ts_userdata_enabled_m;

    /** The timestamp when connect request to drivers was issued. */
    u64_t roam_ts_connect_start_m;

    /** The timestamp when connect request to drivers was completed. */
    u64_t roam_ts_connect_completed_m;

    /** The total number of successful roams. */
    u16_t roam_success_count_m;

    /** The amount of roam attempts. */
    u16_t roam_attempt_count_m[core_roam_reason_max];

    /** The amount of failed roam attempts. */
    u16_t roam_attempt_failed_count_m[core_roam_failed_reason_max];

    };

#endif // CORE_ROAM_METRICS_H
