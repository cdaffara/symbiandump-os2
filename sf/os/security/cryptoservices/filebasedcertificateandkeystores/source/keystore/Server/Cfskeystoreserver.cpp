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


#include "cfskeystoreserver.h"
#include "CKeyDataManager.h"
#include "CKeyStoreConduit.h"
#include "CKeyStoreSession.h"
#include "fstokencliserv.h"
#include "CCreateKey.h"
#include "OpenedKeys.h"
#include "keystorepassphrase.h"
#include "fsdatatypes.h"
#include "keystreamutils.h"
#include "fstokenutil.h"
#include <asnpkcs.h>
#include <pbedata.h>
#include <securityerr.h>
#include <pbe.h>
#include <asnpkcs.h>
#include <asn1enc.h>
#include <x509keys.h>
#include <keyidentifierutil.h>
#include <mctauthobject.h>
#include <utf.h>
#include <e32math.h>
#include "x509keyencoder.h"

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <s32mem.h>
#include <e32std.h>
#include <authserver/authclient.h>
#include <authserver/authexpression.h>
#include <authserver/auth_srv_errs.h>
#include <authserver/aspubsubdefs.h>
#include <authserver/authtypes.h>

#include <asymmetrickeys.h>
#include "keystorecenrepconfig.h"
#include "keystore_errs.h"
#include "cfskeystoreserver.inl"

// Length set to retrieve the authentication expression
// from the central repository.
const TInt KAuthStringLength = 255;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

// We don't currently allow any keys larger than 2048 bits.  It may be necessary to
// increase this limit in the future. 
const TUint KTheMinKeySize = 512;
const TUint KTheMaxKeySize = 2048;

// Security policies
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
_LIT_SECURITY_POLICY_C1(KSetTimeoutSecurityPolicy, ECapabilityWriteDeviceData);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
_LIT_SECURITY_POLICY_C1(KWriteUserDataPolicy, ECapabilityWriteUserData);
_LIT_SECURITY_POLICY_C1(KReadUserDataPolicy, ECapabilityReadUserData);

CFSKeyStoreServer* CFSKeyStoreServer::NewL()
	{
	CFSKeyStoreServer* me = new (ELeave) CFSKeyStoreServer();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

CFSKeyStoreServer::CFSKeyStoreServer() :
	CActive(EPriorityStandard),
	iAction(EIdle),
	iExportBuf(NULL, 0)
	{
	}

void CFSKeyStoreServer::ConstructL()
	{

	iConduit = CKeyStoreConduit::NewL(*this);
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	User::LeaveIfError(iAuthClient.Connect());		
	iKeyStoreCenrep = CKeyStoreCenrep::NewL();	
#endif	// SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	iKeyDataManager = CFileKeyDataManager::NewL();
	
	CActiveScheduler::Add(this);
	}

CFSKeyStoreServer::~CFSKeyStoreServer()
	{
	Cancel();

	delete iKeyDataManager;
	delete iConduit;
	delete iKeyCreator;
	iSessions.Close();

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iAuthClient.Close();	
	delete iUserIdentity;
	delete iKeyStoreCenrep;
	delete iAuthString;	
#endif //SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	}

CKeyStoreSession* CFSKeyStoreServer::CreateSessionL()
	{
	CPassphraseManager* passMan = iKeyDataManager->CreatePassphraseManagerLC();
	CKeyStoreSession* session = CKeyStoreSession::NewL(*this, passMan);
	CleanupStack::Pop(passMan);
	CleanupStack::PushL(session);
	User::LeaveIfError(iSessions.Append(session));
	CleanupStack::Pop(session);
	return session;
	}

void CFSKeyStoreServer::RemoveSession(CKeyStoreSession& aSession)
	{
	if (iSession == &aSession) // just in case
		{
		iSession = NULL; 
		}
	
	for (TInt index = 0 ; index < iSessions.Count() ; ++index)
		{
		if (iSessions[index] == &aSession)
			{
			iSessions.Remove(index);
			return;
			}
		}
	User::Invariant();
	}

void CFSKeyStoreServer::ServiceRequestL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	iMessage = &aMessage;
	iSession = &aSession;
	iConduit->ServiceRequestL(aMessage, aSession);
	}

//	*********************************************************************************
//	From MCTKeyStore
//	*********************************************************************************
void CFSKeyStoreServer::ListL(const TCTKeyAttributeFilter& aFilter,
							  RPointerArray<CKeyInfo>& aKeys)
	{
	ASSERT(iMessage);
	
	// Check the calling process has ReadUserData capability
	if (!KReadUserDataPolicy.CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
		
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iIdentityId = iKeyDataManager->CachedIdentity(); 
	if(iIdentityId == AuthServer::KUnknownIdentity)
		{
		HBufC* authExpression = HBufC::NewLC(KAuthStringLength);
		TPtr ptr = authExpression->Des();
		iKeyStoreCenrep->AuthExpressionL(ptr);
		TInt freshness = iKeyStoreCenrep->FreshnessL();
		CheckRangeL(freshness);
		
		AuthServer::CIdentity* identity = SyncAuthenticateLC(*authExpression,freshness);
		iIdentityId = identity->Id();
		CleanupStack::PopAndDestroy(2, authExpression); // identity
		}
#endif //SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	TInt count = iKeyDataManager->Count();

	for (TInt i = 0; i < count; ++i)
		{
		const CFileKeyData* data = (*iKeyDataManager)[i];
		CKeyInfo* info = iKeyDataManager->ReadKeyInfoLC(*data);
		if (KeyMatchesFilterL(*info, aFilter))
			{
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			if(info->Identity() == iIdentityId)
				{
				// this is required as the client side object does
				// not contain authentication details.
				info->ResetAuthExpression();
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
				
				User::LeaveIfError(aKeys.Append(info));
				CleanupStack::Pop(info);
				
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
				}
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			}
		else	
			{
			CleanupStack::PopAndDestroy(info);
			}		
		}
	}

TBool CFSKeyStoreServer::KeyMatchesFilterL(const CKeyInfo& aInfo,
										   const TCTKeyAttributeFilter& aFilter)
	{
	ASSERT(iMessage);
	
	if (aFilter.iKeyId.Length() && aFilter.iKeyId != aInfo.ID())
		{
		return EFalse;
		}

	if (aFilter.iUsage != EPKCS15UsageAll)
		{
		if ((aInfo.Usage() & aFilter.iUsage) == 0)
			return EFalse;
		}

	if (aFilter.iKeyAlgorithm != CCTKeyInfo::EInvalidAlgorithm && 
		aFilter.iKeyAlgorithm != aInfo.Algorithm())
		{
		return EFalse;
		}

	switch (aFilter.iPolicyFilter)
		{
		case TCTKeyAttributeFilter::EAllKeys:
			// All keys pass
			break;
			   
		case TCTKeyAttributeFilter::EUsableKeys:
			if (!aInfo.UsePolicy().CheckPolicy(*iMessage))
				{
				return EFalse;
				}
			break;
			
		case TCTKeyAttributeFilter::EManageableKeys:
			if (!aInfo.ManagementPolicy().CheckPolicy(*iMessage))
				{
				return EFalse;
				}
			break;

		case TCTKeyAttributeFilter::EUsableOrManageableKeys:
			if (!aInfo.UsePolicy().CheckPolicy(*iMessage) &&
				!aInfo.ManagementPolicy().CheckPolicy(*iMessage))
				{
				return EFalse;
				}
			break;
						
		default:
			User::Leave(KErrArgument);
		}

	return ETrue;
	}

void CFSKeyStoreServer::GetKeyInfoL(TInt aObjectId, CKeyInfo*& aInfo)
	{
	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}
	
	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);
	
	if (!keyInfo->UsePolicy().CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iIdentityId = iKeyDataManager->CachedIdentity(); 
	if(iIdentityId == AuthServer::KUnknownIdentity)
		{
		AuthServer::CIdentity* userIdentity = SyncAuthenticateLC(keyInfo->AuthExpression(), keyInfo->Freshness());
		iIdentityId = userIdentity->Id();
		CleanupStack::PopAndDestroy(userIdentity);
		}
	if(iIdentityId != keyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
#endif //SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	aInfo = keyInfo;
	CleanupStack::Pop(keyInfo);
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

AuthServer::CIdentity* CFSKeyStoreServer::SyncAuthenticateLC(const TDesC& aAuthExpression, TInt aFreshness)
	{
	AuthServer::CAuthExpression* expression = iAuthClient.CreateAuthExpressionL(aAuthExpression);
	CleanupStack::PushL(expression);
	TUid uid = TUid::Uid(0);
	AuthServer::CIdentity* userIdentity = iAuthClient.AuthenticateL(*expression,aFreshness, EFalse, uid, EFalse, KNullDesC);
	CleanupStack::PushL(userIdentity);
	if(userIdentity->Id() == AuthServer::KUnknownIdentity)
		{
		User::Leave(KErrAuthenticationFailure);
		}
	CleanupStack::Pop(userIdentity);
	CleanupStack::PopAndDestroy(expression);
	CleanupStack::PushL(userIdentity);
	return userIdentity;
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

TInt CFSKeyStoreServer::GetKeyLengthL(TInt aObjectId)
	{
	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}

	// this could be cached in memory (would break file format though)
	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);
	TInt result = keyInfo->Size();
	CleanupStack::PopAndDestroy(keyInfo);

	return result;
	}

COpenedKey* CFSKeyStoreServer::OpenKeyL(TInt aHandle, TUid aOpenedKeyType)
	{
	ASSERT(iMessage);
	
	const CFileKeyData *keyData = iKeyDataManager->Lookup(aHandle);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}
	
	return COpenedKey::NewL(*keyData, aOpenedKeyType, *iMessage,
							*iKeyDataManager, iSession->PassphraseManager());
	}

void CFSKeyStoreServer::ExportPublicL(TInt aObjectId,
									  TDes8& aOut)
	{
	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}

	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);

	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	AuthServer::CIdentity* identity = SyncAuthenticateLC(keyInfo->AuthExpression(), keyInfo->Freshness());
	if(identity->Id() != keyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(identity);
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	RStoreReadStream stream;
	iKeyDataManager->OpenPublicDataStreamLC(*keyData, stream);

	CKeyInfo::EKeyAlgorithm keyAlgorithm = keyInfo->Algorithm();
	
	switch(keyAlgorithm)
		{
		case (CKeyInfo::ERSA):
			{
			CRSAPublicKey* publicKey = NULL;

			CreateL(stream, publicKey);
			ASSERT(publicKey);
			CleanupStack::PushL(publicKey);

			TX509RSAKeyEncoder encoder(*publicKey, ESHA1);
			CASN1EncBase* encoded = encoder.EncodeKeyLC();

			if (encoded->LengthDER() > static_cast<TUint>(aOut.MaxLength()))
				{
				User::Leave(KErrOverflow);
				}
			
			//	Get the Public key DER encoding
			TUint pos=0;
			encoded->WriteDERL(aOut, pos);
			
			// WriteDERL does not set the length of the buffer, we do it ourselves			
			aOut.SetLength(encoded->LengthDER());			
			
			CleanupStack::PopAndDestroy(2, publicKey);
			}
		break;

		case (CKeyInfo::EDSA):
			{
			CDSAPublicKey* publicKey = NULL;

			CreateL(stream, publicKey);
			ASSERT(publicKey);
			CleanupStack::PushL(publicKey);

			TX509DSAKeyEncoder encoder(*publicKey, ESHA1);
			CASN1EncBase* encoded = encoder.EncodeKeyLC();

			if (encoded->LengthDER() > static_cast<TUint>(aOut.MaxLength()))
				{
				User::Leave(KErrOverflow);
				}

			//	Get the Public key DER encoding
			TUint pos=0;
			encoded->WriteDERL(aOut, pos);
			
			// WriteDERL does not set the length of the buffer, we do it ourselves			
			aOut.SetLength(encoded->LengthDER());						
			
			CleanupStack::PopAndDestroy(2, publicKey);
			}
		break;

		case (CKeyInfo::EDH):
		default:
			User::Leave(KErrKeyAlgorithm);
			break;
	}

	CleanupStack::PopAndDestroy(2, keyInfo); //stream, keyinfo
	}

