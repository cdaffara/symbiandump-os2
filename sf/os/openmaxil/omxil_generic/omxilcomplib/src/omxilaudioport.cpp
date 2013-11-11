// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "omxilaudioportimpl.h"
#include <openmax/il/common/omxilaudioport.h>
#include <openmax/il/common/omxilutil.h>

EXPORT_C
COmxILAudioPort::~COmxILAudioPort()
	{
    DEBUG_PRINTF(_L8("COmxILAudioPort::~COmxILAudioPort"));
    delete ipAudioPortImpl;
	}

EXPORT_C
COmxILAudioPort::COmxILAudioPort()
	{
    DEBUG_PRINTF(_L8("COmxILAudioPort::COmxILAudioPort"));
	}

EXPORT_C void COmxILAudioPort::ConstructL(const TOmxILCommonPortData& aCommonPortData, const RArray<OMX_AUDIO_CODINGTYPE>& aSupportedAudioFormats)
	{
    COmxILPort::ConstructL(aCommonPortData); //create COmxILPortImpl
	ipAudioPortImpl=COmxILAudioPortImpl::NewL(aSupportedAudioFormats, aCommonPortData); //create COmxILAudioPortImpl
	}

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const
	{
    DEBUG_PRINTF(_L8("COmxILAudioPort::GetLocalOmxParamIndexes"));

	// Always collect local indexes from parent
	OMX_ERRORTYPE omxRetValue =
		COmxILPort::GetLocalOmxParamIndexes(aIndexArray);

	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	TInt err = aIndexArray.InsertInOrder(OMX_IndexParamAudioPortFormat);

	// Note that index duplication is OK.
	if (KErrNone != err && KErrAlreadyExists != err)
		{
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;

	}

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const
	{
    DEBUG_PRINTF(_L8("COmxILAudioPort::GetLocalOmxConfigIndexes"));

	// Always collect local indexes from parent
	return COmxILPort::GetLocalOmxConfigIndexes(aIndexArray);

	}

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::GetParameter(OMX_INDEXTYPE aParamIndex,
							  TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILAudioPort::GetParameter"));
	__ASSERT_ALWAYS(ipAudioPortImpl, User::Panic(KOmxILAudioPortPanicCategory, 1));
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	
	switch(aParamIndex)
		{
	case OMX_IndexParamAudioPortFormat:
		{
		omxRetValue=ipAudioPortImpl->GetParameter(aParamIndex, apComponentParameterStructure);
		}
		break;

	default:
		{
		// Try the parent's indexes
		omxRetValue=COmxILPort::GetParameter(aParamIndex, apComponentParameterStructure);
		}
		};

	return omxRetValue;

	}

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::SetParameter(OMX_INDEXTYPE aParamIndex,
							  const TAny* apComponentParameterStructure,
							  TBool& aUpdateProcessingFunction)
	{
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetParameter"));
	__ASSERT_ALWAYS(ipAudioPortImpl, User::Panic(KOmxILAudioPortPanicCategory, 1));
	aUpdateProcessingFunction = EFalse;

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
	case OMX_IndexParamAudioPortFormat:
		{
		omxRetValue=ipAudioPortImpl->SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
		}
		break;

	default:
		{
		// Try the parent's indexes
		omxRetValue = COmxILPort::SetParameter(aParamIndex,
										apComponentParameterStructure,
										aUpdateProcessingFunction);
		}
		};

	return omxRetValue;
	}


EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::GetConfig(OMX_INDEXTYPE aConfigIndex,
                      TAny* apComponentConfigStructure) const
    {
    DEBUG_PRINTF(_L8("COmxILPort::GetConfig"));
    return COmxILPort::GetConfig(aConfigIndex,
                                 apComponentConfigStructure);
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::SetConfig(OMX_INDEXTYPE aConfigIndex,
                      const TAny* apComponentConfigStructure,
                      TBool& aUpdateProcessingFunction)
    {
    DEBUG_PRINTF(_L8("COmxILPort::SetConfig"));
    return COmxILPort::SetConfig(aConfigIndex,
                                apComponentConfigStructure,
                                aUpdateProcessingFunction);

    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::GetExtensionIndex(OMX_STRING aParameterName,
                              OMX_INDEXTYPE* apIndexType) const
    {
    return COmxILPort::GetExtensionIndex(aParameterName,
                                        apIndexType);
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
                           const OMX_PTR apAppPrivate,
                           OMX_U32 aSizeBytes,
                           OMX_U8* apBuffer,
                           TBool& aPortPopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILAudioPort::PopulateBuffer : pBuffer [%X]"), apBuffer);
    return COmxILPort::PopulateBuffer(appBufferHdr,
                                        apAppPrivate,
                                        aSizeBytes,
                                        apBuffer,
                                        aPortPopulationCompleted);
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::FreeBuffer(OMX_BUFFERHEADERTYPE* apBufferHeader,
                       TBool& aPortDepopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILAudioPort::FreeBuffer : BUFFER [%X]"), apBufferHeader);
    return COmxILPort::FreeBuffer(apBufferHeader, aPortDepopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::TunnelRequest(OMX_HANDLETYPE aTunneledComp,
                          OMX_U32 aTunneledPort,
                          OMX_TUNNELSETUPTYPE* apTunnelSetup)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::TunnelRequest"));
    return COmxILPort::TunnelRequest(aTunneledComp,
                                    aTunneledPort,
                                    apTunnelSetup);
    }


EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::PopulateTunnel(TBool& portPopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::PopulateTunnel"));
    return COmxILPort::PopulateTunnel(portPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::FreeTunnel(TBool& portDepopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::FreeTunnel"));
    return COmxILPort::FreeTunnel(portDepopulationCompleted);
    }

EXPORT_C TBool
COmxILAudioPort::SetBufferSent(OMX_BUFFERHEADERTYPE* apBufferHeader,
                          TBool& aBufferMarkedWithOwnMark)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetBufferSent"));
    return COmxILPort::SetBufferSent(apBufferHeader,
                                    aBufferMarkedWithOwnMark);
    }

EXPORT_C TBool
COmxILAudioPort::SetBufferReturned(OMX_BUFFERHEADERTYPE* apBufferHeader)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetBufferReturned"));
    return COmxILPort::SetBufferReturned(apBufferHeader);
    }

EXPORT_C void
COmxILAudioPort::SetTransitionToEnabled()
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetTransitionToEnabled"));
    return COmxILPort::SetTransitionToEnabled();
    }

EXPORT_C void
COmxILAudioPort::SetTransitionToDisabled()
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetTransitionToDisabled"));
    return COmxILPort::SetTransitionToDisabled();
    }

EXPORT_C void
COmxILAudioPort::SetTransitionToDisabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetTransitionToDisabledCompleted"));
    return COmxILPort::SetTransitionToDisabledCompleted();
    }

EXPORT_C void
COmxILAudioPort::SetTransitionToEnabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetTransitionToEnabledCompleted"));
    return COmxILPort::SetTransitionToEnabledCompleted();
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::StoreBufferMark(const OMX_MARKTYPE* apMark)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::StoreBufferMark"));
    return COmxILPort::StoreBufferMark(apMark);
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::SetComponentRoleDefaults(TUint aComponentRoleIndex)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::SetComponentRoleDefaults"));

    return COmxILPort::SetComponentRoleDefaults(aComponentRoleIndex);
    }

