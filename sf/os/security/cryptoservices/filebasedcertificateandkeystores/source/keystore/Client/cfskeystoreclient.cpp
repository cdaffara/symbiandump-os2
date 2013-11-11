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


#include "cfskeystoreclient.h"
#include "CFStokenclient.h"
#include "fsmarshaller.h"
#include "fsdatatypes.h"
#include "ClientOpenedKeys.h"
#include "clientutils.h"
#include <mctauthobject.h>
#include <asymmetrickeys.h>
#include <pbedata.h>
#include "ct/logger.h"
#include <mctkeystoreuids.h>


// When retrieving data from the server we sometimes make a guess at the size of
// the data that will be returned so we can allocate a buffer for it.  If it's
// not big enough, the server tells us the required size and we retry.  These
// values are the initial guesses at buffer size:
const TInt KInitialBufSizeList = 1024;
const TInt KInitialBufSizeOpen = 64;

/**
 * A class that wraps an array of key info pointers.
 *
 * Implements the MKeyInfoArray interface required for marshalling key info
 * arrays.  The reason for all this is that we want to be able to use either an
 * array of CCTKeyInfo pointers or an array of MCTTokenObjects interchangably -
 * but the array classes are implemented as templates, so there is no
 * inheritance relation between Array\<A\> and Array\<B\> even if B is derived from
 * A.
 */
template <class KeyInfoT> class TKeyInfoArray : public MKeyInfoArray
	{
 public:
	TKeyInfoArray(RMPointerArray<KeyInfoT>& aArray) : iArray(aArray) { }
	virtual TInt Count() { return iArray.Count(); }
	virtual CCTKeyInfo* operator[](TInt aIndex) { return static_cast<CCTKeyInfo*>(iArray[aIndex]); }
	virtual TInt Append(CCTKeyInfo* aInfo) { return iArray.Append(aInfo); }
	virtual void Close() { iArray.Close(); }
 private:
	RMPointerArray<KeyInfoT>& iArray;
	};

