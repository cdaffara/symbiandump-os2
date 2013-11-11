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
* Description:  Class holding all IAP related data
*
*/

/*
* %version: 11 %
*/

#ifndef CORE_IAP_DATA_H
#define CORE_IAP_DATA_H

#include "core_types.h"
#include "core_type_list.h"

/**
 * Class holding all IAP related data
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_iap_data_c )
    {

public:

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param iap_data containing structure of IAP data
     */
    core_iap_data_c(
        const core_iap_data_s& iap_data );

    /**
     * Destructor
     */
    ~core_iap_data_c();
    
    /**
     * Return the ID of the IAP.
     *
     * @since S60 v3.1
     * @return The ID of the IAP.
     */
    u32_t id() const;
    
    /**
     * Return the IAP ID of the IAP.
     *
     * @since S60 v5.2
     * @return The IAP ID of the IAP.
     */
    u32_t iap_id() const;
    
    /**
     * Return the configured operating mode.
     *
     * @since S60 v3.1
     * @return The configured operating mode.
     */    
    core_operating_mode_e operating_mode() const;        
    
    /**
     * Return the configured SSID.
     *
     * @since S60 v3.1
     * @return The configured SSID.
     */
    const core_ssid_s& ssid() const;
    
    /**
     * Check whether the SSID is hidden.
     *
     * @since S60 v3.1
     * @return Whether the SSID is hidden.
     */    
    bool_t is_hidden() const;
    
    /**
     * Return the requested WEP key.
     *
     * @since S60 v3.1
     * @param Index of the WEP key ( 1 - 4 ).
     * @return The requested WEP key.
     */    
    const core_wep_key_s& wep_key(
        u32_t key_index ) const;

    /**
     * Return the index of the default WEP key.
     *
     * @since S60 v3.1
     * @return The index of the default WEP key.
     */        
    u32_t default_wep_key_index() const;

    /**
     * Return the configured security mode.
     *
     * @since S60 v3.1
     * @return The configured security mode.
     */     
    core_security_mode_e security_mode() const;

    /**
     * Return the configured authentication mode.
     *
     * @since S60 v3.1
     * @return The configured authentication mode.
     */    
    core_authentication_mode_e authentication_mode() const;

    /**
     * Set the authentication mode to the given mode.
     *
     * @since S60 v3.2
     * @param mode Authentication mode to be set.     
     */
    void set_authentication_mode(
        core_authentication_mode_e mode );

    /**
     * Check whether PSK is used or not.
     *
     * @since S60 v3.1
     * @return Whether PSK is used or not.
     */
    bool_t is_psk_used() const;
    
    /**
     * Check whether PSK key has been overridden.
     *
     * @since S60 v3.1
     * @return Whether PSK key has been overridden.
     */    
    bool_t is_psk_overridden() const;
    
    /**
     * Return the overridden PSK key.
     *
     * @since S60 v3.1
     * @return The overridden PSK key.
     */    
    const core_wpa_preshared_key_s& psk_key() const;
    
    /**
     * Return the used ad-hoc channel.
     *
     * @since S60 v3.1 
     * @return The used ad-hoc channel. 0 if automatic configuration is used.
     */
    u32_t adhoc_channel() const;
    
    /**
     * Check whether IP address is configured dynamically.
     *
     * @since S60 v3.1
     * @return Whether IP address is configured dynamically.
     */   
    bool_t is_dynamic_ip_addr() const;
    
    /**
     * Check whether roaming between APs is allowed.
     *
     * @since S60 v3.1
     * @return Whether roaming between APs is allowed.
     */    
    bool_t is_roaming_allowed() const;
    
    /**
     * Check whether the configured security mode uses EAP.
     *
     * @since S60 v3.1
     * @return Whether the configured security mode uses EAP.
     */
    bool_t is_eap_used() const;

    /**
     * Check whether the configured security mode uses WAPI.
     *
     * @since S60 v3.2
     * @return Whether the configured security mode uses WAPI.
     */
    bool_t is_wapi_used() const;
    
    /**
     * Check whether the configured security mode is 802.1x.
     *
     * @since S60 v3.1
     * @return Whether the configured security mode is 802.1x.
     */   
    bool_t is_802dot1x_used() const;
    
    /**
     * Check whether the IAP whitelist is empty.
     *
     * @since S60 v3.1
     * @return Whether the IAP whitelist is empty.
     */    
    bool_t is_iap_whitelist_empty() const;
    
    /**
     * Check whether the given MAC address is whitelisted.
     *
     * @since S60 v3.1
     * @param mac MAC address to be checked.
     * @return Whether the given MAC address is whitelisted.
     */    
    bool_t is_mac_in_iap_whitelist(
        const core_mac_address_s& mac );

    /**
     * Check whether the given MAC address is blacklisted.
     *
     * @since S60 v3.1
     * @param mac MAC address to be checked.
     * @return Whether the given MAC address is blacklisted.
     */    
    bool_t is_mac_in_iap_blacklist(
        const core_mac_address_s& mac );

    /**
     * Add a MAC address to the IAP whitelist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be added.
     */    
    void add_mac_to_iap_whitelist(
        const core_mac_address_s& mac );

    /**
     * Remove a MAC address from the IAP whitelist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be removed.
     */    
    void remove_mac_from_iap_whitelist(
        const core_mac_address_s& mac );

    /**
     * Add a MAC address to the IAP blacklist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be added.
     */           
    void add_mac_to_iap_blacklist(
        const core_mac_address_s& mac );

    /**
     * Remove a MAC address from the IAP blacklist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be removed.
     */    
    void remove_mac_from_iap_blacklist(
        const core_mac_address_s& mac );

    /**
     * Check whether the IAP data contains a valid
     * combination of parameters. 
     */    
    bool_t is_valid() const;

private: // data

    /** Adaptation side data about the WLAN connection */
    core_iap_data_s iap_data_m;

    /** IAP whitelist of APs. */
    core_type_list_c<core_mac_address_s> iap_whitelist_m;

    /** IAP blacklist of APs. */
    core_type_list_c<core_mac_address_s> iap_blacklist_m;
    };

#endif // CORE_IAP_DATA_H
