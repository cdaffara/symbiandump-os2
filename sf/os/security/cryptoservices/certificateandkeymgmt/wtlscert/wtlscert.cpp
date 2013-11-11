/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* -- FingerPrint Note:
* Developers have to be aware that if they are going to change the fingerprint for this certificate
* for a different hash, then there are other places that need to reflect this change
* -- Location
* void CWTLSCertificate::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
* EXPORT_C void CWTLSCertificate::InternalizeL(RReadStream& aStream)
* Also change the CX509Certificate and search for other occurences of the current
* hash.
*
*/


#include <wtlscert.h>
#include <wtlskeys.h>
#include "wtlsdec.h"
#include <hash.h>

enum TEncAlgorithmType
	{
	EEncRSA = 0x02
	};

enum TEncSigAlgorithmType
	{
	EEncRSAwithSHA1 = 0x02
	};

const TInt KMinAlgIdLength = 2;
const TInt KMinExpLengthBytes = 1;
const TInt KMaxExpLengthBytes = 65535;
const TInt KMinModLengthBytes = 1;
const TInt KMaxModLengthBytes = 65535;

//WTLS RSA signature result
EXPORT_C CWTLSRSASignatureResult* CWTLSRSASignatureResult::NewL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest)
	{
	CWTLSRSASignatureResult* self = CWTLSRSASignatureResult::NewLC(aDigestAlgorithm, aDigest);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSRSASignatureResult* CWTLSRSASignatureResult::NewLC(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest)
	{
	CWTLSRSASignatureResult* self = new(ELeave) CWTLSRSASignatureResult;
	CleanupStack::PushL(self);
	self->ConstructL(aDigestAlgorithm, aDigest);
	return self;
	}

EXPORT_C TBool CWTLSRSASignatureResult::VerifyL(const TDesC8& aResult)
	{
	return aResult == *iDigest;
	}

void CWTLSRSASignatureResult::ConstructL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest)
	{
	iDigestAlgorithm = CAlgorithmIdentifier::NewL(aDigestAlgorithm);
	iDigest = aDigest.AllocL();
	}

//WTLS KeyFactory
CRSAPublicKey* TWTLSKeyFactory::RSAPublicKeyL(const TDesC8& aEncoding) const
	{
	return CWTLSRSAPublicKey::NewL(aEncoding);
	}

CRSASignatureResult* TWTLSKeyFactory::RSASignatureResultL(const CAlgorithmIdentifier& aDigestAlgorithm, TDesC8& aDigest) const
	{
	return CWTLSRSASignatureResult::NewL(aDigestAlgorithm, aDigest);
	}

CDSAPublicKey* TWTLSKeyFactory::DSAPublicKeyL(const CDSAParameters& /*aParams*/, const TDesC8& /*aEncoding*/) const
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CDSAPublicKey* TWTLSKeyFactory::DSAPublicKeyL(const TDesC8& /*aParams*/, const TDesC8& /*aEncoding*/) const
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CDSASignature* TWTLSKeyFactory::DSASignatureL(const TDesC8& /*aEncoding*/) const
{
	User::Leave(KErrNotSupported);
	return NULL;	
}


CDSAParameters* TWTLSKeyFactory::DSAParametersL(const TDesC8& /*aParamsEncoding*/) const
{
	User::Leave(KErrNotSupported);
	return NULL;	
}

//validity period
EXPORT_C CWTLSValidityPeriod* CWTLSValidityPeriod::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSValidityPeriod::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSValidityPeriod* CWTLSValidityPeriod::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSValidityPeriod::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSValidityPeriod* CWTLSValidityPeriod::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSValidityPeriod* self = CWTLSValidityPeriod::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSValidityPeriod* CWTLSValidityPeriod::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSValidityPeriod* self = new(ELeave) CWTLSValidityPeriod;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CWTLSValidityPeriod::CWTLSValidityPeriod()
	{
	}

void CWTLSValidityPeriod::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TWTLSDecTime timeDec;
	iStart = timeDec.DecodeL(aBinaryData, aPos);
	iFinish = timeDec.DecodeL(aBinaryData, aPos);
	}

