/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/
#ifndef __PKCS12_MACDATA_H__
#define __PKCS12_MACDATA_H__

#include <e32base.h>
#include <asn1dec.h>
#include <hash.h>
#include <pkcs12kdf.h>
#include <pkcs7digestinfo.h>

namespace PKCS12
{
/** Default Iteration Count */
const TInt KDefaultIterationCount = 1;

/** SHA-1 HMAC 160 bit Key length for key derivation. */
const TInt KSha1HmacKeyLength = 20;

/**
 This class decodes the MacData
 It has methods to return the PKCS#12 MACDATA structure.
 */
class CDecPkcs12MacData : public CBase
	{
public:
    /**
	 Creates a new PKCS#12MacData object.
	  
	 @param aMacData contains a PKCS#12 MacData Structure.
	 @param aAuthSafeData is the ContentData present in the authSafe Sequence
			of PKCS#12 PFX Structure.
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the data is not Pkcs12 macData structure.
	 */
	IMPORT_C static CDecPkcs12MacData* NewL(const TDesC8& aMacData, const TDesC8& aAuthSafeData);
	
	/**
	 The DigestInfo present in the MacData.
	 It has the DigestAlgorithmIdentifier, and the Digest.
	 @return A reference to the CPKCS7DigestInfo object containing 
			 the decoded DigestInfo
	 */
	IMPORT_C const CPKCS7DigestInfo& DigestInfo() const; 
	
	/**
	 This method returns the MacSalt. 
	 MacSalt is used as input to the key generation mechanism.
	 @return A value indicating the MacSalt
	 */
	IMPORT_C const TDesC8& MacSalt() const;
	
	/**
	 This method returns the Iteration Count. It is used in creating the key.
	 @return An integer value indicating the IterationCount.
	 */
	IMPORT_C TInt IterationCount() const;
	
	/**
	 This method does the Integrity Check for Password Integrity Mode 
     by comparing the Digest present in the MacData of the PKCS#12 Structure
     with the hash generated from the content field of the authenticated Safe,
     password, the Iteration Count and the Salt present in the MacData.
	 @param aPassword contains the password to derive the key.
     @return Returns ETrue, if the Integrity verification passes.
             Returns EFalse, if the Integrity verification fails.
     @leave KErrNotSupported if the Pkcs7 digest algorithm is otherthan MD2, 
     						 MD5 and SHA-1
     @see   PKCS12KDF, CMessageDigest, 
     */
	IMPORT_C TBool VerifyIntegrityL(const TDesC& aPassword) const;
	
	/**
	 Destructor.
	 */
	virtual ~CDecPkcs12MacData();
	
private:
	/**
     This decodes the entire MacData structure.
     @param aMacData contains a PKCS#12 MacData Structure.
     @param aAuthSafeData is the ContentData present in the authSafe Sequence
			of PKCS#12 PFX Structure.
     @leave  KErrArgument if the data is not Pkcs12 macData structure.
     @see CPKCS7DigestInfo
     */
	void ConstructL(const TDesC8& aMacData, const TDesC8& aAuthSafeData);
		
	/**
	 Constructor.
	 */
	CDecPkcs12MacData();
	
	/**
	 Copy Constructor.
	 @param aDecPkcs12MacData A CDecPkcs12MacData object.
	 */
	CDecPkcs12MacData(const CDecPkcs12MacData& aDecPkcs12MacData);
	
	/**
	 Assignment operator.
	 @param aDecPkcs12MacData A CDecPkcs12MacData object.
	 @return A reference to CDecPkcs12MacData class.
	 */
	CDecPkcs12MacData& operator=(const CDecPkcs12MacData& aDecPkcs12MacData);
	
private:
	/** DigestInfo(PKCS#7 structure) present in the PKCS#12 MacData structure */
	CPKCS7DigestInfo* iDigestInfo;
	
   	/** MacSalt present in the PKCS#12 MacData structure */
	TPtrC8 iMacSalt;
	
	/** Iteration Count present in the PKCS#12 MacData structure */
	TInt   iIterationCount;
	
	/** authSafe sequence present in PKCS#12 PFX structure.*/
	TPtrC8 iAuthSafeDataPtr;

	};
} // namespace PKCS12
#endif // __PKCS12_MACDATA_H__

