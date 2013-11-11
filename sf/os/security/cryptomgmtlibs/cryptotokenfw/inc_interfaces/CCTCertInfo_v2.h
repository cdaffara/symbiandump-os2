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
* CCTCertInfo.H
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __CCTCERTINFO_H__
#define __CCTCERTINFO_H__

#include <e32base.h>
#include <s32strm.h>

#include <securitydefs.h>
#include <ct/mcttokentype.h>
#include <ct/mcttoken.h>
#include <ct/mcttokenobject.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** Mask constants used for serializing iDeletable and iFormat attributes 
*/
const TUint KReadOnlyFlagMask = 128;
const TUint KFormatMask = 127;

/** The UID of a CertInfo MCTTokenObject. */
const TInt KCTObjectCertInfo = 0x101F50E6;

#endif

/** The maximum length of a certificate label. */
const TUint32 KMaxCertLabelLength = 64;

/** Defines a modifiable buffer descriptor to contain a human-readable certificate label. 
*
*/
typedef TBuf<KMaxCertLabelLength> TCertLabel;

/**
 * Mix-in class representnig data about a stored certificate.  Provides
 * implementation of serialization.
 * 
 * Note that for backward compatibility reasons, the issuer hash is not serialised.
 *
 */
class MCertInfo
	{
 public:
	// Internalization/Externalization
	// Externalize. Writes the data out to a stream
	
  /** Externalises an object of this class to a write stream.
	* 
	* The presence of this function means that the standard templated operator<<() 
	* can be used to externalise objects of this class.
	* 
	* @param aStream	Stream to which the object should be externalised. */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	// Internalize. Reads the data from a stream 
	/** Internalises an object of this class from a read stream.
	* 
	* The presence of this function means that the standard templated operator>>() 
	* can be used to internalise objects of this class.
	* 
	* Note that this function has assignment semantics: it replaces the old value 
	* of the object with a new value read from the read stream.	
	* 
	* @param aStream	Stream from which the object should be internalised. */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
 protected:
	IMPORT_C MCertInfo();
	IMPORT_C MCertInfo(const TDesC& aLabel,
					   TCertificateFormat aFormat,
					   TCertificateOwnerType aCertificateOwnerType, 
					   TInt aSize,
					   const TKeyIdentifier* aSubjectKeyId,
					   const TKeyIdentifier* aIssuerKeyId, 
					   TInt aCertificateId,
					   TBool aDeletable);
	IMPORT_C MCertInfo(const MCertInfo& aOther);
	IMPORT_C ~MCertInfo();

	IMPORT_C void ConstructL(const TDesC8* aIssuerHash);

	const TDesC8* IssuerHash() const;

 private:
	TBool Valid() const;
	const MCertInfo& operator=(const MCertInfo& aOther);

 protected:
	TCertLabel iLabel;
	TInt iCertificateId;
	TCertificateFormat iFormat;
	TCertificateOwnerType iCertificateOwnerType;
	TInt iSize;
	TKeyIdentifier iSubjectKeyId;
	TKeyIdentifier iIssuerKeyId;
	TBool iDeletable;

 private:
	HBufC8* iIssuerHash;
	};

/** Encapsulates information about a stored certificate.
 * 
 * Objects of this type are usually returned by a certificate store, to allow 
 * a client to query the contents of the store.
 * 
 * Note that these objects are normally constructed by certificate stores, not 
 * by clients. 
 *
 */
