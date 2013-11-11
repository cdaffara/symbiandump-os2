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
//


/**
 @file
 @internalComponent
*/

#include "log.h"
#include <openmax/il/common/omxilaudioport.h>
#include "omxilaudioportimpl.h"
#include <openmax/il/common/omxilutil.h>

COmxILAudioPortImpl* COmxILAudioPortImpl::NewL(const RArray<OMX_AUDIO_CODINGTYPE>& aSupportedAudioFormats,
											const TOmxILCommonPortData& aCommonPortData)
	{
	COmxILAudioPortImpl* self = new(ELeave) COmxILAudioPortImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aSupportedAudioFormats, aCommonPortData);
	CleanupStack::Pop();
	return self;
	}

void COmxILAudioPortImpl::ConstructL(const RArray<OMX_AUDIO_CODINGTYPE>& aSupportedAudioFormats,
									const TOmxILCommonPortData& aCommonPortData)
	{
	iParamAudioPortFormat.nSize		 = sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE);
	iParamAudioPortFormat.nVersion	 = aCommonPortData.iOmxVersion;
	iParamAudioPortFormat.nPortIndex = aCommonPortData.iPortIndex;
	iParamAudioPortFormat.nIndex	 = 0;
	
	TUint count = aSupportedAudioFormats.Count();
	for (TInt i = 0; i < count; i++)
		{
		iSupportedAudioFormats.AppendL(aSupportedAudioFormats[i]);
		}
	iParamAudioPortFormat.eEncoding	 = count ? iSupportedAudioFormats[0] : OMX_AUDIO_CodingUnused;
	}
	
COmxILAudioPortImpl::~COmxILAudioPortImpl()
	{
    DEBUG_PRINTF(_L8("COmxILAudioPortImpl::~COmxILAudioPortImpl"));
    iSupportedAudioFormats.Close();
	}

COmxILAudioPortImpl::COmxILAudioPortImpl()
	{
    DEBUG_PRINTF(_L8("COmxILAudioPortImpl::COmxILAudioPortImpl"));
	}

OMX_ERRORTYPE
COmxILAudioPortImpl::GetParameter(OMX_INDEXTYPE aParamIndex,
							  TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILAudioPortImpl::GetParameter"));

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
	case OMX_IndexParamAudioPortFormat:
		{
		if (OMX_ErrorNone != (omxRetValue =
							  TOmxILUtil::CheckOmxStructSizeAndVersion(
								  apComponentParameterStructure,
								  sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE))))
			{
			return omxRetValue;
			}

		OMX_AUDIO_PARAM_PORTFORMATTYPE* pParamAudioPortFormat
			= static_cast<OMX_AUDIO_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);

		if (pParamAudioPortFormat->nIndex >=
			iSupportedAudioFormats.Count())
			{
			return OMX_ErrorNoMore;
			}

		pParamAudioPortFormat->eEncoding =
			iSupportedAudioFormats[pParamAudioPortFormat->nIndex];
		}
		break;

	default:
		__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILAudioPortPanicCategory, 1));

		};
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE
COmxILAudioPortImpl::SetParameter(OMX_INDEXTYPE aParamIndex,
							  const TAny* apComponentParameterStructure,
							  TBool& aUpdateProcessingFunction)
	{
    DEBUG_PRINTF(_L8("COmxILAudioPortImpl::SetParameter"));
	aUpdateProcessingFunction = EFalse;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
	case OMX_IndexParamAudioPortFormat:
		{
		if (OMX_ErrorNone != (omxRetValue =
							  TOmxILUtil::CheckOmxStructSizeAndVersion(
								  const_cast<OMX_PTR>(apComponentParameterStructure),
								  sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE))))
			{
			return omxRetValue;
			}

		const OMX_AUDIO_PARAM_PORTFORMATTYPE* pParamAudioPortFormat
			= static_cast<const OMX_AUDIO_PARAM_PORTFORMATTYPE*>(
				apComponentParameterStructure);

		// Check in case the specified format is not actually supported by this
		// port
		OMX_AUDIO_CODINGTYPE newDefaultCodingType =
			pParamAudioPortFormat->eEncoding;

		if (newDefaultCodingType > OMX_AUDIO_CodingMax)
			{
			return OMX_ErrorBadParameter;
			}

		if (KErrNotFound == iSupportedAudioFormats.Find(newDefaultCodingType))
			{
			return OMX_ErrorUnsupportedSetting;
			}

		// Set the new default format, but check first that we are actually
		// changing something...
		if (iParamAudioPortFormat.eEncoding != newDefaultCodingType)
			{
			iParamAudioPortFormat.eEncoding = newDefaultCodingType;
			// This is an indication to the PortManager that the processing
			// function needs to get updated
			//
			aUpdateProcessingFunction = ETrue;
			}
		}
		break;

	default:
		__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILAudioPortPanicCategory, 1));
		};

	return OMX_ErrorNone;
	}
