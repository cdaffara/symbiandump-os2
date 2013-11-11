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

EXPORT_C
COmxILImagePort::COmxILImagePort()
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::COmxILImagePort"));
	}

EXPORT_C
COmxILImagePort::~COmxILImagePort()
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::~COmxILImagePort"))
	delete ipImagePortImpl;
	}

EXPORT_C OMX_ERRORTYPE 
COmxILImagePort::GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::GetLocalOmxParamIndexes"));

	// Always collect local indexes from parent
	OMX_ERRORTYPE omxRetValue = COmxILPort::GetLocalOmxParamIndexes(aIndexArray);
	
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}
		
	TInt err = aIndexArray.InsertInOrder(OMX_IndexParamImagePortFormat);
	
	// Note that index duplication is OK.
	if (KErrNone != err && KErrAlreadyExists != err)
		{
		return OMX_ErrorInsufficientResources;
		}
	
	return OMX_ErrorNone;

	}

EXPORT_C OMX_ERRORTYPE 
COmxILImagePort::GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::GetLocalOmxConfigIndexes"));

	// Always collect local indexes from parent
	return COmxILPort::GetLocalOmxConfigIndexes(aIndexArray);

	}

EXPORT_C OMX_ERRORTYPE COmxILImagePort::GetParameter(OMX_INDEXTYPE aParamIndex, TAny* apComponentParameterStructure) const
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::GetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
		case OMX_IndexParamImagePortFormat:
			{
			__ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
			omxRetValue = ipImagePortImpl->GetParameter(aParamIndex, apComponentParameterStructure);
			}
			break;
		default:
			{
			// Try the parent's indexes
			omxRetValue = COmxILPort::GetParameter(aParamIndex, apComponentParameterStructure);
			}
		};

	return omxRetValue;
	}

EXPORT_C OMX_ERRORTYPE COmxILImagePort::SetParameter(OMX_INDEXTYPE aParamIndex, const TAny* apComponentParameterStructure, TBool& aUpdateProcessingFunction)
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::SetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;

	switch(aParamIndex)
		{
		case OMX_IndexParamImagePortFormat:
			{
			__ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
			omxRetValue = ipImagePortImpl->SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
			}
			break;
		default:
			{
			// Try the parent's indexes
			omxRetValue = COmxILPort::SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
			}
		};
	return omxRetValue;
	}

EXPORT_C 
void COmxILImagePort::ConstructL(const TOmxILCommonPortData& aCommonPortData, 
                                const RArray<OMX_IMAGE_CODINGTYPE>& aSupportedImageFormats,
								const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats)
	{
    COmxILPort::ConstructL(aCommonPortData); //create COmxILPortImpl
	ipImagePortImpl=COmxILImagePortImpl::NewL(aCommonPortData, aSupportedImageFormats, aSupportedColorFormats); //create COmxILImagePortImpl
	}
	
