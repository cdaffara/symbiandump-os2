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
*
*/


#include "CFSCertStoreClient.h"
#include "CFSTokenTypeClient.h"
#include "CFStokenclient.h"
#include "clientutils.h"

// This is sized for roughly five cert infos, to ensure that the overflow
// mechanism gets tested.
const TInt KDefaultBufferSize = 256;

MCTTokenInterface* CFSCertStoreClient::NewWritableInterfaceL(MCTToken& aToken, 
															 RFileStoreClientSession& aClient)
	{
	// Destroyed by MCTTokenInterface::DoRelease() (no refcounting)
	CFSCertStoreClient* me = new (ELeave) CFSCertStoreClient(KInterfaceWritableCertStore, 
															 aToken, 
															 aClient);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

MCTTokenInterface* CFSCertStoreClient::NewReadableInterfaceL(MCTToken& aToken, 
															 RFileStoreClientSession& aClient)
	{
	// Destroyed by MCTTokenInterface::DoRelease() (no refcounting)
	CFSCertStoreClient* me = new (ELeave) CFSCertStoreClient(KInterfaceCertStore,
															 aToken, 
															 aClient);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
	}

CFSCertStoreClient::CFSCertStoreClient(TInt aUID,
									   MCTToken& aToken, 
									   RFileStoreClientSession& aClient) :
	CFSClient(aUID, aToken, aClient)	
	{
	}

CFSCertStoreClient::~CFSCertStoreClient()
	{
	}

void CFSCertStoreClient::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

void CFSCertStoreClient::DoRelease()
	{
	MCTTokenInterface::DoRelease();
	}

MCTToken& CFSCertStoreClient::Token()
	{
	return iToken;
	}

//	*********************************************************************************
//	CActive implementation
//	*********************************************************************************
	
void CFSCertStoreClient::RunL()
	{
	// Never active, so should never get called
	FSTokenPanic(ENoRequestOutstanding);
	}

//	*********************************************************************************
//	MCTCertStore interface
//	*********************************************************************************

void CFSCertStoreClient::List(RMPointerArray<CCTCertInfo>& aCerts, 
							  const CCertAttributeFilter& aFilter,
							  TRequestStatus& aStatus)
	{
	TRAPD(r, DoListL(aCerts, aFilter));
	FreeRequestBuffer();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, r);	
	}

void CFSCertStoreClient::DoListL(RMPointerArray<CCTCertInfo>& aCerts, 
								 const CCertAttributeFilter& aFilter)
	{
	HBufC8* filterBuf = HBufC8::NewLC(TokenDataMarshaller::Size(aFilter));
	TPtr8 ptr(filterBuf->Des());
	TokenDataMarshaller::WriteL(aFilter, ptr);
	TIpcArgs args(0, filterBuf, 0, &iRequestPtr);
	SendSyncRequestAndHandleOverflowL(EListCerts, KDefaultBufferSize, args);
	CleanupStack::PopAndDestroy(filterBuf);
	TokenDataMarshaller::ReadL(iRequestPtr, iToken, aCerts);
	}

void CFSCertStoreClient::CancelList()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::GetCert(CCTCertInfo*& aCertInfo, 
								 const TCTTokenObjectHandle& aHandle,
								 TRequestStatus& aStatus)
	{
	TRAPD(r, DoGetCertL(aCertInfo, aHandle));
	FreeRequestBuffer();
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, r);	
	}

void CFSCertStoreClient::DoGetCertL(CCTCertInfo*& aCertInfo, 
									const TCTTokenObjectHandle& aHandle)
	{
	User::LeaveIfError(AllocRequestBuffer(sizeof(CCTCertInfo))); // assumes serialized size <= sizeof class
	TIpcArgs args(aHandle.iObjectId, 0, 0, &iRequestPtr);
	User::LeaveIfError(iClient.SendRequest(EGetCert, args));
	TokenDataMarshaller::ReadL(iRequestPtr, iToken, aCertInfo);
	}

void CFSCertStoreClient::CancelGetCert()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::Applications(const CCTCertInfo& aCertInfo, 
									  RArray<TUid>& aApplications,
									  TRequestStatus& aStatus)
	{
	TRAPD(r, DoApplicationsL(aCertInfo, aApplications));
	FreeRequestBuffer();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, r);	
	}

void CFSCertStoreClient::DoApplicationsL(const CCTCertInfo& aCertInfo, 
										 RArray<TUid>& aApplications)
	{
	TIpcArgs args(aCertInfo.Handle().iObjectId, 0, 0, &iRequestPtr);
	SendSyncRequestAndHandleOverflowL(EApplications, KDefaultBufferSize, args);
	TokenDataMarshaller::ReadL(iRequestPtr, aApplications);			  
	}

void CFSCertStoreClient::CancelApplications()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication, 
									  TBool& aIsApplicable, TRequestStatus& aStatus)
	{
	TIpcArgs args(aCertInfo.Handle().iObjectId, 0, aApplication.iUid);
	TInt r = iClient.SendRequest(EIsApplicable, args);	
	if (r >= 0)
		{
		aIsApplicable = !!r;  // Convert TInt to TBool
		r = KErrNone;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, r);	
	}

