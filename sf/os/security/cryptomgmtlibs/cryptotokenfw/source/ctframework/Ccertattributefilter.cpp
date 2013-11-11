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


#include "ccertattributefilter.h"

/**
 * An enumeration identifying the possible fields in a CCertAttributeFilter, use
 * for serialisation.
 */
enum TCertAttribute
	{
	ECertAttributeNull,
	ECertAttributeLabel,
	ECertAttributeUid,
	ECertAttributeFormat,
	ECertAttributeOwnerType,
	ECertAttributeKeyUsage,
	ECertAttributeSubjectKeyId,
	ECertAttributeIssuerKeyId,
	ECertAttributeSubjectKeyId_v2,
	ECertAttributeIssuerKeyId_v2
	};

EXPORT_C CCertAttributeFilter* CCertAttributeFilter::NewL()
	{
	CCertAttributeFilter* self = CCertAttributeFilter::NewLC();
	CleanupStack::Pop(self);	// self
	return self;
	}

EXPORT_C CCertAttributeFilter* CCertAttributeFilter::NewLC()
	{
	CCertAttributeFilter* self = new(ELeave) CCertAttributeFilter();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CCertAttributeFilter* CCertAttributeFilter::NewL(RReadStream& aStream)
	{
	CCertAttributeFilter* self = new(ELeave) CCertAttributeFilter();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Externalisation - only output the fields that are set, using the
 * TCertAttribute enum to identify them.  Mark the end of the list with
 * ECertAttributeNull.
 */
EXPORT_C void CCertAttributeFilter::ExternalizeL(RWriteStream& aStream) const
	{
	if (iLabelIsSet)
		{
		aStream.WriteUint8L(ECertAttributeLabel);
		aStream.WriteInt32L(iLabel.Length()); 
		aStream.WriteL(iLabel, iLabel.Length());
		}
	if (iUidIsSet)
		{
		aStream.WriteUint8L(ECertAttributeUid);
		aStream.WriteUint32L(iUid.iUid);
		}
	if (iFormatIsSet)
		{
		aStream.WriteUint8L(ECertAttributeFormat);
		aStream.WriteUint32L(iFormat);
		}
	if (iOwnerTypeIsSet)
		{
		aStream.WriteUint8L(ECertAttributeOwnerType);
		aStream.WriteUint32L(iOwnerType);
		}
	if (iKeyUsage != EX509UsageNone)
		{
		aStream.WriteUint8L(ECertAttributeKeyUsage);
		aStream.WriteUint32L(iKeyUsage);
		}
	if (iSubjectKeyIdIsSet)
		{
		if(iSubjectKeyId.Length() == KSHA1HashLengthBytes /* 20 */)
			{
			aStream.WriteUint8L(ECertAttributeSubjectKeyId);
			}
		else
			{
			aStream.WriteUint8L(ECertAttributeSubjectKeyId_v2);
			aStream.WriteInt32L(iSubjectKeyId.Length());
			}
		
		aStream.WriteL(iSubjectKeyId);
		}
	if (iIssuerKeyIdIsSet)
		{
		if(iIssuerKeyId.Length() == KSHA1HashLengthBytes /* 20 */)
			{
			aStream.WriteUint8L(ECertAttributeIssuerKeyId);
			}
		else
			{
			aStream.WriteUint8L(ECertAttributeIssuerKeyId_v2);
			aStream.WriteInt32L(iIssuerKeyId.Length());
			}
		
		aStream.WriteL(iIssuerKeyId);
		}
	aStream.WriteUint8L(ECertAttributeNull);
	}

void CCertAttributeFilter::InternalizeL(RReadStream& aStream)
	{
	for (;;)
		{
		TCertAttribute att = static_cast<TCertAttribute>(aStream.ReadUint8L());
		switch (att)
			{
			case ECertAttributeNull:
				// We're finished
				return;

			case ECertAttributeLabel:
				{
								
				TCertLabel label;
				TInt length = aStream.ReadInt32L();
				aStream.ReadL(label , length );
							
				SetLabel(label);
				}
				break;
				
			case ECertAttributeUid:
				{
				TUid uid;
				uid.iUid = aStream.ReadUint32L();
				SetUid(uid);
				}
				break;
				
			case ECertAttributeFormat:
				SetFormat(static_cast<TCertificateFormat>(aStream.ReadUint32L()));
				break;
				
			case ECertAttributeOwnerType:
				SetOwnerType(static_cast<TCertificateOwnerType>(aStream.ReadUint32L()));
				break;
				
			case ECertAttributeKeyUsage:
				SetKeyUsage(static_cast<TKeyUsageX509>(aStream.ReadUint32L()));
				break;
			
			case ECertAttributeSubjectKeyId:
				{
				TKeyIdentifier id;
				aStream.ReadL(id);
				SetSubjectKeyId(id);
				}
				break;
				
			case ECertAttributeSubjectKeyId_v2:
				{
				TKeyIdentifier id;
				TInt length = aStream.ReadInt32L();
				aStream.ReadL(id, length);
				SetSubjectKeyId(id);
				}
				break;
				
			case ECertAttributeIssuerKeyId:
				{
				TKeyIdentifier id;
				aStream.ReadL(id);
				SetIssuerKeyId(id);
				}
				break;
				
			case ECertAttributeIssuerKeyId_v2:
				{
				TKeyIdentifier id;
				TInt length = aStream.ReadInt32L();
				aStream.ReadL(id, length);
				SetIssuerKeyId(id);
				}
				break;
			default:
				User::Leave(KErrCorrupt);
			}
		}
	}

EXPORT_C void CCertAttributeFilter::SetLabel(const TCertLabel& aLabel)
	{
	iLabel = aLabel;
	iLabelIsSet = ETrue;
	}

EXPORT_C void CCertAttributeFilter::SetUid(TUid aUid)
	{
	iUid = aUid;
	iUidIsSet = ETrue;
	}

EXPORT_C void CCertAttributeFilter::SetFormat(TCertificateFormat aFormat)
	{
	iFormat = aFormat;
	iFormatIsSet = ETrue;
	}

EXPORT_C void CCertAttributeFilter::SetOwnerType(TCertificateOwnerType aOwnerType)
	{
	iOwnerType = aOwnerType;
	iOwnerTypeIsSet = ETrue;
	}

EXPORT_C void CCertAttributeFilter::SetKeyUsage(const TKeyUsageX509 aKeyUsage)
	{
	iKeyUsage = aKeyUsage;
	}

CCertAttributeFilter::CCertAttributeFilter()
	: iKeyUsage(EX509UsageAll)
	{
	}

EXPORT_C void CCertAttributeFilter::SetSubjectKeyId(const TKeyIdentifier& aSubjectKeyId)
	{
	iSubjectKeyId = aSubjectKeyId;
	iSubjectKeyIdIsSet = ETrue;
	}

EXPORT_C void CCertAttributeFilter::SetIssuerKeyId(const TKeyIdentifier& aIssuerKeyId)
	{
	iIssuerKeyId = aIssuerKeyId;
	iIssuerKeyIdIsSet = ETrue;
	}
