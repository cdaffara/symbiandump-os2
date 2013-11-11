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
#include "log.h"

EXPORT_C COmxILOtherPort::COmxILOtherPort()
	{
	}
	
EXPORT_C void COmxILOtherPort::ConstructL(const TOmxILCommonPortData& aCommonPortData, const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats)
	{
    COmxILPort::ConstructL(aCommonPortData); //create COmxILPortImpl
	ipOtherPortImpl=COmxILOtherPortImpl::NewL(aSupportedOtherFormats, aCommonPortData); //create COmxILOtherPortImpl	
	}

EXPORT_C COmxILOtherPort::~COmxILOtherPort()
	{
	delete ipOtherPortImpl;
	}

EXPORT_C OMX_ERRORTYPE COmxILOtherPort::GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const
	{
	// Always collect local indexes from parent
	OMX_ERRORTYPE omxRetValue = COmxILPort::GetLocalOmxParamIndexes(aIndexArray);
	if (omxRetValue != OMX_ErrorNone)
		{
		return omxRetValue;
		}

	TInt err = aIndexArray.InsertInOrder(OMX_IndexParamOtherPortFormat);
	// Note that index duplication is OK
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;
	}

EXPORT_C OMX_ERRORTYPE COmxILOtherPort::GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const
	{
	// Always collect local indexes from parent
	return COmxILPort::GetLocalOmxConfigIndexes(aIndexArray);
	}

