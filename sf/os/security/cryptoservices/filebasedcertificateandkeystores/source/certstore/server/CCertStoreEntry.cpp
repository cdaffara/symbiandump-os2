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


#include "CCertStoreEntry.h"
#include "fsdatatypes.h"
#include "fstokenutil.h"

////////////////////////////////////////////////////////////////////////////////
//CCertStoreEntry
/////////////////////////////////////////////////////////////////////////////////

CCertStoreEntry* CCertStoreEntry::NewL(const CCertInfo& aCertInfo,
									   RArray<TUid> aCertificateApps,
									   TBool aTrusted,
									   TStreamId aDataStreamId)
	{
	CCertStoreEntry* self = new(ELeave) CCertStoreEntry();
	CleanupStack::PushL(self);
	self->ConstructL(aCertInfo, aCertificateApps, aTrusted, aDataStreamId);
	CleanupStack::Pop(self);
	return self;
	}

CCertStoreEntry* CCertStoreEntry::NewLC(RReadStream& aStream)
	{
	CCertStoreEntry* self = new(ELeave) CCertStoreEntry();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}

CCertStoreEntry::CCertStoreEntry()
	{	
	}

void CCertStoreEntry::ConstructL(const CCertInfo& aCertInfo,
								 RArray<TUid> aCertificateApps,
								 TBool aTrusted,
								 TStreamId aDataStreamId)
	{
	assert(aDataStreamId != KNullStreamId, EPanicCertStoreEntryConstructArguments);

	iCertInfo = CCertInfo::NewL(aCertInfo);
	
	for (TInt i = 0 ; i < aCertificateApps.Count() ; ++i)
		{
		User::LeaveIfError(iCertificateApps.Append(aCertificateApps[i]));
		}

	iTrusted = aTrusted;	
	iDataStreamId = aDataStreamId;
	}

CCertStoreEntry::~CCertStoreEntry()
	{
	delete iCertInfo;
	iCertificateApps.Close();
	}

void CCertStoreEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iCertInfo;
	TInt count = iCertificateApps.Count();
	aStream.WriteInt32L(count);
	for (TInt i = 0 ; i < count ; ++i)
		{
		aStream << iCertificateApps[i];
		}
	aStream.WriteUint8L(iTrusted);
	aStream << iDataStreamId;
	}

void CCertStoreEntry::InternalizeL(RReadStream& aStream)
	{
	assert(!iCertInfo, EPanicCertStoreEntryInternalizeState);
	iCertInfo = CCertInfo::NewL(aStream);
	
	TInt count = aStream.ReadInt32L();
	for (TInt i = 0 ; i < count ; ++i)
		{
		TUid id;
		aStream >> id;
		User::LeaveIfError(iCertificateApps.Append(id));
		}
	
	iTrusted = !!aStream.ReadUint8L();
	aStream >> iDataStreamId;
	}

const CCertInfo& CCertStoreEntry::CertInfo() const
	{
	return *iCertInfo;
	}

const RArray<TUid>& CCertStoreEntry::CertificateApps() const
	{
	return iCertificateApps;
	}

TBool CCertStoreEntry::IsApplicable(const TUid& aApplication) const
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

TBool CCertStoreEntry::Trusted() const
	{
	return iTrusted;
	}

TStreamId CCertStoreEntry::DataStreamId() const
	{
	return iDataStreamId;
	}