EXPORT_C TBool
COmxILAudioPort::HasAllBuffersAtHome() const
    {
    return COmxILPort::HasAllBuffersAtHome();
    }

EXPORT_C TBool
COmxILAudioPort::IsBufferAtHome(OMX_BUFFERHEADERTYPE* apBufferHeader) const
    {
    DEBUG_PRINTF2(_L8("COmxILAudioPort::IsBufferAtHome : [%X]"), apBufferHeader);
    return COmxILPort::IsBufferAtHome(apBufferHeader);
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::DoPortReconfiguration(TUint aPortSettingsIndex,
                                  const TDesC8& aPortSettings,
                                  OMX_EVENTTYPE& aEventForILClient)
    {
    DEBUG_PRINTF(_L8("COmxILAudioPort::DoPortReconfiguration"));

    return COmxILPort::DoPortReconfiguration(aPortSettingsIndex,
                                                aPortSettings,
                                                aEventForILClient);
    }

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::DoBufferAllocation(OMX_U32 aSizeBytes,
							   OMX_U8*& apPortSpecificBuffer,
							   OMX_PTR& apPortPrivate,
							   OMX_PTR& apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF2(_L8("COmxILAudioPort::DoBufferAllocation : aSizeBytes[%u]"), aSizeBytes);
	return COmxILPort::DoBufferAllocation(aSizeBytes,
										apPortSpecificBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILAudioPort::DoBufferDeallocation(OMX_PTR apPortSpecificBuffer,
								 OMX_PTR apPortPrivate,
								 OMX_PTR apPlatformPrivate,
								 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILAudioPort::DoBufferDeallocation"));
	return COmxILPort::DoBufferDeallocation(apPortSpecificBuffer,
								apPortPrivate,
								apPlatformPrivate,
								apAppPrivate);
	}


EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::DoBufferWrapping(OMX_U32 aSizeBytes,
							 OMX_U8* apBuffer,
							 OMX_PTR& apPortPrivate,
							 OMX_PTR& apPlatformPrivate,
							 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILAudioPort::DoBufferWrapping"));
	return COmxILPort::DoBufferWrapping(aSizeBytes,
										apBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILAudioPort::DoBufferUnwrapping(OMX_PTR apBuffer,
							   OMX_PTR appPortPrivate,
							   OMX_PTR apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{

	DEBUG_PRINTF(_L8("COmxILAudioPort::DoBufferUnwrapping"));
	return COmxILPort::DoBufferUnwrapping(apBuffer,
										appPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C OMX_ERRORTYPE
COmxILAudioPort::DoOmxUseBuffer(OMX_HANDLETYPE aTunnelledComponent,
						   OMX_BUFFERHEADERTYPE** appBufferHdr,
						   OMX_U32 aTunnelledPortIndex,
						   OMX_PTR apPortPrivate,
						   OMX_PTR apPlatformPrivate,
						   OMX_U32 aSizeBytes,
						   OMX_U8* apBuffer)
	{

	DEBUG_PRINTF(_L8("COmxILAudioPort::DoOmxUseBuffer"));
	return COmxILPort::DoOmxUseBuffer(aTunnelledComponent,
									   appBufferHdr,
									   aTunnelledPortIndex,
									   apPortPrivate,
									   apPlatformPrivate,
									   aSizeBytes,
									   apBuffer);
	}

EXPORT_C
TInt COmxILAudioPort::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return COmxILPort::Extension_(aExtensionId, a0, a1);
	}
	
