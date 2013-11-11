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
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CCERTATTRIBUTEFILTER_H__
#define __CCERTATTRIBUTEFILTER_H__

#include <e32base.h>
#include <s32strm.h>
#include <securitydefs.h>
#include <cctcertinfo.h>
#include <ct.h>

/** 
 * A filter for specifying which certificates are required. 
 *
 * On construction it will accept everything. 
 * The Set... functions can then be used to restrict it.
 */
class CCertAttributeFilter : public CBase
	{
public:
	/**
	 * Returns a default filter. This filter accepts any
	 * certificate. Call the Set... functions to restrict what it accepts.
	 *
	 * @return	A pointer to a new CCertAttributeFilter object.
	 */
	IMPORT_C static CCertAttributeFilter* NewL();
	/**
	 * Returns a default filter. This filter accepts any certificate.
	 * It leaves the filter on the cleanup stack.
	 *
	 * @return	A pointer to a new CCertAttributeFilter object.
	 */
	IMPORT_C static CCertAttributeFilter* NewLC();

	/**
	 * Read a certificate attribute filter from a stream.
	 */
	IMPORT_C static CCertAttributeFilter* NewL(RReadStream& aStream);
	/**
	 * Write a certificate attribute filter to a stream.
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
public:
	// Setter methods
	/** 
	 * Sets the label of the cert that the filter will accept.
	 *	
	 *	@param aLabel	The new label 
	 */
	IMPORT_C void SetLabel(const TCertLabel& aLabel);
	
	/** 
	 * Sets the UID of an application which the certificate must support
	 *
	 * @param aUid	The new UID
	 */
	IMPORT_C void SetUid(TUid aUid);
	
	/** 
	 * Sets the format of the returned certificates
	 *
	 * @param aFormat	The required format
	 */
	IMPORT_C void SetFormat(TCertificateFormat aFormat);
	
	/** 
	 * Sets the owner type of the returned certificates
	 *
	 * @param aOwnerType	The owner type.
	 */
	IMPORT_C void SetOwnerType(TCertificateOwnerType aOwnerType);
	
	/** 
	 * Sets the usage that returned certs must be used for.  
	 * 
	 * Note that the usage is supplied using X509 scheme - 
	 * keystore interfaces use the PKCS#15 scheme!
	 *
	 * @param aKeyUsage	The key usage
	 */
	IMPORT_C void SetKeyUsage(TKeyUsageX509 aKeyUsage);
	
	/** 
	 * Sets the subject key ID that returned certificates must have.
	 *
	 * @param aSubjectKeyId	The subject key ID
	 */
	IMPORT_C void SetSubjectKeyId(const TKeyIdentifier& aSubjectKeyId);
	
	/** 
	 * Sets the issuer key ID that returned certificates must have.
	 * 
	 * @param aIssuerKeyId	The issuer key ID
	 */
	IMPORT_C void SetIssuerKeyId(const TKeyIdentifier& aIssuerKeyId);

private:
	CCertAttributeFilter();
	void InternalizeL(RReadStream& aStream);
	
public:
	/** The filter data. Each data item is accompanied by a
	   '..IsSet' boolean which indicates if the item should be used.
	   Unset values (whose ..IsSet variable is EFalse) should be ignored.
	*/
		
	/** The label that returned certificates must have. */
	TCertLabel iLabel;
	/** Indicates that iLabel should be used. */
	TBool iLabelIsSet;
	/** The application UID that all returned certificates must have. */
	TUid iUid;
	/** Indicates that iUid should be used. */
	TBool iUidIsSet;
	/** The format of certificates that should be included. */
	TCertificateFormat iFormat;
	/** Indicates that iFormat should be used. */
	TBool iFormatIsSet;
	/** The owner type of returned certificates. */
	TCertificateOwnerType iOwnerType;
	/** Indicates that iOwnerType should be used. */
	TBool iOwnerTypeIsSet;
	/** The usage of keys that should be returned. */
	TKeyUsageX509 iKeyUsage;
	/** The subject key that returned certs should have. */
	TKeyIdentifier iSubjectKeyId;
	/** Indicates that iSubjectKeyId should be used. */
	TBool iSubjectKeyIdIsSet;
	/** The issuer key that returned certs should have. */
	TKeyIdentifier iIssuerKeyId;
	/** Indicates that iIssuerKeyId should be used. */
	TBool iIssuerKeyIdIsSet;
	
	};

#endif
