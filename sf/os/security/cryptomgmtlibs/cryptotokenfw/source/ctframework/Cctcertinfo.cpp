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


#include "cctcertinfo.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS

/** Mask constants used for serializing iDeletable and iFormat attributes 
*/
const TUint KReadOnlyFlagMask = 128;
const TUint KFormatMask = 127;

/** The UID of a CertInfo MCTTokenObject. */
const TInt KCTObjectCertInfo = 0x101F50E6;

#endif

// MCertInfo ///////////////////////////////////////////////////////////////////

EXPORT_C MCertInfo::MCertInfo() :
	iDeletable(ETrue)
	{
	}

EXPORT_C MCertInfo::MCertInfo(const TDesC& aLabel,
							  TCertificateFormat aFormat,
							  TCertificateOwnerType aCertificateOwnerType, 
							  TInt aSize,
							  const TKeyIdentifier* aSubjectKeyId,
							  const TKeyIdentifier* aIssuerKeyId,
							  TInt aCertificateId,
							  TBool aDeletable) :
	iLabel(aLabel), iCertificateId(aCertificateId),
	iFormat(aFormat), iCertificateOwnerType(aCertificateOwnerType),
	iSize(aSize), iDeletable(aDeletable)
	{
	if (aSubjectKeyId)
		{
		iSubjectKeyId = *aSubjectKeyId;
		}
	else
		{
		iSubjectKeyId = KNullDesC8;
		}
	if (aIssuerKeyId)
		{
		iIssuerKeyId = *aIssuerKeyId;
		}
	else
		{
		iIssuerKeyId = KNullDesC8;
		}
	}

EXPORT_C MCertInfo::MCertInfo(const MCertInfo& aOther) :
	iLabel(aOther.iLabel),
	iCertificateId(aOther.iCertificateId),
	iFormat(aOther.iFormat),
	iCertificateOwnerType(aOther.iCertificateOwnerType),
	iSize(aOther.iSize),
	iSubjectKeyId(aOther.iSubjectKeyId),
	iIssuerKeyId(aOther.iIssuerKeyId),
	iDeletable(aOther.iDeletable)
	{
	}

EXPORT_C MCertInfo::~MCertInfo()
	{
	delete iIssuerHash;
	}

const TDesC8* MCertInfo::IssuerHash() const
	{
	return iIssuerHash;
	}

EXPORT_C void MCertInfo::ConstructL(const TDesC8* aIssuerHash)
	{
	delete iIssuerHash;
	iIssuerHash = NULL;
	if (aIssuerHash)
		{
		iIssuerHash = aIssuerHash->AllocL();
		}

	if (!Valid())
		{
		User::Leave(KErrArgument);
		}
	}
	
TBool MCertInfo::Valid() const
	{
	if (iLabel.Length() == 0)
		{
		return EFalse;
		}

	if (iCertificateId < 0)
		{
		return EFalse;
		}

	if (iFormat != EX509Certificate && iFormat != EWTLSCertificate &&
		iFormat != EX968Certificate && iFormat != EX509CertificateUrl &&
		iFormat != EWTLSCertificateUrl && iFormat != EX968CertificateUrl)
		{
		return EFalse;
		}
	
	if (iCertificateOwnerType != ECACertificate &&
		iCertificateOwnerType != EUserCertificate &&
		iCertificateOwnerType != EPeerCertificate)
		{
		return EFalse;
		}

	if (iSize <= 0)
		{
		return EFalse;
		}

	if (iIssuerHash && *iIssuerHash == KNullDesC8)
		{
		return EFalse;
		}

	return ETrue;
	}

