/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
 
#ifndef __SECURITYDEFS_H__
#define __SECURITYDEFS_H__

#include <e32std.h>
#include <e32base.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <securitydefsconst.h>
#endif

/** General Security Definitions */

// Old keystore interface, deprecated //////////////////////////////////////////

/**
 * What a key can be used for.
 * The values this can take are defined in TKeyUsageVals.
 *
 * @deprecated
 */
typedef TInt TKeyUsage;

/**
 * What a key can be used for.
 *
 * These values can be ORed together if a key has several usages. EAllKeyUsages
 * is used when searching for all keys, rather than ones with a particular
 * usage.  As these can be combined, TKeyUsage is used to store them.
 *
 * @deprecated
 */
enum TKeyUsageVals
	{
    EDigitalSignature =  0x80000000,
    ENonRepudiation =  0x40000000,
    EKeyEncipherment =  0x20000000,
    EDataEncipherment =  0x10000000,
    EKeyAgreement =   0x08000000,
    EKeyCertSign =   0x04000000,
    ECRLSign =    0x02000000,
    EEncipherOnly =   0x01000000,
    EDecipherOnly =   0x00800000,
    EAllKeyUsages =   -1
	};

// End of deprecated keystore API //////////////////////////////////////////////

/**
 * What a key can be used for - PKCS#15 scheme.
 *
 */
enum TKeyUsagePKCS15
	{
	EPKCS15UsageEncrypt				= 0x001,
	EPKCS15UsageDecrypt				= 0x002,
	EPKCS15UsageSign				= 0x004,
	EPKCS15UsageSignRecover			= 0x008,
	EPKCS15UsageWrap				= 0x010,
	EPKCS15UsageUnwrap				= 0x020,	
	EPKCS15UsageVerify				= 0x040,
	EPKCS15UsageVerifyRecover		= 0x080,
	EPKCS15UsageDerive				= 0x100,
	EPKCS15UsageNonRepudiation		= 0x200,
	// Common combinations
	EPKCS15UsageEncryptWrap         = 0x011,
	EPKCS15UsageVerifyVerifyRecover = 0x0C0,
	EPKCS15UsageDecryptUnwrap       = 0x022,
	EPKCS15UsageSignSignRecover     = 0x00C,
	EPKCS15UsageVerifyEncrypt       = 0x0D1,
	EPKCS15UsageSignDecrypt         = 0x02E,
	// For use in filters to return all keys
	EPKCS15UsageAll					= 0xffffffff,
	EPKCS15UsageNone				= 0x00000000
	};

inline TKeyUsagePKCS15 operator|(TKeyUsagePKCS15 aLeft, TKeyUsagePKCS15 aRight);
inline TKeyUsagePKCS15 operator&(TKeyUsagePKCS15 aLeft, TKeyUsagePKCS15 aRight);
inline const TKeyUsagePKCS15& operator|=(TKeyUsagePKCS15& aLeft, TKeyUsagePKCS15 aRight);
inline const TKeyUsagePKCS15& operator&=(TKeyUsagePKCS15& aLeft, TKeyUsagePKCS15 aRight);

/**
 * What a key can be used for - X.509 scheme.
 *
 */
enum TKeyUsageX509
	{
	EX509UsageDigitalSignature		= 0x80000000,
	EX509UsageNonRepudiation		= 0x40000000,
	EX509UsageKeyEncipherment		= 0x20000000,
	EX509UsageDataEncipherment		= 0x10000000,
	EX509UsageKeyAgreement			= 0x08000000,
	EX509UsageKeyCertSign			= 0x04000000,
	EX509UsageCRLSign				= 0x02000000,
	EX509UsageEncipherOnly			= 0x01000000,
	EX509UsageDecipherOnly			= 0x00800000,
	// Values for commonly permitted combinations
	EX509UsageAnySign               = 0x86000000,
	EX509UsageAllEncipher           = 0x30000000,
	EX509UsageAllSignEncipher       = 0xB6000000,
	/// For use in filters to return all keys
	EX509UsageAll					= 0xffffffff,
	EX509UsageNone					= 0x00000000
	};