/*static*/ MCTTokenInterface* CFSKeyStoreClient::NewKeyStoreUserInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient)
	{
	//	Destroyed by MCTTokenInterface::DoRelease() 
	CFSKeyStoreClient* me = new (ELeave) CFSKeyStoreClient(KInterfaceKeyStore, 
														   aToken, 
														   aClient);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

/*static*/ MCTTokenInterface* CFSKeyStoreClient::NewKeyStoreManagerInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient)
	{
	//	Destroyed by MCTTokenInterface::DoRelease()
	CFSKeyStoreClient* me = new (ELeave) CFSKeyStoreClient(KInterfaceKeyStoreManager, 
														   aToken, 
														   aClient);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

CFSKeyStoreClient::CFSKeyStoreClient(TInt aUID, MCTToken& aToken, RFileStoreClientSession& aClient)
	: CFSClient(aUID, aToken, aClient),
	  // Reference count starts at one as we're always created and returned by a GetInterface() call on the token
	  iRefCount(1)
#ifdef SYMBIAN_AUTH_SERVER
	  ,iUseNewKeyServer(EFalse)
#endif // SYMBIAN_AUTH_SERVER
	{
	LOG(_L("CFSKeyStoreClient::CFSKeyStoreClient: keystore client interface created"));
	}

void CFSKeyStoreClient::ConstructL()
	{
	CActiveScheduler::Add(this);

#ifdef SYMBIAN_AUTH_SERVER
	TPckg<TBool> idNewKeyServer(iUseNewKeyServer);
	iClient.SendRequest(EUseNewKeyServer, TIpcArgs(0,&idNewKeyServer));
	
	if(iUseNewKeyServer == EFalse)
#endif // SYMBIAN_AUTH_SERVER		
		{
		iAuthObject = CKeyStoreAuthObject::NewL(*this);
		iAuthObject->AddRef();
		}
	
	}

CFSKeyStoreClient::~CFSKeyStoreClient()
	{
	LOG(_L("CFSKeyStoreClient::~CFSKeyStoreClient"));
	LOG_INC_INDENT();
	
	Cleanup();
	if (iAuthObject)
		{
		iAuthObject->Release();
		}

	LOG_DEC_INDENT();
	}

MCTToken& CFSKeyStoreClient::Token()
	{
	return iToken;
	}

void CFSKeyStoreClient::AddRef()
	{
	++iRefCount;
	
	LOG1(_L("CFSKeyStoreClient::AddRef: keystore client interface referenced, refcount = %d"), iRefCount);
	LOG_INC_INDENT();

	iToken.AddRef();
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::DoRelease()
	{
	--iRefCount;

	LOG1(_L("CFSKeyStoreClient::DoRelease: keystore client interface released, refcount = %d"), iRefCount);
	LOG_INC_INDENT();
	
	ASSERT(iRefCount >= 0);
	if (iRefCount == 0)
		{
		MCTTokenInterface::DoRelease();
		}
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::ReleaseObject(const TCTTokenObjectHandle& aHandle)
	{
	iClient.SendRequest(ECloseObject, TIpcArgs(aHandle.iObjectId));
	}

void CFSKeyStoreClient::FreeKeyBuffer()
	{
	delete iKey; 
	iKey = NULL;
	}

TInt CFSKeyStoreClient::AllocKeyBuffer(TInt aReqdSize)
	{
	ASSERT(aReqdSize > 0);
	
	TInt result = KErrNoMemory;
	FreeKeyBuffer();
	iKey = HBufC8::NewMax(aReqdSize);
	if (iKey)
		{
		TPtr8 thePtr(iKey->Des());
		thePtr.FillZ();
		result = KErrNone;
		}

	return result;
}

//	*********************************************************************************
//	CActive implementation
//	*********************************************************************************

/**
 * Called when the server completes an asynchronous request.  Unmarshalls the
 * data from the server and passes it back to the caller.
 */
void CFSKeyStoreClient::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch (iCurrentRequest.OutstandingRequest())
		{
#ifdef SYMBIAN_AUTH_SERVER
		case ESetAuthenticationPolicy:
			break;
		case ECreateUserKey:
		case EImportUserKey:
		case EImportEncryptedUserKey:
#endif //SYMBIAN_AUTH_SERVER
		
		case ECreateKey:
		case EImportKey:
		case EImportEncryptedKey:
			// Unmarshal the created key data and create CCTKeyInfo to pass back to caller
			UpdateKey();
			break;

		case EExportKey:
		case EExportEncryptedKey:
		case EDHAgree:
			{
			*iClientPtr.iBuffer = HBufC8::NewL(iRequestPtr.Length());
			TPtr8 ptr((*iClientPtr.iBuffer)->Des());
			ptr.FillZ();
			ptr.Copy(iRequestPtr);
			break;
			}
			
		case ERepudiableDSASign:
			TokenDataMarshaller::ReadL(iRequestPtr, *iClientPtr.iDSASignature);
			break;
			
		case ERepudiableRSASign:
			TokenDataMarshaller::ReadL(iRequestPtr, *iClientPtr.iRSASignature);
			break;

		case EDecryptText:
			iClientPtr.iDecryptedText->Copy(iRequestPtr);		
			break;

		case EChangePassphrase:
		case EAuthOpen:
			// Nothing to do for these operations
			break;

		case EDHPublicKey:
			{
			ASSERT(iDHParams);
			RInteger n = iDHParams->TakeN();
			CleanupStack::PushL(n);
			RInteger g = iDHParams->TakeG();
			CleanupStack::PushL(g);
			RInteger X;
			TokenDataMarshaller::ReadL(iRequestPtr, X);
			CleanupStack::PushL(X);
			*iClientPtr.iDHPublicKey = CDHPublicKey::NewL(n, g, X);
			CleanupStack::Pop(3); // X, g, n
			break;
			}

		default:
			__ASSERT_DEBUG(EFalse, FSTokenPanic(EInvalidRequest));
		}
	Cleanup();
	iCurrentRequest.Complete(KErrNone);	
	}

/// Clean up internal state
void CFSKeyStoreClient::Cleanup()
	{
	FreeRequestBuffer();
	FreeKeyBuffer();
	
	delete iPbeParamsBuf;
	iPbeParamsBuf = NULL;	

	// Zero client pointers
	iClientPtr.iAny = NULL;
	
	delete iDHParams;
	iDHParams = NULL;
	
	delete iText;
	iText=NULL;
	}

TInt CFSKeyStoreClient::RunError(TInt aError)
	{
	Cleanup();
	return CFSClient::RunError(aError);
	}

void CFSKeyStoreClient::DoCancel()
	{
	TFSTokenMessages message = EIdle;

	switch (iCurrentRequest.OutstandingRequest())
		{
		case ECreateKey:
			message = ECancelCreateKey;
			break;

		case ERepudiableRSASign:
			message = ECancelRSASign;
			break;
			
		case ERepudiableDSASign:
			message = ECancelDSASign;
			break;
			
		case EDecryptText:
			message = ECancelDecrypt;
			break;
			
		case EDHPublicKey:
		case EDHAgree:
			message = ECancelDH;
			break;			

		case EImportKey:
			message = ECancelImportKey;
			break;

		case EImportEncryptedKey:
			message = ECancelImportEncryptedKey;
			break;

		case EExportKey:
			message = ECancelExportKey;
			break;
			
		case EExportEncryptedKey:
			message = ECancelExportEncryptedKey;
			break;
			
		case EChangePassphrase:
			message = ECancelChangePassphrase;
			break;

		case EAuthOpen:
			message = ECancelAuthOpen;
			break;

		default:
			// Nothing to do
			break;
		}

	if (message != EIdle)
		{
		iClient.SendRequest(message, TIpcArgs());
		}

	Cleanup();	
	CFSClient::DoCancel();	
	}

//	*********************************************************************************
//	MKeyStore
//	*********************************************************************************
void CFSKeyStoreClient::List(RMPointerArray<CCTKeyInfo>& aKeys, const TCTKeyAttributeFilter& aFilter, TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::List: list keys"));
	LOG_INC_INDENT();
	
	TKeyInfoArray<CCTKeyInfo> array(aKeys);
	TRAPD(err, DoListL(aFilter, array));
	FreeRequestBuffer();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::DoListL(const TCTKeyAttributeFilter& aFilter, MKeyInfoArray& aOut)
	{
	TInt startOfNew = aOut.Count();
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
#endif // SYMBIAN_AUTH_SERVER
		ASSERT(iAuthObject);
#ifdef SYMBIAN_AUTH_SERVER
		}
#endif // SYMBIAN_AUTH_SERVER
	
	TPckg<TCTKeyAttributeFilter> filterPckg(aFilter);
	SendSyncRequestAndHandleOverflowL(EListKeys, KInitialBufSizeList, TIpcArgs(&filterPckg, 0, &iRequestPtr));
   
	CleanupClosePushL(aOut);
	TokenDataMarshaller::ReadL(iRequestPtr, iToken, aOut);
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
#endif //SYMBIAN_AUTH_SERVER
		for (TInt index = startOfNew ; index < aOut.Count() ; ++index)
			{		
			iAuthObject->AddRef();
			LOG1(_L("CFSKeyStoreClient::DoListL: adding keyinfo %08x."), aOut[index]);
			aOut[index]->SetProtector(*iAuthObject);
			}
#ifdef SYMBIAN_AUTH_SERVER
		}
#endif // SYMBIAN_AUTH_SERVER
	CleanupStack::Pop(&aOut);
	}

void CFSKeyStoreClient::CancelList()
	{
	// synchronous, nothing to do
	}

void CFSKeyStoreClient::GetKeyInfo(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo, 
								   TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::GetKeyInfo: get key info"));
	LOG_INC_INDENT();
	
	TRAPD(err, DoGetKeyInfoL(aHandle, aInfo));
	LOG2(_L("Got key info %08x with protector %08x"), aInfo, aInfo->Protector());
	FreeRequestBuffer();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::DoGetKeyInfoL(TCTTokenObjectHandle aHandle, CCTKeyInfo*& aInfo)
	{
	SendSyncRequestAndHandleOverflowL(EGetKeyInfo, KInitialBufSizeList, TIpcArgs(aHandle.iObjectId, 0, &iRequestPtr));
	TokenDataMarshaller::ReadL(iRequestPtr, iToken, aInfo);
	}

void CFSKeyStoreClient::CancelGetKeyInfo()
	{
	// synchronous, nothing to do
	}

void CFSKeyStoreClient::Open(const TCTTokenObjectHandle& aHandle, 
							 MRSASigner*& aSigner, 
							 TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::Open: open rsa signer"));
	LOG_INC_INDENT();
	
	CRSARepudiableSigner* signer = CRSARepudiableSigner::New(this);
	
	TInt err = DoOpenKey(EOpenKeyRepudiableRSASign, aHandle, signer);
	if (err == KErrNone)
		{
		aSigner = signer;
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::Open(const TCTTokenObjectHandle& aHandle, 
							 MCTSigner<CDSASignature*>*& aSigner, 
							 TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::Open: open dsa signer"));
	LOG_INC_INDENT();
	
	CDSARepudiableSigner* signer = CDSARepudiableSigner::New(this);
	TInt err = DoOpenKey(EOpenKeyRepudiableDSASign, aHandle, signer);
	if (err == KErrNone)
		{
		aSigner = signer;
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::Open(const TCTTokenObjectHandle& aHandle, 
							 MCTDecryptor*& aDecryptor,
							 TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::Open: open rsa decryptor"));
	LOG_INC_INDENT();
	
	CFSRSADecryptor* decryptor = CFSRSADecryptor::New(this);
	TInt err = DoOpenKey(EOpenKeyDecrypt, aHandle, decryptor);
	if (err == KErrNone)
		{
		aDecryptor = decryptor;
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
	LOG_DEC_INDENT();
	}

void CFSKeyStoreClient::Open(const TCTTokenObjectHandle& aHandle, 
							 MCTDH*& aDH, TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::Open: open dh agreement"));
	LOG_INC_INDENT();
	
	CDHAgreement* dh = CDHAgreement::New(this);
	TInt err = DoOpenKey(EOpenKeyAgree, aHandle, dh);
	if (err == KErrNone)
		{
		aDH = dh;
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
	LOG_DEC_INDENT();
	}

TInt CFSKeyStoreClient::DoOpenKey(TFSTokenMessages aMessage,
								  const TCTTokenObjectHandle& aHandle,
								  COpenedKey* aOpenedKey)
	{
	if (!aOpenedKey)
		{
		return KErrNoMemory;
		}
	
	// This is similar to SendSyncRequestAndHandleOverflowL, but we're returning data in iLabel
	TInt handle;
	TPckg<TInt> handlePckg(handle);

	HBufC* label = HBufC::NewMax(KInitialBufSizeOpen);
	if (!label)
		{
		aOpenedKey->Release();
		return KErrNoMemory;
		}
	
	TPtr labelPtr(label->Des());
	
	TIpcArgs args(aHandle.iObjectId, &handlePckg, 0, &labelPtr);
	
	TInt err = iClient.SendRequest(aMessage, args);
	if (err == KErrOverflow)
		{
		TInt sizeReqd = 0;
		TPckg<TInt> theSize(sizeReqd);
		TPtrC8 sizePtr(reinterpret_cast<const TUint8*>(label->Ptr()), 4);
		theSize.Copy(sizePtr);	

		delete label;
		label = HBufC::NewMax(sizeReqd);
		if (label)
			{
			labelPtr.Set(label->Des());		
			err = iClient.SendRequest(aMessage, args);
			}
		else
			{
			err = KErrNoMemory;
			}
		}

	if (err != KErrNone)
		{
		aOpenedKey->Release();
		delete label;
		return err;
		}
	
	aOpenedKey->SetObjectID(handle);
	aOpenedKey->SetLabel(label);
	return KErrNone;
	}

void CFSKeyStoreClient::CancelOpen()
	{
	// synchronous, nothing to do
	}

TUint CFSKeyStoreClient::GetKeySizeL(TInt aObjectId)
	{
	// Get the key size
	TInt result = iClient.SendRequest(EGetKeyLength, TIpcArgs(aObjectId, &iRequestPtr));
	User::LeaveIfError(result);
	return result;
	}

/** Returns the public key in DER-encoded ASN-1 */
void CFSKeyStoreClient::ExportPublic(const TCTTokenObjectHandle& aHandle,
									 HBufC8*& aPublicKey,
									 TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::ExportPublic: export public"));
	
	TRAPD(err, DoExportPublicL(aHandle, aPublicKey));
	FreeRequestBuffer();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::DoExportPublicL(const TCTTokenObjectHandle& aHandle, HBufC8*& aPublicKey)
	{
	// The size of the exported package is always less than keysize in bytes * 4 
	// DSA generates packages twice as big as RSA
	TUint bufSize = GetKeySizeL(aHandle.iObjectId) / 2;
	
	SendSyncRequestAndHandleOverflowL(EExportPublic, bufSize, TIpcArgs(aHandle.iObjectId, &iRequestPtr));
	
	aPublicKey = HBufC8::NewL(iRequestPtr.MaxLength());
	TPtr8 ptr = aPublicKey->Des();
	ptr.Copy(iRequestPtr);
	}	

void CFSKeyStoreClient::CancelExportPublic()
	{
	// synchronous, nothing to do
	}

//	*********************************************************************************
//	MCTKeyStoreManager
//	*********************************************************************************
void CFSKeyStoreClient::CreateKey(CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::CreateKey: create key (starting)"));

	//	[in, out] CCTKeyInfo, caller fills with data required to create the key, 
	//	If request succeeds, iId and iHandle members are filled in by server

	TInt r = MarshalKeyInfo(*aReturnedKey);
	if (KErrNone!=r)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}

	//	Store CCTKeyInfo to write into later (server will put extra data into it)
	iClientPtr.iKeyInfo = &aReturnedKey;
	SetActive();
	iCurrentRequest(ECreateKey, &aStatus);
	iClient.SendAsyncRequest(ECreateKey, TIpcArgs(0, &iRequestPtr), &iStatus);		
	}

void CFSKeyStoreClient::CancelCreateKey()
	{
	if (iCurrentRequest.OutstandingRequest() == ECreateKey)
		{
		Cancel();
		}
	}

void CFSKeyStoreClient::ImportKey(const TDesC8& aKey, CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus)
	{
	DoImportKey(EImportKey, aKey, aReturnedKey, aStatus);
	}

void CFSKeyStoreClient::CancelImportKey()
	{
	if (iCurrentRequest.OutstandingRequest() == EImportKey)
		{
		Cancel();
		}
	}

void CFSKeyStoreClient::ImportEncryptedKey(const TDesC8& aKey, CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus)
	{
	DoImportKey(EImportEncryptedKey, aKey, aReturnedKey, aStatus);
	}

void CFSKeyStoreClient::CancelImportEncryptedKey()
	{
	if (iCurrentRequest.OutstandingRequest() == EImportEncryptedKey)
		{
		Cancel();
		}
	}

void CFSKeyStoreClient::DoImportKey(TFSTokenMessages aMessage, const TDesC8& aKey, CCTKeyInfo*& aReturnedKey, TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::DoImportKey: import key (starting)"));
	
	// [in] CCTKeyInfo, caller fills with data required to create the key,
	// [out] TKeyUpdate, key iId and iHandle members from the server
	TInt r = AllocKeyBuffer(aKey.Size());
	if (KErrNone!=r)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}

	TPtr8 keyPtr(iKey->Des());
	keyPtr.Copy(aKey);
	
	r = MarshalKeyInfo(*aReturnedKey);
	if (KErrNone!=r)
		{
		FreeKeyBuffer();
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}

	iClientPtr.iKeyInfo = &aReturnedKey;
	SetActive();
	iCurrentRequest(aMessage, &aStatus);
	
	iClient.SendAsyncRequest(aMessage, TIpcArgs(iKey, &iRequestPtr), &iStatus);
	
	}

void CFSKeyStoreClient::DoImportUserKey(TFSTokenMessages aMessage, 
										const TDesC8& aKey, 
										CCTKeyInfo*& aReturnedKey,
										const TDesC& aAuthExpression,
										TInt aFreshness,
										TRequestStatus& aStatus)
	{
	LOG(_L("CFSKeyStoreClient::DoImportUserKey: import key (starting)"));
	
	// [in] CCTKeyInfo, caller fills with data required to create the key,
	// [out] TKeyUpdate, key iId and iHandle members from the server
	TInt r = AllocKeyBuffer(aKey.Size());
	if (KErrNone!=r)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}

	TPtr8 keyPtr(iKey->Des());
	keyPtr.Copy(aKey);
	
	r = MarshalKeyInfo(*aReturnedKey);
	if (KErrNone!=r)
		{
		FreeKeyBuffer();
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}

	iClientPtr.iKeyInfo = &aReturnedKey;
	SetActive();
	iCurrentRequest(aMessage, &aStatus);
	
	iClient.SendAsyncRequest(aMessage, TIpcArgs(iKey, &iRequestPtr, &aAuthExpression, aFreshness), &iStatus);
	
	}

TInt CFSKeyStoreClient::MarshalKeyInfo(CCTKeyInfo& aKey)
	{
	TInt r = AllocRequestBuffer(TokenDataMarshaller::Size(aKey));
	if (KErrNone==r)	//	OOM or some catastrophe, no point continuing
		{
		TokenDataMarshaller::Write(aKey, iRequestPtr);
		}
	
	return (r);
	}

void CFSKeyStoreClient::ExportKey(TCTTokenObjectHandle aHandle, HBufC8*& aKey, TRequestStatus& aStatus)
{
	TUint keySize=0;
	TRAPD(r, keySize = GetKeySizeL(aHandle.iObjectId));
	if (KErrNone!=r)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;	
		}

	ASSERT(keySize);	

	iClientPtr.iBuffer = &aKey;
	iObjectId = aHandle.iObjectId;

	 r = AllocRequestBuffer((keySize/8)*6);   // Heuristic, pkcs8 cleartext format is about 6 times the keysize in bytes...

	if (KErrNone!=r)	//	OOM or some catastrophe, no point continuing
	{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);	
	}
	else
	{			
		SetActive();

		iCurrentRequest(EExportKey, &aStatus);
		
		iClient.SendAsyncRequest(iCurrentRequest.OutstandingRequest(), TIpcArgs(iObjectId,  &iRequestPtr), &iStatus);		
	}
}

void CFSKeyStoreClient::CancelExportKey()
	{
	if (iCurrentRequest.OutstandingRequest() == EExportKey)
		{
		Cancel();
		}
	}

void CFSKeyStoreClient::ExportEncryptedKey(TCTTokenObjectHandle aHandle, const CPBEncryptParms& aParams,  HBufC8*& aKey, TRequestStatus& aStatus)
 	{
	TUint keySize=0;
	TRAPD(r, keySize = GetKeySizeL(aHandle.iObjectId));
	if (KErrNone!=r)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;	
		}
	
	ASSERT(keySize);	
	
	iClientPtr.iBuffer = &aKey;
	iObjectId = aHandle.iObjectId;

	r = AllocRequestBuffer((keySize / 8) * 7);  // Heuristic, pkcs8 encrypted format is about 7 times the keysize in bytes...

	if (KErrNone!=r)	//	OOM or some catastrophe, no point continuing
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);	
		return;
		}
		
	iPbeParamsBuf = HBufC8::NewMax(TokenDataMarshaller::Size(aParams));
	if (!iPbeParamsBuf)
		{
		FreeRequestBuffer();
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNoMemory);
		return;	
		}
	
	TPtr8 pbeParamsPtr(iPbeParamsBuf->Des());
	pbeParamsPtr.FillZ();
	TokenDataMarshaller::Write(aParams, pbeParamsPtr);
	
	SetActive();
	iCurrentRequest(EExportEncryptedKey, &aStatus);

	// Marshall encryption parameters	
	iClient.SendAsyncRequest(iCurrentRequest.OutstandingRequest(), TIpcArgs(iObjectId,  &iRequestPtr, iPbeParamsBuf), &iStatus);	
 	}

