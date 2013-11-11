// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <openmax/il/common/omxilimageport.h>
#include <openmax/il/common/omxilutil.h>
#include "log.h"
#include "omxilimageportimpl.h"

COmxILImagePortImpl* COmxILImagePortImpl::NewL(const TOmxILCommonPortData& aCommonPortData,
											const RArray<OMX_IMAGE_CODINGTYPE>& aSupportedImageFormats,
											const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats)
	{
	COmxILImagePortImpl* self = new(ELeave) COmxILImagePortImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aCommonPortData, aSupportedImageFormats, aSupportedColorFormats);
	CleanupStack::Pop();
	return self;		
	}

COmxILImagePortImpl::COmxILImagePortImpl()
	{
	DEBUG_PRINTF(_L8("COmxILImagePortImpl::COmxILImagePortImpl"));
	}

void COmxILImagePortImpl::ConstructL(const TOmxILCommonPortData& aCommonPortData,
									const RArray<OMX_IMAGE_CODINGTYPE>& aSupportedImageFormats,
									const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats)
	{
	DEBUG_PRINTF(_L8("COmxILImagePortImpl::ConstructL"));
	TUint count = aSupportedImageFormats.Count();
	for (TInt i = 0; i < count; ++i)
		{
		iSupportedImageFormats.AppendL(aSupportedImageFormats[i]);
		}
		
	count = aSupportedColorFormats.Count();
	for (TInt i = 0; i < count; ++i)
		{
		iSupportedColorFormats.AppendL(aSupportedColorFormats[i]);
		}
		
	TInt numImageFormats = iSupportedImageFormats.Count();
	TInt numColorFormats = iSupportedColorFormats.Count();
	iParamImagePortFormat.nSize = sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE);
	iParamImagePortFormat.nVersion = aCommonPortData.iOmxVersion;
	iParamImagePortFormat.nPortIndex = aCommonPortData.iPortIndex;
	iParamImagePortFormat.nIndex = numImageFormats ? numImageFormats - 1 : 0;
	iParamImagePortFormat.eCompressionFormat = numImageFormats ? iSupportedImageFormats[0] : OMX_IMAGE_CodingUnused;
	iParamImagePortFormat.eColorFormat = numColorFormats ? iSupportedColorFormats[0] : OMX_COLOR_FormatUnused;
	}
	
COmxILImagePortImpl::~COmxILImagePortImpl()
	{
	DEBUG_PRINTF(_L8("COmxILImagePortImpl::~COmxILImagePortImpl"));
	iSupportedImageFormats.Close();
	iSupportedColorFormats.Close();
	}

OMX_ERRORTYPE COmxILImagePortImpl::GetParameter(OMX_INDEXTYPE aParamIndex, TAny* apComponentParameterStructure) const
	{
	DEBUG_PRINTF(_L8("COmxILImagePortImpl::GetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	
	switch(aParamIndex)
		{
		case OMX_IndexParamImagePortFormat:
			{
			if (OMX_ErrorNone != (omxRetValue =
							  TOmxILUtil::CheckOmxStructSizeAndVersion(
								  const_cast<OMX_PTR>(apComponentParameterStructure),
								  sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE))))
				{
				return omxRetValue;
				}
				
			OMX_IMAGE_PARAM_PORTFORMATTYPE* imagePortDefinition = static_cast<OMX_IMAGE_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);
			
			if(OMX_IMAGE_CodingUnused == iParamImagePortFormat.eCompressionFormat)
				{							
				if (imagePortDefinition->nIndex >= iSupportedColorFormats.Count())
					{
					return OMX_ErrorNoMore;
					}
				imagePortDefinition->eCompressionFormat = OMX_IMAGE_CodingUnused;
				imagePortDefinition->eColorFormat = iSupportedColorFormats[imagePortDefinition->nIndex];	
				}
			else
				{
				if (imagePortDefinition->nIndex >= iSupportedImageFormats.Count())
					{
					return OMX_ErrorNoMore;
					}
				imagePortDefinition->eCompressionFormat = iSupportedImageFormats[imagePortDefinition->nIndex];
				}
			break;
			}
		default:
			__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILImagePortPanicCategory, 1));
		};

	return OMX_ErrorNone;
	}

