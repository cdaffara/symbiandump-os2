/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @released
*/

#ifndef __PKCS7_SIGNER_INFO_H__
#define __PKCS7_SIGNER_INFO_H__

#include <e32base.h>

class CX509AlgorithmIdentifier;
class CPKCS7IssuerAndSerialNumber;

/**
 * Represents the signer of PKCS7 signed data
 * @internalTechnology
 */
class CPKCS7SignerInfo : public CBase
	{
public:
	static CPKCS7SignerInfo* NewL(const TDesC8& aRawData);
	~CPKCS7SignerInfo();
	IMPORT_C TInt Version() const;
	IMPORT_C const CPKCS7IssuerAndSerialNumber& IssuerAndSerialNumber() const;
	IMPORT_C const CX509AlgorithmIdentifier& DigestAlgorithm() const;
	IMPORT_C const CX509AlgorithmIdentifier& DigestEncryptionAlgorithm() const;
	IMPORT_C const TPtrC8 EncryptedDigest() const;
private:
	CPKCS7SignerInfo(void);
	CPKCS7SignerInfo(const CPKCS7SignerInfo&);
	void ConstructL(const TDesC8& aRawData);

	void DecodeEncryptedDigestL(const TDesC8& aRawData);
private:
	TInt iVersion;
	CPKCS7IssuerAndSerialNumber* iIssuerAndSerialNumber;
	CX509AlgorithmIdentifier* iDigestAlgorithm;
	CX509AlgorithmIdentifier* iDigestEncryptionAlgorithm;
	HBufC8* iEncryptedDigest;
	};
#endif