void CFSKeyStoreClient::CancelExportEncryptedKey()
	{
	if (iCurrentRequest.OutstandingRequest() == EExportEncryptedKey)
		{
		Cancel();
		}
	}

void CFSKeyStoreClient::RepudiableDSASign(const TCTTokenObjectHandle& aHandle,
										  const TDesC8& aDigest,
										  CDSASignature*& aSignature,
										  TRequestStatus& aStatus)
{	
	if (aDigest.Length() > KMaxDSADigestSize)
	{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrOverflow);	
	}
	else
	{//	Calls the same DSA digest function server side for DSA
		iClientPtr.iDSASignature = &aSignature;	
		Process(aHandle,aDigest,ERepudiableDSASign,aStatus,KMaxDSASignatureSize);
	}
}

void CFSKeyStoreClient::CancelRepudiableDSASign()
	{
	if (iCurrentRequest.OutstandingRequest() == ERepudiableDSASign)
		{
		Cancel();
		}	
	}

void CFSKeyStoreClient::RepudiableRSASign(const TCTTokenObjectHandle& aHandle,
										  const TDesC8& aDigest,
										  CRSASignature*& aSignature,
										  TRequestStatus& aStatus)
{
	if (aDigest.Length() > KMaxRSADigestSize)
	{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrOverflow);	
	}
	else
	{
		iClientPtr.iRSASignature = &aSignature;	
		Process(aHandle,aDigest,ERepudiableRSASign,aStatus,KMaxRSASignatureSize);
	}
}

