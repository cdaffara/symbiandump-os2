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
#include "omxilvideoportimpl.h"
#include <openmax/il/common/omxilvideoport.h>
#include <openmax/il/common/omxilutil.h>

COmxILVideoPortImpl* COmxILVideoPortImpl::NewL(const RArray<OMX_VIDEO_CODINGTYPE>& aSupportedVideoFormats,
                                                const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats,
                                                const TOmxILCommonPortData& aCommonPortData)
	{
	COmxILVideoPortImpl* self = new (ELeave) COmxILVideoPortImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aSupportedVideoFormats, aSupportedColorFormats, aCommonPortData);
	CleanupStack::Pop();
	return self;
	}
	
void COmxILVideoPortImpl::ConstructL(const RArray<OMX_VIDEO_CODINGTYPE>& aSupportedVideoFormats,
                                    const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats,
                                    const TOmxILCommonPortData& aCommonPortData)
	{
    
    TUint count = aSupportedVideoFormats.Count();
    for (TInt i = 0; i < count; i++)
        {
        iSupportedVideoFormats.AppendL(aSupportedVideoFormats[i]);
        }    
    
    count = aSupportedColorFormats.Count();
    for (TInt i = 0; i < count; i++)
        {
        iSupportedColorFormats.AppendL(aSupportedColorFormats[i]);
        }    
    
	TInt numVideoFormats = iSupportedVideoFormats.Count();
	TInt numColorFormats = iSupportedColorFormats.Count();
	iParamVideoPortFormat.nSize = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
	iParamVideoPortFormat.nVersion = aCommonPortData.iOmxVersion;
	iParamVideoPortFormat.nPortIndex = aCommonPortData.iPortIndex;
	iParamVideoPortFormat.nIndex = numVideoFormats ? numVideoFormats - 1 : 0;
	iParamVideoPortFormat.eCompressionFormat = numVideoFormats ? iSupportedVideoFormats[0] : OMX_VIDEO_CodingUnused;
	iParamVideoPortFormat.eColorFormat = numColorFormats ? iSupportedColorFormats[0] : OMX_COLOR_FormatUnused;
	iParamVideoPortFormat.xFramerate = 0;//aCommonPortData.xFramerate???;	
	}
	
COmxILVideoPortImpl::COmxILVideoPortImpl()
	{
	DEBUG_PRINTF(_L8("COmxILVideoPortImpl::COmxILVideoPortImpl"));
	}

COmxILVideoPortImpl::~COmxILVideoPortImpl()
	{
	DEBUG_PRINTF(_L8("COmxILVideoPortImpl::~COmxILVideoPortImpl"));
	iSupportedVideoFormats.Close();
	iSupportedColorFormats.Close();
	}

OMX_ERRORTYPE COmxILVideoPortImpl::GetParameter(OMX_INDEXTYPE aParamIndex, TAny* apComponentParameterStructure) const
	{
	DEBUG_PRINTF(_L8("COmxILVideoPortImpl::GetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	
	switch(aParamIndex)
		{
		case OMX_IndexParamVideoPortFormat:
			{
			if (OMX_ErrorNone != (omxRetValue =
							  TOmxILUtil::CheckOmxStructSizeAndVersion(
								  const_cast<OMX_PTR>(apComponentParameterStructure),
								  sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE))))
				{
				return omxRetValue;
				}
				
			OMX_VIDEO_PARAM_PORTFORMATTYPE* videoPortDefinition = static_cast<OMX_VIDEO_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);
			
			if(OMX_VIDEO_CodingUnused == iParamVideoPortFormat.eCompressionFormat)
				{							
				if (videoPortDefinition->nIndex >= iSupportedColorFormats.Count())
					{
					return OMX_ErrorNoMore;
					}
				videoPortDefinition->eColorFormat = iSupportedColorFormats[videoPortDefinition->nIndex];	
				}
			else
				{
				if (videoPortDefinition->nIndex >= iSupportedVideoFormats.Count())
					{
					return OMX_ErrorNoMore;
					}
				videoPortDefinition->eCompressionFormat = iSupportedVideoFormats[videoPortDefinition->nIndex];
				}
			videoPortDefinition->xFramerate = iParamVideoPortFormat.xFramerate;	
			break;
			}
		default:
			__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILVideoPortPanicCategory, 1));
		};

	return OMX_ErrorNone;
	}

OMX_ERRORTYPE COmxILVideoPortImpl::SetParameter(OMX_INDEXTYPE aParamIndex, const TAny* apComponentParameterStructure, TBool& aUpdateProcessingFunction)
	{
	DEBUG_PRINTF(_L8("COmxILVideoPortImpl::SetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
		case OMX_IndexParamVideoPortFormat:
			{
			if (OMX_ErrorNone != (omxRetValue =
							  TOmxILUtil::CheckOmxStructSizeAndVersion(
								  const_cast<OMX_PTR>(apComponentParameterStructure),
								  sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE))))
				{
				return omxRetValue;
				}

			const OMX_VIDEO_PARAM_PORTFORMATTYPE *componentParameterStructure = static_cast<const OMX_VIDEO_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);

			if (!UpdateColorFormat(iParamVideoPortFormat.eColorFormat, componentParameterStructure->eColorFormat, aUpdateProcessingFunction))
				{
				return OMX_ErrorUnsupportedSetting;
				}

			if (!UpdateCodingType(iParamVideoPortFormat.eCompressionFormat, componentParameterStructure->eCompressionFormat, aUpdateProcessingFunction))
				{
				return OMX_ErrorUnsupportedSetting;
				}

			if(iParamVideoPortFormat.eCompressionFormat == OMX_VIDEO_CodingUnused)
				{
				if(iParamVideoPortFormat.xFramerate != componentParameterStructure->xFramerate)
					{
					iParamVideoPortFormat.xFramerate = componentParameterStructure->xFramerate;
					aUpdateProcessingFunction = ETrue;
					}
				}
			break;
			}
		default:
			__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILVideoPortPanicCategory, 1));
		};
	return OMX_ErrorNone;
	}

TBool COmxILVideoPortImpl::UpdateColorFormat(OMX_COLOR_FORMATTYPE& aOldColor, OMX_COLOR_FORMATTYPE aNewColor, TBool& aUpdated)
	{
	if (aNewColor != aOldColor)
		{
		if(iSupportedColorFormats.Find(aNewColor) == KErrNotFound)
			{
			return EFalse;
			}
		aOldColor = aNewColor;
		aUpdated = ETrue;
		}

	return ETrue;
	}

TBool COmxILVideoPortImpl::UpdateCodingType(OMX_VIDEO_CODINGTYPE& aOldCodingType, OMX_VIDEO_CODINGTYPE aNewCodingType, TBool& aUpdated)
	{
	if (aNewCodingType != aOldCodingType)
		{
		if(iSupportedVideoFormats.Find(aNewCodingType) == KErrNotFound)
			{
			return EFalse;
			}
		aOldCodingType = aNewCodingType;
		aUpdated = ETrue;
		}

	return ETrue;
	}

RArray<OMX_VIDEO_CODINGTYPE>& COmxILVideoPortImpl::GetSupportedVideoFormats()
	{
	return iSupportedVideoFormats;
	}
	
RArray<OMX_COLOR_FORMATTYPE>& COmxILVideoPortImpl::GetSupportedColorFormats()
	{
	return iSupportedColorFormats;
	}

OMX_VIDEO_PARAM_PORTFORMATTYPE& COmxILVideoPortImpl::GetParamVideoPortFormat()
    {
    return iParamVideoPortFormat;
    }
