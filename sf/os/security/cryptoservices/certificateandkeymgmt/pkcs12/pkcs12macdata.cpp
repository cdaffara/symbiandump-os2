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


#include "pkcs12macdata.h"

using namespace PKCS12;

CDecPkcs12MacData::CDecPkcs12MacData()
	{
	}
	
EXPORT_C CDecPkcs12MacData* CDecPkcs12MacData::NewL(const TDesC8& aMacData, const TDesC8& aAuthSafeData)
	{
	CDecPkcs12MacData* self = new(ELeave) CDecPkcs12MacData();
	CleanupStack::PushL(self);
	self->ConstructL(aMacData, aAuthSafeData);
	CleanupStack::Pop(self);
	return self;
	}

CDecPkcs12MacData::~CDecPkcs12MacData()
	{
	delete iDigestInfo;
	}
	
void CDecPkcs12MacData::ConstructL(const TDesC8& aMacData, const TDesC8& aAuthSafeData)
	{
	iAuthSafeDataPtr.Set(aAuthSafeData);
		
	// MacData is a Sequence
	TASN1DecGeneric seqGen(aMacData);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
		
	// Decode the MacData Sequence
	TASN1DecSequence seq;
	CArrayPtr<TASN1DecGeneric>* macDataSequence = seq.DecodeDERLC(seqGen);
	TInt macDataCount = macDataSequence->Count();
	// Check if Mac, MacSalt and Iteration Count are present
	// Iteration Count is 1 by default
	if (macDataCount < 2 || macDataCount > 3)
		{
		User::Leave(KErrArgument);
		}
	
	// DigestInfo
	const TASN1DecGeneric* macDataSequenceAt0 = macDataSequence->At(0);
	// DigestInfo is a Sequence
	if (macDataSequenceAt0->Tag() != EASN1Sequence || macDataSequenceAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	iDigestInfo = CPKCS7DigestInfo::NewL(macDataSequenceAt0->Encoding());
		
	// MacSalt
	const TASN1DecGeneric* macDataSequenceAt1 = macDataSequence->At(1);
	// MacSalt is an OctetString
	if (macDataSequenceAt1->Tag() != EASN1OctetString || macDataSequenceAt1->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}  
    iMacSalt.Set(macDataSequenceAt1->GetContentDER());
    
    // Iteration Count
	if(macDataSequence->Count() == 3)
		{
		// Check for IterationCount
		const TASN1DecGeneric* macDataSequenceAt2 = macDataSequence->At(2);
		if (macDataSequenceAt2->Tag() != EASN1Integer || macDataSequenceAt2->Class() != EUniversal)
			{
			User::Leave(KErrArgument);
			}
		
		TASN1DecInteger intDecoder;
		iIterationCount = intDecoder.DecodeDERShortL(*macDataSequenceAt2);
		if(iIterationCount <= 0)
			{
			User::Leave(KErrArgument);
			}
		}
    // Assign the Default value as 1 if Iteration Count is not present.
	else
		{
		iIterationCount = KDefaultIterationCount;
		}	
	CleanupStack::PopAndDestroy(macDataSequence);
    }
	
EXPORT_C const CPKCS7DigestInfo& CDecPkcs12MacData::DigestInfo() const
	{
	return *iDigestInfo;
	}
	
EXPORT_C const TDesC8& CDecPkcs12MacData::MacSalt() const
	{
	return iMacSalt;	
	}
	
EXPORT_C TInt CDecPkcs12MacData::IterationCount() const
	{
	return iIterationCount;	
	}

EXPORT_C TBool CDecPkcs12MacData::VerifyIntegrityL(const TDesC& aPassword) const
	{	
	__UHEAP_MARK;
	HBufC8* encryptKey = HBufC8::NewMaxLC(KSha1HmacKeyLength);
	TPtr8 encryptedKey(encryptKey->Des());
	
	// Convert the supplied string to a byte string. Each character is converted to a big 
	// endian two-byte value, and a terminating NULL character is appended to the end.
	HBufC8* password = PKCS12KDF::GeneratePasswordLC(aPassword);
		
	PKCS12KDF::DeriveKeyL(encryptedKey, PKCS12KDF::EIDByteMACKey, *password ,MacSalt(), IterationCount());
	
	CMessageDigest* digest1 = NULL;
	TAlgorithmId algorithmId = DigestInfo().Algorithm();
	if (algorithmId != ESHA1)
		{
		User::Leave(KErrNotSupported);
		}
	digest1 = CSHA1::NewL();
	CleanupStack::PushL( digest1 );
	
	CHMAC* hmac1 = CHMAC::NewL( encryptedKey, digest1);
	CleanupStack::PushL( hmac1 );
	
	TPtrC8 tmpHash = hmac1->Final(iAuthSafeDataPtr);
	TInt ret = tmpHash.Compare(DigestInfo().Digest());
	TBool result;
	if(ret)
		{
		result = EFalse;
		}
	else
		{
		result = ETrue;
		}
	
	CleanupStack::PopAndDestroy(hmac1);
	CleanupStack::Pop(digest1);
	CleanupStack::PopAndDestroy(2,encryptKey); // encryptKey, password
			
	__UHEAP_MARKEND;

	return result;	
	}
	


