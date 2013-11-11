/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains the implementation of CHardwareKeyStoreClient. In this 
* class only the newly added APIs for WMDRM have been implemented. 
* So this is not a full functional crypto token.
*
*/


#include "thwkeystoreclient.h"
#include "thwkeystoredefs.h"
#include "keys.h"
#include <cryptospi/cryptospidef.h>
#include "cryptoasymmetriccipherapi.h"
#include "cryptosignatureapi.h"
#include <s32mem.h>

MCTTokenInterface* CHardwareKeyStoreClient::NewKeyStoreInterfaceL(MCTToken& aToken)
	{
	//	Destroyed by MCTTokenInterface::DoRelease() 
	CHardwareKeyStoreClient* me = new (ELeave) CHardwareKeyStoreClient(aToken);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

CHardwareKeyStoreClient::CHardwareKeyStoreClient(MCTToken& aToken)
	: CActive(EPriorityNormal), 
	// Reference count starts at one as we're always created and returned by a GetInterface() call on the token
	iRefCount(1),
	iToken(aToken)
	{
	}

void CHardwareKeyStoreClient::ConstructL()
	{
	CActiveScheduler::Add(this);
	iCryptoTokenHai = CCryptoTokenHai::NewL(&iToken);
	}

CHardwareKeyStoreClient::~CHardwareKeyStoreClient()
	{
	Cancel();
	delete iCryptoTokenHai;
	}

MCTToken& CHardwareKeyStoreClient::Token()
	{
	return iToken;
	}

void CHardwareKeyStoreClient::AddRef()
	{
	++iRefCount;
	
	iToken.AddRef();
	}

void CHardwareKeyStoreClient::DoRelease()
	{
	--iRefCount;

	ASSERT(iRefCount >= 0);
	if (iRefCount == 0)
		{
		MCTTokenInterface::DoRelease();
		}
	}

void CHardwareKeyStoreClient::ReleaseObject(const TCTTokenObjectHandle& /*aHandle*/)
	{
	}

void CHardwareKeyStoreClient::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch(iState)
		{
		case ECreateSigner:
		{
			if(iCryptoTokenHai->KeyPresent(iHandle.iObjectId) != KErrNotFound)
			    {
                CryptoSpi::TKeyProperty keyProperty;
                keyProperty.iAlgorithmUid = CryptoSpi::KEccSignerUid;
                    
                CryptoSpi::CCryptoParams* keyParameters = CryptoSpi::CCryptoParams::NewLC();
                keyParameters->AddL(iHandle.iObjectId, CryptoSpi::KPassedHandleToKeyUid);
                CryptoSpi::CKey* newKey = CryptoSpi::CKey::NewL(keyProperty, *keyParameters);
                CleanupStack::PushL(newKey);
                
                CryptoSpi::CSignatureFactory::CreateSignerL( *(iClientBuffer.iSigner), CryptoSpi::KEccSignerUid, 
                                                            *newKey, CryptoSpi::KPaddingModeNoneUid, 
                                                            NULL );
                
                CleanupStack::PopAndDestroy(newKey);
                CleanupStack::PopAndDestroy(keyParameters);
			    }
			else
                {
                User::Leave(KErrNotFound);
                }
		}
		break;
		case ECreateAsymmetricCipher:
		{
			if(iCryptoTokenHai->KeyPresent(iHandle.iObjectId) != KErrNotFound)
                {
                CryptoSpi::TKeyProperty keyProperty;
                keyProperty.iAlgorithmUid = CryptoSpi::KEccCipherUid;
                    
                CryptoSpi::CCryptoParams* keyParameters = CryptoSpi::CCryptoParams::NewLC();
                keyParameters->AddL(iHandle.iObjectId, CryptoSpi::KPassedHandleToKeyUid);
                CryptoSpi::CKey* newKey = CryptoSpi::CKey::NewL(keyProperty, *keyParameters);
                CleanupStack::PushL(newKey);
                
                CryptoSpi::CAsymmetricCipherFactory::CreateAsymmetricCipherL(
                                    *(iClientBuffer.iAsymmetricCipher), CryptoSpi::KEccCipherUid,
                                    *newKey, CryptoSpi::KCryptoModeDecryptUid,
                                    CryptoSpi::KPaddingModeNoneUid, NULL);
                
                CleanupStack::PopAndDestroy(newKey);
                CleanupStack::PopAndDestroy(keyParameters);
                }
			else
			    {
			    User::Leave(KErrNotFound);
			    }
		}
		break;
		case EDecryptInHardware:
        {
            if(iCryptoTokenHai->KeyPresent(iHandle.iObjectId) != KErrNotFound)
                {
                iCryptoTokenHai->DecryptL(iHandle.iObjectId, *iPtr, *(iClientBuffer.decryptedText));
                }
            else
                {
                User::Leave(KErrNotFound);
                }
        }
        break;
        case ESignInHardware:
        	{
            if(iCryptoTokenHai->KeyPresent(iHandle.iObjectId) != KErrNotFound)
                {
                HBufC8* signature;
                iCryptoTokenHai->SignL(iHandle.iObjectId, *iPtr, signature);
                CleanupStack::PushL(signature);
                iClientBuffer.iSignature->AddL(signature->Des(), CryptoSpi::KEccKeyTypeUid);
                CleanupStack::PopAndDestroy(signature);
                }
            else
                {
                User::Leave(KErrNotFound);
                }
            break;
        	}
        		
		case EExportKey:
			{
		    if(iCryptoTokenHai->KeyPresent(iHandle.iObjectId) != KErrNotFound)
		        {
		        iCryptoTokenHai->ExportPublicKeyL(iHandle.iObjectId, *iPublicKey);
		        }
		    else
		        {
		        User::Leave(KErrNotFound);
		        }
		    break;
			}
		default:
			User::Leave(KErrArgument);
		}
	
	User::RequestComplete(iClientStatus,KErrNone);	
	}

