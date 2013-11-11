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


#include <mctkeystore.h>
#include <f32file.h>
#include <s32mem.h>

#include <authserver/authtypes.h>
#include <centralrepository.h>
#include <authserver/authclient.h>
#include <authserver/authexpression.h>
#include <authserver/auth_srv_errs.h>
#include <authserver/aspubsubdefs.h>
#include <authserver/authtypes.h>
#include <pbedata.h>
#include <mctkeystore.h>
#include <keystore_errs.h>
#include <securityerr.h>
#include <keytool.rsg>

#include "keystorecenrepconfig.h"
#include "fsdatatypes.h"
#include "ckeydatamanager.h"
#include "keystorepassphrase.h"
#include "keystreamutils.h"
#include "asymmetrickeys.h"
#include "keytool_migratestore.inl"

#include "keytool_utils.h"
#include "keytool_commands.h"
#include "keytool_controller.h"


/*static*/ CKeytoolMigrateStore* CKeytoolMigrateStore::NewLC(CKeyToolParameters* aParams)
	{
	CKeytoolMigrateStore* self = new (ELeave) CKeytoolMigrateStore();
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}
	
/*static*/ CKeytoolMigrateStore* CKeytoolMigrateStore::NewL(CKeyToolParameters* aParams)
	{
	CKeytoolMigrateStore* self = CKeytoolMigrateStore::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

CKeytoolMigrateStore::CKeytoolMigrateStore()
:CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);
	}
	
CKeytoolMigrateStore::~CKeytoolMigrateStore()
	{	
	delete iWriteFileStore;
	delete iReadFileStore;
	delete iKeyStoreCenrep;
	delete iPassphrase;
	delete iUserIdentity;
	iKeyList.ResetAndDestroy();
	iFs.Close();
	iAuthClient.Close();
	}

void CKeytoolMigrateStore::ConstructL(CKeyToolParameters* aParams)
	{
	iParams = aParams;
	
	// retrieve the value of authexpression and freshness from the cenrep 
	// file if not supplied in the command line.
	iKeyStoreCenrep = CKeyStoreCenrep::NewL();
	if(iParams->iAuthExpression == NULL)
		{
		iParams->iAuthExpression = HBufC::NewL(1024);
		TPtr exprPtr = iParams->iAuthExpression->Des();
		iKeyStoreCenrep->AuthExpressionL( exprPtr );
		}
	if(iParams->iFreshness == -1)
		{
		iParams->iFreshness = iKeyStoreCenrep->FreshnessL();
		}
	
	User::LeaveIfError(iFs.Connect());
	
	// open the file store to be created with the latest details.
	iWriteFileStore = CPermanentFileStore::ReplaceL(iFs, *iParams->iNewKeyFile, EFileRead | EFileWrite | EFileShareExclusive);
	iWriteFileStore->SetTypeL(KPermanentFileStoreLayoutUid);

	RStoreWriteStream writeLookupStream;
	iWriteLookupStreamId = writeLookupStream.CreateLC(*iWriteFileStore);
	writeLookupStream.WriteUint32L(0); // Write key count of zero
	writeLookupStream.CommitL();
	CleanupStack::PopAndDestroy(&writeLookupStream);
	
	// Create root stream - just contains id of info stream
	RStoreWriteStream writeRootStream;
	TStreamId writeRootStreamId = writeRootStream.CreateLC(*iWriteFileStore);
	iWriteFileStore->SetRootL(writeRootStreamId);
		
	writeRootStream.WriteUint32L(iWriteLookupStreamId.Value());		
	writeRootStream.CommitL();
	CleanupStack::PopAndDestroy(&writeRootStream);
	
	// open the already existing key file.
	// Make sure the file isn't write protected
	User::LeaveIfError(iFs.SetAtt(*iParams->iOldKeyFile, 0, KEntryAttReadOnly));
	
	RFile readFile;
	CleanupClosePushL(readFile);
	User::LeaveIfError(readFile.Open(iFs, *iParams->iOldKeyFile, EFileRead | EFileWrite | EFileShareExclusive));
	iReadFileStore = CPermanentFileStore::FromL(readFile);		
	CleanupStack::PopAndDestroy(&readFile);
	
	TStreamId readRootStreamId = iReadFileStore->Root();
	if (readRootStreamId == KNullStreamId)
		{
		User::Leave(KErrCorrupt);
		}
	
	RStoreReadStream readRootStream;
	readRootStream.OpenLC(*iReadFileStore, readRootStreamId);
	TStreamId readLookupStreamId = (TStreamId)(readRootStream.ReadUint32L());
	CleanupStack::PopAndDestroy(&readRootStream);
	
	RStoreReadStream readLookupStream;
	readLookupStream.OpenLC(*iReadFileStore, readLookupStreamId);
	
	// ignore passStreamId and timeoutStreamId for migration
	TStreamId passStreamId = readLookupStream.ReadUint32L();
	TStreamId timeoutStreamId = readLookupStream.ReadUint32L();
	
	TInt keyCount = readLookupStream.ReadInt32L();
	
	// read the keys that are stored in this store
	for (TInt index = 0; index < keyCount; index++)
		{
		const CFileKeyData* keyData = CFileKeyData::CreateOldKeyL(readLookupStream);
		iKeyList.Append(keyData);
		}
		
	CleanupStack::PopAndDestroy(&readLookupStream);
	
	RStoreReadStream timeoutStream;
	timeoutStream.OpenLC(*iReadFileStore, timeoutStreamId);
	TInt timeout = timeoutStream.ReadInt32L();
	CleanupStack::PopAndDestroy(&timeoutStream);
	
	iPassphrase = CPassphrase::NewL( timeout, *iReadFileStore, 
									  passStreamId, *iParams->iPassphrase);
	
	User::LeaveIfError(iAuthClient.Connect());
	
	}
	
