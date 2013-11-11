/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the ScanInfoIe class.
*
*/


#include "genscaninfoie.h"

// Defines the id byte of the RSN Information Element.
const u8_t SCANINFOIE_RSNIE_ID = 48;

// Defines the static offsets for different fields in RSN IE.
const u32_t SCANINFOIE_RSNIE_GROUP_SUITE_OFFSET = 2;
const u32_t SCANINFOIE_RSNIE_PAIRWISE_SUITE_COUNT_OFFSET = 6;
const u32_t SCANINFOIE_RSNIE_PAIRWISE_SUITE_OFFSET = 8;

// Defines the OUIs used in RSN IEs.
const u32_t SCANINFOIE_OUI_LENGTH = 4;
const u8_t SCANINFOIE_RSNIE_OUI_CCMP[] = { 0x00, 0x0F, 0xAC, 0x04 };
const u8_t SCANINFOIE_RSNIE_OUI_EAP[] = { 0x00, 0x0F, 0xAC, 0x01 };
const u8_t SCANINFOIE_RSNIE_OUI_PSK[] = { 0x00, 0x0F, 0xAC, 0x02 };

// Defines the static offsets for different fields in RSN IE.
const u32_t SCANINFOIE_WPAIE_PAIRWISE_SUITE_COUNT_OFFSET = 10;
const u32_t SCANINFOIE_WPAIE_PAIRWISE_SUITE_OFFSET = 12;

// Defines the OUIs used in WPA IEs.
const u8_t SCANINFOIE_WPAIE_OUI_EAP[] = { 0x00, 0x50, 0xF2, 0x01 };
const u8_t SCANINFOIE_WPAIE_OUI_PSK[] = { 0x00, 0x50, 0xF2, 0x02 };

// Defines the id byte of the WAPI Information Element.
const u8_t SCANINFOIE_WAPI_ID = 68;

// Defines the static offsets for different fields in WAPI IE.
const u32_t SCANINFOIE_WAPI_KEY_MANAGEMENT_COUNT_OFFSET = 2;

// Defines the OUIs used in WAPI IEs.
const u8_t SCANINFOIE_WAPI_OUI_CERTIFICATE[] = { 0x00, 0x14, 0x72, 0x01 };
const u8_t SCANINFOIE_WAPI_OUI_PSK[] = { 0x00, 0x14, 0x72, 0x02 };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
ScanInfoIe::ScanInfoIe()
    {
    }
    
