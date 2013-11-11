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


#include <e32base.h>
#include <ct.h>
#include <mctkeystore.h>
#include "ClientOpenedKeys.h"
#include "cfskeystoreclient.h"
#include "hash.h"
#include <mctkeystoreuids.h>


// COpenedKey //////////////////////////////////////////////////////////////////
	
COpenedKey::~COpenedKey()
	{
	iClient->ReleaseObject(iHandle);
	delete iLabel;
	}

// CRSARepudiableSigner ////////////////////////////////////////////////////////

CRSARepudiableSigner* CRSARepudiableSigner::New(CFSKeyStoreClient* aClient)
	{
	return new CRSARepudiableSigner(aClient);
	}

CRSARepudiableSigner::CRSARepudiableSigner(CFSKeyStoreClient* aClient)
		: MCTSigner<CRSASignature*>(aClient->Token())
	{
	iClient = aClient;
	iHandle.iTokenHandle = aClient->Token().Handle();
	iHandle.iObjectId = 0;
	}

CRSARepudiableSigner::~CRSARepudiableSigner()
	{
	delete iDigest;
	}

void CRSARepudiableSigner::Release()
	{
	MCTTokenObject::Release();
	}

const TDesC& CRSARepudiableSigner::Label() const
	{
	return iLabel ? static_cast<const TDesC&>(*iLabel) : static_cast<const TDesC&>(KNullDesC);
	}

MCTToken& CRSARepudiableSigner::Token() const
	{
	return iClient->Token();
	}

TUid CRSARepudiableSigner::Type() const
	{
	return KRSARepudiableSignerUID;
	}

TCTTokenObjectHandle CRSARepudiableSigner::Handle() const
	{
	return iHandle;
	}