void CKeytoolMigrateStore::DoCommandL()
	{	
	if(iParams->iOldKeyFile == NULL ||
		iParams->iPassphrase == NULL )
		{
		User::Leave(CKeyToolParameters::EMandatoryArgumentMissing);
		}
			
	TUid uid = TUid::Uid(0);
	AuthServer::CAuthExpression* expression = iAuthClient.CreateAuthExpressionL(*(iParams->iAuthExpression));
	CleanupStack::PushL(expression);
	iState = EAfterAuthentication;
	SetActive();
	iAuthClient.AuthenticateL(*expression,iParams->iFreshness, EFalse, uid, EFalse, KNullDesC, iUserIdentity, iStatus);
	
	CleanupStack::PopAndDestroy(expression); 
		
	}

void CKeytoolMigrateStore::CleanKeyInfo(TAny* aKeyInfo)
	{
	delete reinterpret_cast<CKeyInfo*>(aKeyInfo);
	}

void CKeytoolMigrateStore::WriteKeyInformationL()
	{
	
	TInt keysCount = iKeyList.Count();
	
	for (TInt index = 0; index < keysCount; index++)
		{
		CFileKeyData* keyData = iKeyList[index];
				
		// Write the key information 
		RStoreReadStream keyInfoStream;
		keyInfoStream.OpenLC(*iReadFileStore, keyData->InfoDataStreamId());
		CKeyInfo* keyInfo = CKeyInfo::NewL(keyInfoStream);
		TCleanupItem cleanupKeyInfo(CleanKeyInfo, keyInfo);
		CKeyInfoBase::EKeyAlgorithm algo = keyInfo->Algorithm();
		CleanupStack::PopAndDestroy(&keyInfoStream);
		CleanupStack::PushL(cleanupKeyInfo);
		
		switch(algo)
			{
			case (CKeyInfo::ERSA):
				{
				// retrieve the public key and write it in the new stream
				CRSAPublicKey* publicKey = NULL;
				RetrieveAndStorePublicKeyL(keyData, publicKey);
				
				// retrieve the private key, encrypt it with the protection key and store 
				// it in the new key stream. 
				CRSAPrivateKey* privateKey = NULL;
				RetrieveAndStorePrivateKeyL(keyData, privateKey);
				break;
				}
			case (CKeyInfo::EDSA):
				{
				// retrieve the public key and write it in the new stream
				CDSAPublicKey* publicKey = NULL;
				RetrieveAndStorePublicKeyL(keyData, publicKey);
				
				// retrieve the private key, encrypt it with the protection key and store 
				// it in the new key stream. 
				CDSAPrivateKey* privateKey = NULL;
				RetrieveAndStorePrivateKeyL(keyData, privateKey);
				break;
				}
			case (CKeyInfo::EDH):
				{
				// write the public stream id 
				RStoreWriteStream writePublicStream;
				TStreamId publicStrId = writePublicStream.CreateLC(*iWriteFileStore);
				keyData->SetPublicDataStreamId(publicStrId);
				writePublicStream.CommitL();
				CleanupStack::PopAndDestroy(&writePublicStream);
				// open the privatekeystream based on the supplied passphrase
				RStoreReadStream privateStream;
				privateStream.OpenLC(iPassphrase->Store(), keyData->PrivateDataStreamId());
								
				RInteger dhKey;
				CreateLC(privateStream, dhKey);
				RStoreWriteStream writePvtStr;
				TStreamId pvtStrId = writePvtStr.CreateLC(*iWriteFileStore);
				keyData->SetPrivateDataStreamId(pvtStrId);
				
				if (KNullStreamId == pvtStrId)
					{
					User::Leave(KErrBadHandle);
					}
				EncryptAndStoreL(dhKey, writePvtStr );
				writePvtStr.CommitL();
				CleanupStack::PopAndDestroy(3, &privateStream); // dhKey, writePvtStr
				
				break;
				
				}
			default:
				User::Leave(KErrKeyAlgorithm);
			}
		
		// write the info stream for the key
		RStoreWriteStream writeInfoStream;
		TStreamId infoStrId = writeInfoStream.CreateLC(*iWriteFileStore);
		keyData->SetInfoDataStreamId(infoStrId);
		WriteKeyL(*keyInfo, writeInfoStream );
		CleanupStack::PopAndDestroy( &writeInfoStream);
		CleanupStack::PopAndDestroy(); // cleanupKeyInfo	
		
		}
		
	RStoreWriteStream writeLookupStream;
	writeLookupStream.ReplaceLC(*iWriteFileStore, iWriteLookupStreamId );
	
	// write the count of keys 
	writeLookupStream.WriteInt32L(keysCount);
		
	for(TInt index=0;index<keysCount;++index)
		{
		CFileKeyData* keyData = iKeyList[index];
		// write the key using the lookupstream
		keyData->ExternalizeWithAuthL(writeLookupStream);
		}
	
	writeLookupStream.CommitL();
	CleanupStack::PopAndDestroy(&writeLookupStream);
	
	iWriteFileStore->CommitL();
	iReadFileStore->CommitL();
	
	}

