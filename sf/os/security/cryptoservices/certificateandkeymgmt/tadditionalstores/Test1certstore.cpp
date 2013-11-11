/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "test1certstore.h"
#include "tadditionalstoremappings.h"
#include <signed.h>
#include <x509cert.h>
#include <wtlscert.h>
#include <x509keys.h>
#include <ecom/implementationproxy.h>


_LIT(KName1, "Test store 1");
_LIT(KName2, "Test store 2");


//////////////////////////////////////////////////////////////////////////////////////////
//CFileCertStore
/////////////////////////////////////////////////////////////////////////////////////////

CTest1CertStore* CTest1CertStore::NewL(RFs& aFs,
									   CTest1CertStoreToken& aToken, 
									   const TDesC& aFileName,
									   TFileMode aMode)
	{
	CTest1CertStore* self = new(ELeave) CTest1CertStore(aToken, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName, aMode);
	CleanupStack::Pop(self);
	return self;
	}

MCTToken& CTest1CertStore::Token()
	{
	return iToken;
	}

void CTest1CertStore::DoRelease()
	{
	if (iToken.Label() == KName1)
		{
		iToken.iRefCountInterface--;
		if (!iToken.iRefCountInterface)
			{
			delete this;
			}
		}
	else if (iToken.Label() == KName2)
		{
		iToken.iRefCountInterface2--;
		if (!iToken.iRefCountInterface2)
			{
			delete this;
			}
		}
	}

