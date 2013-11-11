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


#include "CKeyStoreConduit.h"
#include "cfskeystoreserver.h"
#include "CKeyStoreSession.h"
#include "OpenedKeys.h"
#include "fsdatatypes.h"
#include "fsmarshaller.h"
#include "fstokenutil.h"
#include <asymmetric.h>
#include <pbedata.h>
#include <mctkeystoremanager.h>
#include "cfskeystoreserver.h"
#include <mctkeystoreuids.h>

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <authserver/identity.h>
#include <e32debug.h>
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER


// CKeyStoreConduit ////////////////////////////////////////////////////////////

// can keep pointer to session as member
// also, do we need to pass the RMessage everywhere?

/*static*/ CKeyStoreConduit* CKeyStoreConduit::NewL(CFSKeyStoreServer& aServer)
{
	CKeyStoreConduit* self = new (ELeave) CKeyStoreConduit(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

CKeyStoreConduit::~CKeyStoreConduit()
	{
	Cancel();
	}

CKeyStoreConduit::CKeyStoreConduit(CFSKeyStoreServer& aServer)
	: CActive(EPriorityHigh), iServer(aServer), iCurrentRequest(iStatus)
	{
	}

void CKeyStoreConduit::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

void CKeyStoreConduit::ServiceRequestL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	
	TFSTokenMessages request = static_cast<TFSTokenMessages>(aMessage.Function());

	if (iCurrentRequest.OutstandingRequest()!=EIdle)
		{
		//	There is currently a request outstanding, only allow this one if it's a cancel
		if ( (request!=ECancelCreateKey) &&
			 (request!=ECancelImportKey) &&
			 (request!=ECancelImportEncryptedKey) &&
			 (request!=ECancelExportKey) &&
			 (request!=ECancelExportEncryptedKey) &&
			 (request!=ECancelRSASign) &&
			 (request!=ECancelDSASign) &&
			 (request!=ECancelDecrypt) &&
			 (request!=ECancelChangePassphrase) &&
			 (request!=ECancelAuthOpen))
			{
			User::Leave(KErrServerBusy);
			}
		}

	switch (request)
		{
	case EListKeys:
		ListL(aMessage);
		break;
	case EGetKeyInfo:
		GetKeyInfoL(aMessage);
		break;
	case ECreateKey:
		CreateKeyL(aMessage);
		break;
	case ECancelCreateKey:
		CancelCreateKey(aMessage);
		break;
	case EImportKey:
	case EImportEncryptedKey:
		ImportKeyL(aMessage);
		break;
	case ECancelImportKey:
	case ECancelImportEncryptedKey:
		CancelImportKey(aMessage);
		break;
	case EExportKey:
		ExportKeyL(aMessage);
		break;
	case ECancelExportKey:
		CancelExportKey(aMessage);
		break;
	case EExportEncryptedKey:
		ExportKeyL(aMessage);
		break;
	case ECancelExportEncryptedKey:
		CancelExportEncryptedKey(aMessage);
		break;
	case EExportPublic:
		ExportPublicL(aMessage);
		break;
	case EGetKeyLength:
		GetKeyLengthL(aMessage);
		break;
	case EDeleteKey:
		DeleteKeyL(aMessage);
		break;
	case ESetUsePolicy:
		SetUsePolicyL(aMessage);
		break;
	case ESetManagementPolicy:
		SetManagementPolicyL(aMessage);
		break;
	case EOpenKeyRepudiableRSASign:
		OpenKeyL(aMessage, aSession, KRSARepudiableSignerUID);
		break;
	case EOpenKeyRepudiableDSASign:
		OpenKeyL(aMessage, aSession, KDSARepudiableSignerUID);
		break;
	case EOpenKeyDecrypt:
		OpenKeyL(aMessage, aSession, KPrivateDecryptorUID);
		break;
	case EOpenKeyAgree:
		OpenKeyL(aMessage, aSession, KKeyAgreementUID);
		break;
	case ECloseObject:
		CloseObjectL(aMessage, aSession);
		break;
	case ERepudiableDSASign:
		RepudiableDSASignL(aMessage, aSession);
		break;
   	case ECancelDSASign:
		CancelDSASign(aMessage, aSession);
		break;			   
	case ERepudiableRSASign:
		RepudiableRSASignL(aMessage, aSession);
		break;
   	case ECancelRSASign:
		CancelRSASign(aMessage, aSession);
		break;			   
	case EDecryptText:
		DecryptL(aMessage, aSession);
		break;
   	case ECancelDecrypt:
		CancelDecrypt(aMessage, aSession);
		break;			   
	case EDHPublicKey:
		DHPublicKeyL(aMessage, aSession);
		break;
   	case EDHAgree:
		DHAgreeL(aMessage, aSession);
		break;
   	case ECancelDH:
		CancelDH(aMessage, aSession);
		break;	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	case ECreateUserKey:
		CreateUserKeyL(aMessage);
		break;
	
	case EImportUserKey:
	case EImportEncryptedUserKey:
		ImportUserKeyL(aMessage);
		break;
		
	case ESetAuthenticationPolicy:
		SetAuthenticationPolicyL(aMessage);
		break;
	
	case EGetAuthenticationPolicy:
		GetAuthenticationPolicyL(aMessage);
		break;
#else
	case EChangePassphrase:
		ChangePassphrase(aMessage);
		break;
	case ECancelChangePassphrase:
		CancelChangePassphrase(aMessage);
		break;
	case EAuthOpen:
		AuthOpen(aMessage);
		break;
	case ECancelAuthOpen:
		CancelAuthOpen(aMessage);
		break;
	case EAuthClose:
		AuthClose(aMessage);
		break;
	case EAuthTimeRemaining:
		GetTimeRemaining(aMessage);
		break;
	case ESetTimeout:
		SetTimeout(aMessage);
		break;
	case EGetTimeout:
		GetTimeout(aMessage);
		break;
	case ERelock:
		Relock(aMessage);
		break;	
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
#ifdef SYMBIAN_AUTH_SERVER
	case EUseNewKeyServer:
			IsKeyServerNewL(aMessage);
			break;
#endif // SYMBIAN_AUTH_SERVER
			
	default:	//	Should not reach here
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CKeyStoreConduit::ListL(const RMessage2& aMessage)
	{
	//	p[0] has the filter to use
	RCPointerArray<CKeyInfo> keyInfos;
	CleanupClosePushL(keyInfos);

	TPckgBuf<TCTKeyAttributeFilter> filter;
	aMessage.ReadL(0, filter);

	iServer.ListL(filter(), keyInfos);

	TInt bufSize = User::LeaveIfError(aMessage.GetDesMaxLength(2)); 
	iServer.CheckRangeL(bufSize);
	TInt reqdSize = TokenDataMarshaller::Size(keyInfos);
	
	if (bufSize >= reqdSize)
		{
		HBufC8* buffer = HBufC8::NewMaxLC(reqdSize);
		TPtr8 thePtr(buffer->Des());
		TokenDataMarshaller::Write(keyInfos, thePtr);
		aMessage.WriteL(2, thePtr);
		CleanupStack::PopAndDestroy(buffer);
		}
	else
		{
		TPckg<TInt> sizePckg(reqdSize);
		aMessage.WriteL(2, sizePckg);
		User::Leave(KErrOverflow);
		}
	CleanupStack::PopAndDestroy(&keyInfos); // keyInfos
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::GetKeyInfoL(const RMessage2& aMessage)
	{
	//	Token object to delete is in p[0]
	TInt objectId = aMessage.Int0();
	CKeyInfo* info = NULL;
	iServer.GetKeyInfoL(objectId, info);
	info->CleanupPushL();

	TInt bufferSize = User::LeaveIfError(aMessage.GetDesMaxLength(2));
	iServer.CheckRangeL(bufferSize);
	TInt requiredSize = TokenDataMarshaller::Size(*info);

	if (bufferSize >= requiredSize)
		{
		HBufC8* buffer = HBufC8::NewMaxLC(requiredSize);
		TPtr8 thePtr(buffer->Des());
		TokenDataMarshaller::Write(*info, thePtr);
		aMessage.WriteL(2, thePtr);
		CleanupStack::PopAndDestroy(buffer);
		}
	else
		{
		TPckg<TInt> theSize(requiredSize);
		aMessage.WriteL(2, theSize);
		User::Leave(KErrOverflow);
		}

	CleanupStack::PopAndDestroy(info);
	aMessage.Complete(KErrNone);
	}

/** Work out the UID of the client process. */
TUid CKeyStoreConduit::GetClientUidL(const RMessage2& aMessage) const
	{
	RThread clientThread;
	User::LeaveIfError(aMessage.Client(clientThread));
	CleanupClosePushL(clientThread);	
	RProcess clientProcess;
	User::LeaveIfError(clientThread.Process(clientProcess));
	CleanupClosePushL(clientProcess);
	TUid uid = clientProcess.Type()[2];
	CleanupStack::PopAndDestroy(2); // clientProcess, clientThread
	return uid;
	}

void CKeyStoreConduit::CreateKeyL(const RMessage2& aMessage)
	{
	ASSERT(!iKeyInfo);
	
	//	p[0] has the length of the buffer. Check our buffer is big
	//	enough, to cope with requests for keys with very long labels.

	TInt bufLength = User::LeaveIfError(aMessage.GetDesLength(1));
	iServer.CheckRangeL(bufLength);
	HBufC8* keyInfoBuf = HBufC8::NewMaxLC(bufLength);

	//	p[1] has the CKeyInfo structure required to create a key
	//	Read it and convert from descriptor back to a CKeyInfo
	TPtr8 thePtr(keyInfoBuf->Des());
	thePtr.FillZ();
	
	aMessage.ReadL(1, thePtr);
	TokenDataMarshaller::ReadL(*keyInfoBuf, iKeyInfo);
	CleanupStack::PopAndDestroy(keyInfoBuf);
	
	iCurrentRequest.Set(ECreateKey, aMessage);
	iStatus = KRequestPending;
	SetActive();
	iServer.CreateKey(*iKeyInfo, iStatus);
	}

#ifdef SYMBIAN_AUTH_SERVER
void CKeyStoreConduit::IsKeyServerNewL(const RMessage2& aMessage)
	{
	TBool newKeyServer = EFalse;
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	newKeyServer = ETrue;
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	TPckg<TBool> idNewKeyServer(newKeyServer);
	aMessage.WriteL(1,idNewKeyServer);
	aMessage.Complete(KErrNone);
	}
#endif // SYMBIAN_AUTH_SERVER

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CKeyStoreConduit::CreateUserKeyL(const RMessage2& aMessage)
	{
	ASSERT(!iKeyInfo);
		
	//	p[0] has the length of the buffer. Check our buffer is big
	//	enough, to cope with requests for keys with very long labels.

	TInt bufLength = User::LeaveIfError(aMessage.GetDesLength(1));
	iServer.CheckRangeL(bufLength);
	
	TInt authExprLength = User::LeaveIfError(aMessage.GetDesLength(2));
	iServer.CheckRangeL(authExprLength);
	HBufC* authExprBuf = HBufC::NewMaxLC(authExprLength);

	//	p[2] has the authentication expression
	TPtr authExprPtr(authExprBuf->Des());
	authExprPtr.FillZ();
	
	aMessage.ReadL(2, authExprPtr);
	
	TInt freshness = aMessage.Int3();
	
	// the iKeyInfo buffer should be read at the last as in 
	// case of OOM this memory would not get freed.
	HBufC8* keyInfoBuf = HBufC8::NewMaxLC(bufLength);

	//	p[1] has the CKeyInfo structure required to create a key
	//	Read it and convert from descriptor back to a CKeyInfo
	TPtr8 thePtr(keyInfoBuf->Des());
	thePtr.FillZ();

	aMessage.ReadL(1, thePtr);
	TokenDataMarshaller::ReadL(*keyInfoBuf, iKeyInfo);
	CleanupStack::PopAndDestroy(keyInfoBuf);
			
	iCurrentRequest.Set(ECreateUserKey, aMessage);
	
	SetActive();
	iServer.CreateUserKey(*iKeyInfo, *authExprBuf, freshness, iStatus);
	CleanupStack::PopAndDestroy(authExprBuf);
	}

void CKeyStoreConduit::ImportUserKeyL(const RMessage2& aMessage)
	{
	ASSERT(!iImportKey);
	ASSERT(!iKeyInfo);
	
	// p[0] has the descriptor containing the PKCS8 object (may or may not be encrypted)
	TInt keyLen = User::LeaveIfError(aMessage.GetDesLength(0));
	iServer.CheckRangeL(keyLen);
	
	HBufC8* importBuf = HBufC8::NewMaxLC(keyLen);
	TPtr8 theKeyData(importBuf->Des());
	theKeyData.FillZ();
	aMessage.ReadL(0, theKeyData);

		
	TInt bufLen = User::LeaveIfError(aMessage.GetDesLength(1));
	iServer.CheckRangeL(bufLen);
	
	// retrieve the authentication expression
	TInt authExprLength = User::LeaveIfError(aMessage.GetDesLength(2));
	iServer.CheckRangeL(authExprLength);
	HBufC* authExprBuf = HBufC::NewMaxLC(authExprLength);

	//	p[2] has the authentication expression
	TPtr authExprPtr(authExprBuf->Des());
	authExprPtr.FillZ();
	
	aMessage.ReadL(2, authExprPtr);
	
	// retrieve the freshness value
	TInt freshness = aMessage.Int3();
	
	HBufC8* keyInfoBuf = HBufC8::NewMaxLC(bufLen);

	// p[1] has the CKeyInfo structure required to create a key
	// Read it and convert from descriptor back to a CKeyInfo
	TPtr8 thePtr(keyInfoBuf->Des());
	thePtr.FillZ();
	aMessage.ReadL(1, thePtr);

	TokenDataMarshaller::ReadL(*keyInfoBuf, iKeyInfo);
	CleanupStack::PopAndDestroy(keyInfoBuf);

	iImportKey = importBuf;
	iCurrentRequest.Set(static_cast<TFSTokenMessages>(aMessage.Function()), aMessage);
	
	TBool isEncrypted = (aMessage.Function() == EImportEncryptedUserKey);
	SetActive();
	iServer.ImportUserKey(*iImportKey, *iKeyInfo, isEncrypted, *authExprBuf, freshness, iStatus);
	CleanupStack::PopAndDestroy(authExprBuf);
	CleanupStack::Pop(importBuf);
}

void CKeyStoreConduit::SetAuthenticationPolicyL(const RMessage2& aMessage)
	{
	TInt objectId = aMessage.Int0();
	
	TInt authExprLen = User::LeaveIfError(aMessage.GetDesLength(1));
	iServer.CheckRangeL(authExprLen);
	HBufC* authExpr = HBufC::NewMaxLC(authExprLen);
	TPtr authExprPtr(authExpr->Des());
	authExprPtr.FillZ();
	aMessage.ReadL(1, authExprPtr);

	TInt freshness = aMessage.Int2();
	iCurrentRequest.Set(ESetAuthenticationPolicy, aMessage);
		
	SetActive();
	// ownership of authExpr is with the caller
	iServer.SetAuthenticationPolicy(objectId, authExpr, freshness, iStatus);
	CleanupStack::Pop(authExpr);
	}

void CKeyStoreConduit::GetAuthenticationPolicyL(const RMessage2& aMessage)
	{
	TInt objectId = aMessage.Int0();
	HBufC* authExpression = iServer.AuthExpressionL(objectId);
	CleanupStack::PushL(authExpression);
	TInt freshness = iServer.FreshnessL(objectId);
	TPckg<TInt> pckgFreshness(freshness); 
	aMessage.WriteL(1, *authExpression);
	aMessage.WriteL(2, pckgFreshness);
	CleanupStack::PopAndDestroy(authExpression);
	aMessage.Complete(KErrNone);
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CKeyStoreConduit::CancelCreateKey(const RMessage2& aMessage)
	{
	if (iCurrentRequest.OutstandingRequest() == ECreateKey)
		{
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::ImportKeyL(const RMessage2& aMessage)
	{
	ASSERT(!iImportKey);
	ASSERT(!iKeyInfo);
	
	// p[0] has the descriptor containing the PKCS8 object (may or may not be encrypted)
	TInt keyLen = User::LeaveIfError(aMessage.GetDesLength(0));
	iServer.CheckRangeL(keyLen);
	
	HBufC8* importBuf = HBufC8::NewMaxLC(keyLen);
	TPtr8 theKeyData(importBuf->Des());
	theKeyData.FillZ();
	aMessage.ReadL(0, theKeyData);

	TInt bufLen = User::LeaveIfError(aMessage.GetDesLength(1));
	iServer.CheckRangeL(bufLen);
	HBufC8* keyInfoBuf = HBufC8::NewMaxLC(bufLen);

	// p[1] has the CKeyInfo structure required to create a key
	// Read it and convert from descriptor back to a CKeyInfo
	TPtr8 thePtr(keyInfoBuf->Des());
	thePtr.FillZ();
	aMessage.ReadL(1, thePtr);

	TokenDataMarshaller::ReadL(*keyInfoBuf, iKeyInfo);
	CleanupStack::PopAndDestroy(keyInfoBuf);

	iImportKey = importBuf;
	CleanupStack::Pop(importBuf);
	
	iCurrentRequest.Set(static_cast<TFSTokenMessages>(aMessage.Function()), aMessage);
	iStatus = KRequestPending;
	SetActive();

	TBool isEncrypted = (aMessage.Function() == EImportEncryptedKey);
	iServer.ImportKey(*iImportKey, *iKeyInfo, isEncrypted, iStatus);
}

void CKeyStoreConduit::CancelImportKey(const RMessage2& aMessage)
	{
	if (iCurrentRequest.OutstandingRequest() == EImportKey ||
		iCurrentRequest.OutstandingRequest() == EImportEncryptedKey)
		{
		Cancel();
		}
	aMessage.Complete(KErrNone); 
	}

void CKeyStoreConduit::GetKeyLengthL(const RMessage2& aMessage)
	{
	TInt length = iServer.GetKeyLengthL(aMessage.Int0());
	ASSERT(length > 0);
	aMessage.Complete(length);		
	}

void CKeyStoreConduit::CancelExportKey(const RMessage2& aMessage)
	{
	if (iCurrentRequest.OutstandingRequest() == EExportKey)
		{
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::CancelExportEncryptedKey(const RMessage2& aMessage)
	{
	if (iCurrentRequest.OutstandingRequest() == EExportEncryptedKey)
		{
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::ExportKeyL(const RMessage2& aMessage)
	{
	ASSERT(!iExportBuf);
	ASSERT(!iPbeParams);
	
	TInt objectId = aMessage.Int0();
	TInt bufLen = User::LeaveIfError(aMessage.GetDesMaxLength(1));  // #1 IPC argument is the request Ptr
	iServer.CheckRangeL(bufLen);
	
	HBufC8* exportBuf =  HBufC8::NewMaxLC(bufLen);
	TPtr8 temp(exportBuf->Des());
 	temp.FillZ();

	if (aMessage.Function() == EExportEncryptedKey)
		{
		TInt paramsLen = User::LeaveIfError(aMessage.GetDesLength(2));
		HBufC8* pbeParamsBuf = HBufC8::NewMaxLC(paramsLen);
		TPtr8 thePtr(pbeParamsBuf->Des());
		thePtr.FillZ();
		aMessage.ReadL(2, thePtr);

		TokenDataMarshaller::ReadL(*pbeParamsBuf, iPbeParams);	
		CleanupStack::PopAndDestroy(pbeParamsBuf);	
		}

	// No more leaves

	iExportBuf = exportBuf;
	CleanupStack::Pop(exportBuf);
	
	iCurrentRequest.Set(static_cast<TFSTokenMessages>(aMessage.Function()), aMessage);	
	iStatus = KRequestPending;
	SetActive();		

	if (aMessage.Function() == EExportKey) 
		{
		iServer.ExportKey(objectId, temp, iStatus);
		}
	else if (aMessage.Function() == EExportEncryptedKey)
		{
		iServer.ExportEncryptedKey(objectId, temp, *iPbeParams, iStatus);
		}				
	}

void CKeyStoreConduit::ExportPublicL(const RMessage2& aMessage)
	{
	TInt objectId = aMessage.Int0();
	TInt bufLen = User::LeaveIfError(aMessage.GetDesMaxLength(1));
	iServer.CheckRangeL(bufLen);
	
	HBufC8* exportBuf = HBufC8::NewMaxLC(bufLen);
	TPtr8 ptr(exportBuf->Des());
	ptr.FillZ();
	iServer.ExportPublicL(objectId, ptr);
	aMessage.WriteL(1, ptr);

	CleanupStack::PopAndDestroy(exportBuf);
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::DeleteKeyL(const RMessage2& aMessage)
	{
	TInt objectId = aMessage.Int0();
	iServer.DeleteKeyL(objectId);
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::SetUsePolicyL(const RMessage2& aMessage)
	{
	TInt objectId = aMessage.Int0();
	TSecurityPolicyBuf policyBuf;

	aMessage.ReadL(1, policyBuf);
	
	iServer.SetUsePolicyL(objectId, policyBuf());
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::SetManagementPolicyL(const RMessage2& aMessage)
	{
	TInt objectId = aMessage.Int0();
	TSecurityPolicyBuf policyBuf;

	aMessage.ReadL(1, policyBuf);
	
	iServer.SetManagementPolicyL(objectId, policyBuf());
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::OpenKeyL(const RMessage2& aMessage, CKeyStoreSession& aSession, TUid aType)
	{	
	COpenedKey* openedKey = iServer.OpenKeyL(aMessage.Int0(), aType);
	CleanupStack::PushL(openedKey);
	
	const TDesC& label = openedKey->Label();

	TInt writeBufLen = User::LeaveIfError(aMessage.GetDesMaxLength(3));
	TInt reqdLength = label.Length();
	if (writeBufLen < reqdLength)
		{
		// We're writing into a TDes16 so we can't use TPckg<TInt>
		TPtrC sizePtr(reinterpret_cast<TUint16*>(&reqdLength), 2);
		aMessage.WriteL(3, sizePtr);
		User::Leave(KErrOverflow);
		}
	
	aMessage.WriteL(3, label);

	TInt handle = aSession.AddOpenedKeyL(*openedKey);
	
	TPckg<TInt> handlePckg(handle);
	TRAPD(err, aMessage.WriteL(1, handlePckg));

	if (err != KErrNone)
		{
		aSession.RemoveOpenedKeyL(handle);
		User::Leave(err);
		}

	CleanupStack::Pop(openedKey); // now owned by session
	aMessage.Complete(KErrNone);
	}
	
void CKeyStoreConduit::RepudiableRSASignL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	ASSERT(iOpenedKey == NULL);
	TPtr8 thePtr(0,0);
	iOpenedKey = ProcessL(aMessage, aSession, KRSARepudiableSignerUID, ERepudiableRSASign, thePtr);
	static_cast<CRSARepudiableSigner*>(iOpenedKey)->Sign(thePtr, iRSASignature, iStatus);
	}

void CKeyStoreConduit::CancelRSASign(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	if (iCurrentRequest.OutstandingRequest() == ERepudiableRSASign)
		{
		aSession.PassphraseManager().Cancel();
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::RepudiableDSASignL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	ASSERT(iOpenedKey == NULL);
	TPtr8 thePtr(0,0);
	iOpenedKey = ProcessL(aMessage, aSession, KDSARepudiableSignerUID, ERepudiableDSASign, thePtr);
	static_cast<CDSARepudiableSigner*>(iOpenedKey)->Sign(thePtr, iDSASignature, iStatus);
	}

void CKeyStoreConduit::CancelDSASign(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	if (iCurrentRequest.OutstandingRequest() == ERepudiableDSASign)
		{
		aSession.PassphraseManager().Cancel();
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::DecryptL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	ASSERT(iOpenedKey == NULL);
	TPtr8 thePtr(0,0);
	iOpenedKey = ProcessL(aMessage, aSession, KPrivateDecryptorUID, EDecryptText, thePtr);
	static_cast<CFSRSADecryptor*>(iOpenedKey)->Decrypt(thePtr, iPlaintext, iStatus);
	}

void CKeyStoreConduit::CancelDecrypt(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	if (iCurrentRequest.OutstandingRequest() == EDecryptText)
		{
		aSession.PassphraseManager().Cancel();
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

COpenedKey* CKeyStoreConduit::ProcessL(const RMessage2& aMessage,
									   CKeyStoreSession& aSession,
									   const TUid& aCKeyInfoID,
									   const TFSTokenMessages& aState,
									   TPtr8& aTextPtr)
	{
	COpenedKey* object = aSession.OpenedKey(aMessage.Int0());
	if (!object)
		{
		User::Leave(KErrNotFound);
		}
	
	if (aCKeyInfoID != object->Type())
		{
		User::Leave(KErrAccessDenied);
		}

	TInt length = User::LeaveIfError(aMessage.GetDesLength(1));
	iText = HBufC8::NewL(length);
	aTextPtr.Set(iText->Des());
	aMessage.ReadL(1, aTextPtr);

	iCurrentRequest.Set(aState, aMessage);
	iStatus=KRequestPending;
	SetActive();
	
	return object;
	}

void CKeyStoreConduit::DHPublicKeyL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	// 0: Object id
	// 1: DH paramters

	ASSERT(iDHParams == NULL);
	ASSERT(iOpenedKey == NULL);
	TRAPD(err, DoDHPublicKeyL(aMessage, aSession));
	if (err != KErrNone)
		{
		iOpenedKey = NULL;
		delete iDHParams;
		iDHParams = NULL;
		User::Leave(err);
		}
	}

void CKeyStoreConduit::DoDHPublicKeyL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	iOpenedKey = aSession.OpenedKey(aMessage.Int0());
	if (!iOpenedKey)
		{
		User::Leave(KErrNotFound);
		}

	//Check that this is a DH object
	if (KKeyAgreementUID != iOpenedKey->Type())
		{
		iOpenedKey = NULL;
		User::Leave(KErrAccessDenied);
		}
	TInt bufLen = User::LeaveIfError(aMessage.GetDesLength(1));
	iServer.CheckRangeL(bufLen);
	HBufC8* clientBuf = HBufC8::NewMaxLC(bufLen);
	TPtr8 clientPtr = clientBuf->Des();
	aMessage.ReadL(1, clientPtr);
	TokenDataMarshaller::ReadL(*clientBuf, iDHParams);
	CleanupStack::PopAndDestroy(clientBuf);

	static_cast<CDHAgreement*>(iOpenedKey)->PublicKey(*iDHParams, iDHPublicKeyOut, iStatus);

	iCurrentRequest.Set(EDHPublicKey, aMessage);
	iStatus=KRequestPending;
	SetActive();
	}

void CKeyStoreConduit::FinishDHPublicKeyL()
	{
	// Client buffer must be big enough for result because it had the DH
	// parameters in it
	TInt reqdSize = TokenDataMarshaller::Size(iDHPublicKeyOut);
	ASSERT(reqdSize <= iCurrentRequest.Message().GetDesMaxLength(1));

	HBufC8* clientBuf = HBufC8::NewMaxLC(reqdSize);
	TPtr8 clientPtr = clientBuf->Des();
	TokenDataMarshaller::WriteL(iDHPublicKeyOut, clientPtr);
	iCurrentRequest.Message().WriteL(1, clientPtr);
	CleanupStack::PopAndDestroy(clientBuf);
	}

void CKeyStoreConduit::DHAgreeL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	// 0: Object id
	// 1: DH public key

	ASSERT(iOpenedKey == NULL);
	ASSERT(iDHPublicKey == NULL);
	TRAPD(err, DoDHAgreeL(aMessage, aSession));
	if (err != KErrNone)
		{
		iOpenedKey = NULL;
		delete iDHPublicKey;
		iDHPublicKey = NULL;
		User::Leave(err);
		}
	}

void CKeyStoreConduit::DoDHAgreeL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	
	COpenedKey* iOpenedKey = aSession.OpenedKey(aMessage.Int0());
	if (!iOpenedKey)
		{
		User::Leave(KErrNotFound);
		}

	//Check that this is a DH object
	if (KKeyAgreementUID != iOpenedKey->Type())
		{
		iOpenedKey = NULL;
		User::Leave(KErrAccessDenied);
		}
	
	TInt bufLen = User::LeaveIfError(aMessage.GetDesLength(1)); 
	iServer.CheckRangeL(bufLen);
	HBufC8* clientBuf = HBufC8::NewMaxLC(bufLen);
	TPtr8 clientPtr = clientBuf->Des();
	aMessage.ReadL(1, clientPtr);
	TokenDataMarshaller::ReadL(*clientBuf, iDHPublicKey);
	CleanupStack::PopAndDestroy(clientBuf);
	
	static_cast<CDHAgreement*>(iOpenedKey)->Agree(*iDHPublicKey, iDHAgreedKeyOut, iStatus);

	iCurrentRequest.Set(EDHAgree, aMessage);
	iStatus=KRequestPending;
	SetActive();
	}

void CKeyStoreConduit::FinishDHAgreeL()
	{
	// Client buffer must be big enough for result because it had the DH
	// public key in it
	ASSERT(iDHAgreedKeyOut->Length() <= iCurrentRequest.Message().GetDesMaxLength(1));

	iCurrentRequest.Message().WriteL(1, *iDHAgreedKeyOut);
	}

void CKeyStoreConduit::CancelDH(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	if (iCurrentRequest.OutstandingRequest() == EDHPublicKey ||
		iCurrentRequest.OutstandingRequest() == EDHAgree)
		{
		aSession.PassphraseManager().Cancel();
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::CloseObjectL(const RMessage2& aMessage, CKeyStoreSession& aSession)
	{
	aSession.RemoveOpenedKeyL(aMessage.Int0());
	aMessage.Complete(KErrNone);
	}

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CKeyStoreConduit::ChangePassphrase(const RMessage2& aMessage)
	{
	iCurrentRequest.Set(EChangePassphrase, aMessage);
	iStatus=KRequestPending;
	SetActive();
	iServer.ChangePassphrase(iStatus);
	}

void CKeyStoreConduit::CancelChangePassphrase(const RMessage2& aMessage)
	{
	if (iCurrentRequest.OutstandingRequest() == EChangePassphrase)
		{
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::AuthOpen(const RMessage2& aMessage)
	{
	iCurrentRequest.Set(EAuthOpen, aMessage);
	iStatus=KRequestPending;
	SetActive();
	iServer.AuthOpen(iStatus);
	}

void CKeyStoreConduit::CancelAuthOpen(const RMessage2& aMessage)
	{
	if (iCurrentRequest.OutstandingRequest() == EAuthOpen)
		{
		Cancel();
		}
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::AuthClose(const RMessage2& aMessage)
	{
	iServer.AuthClose();
	aMessage.Complete(KErrNone);
	}

void CKeyStoreConduit::GetTimeRemaining(const RMessage2& aMessage)
	{
	TInt result = 0;
	TRAPD(err, result = iServer.GetTimeRemainingL());
	if (err != KErrNone)
		{
		result = err;
		}
	aMessage.Complete(result);
	}

void CKeyStoreConduit::SetTimeout(const RMessage2& aMessage)
	{
	// p[1] is new timeout
	TInt timeout = aMessage.Int1();
	TRAPD(err, iServer.SetTimeoutL(timeout));
	aMessage.Complete(err);
	}

void CKeyStoreConduit::GetTimeout(const RMessage2& aMessage)
	{
	TInt result = iServer.GetTimeout();
	aMessage.Complete(result);
	}

void CKeyStoreConduit::Relock(const RMessage2& aMessage)
	{
	iServer.Relock();
	aMessage.Complete(KErrNone);
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

//
//	CActive functions
//
void CKeyStoreConduit::DoCancel()
	{
	switch (iCurrentRequest.OutstandingRequest())
		{
		case ECreateKey:
			iServer.CancelCreateKey();
			break;

		case ERepudiableRSASign:
		case ERepudiableDSASign:
		case EDecryptText:
		case EDHAgree:
		case EDHPublicKey:
			if (iOpenedKey)
				{				
				iOpenedKey->Cancel();
				}
			break;

		case EImportKey:
		case EImportEncryptedKey:
			iServer.CancelImportKey();
			break;

		case EExportKey:
			iServer.CancelExportKey();
			break;

		case EExportEncryptedKey:
			iServer.CancelExportEncryptedKey();
			break;
			
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		case ECreateUserKey:
			iServer.CancelCreateKey();
			break;
		case EImportUserKey:
		case EImportEncryptedUserKey:
			iServer.CancelImportKey();
#else
		case EChangePassphrase:
			iServer.CancelChangePassphrase();
			break;

		case EAuthOpen:
			iServer.CancelAuthOpen();
			break;

			
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
		default:
			// Nothing to do
			break;
		}
	
	Cleanup();
	iCurrentRequest.Cancel();
	}


/**
 * The token interface has completed the request - munge any return parameters
 * and write back to client.
 */
void CKeyStoreConduit::RunL()
	{
	// Handle errors from server
	User::LeaveIfError(iStatus.Int());

	switch (iCurrentRequest.OutstandingRequest())
		{
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		case ESetAuthenticationPolicy:
			break;
		case ECreateUserKey:
		case EImportUserKey:
		case EImportEncryptedUserKey:
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER	
		case ECreateKey:
		case EImportKey:
		case EImportEncryptedKey:
			{
			//	Marshal TKeyUpdate to client - the client's buffer will be large
			//	enough as it passed us a CCTKeyInfo in the first place
			ASSERT(iKeyInfo);
			TKeyUpdate update;
			update.iReference = iKeyInfo->HandleID();
			update.iId = iKeyInfo->ID();
			update.iSize = iKeyInfo->Size();
			update.iAlgorithm = iKeyInfo->Algorithm();
			TPckg<TKeyUpdate> pckg(update);
			iCurrentRequest.Message().WriteL(1, pckg);
			break;
			}
		case EExportKey:
		case EExportEncryptedKey:
			{
			ASSERT(iExportBuf);
			TPtr8 clientPtr(iExportBuf->Des());
			iCurrentRequest.Message().WriteL(1, clientPtr);
			break;
			}
		case ERepudiableDSASign:
			{
			ASSERT(iDSASignature);
			
			TInt length = User::LeaveIfError(iCurrentRequest.Message().GetDesMaxLength(2));
			TInt reqdLength = TokenDataMarshaller::Size(*iDSASignature);
			if (length < reqdLength)
				{
				User::Leave(KErrOverflow);
				}
			
			HBufC8* clientBuffer = HBufC8::NewLC(reqdLength);
			TPtr8 ptr(clientBuffer->Des());
			TokenDataMarshaller::WriteL(*iDSASignature, ptr);
			iCurrentRequest.Message().WriteL(2, ptr);
			CleanupStack::PopAndDestroy(clientBuffer);
			break;
			}
		case ERepudiableRSASign:
			{
			ASSERT(iRSASignature);
			TInt length = User::LeaveIfError(iCurrentRequest.Message().GetDesMaxLength(2));
			TInt reqdLength = TokenDataMarshaller::Size(*iRSASignature);
			if (length < reqdLength)
				{
				User::Leave(KErrOverflow);
				}

			HBufC8* clientBuffer = HBufC8::NewLC(reqdLength);
			TPtr8 ptr(clientBuffer->Des());
			TokenDataMarshaller::WriteL(*iRSASignature, ptr);
			iCurrentRequest.Message().WriteL(2, ptr);
			CleanupStack::PopAndDestroy(clientBuffer);
			break;
			}
		case EDecryptText:
			{
			ASSERT(iPlaintext);
			TInt length = User::LeaveIfError(iCurrentRequest.Message().GetDesMaxLength(2));
			if (length < iPlaintext->Length())
				{
				User::Leave(KErrOverflow);
				}
			iCurrentRequest.Message().WriteL(2, *iPlaintext);
			break;
			}
			
		case EDHPublicKey:
			FinishDHPublicKeyL();
			break;
		case EDHAgree:
			FinishDHAgreeL();
			break;
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		case EChangePassphrase:
		case EAuthOpen:
			// Nothing to do
			break;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			
		default:
			__ASSERT_DEBUG(EFalse, PanicServer(EPanicInvalidRequest));
			User::Leave(KErrNotSupported);
		}

	Cleanup();
	iCurrentRequest.Complete(KErrNone);
	}

/**
 * Cleans up data members used in processing a client request.  Called whenenver a
 * request is completed, either from RunL, RunError or indirectly from DoCancel.
 */

void CKeyStoreConduit::Cleanup()
	{
	delete iKeyInfo; 		 iKeyInfo = NULL;
	delete iImportKey; 		 iImportKey = NULL;
	delete iText; 			 iText = NULL;
	delete iDSASignature;	 iDSASignature = NULL;
	delete iRSASignature;	 iRSASignature = NULL;
	delete iExportBuf; 		 iExportBuf = NULL;
	delete iPbeParams;		 iPbeParams = NULL;
	delete iDHParams;		 iDHParams = NULL;
	iDHPublicKeyOut.Close();
	delete iDHPublicKey;	 iDHPublicKey = NULL;
	delete iDHAgreedKeyOut;	 iDHAgreedKeyOut = NULL;
	delete iPlaintext;		 iPlaintext = NULL;
	iOpenedKey = NULL;
	}

TInt CKeyStoreConduit::RunError(TInt aError)
	{
	Cleanup();

	//	Handle error by completing client appropriately
	iCurrentRequest.Complete(aError);

	return (KErrNone);
	}

// CKeyStoreConduit::TAsyncRequest /////////////////////////////////////////////

CKeyStoreConduit::TAsyncRequest::TAsyncRequest(TRequestStatus& aStatus) :
	iOwnerStatus(aStatus)
	{
	}

CKeyStoreConduit::TAsyncRequest::~TAsyncRequest()
	{
	__ASSERT_DEBUG(EIdle==iRequest, PanicServer(EPanicRequestOutstanding));
	}

void CKeyStoreConduit::TAsyncRequest::Set(TFSTokenMessages aRequest, const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(EIdle==iRequest, PanicServer(EPanicRequestOutstanding));

	iOwnerStatus = KRequestPending;
	iRequest = aRequest;
	iMessage = aMessage;
	}

void CKeyStoreConduit::TAsyncRequest::Complete(TInt aResult)
	{
	iMessage.Complete(aResult);
	iRequest = EIdle;
	}

void CKeyStoreConduit::TAsyncRequest::Cancel()
	{//	Complete outstanding request with KErrCancel
	iMessage.Complete(KErrCancel);
	iRequest = EIdle;
	}


