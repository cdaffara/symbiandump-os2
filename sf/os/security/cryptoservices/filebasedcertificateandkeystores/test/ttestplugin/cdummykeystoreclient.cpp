/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cdummykeystoreclient.h"

MCTTokenInterface* CDummyKeyStoreClient::NewKeyStoreInterfaceL(MCTToken& aToken)
	{
	//	Destroyed by MCTTokenInterface::DoRelease() 
	CDummyKeyStoreClient* me = new (ELeave) CDummyKeyStoreClient(aToken);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

CDummyKeyStoreClient::CDummyKeyStoreClient(MCTToken& aToken)
	: // Reference count starts at one as we're always created and returned by a GetInterface() call on the token
	  iRefCount(1),
	  iToken(aToken)
	{
	}

void CDummyKeyStoreClient::ConstructL()
	{
	}

CDummyKeyStoreClient::~CDummyKeyStoreClient()
	{
	}

MCTToken& CDummyKeyStoreClient::Token()
	{
	return iToken;
	}

void CDummyKeyStoreClient::AddRef()
	{
	++iRefCount;
	
	iToken.AddRef();
	}

void CDummyKeyStoreClient::DoRelease()
	{
	--iRefCount;

	ASSERT(iRefCount >= 0);
	if (iRefCount == 0)
		{
		MCTTokenInterface::DoRelease();
		}
	}

void CDummyKeyStoreClient::ReleaseObject(const TCTTokenObjectHandle& /*aHandle*/)
	{
	}

//	*********************************************************************************
//	MKeyStore
//	*********************************************************************************
void CDummyKeyStoreClient::List(RMPointerArray<CCTKeyInfo>& /*aKeys*/, const TCTKeyAttributeFilter& /*aFilter*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelList()
	{
	// synchronous, nothing to do
	}

void CDummyKeyStoreClient::GetKeyInfo(TCTTokenObjectHandle /*aHandle*/, CCTKeyInfo*& /*aInfo*/, 
								   TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelGetKeyInfo()
	{
	// synchronous, nothing to do
	}

void CDummyKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MRSASigner*& /*aSigner*/, 
							 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MCTSigner<CDSASignature*>*& /*aSigner*/, 
							 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MCTDecryptor*& /*aDecryptor*/,
							 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::Open(const TCTTokenObjectHandle& /*aHandle*/, 
							 MCTDH*& /*aDH*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelOpen()
	{
	// synchronous, nothing to do
	}

/** Returns the public key in DER-encoded ASN-1 */
void CDummyKeyStoreClient::ExportPublic(const TCTTokenObjectHandle& /*aHandle*/,
									 HBufC8*& /*aPublicKey*/,
									 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelExportPublic()
	{
	// synchronous, nothing to do
	}

//	*********************************************************************************
//	MCTKeyStoreManager
//	*********************************************************************************
void CDummyKeyStoreClient::CreateKey(CCTKeyInfo*& /*aReturnedKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelCreateKey()
	{
	}

void CDummyKeyStoreClient::ImportKey(const TDesC8& /*aKey*/, CCTKeyInfo*& /*aReturnedKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	//DoImportKey(EImportKey, aKey, aReturnedKey, aStatus);
	}

void CDummyKeyStoreClient::CancelImportKey()
	{
	}

void CDummyKeyStoreClient::ImportEncryptedKey(const TDesC8& /*aKey*/, CCTKeyInfo*& /*aReturnedKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	//DoImportKey(EImportEncryptedKey, aKey, aReturnedKey, aStatus);
	}

void CDummyKeyStoreClient::CancelImportEncryptedKey()
	{
	}

void CDummyKeyStoreClient::ExportKey(TCTTokenObjectHandle /*aHandle*/, HBufC8*& /*aKey*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelExportKey()
	{
	}

void CDummyKeyStoreClient::ExportEncryptedKey(TCTTokenObjectHandle /*aHandle*/, const CPBEncryptParms& /*aParams*/,  HBufC8*& /*aKey*/, TRequestStatus& aStatus)
 	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
 	}

void CDummyKeyStoreClient::CancelExportEncryptedKey()
	{
	}

void CDummyKeyStoreClient::RepudiableDSASign(const TCTTokenObjectHandle& /*aHandle*/,
										  const TDesC8& /*aDigest*/,
										  CDSASignature*& /*aSignature*/,
										  TRequestStatus& aStatus)
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::RepudiableRSASign(const TCTTokenObjectHandle& /*aHandle*/,
										  const TDesC8& /*aDigest*/,
										  CRSASignature*& /*aSignature*/,
										  TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::Decrypt(const TCTTokenObjectHandle& /*aHandle*/,
								const TDesC8& /*aCiphertext*/,
								TDes8& /*aDecryptedText*/,
								TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::DHPublicKey(const TCTTokenObjectHandle& /*aHandle*/, const TInteger& /*aN*/, const TInteger& /*aG*/,
									CDHPublicKey*& /*aX*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::DHAgree(const TCTTokenObjectHandle& /*aHandle*/, const CDHPublicKey& /*iY*/,
								HBufC8*& /*aAgreedKey*/, TRequestStatus& aStatus)
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::DeleteKey(TCTTokenObjectHandle /*aHandle*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelDeleteKey()
	{
	// synchronous, nothing to do
	}

void CDummyKeyStoreClient::SetUsePolicy(TCTTokenObjectHandle /*aHandle*/, 
									 const TSecurityPolicy& /*aPolicy*/,
									 TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelSetUsePolicy()
	{
	// synchronous, nothing to do
	}

void CDummyKeyStoreClient::SetManagementPolicy(TCTTokenObjectHandle /*aHandle*/, 
											const TSecurityPolicy& /*aPolicy*/,
											TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelSetManagementPolicy()
	{
	// synchronous, nothing to do
	}

void CDummyKeyStoreClient::SetPassphraseTimeout(TInt /*aTimeout*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelSetPassphraseTimeout()
	{
	// No point cancelling, not asynchronous
	}

void CDummyKeyStoreClient::Relock(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CDummyKeyStoreClient::CancelRelock()
	{
	// No point cancelling, not asynchronous
	}