class CCTCertInfo : protected CBase, public MCTTokenObject, public MCertInfo
	{
public:
	/** Construction -- Note that these objects are normally constructed by certificate stores, not by clients. */

	/** 
	*
	* Creates the certificate information object by copying from an existing object.
	* 
	* @param aCertInfo			The source certificate information.
	* @return					A pointer to the new certificate information object.
	* @leave KErrNoMemory	There is no memory to construct it. */
	IMPORT_C static CCTCertInfo* NewL(const CCTCertInfo& aCertInfo);

	/** 
	*
	* Creates the certificate information object by copying from an existing object, 
	* and puts a pointer to the new object onto the cleanup stack.
	* 
	* @param aCertInfo			The source certificate information.
	* @return					A pointer to the new certificate information object.
	* @leave KErrNoMemory	There is no memory to construct it. */
	IMPORT_C static CCTCertInfo* NewLC(const CCTCertInfo& aCertInfo);

	/** 
	*
	* Creates the certificate information object from its constituent parts.
	* 
	* @param aLabel					The certificate's label.
	* @param aFormat				The certificate's format.
	* @param aCertificateOwnerType	The owner type.
	* @param aSize					The size of the certificate.
	* @param aSubjectKeyId			The subject key ID.
	* @param aIssuerKeyId			The issuer key ID.
	* @param aToken					The token that the certificate is within.
	* @param aCertificateId			The ID within the object handle.
	* @param aDeletable				The certificate is deletable.
	* @param aIssuerHash			The hash of the DN of the issuer.
	* @return						A pointer to the new certificate information object. 
	* @leave KErrNoMemory		There is no memory to construct it.*/
	IMPORT_C static CCTCertInfo* NewL(const TDesC& aLabel, TCertificateFormat aFormat,
		TCertificateOwnerType aCertificateOwnerType, TInt aSize,
		const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
		MCTToken& aToken, TInt aCertificateId, TBool aDeletable, 
									  const TDesC8* aIssuerHash = NULL);

	/** 
	*
	* Creates the certificate information object from its constituent parts.	
	* 
	* @param aLabel					The certificate's label.
	* @param aFormat 				The certificate's format.
	* @param aCertificateOwnerType	The owner type.
	* @param aSize					The size of the certificate.
	* @param aSubjectKeyId			The subject key ID.
	* @param aIssuerKeyId			The issuer key ID.
	* @param aToken					The token that the certificate is within.
	* @param aCertificateId			The ID within the object handle.
	* @return 						A pointer to the new certificate information object.
	* @leave KErrNoMemory		There is no memory to construct it.*/
	IMPORT_C static CCTCertInfo* NewL(const TDesC& aLabel, TCertificateFormat aFormat,
		TCertificateOwnerType aCertificateOwnerType, TInt aSize,
		const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
		MCTToken& aToken, TInt aCertificateId);

	/** 
	* 
	* Creates the certificate information object from its constituent parts, 
	* and puts a pointer to the new object onto the cleanup stack.
	* 
	* @param aLabel					The certificate's label.
	* @param aFormat				The certificate's format.
	* @param aCertificateOwnerType	The owner type.
	* @param aSize					The size of the certificate.
	* @param aSubjectKeyId			The subject key ID.
	* @param aIssuerKeyId			The issuer key ID.
	* @param aToken					The token that the certificate is within.
	* @param aCertificateId			The ID within the object handle.
	* @param aDeletable				The certificate is deletable.
	* @param aIssuerHash			The hash of the issuer's DN.
	* @return						A pointer to the new certificate information object. 
	* @leave KErrNoMemory		There is no memory to construct it.*/
	IMPORT_C static CCTCertInfo* NewLC(const TDesC& aLabel, TCertificateFormat aFormat,
		TCertificateOwnerType aCertificateOwnerType, TInt aSize,
		const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
		MCTToken& aToken, TInt aCertificateId, TBool aDeletable, 
									   const TDesC8* aIssuerHash = NULL);

	/**
	*
	* Creates the certificate information object from its constituent parts, and puts 
	* a pointer to the new object onto the cleanup stack.
	* 
	* @param aLabel					The certificate's label.
	* @param aFormat				The certificate's format.
	* @param aCertificateOwnerType	The owner type.
	* @param aSize					The size of the certificate.
	* @param aSubjectKeyId			The subject key ID.
	* @param aIssuerKeyId			The issuer key ID.
	* @param aToken					The token that the certificate is within.
	* @param aCertificateId			The ID within the object handle.
	* @return						A pointer to the new certificate information object.
	* @leave KErrNoMemory		There is no memory to construct it.*/
	IMPORT_C static CCTCertInfo* NewLC(const TDesC& aLabel, TCertificateFormat aFormat,
		TCertificateOwnerType aCertificateOwnerType, TInt aSize,
		const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
		MCTToken& aToken, TInt aCertificateId);

	/** 
	* 
	* Creates the certificate information object by internalising a previously externalised 
	* one.
	* 
	* @param aStream			The stream from which the object is to be internalised.
	* @param aToken				The token that it is within.
	* @return					A pointer to the new certificate information object. 
	* @leave KErrNoMemory	There is no memory to construct it.
	* @see ExternalizeL
	* @see InternalizeL */
	IMPORT_C static CCTCertInfo* NewL(RReadStream& aStream, MCTToken& aToken);

	/** 
	* 
	* Creates the certificate information object, by internalising a previously externalised 
	* one, and puts a pointer to the new object onto the cleanup stack.	
	* 
	* @param aStream			The stream from which the object is to be internalised.
	* @param aToken				The token that it is within.
	* @return					A pointer to the new certificate information object. 
	* @leave KErrNoMemory	There is no memory to construct it.
	* @see ExternalizeL
	* @see InternalizeL */
	IMPORT_C static CCTCertInfo* NewLC(RReadStream& aStream, MCTToken& aToken);

	/** Gets the subject key ID.	
	* 
	* @return A reference to a key identifier object. */
	IMPORT_C const TKeyIdentifier& SubjectKeyId() const;

	/** Gets the issuer key ID.
	* 
	* @return A reference to a key identifier object. */
	IMPORT_C const TKeyIdentifier& IssuerKeyId() const;

	/** Gets the certificate format.
	* 
	* @return The certificate format. */
	IMPORT_C TCertificateFormat CertificateFormat() const;

	/** Gets the owner type.
	* 
	* @return The owner type. */
	IMPORT_C TCertificateOwnerType CertificateOwnerType() const;

	/** Gets the size of the certificate.
	* 
	* Note that this function must be called so that the correct size of descriptor 
	* can be allocated when retrieving the certificate (with MCertStore::Retrieve()).
	* 
	* @return The size of the certificate. */
	IMPORT_C TInt Size() const;

	/** Gets a handle for the object.
	* 
	* The primary purpose of the handle is to allow token objects to be 'passed' 
	* between processes.
	* 
	* @return	A handle for the object. 
	* @see TCTTokenObjectHandle */
	IMPORT_C TCTTokenObjectHandle Handle() const;

	/** Whether the certificate is deletable.
	* 
	* @return	ETrue if it is possible to delete the certificate; EFalse, otherwise. */
	IMPORT_C TBool IsDeletable() const;

	/** Gets the hash of the issuer's DN.
	* 
	* @return	The hash of the issuer's DN, or NULL if not known. */
	IMPORT_C const TDesC8* IssuerHash() const;

 public:	
  // from MCTTokenObject
	/** Gets the object's human-readable label.
	* 
	* @return	The object's human-readable label.
	* @see MCTTokenObject::Label()*/
	virtual const TDesC& Label() const;

	/** Gets a reference to the associated token.
	* 
	* @return	A reference to the associated token.
	* @see MCTTokenObject::Token()*/
	virtual MCTToken& Token() const;

	/** Gets a UID representing the type of the token object.
	* 
	* The function overrides MCTTokenObject::Type().
	* 
	* The meanings of possible UIDs should be documented in the documentation for 
	* the interface that returns them.
	* 
	* @return	A UID representing the type of the token object; this implementation 
	* 			returns KCTObjectCertInfo.
	* @see MCTTokenObject::Type() */
	virtual TUid Type() const;
	
	// Compares 2 cert infos
	/** Compares this certificate information object with a specified Certificate 
	* Information object for equality.
	* 
	* @param aCertInfo	The certificate information object to be compared.
	* @return			ETrue, if they are the same; EFalse, otherwise. */
	IMPORT_C TBool operator ==(const CCTCertInfo& aCertInfo) const;

	/** Sets the certificate Id.
	* 
	* @param aCertId The certificate Id.*/
	IMPORT_C void SetCertificateId(TInt aCertId);

 protected:
	IMPORT_C virtual ~CCTCertInfo();
	
 private:
	CCTCertInfo(MCTToken& aToken);
	CCTCertInfo(const TDesC& aLabel, TCertificateFormat aFormat,
				TCertificateOwnerType aCertificateOwnerType, 
				TInt aSize,
				const TKeyIdentifier* aSubjectKeyId,
				const TKeyIdentifier* aIssuerKeyId,
				MCTToken& aToken, TInt aCertificateId,
				TBool aDeletable);
	CCTCertInfo(const CCTCertInfo& aCertInfo);
	void ConstructL(RReadStream& aStream);
	void ConstructL(const TDesC8* aIssuerHash);
	
 private:
	const CCTCertInfo& operator=(const CCTCertInfo& aOther);
	
 private:	
	MCTToken& iToken;
	};

#endif
