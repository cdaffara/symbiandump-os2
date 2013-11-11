/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "fsdatatypes.h"

EXPORT_C CCertInfo* CCertInfo::NewLC(const CCertInfo& aOther)
	{
	CCertInfo* self = NewL(aOther);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CCertInfo* CCertInfo::NewL(const CCertInfo& aOther)
	{
	return new (ELeave) CCertInfo(aOther);
	}
CCertInfo::CCertInfo(const CCertInfo& aOther) :
	MCertInfo(aOther)
	{
	}

EXPORT_C CCertInfo* CCertInfo::NewLC(const TDesC& aLabel,
						   TCertificateFormat aFormat,
						   TCertificateOwnerType aCertificateOwnerType,
						   TInt aSize,
						   const TKeyIdentifier* aSubjectKeyId,		
						   const TKeyIdentifier* aIssuerKeyId,
						   TInt aCertificateId)
	{
	CCertInfo* self = new(ELeave) CCertInfo(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId,
		aCertificateId);
	CleanupStack::PushL(self);
	self->ConstructL(NULL);
	return self;
	}

EXPORT_C CCertInfo* CCertInfo::NewLC(const TDesC& aLabel,
						   TCertificateFormat aFormat,
						   TCertificateOwnerType aCertificateOwnerType,
						   TInt aSize,
						   const TKeyIdentifier* aSubjectKeyId,		
						   const TKeyIdentifier* aIssuerKeyId,
						   TInt aCertificateId,
						   const TBool aDeletable)
	{
	CCertInfo* self = new(ELeave) CCertInfo(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId,
		aCertificateId, aDeletable);
	CleanupStack::PushL(self);
	self->ConstructL(NULL);
	return self;
	}

EXPORT_C CCertInfo* CCertInfo::NewL(const TDesC& aLabel,
							TCertificateFormat aFormat,
							TCertificateOwnerType aCertificateOwnerType,
							TInt aSize,
							const TKeyIdentifier* aSubjectKeyId,
							const TKeyIdentifier* aIssuerKeyId,
							TInt aCertificateId)
	{
	CCertInfo* self = CCertInfo::NewLC(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId,
		aCertificateId);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCertInfo* CCertInfo::NewL(const TDesC& aLabel,
							TCertificateFormat aFormat,
							TCertificateOwnerType aCertificateOwnerType,
							TInt aSize,
							const TKeyIdentifier* aSubjectKeyId,
							const TKeyIdentifier* aIssuerKeyId,
							TInt aCertificateId,
						    const TBool aDeletable)
	{
	CCertInfo* self = CCertInfo::NewLC(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId,
		aCertificateId, aDeletable);
	CleanupStack::Pop(self);
	return self;
	}

CCertInfo::CCertInfo(const TDesC& aLabel, 
					 TCertificateFormat aFormat,
					 TCertificateOwnerType aCertificateOwnerType,
					 TInt aSize,
					 const TKeyIdentifier* aSubjectKeyId,
					 const TKeyIdentifier* aIssuerKeyId,
					 TInt aCertificateId,
					 const TBool aDeletable) :
	MCertInfo(aLabel, aFormat, aCertificateOwnerType, aSize, aSubjectKeyId,
			  aIssuerKeyId, aCertificateId, aDeletable)
	{
	}

EXPORT_C CCertInfo* CCertInfo::NewL(RReadStream& aStream)
	{
	CCertInfo* me = NewLC(aStream);
	CleanupStack::Pop(me);
	return me;
	}

EXPORT_C CCertInfo* CCertInfo::NewLC(RReadStream& aStream)
	{
	CCertInfo* self = new (ELeave) CCertInfo();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}

CCertInfo::CCertInfo() :
	MCertInfo()
	{
	}