void CTest1CertStore::List(RMPointerArray<CCTCertInfo>& aCertInfos,
						  const CCertAttributeFilter& aFilter,
						  TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;
	
	iCertInfos = &aCertInfos;
	iFilter = &aFilter;

	//Iinitialize the ketstore and then list the key info
	if (iFilter->iKeyUsage != EX509UsageAll)
		{
		// We have to initialize the unified key store
		TRAPD(err, iUnifiedKeyStore = CUnifiedKeyStore::NewL(iFs));
		if (err != KErrNone)
			{
			User::RequestComplete(iOriginalRequestStatus, err);
			iOriginalRequestStatus = 0;
			}
		else
			{
			iUnifiedKeyStore->Initialize(iStatus);
			iState = EGetKeyInfos;
			SetActive();
			}
		}
	else
		{
		iState = EList;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

void CTest1CertStore::CancelList()
	{
	Cancel();
	}

void CTest1CertStore::GetCert(CCTCertInfo*& aCertInfo,
							 const TCTTokenObjectHandle& aHandle,
							 TRequestStatus& aStatus)
	{
	TInt err = KErrNotFound;
	if ((aHandle.iTokenHandle == iToken.Handle()) &&
		(aHandle.iObjectId < iCerts->Count()))
		{
		TRAP(err, aCertInfo = CCTCertInfo::NewL(iCerts->Entry(aHandle.iObjectId)));
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CTest1CertStore::CancelGetCert()
	{
	// Nothing to do because GetCert is not asynchronous.
	}

void CTest1CertStore::Applications(const CCTCertInfo& aCertInfo,
								  RArray<TUid>& aApplications,
								  TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	TInt index = iCerts->Index(aCertInfo);
	if (index != KErrNotFound)
		{
		const RArray<TUid>& apps = iCerts->Mapping(index)->CertificateApps();
		TInt end = apps.Count();
		for (TInt i = 0; (i < end) && (err == KErrNone); i++)
			{
			err = aApplications.Append(apps[i]);
			}
		}
	else
		{
		err = index;
		}
	if (err != KErrNone)
		{
		aApplications.Reset();
		}
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CTest1CertStore::CancelApplications()
	{
	}

void CTest1CertStore::IsApplicable(const CCTCertInfo& aCertInfo,
								  TUid aApplication, 
								  TBool& aIsApplicable,
								  TRequestStatus& aStatus)
	{
	TInt index = iCerts->Index(aCertInfo);
	if (index != KErrNotFound)
		{
		const RArray<TUid>& apps = iCerts->Mapping(index)->CertificateApps();
		TInt end = apps.Count();
		TInt i;
		for (i = 0; i < end; i++)
			{
			if (apps[i] == aApplication)
				{
				break;
				}
			}
		if (i == end)
			{
			aIsApplicable = EFalse;
			}
		else
			{
			aIsApplicable = ETrue;
			}
		index = KErrNone;
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, index);
	}

void CTest1CertStore::CancelIsApplicable()
	{
	// Nothing to do because IsApplicable is not asynchronous.
	}

void CTest1CertStore::Trusted(const CCTCertInfo& aCertInfo,
							 TBool& aTrusted, 
							 TRequestStatus& aStatus)
	{	
	TInt index = iCerts->Index(aCertInfo);
	if (index != KErrNotFound)
		{
		aTrusted = iCerts->Mapping(index)->Trusted();
		index = KErrNone;
		}
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, index);
	}

void CTest1CertStore::CancelTrusted()
	{
	// Nothing to do because Trusted is not asynchronous.
	}

void CTest1CertStore::Retrieve(const CCTCertInfo& aCertInfo,
							  TDes8& aCertificate,
							  TRequestStatus& aStatus)
	{
#ifdef CERTSTORE_SOFTWARE_ASYNCH
	// perform an asynchronous retrieval of the certificate
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	iAsynchCertInfo = &aCertInfo;
	iAsynchCertificate = &aCertificate;

	iState = ERetrieve;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
#else
	RetrieveNow(aCertInfo,aCertificate,aStatus);
#endif
	}

void CTest1CertStore::RetrieveNow(const CCTCertInfo& aCertInfo,
							  TDes8& aCertificate,
							  TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt err;
	TInt index = iCerts->Index(aCertInfo);
	if (index == KErrNotFound)
		{
		err = KErrNotFound;
		}
	else
		{
		CFileCertStoreMapping* mapping = NULL;
		mapping = iCerts->Mapping(index);
		err = index;
		if (mapping)
			{
			TRAP(err, DoLoadL(aCertificate, *mapping));
			}
		}
	User::RequestComplete(status, err);
	}

void CTest1CertStore::CancelRetrieve()
	{
	}


void CTest1CertStore::Capabilities(const CCTCertInfo& /*aCertInfo*/, TCapabilitySet& /*aCapbilitiesOut*/,
								  TRequestStatus& aStatus)
	{
	// currently not supported
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CTest1CertStore::CancelCapabilities()
	{
	// Nothing to do because Capabilities is not asynchronous.
	}

void CTest1CertStore::IsMandatory(const CCTCertInfo& /*aCertInfo*/, TBool& /*aMandatoryOut*/,
								 TRequestStatus& aStatus)
	{
	// currently not supported
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CTest1CertStore::CancelIsMandatory()
	{
	// Nothing to do because IsMandatory is not asynchronous.
	}


void CTest1CertStore::Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus)
	{
	// This removes the certificate from the file store.
	TRAPD(err, DoRemoveL(aCertInfo));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CTest1CertStore::CancelRemove()
	{
	}


void CTest1CertStore::SetApplicability(const CCTCertInfo& aCertInfo,
									  const RArray<TUid>& aTrusters,
									  TRequestStatus& aStatus)
	{
	TRAPD(err, DoSetApplicabilityL(aCertInfo, aTrusters));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}


void CTest1CertStore::CancelSetApplicability()
	{
	}

void CTest1CertStore::SetTrust(const CCTCertInfo& aCertInfo, 
							  TBool aTrusted, 
							  TRequestStatus& aStatus)
	{
	TRAPD(err, DoSetTrustL(aCertInfo, aTrusted));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CTest1CertStore::CancelSetTrust()
	{
	// Nothing to do because SetTrust is not asynchronous.
	}

void CTest1CertStore::SetCapabilities(const CCTCertInfo& /*aCertInfo*/, const TCapabilitySet& /*aCapabilities*/, 
									 TRequestStatus& aStatus)
	{
	// currently not supported
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CTest1CertStore::CancelSetCapabilities()
	{
	// Nothing to do because SetCapabilities is not asynchronous.
	}

void CTest1CertStore::SetMandatory(const CCTCertInfo& /*aCertInfo*/, TBool /*aMandatory*/,
								  TRequestStatus& aStatus)
	{
	// currently not supported
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

void CTest1CertStore::CancelSetMandatory()
	{
	// Nothing to do because SetMandatory is not asynchronous.
	}


void CTest1CertStore::RevertStore(TAny* aStore)
	{
	//this is a CleanupItem
	CPermanentFileStore* store = REINTERPRET_CAST(CPermanentFileStore*, aStore);
	store->Revert();
	}


void CTest1CertStore::DeleteFile(TAny* aThis)
	{
	CTest1CertStore* self = REINTERPRET_CAST(CTest1CertStore*, aThis);
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<128> certStoreDefaultFullPath (driveName);
	certStoreDefaultFullPath.Append(_L("\\system\\data\\cacerts.dat"));
	self->iFile.Close();
	self->iFs.Delete(certStoreDefaultFullPath);
	}



CTest1CertStore::~CTest1CertStore()
	{
	Cancel();

	if (iStore != NULL)
		{
#ifdef _DEBUG
#else		
		TInt err;
		TRAP(err, iStore->ReclaimL());
		TRAP(err, iStore->CompactL());
		if (err == KErrNone)
			{
			TRAP(err, iStore->CommitL());
			}
#endif
		delete iStore;
		}

	iFile.Close();
	delete iCerts;

	iKeyInfos.Close();

	delete iUnifiedKeyStore;
	}


void CTest1CertStore::DoSetApplicabilityL(const CCTCertInfo& aCertInfo,
										 const RArray<TUid>& aTrusters)
	{
	TInt index = iCerts->Index(aCertInfo);
	if (index == KErrNotFound)
		{
		User::Leave(index);
		}
	
	CFileCertStoreMapping* mapping = iCerts->Mapping(index);
	const RArray<TUid>& trusters = mapping->CertificateApps(); //oldEntry->Trusters();
	RArray<TUid>* oldTrusters = new(ELeave) RArray<TUid>();
	CleanupStack::PushL(oldTrusters);
	CleanupClosePushL(*oldTrusters);
	TInt iEnd = trusters.Count();
	TInt i;
	for (i = 0; i < iEnd; i++)
		{
		User::LeaveIfError(oldTrusters->Append(trusters[i]));
		}

	RArray<TUid>* newTrusters = new (ELeave) RArray<TUid>;
	CleanupStack::PushL(newTrusters);
	CleanupClosePushL(*newTrusters);
	for (i = 0 ; i < aTrusters.Count() ; ++i)
		{
		User::LeaveIfError(newTrusters->Append(aTrusters[i]));		
		}

	mapping->SetCertificateApps(newTrusters);
	CleanupStack::Pop(2, newTrusters);

	TRAPD(err, UpdateStoreL());
	CleanupStack::Pop(2);	// *oldTrusters, oldTrusters
	if (err != KErrNone)
		{
		// If there is an error, we undo the change in oldEntry
		mapping->SetCertificateApps(oldTrusters);
		}
	else
		{
		oldTrusters->Close();
		delete oldTrusters;
		}
	}


void CTest1CertStore::DoSetTrustL(const CCTCertInfo& aCertInfo,
								 TBool aTrusted)
	{
	TInt index = iCerts->Index(aCertInfo);
	if (index == KErrNotFound)
		{
		User::Leave(index);
		}
	CFileCertStoreMapping* mapping = iCerts->Mapping(index);
	TBool oldValue = mapping->Trusted();
	mapping->SetTrusted(aTrusted);
	TRAPD(err, UpdateStoreL());
	if (err != KErrNone)
		{
		// If there is an error, we undo the change in oldEntry
		mapping->SetTrusted(oldValue);
		}
	}

void CTest1CertStore::Add(const TDesC& aLabel,
						 TCertificateFormat aFormat,
						 TCertificateOwnerType aCertificateOwnerType,
						 const TKeyIdentifier* aSubjectKeyId,
						 const TKeyIdentifier* aIssuerKeyId,
						 const TDesC8& aCert, 
						 TRequestStatus& aStatus)
	{
	TRAPD(err, DoAddL(aLabel, aFormat, aCertificateOwnerType, aSubjectKeyId,
		aIssuerKeyId, aCert));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CTest1CertStore::CancelAdd()
	{
	Cancel();
	}

void CTest1CertStore::DoAddL(const TDesC& aLabel,
							TCertificateFormat aFormat,
							TCertificateOwnerType aCertificateOwnerType,
							const TKeyIdentifier* aSubjectKeyId,
							const TKeyIdentifier* aIssuerKeyId,
							const TDesC8& aCert)
	{
	TKeyIdentifier subjectKeyId;
	switch (aFormat)
		{
		case EX509Certificate:
			if (!aSubjectKeyId)
				{
				CCertificate* cert = CX509Certificate::NewLC(aCert);
				subjectKeyId = cert->KeyIdentifierL();
				aSubjectKeyId = &subjectKeyId;
				CleanupStack::PopAndDestroy(cert);
				}
			break;
	
		case EWTLSCertificate:
			if (!aSubjectKeyId)
				{
				CCertificate* cert = CWTLSCertificate::NewLC(aCert);
				subjectKeyId = cert->KeyIdentifierL();
				aSubjectKeyId = &subjectKeyId;
				CleanupStack::PopAndDestroy(cert);
				}
			break;

		case EX509CertificateUrl:
			if (!aSubjectKeyId)
				{
				User::Leave(KErrArgument);
				}
			break;

		default:
			User::Leave(KErrNotSupported);	
			break;
		}


	TInt iend = iCerts->Count();
	for (TInt i = 0; i < iend; i++)
		{
		if (iCerts->Entry(i).Label() == aLabel)
			{
			User::Leave(KErrBadName);
			}
		}

	CCTCertInfo* entry = CCTCertInfo::NewL(aLabel, aFormat, 
		aCertificateOwnerType, aCert.Length(), aSubjectKeyId, aIssuerKeyId, iToken, iCerts->Count());
	// AddCertL takes ownership of entry no matter what happens.
	AddCertL(entry, aCert, *iCerts);
	}

/*void CFileCertStore::AddUserCertL(const CCertificate& aCert, 
								  const TDesC& aLabel,
								  TCertificateFormat aFormat, 
								  const TKeyIdentifier& aIssuerKeyHash, 
								  const TKeyIdentifier& aSubjectKeyHash)
	{
	if (aFormat != EX509Certificate)
		{
		User::Leave(KErrNotSupported);
		}

	TInt iend = iUserCerts->Count();
	for (TInt i = 0; i < iend; i++)
		{
		if (iUserCerts->Entry(i).Label() == aLabel)
			{
			User::Leave(KErrBadName);
			}
		}

	// We compute the subject key hash using the information contained in the certificate
	const CSubjectPublicKeyInfo& key = aCert.PublicKey();
	CX509RSAPublicKey* rsaKey = CX509RSAPublicKey::NewLC(key.KeyData());
#ifdef SYMBIAN_CRYPTO	
	const TInteger& modulus = rsaKey->Modulus();
#else
	const CInteger& modulus = rsaKey->Modulus();
#endif
	HBufC8* modulusBuffer = modulus.BufferLC();

	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	
	TPtrC8 hash = sha1->Hash(*modulusBuffer);


	TKeyIdentifier keyId;
	keyId = hash;

	CleanupStack::PopAndDestroy(3);	// rsaKey, modulusBuffer, sha1

	// If the caller supplied a aSubjectKeyHash, we must compare it with the computed
	// value and ensure they are the same
	if ((aSubjectKeyHash != KNullDesC8) && (aSubjectKeyHash != keyId))
		{
		User::Leave(KErrArgument);
		}
	
	CCTCertInfo* entry = CCTCertInfo::NewL(aLabel, 
		*this, EX509Certificate, aCert.Encoding().Length(), keyId, aIssuerKeyHash);
	AddCertL(entry, aCert, *iUserCerts);
	}*/

/*void CFileCertStore::AddUserCertURLL(const TDesC8& aCert, 
									 const TDesC& aLabel,
									 const TKeyIdentifier& aIssuerKeyHash, 
									 const TKeyIdentifier& aSubjectKeyHash)
	{
	TInt iEnd = iUserCerts->Count();
	for (TInt i = 0; i < iEnd; i++)
		{
		if (iUserCerts->Entry(i).Label() == aLabel)
			{
			User::Leave(KErrBadName);
			}
		}

	CCTCertInfo* entry = CCTCertInfo::NewL(aLabel, 
		*this, EX509CertificateUrl, aCert.Length(), aSubjectKeyHash, aIssuerKeyHash);
	AddCertL(entry, aCert, *iUserCerts);
	}*/

void CTest1CertStore::AddCertL(CCTCertInfo* aCertInfo,
							  const TDesC8& aCert,
							  CFileCertStoreMappings& aMappings)
	{
	CleanupReleasePushL(*aCertInfo);
//create the mapping object		
	CFileCertStoreMapping* mapping = CFileCertStoreMapping::NewL();
	mapping->SetEntry(aCertInfo);
	CleanupStack::Pop();//aEntry, mapping has taken ownership
	CleanupStack::PushL(mapping);

	TCleanupItem cleanupStore(CTest1CertStore::RevertStore, iStore);//store will revert() if a leave occurs
	CleanupStack::PushL(cleanupStore);

//store cert
	RStoreWriteStream stream;
	TStreamId certId = stream.CreateLC(*iStore);//stream for cert
	stream.WriteL(aCert);
	stream.CommitL();
	CleanupStack::PopAndDestroy();//stream
	mapping->SetId(certId);

//add mapping to mappings, & store mappings
	aMappings.AddL(mapping); //takes ownership
	CleanupStack::Pop();//mapping; mappings has taken ownership

//Update the mapping. if error, remove the entry.
	TRAPD(err, aMappings.ReplaceL());
	if (err == KErrNone)
		{
		TRAP(err, iStore->CommitL());
		if (err != KErrNone)
			{
			aMappings.Remove(*mapping->Entry());
			User::Leave(err);
			}
		}
	else
		{
		//oom tests pass currently
		aMappings.Remove(*mapping->Entry());
		User::Leave(err);
		}
	CleanupStack::Pop();//revert store
	}

void CTest1CertStore::UpdateStoreL()
	{
//tries to write out the new cacerts to the file
	TCleanupItem cleanupStore(RevertStore, iStore);//store will revert() if a leave occurs
	CleanupStack::PushL(cleanupStore);
	iCerts->ReplaceL();
	iStore->CommitL();
	CleanupStack::Pop();//revert store
	}





void CTest1CertStore::DoLoadL(TDes8& aCertificate, CFileCertStoreMapping& aMapping) const
	{
	RStoreReadStream stream;
	stream.OpenLC(*iStore, aMapping.Id());
	CCTCertInfo* entry = aMapping.Entry();
	stream.ReadL(aCertificate, entry->Size());
	CleanupStack::PopAndDestroy();//stream
	}

void CTest1CertStore::DoRemoveL(const CCTCertInfo& aCertInfo)
	{
	switch(aCertInfo.CertificateFormat())
		{
		case EWTLSCertificate://must be a CA cert
		case EX509CertificateUrl:
		case EX509Certificate:
			User::LeaveIfError(iCerts->SetTempRemove(aCertInfo, ETrue));
			break;
			
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	TRAPD(err, UpdateStoreL());
	if (err != KErrNone)
		{
		User::LeaveIfError(iCerts->SetTempRemove(aCertInfo, EFalse));
		}
	else
		{
		User::LeaveIfError(iCerts->Remove(aCertInfo));	
		}
	}

//private functions
CTest1CertStore::CTest1CertStore(CTest1CertStoreToken& aToken, RFs& aFs)
	: CActive(EPriorityNormal), iToken(aToken), iFs(aFs)
	{
	CActiveScheduler::Add(this);
	}

void CTest1CertStore::ConstructL(const TDesC& aFileName, TFileMode aMode)
	{
	iStore = OpenStoreL(aFileName, aMode);
	RestoreL();
	}

void CTest1CertStore::RestoreL()
	{
	TStreamId caCertEntryStreamId;
	RStoreReadStream stream;
	stream.OpenLC(*iStore, iStore->Root());

	stream >> caCertEntryStreamId;
	CleanupStack::PopAndDestroy();//stream

	iCerts = CFileCertStoreMappings::NewL(caCertEntryStreamId, *iStore);
	RStoreReadStream caCertEntryStream;
	caCertEntryStream.OpenLC(*iStore, caCertEntryStreamId);

	TInt caCount = caCertEntryStream.ReadInt32L();
	for (TInt i = 0; i < caCount; i++)
		{
		CFileCertStoreMapping* caCertMapping = CFileCertStoreMapping::NewL();
		CleanupStack::PushL(caCertMapping);
		CCTCertInfo* caCertEntry = CCTCertInfo::NewLC(caCertEntryStream, iToken);
		caCertMapping->SetEntry(caCertEntry);
		CleanupStack::Pop();//caCertEntry
		// Read the CertificateApps uids
		RArray<TUid>* certificateApps = new(ELeave) RArray<TUid>();
		CleanupStack::PushL(certificateApps);
		CleanupClosePushL(*certificateApps);
		TInt count = caCertEntryStream.ReadInt32L();
		for (TInt j = 0; j < count; j++)
			{
			TUid id;
			caCertEntryStream >> id;
			User::LeaveIfError(certificateApps->Append(id));
			}
		CleanupStack::Pop(2);	// *certificateApps, certificateApps
		caCertMapping->SetCertificateApps(certificateApps);
		TBool trusted = caCertEntryStream.ReadUint8L();
		caCertMapping->SetTrusted(trusted);
		TStreamId caCertStreamId;
		caCertEntryStream >> caCertStreamId;
		caCertMapping->SetId(caCertStreamId);
		iCerts->AddL(caCertMapping);
		CleanupStack::Pop();//caCertMapping
		}
	CleanupStack::PopAndDestroy();//caCertStream
	}





CPermanentFileStore* CTest1CertStore::OpenStoreLC(const TDesC& aFileName, TFileMode aMode)
	{
//this function creates, opens and returns a permanent file store in KCertStorePath,
//on the drive letter passed in, leaving it on the cleanup stack. 
//if the store isn't found it returns NULL
	TInt err = iFile.Open(iFs, aFileName, aMode); 	
    if (err == KErrNone)
		{
		CPermanentFileStore* store = CPermanentFileStore::FromLC(iFile);
		return store;
		}
	else
		{
		User::Leave(err);
		}
	return NULL;
	}

CPermanentFileStore* CTest1CertStore::OpenStoreL(const TDesC& aFileName, TFileMode aMode)
	{
	CPermanentFileStore* store = NULL;
	store = OpenStoreLC(aFileName, aMode);	
	CleanupStack::Pop(store);
	return store;
	}

void CTest1CertStore::RunL()
	{
	switch (iState)
		{
	case EGetKeyInfos:
		iKeyFilter.iUsage = KeyUsageX509ToPKCS15Private(iFilter->iKeyUsage);
		iUnifiedKeyStore->List(iKeyInfos, iKeyFilter, iStatus);
		iState = EList;
		SetActive();
		break;

	case ERetrieve:
		// Asynch Retrieve 
		RetrieveNow(*iAsynchCertInfo,*iAsynchCertificate,*iOriginalRequestStatus);
		break;

	case EList:
		{	
		TInt count = iCerts->Count();
		for (TInt index = 0; index < count; index++)
			{
			const CCTCertInfo& certInfo = iCerts->Entry(index);
			
			TBool accept = ETrue;
			if (iFilter->iUidIsSet)
				{
				accept = iCerts->Mapping(index)->IsApplicable(iFilter->iUid);
				}
			if (iFilter->iFormatIsSet && accept)
				{
				accept = (iFilter->iFormat == certInfo.CertificateFormat());
				}
			if (iFilter->iOwnerTypeIsSet && accept)
				{
				accept = (iFilter->iOwnerType == certInfo.CertificateOwnerType());
				}
			if ((iFilter->iSubjectKeyId != KNullDesC8) && accept)
				{
				accept = (iFilter->iSubjectKeyId == certInfo.SubjectKeyId());
				}
			
			if (accept)
				{
				// Fill in the cert hash.
				// (This returns an incorrect hard-coded value, which allows 
				// the test code to check that the store is being treated
				// correctly as a hardware store, as it'll behave differently
				// to if it returned the correct hashes.)
				_LIT8(KHash, "\x70\xe4\xf4\x54\x5f\x8e\xe6\xf2\xbd\x4e\x76\x2b\x8d\xa1\x83\xd8\xe0\x5d\x4a\x7d");
				CCTCertInfo* copy = CCTCertInfo::NewLC(
					certInfo.Label(), certInfo.CertificateFormat(),
					certInfo.CertificateOwnerType(), certInfo.Size(),
					&certInfo.SubjectKeyId(), &certInfo.IssuerKeyId(),
					certInfo.Token(), certInfo.Handle().iObjectId,
					ETrue, &KHash);
				User::LeaveIfError(iCertInfos->Append(copy));
				CleanupStack::Pop();
				}
			}
		iKeyInfos.Close();
		delete iUnifiedKeyStore;
		iUnifiedKeyStore = 0;
		User::RequestComplete(iOriginalRequestStatus, KErrNone);
		}
		break;

	default:
		// ignore the undefined operations
		break;
		}
	}

void CTest1CertStore::DoCancel()
	{
	if (iUnifiedKeyStore)
		{
		if (iState == EGetKeyInfos)
			{
			iUnifiedKeyStore->CancelInitialize();
			}
		else if (iState == EList)
			{
			iUnifiedKeyStore->CancelList();
			}
		iState = EList;
		iKeyInfos.Close();
		delete iUnifiedKeyStore;
		iUnifiedKeyStore = 0;
		}
	User::RequestComplete(iOriginalRequestStatus, KErrCancel);
	}

const TImplementationProxy ImplementationTable[] =
	{
#ifdef CERTSTORE_SOFTWARE_ASYNCH
		IMPLEMENTATION_PROXY_ENTRY(0x101FF738, CTest1CertStoreTokenType::NewL)
#else
	#ifdef CERTSTORE_HARDWARE_SIM
		IMPLEMENTATION_PROXY_ENTRY(0x10206846, CTest1CertStoreTokenType::NewL)
	#else
		#ifdef CERTSTORE_HARDWARE_WIM
			IMPLEMENTATION_PROXY_ENTRY(0x10206847, CTest1CertStoreTokenType::NewL)
		#else
			#ifdef CERTSTORE_HARDWARE_UICC
				IMPLEMENTATION_PROXY_ENTRY(0x10206848, CTest1CertStoreTokenType::NewL)
 			#else
				#ifdef CERTSTORE_DEVICE_IMMUTABLE
					IMPLEMENTATION_PROXY_ENTRY(0x102077C3, CTest1CertStoreTokenType::NewL)
				#else
					IMPLEMENTATION_PROXY_ENTRY(0x101F5279, CTest1CertStoreTokenType::NewL)
				#endif
 			#endif
		#endif
	#endif
#endif
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}





