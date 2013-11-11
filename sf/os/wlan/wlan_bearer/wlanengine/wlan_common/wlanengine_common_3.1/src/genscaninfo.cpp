/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to dig information of scan results.
*
*/


#include "genscaninfo.h"
#include "genscaninfoie.h"

// Default values for information element IDs.
const u8_t WPA_IE_ID              = 221;
const u8_t SCAN_WPA_OUI_LENGTH    = 4;
const u8_t SCAN_WPA_OUI[]         = { 0x00, 0x50, 0xF2, 0x01 };
const u8_t SCAN_WSC_OUI[]         = { 0x00, 0x50, 0xF2, 0x04 };

const u8_t SCAN_OUI_TYPE_OFFSET = 3;
const u8_t SCAN_OUI_SUBTYPE_OFFSET = 4;

/** Offset from AttributeType field to DataLength field */
const u8_t SCAN_WSC_IE_DATA_LENGTH_OFFSET = 2;

/** Offset from AttributeType field to Data field */
const u8_t SCAN_WSC_IE_DATA_AREA_OFFSET = 4;

/** Offset to first Protected Setup attribute type in IE */
const u8_t SCAN_WSC_IE_PROTECTED_SETUP_DATA_OFFSET = 2+4;

/** IDs for different data elements in WSC IE. */
const u16_t SCAN_WSC_IE_AP_SETUP_LOCKED       = 0x1057;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
ScanInfo::ScanInfo( const ScanList& scan_list ) :
    ScanListIterator( scan_list ), 
    ie_iter_m( current_m + MGMT_BODY_OFFSET )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanSecurityMode ScanInfo::SecurityMode()
    {
    ScanInfoIe ie;
    return ie.SecurityMode( *this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::InformationElement(
    u8_t ie_id, 
    u8_t& ie_length, 
    const u8_t** ie_data )
    {
    // IE is not found when the whole scan info element is gone through.
    //
    // The Element format is:
    // +----+----+--...--+
    // | a  | b  | c     |
    // +----+----+--...--+
    // where
    // a) Element ID (1 byte)
    // b) Length (1 byte)
    // c) Information (length bytes)
    //

    u8_t ie, len;
    const u8_t* data;

    if ( FirstIE( ie, len, &data ) != WlanScanError_Ok )
        {
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    WlanScanError ret( WlanScanError_Ok );

    while ( ie != ie_id && ret == WlanScanError_Ok )
        {
        ret = NextIE( ie, len, &data );
        }

    ie_length = len;
    *ie_data = data;
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::WpaIE(
    u8_t& ie_length, 
    const u8_t** ie_data )
    {
    // Element format is:
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // | ID | Len | OUI | Ver | GKCS | PKCSC | PKSCL | AKMSC | AKMSL | RSNCap |
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // where
    // ID     = Element ID = 221 (1 octet)
    // Len    = Length (1 octet)
    // OUI    = 00:50:F2:01 (4 octets)
    // Ver    = Version (2 octets)
    // GKCS   = Group Key Cipher Suite (4 octets)
    // PKCSC  = Pairwise Key Cipher Suite Count (2 octets)
    // PKCSL  = Pairwise Key Cipher Suite List (4*m octets)
    // AKMSC  = Authenticated Key Management Suite Count (2 octets)
    // AKMSL  = Authenticated Key Management Suite List (4*n octets)
    // RSNCap = RSN Capabilities

    u8_t ie( WPA_IE_ID ), len;
    const u8_t* data;

    // Find out the first element
    if ( InformationElement( WPA_IE_ID, len, &data ) != WlanScanError_Ok )
        { // Okay, it didn't exist.
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    // The ID is correct but check also the UID.
    WlanScanError ret( WlanScanError_Ok );

    while ( ret == WlanScanError_Ok &&
            ( ie != WPA_IE_ID ||
              len < SCAN_WPA_OUI_LENGTH ||
              !( data[0] == SCAN_WPA_OUI[0] && data[1] == SCAN_WPA_OUI[1] &&
                 data[2] == SCAN_WPA_OUI[2] && data[3] == SCAN_WPA_OUI[3] ) ) )
        {
        ret = NextIE( ie, len, &data );
        }

    // Check is the element was corrupted
    if ( ie != WPA_IE_ID )
        {
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    ie_length = len;
    *ie_data = data;
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::InformationElement(
    u8_t ie_id,
    const WlanIeOui& ie_oui,
    u8_t ie_oui_type,
    u8_t& ie_length, 
    const u8_t** ie_data )
    {
    // Element format is:
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // | ID | Len | OUI |     |      |       |       |       |       |        |
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // where
    // ID     = Element ID (1 octet)
    // Len    = Length (1 octet)
    // OUI    = OUI (3 octets)
    // OUItyp = OUI Type (1 octet)

    u8_t ie( ie_id ); 
    u8_t len( 0 );
    const u8_t* data;

    // Find the first element
    if ( InformationElement( ie_id, len, &data ) 
         != WlanScanError_Ok )
        { // Okay, it didn't exist.
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    // The ID is correct but also the OUI and OUI Type need to match
    WlanScanError ret( WlanScanError_Ok );

    while ( ret == WlanScanError_Ok && 
            ( ie != ie_id || 
              !( data[0] == ie_oui[0] && 
                 data[1] == ie_oui[1] &&
                 data[2] == ie_oui[2] 
               ) ||
              *( data + SCAN_OUI_TYPE_OFFSET ) != ie_oui_type
            ) 
          )
        {
        ret = NextIE( ie, len, &data );
        }

    // Check if the element is corrupted
    if ( ie != ie_id )
        {
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    ie_length = len;
    *ie_data = data;
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::InformationElement(
    u8_t ie_id,
    const WlanIeOui& ie_oui,
    u8_t ie_oui_type,
    u8_t ie_oui_subtype,
    u8_t& ie_length, 
    const u8_t** ie_data )
    {
    // Element format is:
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // | ID | Len | OUI |     |      |       |       |       |       |        |
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // where
    // ID     = Element ID (1 octet)
    // Len    = Length (1 octet)
    // OUI    = OUI (3 octets)
    // OUItyp = OUI Type (1 octet)

    u8_t ie( ie_id ); 
    u8_t len( 0 );
    const u8_t* data;

    // Find the first element
    if ( InformationElement( ie_id, len, &data ) 
         != WlanScanError_Ok )
        { // Okay, it didn't exist.
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    // The ID is correct but also the OUI and OUI Type need to match
    WlanScanError ret( WlanScanError_Ok );

    while ( ret == WlanScanError_Ok && 
            ( ie != ie_id || 
              !( data[0] == ie_oui[0] && 
                 data[1] == ie_oui[1] &&
                 data[2] == ie_oui[2] 
               ) ||
              *( data + SCAN_OUI_TYPE_OFFSET ) != ie_oui_type ||
              *( data + SCAN_OUI_SUBTYPE_OFFSET ) != ie_oui_subtype              
            ) 
          )
        {
        ret = NextIE( ie, len, &data );
        }

    // Check if the element is corrupted
    if ( ie != ie_id )
        {
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    ie_length = len;
    *ie_data = data;
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::FirstIE( 
    u8_t& ie_id, 
    u8_t& ie_length, 
    const u8_t** ie_data )
    {
    if( !current_m )
        {
        return WlanScanError_IeNotFound;
        }
    ie_iter_m = current_m + MGMT_BODY_OFFSET;
    return CurrentIE( ie_id, ie_length, ie_data );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::NextIE(
    u8_t& ie_id, 
    u8_t& ie_length, 
    const u8_t** ie_data )
    {
    if( !current_m )
        {
        return WlanScanError_IeNotFound;
        }
    ie_iter_m += *( ie_iter_m + 1 ) + 2;
    return CurrentIE( ie_id, ie_length, ie_data );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
WlanScanError ScanInfo::CurrentIE(
    u8_t& ie_id, 
    u8_t& ie_length, 
    const u8_t** ie_data ) const
    {
    if ( !current_m || ie_iter_m >= current_m + Size() )
        {
        ie_id = 0;
        ie_length = 0;
        *ie_data = NULL;
        return WlanScanError_IeNotFound;
        }

    ie_id = *ie_iter_m;
    ie_length = *( ie_iter_m+1 );
    *ie_data = ie_iter_m+2;
    return WlanScanError_Ok;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t ScanInfo::IsProtectedSetupSupported()
    {
    u8_t ie_length = 0;
    const u8_t* ie_data = NULL;
    const WlanIeOui ie_oui = { SCAN_WSC_OUI[0], SCAN_WSC_OUI[1], SCAN_WSC_OUI[2] };
    const u8_t ie_oui_type = SCAN_WSC_OUI[3];
    
    WlanScanError err = InformationElement( WPA_IE_ID,
                                            ie_oui, ie_oui_type,
                                            ie_length, &ie_data );
    
    if (err == WlanScanError_IeNotFound
        || ie_data == NULL 
        || ie_length == 0 )
        {
        return false_t;
        }
    
    // WSC IE is found, next check whether AP setup is locked (locked state would prevent use of Protected Setup)
    return !IsApSetupLocked( ie_data, ie_length );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
bool_t ScanInfo::IsApSetupLocked(
    const u8_t* ie_data,
    const u8_t ie_length ) const
    {
    u16_t index( SCAN_WSC_IE_PROTECTED_SETUP_DATA_OFFSET );
    
    while ( (index + SCAN_WSC_IE_DATA_LENGTH_OFFSET + 1) < ie_length)
        {
        // Make big endian to little endian conversion.
        u16_t attribute_type = (ie_data[index]<<8) 
                             | (ie_data[index+1]);
        
        u16_t attribute_data_length = (ie_data[index + SCAN_WSC_IE_DATA_LENGTH_OFFSET]<<8) 
                                    | (ie_data[index + SCAN_WSC_IE_DATA_LENGTH_OFFSET+1]);
        
        // Check if all attribute data does not fit current IE.
        if ( ( index + attribute_data_length + SCAN_WSC_IE_DATA_AREA_OFFSET ) > ie_length)
            {
            // attribute does not fit current IE
            return false_t;
            }
            
        if (attribute_type == SCAN_WSC_IE_AP_SETUP_LOCKED)
            {
            // AP setup locked attribute found, return its value.
            return ie_data[index + SCAN_WSC_IE_DATA_AREA_OFFSET];
            }
        
        // update index to start of next attribute
        index += SCAN_WSC_IE_DATA_AREA_OFFSET + attribute_data_length;
        }
    // 
    return false_t;
    }