//	*********************************************************************************
//	From MCTKeyStoreManager
//	*********************************************************************************

TInt CFSKeyStoreServer::CheckKeyAttributes(CKeyInfo& aKey, TNewKeyOperation aOp)
	{
	ASSERT(iMessage);
	
	// Sort out the access rights
	TInt access = aKey.AccessType(); 
	
	// Only allow sensitive and extractable to be sepcified
	if (access & ~(CKeyInfo::ESensitive | CKeyInfo::EExtractable))
		{
		return KErrKeyAccess;
		}	
		
	// If it's sensitive and either created internally 
	// or imported from an encrypted source then it's always been sensitive
	if ((access & CKeyInfo::ESensitive) &&
		(aOp == ENewKeyCreate || aOp == ENewKeyImportEncrypted))
		{
		access |= CKeyInfo::EAlwaysSensitive;		
		}
		
	// If it's not extractable and it's created internally
	// then it's never been extractable
	if ((!(access & CKeyInfo::EExtractable)) && aOp == ENewKeyCreate)
		{
		access |= CKeyInfo::ENeverExtractable;		
		}
		
	aKey.SetAccessType(access);
	
	// check management policy allows the calling process to manage the key
	if (!aKey.ManagementPolicy().CheckPolicy(*iMessage))
		{
		return KErrArgument;
		}

	// check end date is not in the past
	TTime timeNow;
	timeNow.UniversalTime();
	if (aKey.EndDate().Int64() != 0 && aKey.EndDate() <= timeNow)
		{
		return KErrKeyValidity;
		}

	// We don't support non-repudiation, however we currently allow keys
	// to be created with this usage

	return KErrNone;
	}