// Destructor
ScanInfoIe::~ScanInfoIe()
    {    
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
WlanSecurityMode ScanInfoIe::SecurityMode(
    ScanInfo& info ) const
    {
    u8_t wpaie_length( 0 );
    const u8_t* wpaie_data = NULL;
    u8_t rsnie_length( 0 );
    const u8_t* rsnie_data = NULL;
    u8_t wapi_length( 0 );
    const u8_t* wapi_data = NULL;

    info.InformationElement( SCANINFOIE_RSNIE_ID, rsnie_length, &rsnie_data );
    info.WpaIE( wpaie_length, &wpaie_data );
    info.InformationElement( SCANINFOIE_WAPI_ID, wapi_length, &wapi_data );

    if ( !info.Privacy() )
        {
        if ( !wpaie_length && !rsnie_length )
            {
            return WlanSecurityModeOpen;
            }
        return WlanSecurityMode802_1x;
        }
        
    if ( !wpaie_length && !rsnie_length && !wapi_length )
        {
        return WlanSecurityModeWep;
        }

    if ( rsnie_length )
        {        
        if ( IsKeyManagement(
            ScanInfoIeTypeRsn,
            ScanInfoIeKeyManagementPsk,
            rsnie_length,
            rsnie_data ) )
            {
            if ( IsWpa2Ciphers(
                ScanInfoIeTypeRsn,
                rsnie_length,
                rsnie_data ) )
                {
                return WlanSecurityModeWpa2Psk;
                }
            
            return WlanSecurityModeWpaPsk;            
            }
        else if ( IsKeyManagement(
            ScanInfoIeTypeRsn,
            ScanInfoIeKeyManagementEap,
            rsnie_length,
            rsnie_data ) )
            {
            if ( IsWpa2Ciphers(
                ScanInfoIeTypeRsn,
                rsnie_length,
                rsnie_data ) )
                {                
                return WlanSecurityModeWpa2Eap;
                }
            
            return WlanSecurityModeWpaEap;            
            }                        
        }
        
    if ( wpaie_length )
        {
        if ( IsKeyManagement(
            ScanInfoIeTypeWpa,
            ScanInfoIeKeyManagementPsk,
            wpaie_length,
            wpaie_data ) )
            {
            return WlanSecurityModeWpaPsk;            
            }
        else if ( IsKeyManagement(
            ScanInfoIeTypeWpa,
            ScanInfoIeKeyManagementEap,
            wpaie_length,
            wpaie_data ) )
            {
            return WlanSecurityModeWpaEap;            
            }        
        }

    if ( wapi_length )
        {
        if ( IsKeyManagement(
            ScanInfoIeTypeWapi,
            ScanInfoIeKeyManagementWapiCertificate,
            wapi_length,
            wapi_data ) )
            {
            return WlanSecurityModeWapi;            
            }
        else if ( IsKeyManagement(
            ScanInfoIeTypeWapi,
            ScanInfoIeKeyManagementWapiPsk,
            wapi_length,
            wapi_data ) )
            {
            return WlanSecurityModeWapiPsk;            
            }
        }

    return WlanSecurityMode802_1x;
    }
 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//   
int ScanInfoIe::Compare(
    const unsigned char* pl,
    int ll, 
    const unsigned char* pr, 
    int rl ) const
    {
    if ( ll != rl )
        {
        return ll - rl;
        }        

    if ( pl == pr )
        {
        return 0;
        }        

    for ( int i( 0 ); i < ll; ++i )
        {
        if ( *(pl+i) != *(pr+i) )
            {
            return *(pl+i) - *(pr+i);
            }
        }

    return 0;    
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//

bool_t ScanInfoIe::IsKeyManagement(
    ScanInfoIeType ie_type,
    ScanInfoIeKeyManagement key_type,
    u8_t /* ie_length */,
    const u8_t* ie_data ) const
    {
    const u8_t* key_data = NULL;
    const u8_t* key_comp = NULL;
   
    if ( ie_type == ScanInfoIeTypeRsn )
        {
        if ( key_type == ScanInfoIeKeyManagementEap )
            {
            key_comp = &SCANINFOIE_RSNIE_OUI_EAP[0];
            }
        else
            {
            key_comp = &SCANINFOIE_RSNIE_OUI_PSK[0];
            }
        u32_t suites( *( ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_COUNT_OFFSET ) );
        key_data = ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_OFFSET +
            ( suites * SCANINFOIE_OUI_LENGTH );
        }
    else if ( ie_type == ScanInfoIeTypeWpa )
        {
        if ( key_type == ScanInfoIeKeyManagementEap )
            {
            key_comp = &SCANINFOIE_WPAIE_OUI_EAP[0];
            }
        else
            {
            key_comp = &SCANINFOIE_WPAIE_OUI_PSK[0];
            }
        u32_t suites( *( ie_data + SCANINFOIE_WPAIE_PAIRWISE_SUITE_COUNT_OFFSET ) );
        key_data = ie_data + SCANINFOIE_WPAIE_PAIRWISE_SUITE_OFFSET +
            ( suites * SCANINFOIE_OUI_LENGTH );        
        }
    else
        {
        if ( key_type == ScanInfoIeKeyManagementWapiCertificate )
            {
            key_comp = &SCANINFOIE_WAPI_OUI_CERTIFICATE[0];
            }
        else
            {
            key_comp = &SCANINFOIE_WAPI_OUI_PSK[0];
            }
        key_data = ie_data + SCANINFOIE_WAPI_KEY_MANAGEMENT_COUNT_OFFSET;
        }
    
    u16_t key_suites( *key_data );
    key_data += sizeof( key_suites );

    while ( key_suites-- )
        {
        if ( !Compare(
            key_data + ( key_suites * SCANINFOIE_OUI_LENGTH ), SCANINFOIE_OUI_LENGTH,
            key_comp, SCANINFOIE_OUI_LENGTH ) )
            {
            return true_t;
            }
        }

    return false_t;
    };
   
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
bool_t ScanInfoIe::IsWpa2Ciphers(
    ScanInfoIeType ie_type,
    u8_t /* ie_length */,
    const u8_t* ie_data ) const
    {
    /**
    * WPA IE is not used in WPA2
    */    
    if ( ie_type == ScanInfoIeTypeWpa )
        {
        return false_t;
        }

    const u8_t* cipher =
        ie_data + SCANINFOIE_RSNIE_GROUP_SUITE_OFFSET;

    if ( Compare(
        cipher, SCANINFOIE_OUI_LENGTH,
        &SCANINFOIE_RSNIE_OUI_CCMP[0], SCANINFOIE_OUI_LENGTH ) )
        {
        return false_t;
        }

    u32_t suites( *( ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_COUNT_OFFSET ) );
    cipher = ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_OFFSET;

    while ( suites-- )
        {
        if ( !Compare(
            cipher + ( suites * SCANINFOIE_OUI_LENGTH ), SCANINFOIE_OUI_LENGTH,
            &SCANINFOIE_RSNIE_OUI_CCMP[0], SCANINFOIE_OUI_LENGTH ) )
            {
            return true_t;
            }
        }

    return false_t;
    }