void CKeytoolMigrateStore::WriteKeyL(	const CKeyInfo& aKeyInfo, 
										RStoreWriteStream& aWriteInfoStream )
	{
	aWriteInfoStream << aKeyInfo;
	WriteAuthDetailsL(aWriteInfoStream);
	aWriteInfoStream.CommitL();
	}

void CKeytoolMigrateStore::WriteAuthDetailsL( 	RStoreWriteStream& aWriteInfoStream	)
	{
	aWriteInfoStream.WriteInt32L(iUserIdentity->Id());
	aWriteInfoStream << *(iParams->iAuthExpression);
	aWriteInfoStream.WriteInt32L(iParams->iFreshness);
	}

// This is a cleanup item that reverts the store
void CKeytoolMigrateStore::RevertStore(TAny* aStore)
	{
	CPermanentFileStore* store = reinterpret_cast<CPermanentFileStore*>(aStore);
	TRAP_IGNORE(store->RevertL());
	// We're ignoring the leave code from this because there's no way we can
	// handle this sensibly.  This shouldn't be a problem in practice - this
	// will leave if for example the file store is on removable which is
	// unexpectedly remove, and this is never the case for us.
	}

void CKeytoolMigrateStore::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch(iState)
		{
		case EAfterAuthentication:
			{
			if(iUserIdentity == AuthServer::KUnknownIdentity)
				{
				User::Leave(KErrAuthenticationFailure);
				}
			WriteKeyInformationL();
			// once keyinformation has been written 
			// delete the user identity
			delete iUserIdentity;
			iUserIdentity = NULL;
			
			CActiveScheduler::Stop();
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		}
	}

void CKeytoolMigrateStore::DoCancel()
	{
	
	}

TInt CKeytoolMigrateStore::RunError(TInt aError)
	{
	if(aError != KErrNone)
		{
		aError = KErrNone;
		}
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CKeytoolMigrateStore::StoreKeyL(const TDesC8& aKeyData, RStoreWriteStream& aStream)
	{
	// retrieve the protection key of the current authenticated user for encrypting the 
	// user's private key.
	TPtrC8 key = iUserIdentity->Key().KeyData();
	// used for pbe, the class can be used for encryption/decryption based on the 
	// password supplied.
	CPBEncryptElement* pbeEncrypt = CPBEncryptElement::NewL(key);
	CleanupStack::PushL(pbeEncrypt);
	// create an object for ecryption
	CPBEncryptor* encryptor = pbeEncrypt->NewEncryptLC();
	// cerate the buffer size required for storing the encrypted data.
	HBufC8* ciphertext = HBufC8::NewLC(encryptor->MaxFinalOutputLength(aKeyData.Length()));
	TPtr8 ciphertextTemp = ciphertext->Des();
	// data gets encrypted and stored in the buffer
	encryptor->ProcessFinalL(aKeyData, ciphertextTemp);
	// externalixe the encryption parameters, this information is required later
	// for decrypting the text later.
	pbeEncrypt->EncryptionData().ExternalizeL(aStream);
	// write out the cipher data length
	aStream.WriteInt32L(ciphertext->Length());
	// write the cipher data to the stream.
	aStream.WriteL(*ciphertext);
		
	CleanupStack::PopAndDestroy(3, pbeEncrypt); // ciphertext,encryptor
	}

