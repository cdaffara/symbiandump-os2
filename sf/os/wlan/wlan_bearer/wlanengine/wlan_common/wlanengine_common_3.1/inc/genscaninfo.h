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
* Description:  Class to dig information from scan results.
*
*/


#ifndef GENSCANINFO_H
#define GENSCANINFO_H


//  INCLUDES
#include "genscanlistiterator.h"

// CONSTANTS
/** Length of BSSID element. */
const u8_t BSSID_LENGTH = 6;
/** The bitmask for determining operating mode. */
const u16_t SCAN_CAPABILITY_BIT_MASK_ESS     = 0x0001;
/** The bitmask for privacy bit. */
const u16_t SCAN_CAPABILITY_BIT_MASK_PRIVACY = 0x0010;
/** Length of OUI element. */
const u8_t SCAN_IE_OUI_LENGTH = 3;
/** Length of TSF element. */
const u8_t TIMESTAMP_LENGTH = 8;

// LOCAL DATATYPES
/** The possible error codes returned by parsing methods. */
typedef enum _WlanScanError
    {
    WlanScanError_Ok,
    WlanScanError_IeNotFound
    } WlanScanError;

/** The possible security modes of a BSS. */
typedef enum _WlanSecurityMode
    {
    WlanSecurityModeOpen,
    WlanSecurityModeWep,
    WlanSecurityMode802_1x,
    WlanSecurityModeWpaEap,
    WlanSecurityModeWpaPsk,
    WlanSecurityModeWpa2Eap,
    WlanSecurityModeWpa2Psk,
    WlanSecurityModeWapi,
    WlanSecurityModeWapiPsk
    } WlanSecurityMode;

/** The possible operating modes of a BSS. */
typedef enum _WlanOperatingMode
    {
    WlanOperatingModeInfra,
    WlanOperatingModeAdhoc
    } WlanOperatingMode;

/** Type definition for OUI element. */
typedef u8_t WlanIeOui[SCAN_IE_OUI_LENGTH];

/**
 *  Wrapper class to parse data fields from scan info.
 */
NONSHARABLE_CLASS(  ScanInfo ) : public ScanListIterator
    {
public:  // Methods

    /**
     * C++ constructor.
     *
     * @param scan_list Reference to the scan list container.
     */
    ScanInfo( const ScanList& scan_list );

  // New methods
    
    /**
     * Return the measured signal noise ratio of the BSS.
     *
     * @return Signal noise ratio (SNR).
     * @deprecated
     */
    inline u8_t SignalNoiseRatio() const;

    /**
    * Return the measured Received Channel Power Indicator value of the BSS.
    *
    * @return RCPI value.
    */
    inline u8_t RXLevel() const;

    /**
     * Return BSSID of the BSS.
     *
     * @param bssid BSSID of the BSS.
     */
    inline void BSSID(
        u8_t bssid[BSSID_LENGTH] ) const;

    /**
     * Return the beacon interval of the BSS.
     *
     * @return The beacon interval in TUs.
     */
    inline u16_t BeaconInterval() const;

    /**
     * Return the capability information of the BSS.
     *
     * @return The capability information.
     */
    inline u16_t Capability() const;

    /**
     * Return whether the privacy bit is enabled in BSs capabilities.
     *
     * @return Whether the privacy is enabled.
     */
    inline bool_t Privacy() const;

    /**
     * Return the operating mode of the BSS.
     *
     * @return The operating mode of the BSS.
     */
    inline WlanOperatingMode OperatingMode() const;

    /**
     * Return the security mode of the BSS.
     * @return The security mode of the BSS.
     */
    WlanSecurityMode SecurityMode();

    /**
     * Return the timestamp (TSF) of the BSS.
     *
     * @param timestamp Timestamp of the BSS.
     */
    inline void Timestamp(
        u8_t timestamp[TIMESTAMP_LENGTH] ) const;

    /**
     * Return the requested information element.
     *
     * @param ie_id Element ID of the information element.
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError InformationElement(
        u8_t ie_id,
        u8_t& ie_length, 
        const u8_t** ie_data );

    /**
     * Return the WPA information element.
     *
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError WpaIE(
        u8_t& ie_length, 
        const u8_t** ie_data );

    /**
     * Return the first information element.
     *
     * @param ie_id Element ID of the information element.
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError FirstIE(
        u8_t& ie_id, 
        u8_t& ie_length, 
        const u8_t** ie_data );

    /**
     * Return next information element.
     *
     * @param ie_id Element ID of the information element.
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError NextIE(
        u8_t& ie_id, 
        u8_t& ie_length, 
        const u8_t** ie_data );                                       

    /**
     * Return the first information element with given OUI and OUI type.
     *
     * @param ie_id Element ID of the information element.
     * @param ie_oui OUI element of the requested IE.
     * @param ie_oui_type OUI type element of the requested IE.
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError InformationElement(
        u8_t ie_id,
        const WlanIeOui& ie_oui,
        u8_t ie_oui_type,
        u8_t& ie_length, 
        const u8_t** ie_data );

    /**
     * Return the first information element with given OUI, OUI type and OUI subtype.
     *
     * @param ie_id Element ID of the information element.
     * @param ie_oui OUI element of the requested IE.
     * @param ie_oui_type OUI type element of the requested IE.
     * @param ie_oui_subtype OUI subtype element of the requested IE.
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError InformationElement(
        u8_t ie_id,
        const WlanIeOui& ie_oui,
        u8_t ie_oui_type,
        u8_t ie_oui_subtype,
        u8_t& ie_length, 
        const u8_t** ie_data );                                       

    /**
     * Check whether Wi-Fi Protected Setup is supported.
     *
     * @return true_t if AP supports Wi-Fi Protected Setup,
     *         false_t otherwise.
     */
    bool_t IsProtectedSetupSupported();

private: //Methods

    /**
     * Prohibit copy constructor.
     */
    ScanInfo(
        const ScanInfo& );
    /**
     * Prohibit assigment operator.
     */
    ScanInfo& operator= (
        const ScanInfo& );

    /**
     * Return current information element.
     *
     * @param ie_id Element ID of the information element.
     * @param ie_length Length of the IE. Zero if IE not found.
     * @param ie_data Pointer to the beginning of the IE data. NULL if IE not found.
     * @return WlanScanError_Ok if IE was found, an error otherwise.
     */
    WlanScanError CurrentIE(
        u8_t& ie_id, 
        u8_t& ie_length, 
        const u8_t** ie_data ) const;

    /** 
     * Search through WSC IE and return value of AP setup locked attribute.
     * 
     * @param ie_data Pointer to the beginning of the IE data.
     * @param ie_length Length of the IE.
     * @return  true_t if value of AP setup locked is true.
     *          false_t otherwise.
     */
    bool_t IsApSetupLocked(
        const u8_t* ie_data,
        const u8_t ie_length ) const;

private:   // Data

    /** Iterator for going through IEs of scan info. */
    const u8_t* ie_iter_m;
    };

#include "genscaninfo.inl"

#endif      // GENSCANINFO_H
        
// End of File
