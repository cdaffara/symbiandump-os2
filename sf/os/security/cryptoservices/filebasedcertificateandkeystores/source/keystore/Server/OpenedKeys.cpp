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


#include "OpenedKeys.h"
#include "cfskeystoreserver.h"
#include "keystreamutils.h"
#include "fsdatatypes.h"
#include "keystorepassphrase.h"

#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <bigint.h>
#include <ct.h>
#include <securityerr.h>
#include <e32base.h>
#include <mctkeystoreuids.h>

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <authserver/authtypes.h>
#include <authserver/auth_srv_errs.h>
#include <s32mem.h>
#include "keystore_errs.h"
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

// COpenedKey //////////////////////////////////////////////////////////////////

COpenedKey* COpenedKey::NewL(const CFileKeyData& aKeyData, TUid aType, const RMessage2& aMessage,
							 CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan)
	{
	COpenedKey* self = NULL;

	if (aType == KRSARepudiableSignerUID)
		{
		self = new (ELeave) CRSARepudiableSigner(aKeyData, aKeyDataMan, aPassMan);
		}
	else if (aType == KDSARepudiableSignerUID)
		{
		self = new (ELeave) CDSARepudiableSigner(aKeyData, aKeyDataMan, aPassMan);
		}
	else if (aType == KPrivateDecryptorUID)
		{
		self = new (ELeave) CFSRSADecryptor(aKeyData, aKeyDataMan, aPassMan);
		}
	else if (aType == KKeyAgreementUID)
		{
		self = new (ELeave) CDHAgreement(aKeyData, aKeyDataMan, aPassMan);
		}
	else
		{
		User::Invariant();
		}

	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	CleanupStack::Pop(self);
	return self;
	}

COpenedKey::COpenedKey(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	CActive(EPriorityStandard),
	iKeyData(aKeyData),
	iKeyDataMan(aKeyDataMan),
	iPassMan(aPassMan)
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	,iUserIdentity(NULL)
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	{
	}

void COpenedKey::ConstructL(const RMessage2& aMessage)
	{
	CKeyInfo* keyInfo = iKeyDataMan.ReadKeyInfoLC(iKeyData);
	CleanupStack::Pop(keyInfo);
	iKeyInfo = keyInfo;
	CheckKeyL(aMessage);
	iLabel = iKeyInfo->Label().AllocL();
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	User::LeaveIfError(iAuthClient.Connect());
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	CActiveScheduler::Add(this);
	}

COpenedKey::~COpenedKey()
	{
	Cancel();
	delete iLabel;
	delete iKeyInfo;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iAuthClient.Close();
	delete iExpression;
	delete iUserIdentity;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	}

const TDesC& COpenedKey::Label() const
	{
	return *iLabel;
	}

TInt COpenedKey::Handle() const
	{
	return iKeyData.Handle();
	}

void COpenedKey::CheckKeyL(const RMessage2& aMessage)
	{
	// Check the client is allowed to use the key
	if (!iKeyInfo->UsePolicy().CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

	// Check that the operation represented by this object is supported for this
	// type of key
	if (iKeyInfo->Algorithm() != Algorithm())
		{
		User::Leave(KErrKeyAlgorithm);
		}

	// Check the key usage allows the operation
	if ((iKeyInfo->Usage() & RequiredUsage()) == 0)
		{
		User::Leave(KErrKeyUsage);
		}

	// Check current time is after start date (if set) and before end date (if
	// set)
	TTime timeNow;
	timeNow.UniversalTime();
	if (iKeyInfo->StartDate().Int64() != 0 && timeNow < iKeyInfo->StartDate())
		{
		User::Leave(KErrKeyValidity);
		}
	if (iKeyInfo->EndDate().Int64() != 0 && timeNow >= iKeyInfo->EndDate())
		{
		User::Leave(KErrKeyValidity);
		}
		
	}

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
void COpenedKey::GetPassphrase(TRequestStatus& aStatus)
	{
	ASSERT(iState == EIdle);

	TInt timeout = iKeyDataMan.GetPassphraseTimeout();
	TStreamId passphraseId = iKeyDataMan.DefaultPassphraseId();
	ASSERT(passphraseId != KNullStreamId);
	iClientStatus = &aStatus;
	
	iPassMan.GetPassphrase(passphraseId, timeout, iPassphrase, iStatus);
	iState = EGetPassphrase;
	SetActive();
	}
#else
void COpenedKey::AuthenticateL()
	{	
	iExpression = iAuthClient.CreateAuthExpressionL(iKeyInfo->AuthExpression());
	TUid uid = TUid::Uid(0);
	iAuthClient.AuthenticateL(*iExpression,iKeyInfo->Freshness(), EFalse, uid, EFalse, KNullDesC, iUserIdentity, iStatus);
	iState = EAuthenticate;
	SetActive();
	}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void COpenedKey::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		case EDoAuthenticate:
			AuthenticateL();
			break;
			
		case EAuthenticate:
			if(iUserIdentity->Id() == AuthServer::KUnknownIdentity)
				{
				User::Leave(KErrAuthenticationFailure);
				}
					
			if (!iKeyRead)
				{
				RStoreReadStream stream;
				iKeyDataMan.OpenPrivateDataStreamLC(iKeyData, stream);
				TPtrC8 key = iUserIdentity->Key().KeyData();
				HBufC8* plaintext = DecryptFromStreamL(stream, key);
				CleanupStack::PushL(plaintext);
				TAny* ptr = const_cast<TAny*>(static_cast<const TAny*>(plaintext->Des().PtrZ()));
							
				RMemReadStream decryptedStream(ptr, plaintext->Length());
				decryptedStream.PushL();
				ReadPrivateKeyL(decryptedStream);
				CleanupStack::PopAndDestroy(3,&stream); // plaintext, decryptedStream
				iKeyRead = ETrue;
				}
			
			delete iUserIdentity;
			iUserIdentity = NULL;
			delete iExpression;
			iExpression = NULL;
			PerformOperationL();
			Complete(KErrNone);
			break;
		#else
		case EGetPassphrase:
			ASSERT(iPassphrase);
			if (!iKeyRead)
				{
				RStoreReadStream stream;
				iKeyDataMan.OpenPrivateDataStreamLC(iKeyData, *iPassphrase, stream);
				ReadPrivateKeyL(stream);
				CleanupStack::PopAndDestroy(&stream);
				iKeyRead = ETrue;
				}
			PerformOperationL();
			Complete(KErrNone);
			break;	
		#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		default:
			ASSERT(EFalse);
		}
	}