//algorithm id
EXPORT_C CWTLSAlgorithmIdentifier* CWTLSAlgorithmIdentifier::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSAlgorithmIdentifier::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSAlgorithmIdentifier* CWTLSAlgorithmIdentifier::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSAlgorithmIdentifier::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSAlgorithmIdentifier* CWTLSAlgorithmIdentifier::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSAlgorithmIdentifier* self = CWTLSAlgorithmIdentifier::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSAlgorithmIdentifier* CWTLSAlgorithmIdentifier::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSAlgorithmIdentifier* self = new(ELeave) CWTLSAlgorithmIdentifier;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CWTLSAlgorithmIdentifier::CWTLSAlgorithmIdentifier()
	{
	}

void CWTLSAlgorithmIdentifier::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < KMinAlgIdLength)
		{
		User::Leave(KErrArgument);
		}
	switch (aBinaryData[aPos])
		{
		case EEncRSA:
			{
			iAlgorithmId = ERSA;
			aPos++;
			if (aBinaryData[aPos] != 0)
				{
				User::Leave(KErrArgument);
				}
			aPos++;
			iEncodedParams = HBufC8::NewL(1);
			*iEncodedParams = KNullDesC8;
			break;
			}
		default:
			//we only support RSA just now...
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

//signing algorithm id
EXPORT_C CWTLSSigningAlgorithmIdentifier* CWTLSSigningAlgorithmIdentifier::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSSigningAlgorithmIdentifier::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSSigningAlgorithmIdentifier* CWTLSSigningAlgorithmIdentifier::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSSigningAlgorithmIdentifier::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSSigningAlgorithmIdentifier* CWTLSSigningAlgorithmIdentifier::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSSigningAlgorithmIdentifier* self = CWTLSSigningAlgorithmIdentifier::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSSigningAlgorithmIdentifier* CWTLSSigningAlgorithmIdentifier::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSSigningAlgorithmIdentifier* self = new(ELeave) CWTLSSigningAlgorithmIdentifier;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CWTLSSigningAlgorithmIdentifier::CWTLSSigningAlgorithmIdentifier()
	{
	}

void CWTLSSigningAlgorithmIdentifier::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < 1)
		{
		User::Leave(KErrArgument);
		}
	switch (aBinaryData[aPos])
		{
		case EEncRSAwithSHA1:
			{
			TAlgorithmId asym = ERSA;
			TAlgorithmId dig = ESHA1;
			iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(asym, KNullDesC8);
			iDigestAlgorithm = CAlgorithmIdentifier::NewL(dig, KNullDesC8);
			aPos++;
			break;
			}
		default:
			//we only support RSA-SHA1 just now...
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

//wtls subject public key info
EXPORT_C CWTLSSubjectPublicKeyInfo* CWTLSSubjectPublicKeyInfo::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSSubjectPublicKeyInfo::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSSubjectPublicKeyInfo* CWTLSSubjectPublicKeyInfo::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSSubjectPublicKeyInfo::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSSubjectPublicKeyInfo* CWTLSSubjectPublicKeyInfo::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSSubjectPublicKeyInfo* self = CWTLSSubjectPublicKeyInfo::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSSubjectPublicKeyInfo* CWTLSSubjectPublicKeyInfo::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSSubjectPublicKeyInfo* self = new(ELeave) CWTLSSubjectPublicKeyInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CWTLSSubjectPublicKeyInfo::CWTLSSubjectPublicKeyInfo()
	{
	}

void CWTLSSubjectPublicKeyInfo::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	iAlgId = CWTLSAlgorithmIdentifier::NewL(aBinaryData, aPos);
	if (iAlgId->Algorithm() != ERSA)
		{
		User::Leave(KErrNotSupported);
		}
	TInt totalLength = aBinaryData.Length();
	TInt tempPos = aPos;

	const TPtrC8 expEnc = aBinaryData.Right(totalLength - aPos);
	TWTLSDecVector exp(expEnc, KMinExpLengthBytes, KMaxExpLengthBytes);
	exp.InitL();

	aPos += exp.EncodingLength();

	const TPtrC8 modEnc = aBinaryData.Right(totalLength - aPos);
	TWTLSDecVector mod(modEnc, KMinModLengthBytes, KMaxModLengthBytes);
	mod.InitL();

	aPos+= mod.EncodingLength();
	iEncodedKeyData = (aBinaryData.Mid(tempPos, aPos - tempPos)).AllocL();
	}

