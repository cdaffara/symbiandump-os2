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
* %version: 16 %
*/

#include "core_iap_data.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_iap_data_c::core_iap_data_c(
    const core_iap_data_s& iap_data ) :
    iap_data_m( iap_data ),
    iap_whitelist_m( ),
    iap_blacklist_m( )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_iap_data_c::~core_iap_data_c()
    {
    iap_whitelist_m.clear();
    iap_blacklist_m.clear();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_iap_data_c::id() const
    {
    return iap_data_m.id;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_iap_data_c::iap_id() const
    {
    return iap_data_m.iap_id;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operating_mode_e core_iap_data_c::operating_mode() const
    {
    return iap_data_m.op_mode;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_ssid_s& core_iap_data_c::ssid() const
    {
    return iap_data_m.ssid;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_hidden() const
    {
    return iap_data_m.is_hidden;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_wep_key_s& core_iap_data_c::wep_key(
    u32_t key_index ) const
    {
    switch ( key_index )
        {
        case WEP_KEY1:
            return iap_data_m.wep_key1;
        case WEP_KEY2:
            return iap_data_m.wep_key2;
        case WEP_KEY3:
            return iap_data_m.wep_key3;
        default:
            return iap_data_m.wep_key4;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_iap_data_c::default_wep_key_index() const
    {
    return iap_data_m.default_wep_key;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_security_mode_e core_iap_data_c::security_mode() const
    {
    return iap_data_m.security_mode;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_authentication_mode_e core_iap_data_c::authentication_mode() const
    {
    return iap_data_m.authentication_mode;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_iap_data_c::set_authentication_mode(
    core_authentication_mode_e mode )
    {
    iap_data_m.authentication_mode = mode;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_psk_used() const
    {
    return iap_data_m.wpa_preshared_key_in_use;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_psk_overridden() const
    {
    return iap_data_m.is_wpa_overriden;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_wpa_preshared_key_s& core_iap_data_c::psk_key() const
    {
    return iap_data_m.wpa_preshared_key;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_iap_data_c::adhoc_channel() const
    {
    return iap_data_m.used_adhoc_channel;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_dynamic_ip_addr() const
    {
    return iap_data_m.is_dynamic_ip_addr;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_roaming_allowed() const
    {
    return iap_data_m.is_roaming_allowed;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_eap_used() const
    {
    if ( iap_data_m.security_mode == core_security_mode_802dot1x ||
        iap_data_m.security_mode == core_security_mode_wpa ||
        iap_data_m.security_mode == core_security_mode_wpa2only ||
        iap_data_m.security_mode == core_security_mode_protected_setup ||
        iap_data_m.security_mode == core_security_mode_802dot1x_unencrypted )
        {
        return true_t;
        }
    
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_wapi_used() const
    {
    return( iap_data_m.security_mode == core_security_mode_wapi );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_802dot1x_used() const
    {
    return iap_data_m.security_mode == core_security_mode_802dot1x;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_iap_whitelist_empty() const
    {
    return !iap_whitelist_m.count();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_mac_in_iap_whitelist(
    const core_mac_address_s& mac )
    {
    core_mac_address_s* addr = iap_whitelist_m.first();    
    while ( addr )
        {
        if ( *addr == mac )
            {
            return true_t;
            }
        
        addr = iap_whitelist_m.next();
        }        
    
    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_mac_in_iap_blacklist(
    const core_mac_address_s& mac )
    {
    core_mac_address_s* addr = iap_blacklist_m.first();    
    while ( addr )
        {
        if ( *addr == mac )
            {
            return true_t;
            }
        
        addr = iap_blacklist_m.next();
        }        

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_iap_data_c::add_mac_to_iap_whitelist(
    const core_mac_address_s& mac )
    {
    if ( !is_mac_in_iap_whitelist( mac ) )
        {
        core_mac_address_s* entry = new core_mac_address_s;
        if ( entry )
            {
            *entry = mac;
            iap_whitelist_m.append( entry );
            }                        
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_iap_data_c::remove_mac_from_iap_whitelist(
    const core_mac_address_s& mac )
    {
    core_mac_address_s* addr = iap_whitelist_m.first();    
    while ( addr )
        {
        if ( *addr == mac )
            {
            iap_whitelist_m.remove( addr );
            delete addr;

            return;
            }
        
        addr = iap_whitelist_m.next();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_iap_data_c::add_mac_to_iap_blacklist(
    const core_mac_address_s& mac )
    {
    if ( !is_mac_in_iap_blacklist( mac ) )
        {
        core_mac_address_s* entry = new core_mac_address_s;
        if ( entry )
            {
            *entry = mac;
            iap_blacklist_m.append( entry );
            }                        
        }       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_iap_data_c::remove_mac_from_iap_blacklist(
    const core_mac_address_s& mac )
    {
    core_mac_address_s* addr = iap_blacklist_m.first();    
    while ( addr )
        {
        if ( *addr == mac )
            {
            iap_blacklist_m.remove( addr );
            delete addr;

            return;
            }
        
        addr = iap_blacklist_m.next();
        }       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_iap_data_c::is_valid() const
    {
    if( iap_data_m.security_mode == core_security_mode_wep )
        {
        if( !iap_data_m.wep_key1.key_length &&
            !iap_data_m.wep_key2.key_length &&
            !iap_data_m.wep_key3.key_length &&
            !iap_data_m.wep_key4.key_length )
            {
            DEBUG( "core_iap_data_c::is_valid() - security mode is WEP but no keys defined" );

            return false_t;
            }

        if( ( iap_data_m.default_wep_key > WEP_KEY4 ) ||             
            ( iap_data_m.default_wep_key == WEP_KEY1 &&
              !iap_data_m.wep_key1.key_length ) ||
            ( iap_data_m.default_wep_key == WEP_KEY2 &&
              !iap_data_m.wep_key2.key_length ) ||
            ( iap_data_m.default_wep_key == WEP_KEY3 &&
              !iap_data_m.wep_key3.key_length ) ||
            ( iap_data_m.default_wep_key == WEP_KEY4 &&
              !iap_data_m.wep_key4.key_length ) )
            {
            DEBUG( "core_iap_data_c::is_valid() - security mode is WEP but default key not defined" );

            return false_t;
            }
        }

    if( iap_data_m.wpa_preshared_key_in_use &&
        !iap_data_m.wpa_preshared_key.key_length )
        {
        DEBUG( "core_iap_data_c::is_valid() - security mode is WPA-PSK but key is not defined" );

        return false_t;   
        }
    
    if( iap_data_m.op_mode == core_operating_mode_ibss &&
        ( iap_data_m.security_mode != core_security_mode_allow_unsecure &&
          iap_data_m.security_mode != core_security_mode_wep ) )
        {
        DEBUG1( "core_iap_data_c::is_valid() - IBSS mode with security mode %u not supported",
            iap_data_m.security_mode );

        return false_t;        
        }

    return true_t;
    }