void CRSARepudiableSigner::SignMessage(const TDesC8& aPlaintext, 
  				CRSASignature*& aSignature, 
  				TRequestStatus& aStatus)
	{
	// Hash the data on the client side
	TRAPD(err, iDigest = CSHA1::NewL());
	if (err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
    else
        {
        iDigest->Update(aPlaintext);	
        Sign(iDigest->Final(), aSignature, aStatus);
        }
	}

void CRSARepudiableSigner::Sign(const TDesC8& aPlaintext, 
  				CRSASignature*& aSignature, 
  				TRequestStatus& aStatus)
	{
	iClient->RepudiableRSASign(Handle(),aPlaintext, aSignature, aStatus);
	}

void CRSARepudiableSigner::CancelSign()
	{
	iClient->CancelRepudiableRSASign();
	}

// CDSARepudiableSigner ////////////////////////////////////////////////////////

CDSARepudiableSigner* CDSARepudiableSigner::New(CFSKeyStoreClient* aClient)
	{
	return new CDSARepudiableSigner(aClient);
	}

CDSARepudiableSigner::CDSARepudiableSigner(CFSKeyStoreClient* aClient)
		: MCTSigner<CDSASignature*>(aClient->Token())
	{
	iClient = aClient;
	iHandle.iTokenHandle = aClient->Token().Handle();
	iHandle.iObjectId = 0;
	}


CDSARepudiableSigner::~CDSARepudiableSigner()
	{
	delete iDigest;
	}

void CDSARepudiableSigner::Release()
	{
	MCTTokenObject::Release();
	}

const TDesC& CDSARepudiableSigner::Label() const
	{
	return iLabel ? static_cast<const TDesC&>(*iLabel) : static_cast<const TDesC&>(KNullDesC);
	}

MCTToken& CDSARepudiableSigner::Token() const
	{
	return iClient->Token();
	}

TUid CDSARepudiableSigner::Type() const
	{
	return KDSARepudiableSignerUID;
	}

TCTTokenObjectHandle CDSARepudiableSigner::Handle() const
	{
	return iHandle;
	}

void CDSARepudiableSigner::SignMessage(const TDesC8& aPlaintext, 
  				CDSASignature*& aSignature, 
 				TRequestStatus& aStatus)
	{
	// Hash the data on the client side
	TRAPD(err, iDigest = CSHA1::NewL());
	if (err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
    else
        {
        iDigest->Update(aPlaintext);
        Sign(iDigest->Final(), aSignature, aStatus);
        }
	}

void CDSARepudiableSigner::Sign(const TDesC8& aPlaintext, 
  				CDSASignature*& aSignature, 
 				TRequestStatus& aStatus)
	{
	iClient->RepudiableDSASign(Handle(),aPlaintext, aSignature, aStatus);
	}

void CDSARepudiableSigner::CancelSign()
	{
	iClient->CancelRepudiableDSASign();
	}

// CFSRSADecryptor /////////////////////////////////////////////////////////////

CFSRSADecryptor* CFSRSADecryptor::New(CFSKeyStoreClient* aClient)
	{
	return new CFSRSADecryptor(aClient);
	}

CFSRSADecryptor::CFSRSADecryptor(CFSKeyStoreClient* aClient)
		: MCTDecryptor(aClient->Token())
	{
	iClient = aClient;
	iHandle.iTokenHandle = aClient->Token().Handle();
	iHandle.iObjectId = 0;
	}

CFSRSADecryptor::~CFSRSADecryptor()
	{
	}

void CFSRSADecryptor::Release()
	{
	MCTTokenObject::Release();
	}

const TDesC& CFSRSADecryptor::Label() const
	{
	return iLabel ? static_cast<const TDesC&>(*iLabel) : static_cast<const TDesC&>(KNullDesC);
	}

MCTToken& CFSRSADecryptor::Token() const
	{
	return iClient->Token();
	}

TUid CFSRSADecryptor::Type() const
	{
	return KPrivateDecryptorUID;
	}

TCTTokenObjectHandle CFSRSADecryptor::Handle() const
	{
	return iHandle;
	}

void CFSRSADecryptor::Decrypt(const TDesC8& aCiphertext,
			TDes8& aPlaintext, 
			TRequestStatus& aStatus
	)
	{
	iClient->Decrypt(Handle(),aCiphertext,aPlaintext, aStatus);
	}

void CFSRSADecryptor::CancelDecrypt()
	{
	iClient->CancelDecrypt();
	}

// CDHAgreement ////////////////////////////////////////////////////////////////

CDHAgreement* CDHAgreement::New(CFSKeyStoreClient* aClient)
	{
	return new CDHAgreement(aClient);
	}

CDHAgreement::CDHAgreement(CFSKeyStoreClient* aClient)
	: MCTDH(aClient->Token())
	{
	iClient = aClient;
	iHandle.iTokenHandle = aClient->Token().Handle();
	iHandle.iObjectId = 0;
	}

CDHAgreement::~CDHAgreement()
	{
	}

void CDHAgreement::Release()
	{
	MCTTokenObject::Release();
	}

const TDesC& CDHAgreement::Label() const
	{
	return iLabel ? static_cast<const TDesC&>(*iLabel) : static_cast<const TDesC&>(KNullDesC);
	}

MCTToken& CDHAgreement::Token() const
	{
	return iClient->Token();
	}

TUid CDHAgreement::Type() const
	{
	return KKeyAgreementUID;
	}

TCTTokenObjectHandle CDHAgreement::Handle() const
	{
	return iHandle;
	}

/** Returns the public key ('Big X') for the supplied set of parameters */
void CDHAgreement::PublicKey(const TInteger& aN, const TInteger& aG, 
							 CDHPublicKey*& aX, TRequestStatus& aStatus)
	{
	iClient->DHPublicKey(Handle(), aN, aG, aX, aStatus);
	}

/** Agrees a session key given the public key of the other party */
void CDHAgreement::Agree(const CDHPublicKey& iY, HBufC8*& aAgreedKey,
						 TRequestStatus& aStatus)
	{
	iClient->DHAgree(Handle(), iY, aAgreedKey, aStatus);
	}

/** Cancels either a PublicKey or Agree operation */
void CDHAgreement::CancelAgreement()
	{
	iClient->CancelDH();
	}