void CFSKeyStoreClient::CancelRepudiableRSASign()
	{
	if (iCurrentRequest.OutstandingRequest() == ERepudiableRSASign)
		{
		Cancel();
		}	
	}

void CFSKeyStoreClient::Process(const TCTTokenObjectHandle& aHandle,
								const TDesC8& aText,
								TFSTokenMessages aMessage,
								TRequestStatus& aStatus, TInt aBuffSize)
{
	TInt err = AllocRequestBuffer(aBuffSize);	//Assuming the buf is large enough
	if (KErrNone!=err)	//	OOM or some catastrophe, no point continuing
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, err);
		return;
		}
		
	iObjectId = aHandle.iObjectId;
		
	iText = aText.Alloc();
	if (iText==NULL)
		{
		FreeRequestBuffer();
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNoMemory);
		return;
		}

	SetActive();
	iCurrentRequest(aMessage, &aStatus);

	// can use same buffer for in and out?
	iClient.SendAsyncRequest(aMessage, TIpcArgs(iObjectId, iText, &iRequestPtr), &iStatus);
}

void CFSKeyStoreClient::Decrypt(const TCTTokenObjectHandle& aHandle,
								const TDesC8& aCiphertext,
								TDes8& aDecryptedText,
								TRequestStatus& aStatus)
{
	iClientPtr.iDecryptedText = &aDecryptedText;
	Process(aHandle,aCiphertext,EDecryptText,aStatus, KMaxRSAPlaintextSize);
}