void CHardwareKeyStoreClient::DoCancel()
	{
	User::RequestComplete(iClientStatus,KErrCancel);	
	}

TInt CHardwareKeyStoreClient::RunError(TInt aError)
    {
    User::RequestComplete(iClientStatus, aError);
    return KErrNone;
    }

/*
 * MKeystore interfaces
 */	
	
void CHardwareKeyStoreClient::List(
		RMPointerArray<CCTKeyInfo>& aKeys, 
		const TCTKeyAttributeFilter& aFilter, 
		TRequestStatus& aStatus)
	{
	iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    TRAPD(err,iCryptoTokenHai->ListL(aFilter,aKeys));
    User::RequestComplete(status, err);
    }

void CHardwareKeyStoreClient::CancelList()
	{
	
	}

void CHardwareKeyStoreClient::GetKeyInfo(TCTTokenObjectHandle /*aHandle*/, CCTKeyInfo*& /*aInfo*/, 
								   TRequestStatus& aStatus)
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelGetKeyInfo()
	{
	// synchronous, nothing to do
	}

void CHardwareKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MRSASigner*& /*aSigner*/, 
							 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MCTSigner<CDSASignature*>*& /*aSigner*/, 
							 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MCTDecryptor*& /*aDecryptor*/,
							 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MCTDH*& /*aDH*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelOpen()
	{
	// synchronous, nothing to do
	}

/** Returns the public key in DER-encoded ASN-1 */
void CHardwareKeyStoreClient::ExportPublic(const TCTTokenObjectHandle& aHandle,
									 HBufC8*& aPublicKey,
									 TRequestStatus& aStatus)
	{
	//TRequestStatus* status = &aStatus;
	iClientStatus = &aStatus;
	
	iStatus = KRequestPending;
    *iClientStatus = KRequestPending;
    iPublicKey = &aPublicKey;
    iHandle = aHandle;
    TRequestStatus* status = &iStatus;
    SetActive();
    iState = EExportKey;
    
    User::RequestComplete(status, KErrNone);
	}

void CHardwareKeyStoreClient::CancelExportPublic()
	{
	// synchronous, nothing to do
	}

//	*********************************************************************************
//	MCTKeyStoreManager
//	*********************************************************************************
void CHardwareKeyStoreClient::CreateKey(CCTKeyInfo*& /*aReturnedKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelCreateKey()
	{
	}

/**
 * This function is not implemented since in the WMDRM case, device 
 * are preprovisioned inside the device. Hence implementing this 
 * function is not required.
 */
void CHardwareKeyStoreClient::ImportKey(const TDesC8& /*aKey*/, CCTKeyInfo*& /*aReturnedKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelImportKey()
	{
	}

void CHardwareKeyStoreClient::ImportEncryptedKey(const TDesC8& /*aKey*/, CCTKeyInfo*& /*aReturnedKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelImportEncryptedKey()
	{
	}

void CHardwareKeyStoreClient::ExportKey(TCTTokenObjectHandle /*aHandle*/, HBufC8*& /*aKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelExportKey()
	{
	}

void CHardwareKeyStoreClient::ExportEncryptedKey(TCTTokenObjectHandle /*aHandle*/, const CPBEncryptParms& /*aParams*/,  HBufC8*& /*aKey*/, TRequestStatus& aStatus)
 	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
 	}

