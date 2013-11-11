/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __FSDATATYPES_H__
#define __FSDATATYPES_H__

// move header files to separate inc dirs for each subcomponent, can split
// into cedar and beech versions

/**
 * @file
 * @internalComponent
 * 
 * Internal data types used by the filetokens server.
 */

#include <mctkeystore.h>
#include <cctcertinfo.h>
#include <bigint.h>

#ifdef SYMBIAN_AUTH_SERVER
#include "authserver/authtypes.h"
#endif // SYMBIAN_AUTH_SERVER


/**
 * Server-side key info.
 * 
 * This is the server-side counterpart to the CCTKeyInfo class, containing all
 * the key attributes.
 */
class CKeyInfo : public CKeyInfoBase
	{
public:

	/** Creates a new CKeyInfo from a stream. */
	IMPORT_C static CKeyInfo* NewL(RReadStream& aStream);

	/** Make destructor public again. */
	inline ~CKeyInfo();

	/**
	 * Push object onto the cleanup stack (pointer won't convert to CBase* due
	 * to protected inheritance.
	 */
	inline void CleanupPushL();

public:

	/** Get the handle of the key. */
	inline TInt Handle() const;

	/** Set the key access type.  */
	inline void SetAccessType(TInt aAccessType);

	/** Set the security policy for key use operations. */
	IMPORT_C void SetUsePolicy(const TSecurityPolicy& aPolicy);

	/** Set the security policy for key management operations. */
	IMPORT_C void SetManagementPolicy(const TSecurityPolicy& aPolicy);

	/** Sets the set of DER encoded PKCS8 attributes. */
	IMPORT_C void SetPKCS8AttributeSet(HBufC8* aPKCS8AttributeSet);
	
#ifdef SYMBIAN_AUTH_SERVER
	inline AuthServer::TIdentityId Identity() const;
	inline const TDesC& AuthExpression() const;
	inline TInt Freshness() const;
	inline void SetIdentity(AuthServer::TIdentityId aIdentityId);
	// Will set the authexpresssion aasociated with this key. The
	// ownership is not transferred to this object.
	inline void SetAuthExpressionL(const TDesC& aAuthExpression);
	inline void SetFreshness(TInt aFreshness);
	inline void ResetAuthExpression();
#endif // SYMBIAN_AUTH_SERVER
	
private:
	
	inline CKeyInfo();

#ifdef SYMBIAN_AUTH_SERVER
private:
	AuthServer::TIdentityId iIdentityId;
	HBufC* iAuthExpression;
	TInt iFreshness;
#endif // SYMBIAN_AUTH_SERVER

	
	};

/**
 * An internal abstraction for Diffie-Hellman parmeters.
 */
class CDHParams : public CBase
	{
public:
	IMPORT_C static CDHParams* NewL(const TInteger& aN, const TInteger& aG);
	IMPORT_C ~CDHParams();
	inline const TInteger& N() const;
	inline const TInteger& G() const;
	IMPORT_C RInteger TakeN();			///< Return N and release ownership
	IMPORT_C RInteger TakeG();			///< Return G and release ownership
private:
	IMPORT_C CDHParams(const RInteger aN, const RInteger aG);
	RInteger iN;
	RInteger iG;
	};

/** A common interface for arrays of key infos.  */
class MKeyInfoArray
	{
 public:
	virtual TInt Count() = 0;
	virtual CCTKeyInfo* operator[](TInt aIndex) = 0;
	virtual TInt Append(CCTKeyInfo* aInfo) = 0;
	virtual void Close() = 0;
	};

/**
 * Information sent back to client when a key has been created or imported.
 */
struct TKeyUpdate
	{
	TInt iReference;
	TKeyIdentifier iId;
	TUint iSize;
	CKeyInfoBase::EKeyAlgorithm iAlgorithm;
	};

/**
 * Server-side cert info.
 * 
 * This is the server-side counterpart to the CCTCertInfo class, containing all
 * the certificate attributes.
 *
 * We don't support the issuer hash, as it is not
 * stored.
 *
 * 
 */

class CCertInfo : public CBase, public MCertInfo
	{
public:
	/** Copy a CCertInfo. */
	IMPORT_C static CCertInfo* NewL(const CCertInfo& aOther);

	/** Copy a CCertInfo. */
	IMPORT_C static CCertInfo* NewLC(const CCertInfo& aOther);

	/** Makes a new CCTCertInfo from its constituent parts. */
	IMPORT_C static CCertInfo* NewL(const TDesC& aLabel,
									TCertificateFormat aFormat,
									TCertificateOwnerType aCertificateOwnerType,
									TInt aSize,
									const TKeyIdentifier* aSubjectKeyId,
									const TKeyIdentifier* aIssuerKeyId,
									TInt aCertificateId);

	IMPORT_C static CCertInfo* NewL(const TDesC& aLabel,
									TCertificateFormat aFormat,
									TCertificateOwnerType aCertificateOwnerType,
									TInt aSize,
									const TKeyIdentifier* aSubjectKeyId,
									const TKeyIdentifier* aIssuerKeyId,
									TInt aCertificateId,
									const TBool aDeletable);

	/** Makes a new CCTCertInfo from its constituent parts. */
	IMPORT_C static CCertInfo* NewLC(const TDesC& aLabel,
									 TCertificateFormat aFormat,
									 TCertificateOwnerType aCertificateOwnerType,
									 TInt aSize,
									 const TKeyIdentifier* aSubjectKeyId,
									 const TKeyIdentifier* aIssuerKeyId,
									 TInt aCertificateId);

	IMPORT_C static CCertInfo* NewLC(const TDesC& aLabel,
									 TCertificateFormat aFormat,
									 TCertificateOwnerType aCertificateOwnerType,
									 TInt aSize,
									 const TKeyIdentifier* aSubjectKeyId,
									 const TKeyIdentifier* aIssuerKeyId,
									 TInt aCertificateId,
									 const TBool aDeletable);

	/** Reads a CCertInfo from a stream. */
	IMPORT_C static CCertInfo* NewL(RReadStream& aStream);

	/** Reads a CCertInfo from a stream. */
	IMPORT_C static CCertInfo* NewLC(RReadStream& aStream);

	// Accessors

	inline const TDesC& Label() const;
	inline TInt Reference() const;
	inline const TKeyIdentifier& SubjectKeyId() const;
	inline const TKeyIdentifier& IssuerKeyId() const;
	inline TCertificateFormat CertificateFormat() const;
	inline TCertificateOwnerType CertificateOwnerType() const;
	inline TInt Size() const;
	inline TInt CertificateId() const;
	inline TBool IsDeletable() const;

 private:
	/** Default constructor. */
	CCertInfo();		

	/** Copy constructor. */
	CCertInfo(const CCertInfo& aOther);

	CCertInfo(const TDesC& aLabel,
			  TCertificateFormat aFormat,
			  TCertificateOwnerType aCertificateOwnerType, 
			  TInt aSize,
			  const TKeyIdentifier* aSubjectKeyId,
			  const TKeyIdentifier* aIssuerKeyId,
			  TInt aCertificateId,
			  const TBool aDeletable = ETrue);	
	};

struct TAddCertDataStruct
	{
	TCertLabel iLabel;
	TCertificateFormat iFormat;
	TCertificateOwnerType iCertificateOwnerType;
	TKeyIdentifier iSubjectKeyId;
	TKeyIdentifier iIssuerKeyId;
	TBool iDeletable;
	};

#include "fsdatatypes.inl"

#endif