EXPORT_C OMX_ERRORTYPE COmxILOtherPort::GetParameter(OMX_INDEXTYPE aParamIndex, 
													TAny* apComponentParameterStructure) const
	{
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
		case OMX_IndexParamOtherPortFormat:
			{
			__ASSERT_ALWAYS(ipOtherPortImpl, User::Panic(KOmxILOtherPortPanicCategory, 1));
			omxRetValue = ipOtherPortImpl->GetParameter(aParamIndex, apComponentParameterStructure);
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

EXPORT_C OMX_ERRORTYPE COmxILOtherPort::SetParameter(OMX_INDEXTYPE aParamIndex, 
						     const TAny* apComponentParameterStructure,
						     TBool& aUpdateProcessingFunction)
	{
	aUpdateProcessingFunction = EFalse;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
		case OMX_IndexParamOtherPortFormat:
			{
			__ASSERT_ALWAYS(ipOtherPortImpl, User::Panic(KOmxILOtherPortPanicCategory, 1));
			omxRetValue = ipOtherPortImpl->SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
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
COmxILOtherPort::GetConfig(OMX_INDEXTYPE aConfigIndex,
                      TAny* apComponentConfigStructure) const
    {
    return COmxILPort::GetConfig(aConfigIndex,
                                 apComponentConfigStructure);
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::SetConfig(OMX_INDEXTYPE aConfigIndex,
                      const TAny* apComponentConfigStructure,
                      TBool& aUpdateProcessingFunction)
    {
    return COmxILPort::SetConfig(aConfigIndex,
                                apComponentConfigStructure,
                                aUpdateProcessingFunction);
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::GetExtensionIndex(OMX_STRING aParameterName,
                              OMX_INDEXTYPE* apIndexType) const
    {
    return COmxILPort::GetExtensionIndex(aParameterName,
                                        apIndexType);
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
                           const OMX_PTR apAppPrivate,
                           OMX_U32 aSizeBytes,
                           OMX_U8* apBuffer,
                           TBool& aPortPopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILOtherPort::PopulateBuffer : pBuffer [%X]"), apBuffer);
    return COmxILPort::PopulateBuffer(appBufferHdr,
                                        apAppPrivate,
                                        aSizeBytes,
                                        apBuffer,
                                        aPortPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::FreeBuffer(OMX_BUFFERHEADERTYPE* apBufferHeader,
                       TBool& aPortDepopulationCompleted)
    {
    DEBUG_PRINTF2(_L8("COmxILOtherPort::FreeBuffer : BUFFER [%X]"), apBufferHeader);
    return COmxILPort::FreeBuffer(apBufferHeader, aPortDepopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::TunnelRequest(OMX_HANDLETYPE aTunneledComp,
                          OMX_U32 aTunneledPort,
                          OMX_TUNNELSETUPTYPE* apTunnelSetup)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::TunnelRequest"));
    return COmxILPort::TunnelRequest(aTunneledComp,
                                    aTunneledPort,
                                    apTunnelSetup);
    }


EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::PopulateTunnel(TBool& portPopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::PopulateTunnel"));
    return COmxILPort::PopulateTunnel(portPopulationCompleted);
    }


EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::FreeTunnel(TBool& portDepopulationCompleted)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::FreeTunnel"));
    return COmxILPort::FreeTunnel(portDepopulationCompleted);
    }

EXPORT_C TBool
COmxILOtherPort::SetBufferSent(OMX_BUFFERHEADERTYPE* apBufferHeader,
                          TBool& aBufferMarkedWithOwnMark)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetBufferSent"));
    return COmxILPort::SetBufferSent(apBufferHeader,
                                    aBufferMarkedWithOwnMark);
    }

EXPORT_C TBool
COmxILOtherPort::SetBufferReturned(OMX_BUFFERHEADERTYPE* apBufferHeader)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetBufferReturned"));
    return COmxILPort::SetBufferReturned(apBufferHeader);
    }

EXPORT_C void
COmxILOtherPort::SetTransitionToEnabled()
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetTransitionToEnabled"));
    return COmxILPort::SetTransitionToEnabled();
    }

EXPORT_C void
COmxILOtherPort::SetTransitionToDisabled()
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetTransitionToDisabled"));
    return COmxILPort::SetTransitionToDisabled();
    }

EXPORT_C void
COmxILOtherPort::SetTransitionToDisabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetTransitionToDisabledCompleted"));
    return COmxILPort::SetTransitionToDisabledCompleted();
    }

EXPORT_C void
COmxILOtherPort::SetTransitionToEnabledCompleted()
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetTransitionToEnabledCompleted"));
    return COmxILPort::SetTransitionToEnabledCompleted();
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::StoreBufferMark(const OMX_MARKTYPE* apMark)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::StoreBufferMark"));
    return COmxILPort::StoreBufferMark(apMark);
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::SetComponentRoleDefaults(TUint aComponentRoleIndex)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::SetComponentRoleDefaults"));

    return COmxILPort::SetComponentRoleDefaults(aComponentRoleIndex);
    }

EXPORT_C TBool
COmxILOtherPort::HasAllBuffersAtHome() const
    {
    return COmxILPort::HasAllBuffersAtHome();
    }

EXPORT_C TBool
COmxILOtherPort::IsBufferAtHome(OMX_BUFFERHEADERTYPE* apBufferHeader) const
    {
    DEBUG_PRINTF2(_L8("COmxILOtherPort::IsBufferAtHome : [%X]"), apBufferHeader);
    return COmxILPort::IsBufferAtHome(apBufferHeader);
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::DoPortReconfiguration(TUint aPortSettingsIndex,
                                  const TDesC8& aPortSettings,
                                  OMX_EVENTTYPE& aEventForILClient)
    {
    DEBUG_PRINTF(_L8("COmxILOtherPort::DoPortReconfiguration"));

    return COmxILPort::DoPortReconfiguration(aPortSettingsIndex,
                                                aPortSettings,
                                                aEventForILClient);
    }

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::DoBufferAllocation(OMX_U32 aSizeBytes,
							   OMX_U8*& apPortSpecificBuffer,
							   OMX_PTR& apPortPrivate,
							   OMX_PTR& apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF2(_L8("COmxILOtherPort::DoBufferAllocation : aSizeBytes[%u]"), aSizeBytes);
	return COmxILPort::DoBufferAllocation(aSizeBytes,
										apPortSpecificBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILOtherPort::DoBufferDeallocation(OMX_PTR apPortSpecificBuffer,
								 OMX_PTR apPortPrivate,
								 OMX_PTR apPlatformPrivate,
								 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILOtherPort::DoBufferDeallocation"));
	return COmxILPort::DoBufferDeallocation(apPortSpecificBuffer,
								apPortPrivate,
								apPlatformPrivate,
								apAppPrivate);
	}


EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::DoBufferWrapping(OMX_U32 aSizeBytes,
							 OMX_U8* apBuffer,
							 OMX_PTR& apPortPrivate,
							 OMX_PTR& apPlatformPrivate,
							 OMX_PTR apAppPrivate)
	{
	DEBUG_PRINTF(_L8("COmxILOtherPort::DoBufferWrapping"));
	return COmxILPort::DoBufferWrapping(aSizeBytes,
										apBuffer,
										apPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C void
COmxILOtherPort::DoBufferUnwrapping(OMX_PTR apBuffer,
							   OMX_PTR appPortPrivate,
							   OMX_PTR apPlatformPrivate,
							   OMX_PTR apAppPrivate)
	{

	DEBUG_PRINTF(_L8("COmxILOtherPort::DoBufferUnwrapping"));
	return COmxILPort::DoBufferUnwrapping(apBuffer,
										appPortPrivate,
										apPlatformPrivate,
										apAppPrivate);
	}

EXPORT_C OMX_ERRORTYPE
COmxILOtherPort::DoOmxUseBuffer(OMX_HANDLETYPE aTunnelledComponent,
						   OMX_BUFFERHEADERTYPE** appBufferHdr,
						   OMX_U32 aTunnelledPortIndex,
						   OMX_PTR apPortPrivate,
						   OMX_PTR apPlatformPrivate,
						   OMX_U32 aSizeBytes,
						   OMX_U8* apBuffer)
	{

	DEBUG_PRINTF(_L8("COmxILOtherPort::DoOmxUseBuffer"));
	return COmxILPort::DoOmxUseBuffer(aTunnelledComponent,
									   appBufferHdr,
									   aTunnelledPortIndex,
									   apPortPrivate,
									   apPlatformPrivate,
									   aSizeBytes,
									   apBuffer);
	}

EXPORT_C RArray<OMX_OTHER_FORMATTYPE>& COmxILOtherPort::GetSupportedOtherFormats()
            {
            __ASSERT_ALWAYS(ipOtherPortImpl, User::Panic(KOmxILOtherPortPanicCategory, 1));
            return ipOtherPortImpl->GetSupportedOtherFormats();
            }
EXPORT_C OMX_OTHER_PARAM_PORTFORMATTYPE& COmxILOtherPort::GetParamOtherPortFormat()
    {
    __ASSERT_ALWAYS(ipOtherPortImpl, User::Panic(KOmxILOtherPortPanicCategory, 1));
    return ipOtherPortImpl->GetParamOtherPortFormat();
    }
EXPORT_C const RArray<OMX_OTHER_FORMATTYPE>& COmxILOtherPort::GetSupportedOtherFormats() const
        {
        __ASSERT_ALWAYS(ipOtherPortImpl, User::Panic(KOmxILOtherPortPanicCategory, 1));
        return ipOtherPortImpl->GetSupportedOtherFormats();
    
        }
EXPORT_C const OMX_OTHER_PARAM_PORTFORMATTYPE& COmxILOtherPort::GetParamOtherPortFormat() const
        {
        __ASSERT_ALWAYS(ipOtherPortImpl, User::Panic(KOmxILOtherPortPanicCategory, 1));
        return ipOtherPortImpl->GetParamOtherPortFormat();   
        }

EXPORT_C
TInt COmxILOtherPort::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return COmxILPort::Extension_(aExtensionId, a0, a1);
	}	
