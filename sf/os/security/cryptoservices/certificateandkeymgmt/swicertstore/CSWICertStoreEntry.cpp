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


#include "CSWICertStoreEntry.h"
#include <cctcertinfo.h>

CSWICertStoreEntry* CSWICertStoreEntry::NewLC(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex, TInt aStoreIndex)
	{
	CSWICertStoreEntry* self = new(ELeave) CSWICertStoreEntry(aStoreIndex);
	CleanupStack::PushL(self);
	
	self->InternalizeL(aStream, aToken, aCertIndex);
	return self;
	}

CSWICertStoreEntry* CSWICertStoreEntry::NewL(const CCTCertInfo& aCertInfo,const RArray<TUid>& aCertificateApps,TBool aTrusted,TStreamId aDataStreamId,const TCertMetaInfo& aCertMetaInfo, TInt aStoreIndex)
	{
	CSWICertStoreEntry* self = new(ELeave) CSWICertStoreEntry(aStoreIndex);
	CleanupStack::PushL(self);
	self->ConstructL(aCertInfo, aCertificateApps, aTrusted, aDataStreamId, aCertMetaInfo);
	CleanupStack::Pop(self);
	return self;
	}	
	
void CSWICertStoreEntry::ConstructL(const CCTCertInfo& aCertInfo,const RArray<TUid>& aCertificateApps,TBool aTrusted,TStreamId aDataStreamId, const TCertMetaInfo& aCertMetaInfo)
	{
	iCertInfo = CCTCertInfo::NewL(aCertInfo);
	
	for (TInt i = 0 ; i < aCertificateApps.Count() ; ++i)
		{
		iCertificateApps.AppendL(aCertificateApps[i]);
		}

	iTrusted = aTrusted;	
	iDataStreamId = aDataStreamId;
	iCertMetaInfo = aCertMetaInfo; 
	}	

TInt CSWICertStoreEntry::StoreIndex() const
	{
	return iStoreIndex;
	}

CSWICertStoreEntry::CSWICertStoreEntry(TInt aStoreIndex):iStoreIndex(aStoreIndex)
	{	
	}

CSWICertStoreEntry::~CSWICertStoreEntry()
	{
	if (iCertInfo)
		{
		iCertInfo->Release();
		}
	iCertificateApps.Close();
	}

void CSWICertStoreEntry::InternalizeL(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex)
	{
	ASSERT(!iCertInfo);
	iCertInfo = CCTCertInfo::NewL(aStream, aToken);
	iCertInfo->SetCertificateId(aCertIndex);

	TInt count = aStream.ReadInt32L();
	for (TInt i = 0 ; i < count ; ++i)
		{
		TUid id;
		aStream >> id;
		User::LeaveIfError(iCertificateApps.Append(id));
		}
	
	iTrusted = !!aStream.ReadUint8L();  // !! converts TUint8 to TBool
	aStream >> iDataStreamId;
	TPckg<TCapabilitySet> capsPckg(iCertMetaInfo.iCapabilities);
	aStream >> capsPckg;
	TUint8 value = aStream.ReadUint8L();
	iCertMetaInfo.iIsMandatory = !!(value & (1 << KMandatory)); // !! converts TUint8 to TBool
	iCertMetaInfo.iIsSystemUpgrade = !!(value & (1 << KSystemUpgrade));
	}

const CCTCertInfo& CSWICertStoreEntry::CertInfo() const
	{
	return *iCertInfo;
	}

const RArray<TUid>& CSWICertStoreEntry::CertificateApps() const
	{
	return iCertificateApps;
	}

TBool CSWICertStoreEntry::IsApplicable(const TUid& aApplication) const
	{
	for (TInt i = 0 ; i < iCertificateApps.Count() ; ++i)
		{
		if (iCertificateApps[i] == aApplication)
				{
				return ETrue;
				}
		}
	return EFalse;
	}

TBool CSWICertStoreEntry::Trusted() const
	{
	return iTrusted;
	}

TStreamId CSWICertStoreEntry::DataStreamId() const
	{
	return iDataStreamId;
	}

const TCertMetaInfo& CSWICertStoreEntry::CertMetaInfo() const
	{
	return iCertMetaInfo;	
	}


