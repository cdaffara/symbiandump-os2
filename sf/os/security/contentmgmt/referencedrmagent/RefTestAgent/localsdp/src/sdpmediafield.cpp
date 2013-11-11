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
// Name          : SdpMediaField.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32mem.h>
#include <s32strm.h>
#include "sdpmediafield.h"
#include "sdpconnectionfield.h"
#include "sdpbandwidthfield.h"
#include "sdpattributefield.h"
#include "sdpkeyfield.h"
#include "sdpcodecstringconstants.h"
#include "sdputil.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"
#include "sdpcodecstringpool.h"
#include "sdpfmtattributefield.h"
#include "sdpcodectemplate.h"
#include "sdpcodecerr.h"
#include "sdpcodecparseutil.h"
#include "sdpcodecconstants.h"

// ----------------------------------------------------------------------------
// CSdpMediaField::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpMediaField* 
CSdpMediaField::DecodeL(const TDesC8& aText, TBool aRecurse)
	{
	CSdpMediaField* obj = DecodeLC(aText,aRecurse);
	CleanupStack::Pop();
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::DecodeLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpMediaField* 
CSdpMediaField::DecodeLC(const TDesC8& aText, TBool aRecurse)
	{
	__ASSERT_ALWAYS (aText.Length() > 0, User::Leave(KErrSdpCodecMediaField));
	CSdpMediaField* obj = new(ELeave)CSdpMediaField;
	CleanupStack::PushL(obj);
	obj->ConstructL(aRecurse);
	obj->ParseL(aText);
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpMediaField* CSdpMediaField::NewL(RStringF aMedia,
											TUint aPort,
											RStringF aProtocol,
											const TDesC8& aFormatList)
	{
	CSdpMediaField* obj = NewLC(aMedia, aPort, aProtocol, aFormatList);
	CleanupStack::Pop();
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpMediaField* CSdpMediaField::NewLC(RStringF aMedia,
											TUint aPort,
											RStringF aProtocol,
											const TDesC8& aFormatList)
	{
	CSdpMediaField* obj = new(ELeave)CSdpMediaField;
	CleanupStack::PushL(obj);
	obj->ConstructL(aMedia, aPort, aProtocol, aFormatList);
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::~CSdpMediaField
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpMediaField::~CSdpMediaField()
	{
	delete iInfo;
	delete iSdpKeyField;
	iElementArray.Reset();
	iMedia.Close();
	iProtocol.Close();
	delete iFormatList;
	delete iToken;
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
	if(iConnectionFields)
		{
		iConnectionFields->ResetAndDestroy();
		delete iConnectionFields;
		}
	if(iFmtAttrFields)
		{
		iFmtAttrFields->ResetAndDestroy();
		delete iFmtAttrFields;
		}
	delete iSdpCodecParseUtil;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpMediaField * CSdpMediaField::CloneL(TBool aRecurse) const
	{
	CSdpMediaField* obj = new(ELeave)CSdpMediaField;
	CleanupStack::PushL(obj);
	obj->ConstructL(aRecurse);
	obj->ConstructL(*this);
	CleanupStack::Pop();//obj
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::operator ==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpMediaField::operator == (const CSdpMediaField& aObj) const
	{
	TBool result = ETrue;
	if(	!(iMedia==aObj.Media()) ||
		!(iPort == aObj.Port()) ||
		!(iPortCount == aObj.iPortCount) ||
		!(iProtocol == aObj.Protocol()) ||
		!(*iFormatList==(*aObj.iFormatList)) ||
		(!(*iInfo==(*aObj.iInfo)))||
		(!iSdpKeyField && aObj.Key())||
		(iSdpKeyField && !aObj.Key())||
		((iSdpKeyField && aObj.Key()) && (!(*iSdpKeyField==(*aObj.Key()))))||
		(iBandwidthFields->Count() != aObj.iBandwidthFields->Count())||
		(iAttributeFields->Count() != aObj.iAttributeFields->Count())||
		(iFmtAttrFields->Count() != aObj.iFmtAttrFields->Count())||
		(iConnectionFields->Count() != aObj.iConnectionFields->Count()))
		{
		result = EFalse;
		}

	TInt i  = 0;
	if(result)
		{
		for (i=0; i < (aObj.iConnectionFields)->Count(); i++)
			{
			if((!(*iConnectionFields)[i] && (*aObj.iConnectionFields)[i]) ||
				((*iConnectionFields)[i] && !(*aObj.iConnectionFields)[i]))
				{
				return EFalse;
				}

			if(((*iConnectionFields)[i] && (*aObj.iConnectionFields)[i]) &&
				(!(*(*iConnectionFields)[i]==(*(*aObj.iConnectionFields)[i]))))
				{	
				return EFalse;
				}
			}

		for (i=0; i < (aObj.iBandwidthFields)->Count(); i++)
			{
			if((!(*iBandwidthFields)[i] && (*aObj.iBandwidthFields)[i]) ||
				((*iBandwidthFields)[i] && !(*aObj.iBandwidthFields)[i]))
				{
				return EFalse;
				}

			if(((*iBandwidthFields)[i] && (*aObj.iBandwidthFields)[i]) &&
				(!(*(*iBandwidthFields)[i]==(*(*aObj.iBandwidthFields)[i]))))
				{	
				return EFalse;
				}
			}

		for (i=0; i < (aObj.iAttributeFields)->Count(); i++)
			{
			if((!((*iAttributeFields)[i]) && ((*aObj.iAttributeFields)[i]))||
				((*iAttributeFields)[i] && !(*aObj.iAttributeFields)[i]))
				{
				return EFalse;
				}
	
			if(((*iAttributeFields)[i] && (*aObj.iAttributeFields)[i]) &&
				(!(*(*iAttributeFields)[i]==(*(*aObj.iAttributeFields)[i]))))
				{
				return EFalse;
				}
			}

		for (i=0; i < (aObj.iFmtAttrFields)->Count(); i++)
			{
			if((!((*iFmtAttrFields)[i]) && ((*aObj.iFmtAttrFields)[i]))||
				((*iFmtAttrFields)[i] && !(*aObj.iFmtAttrFields)[i]))
				{
				return EFalse;
				}
	
			if(((*iFmtAttrFields)[i] && (*aObj.iFmtAttrFields)[i]) &&
				(!(*(*iFmtAttrFields)[i]==(*(*aObj.iFmtAttrFields)[i]))))
				{
				return EFalse;
				}
			}
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::Media
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpMediaField::Media() const
	{
	return iMedia;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetMedia
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetMediaL(RStringF aMedia)
	{
	__ASSERT_ALWAYS(SdpUtil::IsTokenChar(aMedia.DesC()), 
					User::Leave(KErrSdpCodecMediaField));
	iMedia.Close();
	iMedia = aMedia.Copy();
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::Protocol
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpMediaField::Protocol() const
	{
	return iProtocol;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetProtocol
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetProtocolL(RStringF aProtocol)
	{
	__ASSERT_ALWAYS(SdpUtil::IsTokenCharWithOptionalSlash(aProtocol.DesC()),
					User::Leave(KErrSdpCodecMediaField));
    if (aProtocol == iPool.StringF( SdpCodecStringConstants::EProtocolUdp, 
                            SdpCodecStringConstants::Table ))
        {
	    __ASSERT_ALWAYS((iPort == 0 || (iPort >= 1024 && iPort <= 65535)),
					User::Leave(KErrSdpCodecMediaField));
        }

	iProtocol.Close();
	iProtocol = aProtocol.Copy();    
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::Port
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CSdpMediaField::Port() const
	{
	return iPort;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetPortL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetPortL(TUint aPort)
	{
    if ( iProtocol == iPool.StringF( SdpCodecStringConstants::EProtocolUdp, 
                            SdpCodecStringConstants::Table ))
        {
	    __ASSERT_ALWAYS((aPort == 0 || (aPort >= 1024 && aPort <= 65535)),
					User::Leave(KErrSdpCodecMediaField));
        }
	iPort = aPort;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::PortCount
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CSdpMediaField::PortCount() const
	{
	return iPortCount;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetPortCount
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetPortCountL(TUint aCount)
	{
	__ASSERT_ALWAYS(aCount > 0, User::Leave(KErrSdpCodecMediaField));
	iPortCount = aCount;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::FormatList
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpMediaField::FormatList() const
	{
	return *iFormatList;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetFormatListL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetFormatListL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS(SdpUtil::IsTokenCharWithSpacesL(aValue), 
										User::Leave(KErrSdpCodecMediaField));
	HBufC8* tmp = aValue.AllocL();
	delete iFormatList;
	iFormatList = tmp;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::CSdpMediaField
// ----------------------------------------------------------------------------
//
CSdpMediaField::CSdpMediaField()
	{
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpMediaField::ConstructL()
	{
	iPool = SdpCodecStringPool::StringPoolL();
	iInfo = KNullDesC8().AllocL();
	iFormatList = KNullDesC8().AllocL();
	iToken = KNullDesC8().AllocL();
	iBandwidthFields = new(ELeave)RPointerArray<CSdpBandwidthField>;
	iConnectionFields = new(ELeave)RPointerArray<CSdpConnectionField>;
	iAttributeFields = new(ELeave)RPointerArray<CSdpAttributeField>;
	iFmtAttrFields = new(ELeave)RPointerArray<CSdpFmtAttributeField>;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpMediaField::ConstructL(TBool aRecurse)
	{
	ConstructL();
	iRecurse = aRecurse;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::ConstructL
// ----------------------------------------------------------------------------
//
void 
CSdpMediaField::ConstructL(RStringF aMedia, TUint aPort, RStringF aProtocol,
						const TDesC8& aFormatList)
	{
	__ASSERT_ALWAYS(SdpUtil::IsTokenChar(aMedia.DesC()) &&
					(aPort== 0 || (aPort >= 1024 && aPort <= 65535)) &&
					SdpUtil::IsTokenCharWithOptionalSlash(aProtocol.DesC()) &&
					SdpUtil::IsTokenCharWithSpacesL(aFormatList), 
					User::Leave(KErrArgument));
	ConstructL();
	iMedia = aMedia.Copy();
	iProtocol = aProtocol.Copy(); 
	SetPortL(aPort);
	delete iFormatList;
	iFormatList = 0;
	iFormatList = aFormatList.AllocL();	
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpMediaField::ConstructL(const CSdpMediaField& aSdpMediaField)
	{
	iMedia = aSdpMediaField.Media().Copy();
	iProtocol = aSdpMediaField.Protocol().Copy();
    SetPortL(aSdpMediaField.Port());
	iPortCount = aSdpMediaField.PortCount();
	HBufC8* tmp = aSdpMediaField.FormatList().AllocL();
	delete iFormatList;
	iFormatList = 0;
	iFormatList = tmp;
	if(iRecurse)
		{
		tmp = aSdpMediaField.Info().AllocL();
		delete iInfo;
		iInfo = tmp;
		SdpCodecTemplate<CSdpConnectionField>::CloneArrayL(*iConnectionFields,
			*aSdpMediaField.iConnectionFields);
		SdpCodecTemplate<CSdpBandwidthField>::CloneArrayL(*iBandwidthFields,
			*aSdpMediaField.iBandwidthFields);
		if(aSdpMediaField.Key())
			{
			iSdpKeyField = aSdpMediaField.Key()->CloneL();
			}
		SdpCodecTemplate<CSdpAttributeField>::CloneArrayL(*iAttributeFields,
			*aSdpMediaField.iAttributeFields);
		SdpCodecTemplate<CSdpFmtAttributeField>::CloneArrayL(*iFmtAttrFields,
			*aSdpMediaField.iFmtAttrFields);		
		}
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::Info
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpMediaField::Info() const
	{
	return *iInfo;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetInfoL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetInfoL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Compare(KNullDesC8) == 0||
					SdpUtil::IsByteString(aValue),
					User::Leave(KErrSdpCodecMediaInfoField));
	HBufC8* tmp = aValue.AllocL();
	tmp->Des().Trim();
	delete iInfo;
	iInfo = tmp;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::Key
// ----------------------------------------------------------------------------
//
EXPORT_C CSdpKeyField* CSdpMediaField::Key()
	{
	return iSdpKeyField;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::Key
// ----------------------------------------------------------------------------
//
EXPORT_C const CSdpKeyField* CSdpMediaField::Key() const
	{
	return iSdpKeyField;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::SetKey
// ----------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::SetKey(CSdpKeyField* aObj)
	{
	delete iSdpKeyField;
	iSdpKeyField = aObj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::ConnectionFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpConnectionField>& CSdpMediaField::ConnectionFields()
	{
	return *iConnectionFields;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::BandwidthFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpBandwidthField>& CSdpMediaField::BandwidthFields()
	{
	return *iBandwidthFields;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::AttributeFields
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpAttributeField>& CSdpMediaField::AttributeFields()
	{
	return *iAttributeFields;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::FormatAttributeFields()
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSdpFmtAttributeField>& 
CSdpMediaField::FormatAttributeFields()
	{
	return *iFmtAttrFields;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::EncodeL
// ----------------------------------------------------------------------------
//
EXPORT_C void 
CSdpMediaField::EncodeL(RWriteStream& aStream, TBool aRecurse) const
	{
	RStringF headername = iPool.StringF( SdpCodecStringConstants::EMedia, 
                                               SdpCodecStringConstants::Table );
    aStream.WriteL(headername.DesC());
	aStream.WriteL(iMedia.DesC());
	aStream.WriteL(KSPStr);
	TBuf8<80> text;
	text.Format(_L8("%u"), iPort);
	aStream.WriteL(text);
	if(iPortCount>0)
		{
		aStream.WriteL(_L8("/"));
		text.Format(_L8("%u"), iPortCount);
	    aStream.WriteL(text);
		}
	aStream.WriteL(KSPStr);
	aStream.WriteL(iProtocol.DesC());
	aStream.WriteL(KSPStr);
	aStream.WriteL(*iFormatList);
	aStream.WriteL(KCRLFStr);	
	if(aRecurse)
		{
		SdpUtil::EncodeBufferL(*iInfo, 
						SdpCodecStringConstants::EInfo, aStream);
		SdpCodecTemplate<CSdpConnectionField>::EncodeArrayL(*iConnectionFields,
															aStream);
		SdpCodecTemplate<CSdpBandwidthField>::EncodeArrayL(*iBandwidthFields,
															aStream);
		SdpCodecTemplate<CSdpKeyField>::EncodeL(Key(), aStream);

		for (TInt i = 0;i < iAttributeFields->Count();i++)
			{
			if (!(((*iAttributeFields)[i])->IsFmtAttribute()))
				{
				((*iAttributeFields)[i])->EncodeL(aStream);
				}
			}

		for (TInt i = 0;i < iFmtAttrFields->Count();i++)
			{
			((*iFmtAttrFields)[i])->EncodeL(aStream);
			
			for (TInt j=0; j<iAttributeFields->Count(); j++)
				{
				if ((((*iAttributeFields)[j])->IsFmtAttribute()) &&
					((*iAttributeFields)[j])->BelongsTo(*(*iFmtAttrFields)[i]))
					{
					((*iAttributeFields)[j])->EncodeL(aStream);
					}
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::IsValid
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpMediaField::IsValid() const
	{
	TBool result = EFalse;
	if(!(iMedia.DesC()==(KNullDesC8)) &&
		!(iProtocol.DesC()==(KNullDesC8))&&
		!(*iFormatList==(KNullDesC8)))
		{
		result = ETrue;
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::IsContactPresent
// ----------------------------------------------------------------------------
//
TBool CSdpMediaField::IsContactPresent() const
	{
	TBool result = EFalse;
	if(iConnectionFields->Count() > 0)
		{
		result = ETrue;
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::ExternalizeL
// ----------------------------------------------------------------------------
//
void CSdpMediaField::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L (iMedia.DesC().Length());
	if (iMedia.DesC().Length() > 0)
		{
		aStream.WriteL (iMedia.DesC());
		}
	aStream.WriteUint32L (iPort);
	if(iPortCount)
		{
		aStream.WriteUint8L (1);
		aStream.WriteUint32L (iPortCount);
		}
	else
		{
		aStream.WriteUint8L (0);
		}
	aStream.WriteUint32L (iProtocol.DesC().Length());
	if (iProtocol.DesC().Length() > 0)
		{
		aStream.WriteL (iProtocol.DesC());
		}
	aStream.WriteUint32L (iFormatList->Length());
	if (iFormatList->Length() > 0)
		{
		aStream.WriteL (*iFormatList);
		}
	aStream.WriteUint32L (iInfo->Length());
	if (iInfo->Length() > 0)
		{
		aStream.WriteL (*iInfo);
		}
	SdpCodecTemplate<CSdpConnectionField>::ExternalizeArrayL(*iConnectionFields,
															aStream);
	SdpCodecTemplate<CSdpBandwidthField>::ExternalizeArrayL(*iBandwidthFields,
															aStream);
	SdpCodecTemplate<CSdpKeyField>::ExternalizeL(Key(), aStream);
	SdpCodecTemplate<CSdpAttributeField>::ExternalizeArrayL(*iAttributeFields,
															aStream);
	SdpCodecTemplate<CSdpFmtAttributeField>::ExternalizeArrayL(*iFmtAttrFields,
																aStream);
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::InternalizeL
// ----------------------------------------------------------------------------
//
CSdpMediaField* CSdpMediaField::InternalizeL(RReadStream& aStream)
	{
	CSdpMediaField* obj = new(ELeave)CSdpMediaField();
	CleanupStack::PushL(obj);
	obj->ConstructL();
	obj->DoInternalizeL (aStream);
	CleanupStack::Pop();
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpMediaField::DoInternalizeL
// ----------------------------------------------------------------------------
//
void CSdpMediaField::DoInternalizeL(RReadStream& aStream)
	{
	RStringF media = iPool.OpenFStringL(GetTokenFromStreamL(aStream));
	CleanupClosePushL(media);
	SetMediaL(media);
	CleanupStack::Pop();//media
	media.Close();
    TUint32 port = aStream.ReadUint32L();
	//SetPortL(aStream.ReadUint32L());
	TUint flag = aStream.ReadUint8L();
	if(flag)
		{
		SetPortCountL(aStream.ReadUint32L());
		}
	RStringF protocol =iPool.OpenFStringL(GetTokenFromStreamL(aStream));
	CleanupClosePushL(protocol);
	SetProtocolL(protocol);
	CleanupStack::Pop();//protocol
	protocol.Close();
    SetPortL(port);
	SetFormatListL(GetTokenFromStreamL(aStream));	
	SetInfoL(GetTokenFromStreamL(aStream));
	SdpCodecTemplate<CSdpConnectionField>::InternalizeArrayL(*iConnectionFields,
															aStream);
	SdpCodecTemplate<CSdpBandwidthField>::InternalizeArrayL(*iBandwidthFields,
															aStream);
	if(aStream.ReadUint8L())
		{
		SetKey(CSdpKeyField::InternalizeL(aStream));
		}
	SdpCodecTemplate<CSdpAttributeField>::InternalizeArrayL(*iAttributeFields,
													aStream);
	SdpCodecTemplate<CSdpFmtAttributeField>::InternalizeArrayL(*iFmtAttrFields,
													aStream);
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseL (const TDesC8& aText)
	{
	iElementArray.Reset();
	iSdpCodecParseUtil = 
			CSdpCodecParseUtil::NewL(iPool,aText, KErrSdpCodecMediaField);
	ParseMediaL();
	if(iRecurse)
		{
		ParseAttributeFieldsL();
		ParseInformationL();
		ParseConnectionL();
		ParseBandwithL();
		ParseEncryptionKeyL();
		}
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseMediaL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseMediaL()
	{
	iElementArray = iSdpCodecParseUtil->FirstLineArrayElementL(iPool,
												SdpCodecStringConstants::EMedia,
												KErrSdpCodecMediaField);
	__ASSERT_ALWAYS (iElementArray.Count() >= 4,
					User::Leave(KErrSdpCodecMediaField));

	RStringF media = iPool.OpenFStringL(iElementArray[1]);
	CleanupClosePushL(media);
	SetMediaL(media);
	CleanupStack::Pop();//media
	media.Close();

	iProtocol = iPool.OpenFStringL(iElementArray[3]);

    TInt lineEndPosition = iElementArray[2].Locate('/');
	TUint port;
	if(lineEndPosition == KErrNotFound)
		{
		User::LeaveIfError(
		TLex8(iElementArray[2]).Val(port, EDecimal));
		SetPortL(port);
		}
	else
		{
		User::LeaveIfError(TLex8(
				(iElementArray[2]).Left(lineEndPosition)).Val(port, EDecimal));
		SetPortL(port);
		User::LeaveIfError(TLex8(
			  (iElementArray[2]).Mid(lineEndPosition +1)).Val(port, EDecimal));
		SetPortCountL(port);
		}
	__ASSERT_ALWAYS(SdpUtil::IsTokenCharWithOptionalSlash(iElementArray[3]),
					User::Leave(KErrArgument));
	
    //iProtocol = iPool.OpenFStringL(iElementArray[3]);

	TInt length = 0;
	TInt i;
	for(i=4;i<iElementArray.Count();i++)
		{
		length += iElementArray[i].Length();
		if(i + 1 <iElementArray.Count())
			{
			length += KSPStr().Length();
			}
		}
	HBufC8* value = HBufC8::NewLC(length);
	TPtr8 ptr(value->Des());
	for(i=4;i<iElementArray.Count();i++)
		{
		ptr.Append(iElementArray[i]);
		if(i + 1 <iElementArray.Count())
			{
			ptr.Append(KSPStr);
			}
		}
	SetFormatListL(*value);
	CleanupStack::Pop();//value
	delete value;
	iElementArray.Reset();
	if (iSdpCodecParseUtil->LineArray().Count() > 0)
	    {
	    iSdpCodecParseUtil->LineArray().Remove(0);
	    }
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseInformationL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseInformationL()
	{
	SetInfoL(iSdpCodecParseUtil->ParseInformationL(KErrSdpCodecMediaInfoField));
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseConnectionL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseConnectionL()
	{
	delete iConnectionFields;
	iConnectionFields = 0;
	iConnectionFields = iSdpCodecParseUtil->ParseConnectionFieldsL(
										KErrSdpCodecMediaConnectionField);
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseBandwithL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseBandwithL()
	{
	delete iBandwidthFields;
	iBandwidthFields = 0;
	iBandwidthFields = 
		iSdpCodecParseUtil->ParseBandwidthL(KErrSdpCodecMediaBandwidthField);
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseEncryptionKeyL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseEncryptionKeyL()
	{
	SetKey(iSdpCodecParseUtil->ParseEncryptionKeyL(KErrSdpCodecMediaKeyField));
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::ParseAttributeFieldsL
// ---------------------------------------------------------------------------
//
void CSdpMediaField::ParseAttributeFieldsL()
	{
	CSdpFmtAttributeField* fmtattributefield = 0;

    RArray<TPtrC8>& lineArray = iSdpCodecParseUtil->LineArray();
	for (TInt i=0; i<lineArray.Count(); i++)
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
								SdpCodecStringConstants::EAttribute,
								lineArray[i],
								KErrSdpCodecMediaAttributeField);
		if (iElementArray.Count())
			{
			CSdpAttributeField* attributefield = 
	            CSdpAttributeField::DecodeLC(lineArray[i]);		
		    
            if ( attributefield->Attribute() == 
                 iPool.StringF( SdpCodecStringConstants::EAttributeRtpmap,
                                SdpCodecStringConstants::Table ) ||
                 attributefield->Attribute() ==
                 iPool.StringF( SdpCodecStringConstants::EAttributeFmtp,
                                SdpCodecStringConstants::Table ) )
                {
                CleanupStack::PopAndDestroy(attributefield);
                fmtattributefield = 
                    CSdpFmtAttributeField::DecodeLC(lineArray[i]);
				iFmtAttrFields->AppendL(fmtattributefield);
				CleanupStack::Pop(fmtattributefield);
                }
            else
                {
                iAttributeFields->AppendL(attributefield);
			    CleanupStack::Pop(attributefield);
			    if (fmtattributefield)
			    	{
			    	attributefield->AssignTo(*fmtattributefield);
			    	}
                }
                   
			iElementArray.Reset();
			lineArray.Remove(i);
			if (i < lineArray.Count())
			    {
			    i--;
			    }
			}
		}
	}

// ---------------------------------------------------------------------------
// CSdpMediaField::GetTokenFromStreamL
// ---------------------------------------------------------------------------
//
TDesC8& CSdpMediaField::GetTokenFromStreamL(RReadStream& aStream)
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

// ---------------------------------------------------------------------------
// CSdpMediaField::RemoveFormatL
// ---------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::RemoveFormatL(const TDesC8& aFormat)
    {
    if ( !iFormatList || iFormatList->Des().Length() == 0 )
        {
        User::Leave( KErrSdpCodecMediaField );
        }
    TLex8 lexer( *iFormatList );
    TBool tokenRemoved( EFalse );
    while ( !tokenRemoved && !lexer.Eos() )
        {
        lexer.SkipSpaceAndMark();
        lexer.SkipCharacters();
        if ( lexer.TokenLength() > 0 )
            {
             if ( aFormat.CompareF(lexer.MarkedToken()) == 0)
                {                    
                RArray<TInt> removedObjs;           
                CleanupClosePushL( removedObjs );
                for ( TInt i( 0 ); i < iFmtAttrFields->Count(); i++ )
                    {
					if (aFormat.CompareF((*iFmtAttrFields)[i]->Format()) == 0)
                        {
                        User::LeaveIfError( removedObjs.Append(i) );                        
                        }                        
                    }
                // Remove all format attributes of type format
                TInt removedCount( 0 );
                while ( removedObjs.Count() > 0 )
                    {
                    CSdpFmtAttributeField* obj = 
                        (*iFmtAttrFields)[removedObjs[0] - removedCount];
                    delete obj;
					obj = 0;
                    iFmtAttrFields->Remove( removedObjs[0] - removedCount);
                    removedObjs.Remove(0);
                    removedCount++;
                    }
                CleanupStack::PopAndDestroy();  // removedObjs

                // Remove format from format list
                iFormatList->Des().Delete( lexer.MarkedOffset(), 
                                        lexer.Offset() - lexer.MarkedOffset());
                iFormatList->Des().TrimAll();
                tokenRemoved = ETrue;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CSdpMediaField::KeepFormatL
// ---------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::KeepFormatL(const TDesC8& aFormat)
    {
    if ( !iFormatList )
        {
        User::Leave( KErrSdpCodecMediaField );
        }
    // Empty set will continue to be empty
    if ( iFormatList->Des().Length() > 0 )
        {                
        HBufC8* formatList = aFormat.AllocLC();       
        // Mark all format attributes not aFormat so that they will be deleted
        RArray<TInt> removedObjs;  
        CleanupClosePushL( removedObjs );         
        for ( TInt i( 0 ); i < iFmtAttrFields->Count(); i++ )
            {
			if (aFormat.CompareF((*iFmtAttrFields)[i]->Format()) != 0)
                {
                User::LeaveIfError( removedObjs.Append( i ) );                        
                }                                 
            }
        // Delete attribute fields
        TInt removedCount( 0 );
        while ( removedObjs.Count() > 0 )
            {
            CSdpFmtAttributeField* obj = 
                (*iFmtAttrFields)[removedObjs[0] - removedCount];
            delete obj;
            iFmtAttrFields->Remove( removedObjs[0] - removedCount );
            removedObjs.Remove( 0 );
            removedCount++;
            }
        CleanupStack::PopAndDestroy();  // removedObjs
        // Change format list
        delete iFormatList;
        iFormatList = formatList;
        CleanupStack::Pop();    // formatList
        }
    }

// ---------------------------------------------------------------------------
// CSdpMediaField::RejectMedia
// ---------------------------------------------------------------------------
//
EXPORT_C void CSdpMediaField::RejectMedia()
    {
    iPort = 0;
    iPortCount = 0;

    TLex8 lexer( *iFormatList );
    lexer.SkipSpaceAndMark();
    lexer.SkipCharacters();
    if ( lexer.TokenLength() > 0 )
        {
        TRAPD(err, KeepFormatL( lexer.MarkedToken() ));
        err++; // Nothing to do, if error happens. Silence compiler warning.
        }
    }