TInt COpenedKey::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void COpenedKey::DoCancel()
	{
	Complete(KErrCancel);
	}

void COpenedKey::Complete(TInt aError)
	{
	Cleanup();
	iPassphrase = NULL;
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	iState = EIdle;
	}

void COpenedKey::Cleanup()
	{
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		delete iUserIdentity;
		iUserIdentity = NULL;
		delete iExpression;
		iExpression = NULL;
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

// CRSARepudiableSigner ////////////////////////////////////////////////////////

CRSARepudiableSigner::CRSARepudiableSigner(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CRSARepudiableSigner::~CRSARepudiableSigner()
	{
	delete iPrivateKey;
	}

TUid CRSARepudiableSigner::Type() const
	{
	return KRSARepudiableSignerUID;
	}

CKeyInfo::EKeyAlgorithm CRSARepudiableSigner::Algorithm() const
	{
	return CKeyInfo::ERSA;
	}
 
TKeyUsagePKCS15 CRSARepudiableSigner::RequiredUsage() const
	{
	return EPKCS15UsageSignSignRecover;
	}

void CRSARepudiableSigner::Sign(const TDesC8& aPlaintext,
								CRSASignature*& aSignature,
								TRequestStatus& aStatus)
	{
	ASSERT(iPlaintext.Ptr() == NULL);
	ASSERT(iSignaturePtr == NULL);
	iPlaintext.Set(aPlaintext);
	iSignaturePtr = &aSignature;
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	GetPassphrase(aStatus);
#else
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	iState = EDoAuthenticate;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CRSARepudiableSigner::ReadPrivateKeyL(RReadStream& aStream)
	{
	ASSERT(iPrivateKey == NULL);
	CreateL(aStream, iPrivateKey);
	}

void CRSARepudiableSigner::PerformOperationL()
	{
	ASSERT(iPrivateKey);
	
	CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewLC(*iPrivateKey);
	const CRSASignature* signature = signer->SignL(iPlaintext);
	CleanupStack::PopAndDestroy(signer);
	*iSignaturePtr = const_cast<CRSASignature*>(signature);
	}

void CRSARepudiableSigner::Cleanup()
	{
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	COpenedKey::Cleanup();
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iPlaintext.Set(NULL, 0);
	iSignaturePtr = NULL;
	}

// CDSARepudiableSigner ////////////////////////////////////////////////////////

CDSARepudiableSigner::CDSARepudiableSigner(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CDSARepudiableSigner::~CDSARepudiableSigner()
	{
	delete iPrivateKey;
	}

TUid CDSARepudiableSigner::Type() const
	{
	return KDSARepudiableSignerUID;
	}

CKeyInfo::EKeyAlgorithm CDSARepudiableSigner::Algorithm() const
	{
	return CKeyInfo::EDSA;
	}
 
TKeyUsagePKCS15 CDSARepudiableSigner::RequiredUsage() const
	{
	return EPKCS15UsageSignSignRecover;
	}

void CDSARepudiableSigner::Sign(const TDesC8& aPlaintext,
								CDSASignature*& aSignature,
								TRequestStatus& aStatus)
	{
	ASSERT(iPlaintext.Ptr() == NULL);
	ASSERT(iSignaturePtr == NULL);
	iPlaintext.Set(aPlaintext);
	iSignaturePtr = &aSignature;
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	GetPassphrase(aStatus);
#else
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	iState = EDoAuthenticate;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CDSARepudiableSigner::ReadPrivateKeyL(RReadStream& aStream)
	{
	ASSERT(iPrivateKey == NULL);
	CreateL(aStream, iPrivateKey);
	}

void CDSARepudiableSigner::PerformOperationL()
	{
	ASSERT(iPrivateKey);
	
	CDSASigner* signer = CDSASigner::NewLC(*iPrivateKey);
	const CDSASignature* signature = signer->SignL(iPlaintext);
	CleanupStack::PopAndDestroy(signer);
	*iSignaturePtr = const_cast<CDSASignature*>(signature);
	}

void CDSARepudiableSigner::Cleanup()
	{
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	COpenedKey::Cleanup();
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	iPlaintext.Set(NULL, 0);
	iSignaturePtr = NULL;
	}

// CFSRSADecryptor /////////////////////////////////////////////////////////////

CFSRSADecryptor::CFSRSADecryptor(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CFSRSADecryptor::~CFSRSADecryptor()
	{
	delete iPrivateKey;
	}

TUid CFSRSADecryptor::Type() const
	{
	return KPrivateDecryptorUID;
	}

CKeyInfo::EKeyAlgorithm CFSRSADecryptor::Algorithm() const
	{
	return CKeyInfo::ERSA;
	}
 
TKeyUsagePKCS15 CFSRSADecryptor::RequiredUsage() const
	{
	return EPKCS15UsageDecryptUnwrap;
	}

void CFSRSADecryptor::Decrypt(const TDesC8& aCiphertext,
							  HBufC8*& aPlaintext,
							  TRequestStatus& aStatus)
	{
	ASSERT(iCiphertext.Ptr() == NULL);
	ASSERT(iPlaintextPtr == NULL);
	iCiphertext.Set(aCiphertext);
	iPlaintextPtr = &aPlaintext;
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	GetPassphrase(aStatus);
#else
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	iState = EDoAuthenticate;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CFSRSADecryptor::ReadPrivateKeyL(RReadStream& aStream)
	{
	ASSERT(iPrivateKey == NULL);
	CreateL(aStream, iPrivateKey);
	}

void CFSRSADecryptor::PerformOperationL()
	{
	ASSERT(iPrivateKey);
	
	CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewLC(*iPrivateKey);
	HBufC8* plaintext = HBufC8::NewMaxLC(decryptor->MaxOutputLength());
	TPtr8 ptr = plaintext->Des();
	decryptor->DecryptL(iCiphertext, ptr);

	*iPlaintextPtr = plaintext;
	CleanupStack::Pop(plaintext); // now owned by client
	CleanupStack::PopAndDestroy(decryptor);
	}

void CFSRSADecryptor::Cleanup()
	{
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	COpenedKey::Cleanup();
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	iCiphertext.Set(NULL, 0);
	iPlaintextPtr = NULL;
	}

// CDHAgreement ////////////////////////////////////////////////////////////////

CDHAgreement::CDHAgreement(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CDHAgreement::~CDHAgreement()
	{
	iKey.Close();
	}

TUid CDHAgreement::Type() const
	{
	return KKeyAgreementUID;
	}

CKeyInfo::EKeyAlgorithm CDHAgreement::Algorithm() const
	{
	return CKeyInfo::EDH;
	}
 
TKeyUsagePKCS15 CDHAgreement::RequiredUsage() const
	{
	return EPKCS15UsageDerive;
	}

void CDHAgreement::PublicKey(CDHParams& aParameters, RInteger& aPublicKey, TRequestStatus& aStatus)
	{
	ASSERT(iPKParams == NULL);
	ASSERT(iPKPublicKeyPtr == NULL);
	iPKParams = &aParameters;
	iPKPublicKeyPtr = &aPublicKey;
	iDHState = EPublicKey;
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	GetPassphrase(aStatus);
#else
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	iState = EDoAuthenticate;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CDHAgreement::Agree(CDHPublicKey& aY, HBufC8*& aAgreedKey, TRequestStatus& aStatus)
	{
	ASSERT(iAKPublicKey == NULL);
	ASSERT(iAKAgreedKeyPtr == NULL);
	iAKPublicKey = &aY;
	iAKAgreedKeyPtr = &aAgreedKey;
	iDHState = EAgree;
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	GetPassphrase(aStatus);
#else
	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	iState = EDoAuthenticate;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CDHAgreement::ReadPrivateKeyL(RReadStream& aStream)
	{
	CreateLC(aStream, iKey);
	CleanupStack::Pop(&iKey);
	}

void CDHAgreement::PerformOperationL()
	{
	switch (iDHState)
		{
		case EPublicKey:
			DoPublicKeyL();
			break;
		case EAgree:
			DoAgreeL();
			break;
		default:
			ASSERT(FALSE);
		}
	}

void CDHAgreement::DoPublicKeyL()
	{
	ASSERT(iPKParams);
	ASSERT(iPKPublicKeyPtr);
	
	RInteger n = iPKParams->TakeN();
	CleanupStack::PushL(n);
	RInteger g = iPKParams->TakeG();
	CleanupStack::PushL(g);
	RInteger x = RInteger::NewL(iKey);
	CleanupStack::PushL(x);
	CDHKeyPair* keyPair = CDHKeyPair::NewL(n, g, x);
	CleanupStack::Pop(3); // x, g, n
	CleanupStack::PushL(keyPair);
	
	const CDHPublicKey& pubKey = keyPair->PublicKey();
	*iPKPublicKeyPtr = RInteger::NewL(pubKey.X());
	CleanupStack::PopAndDestroy(keyPair);
	}

void CDHAgreement::DoAgreeL()
	{
	ASSERT(iAKPublicKey);
	ASSERT(iAKAgreedKeyPtr);
	
	RInteger n = RInteger::NewL(iAKPublicKey->N());
	CleanupStack::PushL(n);
	RInteger g = RInteger::NewL(iAKPublicKey->G());
	CleanupStack::PushL(g);
	RInteger x = RInteger::NewL(iKey);
	CleanupStack::PushL(x);
	CDHPrivateKey* privKey = CDHPrivateKey::NewL(n, g, x);
	CleanupStack::Pop(3); // x, g, n
	CleanupStack::PushL(privKey);
	CDH* dh = CDH::NewLC(*privKey);
	*iAKAgreedKeyPtr = const_cast<HBufC8*>(dh->AgreeL(*iAKPublicKey));
	CleanupStack::PopAndDestroy(2, privKey);
	}

void CDHAgreement::Cleanup()
	{
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	COpenedKey::Cleanup();
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	iPKParams = NULL;
	iPKPublicKeyPtr = NULL;
	iAKPublicKey = NULL;
	iAKAgreedKeyPtr = NULL;
	iDHState = EIdle;
	}