void CFSKeyStoreClient::CancelDecrypt()
	{
	if (iCurrentRequest.OutstandingRequest() == EDecryptText)
		{
		Cancel();
		}	
	}

void CFSKeyStoreClient::DHPublicKey(const TCTTokenObjectHandle& aHandle, const TInteger& aN, const TInteger& aG,
									CDHPublicKey*& aX, TRequestStatus& aStatus)
	{
	TRAPD(err, DoDHPublicKeyL(aHandle, aN, aG, aX, aStatus));
	if (err != KErrNone)
		{
		delete iDHParams;
		iDHParams = NULL;
		FreeRequestBuffer();
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

void CFSKeyStoreClient::DoDHPublicKeyL(const TCTTokenObjectHandle& aHandle, const TInteger& aN, const TInteger& aG,
									   CDHPublicKey*& aX, TRequestStatus& aStatus)
	{
	iDHParams = CDHParams::NewL(aN, aG);
	
	TInt sizeReqd = TokenDataMarshaller::Size(*iDHParams);
	User::LeaveIfError(AllocRequestBuffer(sizeReqd));

	TokenDataMarshaller::WriteL(*iDHParams, iRequestPtr);
	
	iObjectId = aHandle.iObjectId;
	iClientPtr.iDHPublicKey = &aX;
	iCurrentRequest(EDHPublicKey, &aStatus);
	iClient.SendAsyncRequest(EDHPublicKey, TIpcArgs(iObjectId, &iRequestPtr), &iStatus);	
	SetActive();
	}

void CFSKeyStoreClient::DHAgree(const TCTTokenObjectHandle& aHandle, const CDHPublicKey& iY,
								HBufC8*& aAgreedKey, TRequestStatus& aStatus)
	{	
	TInt sizeReqd = TokenDataMarshaller::Size(iY);
	TInt err = AllocRequestBuffer(sizeReqd);
	if (err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		return;
		}

	TRAP(err, TokenDataMarshaller::WriteL(iY, iRequestPtr));
	if (err != KErrNone)
		{
		FreeRequestBuffer();
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		return;		
		}
	
	iObjectId = aHandle.iObjectId;
	iClientPtr.iBuffer = &aAgreedKey;
	iCurrentRequest(EDHAgree, &aStatus);
	iClient.SendAsyncRequest(EDHAgree, TIpcArgs(iObjectId, &iRequestPtr), &iStatus);	
	SetActive();
	}

void CFSKeyStoreClient::CancelDH()
	{
	if (iCurrentRequest.OutstandingRequest() == EDHPublicKey ||
		iCurrentRequest.OutstandingRequest() == EDHAgree)
		{
		Cancel();
		}	
	}

void CFSKeyStoreClient::DeleteKey(TCTTokenObjectHandle aHandle, TRequestStatus& aStatus)
	{
	TInt err = iClient.SendRequest(EDeleteKey, TIpcArgs(aHandle.iObjectId));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::CancelDeleteKey()
	{
	// synchronous, nothing to do
	}

void CFSKeyStoreClient::SetUsePolicy(TCTTokenObjectHandle aHandle, 
									 const TSecurityPolicy& aPolicy,
									 TRequestStatus& aStatus)
	{
	TPckgC<TSecurityPolicy> policyPckg(aPolicy);
	TInt err = iClient.SendRequest(ESetUsePolicy, TIpcArgs(aHandle.iObjectId, &policyPckg));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::CancelSetUsePolicy()
	{
	// synchronous, nothing to do
	}

void CFSKeyStoreClient::SetManagementPolicy(TCTTokenObjectHandle aHandle, 
											const TSecurityPolicy& aPolicy,
											TRequestStatus& aStatus)
	{
	TPckgC<TSecurityPolicy> policyPckg(aPolicy);
	TInt err = iClient.SendRequest(ESetManagementPolicy, TIpcArgs(aHandle.iObjectId, &policyPckg));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::CancelSetManagementPolicy()
	{
	// synchronous, nothing to do
	}

void CFSKeyStoreClient::SetPassphraseTimeout(TInt aTimeout, TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif // SYMBIAN_AUTH_SERVER
	SetTimeout(aTimeout, aStatus);
	}

void CFSKeyStoreClient::CancelSetPassphraseTimeout()
	{
	// No point cancelling, not asynchronous
	}

void CFSKeyStoreClient::Relock(TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif // SYMBIAN_AUTH_SERVER
	TInt err = iClient.SendRequest(ERelock, TIpcArgs());
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::CancelRelock()
	{
	// No point cancelling, not asynchronous
	}

void CFSKeyStoreClient::UpdateKey()
	{
	LOG(_L("CFSKeyStoreClient::UpdateKey: create/import key (completing)"));
	LOG_INC_INDENT();
	
	__ASSERT_DEBUG((iRequestPtr.Ptr()), FSTokenPanic(ENoDataMarshalled));
	CCTKeyInfo& keyInfo = **iClientPtr.iKeyInfo;

	// Set handle and key id returned from server
	TKeyUpdate update;
	TPckg<TKeyUpdate> pckg(update);
	pckg.Copy(iRequestPtr);
	keyInfo.SetHandle(update.iReference);
	keyInfo.SetIdentifier(update.iId);
	// Set size and algorithm - only strictly necessary for import
	keyInfo.SetSize(update.iSize);
	keyInfo.SetAlgorithm(update.iAlgorithm);
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
		ASSERT(iAuthObject);
		iAuthObject->AddRef();
		keyInfo.SetProtector(*iAuthObject);
		}
#endif // SYMBIAN_AUTH_SERVER
	LOG_DEC_INDENT();	
	}

// Implementation of auth object functionality

void CFSKeyStoreClient::ListProtectedObjects(RMPointerArray<MCTTokenObject>& aObjects, TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	TCTKeyAttributeFilter filter;
	filter.iPolicyFilter = TCTKeyAttributeFilter::EUsableOrManageableKeys;
	TKeyInfoArray<MCTTokenObject> array(aObjects);
	TRAPD(err, DoListL(filter, array));
	FreeRequestBuffer();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::ChangeReferenceData(TRequestStatus &aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	iCurrentRequest(EChangePassphrase, &aStatus);
	SetActive();
	iClient.SendAsyncRequest(EChangePassphrase, TIpcArgs(), &iStatus);
	}

void CFSKeyStoreClient::CancelChangeReferenceData()
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	if (iCurrentRequest.OutstandingRequest() == EChangePassphrase)
		{
		Cancel();
		}
	}

TUint32 CFSKeyStoreClient::AuthStatus() const
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		return 0;
		}
#endif // SYMBIAN_AUTH_SERVER
	
	return EEnabled | EUnblockDisabled;
	}

void CFSKeyStoreClient::AuthOpen(TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif // SYMBIAN_AUTH_SERVER
	iCurrentRequest(EAuthOpen, &aStatus);
	SetActive();
	iClient.SendAsyncRequest(EAuthOpen, TIpcArgs(), &iStatus);	
	}

void CFSKeyStoreClient::CancelAuthOpen()
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		return;
		}
#endif // SYMBIAN_AUTH_SERVER
	if (iCurrentRequest.OutstandingRequest() == EAuthOpen)
		{
		Cancel();
		}
	}

void CFSKeyStoreClient::AuthClose(TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	TInt err = iClient.SendRequest(EAuthClose, TIpcArgs());
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::TimeRemaining(TInt& aTime, TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	TInt result = iClient.SendRequest(EAuthTimeRemaining, TIpcArgs());
	if (result >= 0)
		{
		aTime = result;
		result = KErrNone;
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, result);
	}

void CFSKeyStoreClient::SetTimeout(TInt aTime, TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	TInt err = iClient.SendRequest(ESetTimeout, TIpcArgs(0, aTime));	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSKeyStoreClient::Timeout(TInt& aTime, TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	TInt result = iClient.SendRequest(EGetTimeout, TIpcArgs());	
	if (result >= 0)
		{
		aTime = result;
		result = KErrNone;
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, result);
	}

#ifdef SYMBIAN_AUTH_SERVER

void CFSKeyStoreClient::CreateKey(	const TDesC& aAuthenticationString, 
									TInt aFreshness,
									CCTKeyInfo*& aReturnedKey,
									TRequestStatus& aStatus )
	{
	LOG(_L("CFSKeyStoreClient::CreateKey: create key (starting)"));
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	//	[in, out] CCTKeyInfo, caller fills with data required to create the key, 
	//	If request succeeds, iId and iHandle members are filled in by server

	TInt r = MarshalKeyInfo(*aReturnedKey);
	if (KErrNone!=r)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}

	//	Store CCTKeyInfo to write into later (server will put extra data into it)
	iClientPtr.iKeyInfo = &aReturnedKey;
	SetActive();
	iCurrentRequest(ECreateUserKey, &aStatus);
	
	iClient.SendAsyncRequest(ECreateUserKey, TIpcArgs(0, &iRequestPtr, &aAuthenticationString, aFreshness), &iStatus);
	
	}

void CFSKeyStoreClient::ImportKey( const TDesC8& aKey, 
							const TDesC& aAuthenticationString, 
							TInt aFreshness, CCTKeyInfo*& aReturnedKey, 
							TRequestStatus& aStatus )
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	DoImportUserKey(EImportUserKey, aKey, aReturnedKey, aAuthenticationString, aFreshness, aStatus);
	}
			

void CFSKeyStoreClient::ImportEncryptedKey(const TDesC8& aKey, 
									const TDesC& aAuthenticationString, 
									TInt aFreshness, CCTKeyInfo*& aReturnedKey, 
									TRequestStatus& aStatus )
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
#endif //SYMBIAN_AUTH_SERVER
	DoImportUserKey(EImportEncryptedUserKey, aKey, aReturnedKey, aAuthenticationString, aFreshness, aStatus);
	
	}