//wtls certificate
EXPORT_C CWTLSCertificate* CWTLSCertificate::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSCertificate::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSCertificate::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSCertificate* self = CWTLSCertificate::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSCertificate* self = new(ELeave) CWTLSCertificate;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewL(RReadStream& aStream)
	{
	CWTLSCertificate* self = CWTLSCertificate::NewLC(aStream);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewLC(RReadStream& aStream)
	{
	CWTLSCertificate* self = new(ELeave) CWTLSCertificate;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}
/*
EXPORT_C CWTLSCertificate* CWTLSCertificate::NewL(CCertStore& aStore, const CCertStoreEntry& aEntry)
	{
	CWTLSCertificate* self = CWTLSCertificate::NewLC(aStore, aEntry);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewLC(CCertStore& aStore, const CCertStoreEntry& aEntry)
	{
	CWTLSCertificate* self = new(ELeave) CWTLSCertificate;
	CleanupStack::PushL(self);
	aStore.LoadL(*self, aEntry);
	return self;
	}
*/
EXPORT_C CWTLSCertificate* CWTLSCertificate::NewL(const CWTLSCertificate& aCert)
	{
	CWTLSCertificate* self = CWTLSCertificate::NewLC(aCert);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CWTLSCertificate* CWTLSCertificate::NewLC(const CWTLSCertificate& aCert)
	{
	CWTLSCertificate* self = new(ELeave) CWTLSCertificate;
	CleanupStack::PushL(self);
	self->ConstructL(aCert);
	return self;
	}

EXPORT_C CWTLSCertificate::~CWTLSCertificate()
	{
	delete iIssuerName;
	delete iSubjectName;
		
	if (iDataElements != NULL)
		{
		for (TInt i = 0; i < KWTLSCertMaxDataElements; i++)
			{
			delete iDataElements->At(i);
			}
		delete iDataElements;
		}
	}

EXPORT_C TBool CWTLSCertificate::IsEqualL(const CWTLSCertificate& aCert) const
	{
	return	(*(iFingerprint) == (*(aCert.iFingerprint)));
	}

	//extra accessors
EXPORT_C const TPtrC8 CWTLSCertificate::SignedDataL() const
	{
	return iEncoding->Left(iEncoding->Length() - (iSignature->Length() +2));
	}

EXPORT_C TInt CWTLSCertificate::Version() const
	{
	return iVersion;
	}

EXPORT_C const CWTLSName& CWTLSCertificate::IssuerName() const
	{
	return *iIssuerName;
	}

EXPORT_C const CWTLSName& CWTLSCertificate::SubjectName() const
	{
	return *iSubjectName;
	}

EXPORT_C HBufC* CWTLSCertificate::IssuerL() const
	{
	return iIssuerName->DisplayNameL();
	}

EXPORT_C HBufC* CWTLSCertificate::SubjectL() const
	{
	return iSubjectName->DisplayNameL();
	}

EXPORT_C TBool CWTLSCertificate::IsSelfSignedL() const
	{
	return iSubjectName->ExactMatchL(*iIssuerName);
	}

EXPORT_C const TPtrC8* CWTLSCertificate::DataElementEncoding(const TUint aIndex) const
	{
	return iDataElements->At(aIndex);
	}

EXPORT_C void CWTLSCertificate::InternalizeL(RReadStream& aStream)
	{
	if (iIssuerName != NULL) //just to check cert is uninitialised
		{
		User::Leave(KErrArgument);
		}
	iKeyFactory = new(ELeave) TWTLSKeyFactory;
	TInt len = aStream.ReadInt32L();
	iEncoding = HBufC8::NewL(aStream,len);
	TInt pos = 0;
	ConstructCertL(*iEncoding, pos);

	TWTLSDecUnsignedInteger decInt;
	TInt sigLength = decInt.DecodeShortL(*iEncoding, pos, 2);
	iSignature = (iEncoding->Mid(pos, sigLength)).AllocL();
	CSHA1* hash = CSHA1::NewL();
	CleanupStack::PushL(hash);
	iFingerprint = hash->Final(Encoding()).AllocL();
	CleanupStack::PopAndDestroy();

	InitEncodedDataElementsL();
	}

EXPORT_C TBool CWTLSCertificate::IsTCAL() const
	{
	TBool isTCA = EFalse;
	TPtrC8 nameData = SubjectName().NameData();
	CWTLSStructuredText* sText = NULL; //inited to get rid of warning
	TRAPD(err, sText = CWTLSStructuredText::NewL(nameData) );
	if( err == KErrNone )
	    {
		const TWTLSStructuredTextField* sTextField = sText->FieldByName(KWTLSTCAType);
		if(sTextField != NULL)
			{
			if(sTextField->Value().Compare(KWTLSTCAValue) == 0)
				{
				isTCA = ETrue;
				}
			}
		delete sText;
		}
	return isTCA;
	}

CWTLSCertificate::CWTLSCertificate()
	{
	}

void CWTLSCertificate::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TInt tempPos = aPos;
	ConstructCertL(aBinaryData, aPos);
	iKeyFactory = new(ELeave) TWTLSKeyFactory;

	TWTLSDecUnsignedInteger decInt;
	TInt sigLength = decInt.DecodeShortL(aBinaryData, aPos, 2);
	if ((sigLength + aPos) > aBinaryData.Length())
		{
		User::Leave(KErrArgument);
		}
	iSignature = (aBinaryData.Mid(aPos, sigLength)).AllocL();
	aPos+= sigLength;
	iEncoding = aBinaryData.Mid(tempPos, aPos - tempPos).AllocL();

	CSHA1* hash = CSHA1::NewL();
	CleanupStack::PushL(hash);
	iFingerprint = hash->Final(Encoding()).AllocL();
	CleanupStack::PopAndDestroy();
	
	InitEncodedDataElementsL();
	}

void CWTLSCertificate::ConstructL(const CWTLSCertificate& aCertificate)
	{
	iEncoding = aCertificate.Encoding().AllocL();
	iKeyFactory = new(ELeave) TWTLSKeyFactory;
	iSignature = aCertificate.Signature().AllocL();
	iFingerprint = aCertificate.Fingerprint().AllocL();
	iSigningAlgorithm = CSigningAlgorithmIdentifier::NewL(aCertificate.SigningAlgorithm());
	iSerialNumber = aCertificate.iSerialNumber->Des().AllocL();
	iIssuerName = CWTLSName::NewL(*(aCertificate.iIssuerName));
	iValidityPeriod = new(ELeave) CValidityPeriod(*(aCertificate.iValidityPeriod));
	iSubjectName = CWTLSName::NewL(*(aCertificate.iSubjectName));
	iSubjectPublicKeyInfo = CSubjectPublicKeyInfo::NewL(*(aCertificate.iSubjectPublicKeyInfo));

	InitEncodedDataElementsL();
	}

void CWTLSCertificate::ConstructCertL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < 1)
		{
		User::Leave(KErrArgument);
		}
	iVersion = aBinaryData[aPos];

	aPos++;
	iSigningAlgorithm = CWTLSSigningAlgorithmIdentifier::NewL(aBinaryData, aPos);
	iIssuerName = CWTLSName::NewL(aBinaryData, aPos);
	iValidityPeriod = CWTLSValidityPeriod::NewL(aBinaryData, aPos);
	iSubjectName = CWTLSName::NewL(aBinaryData, aPos);
	iSubjectPublicKeyInfo = CWTLSSubjectPublicKeyInfo::NewL(aBinaryData, aPos);
	iSerialNumber = HBufC8::NewL(0);
	*iSerialNumber = KNullDesC8;
	}

