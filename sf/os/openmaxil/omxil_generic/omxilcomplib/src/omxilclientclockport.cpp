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

#include <openmax/il/common/omxilclientclockport.h>
#include "log.h"


EXPORT_C COmxILClientClockPort* COmxILClientClockPort::NewL(const TOmxILCommonPortData& aCommonPortData, 
							    							const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats)
	{
	COmxILClientClockPort* self = new(ELeave) COmxILClientClockPort();
	CleanupStack::PushL(self);
	self->ConstructL(aCommonPortData, aSupportedOtherFormats);
	CleanupStack::Pop();
	return self;
	}

void COmxILClientClockPort::ConstructL(const TOmxILCommonPortData& aCommonPortData, const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats)
    {
    COmxILOtherPort::ConstructL(aCommonPortData, aSupportedOtherFormats);
    }

COmxILClientClockPort::COmxILClientClockPort() 
    : iSpecVersion(TOmxILSpecVersion())
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::COmxILClientClockPort"));
	}

OMX_ERRORTYPE COmxILClientClockPort::GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetLocalOmxParamIndexes"));
	return COmxILOtherPort::GetLocalOmxParamIndexes(aIndexArray);
	}


OMX_ERRORTYPE COmxILClientClockPort::GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetLocalOmxParamIndexes"));
	return COmxILOtherPort::GetLocalOmxConfigIndexes(aIndexArray);
	}


OMX_ERRORTYPE COmxILClientClockPort::GetParameter(OMX_INDEXTYPE aParamIndex,
						     							TAny* apComponentParameterStructure) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetParameter"));
	return COmxILOtherPort::GetParameter(aParamIndex, apComponentParameterStructure);
	}


OMX_ERRORTYPE COmxILClientClockPort::SetParameter(OMX_INDEXTYPE aParamIndex,
						     							const TAny* apComponentParameterStructure,
						     							TBool& aUpdateProcessingFunction)
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::SetParameter"));
	return COmxILOtherPort::SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
	}


OMX_ERRORTYPE COmxILClientClockPort::SetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& /*aPortDefinition*/,
								  										TBool& /*aUpdateProcessingFunction*/)
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::SetFormatInPortDefinition"));
	return OMX_ErrorNone;
	}


TBool COmxILClientClockPort::IsTunnelledPortCompatible(const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::IsTunnelledPortCompatible"));
	// Domain check
	if(aPortDefinition.eDomain != GetParamPortDefinition().eDomain)
		{
		return EFalse;
		}
	
	// Format check
	if (aPortDefinition.format.other.eFormat != GetParamPortDefinition().format.other.eFormat)
		{
		return EFalse;
		}
	
	return ETrue;
	}


OMX_ERRORTYPE COmxILClientClockPort::GetMediaTime(OMX_TICKS& aMediaTime) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetMediaTime"));
	return GetTime(aMediaTime, OMX_IndexConfigTimeCurrentMediaTime);
	}


OMX_ERRORTYPE COmxILClientClockPort::GetWallTime(OMX_TICKS& aWallTime) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetWallTime"));
	return GetTime(aWallTime, OMX_IndexConfigTimeCurrentWallTime);
	}


OMX_ERRORTYPE COmxILClientClockPort::GetClockState(OMX_TIME_CONFIG_CLOCKSTATETYPE& aClockState) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetClockState"));

	if (IsClockComponentAvailable())
		{
		aClockState.nSize = sizeof(OMX_TIME_CONFIG_CLOCKSTATETYPE);
		aClockState.nVersion = iSpecVersion;
		aClockState.eState = OMX_TIME_ClockStateMax;
		aClockState.nStartTime = 0;
		aClockState.nOffset = 0;
		aClockState.nWaitMask = 0;
		return OMX_GetConfig(GetTunnelledComponent(), OMX_IndexConfigTimeClockState, &aClockState);
		}

	return OMX_ErrorIncorrectStateOperation;

	}

OMX_ERRORTYPE COmxILClientClockPort::MediaTimeRequest(const OMX_PTR apPrivate, 
															const OMX_TICKS aMediaTime,
															const OMX_TICKS aOffset) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::MediaTimeRequest"));
	OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE timeInfo;
	timeInfo.nSize = sizeof(timeInfo);
	timeInfo.nVersion = iSpecVersion;
	timeInfo.nPortIndex = GetTunnelledPort();
	timeInfo.pClientPrivate = apPrivate;
	timeInfo.nMediaTimestamp = aMediaTime;
	timeInfo.nOffset = aOffset;

	if (IsClockComponentAvailable())
		{
		return OMX_SetConfig(GetTunnelledComponent(), OMX_IndexConfigTimeMediaTimeRequest, &timeInfo);
		}
	
	return OMX_ErrorIncorrectStateOperation;
	}


OMX_ERRORTYPE COmxILClientClockPort::SetStartTime(const OMX_TICKS aStartTime) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::SetStartTime"));
	return SetTime(aStartTime, OMX_IndexConfigTimeClientStartTime);
	}


OMX_ERRORTYPE COmxILClientClockPort::SetVideoReference(const OMX_TICKS aVideoRef) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::SetVideoReference"));
	return SetTime(aVideoRef, OMX_IndexConfigTimeCurrentVideoReference);
	}


OMX_ERRORTYPE COmxILClientClockPort::SetAudioReference(const OMX_TICKS aAudioRef) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::SetAudioReference"));
	return SetTime(aAudioRef, OMX_IndexConfigTimeCurrentAudioReference);
	}

OMX_BOOL COmxILClientClockPort::IsClockComponentAvailable() const
{
	if (GetTunnelledComponent() != NULL && GetParamPortDefinition().bEnabled)
		{
		return OMX_TRUE;
		}
	
	return OMX_FALSE;
}

OMX_ERRORTYPE COmxILClientClockPort::GetTime(OMX_TICKS& aWallTime, const OMX_INDEXTYPE aTimeIndex) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::GetTime"));
	OMX_TIME_CONFIG_TIMESTAMPTYPE timeInfo;
	timeInfo.nSize = sizeof(timeInfo);
	timeInfo.nVersion = iSpecVersion;
	timeInfo.nPortIndex = GetTunnelledPort();

	if (!IsClockComponentAvailable())
		{
		return OMX_ErrorIncorrectStateOperation;
		}
		
	OMX_ERRORTYPE error = OMX_GetConfig(GetTunnelledComponent(), aTimeIndex, &timeInfo);

	if (error == OMX_ErrorNone)
		{
		aWallTime = timeInfo.nTimestamp;
		}
	
	return error;
	}


OMX_ERRORTYPE COmxILClientClockPort::SetTime(const OMX_TICKS aStartTime, const OMX_INDEXTYPE aTimeIndex) const
	{
	DEBUG_PRINTF(_L8("COmxILClientClockPort::SetTime"));
	OMX_TIME_CONFIG_TIMESTAMPTYPE timeInfo;
	timeInfo.nSize = sizeof(timeInfo);
	timeInfo.nVersion = iSpecVersion;
	timeInfo.nPortIndex = GetTunnelledPort();
	timeInfo.nTimestamp = aStartTime;
	
	if (IsClockComponentAvailable())
		{
		return OMX_SetConfig(GetTunnelledComponent(), aTimeIndex, &timeInfo);
		}
		
	return OMX_ErrorIncorrectStateOperation;
	}
