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


/**
 @file
*/

#include "tadditionalstoremapping.h"

////////////////////////////////////////////////////////////////////////////////
//CFileCertStoreMapping
/////////////////////////////////////////////////////////////////////////////////

CFileCertStoreMapping* CFileCertStoreMapping::NewL()
	{
	CFileCertStoreMapping* self = CFileCertStoreMapping::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CFileCertStoreMapping* CFileCertStoreMapping::NewLC()
	{
	CFileCertStoreMapping* self = new(ELeave) CFileCertStoreMapping();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CFileCertStoreMapping::ConstructL()
	{
	iCertificateApps = new(ELeave) RArray<TUid>();
	}

CFileCertStoreMapping::CFileCertStoreMapping()
	{	
	}

CFileCertStoreMapping::~CFileCertStoreMapping()
	{
	if (iEntry)
		{
		iEntry->Release();
		}
	if (iCertificateApps)
		{
		iCertificateApps->Close();
		delete iCertificateApps;
		}
	}

void CFileCertStoreMapping::SetEntry(CCTCertInfo* aCertInfo)
	{
	if (iEntry)
		{
		iEntry->Release();
		}
	iEntry = aCertInfo;
	}

void CFileCertStoreMapping::SetCertificateApps(RArray<TUid>* aCertificateApps)
	{
	iCertificateApps->Close();
	delete iCertificateApps;
	iCertificateApps = aCertificateApps;
	}

void CFileCertStoreMapping::SetId(TStreamId aId)
	{
	iId = aId;
	}

CCTCertInfo* CFileCertStoreMapping::Entry() const
	{
	return iEntry;
	}

const RArray<TUid>& CFileCertStoreMapping::CertificateApps() const
	{
	return *iCertificateApps;
	}

TBool CFileCertStoreMapping::IsApplicable(const TUid& aApplication) const
	{
	TInt count = iCertificateApps->Count();
	for (TInt i = 0; i < count; i++)
		{
		TUid app = (*iCertificateApps)[i];
		if (app == aApplication)
				{
				return ETrue;
				}
		}
	return EFalse;
	}

TBool CFileCertStoreMapping::Trusted() const
	{
	return iTrusted;
	}

void CFileCertStoreMapping::SetTrusted(TBool aTrusted)
	{
	iTrusted = aTrusted;
	}

TStreamId CFileCertStoreMapping::Id() const
	{
	return iId;
	}

void CFileCertStoreMapping::ExternalizeL(RWriteStream& aStream) const
	{
	if (!iTempRemoved)
		{
		aStream << *iEntry;
		TInt count = iCertificateApps->Count();
		aStream.WriteInt32L(count);
		for (TInt i = 0; i < count; i++)
			{
			aStream << (*iCertificateApps)[i];
			}
		aStream.WriteUint8L(iTrusted);
		aStream << iId;			
		}
	}

void CFileCertStoreMapping::SetTempRemoved(TBool aFlag)
	{
	iTempRemoved=aFlag;
	}

TBool CFileCertStoreMapping::IsTempRemoved()
	{
	return iTempRemoved;
	}

#include "tadditionalstoremappings.h"

/////////////////////////////////////////////////////////////////////////////////
//CFileCertStoreMappings
/////////////////////////////////////////////////////////////////////////////////
CFileCertStoreMappings::~CFileCertStoreMappings()
	{
	if (iMappings)
		{
		iMappings->ResetAndDestroy();
		delete iMappings;
		}
	}

TInt CFileCertStoreMappings::Count()
	{
	return iMappings->Count();
	}

void CFileCertStoreMappings::AddL(CFileCertStoreMapping* aEntry)
	{
	User::LeaveIfError(iMappings->Append(aEntry));
	}

TInt CFileCertStoreMappings::Remove(const CCTCertInfo& aCertInfo)
	{
	TInt index = Index(aCertInfo);
	if (index == KErrNotFound)
		{
		return KErrNotFound;
		}
	CFileCertStoreMapping* mapping = (*iMappings)[index];
	iMappings->Remove(index);
	delete mapping;
	return KErrNone;
	}

TInt CFileCertStoreMappings::SetTempRemove(const CCTCertInfo& aCertInfo, TBool aFlag)
	{
	TInt index = Index(aCertInfo);
	if (index == KErrNotFound)
		{
		return KErrNotFound;
		}
	(*iMappings)[index]->SetTempRemoved(aFlag);
	return KErrNone;		
	}


void CFileCertStoreMappings::ExternalizeL(RWriteStream& aStream) const
	{
	TInt count = iMappings->Count();
	TInt realCount=count;
	TInt i=0;
	for (i = 0; i < count; i++)
		{
		if ((*iMappings)[i]->IsTempRemoved())
			{
			realCount--;
			}
		}
	aStream.WriteInt32L(realCount);
	for (i = 0; i < count; i++)
		{
		aStream << *(*iMappings)[i];
		}
	}

void CFileCertStoreMappings::ReplaceL()
	{
	RStoreWriteStream stream;
	stream.ReplaceLC(iStore, iStreamId);
	ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	}

TInt CFileCertStoreMappings::Index(const CCTCertInfo& aCertInfo)
	{
	TInt count = iMappings->Count();
	TInt ix = KErrNotFound;
	for (TInt i = 0; i < count; i++)
		{
		CFileCertStoreMapping* mapping = (*iMappings)[i];
		if (aCertInfo==*(mapping->Entry()))
			{
			ix = i;
			break;
			}
		}
	return ix;
	}

CFileCertStoreMapping* CFileCertStoreMappings::Mapping(TInt aIndex)
	{
	return (*iMappings)[aIndex];
	}

TStreamId CFileCertStoreMappings::StreamId() const
	{
	return iStreamId;
	}

CFileCertStoreMappings::CFileCertStoreMappings(TStreamId aStreamId, 
													   CPermanentFileStore& aStore)
	:iStreamId(aStreamId), iStore(aStore)
	{
	}

/////////////////////////////////////////////////////////////////////////////////
//CFileCertStoreMappings
/////////////////////////////////////////////////////////////////////////////////
CFileCertStoreMappings* CFileCertStoreMappings::NewL(TStreamId aStreamId, 
													 CPermanentFileStore& aStore)
	{
	CFileCertStoreMappings* self = new(ELeave) CFileCertStoreMappings(aStreamId, aStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

const CCTCertInfo& CFileCertStoreMappings::Entry(TInt aIndex)
	{
	CFileCertStoreMapping* mapping = (*iMappings)[aIndex];
	return *mapping->Entry();
	}

const CCTCertInfo& CFileCertStoreMappings::EntryByHandleL(TInt aHandle) const
	{
	TInt count = iMappings->Count();
	for (TInt i = 0; i < count; i++)
		{
		CFileCertStoreMapping* mapping = (*iMappings)[i];
		if (aHandle==mapping->Entry()->Handle().iObjectId)
			{
			return *mapping->Entry();
			}
		}
	User::Leave(KErrNotFound);
	CCTCertInfo* info = NULL; // This is to shut up a compiler warning
	return *info;
	}

TInt CFileCertStoreMappings::NextHandle() const 
	{
	TInt count = iMappings->Count();
	TInt maxHandle = -1;
	for (TInt i = 0; i < count; i++)
		{
		CFileCertStoreMapping* mapping = (*iMappings)[i];
		if (mapping->Entry()->Handle().iObjectId > maxHandle)
			{
			maxHandle = mapping->Entry()->Handle().iObjectId;
			}
		}
	return ++maxHandle;
	}

void CFileCertStoreMappings::ConstructL()
	{
	iMappings = new(ELeave) RPointerArray<CFileCertStoreMapping>;
	}
