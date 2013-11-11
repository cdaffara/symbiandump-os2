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
* Description:  Declaration of the ScanInfoIe class.
*
*/


#ifndef GENSCANINFOIE_H
#define GENSCANINFOIE_H

#include "genscaninfo.h"

/** Defines the possible types of security information elements. */
enum ScanInfoIeType
    {
    ScanInfoIeTypeRsn,
    ScanInfoIeTypeWpa,
    ScanInfoIeTypeWapi
    };

/** Defines the possible types of key management suites. */
enum ScanInfoIeKeyManagement
    {
    ScanInfoIeKeyManagementEap,
    ScanInfoIeKeyManagementPsk,
    ScanInfoIeKeyManagementWapiCertificate,
    ScanInfoIeKeyManagementWapiPsk
    };

/**
* Class determining the security mode of an access point.
*/
NONSHARABLE_CLASS( ScanInfoIe )
    {
    public: // Methods

       // Constructors and destructor
        
        /**
         * C++ constructor.
         */
        ScanInfoIe();

        /**
         * Destructor.
         */
        virtual ~ScanInfoIe();

       // New methods

        /**
         * Parses the given scan information and returns the security
         * mode of the BSS.
         *
         * @param info Scan information to be parsed.
         * @return Security mode of the BSS.
         */
        WlanSecurityMode SecurityMode(
            ScanInfo& info ) const;

    private: // Methods

        /**
         * Checks whether the given memory buffers are equal.
         * 
         * @param pl Pointer to parameter1 data.
         * @param ll Length of parameter1 data.
         * @param pr Pointer to parameter2 data.
         * @param rl Length of parameter2 data.
         * @return 0 if data is same or similar
         *         !0 if data is not similar
         */
        int Compare(
            const unsigned char* pl,
            int ll, 
            const unsigned char* pr, 
            int rl ) const;

        /**
         * Checks whether the given information element defines
         * the expected key management suite.
         *
         * @param ie_type Type of of the information element.
         * @param key_type Expected key management suite.
         * @param ie_length Length of the IE.
         * @param ie_data Pointer to the beginning of the IE data.
         * @return true_t if expected key management suite is defined,
         *         false_t otherwise.
         */
        bool_t IsKeyManagement(
            ScanInfoIeType ie_type,
            ScanInfoIeKeyManagement key_type,
            u8_t ie_length,
            const u8_t* ie_data ) const;

        /**
         * Checks whether the given information element defines
         * valid ciphers suites for WPA2.
         *
         * @param ie_type Type of of the information element.
         * @param ie_length Length of the IE.
         * @param ie_data Pointer to the beginning of the IE data.
         * @return true_t if cipher suites are for WPA2, false_t otherwise.
         */
        bool_t IsWpa2Ciphers(
            ScanInfoIeType ie_type,
            u8_t ie_length,
            const u8_t* ie_data ) const;

        /**
         * Prohibit copy constructor.
         */
        ScanInfoIe(
            const ScanInfoIe& );
        /**
         * Prohibit assigment operator.
         */
        ScanInfoIe& operator= (
            const ScanInfoIe& );
    };

#endif // GENSCANINFOIE_H
