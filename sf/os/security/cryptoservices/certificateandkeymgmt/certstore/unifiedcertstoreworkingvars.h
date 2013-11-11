/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CUnifiedCertStoreWorkingVars class implementation
*
*/


/**
 @file 
 @internalComponent 
*/
 
#include <e32base.h>
#include <mctwritablecertstore.h>
class CSHA1;
class CCertificate;
class CX500DistinguishedName;

NONSHARABLE_CLASS(CUnifiedCertStoreWorkingVars) : public CBase
	{
public:
	CUnifiedCertStoreWorkingVars();
	virtual ~CUnifiedCertStoreWorkingVars();

public:
	/**
	 * This stores the list of Token Types that support the
	 * writable cert store interface.
	 * Used while initializing the store.
	 */
	RCPointerArray<CCTTokenTypeInfo> iWritableTokenTypes;
	/**
	 * This stores the list of Token Types that support the
	 * readable cert store interface.
	 * Used while initializing the store.
	 */
	RCPointerArray<CCTTokenTypeInfo> iReadOnlyTokenTypes;

	/**
	 * Used to point to the array given as argument when listing certificates.
	 */
	RMPointerArray<CCTCertInfo>* iCertInfos;
	/**
	 * Used to point to the filter given as argument when listing certificates.
	 */
	const CCertAttributeFilter* iFilter;

	/**
	 * Used to point to the array of applications given as argument when doing 
	 * the following operations :
	 * - Applications
	 * - SetApplicability
	 */
	RArray<TUid>* iApplications;

	/**
	 * Used to point to the certificate info given as argument when doing the following
	 * operations :
	 * - Applications
	 * - IsApplicable
	 * - Trusted
	 * - Retrieve
	 * - Remove
	 * - SetApplicability
	 * - SetTrust
	 */
	const CCTCertInfo* iCertInfo;

	/**
	 * This is used as an index for 
	 * - the iWritableTokenTypes array when we are initializing the store.
	 * - the iReadOnlyTokenTypes array when we are initializing the store.
	 */
	TInt iIndex;

	/** The first returned certificate to be added to the array during
	 *  a list, or the cert whose issuer DN is currently being compared. */
	TInt iCertIndex;

	/** The required issuer name for a list. */
	RPointerArray<TDesC8> iIssuerNames;
	/** The issuer names in parsed format */
	RPointerArray<CX500DistinguishedName> iParsedIssuerNames;
	/** Hashes of issuer names */
	RPointerArray<HBufC8> iHashedIssuerNames;

	/** A hash of the issuer DN. Used for comparing DNs in URL certificates */
	CSHA1* iIssuerHash;

	/** A descriptor used for storing certs while returning them as 
		CCertificate objects */
	HBufC8* iCertDesC;
	/** A pointer to the modifiable descriptor for returning the 
	    CCertificate objects - normally points to to iCertDesC */
	TPtr8   iCertDes;  
	
	/** Pointer to returned certificate pointer */
	CCertificate** iReturnedCert;

	/** Type of the certificate being retrieved */
	TCertificateFormat iCertType;
	};