OMX_ERRORTYPE COmxILImagePortImpl::SetParameter(OMX_INDEXTYPE aParamIndex, const TAny* apComponentParameterStructure, TBool& aUpdateProcessingFunction)
	{
	DEBUG_PRINTF(_L8("COmxILImagePortImpl::SetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;

	switch(aParamIndex)
		{
		case OMX_IndexParamImagePortFormat:
			{
			if (OMX_ErrorNone != (omxRetValue =
							  TOmxILUtil::CheckOmxStructSizeAndVersion(
								  const_cast<OMX_PTR>(apComponentParameterStructure),
								  sizeof(OMX_IMAGE_PARAM_PORTFORMATTYPE))))
				{
				return omxRetValue;
				}
							
			const OMX_IMAGE_PARAM_PORTFORMATTYPE *componentParameterStructure = static_cast<const OMX_IMAGE_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);
	
			if(OMX_IMAGE_CodingUnused == componentParameterStructure->eCompressionFormat)
				{
				if(OMX_COLOR_FormatUnused == componentParameterStructure->eColorFormat)
					{
					// Both Compression Format and Color can not be Unused at the same time.
					return OMX_ErrorBadParameter;
					}
				
				if(iParamImagePortFormat.eColorFormat != componentParameterStructure->eColorFormat)
					{
					if(KErrNotFound == iSupportedColorFormats.Find(componentParameterStructure->eColorFormat))
						{
						return OMX_ErrorUnsupportedSetting;
						}
					else
						{
						iParamImagePortFormat.eColorFormat = componentParameterStructure->eColorFormat;
						}
					aUpdateProcessingFunction = ETrue;
					}
				}
			else
				{
				// Data is compressed. Change relevant variables.
				if (OMX_COLOR_FormatUnused != componentParameterStructure->eColorFormat)
					{
					// Both Compression Format and Color can not be Unused at the same time.
					return OMX_ErrorBadParameter;
					}
						
				if (iParamImagePortFormat.eCompressionFormat != componentParameterStructure->eCompressionFormat)
					{
					if(KErrNotFound == iSupportedImageFormats.Find(componentParameterStructure->eCompressionFormat))
						{
						return OMX_ErrorUnsupportedSetting;
						}
					else
						{
						iParamImagePortFormat.eCompressionFormat = componentParameterStructure->eCompressionFormat;
						aUpdateProcessingFunction = ETrue;
						}
					}	
				}
			break;
			}
		default:
			__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILImagePortPanicCategory, 1));
		};
	return OMX_ErrorNone;
	}

const RArray<OMX_IMAGE_CODINGTYPE>& COmxILImagePortImpl::GetSupportedImageFormats() const
	{
	return iSupportedImageFormats;
	}
	
const RArray<OMX_COLOR_FORMATTYPE>& COmxILImagePortImpl::GetSupportedColorFormats() const
	{
	return iSupportedColorFormats;
	}
	
const OMX_IMAGE_PARAM_PORTFORMATTYPE& COmxILImagePortImpl::GetParamImagePortFormat() const
	{
	return iParamImagePortFormat;
	}

RArray<OMX_IMAGE_CODINGTYPE>& COmxILImagePortImpl::GetSupportedImageFormats()
    {
    return iSupportedImageFormats;
    }
    
RArray<OMX_COLOR_FORMATTYPE>& COmxILImagePortImpl::GetSupportedColorFormats()
    {
    return iSupportedColorFormats;
    }
    
OMX_IMAGE_PARAM_PORTFORMATTYPE& COmxILImagePortImpl::GetParamImagePortFormat()
    {
    return iParamImagePortFormat;
    }

