// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : SdpDocument.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include <e32std.h>
#include "sdpdocument.h"
#include "sdptimefield.h"
#include "sdpbandwidthfield.h"
#include "sdpattributefield.h"
#include "sdpmediafield.h"
#include "sdpconnectionfield.h"
#include "sdpkeyfield.h"
#include "sdporiginfield.h"
#include "sdprepeatfield.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecerr.h"
#include "sdpcodecconstants.h"
#include "uri8.h"
#include "uriutilscommon.h"
#include "badesca.h"
#include "_sdpdefs.h"
#include "sdpcodecstringpool.h"
#include "sdputil.h"
#include "sdpcodectemplate.h"
#include "sdpcodecparseutil.h"

_LIT8(KCRLF, "\r\n");
// ----------------------------------------------------------------------------
// CSdpDocument::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument* 
CSdpDocument::DecodeL(const TDesC8& aText)
	{
	CSdpDocument* sdpDocument = CSdpDocument::DecodeLC(aText);
	CleanupStack::Pop();
	return sdpDocument;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::DecodeLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument* 
CSdpDocument::DecodeLC(const TDesC8& aText)
	{
	__ASSERT_ALWAYS (aText.Length() > 0, User::Leave(KErrArgument));
	CSdpDocument* sdpDocument = new(ELeave)CSdpDocument;
	CleanupStack::PushL(sdpDocument);
	sdpDocument->ConstructL();
	sdpDocument->ParseL(aText);
	return sdpDocument;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument* CSdpDocument::NewL()
	{
	CSdpDocument* self = CSdpDocument::NewLC();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument* CSdpDocument::NewLC()
	{
	CSdpDocument* self = new(ELeave)CSdpDocument;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::~CSdpDocument
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument::~CSdpDocument()
	{
	delete iInfo;
	delete iSessionName;
	delete iZoneAdjustments;
	delete iSdpKeyField;
	delete iSdpOriginField;
	delete iSdpConnectionField;
	if(iTimeFields)
		{
		iTimeFields->ResetAndDestroy();
		delete iTimeFields;
		}
	if(iBandwidthFields)
		{
		iBandwidthFields->ResetAndDestroy();
		delete iBandwidthFields;
		}
	if(iAttributeFields)
		{
		iAttributeFields->ResetAndDestroy();
		delete iAttributeFields;
		}
	if(iMediaFields)
		{
		iMediaFields->ResetAndDestroy();
		delete iMediaFields;
		}
	delete iUri;
	if(iEmailFields)
		{
		iEmailFields->Reset();
		delete iEmailFields;
		}

	if(iPhoneFields)
		{
		iPhoneFields->Reset();
		delete iPhoneFields;
		}
	delete iToken;
	delete iSdpCodecParseUtil;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::ExternalizeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L (iSdpVersion);	
	SdpCodecTemplate<CSdpOriginField>::ExternalizeL(OriginField(), aStream);
	aStream.WriteUint32L (iSessionName->Length());
	if (iSessionName->Length() > 0)
		{
		aStream.WriteL (*iSessionName);
		}
	aStream.WriteUint32L (iInfo->Length());
	if (iInfo->Length() > 0)
		{
		aStream.WriteL (*iInfo);
		}
	if(Uri())
		{
		aStream.WriteUint8L (1);
		aStream.WriteUint32L ((iUri->Uri()).UriDes().Length());
		aStream.WriteL((iUri->Uri()).UriDes());
		}
	else
		{
		aStream.WriteUint8L (0);
		}
	ExternalizeDesCArrayL(*iEmailFields, aStream);
	ExternalizeDesCArrayL(*iPhoneFields, aStream);

	SdpCodecTemplate<CSdpConnectionField>::ExternalizeL(iSdpConnectionField,
														aStream);
	SdpCodecTemplate<CSdpBandwidthField>::ExternalizeArrayL(*iBandwidthFields,
															aStream);
	SdpCodecTemplate<CSdpTimeField>::ExternalizeArrayL(*iTimeFields, aStream);
	aStream.WriteUint32L (iZoneAdjustments->Length());
	if (iZoneAdjustments->Length() > 0)
		{
		aStream.WriteL (*iZoneAdjustments);
		}
	SdpCodecTemplate<CSdpKeyField>::ExternalizeL(Key(), aStream);
	SdpCodecTemplate<CSdpAttributeField>::ExternalizeArrayL(*iAttributeFields,
															aStream);
	SdpCodecTemplate<CSdpMediaField>::ExternalizeArrayL(*iMediaFields, 
															aStream);
	}

// ----------------------------------------------------------------------------
// CSdpDocument::InternalizeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument* 
CSdpDocument::InternalizeL(RReadStream& aStream)
	{
	CSdpDocument* self = new(ELeave)CSdpDocument;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->DoInternalizeL (aStream);
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::DoInternalizeL
// ----------------------------------------------------------------------------
//
void CSdpDocument::DoInternalizeL(RReadStream& aStream)
	{
	if(iTimeFields)
		{
		iTimeFields->ResetAndDestroy();
		}
	iSdpVersion = aStream.ReadUint32L();
	if(aStream.ReadUint8L())
		{
		SetOriginField(CSdpOriginField::InternalizeL(aStream));
		}
	SetSessionNameL(GetTokenFromStreamL(aStream));
	SetInfoL(GetTokenFromStreamL(aStream));
	if(aStream.ReadUint8L())
		{
		TUriParser8 parser;
		User::LeaveIfError(parser.Parse(GetTokenFromStreamL(aStream)));
		SetUri(CUri8::NewL(parser));
		}
	TInt i = 0;
	TInt count = aStream.ReadUint32L();
	for (i = 0;i < count;i++)
		{
		TPtrC8 temp(GetTokenFromStreamL(aStream));
		if(temp.Compare(KNullDesC8) != 0)
			{
			iEmailFields->AppendL(temp);
			}
		}
	count = aStream.ReadUint32L();
	for (i = 0;i < count;i++)
		{
		TPtrC8 temp(GetTokenFromStreamL(aStream));
		if(temp.Compare(KNullDesC8) != 0)
			{
			iPhoneFields->AppendL(temp);
			}
		}
	if(aStream.ReadUint8L())
		{
		SetConnectionField(CSdpConnectionField::InternalizeL(aStream));
		}
	SdpCodecTemplate<CSdpBandwidthField>::InternalizeArrayL(
												*iBandwidthFields, aStream);
	SdpCodecTemplate<CSdpTimeField>::InternalizeArrayL(*iTimeFields,
														aStream);
	SetZoneAdjustmentsL(GetTokenFromStreamL(aStream));
	if(aStream.ReadUint8L())
		{
		SetKey(CSdpKeyField::InternalizeL(aStream));
		}
	SdpCodecTemplate<CSdpAttributeField>::InternalizeArrayL(
												*iAttributeFields, aStream);
	SdpCodecTemplate<CSdpMediaField>::InternalizeArrayL(*iMediaFields,
															aStream);
	}

// ----------------------------------------------------------------------------
// CSdpDocument::EncodeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::EncodeL(RWriteStream& aStream) const
	{
	RStringF headername = iPool.StringF( SdpCodecStringConstants::EVersion, 
                                              SdpCodecStringConstants::Table );
    aStream.WriteL(headername.DesC());
	TBuf8<80> text;
	text.Format(_L8("%u"), iSdpVersion);
	aStream.WriteL(text);
	aStream.WriteL(KCRLF);	
	SdpCodecTemplate<CSdpOriginField>::EncodeL(OriginField(), aStream);
	SdpUtil::EncodeBufferL(*iSessionName, SdpCodecStringConstants::ESession,
																	aStream);
	SdpUtil::EncodeBufferL(*iInfo, SdpCodecStringConstants::EInfo, aStream);
	if(Uri())
		{
		SdpUtil::EncodeBufferL(((iUri->Uri()).UriDes()), 
					SdpCodecStringConstants::EUri, aStream);
		}
	EncodeDesCArrayL(*iEmailFields, SdpCodecStringConstants::EEmail, aStream);
	EncodeDesCArrayL(*iPhoneFields, SdpCodecStringConstants::EPhone, aStream);
	SdpCodecTemplate<CSdpConnectionField>::EncodeL(iSdpConnectionField,
														aStream);
	SdpCodecTemplate<CSdpBandwidthField>::EncodeArrayL(*iBandwidthFields,
														aStream);
	SdpCodecTemplate<CSdpTimeField>::EncodeArrayL(*iTimeFields, aStream);
	SdpUtil::EncodeBufferL(*iZoneAdjustments,
							SdpCodecStringConstants::EZone, aStream);
	SdpCodecTemplate<CSdpKeyField>::EncodeL(Key(), aStream);
	SdpCodecTemplate<CSdpAttributeField>::EncodeArrayL(*iAttributeFields, 
														aStream);
	SdpCodecTemplate<CSdpMediaField>::EncodeArrayL(*iMediaFields, aStream);
	}

// ----------------------------------------------------------------------------
// CSdpDocument::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpDocument* CSdpDocument::CloneL() const
	{
	CSdpDocument* sdpdocument = new(ELeave)CSdpDocument;
	CleanupStack::PushL(sdpdocument);
	sdpdocument->ConstructL(*this);
	CleanupStack::Pop();//sdpdocument
	return sdpdocument;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::IsValid
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpDocument::IsValid() const
	{
	if (iTimeFields->Count() == 0 || !OriginField() ||
        !SessionName().Length()||
		(!iMediaFields->Count() && !ConnectionField()))
		{
		return EFalse;
		}
    TInt j;
	for (j=0; j < iMediaFields->Count(); j++)
		{
		if(!(*iMediaFields)[j]->IsValid() || 
			(!ConnectionField() && (!(*iMediaFields)[j]->IsContactPresent())))
			{
			return EFalse;
			}
		}
    
    for (j=0; j < iTimeFields->Count(); j++)
		{
        if ( !(*iTimeFields)[j]->IsValid())
            {
		    return EFalse;
            }
		}
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SdpVersion
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CSdpDocument::SdpVersion() const
	{
	return iSdpVersion;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::OriginField
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpDocument::OriginField()
	{
	return iSdpOriginField;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::OriginField
// ----------------------------------------------------------------------------
//
EXPORT_C const CSdpOriginField* CSdpDocument::OriginField() const
	{
	return iSdpOriginField;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetOriginField
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetOriginField(CSdpOriginField* aObj)
	{
	delete iSdpOriginField;
	iSdpOriginField = aObj;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SessionName
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpDocument::SessionName() const
	{
	return *iSessionName;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetSessionNameL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetSessionNameL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Compare(KNullDesC8) == 0||
					aValue.Compare(_L8(" ")) == 0||
					SdpUtil::IsByteString(aValue),
	                 User::Leave(KErrSdpCodecSessionField));
	HBufC8* tmp = aValue.AllocL();
	delete iSessionName;
	iSessionName = tmp;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::Info
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpDocument::Info() const
	{
	return *iInfo;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetInfoL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetInfoL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Compare(KNullDesC8) == 0||
					SdpUtil::IsByteString(aValue),
	                 User::Leave(KErrSdpCodecInfoField));
	HBufC8* tmp = aValue.AllocL();
	tmp->Des().Trim();
	delete iInfo;
	iInfo = tmp;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::Uri
// ----------------------------------------------------------------------------
//
EXPORT_C CUri8* CSdpDocument::Uri() 
	{
	return iUri;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::Uri
// ----------------------------------------------------------------------------
//
EXPORT_C const CUri8* CSdpDocument::Uri() const
	{
	return iUri;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetUriL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetUri(CUri8* aValue)
	{
	delete iUri;
	iUri = aValue;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::EmailFields
// ----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array& CSdpDocument::EmailFields()
	{
	return *iEmailFields;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::PhoneFields
// ----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array& CSdpDocument::PhoneFields()
	{
	return *iPhoneFields;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::ConnectionField
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpDocument::ConnectionField()
	{
	return iSdpConnectionField;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::ConnectionField
// ----------------------------------------------------------------------------
//
EXPORT_C const CSdpConnectionField* CSdpDocument::ConnectionField() const
	{
	return iSdpConnectionField;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetConnectionField
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetConnectionField(CSdpConnectionField* aObj)
	{
	delete iSdpConnectionField;
	iSdpConnectionField = aObj;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::BandwidthFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpBandwidthField>& CSdpDocument::BandwidthFields()
	{
	return *iBandwidthFields;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::TimeFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpTimeField>& CSdpDocument::TimeFields()
	{
	return *iTimeFields;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::ZoneAdjustments
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpDocument::ZoneAdjustments() const
	{
	return *iZoneAdjustments;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetZoneAdjustmentsL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetZoneAdjustmentsL(const TDesC8& aValue)
	{
	HBufC8* tmp = aValue.AllocL();
	tmp->Des().Trim();
	delete iZoneAdjustments;
	iZoneAdjustments = tmp;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::Key
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField* CSdpDocument::Key()
	{
	return iSdpKeyField;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::Key
// ----------------------------------------------------------------------------
//
EXPORT_C const CSdpKeyField* CSdpDocument::Key() const
	{
	return iSdpKeyField;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::SetKey
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpDocument::SetKey(CSdpKeyField* aObj)
	{
	delete iSdpKeyField;
	iSdpKeyField = aObj;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::AttributeFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpAttributeField>& CSdpDocument::AttributeFields()
	{
	return *iAttributeFields;
	}

// ----------------------------------------------------------------------------
// CSdpDocument::MediaFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpMediaField>& CSdpDocument::MediaFields()
	{
	return *iMediaFields;
	}
	
// ----------------------------------------------------------------------------
// CSdpDocument::CSdpDocument
// ----------------------------------------------------------------------------
//
CSdpDocument::CSdpDocument()
	{
	}
	
// ----------------------------------------------------------------------------
// CSdpDocument::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpDocument::ConstructL()
	{
	iPool = SdpCodecStringPool::StringPoolL();
	iInfo = KNullDesC8().AllocL();
	iSessionName = KNullDesC8().AllocL();
	iZoneAdjustments = KNullDesC8().AllocL();
	iSdpVersion = 0;
	iEmailFields = new(ELeave)CDesC8ArraySeg(1);
	iPhoneFields = new(ELeave)CDesC8ArraySeg(1);
	iBandwidthFields = new(ELeave)RPointerArray<CSdpBandwidthField>;
	iTimeFields = new(ELeave)RPointerArray<CSdpTimeField>;
	iAttributeFields = new(ELeave)RPointerArray<CSdpAttributeField>;
	iMediaFields = new(ELeave)RPointerArray<CSdpMediaField>;
	CSdpTimeField* timefield = CSdpTimeField::NewLC(_L8("0"),_L8("0"));
	User::LeaveIfError(iTimeFields->Append(timefield));
	CleanupStack::Pop();
	iToken = KNullDesC8().AllocL();
	}

// ----------------------------------------------------------------------------
// CSdpDocument::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpDocument::ConstructL(const CSdpDocument& aSdpDocument)
	{
	ConstructL();
	iSdpVersion = aSdpDocument.SdpVersion();
	if ( aSdpDocument.OriginField() )
		{
		iSdpOriginField = aSdpDocument.OriginField()->CloneL();
		}
	delete iSessionName;
	iSessionName = 0;
	iSessionName = aSdpDocument.SessionName().AllocL();
	HBufC8* tmp = aSdpDocument.Info().AllocL();
	delete iInfo;
	iInfo = tmp;
	if ( aSdpDocument.Uri() )
		{
		iUri = CUri8::NewL(aSdpDocument.Uri()->Uri());
		}
	TInt i = 0;
	for ( i=0; i < (aSdpDocument.iEmailFields)->MdcaCount(); i++ )
		{
		iEmailFields->AppendL(aSdpDocument.iEmailFields->MdcaPoint(i));
		}

	for (i=0; i < (aSdpDocument.iPhoneFields)->MdcaCount(); i++)
		{
		iPhoneFields->AppendL(aSdpDocument.iPhoneFields->MdcaPoint(i));
		}

	if ( aSdpDocument.ConnectionField() )
		{
		iSdpConnectionField = aSdpDocument.ConnectionField()->CloneL();
		}
	SdpCodecTemplate<CSdpBandwidthField>::CloneArrayL(*iBandwidthFields, 
											*aSdpDocument.iBandwidthFields);
	if ( iTimeFields )
		{
		iTimeFields->ResetAndDestroy();
		}
	SdpCodecTemplate<CSdpTimeField>::CloneArrayL(*iTimeFields, 
												*aSdpDocument.iTimeFields);
	delete iZoneAdjustments;
	iZoneAdjustments = 0;
	iZoneAdjustments = aSdpDocument.ZoneAdjustments().AllocL();
	if(aSdpDocument.Key())
		{
		iSdpKeyField = aSdpDocument.Key()->CloneL();
		}
	SdpCodecTemplate<CSdpAttributeField>::CloneArrayL(*iAttributeFields,
											*aSdpDocument.iAttributeFields);
	SdpCodecTemplate<CSdpMediaField>::CloneArrayL(*iMediaFields, 
												*aSdpDocument.iMediaFields);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseL (const TDesC8& aText)
	{
	if(iTimeFields)
		{
		iTimeFields->ResetAndDestroy();
		delete iTimeFields;
		iTimeFields = 0;
		}
	iSdpCodecParseUtil = CSdpCodecParseUtil::NewL(iPool,aText,KErrSdpCodecDecode);
	ParseSessionVersionL();
	ParseSessionOwnerL();
	ParseSessionNameL();
	ParseSessionInformationL();
	ParseUriL();
	ParseEmailL();
	ParsePhoneL();
	ParseConnectionL();
	ParseBandwidthL();
	ParseTimeFieldL();
	ParseZoneAdjustmentL();
	ParseEncryptionKeyL();
	ParseAttributeFieldL();
	ParseMediaLevelL ();
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseSessionVersionL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseSessionVersionL()
	{
	iSdpVersion = 
		iSdpCodecParseUtil->ParseSessionVersionL(KErrSdpCodecSessionField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseSessionOwnerL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseSessionOwnerL()
	{
	iSdpOriginField = iSdpCodecParseUtil->ParseSessionOwnerL(
													KErrSdpCodecOriginField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseSessionNameL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseSessionNameL()
	{
	SetSessionNameL(iSdpCodecParseUtil->ParseSessionNameL(
													KErrSdpCodecSessionField));
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseSessionInformationL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseSessionInformationL()
	{
	SetInfoL(iSdpCodecParseUtil->ParseInformationL(KErrSdpCodecInfoField));
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseUriL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseUriL()
	{
	SetUri(iSdpCodecParseUtil->ParseUriL(KErrSdpCodecUriField));
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseEmailL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseEmailL()
	{
	delete iEmailFields;
	iEmailFields = NULL;
	iEmailFields = iSdpCodecParseUtil->ParseEmailL(KErrSdpCodecEmailField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParsePhoneL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParsePhoneL()
	{
	delete iPhoneFields;
	iPhoneFields = NULL;
	iPhoneFields = iSdpCodecParseUtil->ParsePhoneL(KErrSdpCodecPhoneField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseConnectionL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseConnectionL()
	{
	iSdpConnectionField = 
		iSdpCodecParseUtil->ParseConnectionL(KErrSdpCodecConnectionField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseBandwidthL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseBandwidthL()
	{
	delete iBandwidthFields;
	iBandwidthFields = 0;
	iBandwidthFields = 
		iSdpCodecParseUtil->ParseBandwidthL(KErrSdpCodecBandwidthField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseTimeFieldL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseTimeFieldL()
	{
	delete iTimeFields;
	iTimeFields = 0;
	iTimeFields = iSdpCodecParseUtil->ParseTimeFieldL(KErrSdpCodecTimeField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseZoneAdjustmentL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseZoneAdjustmentL()
	{
	SetZoneAdjustmentsL(
		iSdpCodecParseUtil->ParseZoneAdjustmentL(KErrSdpCodecZoneField));
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseEncryptionKeyL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseEncryptionKeyL()
	{
	SetKey(iSdpCodecParseUtil->ParseEncryptionKeyL(KErrSdpCodecKeyField));
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseAttributeFieldL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseAttributeFieldL()
	{
	delete iAttributeFields;
	iAttributeFields = 0;
	iAttributeFields = 
		iSdpCodecParseUtil->ParseAttributeFieldL(KErrSdpCodecAttributeField);
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ParseMediaLevelL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ParseMediaLevelL()
	{
	delete iMediaFields;
	iMediaFields = 0;
	iMediaFields = iSdpCodecParseUtil->ParseMediaLevelL();
	}

// ---------------------------------------------------------------------------
// CSdpDocument::ExternalizeDesCArrayL
// ---------------------------------------------------------------------------
//
void CSdpDocument::ExternalizeDesCArrayL(CDesC8ArraySeg& aArray,
						   RWriteStream& aStream) const
	{
	aStream.WriteUint32L (aArray.MdcaCount());
	for (TInt i = 0;i < aArray.MdcaCount();i++)
		{
		aStream.WriteUint32L (aArray.MdcaPoint(i).Length());
		aStream.WriteL(aArray.MdcaPoint(i));
		}
	}

// ---------------------------------------------------------------------------
// CSdpDocument::EncodeDesCArrayL
// ---------------------------------------------------------------------------
//
void CSdpDocument::EncodeDesCArrayL(CDesC8ArraySeg& aArray, TInt aIndex,
										RWriteStream& aStream) const
	{
	RStringF headername = 
					iPool.StringF(aIndex, SdpCodecStringConstants::Table);
	for (TInt i = 0;i < aArray.MdcaCount();i++)
		{
		if(aArray.MdcaPoint(i).Compare(KNullDesC8) != 0)
			{
			aStream.WriteL(headername.DesC());
			aStream.WriteL(aArray.MdcaPoint(i));
			aStream.WriteL(KCRLF);
			}
		}
	}

// ---------------------------------------------------------------------------
// CSdpDocument::GetTokenFromStreamL
// ---------------------------------------------------------------------------
//
TDesC8& CSdpDocument::GetTokenFromStreamL(RReadStream& aStream)
	{
	TUint32 tokenLength = aStream.ReadUint32L();
	HBufC8* tmp = 0;
	if (tokenLength > 0)
		{
		tmp = HBufC8::NewLC(tokenLength);
		TPtr8 tokenptr(tmp->Des());
		aStream.ReadL (tokenptr, tokenLength);
		CleanupStack::Pop();//tmp
		}
	else
		{
		tmp = KNullDesC8().AllocL();
		}
	delete iToken;
	iToken = tmp;
	return *iToken;
	}