inline TKeyUsageX509 operator|(TKeyUsageX509 aLeft, TKeyUsageX509 aRight);
inline TKeyUsageX509 operator&(TKeyUsageX509 aLeft, TKeyUsageX509 aRight);
inline const TKeyUsageX509& operator|=(TKeyUsageX509& aLeft, TKeyUsageX509 aRight);
inline const TKeyUsageX509& operator&=(TKeyUsageX509& aLeft, TKeyUsageX509 aRight);

/** 
 *
 *
 * @param aUsage
 * @return
 */
IMPORT_C TKeyUsageX509 KeyUsagePKCS15ToX509(TKeyUsagePKCS15 aUsage);

/** 
 *
 *
 * @param aUsage
 * @return
 */
IMPORT_C TKeyUsagePKCS15 KeyUsageX509ToPKCS15Private(TKeyUsageX509 aUsage);

/** 
 *
 *
 * @param aUsage
 * @return
 */
IMPORT_C TKeyUsagePKCS15 KeyUsageX509ToPKCS15Public(TKeyUsageX509 aUsage);

/**
 * Supported types of certificate format. Note these must be only 1 byte long as
 * the file cert store only seralises them as 1 byte.
 * 
 */
enum TCertificateFormat
	{
    EX509Certificate    = 0x00,
    EWTLSCertificate    = 0x01,
	EX968Certificate    = 0x02,
	EUnknownCertificate = 0x0f,
	EX509CertificateUrl = 0x10,
	EWTLSCertificateUrl = 0x11,
	EX968CertificateUrl = 0x12
	};

/**
 * The owner of a certificate.
 * 
 */
enum TCertificateOwnerType
	{
	ECACertificate,
	EUserCertificate,
	EPeerCertificate
	};

/** The length of a SHA-1 hash 
 * 
 */
const TInt KSHA1HashLengthBytes = 20;

/**
 * A SHA-1 hash.
 * 
 */
typedef  TBuf8<KSHA1HashLengthBytes> TSHA1Hash;

//const TInt KMD5HashLengthBytes = 16;
//typedef TMD5Hash TBufC8<KMD5HashLengthBytes>;

/**
 * A SHA-1 hash is also used as a key identifier.
 * 
 */
typedef TSHA1Hash TKeyIdentifier;

/**
 * Errors that can occur when validating a certificate chain.
 * 
 * Except EValidatedOK, all these are fatal errors unless specified.
 *
 */
enum TValidationError
	{
	/** Validation OK */
	EValidatedOK,
	/** Certificate chain has no root */
	EChainHasNoRoot,
	/** Invalid signature  */
	ESignatureInvalid,
	/** Date out of range */
	EDateOutOfRange,
	/** Name is excluded */
	ENameIsExcluded,
	/** Name is not permitted */
	ENameNotPermitted,	//subtle difference here!
	/** Not a CA certificate */
	ENotCACert,
	/** Certificate revoked */
	ECertificateRevoked,
	/** Unrecognized critical extension */
	EUnrecognizedCriticalExtension,
	/** No basic constraint in CA certificate */
	ENoBasicConstraintInCACert,
	/** No acceptable policy */
	ENoAcceptablePolicy,
	/** Path too long */
	EPathTooLong,
	/** Negative path length specified */
	ENegativePathLengthSpecified,
	/** Names do not chain */
	ENamesDontChain,
	/** Required policy not found */
	ERequiredPolicyNotFound,
	/** Bad key usage */
	EBadKeyUsage,
	/** 
	 * Root certificate not self-signed.
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ERootCertNotSelfSigned,
	/** 
	 * Critical extended key usage
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalExtendedKeyUsage,
	/** 
	 * Critical certificate policies with qualifiers
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalCertPoliciesWithQualifiers,
	/** 
	 * Critical policy mapping
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalPolicyMapping,
	/** 
	 * Critical Device Id
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalDeviceId,
	/** 
	 * Critical Sid
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalSid,
	/** 
	 * Critical Vid
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalVid,
	/** 
	 * Critical Capabilities
	 * 
	 * We cannot tell if this is fatal or not, as we lack the context.
	 */
	ECriticalCapabilities
	};


#include "securitydefs.inl"

#endif
