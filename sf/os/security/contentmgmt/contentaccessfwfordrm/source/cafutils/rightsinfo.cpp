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


#include <s32strm.h>

#include <caf/rightsinfo.h>
#include <caf/agent.h>
#include "cafutils.h"

using namespace ContentAccess;

EXPORT_C CRightsInfo* CRightsInfo::NewL(RReadStream& aStream)
	{
	CRightsInfo* self = new (ELeave) CRightsInfo;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRightsInfo* CRightsInfo::NewL(const TDesC& aDescription, const TDesC& aUniqueId, TInt aRightsType, TRightsStatus aRightsStatus)
	{
	CRightsInfo* self = new (ELeave) CRightsInfo(aRightsType, aRightsStatus);
	CleanupStack::PushL(self);
	self->ConstructL(aDescription, aUniqueId);
	CleanupStack::Pop(self);
	return self;
	}

CRightsInfo::CRightsInfo()
	{
	}

CRightsInfo::CRightsInfo(TInt aRightsType, TRightsStatus aRightsStatus) :
		iRightsType(aRightsType), iRightsStatus(aRightsStatus)
	{
	}
	

CRightsInfo::~CRightsInfo()
	{
	delete iDescription;
	delete iUniqueId;
	}	

void CRightsInfo::ConstructL(const TDesC& aDescription, const TDesC& aUniqueId)
	{
	iDescription = aDescription.AllocL();
	iUniqueId = aUniqueId.AllocL();
	}
	
EXPORT_C const TDesC& CRightsInfo::Description() const
	{
	return *iDescription;
	}

EXPORT_C const TDesC& CRightsInfo::UniqueId() const
	{
	return *iUniqueId;
	}

EXPORT_C TInt CRightsInfo::RightsType() const
	{
	return iRightsType;
	}

EXPORT_C TRightsStatus CRightsInfo::RightsStatus() const
	{
	return iRightsStatus;
	}

EXPORT_C void CRightsInfo::ExternalizeL(RWriteStream &aStream) const
	{
	TCafUtils::WriteDescriptor16L(aStream, *iDescription);
	TCafUtils::WriteDescriptor16L(aStream, *iUniqueId);
	aStream.WriteInt32L(iRightsType);
	aStream.WriteInt32L((TInt) iRightsStatus);
	}

void CRightsInfo::InternalizeL(RReadStream& aStream)
	{
	iDescription = TCafUtils::ReadDescriptor16L(aStream);
	iUniqueId = TCafUtils::ReadDescriptor16L(aStream);
	iRightsType = aStream.ReadInt32L();
	iRightsStatus = (TRightsStatus) aStream.ReadInt32L();
	}
