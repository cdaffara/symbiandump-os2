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
// Name          : SdpCodecParseUtil.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include "sdpcodecparseutil.h"
#include "sdpcodecstringconstants.h"
#include "_sdpdefs.h"
#include <s32strm.h>
#include "sdputil.h"
#include "sdptimefield.h"
#include "sdpbandwidthfield.h"
#include "sdpattributefield.h"
#include "sdpmediafield.h"
#include "sdpconnectionfield.h"
#include "sdpkeyfield.h"
#include "sdporiginfield.h"
#include "sdprepeatfield.h"
#include "sdpcodecerr.h"
#include "sdpcodecconstants.h"
#include "uri8.h"
#include "uriutilscommon.h"
#include "badesca.h"

// ----------------------------------------------------------------------------
// CSdpCodecParseUtil::NewL
// ----------------------------------------------------------------------------
//
CSdpCodecParseUtil* 
CSdpCodecParseUtil::NewL(RStringPool aPool, const TDesC8& aValue,TInt aErrCode)
	{
	CSdpCodecParseUtil* obj = CSdpCodecParseUtil::NewLC(aPool, aValue,aErrCode);
	CleanupStack::Pop();//obj
	return obj;
	}

// ----------------------------------------------------------------------------
// CSdpCodecParseUtil::NewLC
// ----------------------------------------------------------------------------
//
CSdpCodecParseUtil* 
CSdpCodecParseUtil::NewLC(RStringPool aPool, const TDesC8& aValue,TInt aErrCode)
	{
	CSdpCodecParseUtil* obj = new(ELeave)CSdpCodecParseUtil;
	CleanupStack::PushL(obj);
	obj->ConstructL(aPool, aValue,aErrCode);
	return obj;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::FieldLineValueL
// ---------------------------------------------------------------------------
//
TPtrC8 CSdpCodecParseUtil::FieldLineValueL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrArgument));
	TPtrC8 lineStartBuf(aValue.Mid(2));
	TInt lineEndPosition = lineStartBuf.Locate('\r');
	if(lineEndPosition == KErrNotFound)
		{
		lineEndPosition = lineStartBuf.Locate('\n');
		}
	return lineStartBuf.Left(lineEndPosition);
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::CheckFieldNameL
// ---------------------------------------------------------------------------
//
RArray<TPtrC8> CSdpCodecParseUtil::CheckFieldNameL(RStringPool aPool,
												  TInt aIndex, 
												  const TDesC8& aValue,
												  TInt aErrCode)
	{
	if(!aValue.Length())
			{
			User::Leave(KErrArgument);
			}
	RArray<TPtrC8> elementArray = SdpUtil::GetElementsFromLineL(aValue, 
																aErrCode);
	CleanupClosePushL(elementArray);
	RStringF name = aPool.OpenFStringL(elementArray[0]);
	CleanupClosePushL(name);
	if(name!=(aPool.StringF(aIndex, SdpCodecStringConstants::Table)))
		{
		User::Leave(KErrArgument);//parsererror
		}
	CleanupStack::Pop(2);//name, elementArray
	name.Close();
	return elementArray;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::CheckOptionalFieldNameL
// ---------------------------------------------------------------------------
//
RArray<TPtrC8> CSdpCodecParseUtil::CheckOptionalFieldNameL(RStringPool aPool, 
												 TInt aIndex, 
												 const TDesC8& aValue,
												  TInt aErrCode)
	{
	RArray<TPtrC8> elementArray;
	elementArray.Reset();
	if(aValue.Length())
		{
		elementArray = SdpUtil::GetElementsFromLineL(aValue, aErrCode);
	    CleanupClosePushL(elementArray);
		RStringF name = aPool.OpenFStringL(elementArray[0]);
		CleanupClosePushL(name);
		if(name!=(aPool.StringF(aIndex, SdpCodecStringConstants::Table)))
			{
			elementArray.Reset();
			}
		CleanupStack::Pop(2);//name, elementArray
		name.Close();
		}
	return elementArray;
	}

// ----------------------------------------------------------------------------
// CSdpCodecParseUtil::CSdpCodecParseUtil
// ----------------------------------------------------------------------------
//
CSdpCodecParseUtil::CSdpCodecParseUtil()
	{

	}

// ----------------------------------------------------------------------------
// CSdpCodecParseUtil::~CSdpCodecParseUtil
// ----------------------------------------------------------------------------
//
CSdpCodecParseUtil::~CSdpCodecParseUtil()
	{
	iLineArray.Reset();
	iElementArray.Reset();
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

	if(iConnectionFields)
		{
		iConnectionFields->ResetAndDestroy();
		delete iConnectionFields;
		}

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
	}

// ----------------------------------------------------------------------------
// CSdpCodecParseUtil::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpCodecParseUtil::ConstructL(RStringPool aPool, const TDesC8& aValue,
									TInt aErrCode)
	{
	iPool = aPool;
	iData.Set(aValue);
	iToken = KNullDesC8().AllocL();
	iEmailFields = new(ELeave)CDesC8ArraySeg(1);
	iPhoneFields = new(ELeave)CDesC8ArraySeg(1);
	iBandwidthFields = new(ELeave)RPointerArray<CSdpBandwidthField>;
	iTimeFields = new(ELeave)RPointerArray<CSdpTimeField>;
	iAttributeFields = new(ELeave)RPointerArray<CSdpAttributeField>;
	iMediaFields = new(ELeave)RPointerArray<CSdpMediaField>;
	iConnectionFields = new(ELeave)RPointerArray<CSdpConnectionField>;
	iLineArray = SdpUtil::DivideToLinesL(aValue,aErrCode);
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseSessionVersionL
// ---------------------------------------------------------------------------
//
TUint CSdpCodecParseUtil::ParseSessionVersionL(TInt aErrCode)
	{
	TUint sdpVersion = 0;
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckFieldNameL(iPool,
											SdpCodecStringConstants::EVersion,
											iLineArray[0],aErrCode);
		if(iElementArray.Count() > 1)
			{
			TLex8 lex(iElementArray[1]);
			if(lex.Val(sdpVersion) == KErrNone)
				{
				iLineArray.Remove(0);
				iElementArray.Reset();
				}
			else
				{
				User::Leave(KErrSdpCodecVersionField);
				}
			}
		}
	else
		{
		User::Leave(KErrSdpCodecVersionField);
		}
	return sdpVersion;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseSessionOwnerL
// ---------------------------------------------------------------------------
//
CSdpOriginField* CSdpCodecParseUtil::ParseSessionOwnerL(TInt aErrCode)
	{
	CSdpOriginField* sdpOriginField = NULL;
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckFieldNameL(iPool,
											SdpCodecStringConstants::EOrigin,
											iLineArray[0],aErrCode);
		sdpOriginField = 
					CSdpOriginField::DecodeL(iLineArray[0]);
		iElementArray.Reset();
		iLineArray.Remove(0);
		}
	else
		{
		User::Leave(KErrSdpCodecOriginField);
		}
	return sdpOriginField;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseSessionNameL
// ---------------------------------------------------------------------------
//
const TDesC8& CSdpCodecParseUtil::ParseSessionNameL(TInt aErrCode)
	{
	delete iToken;
	iToken = 0;
	iToken = KNullDesC8().AllocL();
	if(iLineArray.Count())
		{
		if(iLineArray[0].Length() < 6 && 
			(iLineArray[0].CompareF(_L8("s= \r\n")) == 0 ||
			iLineArray[0].CompareF(_L8("s= \n")) == 0 ))
			{
			iLineArray.Remove(0);
			delete iToken;
			iToken = 0;
			iToken = _L8(" ").AllocL();
			return	*iToken;
			}
		iElementArray = CSdpCodecParseUtil::CheckFieldNameL(iPool,
											SdpCodecStringConstants::ESession,
											iLineArray[0],aErrCode);
		__ASSERT_ALWAYS (iElementArray.Count() > 1, 
		User::Leave(KErrSdpCodecSessionField));
		delete iToken;
		iToken = 0;
		iToken = (CSdpCodecParseUtil::FieldLineValueL(iLineArray[0])).AllocL();
		iElementArray.Reset();
		iLineArray.Remove(0);
		}
	else
		{
		User::Leave(KErrSdpCodecSessionField);
		}
	return *iToken;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseInformationL
// ---------------------------------------------------------------------------
//
const TDesC8& CSdpCodecParseUtil::ParseInformationL(TInt aErrCode)
	{
	delete iToken;
	iToken = 0;
	iToken = KNullDesC8().AllocL();
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
												SdpCodecStringConstants::EInfo,
												iLineArray[0],aErrCode);

		if(iElementArray.Count())
			{
			__ASSERT_ALWAYS (iElementArray.Count() > 1, 
			User::Leave(KErrSdpCodecInfoField));
			delete iToken;
			iToken = 0;
			iToken = 
				(CSdpCodecParseUtil::FieldLineValueL(iLineArray[0])).AllocL();
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		}
	return *iToken;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseUriL
// ---------------------------------------------------------------------------
//
CUri8* CSdpCodecParseUtil::ParseUriL(TInt aErrCode)
	{
	CUri8* uri = 0;
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
												SdpCodecStringConstants::EUri,
												iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{
			__ASSERT_ALWAYS (iElementArray.Count() == 2, 
				User::Leave(KErrSdpCodecUriField));
			TUriParser8 parser;
			User::LeaveIfError(parser.Parse(iElementArray[1]));
			uri = CUri8::NewL(parser);
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		}
	return uri;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseEmailL
// ---------------------------------------------------------------------------
//
CDesC8ArraySeg* CSdpCodecParseUtil::ParseEmailL(TInt aErrCode)
	{
	TBool endloop = EFalse;
	while (iLineArray.Count() && !endloop)
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
												SdpCodecStringConstants::EEmail,
												iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{
			__ASSERT_ALWAYS (iElementArray.Count() > 1, 
				User::Leave(KErrSdpCodecEmailField));
			iEmailFields->AppendL(
				CSdpCodecParseUtil::FieldLineValueL(iLineArray[0]));
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		else
			{
			endloop = ETrue;
			}
		}
	CDesC8ArraySeg* array = iEmailFields;
	iEmailFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParsePhoneL
// ---------------------------------------------------------------------------
//
CDesC8ArraySeg* CSdpCodecParseUtil::ParsePhoneL(TInt aErrCode)
	{
	TBool endloop = EFalse;
	while (iLineArray.Count() && !endloop)
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
												SdpCodecStringConstants::EPhone,
												iLineArray[0],aErrCode);
		if(iElementArray.Count() != 0)
			{
			__ASSERT_ALWAYS (iElementArray.Count() > 1, 
				User::Leave(KErrSdpCodecPhoneField));
			iPhoneFields->AppendL(
				CSdpCodecParseUtil::FieldLineValueL(iLineArray[0]));
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		else
			{
			endloop = ETrue;
			}
		}
	CDesC8ArraySeg* array = iPhoneFields;
	iPhoneFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseConnectionL
// ---------------------------------------------------------------------------
//
CSdpConnectionField* CSdpCodecParseUtil::ParseConnectionL(TInt aErrCode)
	{
	CSdpConnectionField* sdpConnectionField = NULL;
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
										SdpCodecStringConstants::EConnection,
										iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{
			sdpConnectionField =  
				CSdpConnectionField::DecodeL(iLineArray[0]);
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		}
	return sdpConnectionField;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseBandwidthL
// ---------------------------------------------------------------------------
//
RPointerArray<CSdpBandwidthField>* 
CSdpCodecParseUtil::ParseBandwidthL(TInt aErrCode)
	{
	TBool endloop = EFalse;
	CSdpBandwidthField* bandwidth = NULL;
	while (iLineArray.Count() && !endloop)
		{
		iElementArray = 
		    CSdpCodecParseUtil::CheckOptionalFieldNameL(
		        iPool,SdpCodecStringConstants::EBandwidth,
		        iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{			
			bandwidth = CSdpBandwidthField::DecodeLC(iLineArray[0]);
			iBandwidthFields->AppendL(bandwidth);
			CleanupStack::Pop(bandwidth);
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		else
			{
			endloop = ETrue;
			}
		}
	RPointerArray<CSdpBandwidthField>* array = iBandwidthFields;
	iBandwidthFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseConnectionFieldsL
// ---------------------------------------------------------------------------
//
RPointerArray<CSdpConnectionField>* 
CSdpCodecParseUtil::ParseConnectionFieldsL(TInt aErrCode)
	{
	TBool endloop = EFalse;
	CSdpConnectionField* connection = NULL;
	while(iLineArray.Count() && !endloop)
		{
		iElementArray = 
		    CSdpCodecParseUtil::CheckOptionalFieldNameL(
		        iPool,SdpCodecStringConstants::EConnection,
		        iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{			
			connection = CSdpConnectionField::DecodeLC(iLineArray[0]);
			iConnectionFields->AppendL(connection);
			CleanupStack::Pop(connection);
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		else
			{
			endloop = ETrue;
			}
		}
	RPointerArray<CSdpConnectionField>* array = iConnectionFields;
	iConnectionFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseTimeFieldL
// ---------------------------------------------------------------------------
//
RPointerArray<CSdpTimeField>* 
CSdpCodecParseUtil::ParseTimeFieldL(TInt aErrCode)
	{
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckFieldNameL(iPool,
											SdpCodecStringConstants::ETime,
											iLineArray[0],aErrCode);
		iElementArray.Reset();
		CSdpTimeField* timefield = NULL;
		CSdpRepeatField* repeatfield = NULL;
		TBool endloop = EFalse;
		while(iLineArray.Count() && !endloop)
			{
			iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
												SdpCodecStringConstants::ETime,
												iLineArray[0],aErrCode);
			if(iElementArray.Count())
				{
				timefield = CSdpTimeField::DecodeLC(iLineArray[0]);
				User::LeaveIfError(iTimeFields->Append(timefield));
				CleanupStack::Pop();//timefield
				iElementArray.Reset();
				iLineArray.Remove(0);
				TBool repeatEnd = EFalse;
				while(iLineArray.Count() && !repeatEnd)
					{
					iElementArray = 
						CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
											SdpCodecStringConstants::ERepeat,
											iLineArray[0],aErrCode);
					if(iElementArray.Count())
						{
						repeatfield =
							CSdpRepeatField::DecodeLC(iLineArray[0]);
						User::LeaveIfError((timefield->RepeatFields()).Append(
																  repeatfield));
						CleanupStack::Pop();//repeatfield
						repeatfield = 0;
						iElementArray.Reset();
						iLineArray.Remove(0);
						}
					else
						{
						repeatEnd = ETrue;
						}
					}
				timefield = 0;
				}
			else
				{
				endloop = ETrue;
				}
			}
		}
	RPointerArray<CSdpTimeField>* array = iTimeFields;
	iTimeFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseZoneAdjustmentL
// ---------------------------------------------------------------------------
//
const TDesC8& CSdpCodecParseUtil::ParseZoneAdjustmentL(TInt aErrCode)
	{
	delete iToken;
	iToken = 0;
	iToken = KNullDesC8().AllocL();
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
											SdpCodecStringConstants::EZone,
											iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{
			__ASSERT_ALWAYS (iElementArray.Count() > 1, 
				User::Leave(KErrSdpCodecZoneField));
			delete iToken;
			iToken = 0;
			iToken = 
				(CSdpCodecParseUtil::FieldLineValueL(iLineArray[0])).AllocL();
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		}
	return *iToken;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseEncryptionKeyL
// ---------------------------------------------------------------------------
//
CSdpKeyField* CSdpCodecParseUtil::ParseEncryptionKeyL(TInt aErrCode)
	{
	CSdpKeyField* sdpKeyField = NULL;
	if(iLineArray.Count())
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
												SdpCodecStringConstants::EKey,
												iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{
			__ASSERT_ALWAYS (iElementArray.Count() > 1, 
				User::Leave(KErrSdpCodecKeyField));
			sdpKeyField = CSdpKeyField::DecodeL(iLineArray[0]);
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		}
	return sdpKeyField;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseAttributeFieldL
// ---------------------------------------------------------------------------
//
RPointerArray<CSdpAttributeField>* 
CSdpCodecParseUtil::ParseAttributeFieldL(TInt aErrCode)
	{
	TBool endloop = EFalse;
	CSdpAttributeField* attributefield = NULL;
	while(iLineArray.Count() && !endloop)
		{
		iElementArray = CSdpCodecParseUtil::CheckOptionalFieldNameL(iPool,
										SdpCodecStringConstants::EAttribute,
										iLineArray[0],aErrCode);
		if(iElementArray.Count())
			{
			attributefield = 
						CSdpAttributeField::DecodeLC(iLineArray[0]);
			User::LeaveIfError(iAttributeFields->Append(attributefield));
			CleanupStack::Pop();//attributefield
			attributefield = NULL;
			iElementArray.Reset();
			iLineArray.Remove(0);
			}
		else
			{
			endloop = ETrue;
			}
		}
	RPointerArray<CSdpAttributeField>* array = iAttributeFields;
	iAttributeFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::ParseMediaLevelL
// ---------------------------------------------------------------------------
//
RPointerArray<CSdpMediaField>* 
CSdpCodecParseUtil::ParseMediaLevelL()
	{
	RStringF headername = iPool.StringF( SdpCodecStringConstants::EMedia, 
                                         SdpCodecStringConstants::Table );
	TInt linestartPosition = 0;
	TInt lineEndPosition = 0;
	CSdpMediaField* mediafield = NULL;
	TBool endloop = EFalse;
	while(iData.Length() > 0 && !endloop)
		{
		linestartPosition = iData.FindF(headername.DesC());
		if(linestartPosition != KErrNotFound)
			{
			TPtrC8 firstline(iData.Mid(linestartPosition));
			TPtrC8 templine(iData.Mid(linestartPosition + 1));
			HBufC8* searchString = 
			 	HBufC8::NewLC(KCRLFStr().Length() + 
			 	headername.DesC().Length());
			TPtr8 strPtr(searchString->Des());
			strPtr.Append(KCRLFStr);
			strPtr.Append(headername.DesC());
			lineEndPosition = templine.FindF(strPtr);
			CleanupStack::PopAndDestroy(searchString);
			if(lineEndPosition == KErrNotFound)
				{
				mediafield = 
				CSdpMediaField::DecodeLC(iData.Mid(linestartPosition));
				User::LeaveIfError(iMediaFields->Append(mediafield));
				CleanupStack::Pop();//mediafield
				mediafield = 0;
				iElementArray.Reset();
				iLineArray.Reset();
				endloop = ETrue;
				}
			else 
				{
				mediafield = 
				CSdpMediaField::DecodeLC(firstline.Left(
									lineEndPosition+1+KCRLFStr().Length()));
				User::LeaveIfError(iMediaFields->Append(mediafield));
				CleanupStack::Pop();//mediafield
				mediafield = 0;
				iData.Set(firstline.Mid(lineEndPosition+1));
				}
			}
		else
			{
			endloop = ETrue;
			}
		}
	RPointerArray<CSdpMediaField>* array = iMediaFields;
	iMediaFields = 0;
	return array;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::FirstLineArrayElementL
// ---------------------------------------------------------------------------
//
RArray<TPtrC8> CSdpCodecParseUtil::FirstLineArrayElementL(RStringPool aPool,
												          TInt aIndex,
														  TInt aErrCode)
	{
	if(!iLineArray.Count())
			{
			User::Leave(aErrCode);
			}
	RArray<TPtrC8> elementArray = 
		SdpUtil::GetElementsFromLineL(iLineArray[0],aErrCode);
	CleanupClosePushL(elementArray);
	RStringF name = aPool.OpenFStringL(elementArray[0]);
	CleanupClosePushL(name);
	if(name!=(aPool.StringF(aIndex, SdpCodecStringConstants::Table)))
		{
		User::Leave(aErrCode);//parsererror
		}
	CleanupStack::Pop(2);//name, elementArray
	name.Close();
	return elementArray;
	}

// ---------------------------------------------------------------------------
// CSdpCodecParseUtil::LineArray
// ---------------------------------------------------------------------------
//
RArray<TPtrC8>& CSdpCodecParseUtil::LineArray()
	{
	return iLineArray;
	}