void CWTLSCertificate::InitEncodedDataElementsL()
	{
	iDataElements = new(ELeave) TFixedArray<TPtrC8*, KWTLSCertMaxDataElements>;
	iDataElements->Reset();
	const TPtrC8 signedData = SignedDataL();
	TInt aPos = 0;
	TPtrC8** pElement = iDataElements->Begin();
	*pElement++ = new(ELeave) TPtrC8(signedData.Left(++aPos));
	*pElement++ = new(ELeave) TPtrC8(signedData.Mid(aPos, aPos));
	aPos++;	//	Defect fix from Jetstream
	TInt issuerEncodedLength = IssuerName().NameData().Length() + 1;//1 for the identifier type
	*pElement++ = new(ELeave) TPtrC8(signedData.Mid(aPos, issuerEncodedLength));
	aPos+=+issuerEncodedLength;
	*pElement++ = new(ELeave) TPtrC8(signedData.Mid(aPos, 8));
	aPos+=8;
	TInt subjectEncodedLength = SubjectName().NameData().Length() + 1;//1 for the identifier type
	*pElement++ = new(ELeave) TPtrC8(signedData.Mid(aPos, subjectEncodedLength));
	aPos+=+subjectEncodedLength;
	*pElement++ = new(ELeave) TPtrC8(signedData.Right(signedData.Length() - aPos));
	}

