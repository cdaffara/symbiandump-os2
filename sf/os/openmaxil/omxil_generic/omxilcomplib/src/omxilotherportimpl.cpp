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

#include <openmax/il/common/omxilotherport.h>
#include <openmax/il/common/omxilutil.h>
#include "omxilotherportimpl.h"

COmxILOtherPortImpl* COmxILOtherPortImpl::NewL(const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats, const TOmxILCommonPortData& aCommonPortData)
	{
	COmxILOtherPortImpl* self = new(ELeave) COmxILOtherPortImpl(aCommonPortData);
	CleanupStack::PushL(self);
	self->ConstructL(aSupportedOtherFormats);
	CleanupStack::Pop();
	return self;	
	}

COmxILOtherPortImpl::COmxILOtherPortImpl(const TOmxILCommonPortData& aCommonPortData) 
	{
	iParamOtherPortFormat.nSize = sizeof(OMX_OTHER_PARAM_PORTFORMATTYPE);
	iParamOtherPortFormat.nVersion = aCommonPortData.iOmxVersion;
	iParamOtherPortFormat.nPortIndex = aCommonPortData.iPortIndex;
	iParamOtherPortFormat.nIndex = 0;
	}

void COmxILOtherPortImpl::ConstructL(const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats)
	{
	TUint count = aSupportedOtherFormats.Count();
	for (TInt i = 0; i < count; i++)
		{
		iSupportedOtherFormats.AppendL(aSupportedOtherFormats[i]);
		}
	iParamOtherPortFormat.eFormat = count ? iSupportedOtherFormats[0] : OMX_OTHER_FormatMax;	
	}

COmxILOtherPortImpl::~COmxILOtherPortImpl()
	{
	iSupportedOtherFormats.Close();
	}

OMX_ERRORTYPE COmxILOtherPortImpl::GetParameter(OMX_INDEXTYPE aParamIndex, 
													TAny* apComponentParameterStructure) const
	{
	switch(aParamIndex)
		{
		case OMX_IndexParamOtherPortFormat:
			{
			OMX_ERRORTYPE omxRetValue = TOmxILUtil::CheckOmxStructSizeAndVersion(apComponentParameterStructure,
					  															sizeof(OMX_OTHER_PARAM_PORTFORMATTYPE));
			
			if (omxRetValue != OMX_ErrorNone)
				{
				return omxRetValue;
				}

			OMX_OTHER_PARAM_PORTFORMATTYPE* pParamOtherPortFormat = 
				static_cast<OMX_OTHER_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);

			if (pParamOtherPortFormat->nIndex >= iSupportedOtherFormats.Count())
				{
				return OMX_ErrorNoMore;
				}
	
			pParamOtherPortFormat->eFormat = iSupportedOtherFormats[pParamOtherPortFormat->nIndex];
			}
			break;
		default:
			__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILOtherPortPanicCategory, 1));
		};

	return OMX_ErrorNone;
	}

OMX_ERRORTYPE COmxILOtherPortImpl::SetParameter(OMX_INDEXTYPE aParamIndex, 
						     const TAny* apComponentParameterStructure,
						     TBool& aUpdateProcessingFunction)
	{
	aUpdateProcessingFunction = EFalse;

	switch(aParamIndex)
		{
		case OMX_IndexParamOtherPortFormat:
			{
			OMX_ERRORTYPE omxRetValue = TOmxILUtil::CheckOmxStructSizeAndVersion(const_cast<OMX_PTR>(apComponentParameterStructure),
								  												sizeof(OMX_OTHER_PARAM_PORTFORMATTYPE));

			if (omxRetValue != OMX_ErrorNone)
				{
				return omxRetValue;
				}
						
			const OMX_OTHER_PARAM_PORTFORMATTYPE* pParamOtherPortFormat = 
				static_cast<const  OMX_OTHER_PARAM_PORTFORMATTYPE*>(apComponentParameterStructure);

			// Check in case the specified format is not actually supported by this
			// port
			OMX_OTHER_FORMATTYPE newFormatType = pParamOtherPortFormat->eFormat;

			// OMX_OTHER_FormatVendorReserved is the last of the supported values as of
			// v1.1.1
			if (newFormatType > OMX_OTHER_FormatVendorReserved)
			    {
			    return OMX_ErrorBadParameter;
			    }

			if (KErrNotFound == iSupportedOtherFormats.Find(newFormatType))
			    {
			    return OMX_ErrorUnsupportedSetting;
			    }

			// Set the new default format, but check first that we are actually
			// changing something...
			if (iParamOtherPortFormat.eFormat != newFormatType)
			    {
			    iParamOtherPortFormat.eFormat = newFormatType;

			    // This is an indication to the PortManager that the processing
			    // function needs to get updated
			    //
			    aUpdateProcessingFunction = ETrue;
			    }
			}
			break;
		default:
			__ASSERT_ALWAYS(EFalse, User::Panic(KOmxILOtherPortPanicCategory, 1));
		};

	return OMX_ErrorNone;
	}

const RArray<OMX_OTHER_FORMATTYPE>& COmxILOtherPortImpl::GetSupportedOtherFormats() const
        {
        return iSupportedOtherFormats;
        }

const OMX_OTHER_PARAM_PORTFORMATTYPE& COmxILOtherPortImpl::GetParamOtherPortFormat() const
        {
        return iParamOtherPortFormat;
        }

RArray<OMX_OTHER_FORMATTYPE>& COmxILOtherPortImpl::GetSupportedOtherFormats()
        {
        return iSupportedOtherFormats;
        }

OMX_OTHER_PARAM_PORTFORMATTYPE& COmxILOtherPortImpl::GetParamOtherPortFormat()
        {
        return iParamOtherPortFormat;
        }