/**
		EXPORT_C void MCertInfo::ExternalizeL(RWriteStream& aStream) const
		
	This method externalizes the MCertInfo object to the given stream.
	The iDeletable boolean attribute is combined with the iFormat attribute
	for certstore backward compatibility
*/
EXPORT_C void MCertInfo::ExternalizeL(RWriteStream& aStream) const
	{
	// insert iDeletable flag as most significant digit of iFormat in order
	//   store the flag without changing the externalized record format
	//   The value is OPPOSITE for backward compatibility
	TUint8 tmpValue = static_cast <TUint8>(iFormat | (iDeletable ? 0 : KReadOnlyFlagMask));
	
	aStream.WriteUint8L(tmpValue);
	aStream.WriteInt32L(iSize);
	aStream << iLabel;
	aStream.WriteInt32L(iCertificateId);
	aStream.WriteUint8L(iCertificateOwnerType);
	aStream << iSubjectKeyId;
	aStream << iIssuerKeyId;
	}

/**
	EXPORT_C void MCertInfo::InternalizeL(RReadStream& aStream)
	
	This method internalizes a MCertInfo object from the given stream.
	The iDeletable boolean and iFormat attributes are both extracted
	from the stored iFormat value using for certstore backward compatibility
*/
EXPORT_C void MCertInfo::InternalizeL(RReadStream& aStream)
	{
	// get first byte from stream containing iDeletable flag and iFormat value
	TUint8 tmpValue = aStream.ReadUint8L();
	
	// extract iDeletable flag from most significant digit of iFormat
	//   set iDeletable to the OPPOSITE of the 8th bit value
	iDeletable = !(tmpValue & KReadOnlyFlagMask);
		
	// extract iFormat = the value of the 7 least significant bits
	iFormat = static_cast <TCertificateFormat>(tmpValue & KFormatMask);

	iSize = aStream.ReadInt32L();
	aStream >> iLabel;
	iCertificateId = aStream.ReadInt32L();
	iCertificateOwnerType = static_cast<TCertificateOwnerType>(aStream.ReadUint8L());
	aStream >> iSubjectKeyId;
	aStream >> iIssuerKeyId;
	
	if (!Valid())
		{
		User::Leave(KErrCorrupt);
		}
	}

// CCTCertInfo /////////////////////////////////////////////////////////////////