void CFSKeyStoreClient::SetAuthenticationPolicy(	
		const TCTTokenObjectHandle aHandle,
		const TDesC& aAuthenticationString,
		TInt aFreshness,
		TRequestStatus& aStatus)
		{
#ifdef SYMBIAN_AUTH_SERVER
		if(iUseNewKeyServer == EFalse)
			{
			TRequestStatus* stat = &aStatus;
			User::RequestComplete(stat, KErrNotSupported);
			return;
			}
		iCurrentRequest(ESetAuthenticationPolicy, &aStatus);
		SetActive();
		iClient.SendAsyncRequest(ESetAuthenticationPolicy, TIpcArgs(aHandle.iObjectId, &aAuthenticationString, aFreshness), &iStatus);
			
#endif //SYMBIAN_AUTH_SERVER
		}

void CFSKeyStoreClient::GetAuthenticationPolicy( const TCTTokenObjectHandle aHandle,
												HBufC*& aAuthenticationString,
												TInt& aFreshness,
												TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_AUTH_SERVER
	if(iUseNewKeyServer == EFalse)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotSupported);
		return;
		}
	iAuthExpression = aAuthenticationString;
	iFreshness = aFreshness;
	TPckg<TInt> freshness(aFreshness);
	TPtr authPtr = iAuthExpression->Des();
	TInt err = iClient.SendRequest(EGetAuthenticationPolicy, TIpcArgs(aHandle.iObjectId, &authPtr, &freshness));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	
#endif //SYMBIAN_AUTH_SERVER
	
	
	}
#endif // SYMBIAN_AUTH_SERVER