TInt CFSKeyStoreServer::CheckKeyAlgorithmAndSize(CKeyInfo& aKey)
	{
	CKeyInfo::EKeyAlgorithm keyAlgorithm = aKey.Algorithm();
	if ( ((keyAlgorithm!=CKeyInfo::ERSA) && (keyAlgorithm!=CKeyInfo::EDSA) && (keyAlgorithm!=CKeyInfo::EDH) ))
		{
		return KErrKeyAlgorithm;
		}
	
	if (aKey.Size() < KTheMinKeySize || aKey.Size() > KTheMaxKeySize)
		{
		return KErrKeySize;
		}

	return KErrNone;
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
/**
 * This method uses the default authentication policy to 
 * authenticated a user and encrypt the key using the protection
 * key and store it against the authenticated user .
 */
#else
/**
 * This method stores the keys in the keystore and encrypts the 
 * entire keystore with a passphrase.
 */
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFSKeyStoreServer::CreateKey(CKeyInfo& aReturnedKey, TRequestStatus& aStatus)
	{
	
	iKeyInfo = &aReturnedKey;
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
		
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	// the next state would ask for the user to be authenticated.
	// we do not use the cached identity directly through the
	// published value as the protection key of the user would 
	// be required to store the key in encrypted form. 
	iAction = ESetAuthPolicy;
#else
	iAction = EGetPassphrase;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	// Check the calling process has WriteUserData capability
	if (!KWriteUserDataPolicy.CheckPolicy(*iMessage))
		{
		CompleteClientRequest(KErrPermissionDenied);
		return;
		}
	
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
			
	}

void CFSKeyStoreServer::CheckRangeL(TInt aFreshness)
	{
	if( aFreshness < 0 || aFreshness > KMaxTInt)
		{
		User::Leave(KErrOverflow);
		}
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFSKeyStoreServer::AsyncAuthenticateL(	const TDesC& aAuthString,
											TInt aFreshness)
	{
	TUid uid = TUid::Uid(0);
	AuthServer::CAuthExpression* expression = iAuthClient.CreateAuthExpressionL(aAuthString);
	CleanupStack::PushL(expression);
	iAuthClient.AuthenticateL(*expression,aFreshness, EFalse, uid, EFalse, KNullDesC, iUserIdentity, iStatus);
	SetActive();
	CleanupStack::PopAndDestroy(expression);
	}

/**
 * This method uses the authentication policy as supplied in the 
 * call to authenticate an user and encrypt the key using its 
 * protection key and store it against the authenticated user .
 */
void CFSKeyStoreServer::CreateUserKey(	CKeyInfo& aReturnedKey,
										const TDesC& aAuthString,
										TInt aFreshness,
										TRequestStatus& aStatus)
	{
	iAction = EAuthenticate;
	iNextAction = ECreateUserKey;
		
	iKeyInfo = &aReturnedKey;
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
		
	// Check the calling process has WriteUserData capability
	if (!KWriteUserDataPolicy.CheckPolicy(*iMessage))
		{
		CompleteClientRequest(KErrPermissionDenied);
		return;
		}
		
	TRAPD(err,iKeyInfo->SetAuthExpressionL(aAuthString));
	if(err != KErrNone)
		{
		CompleteClientRequest(err);
		return;
		}
	
	iKeyInfo->SetFreshness(aFreshness);
	
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


void CFSKeyStoreServer::ImportUserKey(	const TDesC8& aKey, 
										CKeyInfo& aReturnedKey, 
										TBool aIsEncrypted,
										const TDesC& aAuthString,
										TInt aFreshness,
										TRequestStatus& aStatus)
	{
	ASSERT(iMessage);
 
	iPKCS8Data.Set(aKey);
	iImportingEncryptedKey = aIsEncrypted;
	iKeyInfo = &aReturnedKey;
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
	
	// Check the calling process has WriteUserData capability
	if (!KWriteUserDataPolicy.CheckPolicy(*iMessage))
		{
		CompleteClientRequest(KErrPermissionDenied);
		return;
		}
		
	TRAPD(err,iKeyInfo->SetAuthExpressionL(aAuthString));
	if(err != KErrNone)
		{
		CompleteClientRequest(err);
		return;
		}
		
	iKeyInfo->SetFreshness(aFreshness);
	
	iAction = EAuthenticate;
	iNextAction = EImportUserKey;
	
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
		
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFSKeyStoreServer::CancelCreateKey()
	{
	if (iAction == ECreateKeyCreate ||
		iAction == ECreateKeyFinal)
		{
		Cancel();
		}
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	if (iAction == ECreateUserKey || 
		iAction == ECreateKeyFinal)
		{
		Cancel();
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CFSKeyStoreServer::DoCreateKeyL()
{
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	__ASSERT_DEBUG(iAction==ECreateUserKey, PanicServer(EPanicECreateKeyNotReady));
#else
	__ASSERT_DEBUG(iAction==ECreateKeyCreate, PanicServer(EPanicECreateKeyNotReady));
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	__ASSERT_DEBUG(iKeyInfo, PanicServer(EPanicNoClientData));
	
	delete iKeyCreator;
	iKeyCreator = NULL;
	
	iKeyCreator = new(ELeave) CKeyCreator();
	SetActive();
	iKeyCreator->DoCreateKeyAsync(iKeyInfo->Algorithm(), iKeyInfo->Size(), iStatus);
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFSKeyStoreServer::SetAuthenticationPolicy(	
							TInt aObjectId, 
							HBufC* aAuthString,
							TInt aFreshness, 
							TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
	iObjectId = aObjectId;
	iAuthString = aAuthString;
	iFreshness = aFreshness;
	
	// Check the calling process has WriteUserData capability
	if (!KWriteUserDataPolicy.CheckPolicy(*iMessage))
		{
		CompleteClientRequest(KErrPermissionDenied);
		return;
		}
		
	iAction = EDoSetAuthenticationPolicy;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
			
	}

HBufC* CFSKeyStoreServer::AuthExpressionL( TInt aObjectId)
	{
	// Check the calling process has ReadUserData capability
	if (!KReadUserDataPolicy.CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
	
	CKeyInfo* keyInfo = KeyDetailsLC(aObjectId);
	HBufC* authExpression = keyInfo->AuthExpression().AllocL();
	CleanupStack::PopAndDestroy(keyInfo);
	
	return authExpression;
	}

TInt CFSKeyStoreServer::FreshnessL(TInt aObjectId)
	{
	// Check the calling process has ReadUserData capability
	if (!KReadUserDataPolicy.CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
		
	CKeyInfo* keyInfo = KeyDetailsLC(aObjectId);
	TInt freshness = keyInfo->Freshness();
	CleanupStack::PopAndDestroy(keyInfo);
			
	return freshness;
	}

CKeyInfo* CFSKeyStoreServer::KeyDetailsLC(TInt aObjectId)
	{
	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}
	
	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);
	AuthServer::TIdentityId identityId = iKeyDataManager->CachedIdentity(); 
		
	if( identityId == AuthServer::KUnknownIdentity)
		{
		AuthServer::CIdentity* userIdentity = SyncAuthenticateLC(keyInfo->AuthExpression(), keyInfo->Freshness());
		identityId = userIdentity->Id();
		CleanupStack::PopAndDestroy(userIdentity);		
		}
	if(identityId == AuthServer::KUnknownIdentity)
		{
		User::Leave(KErrAuthenticationFailure);
		}
	if(identityId != keyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
	
	return keyInfo;
	}

/*
 * This method is used to set the default authentication policy for a key which is being
 * created using the old methods.
 */

void CFSKeyStoreServer::SetDefaultAuthPolicyL()
	{
	ASSERT(iKeyInfo);
	
	HBufC* authExpression = HBufC::NewLC(KAuthStringLength);
	TPtr ptr = authExpression->Des();
	iKeyStoreCenrep->AuthExpressionL(ptr);
	TInt freshness = iKeyStoreCenrep->FreshnessL();
	CheckRangeL(freshness);
	
	iKeyInfo->SetAuthExpressionL(*authExpression);
	iKeyInfo->SetFreshness(freshness);
	CleanupStack::PopAndDestroy(authExpression);			
	}

void CFSKeyStoreServer::WriteAuthenticationPolicyL()
	{
	ASSERT(iKeyInfo);
	
	if(iIdentityId != iKeyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
	
	iKeyInfo->SetAuthExpressionL(*iAuthString);
	delete iAuthString;
	iAuthString = NULL;
	iKeyInfo->SetFreshness(iFreshness);
	iKeyDataManager->SafeWriteKeyInfoL(*iKeyData, *iKeyInfo);
	}	

#else
/**
 * Get the default passphrase for the store, or create one if it hasn't been set
 * yet.  This is used for key creation, import and export.
 *
 * Calls SetActive(), and sets the next state.
 */
void CFSKeyStoreServer::GetKeystorePassphrase(TCurrentAction aNextState)
	{
	ASSERT(iSession);
	TStreamId passphraseId = iKeyDataManager->DefaultPassphraseId();
	TInt timeout = iKeyDataManager->GetPassphraseTimeout();
	if (passphraseId == KNullStreamId)
		{
		iSession->PassphraseManager().CreatePassphrase(timeout, iPassphrase, iStatus);
		}
	else
		{
		iSession->PassphraseManager().GetPassphrase(passphraseId, timeout, iPassphrase, iStatus);
		}
	iAction = aNextState;
	SetActive();
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

/**
 * Store a key.
 *
 * 
 *
 * This method is not transaction-safe, as if a leave occurs after key data has
 * been written to the store, it is not cleaned up.  Also, it shares most of its
 * code with PKCS8ToKeyL (and the same problem applied).  This should be
 * corrected by creating an interface for generic key data, including write
 * private key / write public key / get key identifier operations, which can be
 * passed to a single method in the key data manager to do the job of these two
 * methods.
 *
 * I'm not fixing this now because it's not a big problem for real-world use,
 * and I don't have the time to spend on it.
 *
 * - jc
 */
void CFSKeyStoreServer::DoStoreKeyL()
	{
	__ASSERT_DEBUG(iAction==ECreateKeyFinal, PanicServer(EPanicECreateKeyNotReady));
	__ASSERT_DEBUG(iKeyInfo, PanicServer(EPanicNoClientData));
	__ASSERT_DEBUG(iKeyCreator, PanicServer(ENoCreatedKeyData));
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ASSERT(iPassphrase);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	const CFileKeyData* keyData = NULL;
	RStoreWriteStream privateStream;
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	keyData = iKeyDataManager->CreateKeyDataLC(iKeyInfo->Label(), iKeyInfo->Identity());
	iKeyDataManager->OpenPrivateDataStreamLC(*keyData, privateStream);
#else
	keyData = iKeyDataManager->CreateKeyDataLC(iKeyInfo->Label(), iPassphrase->StreamId());
	iKeyDataManager->OpenPrivateDataStreamLC(*keyData, *iPassphrase, privateStream);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	
	CKeyInfo::EKeyAlgorithm keyAlgorithm = iKeyInfo->Algorithm();

// 	Get key identifier and externalize private key 
	TKeyIdentifier theKeyId;
	switch (keyAlgorithm)
		{
		case (CKeyInfo::ERSA):
			{
			CRSAKeyPair* newKey = iKeyCreator->GetCreatedRSAKey();
			KeyIdentifierUtil::RSAKeyIdentifierL(newKey->PublicKey(), theKeyId);			
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
				EncryptAndStoreL(newKey->PrivateKey(), privateStream);
			#else
				privateStream << newKey->PrivateKey();
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			break;
			}
			
		case (CKeyInfo::EDSA):
			{
			CDSAKeyPair* newKey = iKeyCreator->GetCreatedDSAKey();			
			KeyIdentifierUtil::DSAKeyIdentifierL(newKey->PublicKey(), theKeyId);
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
				EncryptAndStoreL(newKey->PrivateKey(), privateStream);
			#else
				privateStream << newKey->PrivateKey();
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER	
			break;
			}

		case (CKeyInfo::EDH):
			{
			RInteger newKey; 
			iKeyCreator->GetCreatedDHKey(newKey);			
			KeyIdentifierUtil::DHKeyIdentifierL(newKey, theKeyId);

			if (newKey.IsZero())
				User::Leave(KErrArgument);

			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER	
				EncryptAndStoreL(newKey, privateStream);
			#else
				privateStream << newKey;
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			break;
			}
		
		default:
			__ASSERT_DEBUG(EFalse, PanicClient(*iMessage,EPanicInvalidKeyCreateReq));
			break;
		}

	privateStream.CommitL();
	CleanupStack::PopAndDestroy(&privateStream); 
	
//	Fill in the CCTKeyInfo data currently missing (TKeyIdentifier and handle)
	iKeyInfo->SetHandle(keyData->Handle());
	iKeyInfo->SetIdentifier(theKeyId);	

// 	Externalize public key

	RStoreWriteStream publicStream;
	iKeyDataManager->OpenPublicDataStreamLC(*keyData, publicStream);

	switch (keyAlgorithm)
		{
		case (CKeyInfo::ERSA):
			publicStream << iKeyCreator->GetCreatedRSAKey()->PublicKey();			
			break;
			
		case (CKeyInfo::EDSA):
			publicStream << iKeyCreator->GetCreatedDSAKey()->PublicKey();
			break;

		case (CKeyInfo::EDH):
			// Nothing to do for DH
			break;
		
		default:
			__ASSERT_DEBUG(EFalse, PanicClient(*iMessage,EPanicInvalidKeyCreateReq));
			break;
		}
	
	publicStream.CommitL();
	CleanupStack::PopAndDestroy(&publicStream);  

//	Finished with the key creator
	delete iKeyCreator;
	iKeyCreator = NULL;
	
//	Externalize the CKeyInfo data associated with the key,
	iKeyDataManager->WriteKeyInfoL(*keyData, *iKeyInfo);

//	Now add the new key to the data manager (which adds it to the store)
	iKeyDataManager->AddL(keyData);
	CleanupStack::Pop(); // keyData 
	
}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
void CFSKeyStoreServer::StoreKeyL(const TDesC8& aKeyData, RStoreWriteStream& aStream)
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
	// once the key has been store, delete the user identity
	delete iUserIdentity;
	iUserIdentity = NULL;
	CleanupStack::PopAndDestroy(3, pbeEncrypt); // ciphertext,encryptor
	}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFSKeyStoreServer::ImportKey(const TDesC8& aKey, CKeyInfo& aReturnedKey, TBool aIsEncrypted, TRequestStatus& aStatus)
	{
	ASSERT(iMessage);
 
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
		
	// Check the calling process has WriteUserData capability
	if (!KWriteUserDataPolicy.CheckPolicy(*iMessage))
		{
		CompleteClientRequest(KErrPermissionDenied);
		return;
		}
		
	iPKCS8Data.Set(aKey);
	iImportingEncryptedKey = aIsEncrypted;
	
	iKeyInfo = &aReturnedKey;
	
	SetActive();
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iAction = EDoImportKey;
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
#else
	TInt err = CheckImportKeyPolicy(iImportingEncryptedKey ? ENewKeyImportEncrypted : ENewKeyImportPlaintext);
	if(err != KErrNone)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, err);
		return;
		}
	
	iAction = EImportOpenPrivateStream;
	
	if (iImportingEncryptedKey)
		{
		TPasswordManager::ImportPassword(iPassword, iStatus);
		}
	else
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CFSKeyStoreServer::CheckExportKeyPolicyL()
	{
	ASSERT(iMessage);
	ASSERT(!iKeyData);
	
	if(iKeyInfo)
		{
		delete iKeyInfo;
		iKeyInfo = NULL;
		}
	
	const CFileKeyData* keyData = iKeyDataManager->Lookup(iObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}

	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);
		
	// Check the caller is allowed by the management policy
	if (!keyInfo->ManagementPolicy().CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
		
	// Check access flags allow key to be exported
	if (!(keyInfo->AccessType() & CCTKeyInfo::EExtractable) ||
		((keyInfo->AccessType() & CCTKeyInfo::ESensitive) && !iExportingKeyEncrypted))
		{
		User::Leave(KErrKeyAccess);
		}

	// Check this isn't a DH key
	if (keyInfo->Algorithm() != CKeyInfo::ERSA &&
		keyInfo->Algorithm() != CKeyInfo::EDSA)
		{
		User::Leave(KErrNotSupported);
		}

	iKeyData = keyData;
	iKeyInfo = keyInfo;
	CleanupStack::Pop(keyInfo);		
	
	}

/*
 * During the import of a key, attributes should be checked and check whether
 * the key does not exist in the specified store. 
 */

TInt CFSKeyStoreServer::CheckImportKeyPolicy(TNewKeyOperation aKeyOperation)
	{
	ASSERT(iMessage);
 
	TInt err = CheckKeyAttributes(*iKeyInfo, aKeyOperation);		
	
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	if (err == KErrNone && iKeyDataManager->IsKeyAlreadyInStore(iKeyInfo->Label(), iKeyInfo->Identity()))
		{
		err = KErrAlreadyExists;
		}
#else
	if (err == KErrNone && iKeyDataManager->IsKeyAlreadyInStore(iKeyInfo->Label()))
		{
		err = KErrAlreadyExists;
		}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	return err;
	}

/*
 * During the creation of a key in addition to the basic policy check the 
 * access type should be set to local for a key created in this store.
 */

TInt CFSKeyStoreServer::CheckCreateKeyPolicy()
	{
	ASSERT(iSession);

	TInt err = CheckImportKeyPolicy(ENewKeyCreate);
	if (err == KErrNone)
		{
		err = CheckKeyAlgorithmAndSize(*iKeyInfo);
		}
	if (err != KErrNone)
		{
		return err;
		}

	// DEF042306: Make it local only if it's created in the keystore
	iKeyInfo->SetAccessType(iKeyInfo->AccessType() | CKeyInfo::ELocal);

	return err;
	}

void CFSKeyStoreServer::CancelImportKey()
	{
	if (iAction == EImportOpenPrivateStream ||
		iAction == EImportKey)
		{
		Cancel();
		}
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	if (iAction == EImportUserKey)
		{
		Cancel();
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

void CFSKeyStoreServer::DoImportKeyL()
	{
	// Generate a decode PKCS8 data object from the incoming descriptor of PKCS8
	// data Creation of this will parse the DER stream and generate the
	// appropriate key representation based on the algorithm

	ASSERT(iPKCS8Data.Ptr());

	CDecPKCS8Data* pkcs8Data = NULL;

	if (iImportingEncryptedKey)
		{
		// Convert import passphrase to 8 bit representation
		TBuf8<32> password;
		
		CnvUtfConverter::ConvertFromUnicodeToUtf8(password, iPassword);
		pkcs8Data = TASN1DecPKCS8::DecodeEncryptedDERL(iPKCS8Data, password);
		}
	else
		{
		pkcs8Data = TASN1DecPKCS8::DecodeDERL(iPKCS8Data);
		}
	
	CleanupStack::PushL(pkcs8Data);	
	PKCS8ToKeyL(pkcs8Data);
	CleanupStack::PopAndDestroy(pkcs8Data);
}

void CFSKeyStoreServer::PKCS8ToKeyL(CDecPKCS8Data* aPKCS8Data)
{ 
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ASSERT(iPassphrase);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ASSERT(aPKCS8Data);
	
	MPKCS8DecodedKeyPairData* keyPairData = aPKCS8Data->KeyPairData();

	// Set algorithm and size from pkcs8 data, and sanity check them
	if (aPKCS8Data->Algorithm() != ERSA && aPKCS8Data->Algorithm() != EDSA)
		{
		User::Leave(KErrKeyAlgorithm);
		}
	iKeyInfo->SetAlgorithm((aPKCS8Data->Algorithm() == ERSA) ? CKeyInfoBase::ERSA : CKeyInfoBase::EDSA);
	iKeyInfo->SetSize(keyPairData->KeySize());
	User::LeaveIfError(CheckKeyAlgorithmAndSize(*iKeyInfo));

	// Retrieve and store any PKCS8 attributes (in DER encoded descriptor)
	// These will form part of CKeyInfo & available for callers to decode	
	TPtrC8 theAttributes(aPKCS8Data->PKCS8Attributes());
	if (theAttributes != KNullDesC8)
		{
		iKeyInfo->SetPKCS8AttributeSet(theAttributes.AllocL());
		}
		
	const CFileKeyData* keyData = NULL;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	keyData = iKeyDataManager->CreateKeyDataLC(iKeyInfo->Label(),iKeyInfo->Identity());
#else
	keyData = iKeyDataManager->CreateKeyDataLC(iKeyInfo->Label(), iPassphrase->StreamId());
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	RStoreWriteStream privateStream;
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iKeyDataManager->OpenPrivateDataStreamLC(*keyData, privateStream);
#else
	iKeyDataManager->OpenPrivateDataStreamLC(*keyData, *iPassphrase, privateStream);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	// Generate the key identifier
	TKeyIdentifier theKeyId;
	keyPairData->GetKeyIdentifierL(theKeyId);
	
	// Fill in the CKeyInfo data currently missing (TKeyIdentifier and handle)
	iKeyInfo->SetHandle(keyData->Handle());
	iKeyInfo->SetIdentifier(theKeyId);	

	CKeyInfo::EKeyAlgorithm keyAlgorithm = iKeyInfo->Algorithm();
	
	// Externalize private key data
	switch (keyAlgorithm)
		{
		case (CKeyInfo::ERSA):
			{
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			EncryptAndStoreL(static_cast<CPKCS8KeyPairRSA*>(keyPairData)->PrivateKey(), privateStream);
			#else
			privateStream << static_cast<CPKCS8KeyPairRSA*>(keyPairData)->PrivateKey();
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
			break;
			}
		case (CKeyInfo::EDSA):
			{
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			EncryptAndStoreL(static_cast<CPKCS8KeyPairDSA*>(keyPairData)->PrivateKey(), privateStream);
			#else
				privateStream << static_cast<CPKCS8KeyPairDSA*>(keyPairData)->PrivateKey();
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
			break;
			}	
		default:
			__ASSERT_DEBUG(EFalse, PanicClient(*iMessage,EPanicInvalidKeyCreateReq));
			break;
		}

	privateStream.CommitL();
	CleanupStack::PopAndDestroy(&privateStream);

	// Externalize public key data
	RStoreWriteStream publicStream;
	iKeyDataManager->OpenPublicDataStreamLC(*keyData, publicStream);

	switch (keyAlgorithm)
		{
		case (CKeyInfo::ERSA):
			publicStream << static_cast<CPKCS8KeyPairRSA*>(keyPairData)->PublicKey();
			break;
			
		case (CKeyInfo::EDSA):
			publicStream << static_cast<CPKCS8KeyPairDSA*>(keyPairData)->PublicKey();
			break;
			
		default:
			__ASSERT_DEBUG(EFalse, PanicClient(*iMessage,EPanicInvalidKeyCreateReq));
			break;
		}

	publicStream.CommitL();
	CleanupStack::PopAndDestroy(&publicStream);

	// Externalize the CKeyInfo data associated with the key,
	iKeyDataManager->WriteKeyInfoL(*keyData, *iKeyInfo);

	// Now add the new key to the data manager (which adds it to the store)
	iKeyDataManager->AddL(keyData);
	CleanupStack::Pop(); // keyData
}

void CFSKeyStoreServer::ExportKey(TInt aObjectId, const TPtr8& aKey, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
	iExportBuf.Set(aKey);
	iObjectId = aObjectId;
	iExportingKeyEncrypted = EFalse;
	iAction = EDoExportKey;
	SetActive();
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	
	}

void CFSKeyStoreServer::CancelExportKey()
	{

	if (iAction == EExportKey ||
		iAction == EExportKeyGetPassphrase)
		{
		Cancel();
		}
	}	

void CFSKeyStoreServer::ExportEncryptedKey(TInt aObjectId, const TPtr8& aKey, CPBEncryptParms& aParams, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iCallerRequest = &aStatus;
	iExportBuf.Set(aKey);
	iObjectId = aObjectId;
	iExportingKeyEncrypted	= ETrue;	
	iPbeParams = &aParams;		
	iAction = EDoExportKey;
	SetActive();
		
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


void CFSKeyStoreServer::CancelExportEncryptedKey()
	{
	if (iAction == EExportKeyGetPassphrase ||
		iAction == EExportKey)
		{
		Cancel();
		}
	}	


void CFSKeyStoreServer::CompleteKeyExportL(TBool encrypted /*=EFalse*/)
	{
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ASSERT(iPassphrase);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ASSERT(iKeyData);
	ASSERT(iExportBuf.Ptr());
	
	CKeyInfo::EKeyAlgorithm keyAlgorithm = iKeyInfo->Algorithm();
	RStoreReadStream privStream;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iKeyDataManager->OpenPrivateDataStreamLC(*iKeyData, privStream);
#else
	iKeyDataManager->OpenPrivateDataStreamLC(*iKeyData, *iPassphrase, privStream);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	CASN1EncSequence* encoded = NULL;
			
	switch(keyAlgorithm)
		{
		case (CKeyInfo::ERSA):
			{
			RStoreReadStream pubStream;
			iKeyDataManager->OpenPublicDataStreamLC(*iKeyData, pubStream);
			CRSAPublicKey* publicKey = NULL;
			CreateL(pubStream, publicKey);
			ASSERT(publicKey);
			CleanupStack::PushL(publicKey);
					
			CRSAPrivateKey* privateKey = NULL;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
			TPtrC8 key = iUserIdentity->Key().KeyData();
			HBufC8* plaintext = DecryptFromStreamL(privStream, key);
			CleanupStack::PushL(plaintext);
			TAny* ptr = const_cast<TAny*>(static_cast<const TAny*>(plaintext->Des().Ptr()));
			RMemReadStream decryptedStream(ptr, plaintext->Length());
			CleanupClosePushL(decryptedStream);
			CreateL(decryptedStream, privateKey);
			CleanupStack::PopAndDestroy(2,plaintext);

#else
			CreateL(privStream, privateKey);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
			ASSERT(privateKey);
			CleanupStack::PushL(privateKey);			

			if (encrypted)
				{
				TBuf8<32> password;
				CnvUtfConverter::ConvertFromUnicodeToUtf8(password, iPassword);

				CPBEncryptElement* encryptElement = CPBEncryptElement::NewLC(password, *iPbeParams);
				CPBEncryptor* encryptor = encryptElement->NewEncryptLC();
				encoded = TASN1EncPKCS8::EncodeEncryptedL(*(static_cast<CRSAPrivateKeyCRT*>(privateKey)), *publicKey, *encryptor, *iPbeParams, iKeyInfo->PKCS8AttributeSet());
				CleanupStack::PopAndDestroy(2, encryptElement); 	// encryptor, encryptElement 
				}
			else
				{
				encoded = TASN1EncPKCS8::EncodeL(*(static_cast<CRSAPrivateKeyCRT*>(privateKey)), *publicKey, iKeyInfo->PKCS8AttributeSet());					
				}
			CleanupStack::PopAndDestroy(3, &pubStream);          // privateKey,  publicKey, pubStream
			}
			break;

		case (CKeyInfo::EDSA):
			{
			CDSAPrivateKey* privateKey = NULL;

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			TPtrC8 key = iUserIdentity->Key().KeyData();
			HBufC8* plaintext = DecryptFromStreamL(privStream, key);
			CleanupStack::PushL(plaintext);
			TAny* ptr = const_cast<TAny*>(static_cast<const TAny*>(plaintext->Des().Ptr()));
			RMemReadStream decryptedStream(ptr, plaintext->Length());
			CleanupClosePushL(decryptedStream);
			CreateL(decryptedStream, privateKey);
			CleanupStack::PopAndDestroy(2,plaintext);
#else
			CreateL(privStream, privateKey);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

			ASSERT(privateKey);
			CleanupStack::PushL(privateKey);

			if (encrypted)
				{			
				TBuf8<32> password;
				CnvUtfConverter::ConvertFromUnicodeToUtf8(password, iPassword);
										
				CPBEncryptElement* encryptElement = CPBEncryptElement::NewLC(password, *iPbeParams);         				
				CPBEncryptor* encryptor = encryptElement->NewEncryptLC(); 								
				encoded = TASN1EncPKCS8::EncodeEncryptedL(*privateKey, *encryptor, *iPbeParams, iKeyInfo->PKCS8AttributeSet());					
				CleanupStack::PopAndDestroy(2, encryptElement);	// encryptor, encryptElement	
				}
			else
				{
				encoded = TASN1EncPKCS8::EncodeL(*privateKey, iKeyInfo->PKCS8AttributeSet());					
				}					
			CleanupStack::PopAndDestroy(privateKey);
			}
			break;

		case (CKeyInfo::EInvalidAlgorithm):
		default:
			User::Leave(KErrKeyAlgorithm);		
			break;
		}
			
	// common to all algorithms			
	ASSERT(encoded);
	CleanupStack::PushL(encoded);
	if (encoded->LengthDER() > static_cast<TUint>(iExportBuf.MaxLength()))
		{
		User::Leave(KErrOverflow);
		}
	TUint pos=0;
	encoded->WriteDERL(iExportBuf, pos);

	// WriteDERL does not set the length of the buffer, we do it ourselves			
	iExportBuf.SetLength(encoded->LengthDER());
			
	CleanupStack::PopAndDestroy(encoded); 
	CleanupStack::PopAndDestroy(&privStream); 
	CompleteClientRequest(KErrNone);
	}

void CFSKeyStoreServer::DeleteKeyL(TInt aObjectId)
	{
	ASSERT(iMessage);

	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}

	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);

	// Check the caller is allowed by the management policy
	if (!keyInfo->ManagementPolicy().CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	AuthServer::TIdentityId identity = iKeyDataManager->CachedIdentity();
	if(identity == AuthServer::KUnknownIdentity)
		{
		AuthServer::CIdentity* userIdentity = SyncAuthenticateLC(keyInfo->AuthExpression(), keyInfo->Freshness());
		identity = userIdentity->Id();
		CleanupStack::PopAndDestroy(userIdentity);
		}
	if(identity != keyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	CleanupStack::PopAndDestroy(keyInfo);

	// Check if any session has this key open
	for (TInt i = 0 ; i < iSessions.Count() ; ++i)
		{
		CKeyStoreSession& session = *iSessions[i];
		if (session.HasOpenKey(aObjectId))
			{
			User::Leave(KErrInUse);
			}	
		}

	iKeyDataManager->RemoveL(aObjectId);
}

void CFSKeyStoreServer::SetUsePolicyL(TInt aObjectId, const TSecurityPolicy& aPolicy)
	{
	ASSERT(iMessage);
	
	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}

	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);

	// Check the caller is allowed by the management policy
	if (!keyInfo->ManagementPolicy().CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	AuthServer::TIdentityId identity = iKeyDataManager->CachedIdentity();
	if(identity == AuthServer::KUnknownIdentity)
		{
		AuthServer::CIdentity* userIdentity = SyncAuthenticateLC(keyInfo->AuthExpression(), keyInfo->Freshness());
		identity = userIdentity->Id();
		CleanupStack::PopAndDestroy(userIdentity);
		}
	if(identity != keyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	//should revert change if write fails
	keyInfo->SetUsePolicy(aPolicy); 
	iKeyDataManager->SafeWriteKeyInfoL(*keyData, *keyInfo);	

	CleanupStack::PopAndDestroy(keyInfo);
	}
	
void CFSKeyStoreServer::SetManagementPolicyL(TInt aObjectId, const TSecurityPolicy& aPolicy)
	{
	ASSERT(iMessage);
	
	const CFileKeyData* keyData = iKeyDataManager->Lookup(aObjectId);
	if (!keyData)
		{
		User::Leave(KErrNotFound);
		}

	CKeyInfo* keyInfo = iKeyDataManager->ReadKeyInfoLC(*keyData);

	// Check the caller is allowed by current management policy
	if (!keyInfo->ManagementPolicy().CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

	// Check the caller is allowed by new management policy
	if (!aPolicy.CheckPolicy(*iMessage))
		{
		User::Leave(KErrArgument);
		}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	AuthServer::TIdentityId identity = iKeyDataManager->CachedIdentity();
	if(identity == AuthServer::KUnknownIdentity)
		{
		AuthServer::CIdentity* userIdentity = SyncAuthenticateLC(keyInfo->AuthExpression(), keyInfo->Freshness());
		identity = userIdentity->Id();
		CleanupStack::PopAndDestroy(userIdentity);
		}
	if(identity != keyInfo->Identity())
		{
		User::Leave(KErrNotFound);
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	//should revert change if write fails
	keyInfo->SetManagementPolicy(aPolicy);
	iKeyDataManager->SafeWriteKeyInfoL(*keyData, *keyInfo);	

	CleanupStack::PopAndDestroy(keyInfo);
	}

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
// For MCTAuthenticationObject

void CFSKeyStoreServer::ChangePassphrase(TRequestStatus& aStatus)
	{
	if (iKeyDataManager->DefaultPassphraseId() == KNullStreamId)
		{
		// No passphrase set, can't change it
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotFound);		
		}
	
	iCallerRequest = &aStatus;
	iAction = EChangePassphrase;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CFSKeyStoreServer::DoChangePassphrase()
	{
	ASSERT(iSession);
	ASSERT(iKeyDataManager->DefaultPassphraseId() != KNullStreamId);
	iSession->PassphraseManager().ChangePassphrase(iKeyDataManager->DefaultPassphraseId(), iStatus);
	iAction = EChangePassphraseClearCached;
	SetActive();
	}

void CFSKeyStoreServer::CancelChangePassphrase()
	{
	if (iAction == EChangePassphrase ||
		iAction == EChangePassphraseClearCached)
		{
		Cancel();
		}
	}

void CFSKeyStoreServer::AuthOpen(TRequestStatus& aStatus)
	{
	iCallerRequest = &aStatus;
	GetKeystorePassphrase(EAuthOpen);
	}

void CFSKeyStoreServer::CancelAuthOpen()
	{
	if (iAction == EAuthOpen)
		{
		Cancel();
		}
	}

void CFSKeyStoreServer::AuthClose()
	{
	ASSERT(iSession);
	iSession->PassphraseManager().RemoveCachedPassphrases(iKeyDataManager->DefaultPassphraseId());
	}

TInt CFSKeyStoreServer::GetTimeRemainingL()
	{
	ASSERT(iSession);
	TStreamId passStreamId = iKeyDataManager->DefaultPassphraseId();
	return iSession->PassphraseManager().TimeRemainingL(passStreamId);
	}

void CFSKeyStoreServer::SetTimeoutL(TInt aTimeout)
	{
	ASSERT(iMessage);

	if (!KSetTimeoutSecurityPolicy.CheckPolicy(*iMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
	
	if (aTimeout < KTimeoutNever)
		{
		User::Leave(KErrArgument);
		}

	iKeyDataManager->SetPassphraseTimeoutL(aTimeout);
	RemoveCachedPassphrases(KNullStreamId);
	}

TInt CFSKeyStoreServer::GetTimeout()
	{
	return iKeyDataManager->GetPassphraseTimeout();
	}

void CFSKeyStoreServer::Relock()
	{
	RemoveCachedPassphrases(KNullStreamId);
	}

void CFSKeyStoreServer::RemoveCachedPassphrases(TStreamId aStreamId)
	{
	for (TInt index = 0 ; index < iSessions.Count() ; ++index)
		{
		CKeyStoreSession& session = *iSessions[index];					
		session.PassphraseManager().RemoveCachedPassphrases(aStreamId);
		}			
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

//	*********************************************************************************
//	From CActive
//	*********************************************************************************
TInt CFSKeyStoreServer::RunError(TInt aError)
	{ 
	CompleteClientRequest(aError);
	return (KErrNone);		//	Handled
	}

void CFSKeyStoreServer::CompleteClientRequest(TInt aCompletionCode)
	{
	Cleanup();
		
	if (iCallerRequest)
		User::RequestComplete(iCallerRequest, aCompletionCode);

	iAction = EIdle;		//	Reset action

	}

void CFSKeyStoreServer::Cleanup()
	{
	// Delete anything we might have created
	delete iKeyCreator; iKeyCreator = NULL;

	if (iAction == EExportKeyGetPassphrase ||
		iAction == EExportEncryptedKeyGetPassphrase ||
		iAction == EExportKey ||
		iAction == EExportEncryptedKey
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		|| iAction == ESetAuthenticationPolicy 
		|| iAction == EDoSetAuthenticationPolicy
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		)
		{
		// we only own iKeyInfo for export operations
		delete iKeyInfo;
		iKeyInfo = NULL;
		}
	
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	delete iUserIdentity;
	iUserIdentity = NULL;
	delete iAuthString;
	iAuthString = NULL;
	
	// this is a design restriction, the ckeyinfo object 
	// contains the authexpression, but while sending 
	// the key info back to the client, the authentication
	// data needs to be deleted.
	if(	iAction == EImportKey || iAction == EAuthenticate ||
		iAction == EImportUserKey || iAction == EDoImportKey ||
		iAction == ECreateKeyFinal )
		{
		iKeyInfo->ResetAuthExpression();
		}
	#endif //SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	// Zero pointers to things we don't own
	iPassphrase = NULL;
	iKeyInfo = NULL;
	iKeyData = NULL;
	iExportBuf.Set(NULL, 0, 0);
	iPKCS8Data.Set(NULL, 0);
	iSession = NULL;
	iMessage = NULL;

	}

void CFSKeyStoreServer::DoCancel()
	{
	switch (iAction)
		{
		case EImportKey:
		case ECreateKeyCreate:
		case EExportKey:
		case EExportEncryptedKey:
		case EChangePassphraseClearCached:
			ASSERT(iSession);
			iSession->PassphraseManager().Cancel();
			break;

		case ECreateKeyFinal:
			ASSERT(iKeyCreator);
			iKeyCreator->Cancel();
			break;

		default:
			// Nothing to do
			break;
		}
	
	CompleteClientRequest(KErrCancel);
	}

void CFSKeyStoreServer::RunL()
	{
	User::LeaveIfError(iStatus.Int()); 

	switch (iAction)
		{
		case EImportKey:
			{
			TRAPD(err, DoImportKeyL());
			if (err == KErrTooBig)
				{
				// Returned by ASN library if data is unexpected probably as a result of
				// bad import data
				err = KErrArgument;
				}
			User::LeaveIfError(err);
			CompleteClientRequest(KErrNone);
			}
		break;
		case EExportEncryptedKey:
			{
		#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			if(iUserIdentity->Id() == 0)
				{
				User::Leave(KErrAuthenticationFailure);
				}
		#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
			TPasswordManager::ExportPassword(iPassword, iStatus); 
			iAction = EExportKey;
			SetActive();
			}	
			break;
		case EExportKey:
			{
		#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			if( iUserIdentity->Id() == AuthServer::KUnknownIdentity )
				{
				User::Leave(KErrAuthenticationFailure);
				}
			if(iUserIdentity->Id() != iKeyInfo->Identity())
				{
				User::Leave(KErrNotFound);
				}
		#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
			CompleteKeyExportL(iExportingKeyEncrypted);
			}
			break;				
		
		case EDoExportKey:
			{
			CheckExportKeyPolicyL();
			
			#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
				if (iExportingKeyEncrypted)
					{
					iAction = EExportEncryptedKey;		
					}
				else
					{
					iAction = EExportKey;
					}
				
				AsyncAuthenticateL(iKeyInfo->AuthExpression(), iKeyInfo->Freshness());
				
			#else
				
				if (iExportingKeyEncrypted)
					{
					iAction = EExportEncryptedKeyGetPassphrase;
					}
				else
					{
					iAction = EExportKeyGetPassphrase;
					}
				SetActive();
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);		
			#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			}
			break;
				
		case ECreateKeyFinal:
			DoStoreKeyL();			 
			//	Check iKeyInfo was initialised for the caller
			ASSERT(iKeyInfo->HandleID() != 0);						
			CompleteClientRequest(KErrNone);
			break;
		
		#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		
		case ECreateUserKey:
			iIdentityId = iUserIdentity->Id();
			if(iIdentityId == AuthServer::KUnknownIdentity)
				{
				User::Leave(KErrAuthenticationFailure);
				}
			iKeyInfo->SetIdentity(iIdentityId);
			User::LeaveIfError(CheckCreateKeyPolicy()); 
			DoCreateKeyL();
			iAction = ECreateKeyFinal;
			
			break;
					
		case EImportUserKey:
			iIdentityId = iUserIdentity->Id();
			if( iIdentityId == AuthServer::KUnknownIdentity )
				{
				User::Leave(KErrAuthenticationFailure);
				}
			iKeyInfo->SetIdentity(iIdentityId);
			User::LeaveIfError(CheckImportKeyPolicy(iImportingEncryptedKey ? ENewKeyImportEncrypted : ENewKeyImportPlaintext));
			
			iAction = EImportKey;
			SetActive();
			if (iImportingEncryptedKey)
				{
				TPasswordManager::ImportPassword(iPassword, iStatus);
				}
			else
				{
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				}			
			break;
		
		case EDoSetAuthenticationPolicy:
			{
			CheckRangeL(iFreshness);
			iKeyData = iKeyDataManager->Lookup(iObjectId);
			if (!iKeyData)
				{
				User::Leave(KErrNotFound);
				}
				
			iKeyInfo = iKeyDataManager->ReadKeyInfoLC(*iKeyData);
			CleanupStack::Pop(iKeyInfo);
			
			// retrieve the authentication expression stored 
			// against this identity and authenticate
			// the user for changing the policy
			iAction = EAuthenticate;
			iNextAction = ESetAuthenticationPolicy;
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;
						
		case ESetAuthenticationPolicy:
			{
			iIdentityId = iUserIdentity->Id();
			WriteAuthenticationPolicyL();
			CompleteClientRequest(KErrNone);
			}
			break;
		
		case ESetAuthPolicy:
			{
			//sets the default authentication policy to iKeyInfo
			SetDefaultAuthPolicyL();
			AsyncAuthenticateL(iKeyInfo->AuthExpression(), iKeyInfo->Freshness());
			iAction = ECreateUserKey;
			}
			break;
			
		case EDoImportKey:
			{
			SetDefaultAuthPolicyL();
			AsyncAuthenticateL(iKeyInfo->AuthExpression(), iKeyInfo->Freshness());
			iAction = EImportUserKey;
			}
			break;

		case EAuthenticate:
			{
			ASSERT(iKeyInfo);
			CheckRangeL(iKeyInfo->Freshness());
			AsyncAuthenticateL(iKeyInfo->AuthExpression(), iKeyInfo->Freshness());
			iAction = iNextAction;
			iNextAction = EIdle;
			}
			break;
		#else

		case EImportOpenPrivateStream:
			ASSERT(iKeyInfo);
			GetKeystorePassphrase(EImportKey);
			break;
		
		case EChangePassphrase:
			DoChangePassphrase();
			break;
		case EChangePassphraseClearCached:
			// Make sure the passphrase we just changed is not cached anywhere
			RemoveCachedPassphrases(iKeyDataManager->DefaultPassphraseId());
			CompleteClientRequest(KErrNone);
			break;
		case EAuthOpen:
			CompleteClientRequest(KErrNone);
			break;
		case EExportKeyGetPassphrase:
			GetKeystorePassphrase(EExportKey);
			break;
		case EExportEncryptedKeyGetPassphrase:
			GetKeystorePassphrase(EExportEncryptedKey);
			break;
		case ECreateKeyCreate:
			DoCreateKeyL();	
			iAction = ECreateKeyFinal;
			break;
		case EGetPassphrase:
			User::LeaveIfError(CheckCreateKeyPolicy()); 
			GetKeystorePassphrase(ECreateKeyCreate);
			break;
		#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
		default:
			ASSERT(EFalse);
		}
	}