EXPORT_C OMX_ERRORTYPE
COmxILImagePort::GetConfig(OMX_INDEXTYPE aConfigIndex,
                      TAny* apComponentConfigStructure) const
    {
    DEBUG_PRINTF(_L8("COmxILPort::GetConfig"));
    return COmxILPort::GetConfig(aConfigIndex,
                                 apComponentConfigStructure);
    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::SetConfig(OMX_INDEXTYPE aConfigIndex,
                      const TAny* apComponentConfigStructure,
                      TBool& aUpdateProcessingFunction)
    {
    DEBUG_PRINTF(_L8("COmxILPort::SetConfig"));
    return COmxILPort::SetConfig(aConfigIndex,
                                apComponentConfigStructure,
                                aUpdateProcessingFunction);

    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::GetExtensionIndex(OMX_STRING aParameterName,
                              OMX_INDEXTYPE* apIndexType) const
    {
    return COmxILPort::GetExtensionIndex(aParameterName,
                                        apIndexType);
    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
                           const OMX_PTR apAppPrivate,
                           OMX_U32 aSizeBytes,
                           OMX_U8* apBuffer,
                           TBool& aPortPopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILImagePort::PopulateBuffer : pBuffer [%X]"), apBuffer);
    return COmxILPort::PopulateBuffer(appBufferHdr,
                                        apAppPrivate,
                                        aSizeBytes,
                                        apBuffer,
                                        aPortPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILImagePort::FreeBuffer(OMX_BUFFERHEADERTYPE* apBufferHeader,
                       TBool& aPortDepopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILImagePort::FreeBuffer : BUFFER [%X]"), apBufferHeader);
    return COmxILPort::FreeBuffer(apBufferHeader, aPortDepopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILImagePort::TunnelRequest(OMX_HANDLETYPE aTunneledComp,
                          OMX_U32 aTunneledPort,
                          OMX_TUNNELSETUPTYPE* apTunnelSetup)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::TunnelRequest"));
    return COmxILPort::TunnelRequest(aTunneledComp,
                                    aTunneledPort,
                                    apTunnelSetup);
    }


EXPORT_C OMX_ERRORTYPE
COmxILImagePort::PopulateTunnel(TBool& portPopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::PopulateTunnel"));
    return COmxILPort::PopulateTunnel(portPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILImagePort::FreeTunnel(TBool& portDepopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::FreeTunnel"));
    return COmxILPort::FreeTunnel(portDepopulationCompleted);
    }

EXPORT_C TBool
COmxILImagePort::SetBufferSent(OMX_BUFFERHEADERTYPE* apBufferHeader,
                          TBool& aBufferMarkedWithOwnMark)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetBufferSent"));
    return COmxILPort::SetBufferSent(apBufferHeader,
                                    aBufferMarkedWithOwnMark);
    }

EXPORT_C TBool
COmxILImagePort::SetBufferReturned(OMX_BUFFERHEADERTYPE* apBufferHeader)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetBufferReturned"));
    return COmxILPort::SetBufferReturned(apBufferHeader);
    }

EXPORT_C void
COmxILImagePort::SetTransitionToEnabled()
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetTransitionToEnabled"));
    return COmxILPort::SetTransitionToEnabled();
    }

EXPORT_C void
COmxILImagePort::SetTransitionToDisabled()
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetTransitionToDisabled"));
    return COmxILPort::SetTransitionToDisabled();
    }

EXPORT_C void
COmxILImagePort::SetTransitionToDisabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetTransitionToDisabledCompleted"));
    return COmxILPort::SetTransitionToDisabledCompleted();
    }

EXPORT_C void
COmxILImagePort::SetTransitionToEnabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetTransitionToEnabledCompleted"));
    return COmxILPort::SetTransitionToEnabledCompleted();
    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::StoreBufferMark(const OMX_MARKTYPE* apMark)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::StoreBufferMark"));
    return COmxILPort::StoreBufferMark(apMark);
    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::SetComponentRoleDefaults(TUint aComponentRoleIndex)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::SetComponentRoleDefaults"));

    return COmxILPort::SetComponentRoleDefaults(aComponentRoleIndex);
    }

EXPORT_C TBool
COmxILImagePort::HasAllBuffersAtHome() const
    {
    return COmxILPort::HasAllBuffersAtHome();
    }

EXPORT_C TBool
COmxILImagePort::IsBufferAtHome(OMX_BUFFERHEADERTYPE* apBufferHeader) const
    {
    DEBUG_PRINTF2(_L8("COmxILImagePort::IsBufferAtHome : [%X]"), apBufferHeader);
    return COmxILPort::IsBufferAtHome(apBufferHeader);
    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::DoPortReconfiguration(TUint aPortSettingsIndex,
                                  const TDesC8& aPortSettings,
                                  OMX_EVENTTYPE& aEventForILClient)
    {
    DEBUG_PRINTF(_L8("COmxILImagePort::DoPortReconfiguration"));

    return COmxILPort::DoPortReconfiguration(aPortSettingsIndex,
                                                aPortSettings,
                                                aEventForILClient);
    }

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::DoBufferAllocation(OMX_U32 aSizeBytes,
							   OMX_U8*& apPortSpecificBuffer,
							   OMX_PTR& apPortPrivate,
							   OMX_PTR& apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF2(_L8("COmxILImagePort::DoBufferAllocation : aSizeBytes[%u]"), aSizeBytes);
	return COmxILPort::DoBufferAllocation(aSizeBytes,
										apPortSpecificBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILImagePort::DoBufferDeallocation(OMX_PTR apPortSpecificBuffer,
								 OMX_PTR apPortPrivate,
								 OMX_PTR apPlatformPrivate,
								 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::DoBufferDeallocation"));
	return COmxILPort::DoBufferDeallocation(apPortSpecificBuffer,
								apPortPrivate,
								apPlatformPrivate,
								apAppPrivate);
	}


EXPORT_C OMX_ERRORTYPE
COmxILImagePort::DoBufferWrapping(OMX_U32 aSizeBytes,
							 OMX_U8* apBuffer,
							 OMX_PTR& apPortPrivate,
							 OMX_PTR& apPlatformPrivate,
							 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILImagePort::DoBufferWrapping"));
	return COmxILPort::DoBufferWrapping(aSizeBytes,
										apBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILImagePort::DoBufferUnwrapping(OMX_PTR apBuffer,
							   OMX_PTR appPortPrivate,
							   OMX_PTR apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{

	DEBUG_PRINTF(_L8("COmxILImagePort::DoBufferUnwrapping"));
	return COmxILPort::DoBufferUnwrapping(apBuffer,
										appPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C OMX_ERRORTYPE
COmxILImagePort::DoOmxUseBuffer(OMX_HANDLETYPE aTunnelledComponent,
						   OMX_BUFFERHEADERTYPE** appBufferHdr,
						   OMX_U32 aTunnelledPortIndex,
						   OMX_PTR apPortPrivate,
						   OMX_PTR apPlatformPrivate,
						   OMX_U32 aSizeBytes,
						   OMX_U8* apBuffer)
	{

	DEBUG_PRINTF(_L8("COmxILImagePort::DoOmxUseBuffer"));
	return COmxILPort::DoOmxUseBuffer(aTunnelledComponent,
									   appBufferHdr,
									   aTunnelledPortIndex,
									   apPortPrivate,
									   apPlatformPrivate,
									   aSizeBytes,
									   apBuffer);
	}
	
	
EXPORT_C
TInt COmxILImagePort::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return COmxILPort::Extension_(aExtensionId, a0, a1);
	}
	
EXPORT_C
const RArray<OMX_IMAGE_CODINGTYPE>& COmxILImagePort::GetSupportedImageFormats() const
	{
    __ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
	return ipImagePortImpl->GetSupportedImageFormats();
	}
	
EXPORT_C	
const RArray<OMX_COLOR_FORMATTYPE>& COmxILImagePort::GetSupportedColorFormats() const
	{
    __ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
	return ipImagePortImpl->GetSupportedColorFormats();
	}

EXPORT_C
const OMX_IMAGE_PARAM_PORTFORMATTYPE& COmxILImagePort::GetParamImagePortFormat() const
	{
    __ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
	return ipImagePortImpl->GetParamImagePortFormat();
	}

EXPORT_C
RArray<OMX_IMAGE_CODINGTYPE>& COmxILImagePort::GetSupportedImageFormats()
    {
    __ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
    return ipImagePortImpl->GetSupportedImageFormats();
    }
    
EXPORT_C    
RArray<OMX_COLOR_FORMATTYPE>& COmxILImagePort::GetSupportedColorFormats()
    {
    __ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
    return ipImagePortImpl->GetSupportedColorFormats();
    }

EXPORT_C
OMX_IMAGE_PARAM_PORTFORMATTYPE& COmxILImagePort::GetParamImagePortFormat()
    {
    __ASSERT_ALWAYS(ipImagePortImpl, User::Panic(KOmxILImagePortPanicCategory, 1));
    return ipImagePortImpl->GetParamImagePortFormat();
    }
