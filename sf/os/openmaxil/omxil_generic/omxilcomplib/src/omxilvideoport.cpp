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

#include <openmax/il/common/omxilvideoport.h>
#include <openmax/il/common/omxilutil.h>
#include "log.h"
#include "omxilvideoportimpl.h"

EXPORT_C
COmxILVideoPort::COmxILVideoPort()
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::COmxILVideoPort"));
	}

EXPORT_C
void COmxILVideoPort::ConstructL(const TOmxILCommonPortData& aCommonPortData,
                                const RArray<OMX_VIDEO_CODINGTYPE>& aSupportedVideoFormats,
                                const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats)
	{
    COmxILPort::ConstructL(aCommonPortData); //create COmxILPortImpl
	ipVideoPortImpl=COmxILVideoPortImpl::NewL(aSupportedVideoFormats, aSupportedColorFormats, aCommonPortData); //create COmxILVideoPortImpl	
	}
	
EXPORT_C
COmxILVideoPort::~COmxILVideoPort()
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::~COmxILVideoPort"));
	delete ipVideoPortImpl;
	}

EXPORT_C OMX_ERRORTYPE 
COmxILVideoPort::GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::GetLocalOmxParamIndexes"));

	// Always collect local indexes from parent
	OMX_ERRORTYPE omxRetValue = COmxILPort::GetLocalOmxParamIndexes(aIndexArray);
	
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}
		
	TInt err = aIndexArray.InsertInOrder(OMX_IndexParamVideoPortFormat);
	
	// Note that index duplication is OK.
	if (KErrNone != err && KErrAlreadyExists != err)
		{
		return OMX_ErrorInsufficientResources;
		}
	
	return OMX_ErrorNone;

	}

EXPORT_C OMX_ERRORTYPE 
COmxILVideoPort::GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::GetLocalOmxConfigIndexes"));

	// Always collect local indexes from parent
	return COmxILPort::GetLocalOmxConfigIndexes(aIndexArray);

	}

EXPORT_C OMX_ERRORTYPE COmxILVideoPort::GetParameter(OMX_INDEXTYPE aParamIndex, TAny* apComponentParameterStructure) const
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::GetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
		case OMX_IndexParamVideoPortFormat:
			{
			__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
			omxRetValue=ipVideoPortImpl->GetParameter(aParamIndex, apComponentParameterStructure);
			break;
			}
		default:
			{
			// Try the parent's indexes
			omxRetValue=COmxILPort::GetParameter(aParamIndex, apComponentParameterStructure);
			}
		};
	return omxRetValue;
	}

EXPORT_C OMX_ERRORTYPE COmxILVideoPort::SetParameter(OMX_INDEXTYPE aParamIndex, const TAny* apComponentParameterStructure, TBool& aUpdateProcessingFunction)
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::SetParameter"));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
		case OMX_IndexParamVideoPortFormat:
			{
			__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
			omxRetValue=ipVideoPortImpl->SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
			break;
			}
		default:
			{
			// Try the parent's indexes
			omxRetValue=COmxILPort::SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
			}
		};
	return omxRetValue;
	}

EXPORT_C TBool COmxILVideoPort::UpdateColorFormat(OMX_COLOR_FORMATTYPE& aOldColor, OMX_COLOR_FORMATTYPE aNewColor, TBool& aUpdated)
	{
	__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
	return ipVideoPortImpl->UpdateColorFormat(aOldColor, aNewColor, aUpdated);
	}

EXPORT_C TBool COmxILVideoPort::UpdateCodingType(OMX_VIDEO_CODINGTYPE& aOldCodingType, OMX_VIDEO_CODINGTYPE aNewCodingType, TBool& aUpdated)
	{
	__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
	return ipVideoPortImpl->UpdateCodingType(aOldCodingType, aNewCodingType, aUpdated);
	}
	
EXPORT_C RArray<OMX_VIDEO_CODINGTYPE>& COmxILVideoPort::GetSupportedVideoFormats()
	{
	__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
	return ipVideoPortImpl->GetSupportedVideoFormats();

	}
	
EXPORT_C RArray<OMX_COLOR_FORMATTYPE>& COmxILVideoPort::GetSupportedColorFormats()
	{
	__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
	return ipVideoPortImpl->GetSupportedColorFormats();
	}
	

