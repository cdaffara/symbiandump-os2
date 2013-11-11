/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* General Security Definitions
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef SECURITYDEFSCONST_H
#define SECURITYDEFSCONST_H

// Certificate Applicability UIDs

/**
 * This UID is associated with certificates which are trusted for 
 * software installation of native applications. 
 *
 * @see MCertStore::Applications
 * @see MCTWritableCertStore::SetApplicability
 *
 */
const TUid KSwiApplicabilityUid = {0x100042AB};

/**
 * This UID is associated with certificates which are trusted for 
 * OCSP checks.
 *
 * @see MCertStore::Applications
 * @see MCTWritableCertStore::SetApplicability
 *
 */
const TUid KSwiOcspApplicabilityUid = {0x1000A8B6};

/**
 * This UID is associated with certificates which are trusted for 
 * Java midlet installation.
 *
 * @see MCertStore::Applications
 * @see MCTWritableCertStore::SetApplicability
 *
 */
const TUid KMidletInstallApplicabilityUid = {0x101F9B28};

/**
 * This UID is associated with certificates which are trusted for 
 * SSL/TLS connectivity.
 *
 * @see MCertStore::Applications
 * @see MCTWritableCertStore::SetApplicability
 *
 */
const TUid KTlsApplicabilityUid = {0x1000183D};

/**
 * This OID is associated with X.509 certificates
 * trusted for TLS WWW server authentication.
 *
 */
_LIT(KServerAuthOID,"1.3.6.1.5.5.7.3.1");

/**
 * This OID is associated with X.509 certificates
 * trusted for TLS WWW client authentication.
 *
 */
 // SSL Client
 _LIT(KClientAuthOID,"1.3.6.1.5.5.7.3.2");

/**
 * This OID is associated with X.509 certificates
 * trusted for signing of downloadable executable code.
 *
 */
_LIT(KCodeSigningOID,"1.3.6.1.5.5.7.3.3");

/**
 * This OID is associated with X.509 certificates
 * trusted for email protection .
 *
 */
_LIT(KEmailProtectionOID,"1.3.6.1.5.5.7.3.4");

/**
 * This OID is associated with X.509 certificates
 * trusted for Ipsec end system.
 *
 */
_LIT(KIpsecEndSystemOID,"1.3.6.1.5.5.7.3.5");

/**
 * This OID is associated with X.509 certificates
 * trusted for Ipsec tunnel.
 *
 */
_LIT(KIpsecTunnelOID,"1.3.6.1.5.5.7.3.6");

/**
 * This OID is associated with X.509 certificates
 * trusted for Ipsec user.
 *
 */
_LIT(KIpsecUserOID, "1.3.6.1.5.5.7.3.7");

/**
 * This OID is associated with X.509 certificates
 * trusted for binding the hash of an object to a time.
 *
 */
_LIT(KTimeStampingOID,"1.3.6.1.5.5.7.3.8");

/**
 * This OID is associated with X.509 certificates
 * trusted for signing OCSP responses.
 *
 */
_LIT(KOCSPSigningOID,"1.3.6.1.5.5.7.3.9");


#endif