EXPORT_C CCTCertInfo* CCTCertInfo::NewL(RReadStream& aStream, MCTToken& aToken)
	{
	CCTCertInfo* self = CCTCertInfo::NewLC(aStream, aToken);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewLC(RReadStream& aStream, MCTToken& aToken)
	{
	CCTCertInfo* self = new(ELeave) CCTCertInfo(aToken);
	CleanupReleasePushL(*self);
	self->ConstructL(aStream);
	return self;
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewL(const TDesC& aLabel,
										TCertificateFormat aFormat,
										TCertificateOwnerType aCertificateOwnerType,
										TInt aSize,
										const TKeyIdentifier* aSubjectKeyId,		
										const TKeyIdentifier* aIssuerKeyId,
										MCTToken& aToken,		
										TInt aCertificateId)
	{
	CCTCertInfo* self = CCTCertInfo::NewLC(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId, aToken,
		aCertificateId, ETrue);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewL(const TDesC& aLabel,
										TCertificateFormat aFormat,
										TCertificateOwnerType aCertificateOwnerType,
										TInt aSize,
										const TKeyIdentifier* aSubjectKeyId,		
										const TKeyIdentifier* aIssuerKeyId,
										MCTToken& aToken,		
										TInt aCertificateId, 
										TBool aIsDeletable, 
										const TDesC8* aIssuerHash)
	{
	CCTCertInfo* self = CCTCertInfo::NewLC(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId, aToken,
		aCertificateId, aIsDeletable, aIssuerHash);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewLC(const TDesC& aLabel,
										 TCertificateFormat aFormat,
										 TCertificateOwnerType aCertificateOwnerType,
										 TInt aSize,
										 const TKeyIdentifier* aSubjectKeyId,
										 const TKeyIdentifier* aIssuerKeyId,
										 MCTToken& aToken, 
										 TInt aCertificateId)
	{
	return CCTCertInfo::NewLC(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId, aIssuerKeyId, 
		aToken, aCertificateId, ETrue);
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewLC(const TDesC& aLabel,
										 TCertificateFormat aFormat,
										 TCertificateOwnerType aCertificateOwnerType,
										 TInt aSize,
										 const TKeyIdentifier* aSubjectKeyId,
										 const TKeyIdentifier* aIssuerKeyId,
										 MCTToken& aToken, 
										 TInt aCertificateId, 
										 TBool aIsDeletable, 
										 const TDesC8* aIssuerHash)
	{
	CCTCertInfo* self = new(ELeave) CCTCertInfo(aLabel,
		aFormat, aCertificateOwnerType, aSize, aSubjectKeyId,
		aIssuerKeyId, aToken, aCertificateId, aIsDeletable);
	CleanupReleasePushL(*self);
	self->ConstructL(aIssuerHash);
	return self;
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewL(const CCTCertInfo& aCertInfo)
	{
	CCTCertInfo* self = CCTCertInfo::NewLC(aCertInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCTCertInfo* CCTCertInfo::NewLC(const CCTCertInfo& aCertInfo)
	{
	CCTCertInfo* self = new(ELeave) CCTCertInfo(aCertInfo);
	CleanupReleasePushL(*self);
	self->ConstructL(aCertInfo.IssuerHash());
	return self;
	}

CCTCertInfo::CCTCertInfo(MCTToken& aToken)
	: MCTTokenObject(aToken), iToken(aToken)
	{
	}

CCTCertInfo::CCTCertInfo(const TDesC& aLabel, 
						 TCertificateFormat aFormat,
						 TCertificateOwnerType aCertificateOwnerType,
						 TInt aSize,
						 const TKeyIdentifier* aSubjectKeyId,
						 const TKeyIdentifier* aIssuerKeyId,
						 MCTToken& aToken, 
						 TInt aCertificateId,
						 TBool aIsDeletable)
	: MCTTokenObject(aToken),
	  MCertInfo(aLabel, aFormat, aCertificateOwnerType, aSize, aSubjectKeyId,
				aIssuerKeyId, aCertificateId, aIsDeletable),
	  iToken(aToken)
	{
	}

CCTCertInfo::CCTCertInfo(const CCTCertInfo& aOther)
	: MCTTokenObject(aOther.iToken), MCertInfo(aOther), iToken(aOther.iToken)
	{
	}

void CCTCertInfo::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

void CCTCertInfo::ConstructL(const TDesC8* aIssuerHash)
	{
	MCertInfo::ConstructL(aIssuerHash);
	}

EXPORT_C CCTCertInfo::~CCTCertInfo()
	{
	}

const TDesC& CCTCertInfo::Label() const
	{
	return iLabel;
	}

TUid CCTCertInfo::Type() const
	{
	TUid uid = { KCTObjectCertInfo };
	return uid;
	}

EXPORT_C const TKeyIdentifier& CCTCertInfo::SubjectKeyId() const
	{
	return iSubjectKeyId;
	}

EXPORT_C const TKeyIdentifier& CCTCertInfo::IssuerKeyId() const
	{
	return iIssuerKeyId;
	}

EXPORT_C TCertificateFormat CCTCertInfo::CertificateFormat() const
	{
	return iFormat;
	}

EXPORT_C TCertificateOwnerType CCTCertInfo::CertificateOwnerType() const
	{
	return iCertificateOwnerType;
	}

EXPORT_C TInt CCTCertInfo::Size() const
	{
	return iSize;
	}

MCTToken& CCTCertInfo::Token() const
	{
	return iToken;
	}

EXPORT_C TCTTokenObjectHandle CCTCertInfo::Handle() const
	{
	return TCTTokenObjectHandle(iToken.Handle(), iCertificateId);
	}

EXPORT_C TBool CCTCertInfo::IsDeletable() const
	{
	return iDeletable;
	}

EXPORT_C const TDesC8* CCTCertInfo::IssuerHash() const
	{
	return MCertInfo::IssuerHash();
	}

EXPORT_C TBool CCTCertInfo::operator==(const CCTCertInfo& aCertInfo) const
	{
	return aCertInfo.iLabel == iLabel;
	}

EXPORT_C void CCTCertInfo::SetCertificateId(TInt aCertId)
{
	iCertificateId = aCertId;
}