void CHardwareKeyStoreClient::CancelExportEncryptedKey()
	{
	}

void CHardwareKeyStoreClient::DeleteKey(TCTTokenObjectHandle /*aHandle*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelDeleteKey()
	{
	// synchronous, nothing to do
	}

void CHardwareKeyStoreClient::SetUsePolicy(TCTTokenObjectHandle /*aHandle*/, 
									 const TSecurityPolicy& /*aPolicy*/,
									 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelSetUsePolicy()
	{
	// synchronous, nothing to do
	}

void CHardwareKeyStoreClient::SetManagementPolicy(TCTTokenObjectHandle /*aHandle*/, 
											const TSecurityPolicy& /*aPolicy*/,
											TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CHardwareKeyStoreClient::CancelSetManagementPolicy()
	{
	// synchronous, nothing to do
	}

/**
 * @todo
 * This interface should return KErrNotSupported but because of the way 
 * it has been implemented in unifiedkeystore this is returning 
 * KErrNone. This should be fixed later.
 */ 
void CHardwareKeyStoreClient::SetPassphraseTimeout(TInt /*aTimeout*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CHardwareKeyStoreClient::CancelSetPassphraseTimeout()
	{
	// No point cancelling, not asynchronous
	}

/**
 * @todo
 * This interface should return KErrNotSupported but because of the way 
 * it has been implemented in unifiedkeystore this is returning 
 * KErrNone. This should be fixed later.
 */ 
void CHardwareKeyStoreClient::Relock(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CHardwareKeyStoreClient::CancelRelock()
	{
	// No point cancelling, not asynchronous
	}


void CHardwareKeyStoreClient::Open(	const TCTTokenObjectHandle& aHandle,
									CryptoSpi::CSigner*& aSigner,
									TRequestStatus& aStatus )
	{
	iClientStatus = &aStatus;    
    iStatus = KRequestPending;
    *iClientStatus = KRequestPending;
    iClientBuffer.iSigner = &aSigner;
    iHandle = aHandle;
    TRequestStatus* status = &iStatus;
    SetActive();
    iState = ECreateSigner;
    User::RequestComplete(status, KErrNone);
	}

void CHardwareKeyStoreClient::Open(	const TCTTokenObjectHandle& aHandle,
									CryptoSpi::CAsymmetricCipher*& aAsymmetricCipher,
									TRequestStatus& aStatus )
	{
    iClientStatus = &aStatus;    
    iStatus = KRequestPending;
    *iClientStatus = KRequestPending;
    iClientBuffer.iAsymmetricCipher = &aAsymmetricCipher;
    iHandle = aHandle;
    TRequestStatus* status = &iStatus;
    SetActive();
    iState = ECreateAsymmetricCipher;
    User::RequestComplete(status, KErrNone);
	}


void CHardwareKeyStoreClient::Decrypt(	const TCTTokenObjectHandle& aHandle,
        const TDesC8& aCiphertext,
										HBufC8*& aPlaintextPtr,
										TRequestStatus& aStatus )
	{
    iClientStatus = &aStatus;    
    iStatus = KRequestPending;
    *iClientStatus = KRequestPending;
    iHandle = aHandle;
    iPtr = &aCiphertext;
    iClientBuffer.decryptedText = &aPlaintextPtr;
    TRequestStatus* status = &iStatus;
    SetActive();
    iState = EDecryptInHardware;
    User::RequestComplete(status, KErrNone);
	}

void CHardwareKeyStoreClient::Sign(	const TCTTokenObjectHandle& aHandle,
        const TDesC8& aPlaintext,
										CryptoSpi::CCryptoParams*& aSignature,
										TRequestStatus& aStatus )
	{
    iClientStatus = &aStatus;    
    iStatus = KRequestPending;
    *iClientStatus = KRequestPending;
    iHandle = aHandle;
    iPtr = &aPlaintext;
    iClientBuffer.iSignature = aSignature;
    TRequestStatus* status = &iStatus;
    SetActive();
    iState = ESignInHardware;
    User::RequestComplete(status, KErrNone);
	}


void CHardwareKeyStoreClient::ImportKey(	TPtr /* aLabel */, 
											TPtr8 /* aPrivateKey */, TPtr8 /* aPublicKey */ , 
											TRequestStatus& aStatus)
	{
    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, KErrNone);
	}