void CFSCertStoreClient::CancelIsApplicable()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted, 
								 TRequestStatus& aStatus)
	{
	TIpcArgs args(aCertInfo.Handle().iObjectId);
	TInt r = iClient.SendRequest(ETrusted, args);	
	if (r >= 0)
		{
		aTrusted = !!r;  // Convert TInt to TBool
		r = KErrNone;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, r);	
	}

void CFSCertStoreClient::CancelTrusted()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert,
								  TRequestStatus& aStatus)
	{
	TInt err = DoRetrieve(aCertInfo, aEncodedCert);
	FreeRequestBuffer();
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, err);
	}

TInt CFSCertStoreClient::DoRetrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert)
	{
	if (aCertInfo.Size() <= 0)
		{
		return KErrArgument;
		}
	
	if (aEncodedCert.MaxSize() < aCertInfo.Size())
		{
		return KErrOverflow;
		}
	
	TInt r = AllocRequestBuffer(aEncodedCert.MaxSize());	
	if (KErrNone!=r)
		{
		return r;
		}

	TIpcArgs args(aCertInfo.Handle().iObjectId, 0, 0, &iRequestPtr);
	r = iClient.SendRequest(ERetrieve, args);
	if (r == KErrNone)
		{
		aEncodedCert.Copy(iRequestPtr);
		}

	return r;
	}

void CFSCertStoreClient::CancelRetrieve()
	{
	// synchronous, nothing to do
	}

//	*********************************************************************************
//	MCTWritableCertStore interface
//	*********************************************************************************

void CFSCertStoreClient::Add(const TDesC& aLabel, TCertificateFormat aFormat,
							 TCertificateOwnerType aCertificateOwnerType, 
							 const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
							 const TDesC8& aCert, TRequestStatus& aStatus)
	{
	Add(aLabel, aFormat, aCertificateOwnerType, aSubjectKeyId, aIssuerKeyId, aCert, 
		ETrue, aStatus);
	}	

void CFSCertStoreClient::Add(const TDesC& aLabel, TCertificateFormat aFormat,
							 TCertificateOwnerType aCertificateOwnerType, 
							 const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
							 const TDesC8& aCert, const TBool aDeletable,
							 TRequestStatus& aStatus)
	{
	TRequestStatus* stat = &aStatus;
	if (aLabel.Length() > KMaxCertLabelLength)
		{
		//	Too long to be a label
		User::RequestComplete(stat, KErrOverflow);
		return;
		}
	
	TPckgBuf<TAddCertDataStruct> data;
	data().iLabel.Zero();
	data().iSubjectKeyId.Zero();
	data().iIssuerKeyId.Zero();		
	data().iLabel.Copy(aLabel);		
	data().iFormat = aFormat;
	data().iCertificateOwnerType = aCertificateOwnerType;
	if (aSubjectKeyId)
		{
		data().iSubjectKeyId.Copy(*aSubjectKeyId);
		}
	if (aIssuerKeyId)
		{
		data().iIssuerKeyId.Copy(*aIssuerKeyId);
		}
	data().iDeletable = aDeletable;

	TInt r = AllocRequestBuffer(aCert.Size());
	
	if (r!=KErrNone)
		{
		User::RequestComplete(stat, r);
		return;
		}

	iRequestPtr.FillZ();
	iRequestPtr.Copy(aCert);

	TIpcArgs args(0, &data, 0, &iRequestPtr);
	r = iClient.SendRequest(EAddCert, args);

	FreeRequestBuffer();
	User::RequestComplete(stat, r);
	}	

void CFSCertStoreClient::CancelAdd()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus)
	{
	TIpcArgs args(aCertInfo.Handle().iObjectId);
	TInt r = iClient.SendRequest(ERemoveCert, args);	
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, r);
	}

void CFSCertStoreClient::CancelRemove()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::SetApplicability(const CCTCertInfo& aCertInfo, 
										  const RArray<TUid>& aApplications,
										  TRequestStatus &aStatus)
	{
	TInt r = AllocRequestBuffer(TokenDataMarshaller::Size(aApplications));
	if (r != KErrNone)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, r);
		return;
		}
	TokenDataMarshaller::Write(aApplications, iRequestPtr);

	TIpcArgs args(aCertInfo.Handle().iObjectId, 0, &iRequestPtr);
	TInt err = iClient.SendRequest(ESetApplicability, args);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSCertStoreClient::CancelSetApplicability()
	{
	// synchronous, nothing to do
	}

void CFSCertStoreClient::SetTrust(const CCTCertInfo& aCertInfo,
								  TBool aTrusted, 
								  TRequestStatus& aStatus)
	{
	TIpcArgs args(aCertInfo.Handle().iObjectId, 0, aTrusted);
	TInt err = iClient.SendRequest(ESetTrust, args);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CFSCertStoreClient::CancelSetTrust()
	{
	// synchronous, nothing to do
	}