EXPORT_C OMX_VIDEO_PARAM_PORTFORMATTYPE& COmxILVideoPort::GetParamVideoPortFormat()
    {
    __ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
    return ipVideoPortImpl->GetParamVideoPortFormat();
    }

EXPORT_C const RArray<OMX_VIDEO_CODINGTYPE>& COmxILVideoPort::GetSupportedVideoFormats() const
	{
	__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
	return ipVideoPortImpl->GetSupportedVideoFormats();

	}
	
EXPORT_C const RArray<OMX_COLOR_FORMATTYPE>& COmxILVideoPort::GetSupportedColorFormats() const
	{
	__ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
	return ipVideoPortImpl->GetSupportedColorFormats();
	}
	

EXPORT_C const OMX_VIDEO_PARAM_PORTFORMATTYPE& COmxILVideoPort::GetParamVideoPortFormat() const
    {
    __ASSERT_ALWAYS(ipVideoPortImpl, User::Panic(KOmxILVideoPortPanicCategory, 1));
    return ipVideoPortImpl->GetParamVideoPortFormat();
    }	
	
EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::GetConfig(OMX_INDEXTYPE aConfigIndex,
                      TAny* apComponentConfigStructure) const
    {
    DEBUG_PRINTF(_L8("COmxILPort::GetConfig"));
    return COmxILPort::GetConfig(aConfigIndex,
                                 apComponentConfigStructure);
    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::SetConfig(OMX_INDEXTYPE aConfigIndex,
                      const TAny* apComponentConfigStructure,
                      TBool& aUpdateProcessingFunction)
    {
    DEBUG_PRINTF(_L8("COmxILPort::SetConfig"));
    return COmxILPort::SetConfig(aConfigIndex,
                                apComponentConfigStructure,
                                aUpdateProcessingFunction);

    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::GetExtensionIndex(OMX_STRING aParameterName,
                              OMX_INDEXTYPE* apIndexType) const
    {
    return COmxILPort::GetExtensionIndex(aParameterName,
                                        apIndexType);
    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
                           const OMX_PTR apAppPrivate,
                           OMX_U32 aSizeBytes,
                           OMX_U8* apBuffer,
                           TBool& aPortPopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILVideoPort::PopulateBuffer : pBuffer [%X]"), apBuffer);
    return COmxILPort::PopulateBuffer(appBufferHdr,
                                        apAppPrivate,
                                        aSizeBytes,
                                        apBuffer,
                                        aPortPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::FreeBuffer(OMX_BUFFERHEADERTYPE* apBufferHeader,
                       TBool& aPortDepopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILVideoPort::FreeBuffer : BUFFER [%X]"), apBufferHeader);
    return COmxILPort::FreeBuffer(apBufferHeader, aPortDepopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::TunnelRequest(OMX_HANDLETYPE aTunneledComp,
                          OMX_U32 aTunneledPort,
                          OMX_TUNNELSETUPTYPE* apTunnelSetup)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::TunnelRequest"));
    return COmxILPort::TunnelRequest(aTunneledComp,
                                    aTunneledPort,
                                    apTunnelSetup);
    }


EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::PopulateTunnel(TBool& portPopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::PopulateTunnel"));
    return COmxILPort::PopulateTunnel(portPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::FreeTunnel(TBool& portDepopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::FreeTunnel"));
    return COmxILPort::FreeTunnel(portDepopulationCompleted);
    }

EXPORT_C TBool
COmxILVideoPort::SetBufferSent(OMX_BUFFERHEADERTYPE* apBufferHeader,
                          TBool& aBufferMarkedWithOwnMark)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetBufferSent"));
    return COmxILPort::SetBufferSent(apBufferHeader,
                                    aBufferMarkedWithOwnMark);
    }

EXPORT_C TBool
COmxILVideoPort::SetBufferReturned(OMX_BUFFERHEADERTYPE* apBufferHeader)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetBufferReturned"));
    return COmxILPort::SetBufferReturned(apBufferHeader);
    }

EXPORT_C void
COmxILVideoPort::SetTransitionToEnabled()
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetTransitionToEnabled"));
    return COmxILPort::SetTransitionToEnabled();
    }

EXPORT_C void
COmxILVideoPort::SetTransitionToDisabled()
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetTransitionToDisabled"));
    return COmxILPort::SetTransitionToDisabled();
    }

EXPORT_C void
COmxILVideoPort::SetTransitionToDisabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetTransitionToDisabledCompleted"));
    return COmxILPort::SetTransitionToDisabledCompleted();
    }

EXPORT_C void
COmxILVideoPort::SetTransitionToEnabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetTransitionToEnabledCompleted"));
    return COmxILPort::SetTransitionToEnabledCompleted();
    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::StoreBufferMark(const OMX_MARKTYPE* apMark)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::StoreBufferMark"));
    return COmxILPort::StoreBufferMark(apMark);
    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::SetComponentRoleDefaults(TUint aComponentRoleIndex)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::SetComponentRoleDefaults"));

    return COmxILPort::SetComponentRoleDefaults(aComponentRoleIndex);
    }

EXPORT_C TBool
COmxILVideoPort::HasAllBuffersAtHome() const
    {
    return COmxILPort::HasAllBuffersAtHome();
    }

EXPORT_C TBool
COmxILVideoPort::IsBufferAtHome(OMX_BUFFERHEADERTYPE* apBufferHeader) const
    {
    DEBUG_PRINTF2(_L8("COmxILVideoPort::IsBufferAtHome : [%X]"), apBufferHeader);
    return COmxILPort::IsBufferAtHome(apBufferHeader);
    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::DoPortReconfiguration(TUint aPortSettingsIndex,
                                  const TDesC8& aPortSettings,
                                  OMX_EVENTTYPE& aEventForILClient)
    {
    DEBUG_PRINTF(_L8("COmxILVideoPort::DoPortReconfiguration"));

    return COmxILPort::DoPortReconfiguration(aPortSettingsIndex,
                                                aPortSettings,
                                                aEventForILClient);
    }

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::DoBufferAllocation(OMX_U32 aSizeBytes,
							   OMX_U8*& apPortSpecificBuffer,
							   OMX_PTR& apPortPrivate,
							   OMX_PTR& apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF2(_L8("COmxILVideoPort::DoBufferAllocation : aSizeBytes[%u]"), aSizeBytes);
	return COmxILPort::DoBufferAllocation(aSizeBytes,
										apPortSpecificBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILVideoPort::DoBufferDeallocation(OMX_PTR apPortSpecificBuffer,
								 OMX_PTR apPortPrivate,
								 OMX_PTR apPlatformPrivate,
								 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::DoBufferDeallocation"));
	return COmxILPort::DoBufferDeallocation(apPortSpecificBuffer,
								apPortPrivate,
								apPlatformPrivate,
								apAppPrivate);
	}


EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::DoBufferWrapping(OMX_U32 aSizeBytes,
							 OMX_U8* apBuffer,
							 OMX_PTR& apPortPrivate,
							 OMX_PTR& apPlatformPrivate,
							 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILVideoPort::DoBufferWrapping"));
	return COmxILPort::DoBufferWrapping(aSizeBytes,
										apBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILVideoPort::DoBufferUnwrapping(OMX_PTR apBuffer,
							   OMX_PTR appPortPrivate,
							   OMX_PTR apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{

	DEBUG_PRINTF(_L8("COmxILVideoPort::DoBufferUnwrapping"));
	return COmxILPort::DoBufferUnwrapping(apBuffer,
										appPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C OMX_ERRORTYPE
COmxILVideoPort::DoOmxUseBuffer(OMX_HANDLETYPE aTunnelledComponent,
						   OMX_BUFFERHEADERTYPE** appBufferHdr,
						   OMX_U32 aTunnelledPortIndex,
						   OMX_PTR apPortPrivate,
						   OMX_PTR apPlatformPrivate,
						   OMX_U32 aSizeBytes,
						   OMX_U8* apBuffer)
	{

	DEBUG_PRINTF(_L8("COmxILVideoPort::DoOmxUseBuffer"));
	return COmxILPort::DoOmxUseBuffer(aTunnelledComponent,
									   appBufferHdr,
									   aTunnelledPortIndex,
									   apPortPrivate,
									   apPlatformPrivate,
									   aSizeBytes,
									   apBuffer);
	}
	
	
EXPORT_C
TInt COmxILVideoPort::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return COmxILPort::Extension_(aExtensionId, a0, a1);
	}

