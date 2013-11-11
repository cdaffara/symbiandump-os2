/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements PKCS#10 certificate request class.
*
*/


#include <pkcs10.h>
#include <asn1enc.h>
#include <e32std.h>
#include <e32def.h>
#include <x500dn.h>
#include <x509keys.h>
#include <hash.h>
#include <pkcs10attr.h>
#include "keyhelper.h"
#include <mctkeystoreuids.h>

void Panic(TInt aError)
	{
	_LIT(KCategory, "PKCS10");
	User::Panic(KCategory, aError);
	}

// CPKCS10Request Class Implementation

CPKCS10Request::CPKCS10Request(const CX500DistinguishedName* aDN,
							   const CCTKeyInfo* aKeyInfo,
							   CPKCS10Attributes* aAttr) :
	CActive(EPriorityNormal),
	iDN(aDN),
	iKeyInfo(aKeyInfo),
	iAttributes(aAttr),
	iDigestId(ESHA1)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CPKCS10Request* CPKCS10Request::NewLC(const CX500DistinguishedName& aDN,
											   const CCTKeyInfo& aKeyInfo,
											   CPKCS10Attributes* aAttr/* = NULL*/)
	{
	// Sanity Check the input parameters
	if (&aDN == NULL)
		{
		User::Leave(KErrArgument);
		}

	if (&aKeyInfo == NULL)
		{
		User::Leave(KErrArgument);
		}

	CPKCS10Request* self = new (ELeave) CPKCS10Request(&aDN, &aKeyInfo, aAttr);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CPKCS10Request* CPKCS10Request::NewL(const CX500DistinguishedName& aDN,
											  const CCTKeyInfo& aKeyInfo,
											  CPKCS10Attributes* aAttr/* = NULL*/)
	{
	CPKCS10Request* self = NewLC(aDN, aKeyInfo, aAttr);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPKCS10Request::~CPKCS10Request()
	{
	Cancel();
	delete iAttributes;
	Reset();
	}

void CPKCS10Request::Reset()
	{
	delete iExportedKey;
	iExportedKey = NULL;
	delete iTBSData;
	iTBSData = NULL;
	delete iKeyHelper;
	iKeyHelper = NULL;
	if (iKeyStore)
		{
		iKeyStore->Release();
		iKeyStore = NULL;
		}
	iState = EIdle;
	}

EXPORT_C void CPKCS10Request::SetDistinguishedNameL(const CX500DistinguishedName& aDN)
	{
	// Sanity check
	if (&aDN == NULL)
		{
		User::Leave(KErrArgument);
		}
	iDN = &aDN;
	}

EXPORT_C void CPKCS10Request::SetKeyInfoL(const CCTKeyInfo& aKeyInfo)
	{
	// Sanity check
	if (&aKeyInfo == NULL)
		{
		User::Leave(KErrArgument);
		}
	iKeyInfo = &aKeyInfo;
	}

EXPORT_C void CPKCS10Request::SetAttributes(CPKCS10Attributes* aAttr)
	{
	delete iAttributes;
	iAttributes = aAttr;
	}

EXPORT_C void CPKCS10Request::SetDigestAlgL(TAlgorithmId aDigestId)
	{
	if (aDigestId != EMD2 && aDigestId != EMD5 && aDigestId != ESHA1)
		{
		User::Leave(KErrArgument);
		}
	if (iKeyInfo->Algorithm() == CCTKeyInfo::EDSA && aDigestId != ESHA1)
		{
		User::Leave(KErrArgument);
		}
	iDigestId = aDigestId;
	}

EXPORT_C void CPKCS10Request::CreateEncoding(HBufC8*& aResult, TRequestStatus& aStatus)
	{
	ASSERT(iState == EIdle);	
	iClientStatus = &aStatus;
	iResult = &aResult;
	aResult = NULL;	
	aStatus = KRequestPending;
	iState = EInitialize;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

TInt CPKCS10Request::RunError(TInt aErr)
	{
    User::RequestComplete(iClientStatus, aErr);
	iState = EIdle;
    return KErrNone;
	}

void CPKCS10Request::DoCancel()
	{
	switch (iState)
		{
		case EGetKeyStore:
			iKeyInfo->Token().CancelGetInterface();
			break;

		case EGetPublicKey:
			iKeyStore->CancelExportPublic();
			break;

		case EOpenSigner:
			iKeyHelper->CancelOpenSigner();
			break;

		case ESign:
			iKeyHelper->CancelSignDigest();
			break;

		default:
			// do nothing, keep compiler happy
			break;			
		}
	
	if (iClientStatus)
		User::RequestComplete(iClientStatus, KErrCancel);

	iState = EIdle;
	}	

void CPKCS10Request::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EInitialize:
			// Get keystore interface
			if (iKeyStore)
				{
				iKeyStore->Release();
				iKeyStore = NULL;
				}
			iKeyInfo->Token().GetInterface(TUid::Uid(KInterfaceKeyStore),
										  *reinterpret_cast<MCTTokenInterface**>(&iKeyStore),
										  iStatus);
			iState = EGetKeyStore;
			SetActive();
			break;

		case EGetKeyStore:
			// Fetch the public key
			delete iExportedKey;
			iKeyStore->ExportPublic(*iKeyInfo, iExportedKey, iStatus);
			iState = EGetPublicKey;
			SetActive();
			break;

		case EGetPublicKey:
			// Create key helper object
			delete iKeyHelper;
			iKeyHelper = CPKCS10KeyHelper::CreateKeyHelperL(*iKeyStore, *iKeyInfo, *iExportedKey, iDigestId);
			EncodeTBSDataL();

			// Open signing object
			iKeyHelper->OpenSigner(iStatus);
			iState = EOpenSigner;
			SetActive();
			break;

		case EOpenSigner:
			// Create digest
			{
			CMessageDigest* digest = NULL;
			switch (iDigestId)
				{
				case EMD2:
					digest = CMD2::NewL();
					break;
				case EMD5:
					digest = CMD5::NewL();
					break;
				case ESHA1:
					digest = CSHA1::NewL();
					break;
				default:
					User::Invariant();
				}
			CleanupStack::PushL(digest);

			// Hash data and sign
			digest->Update(*iTBSData);
			
			iKeyHelper->SignDigestL(digest->Final(), iStatus);
			CleanupStack::PopAndDestroy(digest); // keystore copies data to be signed
			iState = ESign;
			SetActive();
			}
			break;

		case ESign:
			CreateFinalEncodingL();
			Reset();
			break;
		
		default:
			User::Invariant();
		}
	}

CASN1EncBase* CPKCS10Request::MakeAttrEncLC() 
	{
	if (iAttributes)
		{
		CASN1EncBase* result = iAttributes->TakeEncodingLC();
		delete iAttributes;
		iAttributes = NULL;
		return result;
		}
	else
		{
		CASN1EncSequence* contextSpecific = CASN1EncSequence::NewLC();
		contextSpecific->SetTag(0);
		return contextSpecific;
		}
	}

CASN1EncSequence* CPKCS10Request::MakeCertRequestInfoEncLC()
	{
	// Top-level sequence contains distinguished name and other 
	// stuff. This is what gets signed with the entity's private key.
	CASN1EncSequence* certRequestInfo = CASN1EncSequence::NewLC();

	// Encode version number, which is 0.
	CASN1EncInt* version = CASN1EncInt::NewLC(0);
	certRequestInfo->AddAndPopChildL(version);

	// Encode distinguished name.
	CASN1EncBase* distinguishedName = iDN->EncodeASN1LC();
	certRequestInfo->AddAndPopChildL(distinguishedName);

	// Encode SubjectPublicKeyInfo.
	CASN1EncBase* subjectPubKeyInfo = iKeyHelper->EncodeKeyLC();
	certRequestInfo->AddAndPopChildL(subjectPubKeyInfo);

	// Encode attributes, if any.
	CASN1EncBase* attr = MakeAttrEncLC();
	certRequestInfo->AddAndPopChildL(attr);

	return certRequestInfo;
	}

void CPKCS10Request::EncodeTBSDataL() 
	{
	// The data we provide for signing is the certRequestInfo object.
	CASN1EncBase* certRequestInfo = MakeCertRequestInfoEncLC();
	// Write DER of it to the buffer.
	delete iTBSData;
	iTBSData = HBufC8::NewMaxL(certRequestInfo->LengthDER());
	TPtr8 dataPtr = iTBSData->Des();
	TUint pos = 0;
	certRequestInfo->WriteDERL(dataPtr, pos);
	CleanupStack::PopAndDestroy(certRequestInfo);
	}

void CPKCS10Request::CreateFinalEncodingL()
	{
	// the root sequence contains all other components of a X509 signed object
	CASN1EncSequence* root = CASN1EncSequence::NewLC();

	// wrap data to be signed in a sequence and add it to the root
	CASN1EncEncoding* encenc = CASN1EncEncoding::NewLC(*iTBSData);
	root->AddAndPopChildL(encenc);

	// encode signature algorithm and  parameters and add them to the root
	CASN1EncSequence* sigalg = iKeyHelper->EncodeSignatureAlgorithmLC();
	root->AddAndPopChildL(sigalg);

	// Create ASN.1 bit string from the signature 
	CASN1EncBitString* encSig = iKeyHelper->EncodeSignatureLC();
	root->AddAndPopChildL(encSig);

	// encode the object in a DER encoding
	HBufC8* der = HBufC8::NewMaxLC(root->LengthDER());
	TPtr8 pder(der->Des());
	TUint pos = 0;
	root->WriteDERL(pder, pos);
	CleanupStack::Pop(der);
	CleanupStack::PopAndDestroy(root);

	*iResult = der;
	User::RequestComplete(iClientStatus, KErrNone);
	}

