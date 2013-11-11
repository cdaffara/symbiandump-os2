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

#include "log.h"
#include "omxilstate.h"
#include "omxilfsm.h"
#include <openmax/il/common/omxilstatedefs.h>
#include "omxilcommand.h"
#include <openmax/il/common/omxilconfigmanager.h>
#include "omxilcallbackmanager.h"


OMX_ERRORTYPE
COmxILFsm::COmxILState::GetParameter(
	const COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILState::GetParameter"));

	// This API can be used independently of the current state of the
	// component.  Let's try first the Port Manager to check if the index is
	// known there
	OMX_ERRORTYPE retValue =
		aFsm.iPortManager.GetParameter(aParamIndex,
									   apComponentParameterStructure);

	if (OMX_ErrorUnsupportedIndex == retValue)
		{
		// Otherwise, try Config Manager...
		retValue = aFsm.iConfigManager.GetParameter(
			aParamIndex,
			apComponentParameterStructure);
		}

	return retValue;

	}

// This SetParameter version must be used in the following states:
// - OMX_StateIdle,
// - OMX_StateExecuting,
// - OMX_StatePaused, and
//
OMX_ERRORTYPE
COmxILFsm::COmxILState::SetParameter(
	COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILState::SetParameter"));

	OMX_ERRORTYPE retValue =
		aFsm.iPortManager.SetParameter(
			aParamIndex,
			apComponentParameterStructure,
			OMX_TRUE				// Port must be disabled
			);

	if (OMX_ErrorUnsupportedIndex == retValue)
		{
		// Try Config Manager now...
		retValue = aFsm.iConfigManager.SetParameter(
			aParamIndex,
			apComponentParameterStructure,
			OMX_FALSE // run-time
			);
		}

	return retValue;

	}

// This SetParameter version must be used in the following states:
// - OMX_StateLoaded and derived states,
// - OMX_StateWaitForResources
//
OMX_ERRORTYPE
COmxILFsm::COmxILState::SetParameterV2(
	COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILState::SetParameterV2"));

	OMX_ERRORTYPE retValue =
		aFsm.iPortManager.SetParameter(aParamIndex,
									   apComponentParameterStructure);

	if (OMX_ErrorUnsupportedIndex == retValue)
		{
		// Try Config Manager now...
		retValue = aFsm.iConfigManager.SetParameter(
			aParamIndex,
			apComponentParameterStructure);
		}

	return retValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILState::GetConfig(
	const COmxILFsm& aFsm,
	OMX_INDEXTYPE aConfigIndex,
	TAny* apComponentConfigStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILState::GetConfig"));

	OMX_ERRORTYPE retValue =
		aFsm.iPortManager.GetConfig(aConfigIndex,
									apComponentConfigStructure);

	if (OMX_ErrorUnsupportedIndex == retValue)
		{
		// Try Config Manager now...
		retValue = aFsm.iConfigManager.GetConfig(
			aConfigIndex,
			apComponentConfigStructure);
		}

	return retValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILState::SetConfig(COmxILFsm& aFsm,
								  OMX_INDEXTYPE aConfigIndex,
								  const TAny* apComponentConfigStructure)
	{
    DEBUG_PRINTF(_L8("COmxILState::SetConfig"));

	OMX_ERRORTYPE retValue =
		aFsm.iPortManager.SetConfig(aConfigIndex,
									apComponentConfigStructure);

	if (OMX_ErrorUnsupportedIndex == retValue)
		{
		// Try Config Manager now...
		retValue = aFsm.iConfigManager.SetConfig(
			aConfigIndex,
			apComponentConfigStructure);
		}

	return retValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILState::GetExtensionIndex(const COmxILFsm& aFsm,
										  OMX_STRING aParameterName,
										  OMX_INDEXTYPE* apIndexType) const
	{
    DEBUG_PRINTF(_L8("COmxILState::GetExtensionIndex"));

	OMX_ERRORTYPE retValue =
		aFsm.iPortManager.GetExtensionIndex(aParameterName,
											apIndexType);

	if (OMX_ErrorUnsupportedIndex == retValue)
		{
		// Try Config Manager now...
		retValue = aFsm.iConfigManager.GetExtensionIndex(
			aParameterName,
			apIndexType);
		}

	return retValue;

	}

//
//
//

OMX_ERRORTYPE
COmxILFsm::COmxILState::CommandFlush(COmxILFsm& aFsm,
									 const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILState::CommandFlush"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandFlush,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	return aFsm.iPortManager.BufferFlushIndicationFlushCommand(
		aCommand.iParam1);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILState::CommandPortDisable(COmxILFsm& aFsm,
										   const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILState::CommandPortDisable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortDisable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// First, flush all buffers in the port(s) that is(are) to be disabled.
	return aFsm.iPortManager.PortDisableIndication(
		aCommand.iParam1);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILState::CommandPortEnable(COmxILFsm& aFsm,
										  const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILState::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	TBool indicationIsFinal = EFalse;
	return aFsm.iPortManager.PortEnableIndication(
		aCommand.iParam1,
		indicationIsFinal);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILState::CommandMarkBuffer(COmxILFsm& aFsm,
										  const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILState::CommandMarkBuffer"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandMarkBuffer,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!aCommand.ipCommandData)
		{
		return OMX_ErrorBadParameter;
		}

	return aFsm.iPortManager.BufferMarkIndication(aCommand.iParam1,
												  aCommand.ipCommandData);

	}

//
// COmxILStateInvalid
//
OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::GetParameter(
	const COmxILFsm& /*aFsm*/,
	OMX_INDEXTYPE /*aParamIndex*/,
	TAny* /*apComponentParameterStructure*/) const
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::GetParameter"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::SetParameter(
	COmxILFsm& /*aFsm*/,
	OMX_INDEXTYPE /*aParamIndex*/,
	const TAny* /*apComponentParameterStructure*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::SetParameter"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::GetConfig(
	const COmxILFsm& /*aFsm*/,
	OMX_INDEXTYPE /*aConfigIndex*/,
	TAny* /*apComponentConfigStructure*/) const
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::GetConfig"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::SetConfig(
	COmxILFsm& /*aFsm*/,
	OMX_INDEXTYPE /*aConfigIndex*/,
	const TAny* /*apComponentConfigStructure*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::SetConfig"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::GetExtensionIndex(
	const COmxILFsm& /*aFsm*/,
	OMX_STRING /*aParameterName*/,
	OMX_INDEXTYPE* /*apIndexType*/) const
	{
	DEBUG_PRINTF(_L8("COmxILStateInvalid::GetExtensionIndex"));
	return OMX_ErrorInvalidState;
	}

OMX_STATETYPE
COmxILFsm::COmxILStateInvalid::GetState() const
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::GetState"));
	return OMX_StateInvalid;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::ComponentTunnelRequest(
	COmxILFsm& /*aFsm*/,
	OMX_U32 /*aPort*/,
	OMX_HANDLETYPE /*aTunneledComp*/,
	OMX_U32 /*aTunneledPort*/,
	OMX_TUNNELSETUPTYPE* /*apTunnelSetup*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::ComponentTunnelRequest"));

	return OMX_ErrorInvalidState;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::PopulateBuffer(
	COmxILFsm& /*aFsm*/,
	OMX_BUFFERHEADERTYPE** /*appBufferHdr*/,
	OMX_U32 /*aPortIndex*/,
	OMX_PTR /*apAppPrivate*/,
	OMX_U32 /*aSizeBytes*/,
	OMX_U8* /*apBuffer*/,
	TBool& /*portPopulationCompleted*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::PopulateBuffer"));

	return OMX_ErrorInvalidState;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::FreeBuffer(COmxILFsm& aFsm,
										  OMX_U32 aPortIndex,
										  OMX_BUFFERHEADERTYPE* apBuffer,
										  TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::FreeBuffer"));

	return COmxILState::FreeBufferV2(
		aFsm,
		aPortIndex,
		apBuffer,
		aPortDepopulationCompleted);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::EmptyThisBuffer(
	COmxILFsm& /*aFsm*/,
	OMX_BUFFERHEADERTYPE* /*apBuffer*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::EmptyThisBuffer"));

	return OMX_ErrorInvalidState;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::FillThisBuffer(
	COmxILFsm& /*aFsm*/,
	OMX_BUFFERHEADERTYPE* /*apBuffer*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::FillThisBuffer"));

	return OMX_ErrorInvalidState;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::CommandStateSet(
	COmxILFsm& /*aFsm*/,
	const TOmxILCommand& /*aCommand*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::CommandStateSet"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::CommandFlush(
	COmxILFsm& /*aFsm*/,
	const TOmxILCommand& /*aCommand*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::CommandFlush"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::CommandPortEnable(
	COmxILFsm& /*aFsm*/,
	const TOmxILCommand& /*aCommand*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::CommandPortEnable"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::CommandPortDisable(
	COmxILFsm& /*aFsm*/,
	const TOmxILCommand& /*aCommand*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::CommandPortDisable"));
	return OMX_ErrorInvalidState;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateInvalid::CommandMarkBuffer(
	COmxILFsm& /*aFsm*/,
	const TOmxILCommand& /*aCommand*/)
	{
    DEBUG_PRINTF(_L8("COmxILStateInvalid::CommandMarkBuffer"));
	return OMX_ErrorInvalidState;
	}

//
// COmxILStateLoaded
//
OMX_STATETYPE
COmxILFsm::COmxILStateLoaded::GetState() const
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::GetState"));
	return OMX_StateLoaded;
	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::SetParameter(
	COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::SetParameter"));

	return COmxILState::SetParameterV2(aFsm,
									   aParamIndex,
									   apComponentParameterStructure);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::PopulateBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE** appBufferHdr,
	OMX_U32 aPortIndex,
	OMX_PTR apAppPrivate,
	OMX_U32 aSizeBytes,
	OMX_U8* apBuffer,
	TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::PopulateBuffer"));


#ifdef _OMXIL_COMMON_IL516C_ON

	if (!apBuffer)
		{
		// ... AllocateBuffer
		//
		// At this point, the command requesting the transition from Loaded to
		// Idle has not been received yet.. (see
		// COmxILStateLoadedToIdle). Therefore, this can only be successful if
		// the port is disabled
		return COmxILState::PopulateBuffer(aFsm,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);
		}
	else
		{
		//... UseBuffer...
		OMX_ERRORTYPE omxError =
			COmxILState::PopulateBufferV2(aFsm,
										  appBufferHdr,
										  aPortIndex,
										  apAppPrivate,
										  aSizeBytes,
										  apBuffer,
										  portPopulationCompleted);
		if (apBuffer && OMX_ErrorNone == omxError)
			{
			DEBUG_PRINTF2(_L8("COmxILStateLoaded::PopulateBuffer : PORT [%u] : Buffer population occurring in OMX_StateLoaded"), aPortIndex);
			}
		return omxError;
		}
#else

	// At this point, the command requesting the transition from Loaded to Idle
	// has not been received yet.. (see COmxILStateLoadedToIdle). Therefore,
	// this can only be successful if the port is disabled
	return COmxILState::PopulateBuffer(aFsm,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);
#endif

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::FreeBuffer(COmxILFsm& aFsm,
										 OMX_U32 aPortIndex,
										 OMX_BUFFERHEADERTYPE* apBuffer,
										 TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::FreeBuffer"));

	return COmxILState::FreeBuffer(aFsm,
								   aPortIndex,
								   apBuffer,
								   aPortDepopulationCompleted);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::EmptyThisBuffer(COmxILFsm& aFsm,
											  OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::EmptyThisBuffer"));

#ifdef _OMXIL_COMMON_IL516C_ON
	return COmxILState::EmptyThisBufferV2(aFsm,
										apBuffer);
#endif

	return COmxILState::EmptyThisBuffer(aFsm,
										apBuffer);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::FillThisBuffer(COmxILFsm& aFsm,
											 OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::FillThisBuffer"));

#ifdef _OMXIL_COMMON_IL516C_ON
	return COmxILState::FillThisBufferV2(aFsm,
										apBuffer);
#endif

	return COmxILState::FillThisBuffer(aFsm,
									   apBuffer);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::ComponentTunnelRequest(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::ComponentTunnelRequest"));

	return COmxILState::ComponentTunnelRequestV2(aFsm,
												 aPort,
												 aTunneledComp,
												 aTunneledPort,
												 apTunnelSetup);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// The only two transitions allowed are :
	// - OMX_StateIdle and
	// - OMX_StateWaitForResources.
	TStateIndex nextState = EStateMax;
	switch(aCommand.iParam1)
		{
	case OMX_StateIdle:
		{
		nextState = ESubStateLoadedToIdle;
		}
		break;
	case OMX_StateWaitForResources:
		{
		nextState = EStateWaitForResources;
		}
		break;
	case OMX_StateLoaded:
		{
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value..
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}

		};

	// Initial checks OK. The component is commanded to make a transition to
	// OMX_StateIdle or OMX_StateWaitForResources.
	OMX_ERRORTYPE omxRetValue = aFsm.FsmTransition(nextState);
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	if (ESubStateLoadedToIdle == nextState)
		{
		// Lets tell port manager in case there are buffer supplier tunnelled ports
		TBool componentPopulationCompleted = EFalse;
		omxRetValue =
			aFsm.iPortManager.TunnellingBufferAllocation(
				componentPopulationCompleted);
		if (OMX_ErrorNone != omxRetValue)
			{
			return omxRetValue;
			}

		if (componentPopulationCompleted)
			{
			// Complete here the transition to OMX_StateIdle
			omxRetValue = aFsm.FsmTransition(EStateIdle);
			if (OMX_ErrorNone == omxRetValue)
				{
				// Notify the IL client that port population has completed
				// sucessfully
				omxRetValue =
					aFsm.iCallbacks.TransitionCompleteNotification(
						OMX_StateIdle);
				}

			}

		}
	else
		{
		// Notify the IL client that the transition to
		// OMX_StateWaitForResources has completed sucessfully
		omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
			OMX_StateWaitForResources);

		}

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoaded::CommandPortEnable(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoaded::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	TBool indicationIsFinal = ETrue;
	return aFsm.iPortManager.PortEnableIndication(
		aCommand.iParam1,
		indicationIsFinal);

	}


//
// COmxILStateLoadedToIdle
//
OMX_ERRORTYPE
COmxILFsm::COmxILStateLoadedToIdle::PopulateBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE** appBufferHdr,
	OMX_U32 aPortIndex,
	OMX_PTR apAppPrivate,
	OMX_U32 aSizeBytes,
	OMX_U8* apBuffer,
	TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoadedToIdle::PopulateBuffer"));

	OMX_ERRORTYPE omxError =
		COmxILState::PopulateBufferV2(aFsm,
									  appBufferHdr,
									  aPortIndex,
									  apAppPrivate,
									  aSizeBytes,
									  apBuffer,
									  portPopulationCompleted);

	if (apBuffer && OMX_ErrorNone == omxError)
		{
		DEBUG_PRINTF2(_L8("COmxILStateLoadedToIdle::PopulateBuffer : PORT [%u] : Buffer population occurring in state LoadedToIdle"), aPortIndex);
		}

	return omxError;

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateLoadedToIdle::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoadedToIdle::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	switch(aCommand.iParam1)
		{
	case OMX_StateWaitForResources:
		{
		// Not implemented for now...
		return OMX_ErrorNotImplemented;
		}
	case OMX_StateLoaded:
		{
		// Here, return "same state" as the transition to OMX_StateLoaded has
		// not completed yet.
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateLoadedToIdle::CommandPortEnable(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateLoadedToIdle::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	TBool indicationIsFinal = EFalse;
	OMX_ERRORTYPE omxRetValue =
		aFsm.iPortManager.PortEnableIndication(
			aCommand.iParam1,
			indicationIsFinal);
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	// Lets tell port manager in case the port being enabled is a tunnel
	// supplier...
	TBool componentPopulationCompleted = EFalse;
	omxRetValue =
		aFsm.iPortManager.TunnellingBufferAllocation(
			componentPopulationCompleted, aCommand.iParam1);
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	// Let's do this check here although this situation is going to be very
	// unlikely...
	if (componentPopulationCompleted)
		{
		// Complete here the transition to OMX_StateIdle
		omxRetValue = aFsm.FsmTransition(EStateIdle);
		if (OMX_ErrorNone == omxRetValue)
			{
			// Notify the IL client that port population has completed sucessfully
			aFsm.iCallbacks.TransitionCompleteNotification(
				OMX_StateIdle);
			}

		}

	return omxRetValue;

	}


//
// COmxILStateWaitForResources
//
OMX_STATETYPE
COmxILFsm::COmxILStateWaitForResources::GetState() const
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::GetState"));
	return OMX_StateWaitForResources;
	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::SetParameter(
	COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::SetParameter"));

	return COmxILState::SetParameterV2(aFsm,
									   aParamIndex,
									   apComponentParameterStructure);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::PopulateBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE** appBufferHdr,
	OMX_U32 aPortIndex,
	OMX_PTR apAppPrivate,
	OMX_U32 aSizeBytes,
	OMX_U8* apBuffer,
	TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::PopulateBuffer"));

	// NOTE that according to the spec, PopulateBuffer could be used here even
	// if the port is enabled. However, for now the transition from
	// OMX_StateWaitForResouces -> OMX_StateIdle is not supported, therefore
	// buffer population is only allowed for disabled ports. This should be
	// changed once Resource Management functionality is available and this
	// state is revisited.
	return COmxILState::PopulateBuffer(aFsm,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::FreeBuffer(
	COmxILFsm& aFsm,
	OMX_U32 aPortIndex,
	OMX_BUFFERHEADERTYPE* apBuffer,
	TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::FreeBuffer"));

	return COmxILState::FreeBuffer(aFsm,
								   aPortIndex,
								   apBuffer,
								   aPortDepopulationCompleted);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::EmptyThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::EmptyThisBuffer"));

#ifdef _OMXIL_COMMON_IL516C_ON
	return COmxILState::EmptyThisBufferV2(aFsm,
										apBuffer);
#endif

	return COmxILState::EmptyThisBuffer(aFsm,
										apBuffer);


	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::FillThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::FillThisBuffer"));

#ifdef _OMXIL_COMMON_IL516C_ON
	return COmxILState::FillThisBufferV2(aFsm,
										apBuffer);
#endif

	return COmxILState::FillThisBuffer(aFsm,
									   apBuffer);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::ComponentTunnelRequest(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::ComponentTunnelRequest"));

	return COmxILState::ComponentTunnelRequest(aFsm,
											   aPort,
											   aTunneledComp,
											   aTunneledPort,
											   apTunnelSetup);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// Transition to OMX_StateIdle not implemented for now...
	TStateIndex nextState = EStateMax;
	switch(aCommand.iParam1)
		{
	case OMX_StateLoaded:
		{
		nextState = EStateLoaded;
		}
		break;
	case OMX_StateWaitForResources:
		{
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};

	// Initial checks OK. The component is commanded to make a transition to
	// OMX_StateLoaded
	OMX_ERRORTYPE omxRetValue = aFsm.FsmTransition(nextState);
	if (OMX_ErrorNone == omxRetValue)
		{
		// Notify the IL client that the transition to
		// OMX_StateLoaded has completed sucessfully
		omxRetValue =
			aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateLoaded);
		}

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateWaitForResources::CommandPortEnable(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateWaitForResources::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

    // NOTE: Here, we only care about the port enabled flag. Transition from
    // OMX_StateWaitForResources to OMX_StateIdle is not implemented at this
    // stage until a Resource Manager is available. Whenever a Resource Manager
    // becomes available, a COmxILStateWaitForResourcesToIdle substate should
    // be implemented to handle the allocation of buffers mandated by the
    // standard when a port is enabled in this transition.
	TBool indicationIsFinal = ETrue;
	return aFsm.iPortManager.PortEnableIndication(
		aCommand.iParam1,
		indicationIsFinal);

	}


//
// COmxILStateIdle
//
OMX_STATETYPE
COmxILFsm::COmxILStateIdle::GetState() const
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::GetState"));
	return OMX_StateIdle;
	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::SetParameter(
	COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::SetParameter"));

	return COmxILState::SetParameter(aFsm,
									 aParamIndex,
									 apComponentParameterStructure);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::PopulateBuffer(COmxILFsm& aFsm,
										   OMX_BUFFERHEADERTYPE** appBufferHdr,
										   OMX_U32 aPortIndex,
										   OMX_PTR apAppPrivate,
										   OMX_U32 aSizeBytes,
										   OMX_U8* apBuffer,
										   TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::PopulateBuffer"));

	// At this point, no buffer population can take place unless the port is disabled
	return COmxILState::PopulateBuffer(aFsm,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::FreeBuffer(COmxILFsm& aFsm,
									   OMX_U32 aPortIndex,
									   OMX_BUFFERHEADERTYPE* apBuffer,
									   TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::FreeBuffer"));

	// At this point, the command requesting the transition from Idle to Loaded
	// has not been received yet.. (see COmxILStateIdleToLoaded). Therefore,
	// this can only be successful if the port is disabled
	return COmxILState::FreeBuffer(aFsm,
								   aPortIndex,
								   apBuffer,
								   aPortDepopulationCompleted);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::EmptyThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::EmptyThisBuffer"));

	return COmxILState::EmptyThisBufferV2(aFsm,
										  apBuffer);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::FillThisBuffer(COmxILFsm& aFsm,
										   OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::FillThisBuffer"));

	return COmxILState::FillThisBufferV2(aFsm,
										 apBuffer);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::ComponentTunnelRequest(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::ComponentTunnelRequest"));

	return COmxILState::ComponentTunnelRequest(aFsm,
											   aPort,
											   aTunneledComp,
											   aTunneledPort,
											   apTunnelSetup);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	TStateIndex nextState = EStateInvalid;
	switch(aCommand.iParam1)
		{
	case OMX_StateLoaded:
		{
		nextState = ESubStateIdleToLoaded;
		}
		break;
	case OMX_StateExecuting:
		{
		nextState = EStateExecuting;
		}
		break;
	case OMX_StatePause:
		{
		nextState = EStatePause;
		}
		break;
	case OMX_StateIdle:
		{
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};

	// Initial command checks OK

	// Commit transition to the valid state
	OMX_ERRORTYPE omxRetValue = aFsm.FsmTransition(nextState);
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	if (ESubStateIdleToLoaded == nextState)
		{
		// Lets tell port manager in case there are tunnelled ports...
		TBool componentDepopulationCompleted = EFalse;
		omxRetValue =
			aFsm.iPortManager.TunnellingBufferDeallocation(
				componentDepopulationCompleted);
		if (OMX_ErrorNone != omxRetValue)
			{
			return omxRetValue;
			}

		if (componentDepopulationCompleted)
			{
			// Complete here the transition to OMX_StateLoaded
			omxRetValue = aFsm.FsmTransition(EStateLoaded);
			if (OMX_ErrorNone == omxRetValue)
				{
				// Notify the IL client that port population has completed sucessfully
				omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
					OMX_StateLoaded);
				}
			}

		}
	else
		{
		// Notify the IL client that the transition to the valid state
		// OMX_StatePause or OMX_StateExecuting has completed sucessfully
		omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
			static_cast<OMX_STATETYPE>(aCommand.iParam1));

		if (OMX_ErrorNone == omxRetValue)
			{
			// Fire up the tunnelled buffer exchange, if any tunnelled ports are
			// found in the component...
			omxRetValue = aFsm.iPortManager.InitiateTunnellingDataFlow();
			}

		}

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::CommandFlush(COmxILFsm& aFsm,
										 const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::CommandFlush"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandFlush,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	return aFsm.iPortManager.BufferFlushIndicationFlushCommand(
		aCommand.iParam1, OMX_FALSE); // Do not eject buffers

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateIdle::CommandPortEnable(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdle::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// In this state, the port allocation has finished for enabled
	// ports. However, a port is being enabled here. The port being enabled
	// must populate (if tunnel supplier) or be populated (if non-tunnel
	// supplier or IL Client communication)
	TBool indicationIsFinal = EFalse;
	OMX_ERRORTYPE omxRetValue =
		aFsm.iPortManager.PortEnableIndication(
			aCommand.iParam1,
			indicationIsFinal);

	if (OMX_ErrorNone == omxRetValue)
		{
		// From section 3.2.2.6, "If the IL client enables a port while the
		// component is in any state other than OMX_StateLoaded or
		// OMX_WaitForResources, then that port shall allocate its buffers via
		// the same call sequence used on a transition from OMX_StateLoaded to
		// OMX_StateIdle."

		// Lets tell port manager in case the port being enabled is a tunnel
		// supplier...
		TBool componentPopulationCompleted = EFalse;
		omxRetValue =
			aFsm.iPortManager.TunnellingBufferAllocation(
				componentPopulationCompleted, aCommand.iParam1);
		}

	return omxRetValue;

	}


//
// COmxILStateIdleToLoaded
//
OMX_ERRORTYPE
COmxILFsm::COmxILStateIdleToLoaded::FreeBuffer(COmxILFsm& aFsm,
											   OMX_U32 aPortIndex,
											   OMX_BUFFERHEADERTYPE* apBuffer,
											   TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdleToLoaded::FreeBuffer"));

	return COmxILState::FreeBufferV2(aFsm,
									 aPortIndex,
									 apBuffer,
									 aPortDepopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateIdleToLoaded::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateIdleToLoaded::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	switch (aCommand.iParam1)
		{
	case OMX_StateIdle:
		{
		// Here, return "same state" as the transition to OMX_StateLoaded has
		// not completed yet.
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		__ASSERT_ALWAYS(0, User::Panic(KOmxILFsmPanicCategory, 1));
		}
		};

	return OMX_ErrorNone;

	}


//
// COmxILStateExecuting
//
OMX_STATETYPE
COmxILFsm::COmxILStateExecuting::GetState() const
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::GetState"));
	return OMX_StateExecuting;
	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::SetParameter(COmxILFsm& aFsm,
											  OMX_INDEXTYPE aParamIndex,
											  const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::SetParameter"));

	return COmxILState::SetParameter(aFsm,
									 aParamIndex,
									 apComponentParameterStructure);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::PopulateBuffer(COmxILFsm& aFsm,
												OMX_BUFFERHEADERTYPE** appBufferHdr,
												OMX_U32 aPortIndex,
												OMX_PTR apAppPrivate,
												OMX_U32 aSizeBytes,
												OMX_U8* apBuffer,
												TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::PopulateBuffer"));

	return COmxILState::PopulateBuffer(aFsm,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::FreeBuffer(COmxILFsm& aFsm,
											OMX_U32 aPortIndex,
											OMX_BUFFERHEADERTYPE* apBuffer,
											TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::FreeBuffer"));

	return COmxILState::FreeBuffer(aFsm,
								   aPortIndex,
								   apBuffer,
								   aPortDepopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::EmptyThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::EmptyThisBuffer"));

	return COmxILState::EmptyThisBufferV2(aFsm,
										  apBuffer);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::FillThisBuffer(COmxILFsm& aFsm,
												OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::FillThisBuffer"));

	return COmxILState::FillThisBufferV2(aFsm,
										 apBuffer);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::ComponentTunnelRequest(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::ComponentTunnelRequest"));

	return COmxILState::ComponentTunnelRequest(aFsm,
											   aPort,
											   aTunneledComp,
											   aTunneledPort,
											   apTunnelSetup);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	TStateIndex nextState = EStateMax;
	switch(aCommand.iParam1)
		{
	case OMX_StateIdle:
		{
		nextState = ESubStateExecutingToIdle;
		}
		break;
	case OMX_StatePause:
		{
		nextState = EStatePause;
		}
		break;
	case OMX_StateExecuting:
		{
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};


	// Initial checks OK. The component is commanded to make a transition to
	// ESubStateExecutingToIdle substate or OMX_StatePause.
	OMX_ERRORTYPE omxRetValue = aFsm.FsmTransition(nextState);
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	if (ESubStateExecutingToIdle == nextState)
		{
		// Lets tell port manager since at this point it is mandated that all
		// buffers must be returned to their suppliers (IL Client and/or
		// tunnelled components).
		TBool allBuffersReturnedToSuppliers = EFalse;
		omxRetValue =
			aFsm.iPortManager.BufferFlushIndicationPauseOrExeToIdleCommand(
				allBuffersReturnedToSuppliers);
		if (OMX_ErrorNone != omxRetValue)
			{
			return omxRetValue;
			}

		if (allBuffersReturnedToSuppliers)
			{
			// Complete here the transition to OMX_StateIdle
			omxRetValue = aFsm.FsmTransition(EStateIdle);
			if (OMX_ErrorNone == omxRetValue)
				{
				// Notify the IL client that port population has completed sucessfully
				omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
					OMX_StateIdle);
				}
			}

		}
	else
		{
		// Notify the IL client that the transition to OMX_StatePause has
		// completed sucessfully
		omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
			OMX_StatePause);

		}

	return omxRetValue;

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecuting::CommandPortEnable(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecuting::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// In this state, the port allocation has finished for enabled
	// ports. However, a port is being enabled here. The port being enabled
	// must populate (if tunnel supplier) or be populated (if non-tunnel
	// supplier or IL Client communication)
	TBool indicationIsFinal = EFalse;
	OMX_ERRORTYPE omxRetValue =
		aFsm.iPortManager.PortEnableIndication(
			aCommand.iParam1,
			indicationIsFinal);

	if (OMX_ErrorNone == omxRetValue)
		{
		// From section 3.2.2.6, "If the IL client enables a port while the
		// component is in any state other than OMX_StateLoaded or
		// OMX_WaitForResources, then that port shall allocate its buffers via
		// the same call sequence used on a transition from OMX_StateLoaded to
		// OMX_StateIdle. If the IL client enables while the component is in
		// the OMX_Executing state, then that port shall begin transferring
		// buffers"

		// Lets tell port manager in case the port being enabled is a tunnel
		// supplier...
		TBool componentPopulationCompleted = EFalse;
		omxRetValue =
			aFsm.iPortManager.TunnellingBufferAllocation(
				componentPopulationCompleted, aCommand.iParam1);

		// No need to use here componentPopulationCompleted. A port is being
		// enabled in OMX_StateExecuting. If the port is a supplier, after
		// being re-enabled it should start the buffer allocation phase,
		// regardless of the population state of other ports in the component.
		if (OMX_ErrorNone == omxRetValue)
			{
			// Fire up the tunnelled buffer exchange in the enabled port...
			omxRetValue = aFsm.iPortManager.InitiateTunnellingDataFlow(
				aCommand.iParam1);
			}
		}

	return omxRetValue;

	}


//
// MOmxILPauseOrExecutingToIdle
//
OMX_ERRORTYPE
COmxILFsm::MOmxILPauseOrExecutingToIdle::ReturnThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer,
	OMX_DIRTYPE aDirection)
	{
    DEBUG_PRINTF(_L8("MOmxILPauseOrExecutingToIdle::ReturnThisBuffer"));

	TBool allBuffersReturned = EFalse;
	OMX_ERRORTYPE omxRetValue =
		aFsm.iPortManager.BufferReturnIndication(
			apBuffer,
			aDirection,
			allBuffersReturned
			);

	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	if (allBuffersReturned &&
		aFsm.iPortManager.AllBuffersAtHome())
		{
		// Complete here the transition to OMX_StateIdle
		omxRetValue = aFsm.FsmTransition(EStateIdle);
		if (OMX_ErrorNone == omxRetValue)
			{
			// Notify the IL client that transition to Idle has completed
			// sucessfully
			omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
				OMX_StateIdle);
			}
		}

	return omxRetValue;

	}



//
// COmxILStateExecutingToIdle
//
OMX_ERRORTYPE
COmxILFsm::COmxILStateExecutingToIdle::EmptyThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecutingToIdle::EmptyThisBuffer"));

	return ReturnThisBuffer(aFsm,
							apBuffer,
							OMX_DirInput);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecutingToIdle::FillThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecutingToIdle::FillThisBuffer"));

	return ReturnThisBuffer(aFsm,
							apBuffer,
							OMX_DirOutput);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStateExecutingToIdle::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStateExecutingToIdle::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	switch(aCommand.iParam1)
		{
	case OMX_StateExecuting:
		{
		// Here, return "same state" as the transition to OMX_StateIdle has
		// not completed yet.
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};

	}


//
// COmxILStatePause
//
OMX_STATETYPE
COmxILFsm::COmxILStatePause::GetState() const
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::GetState"));
	return OMX_StatePause;
	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::SetParameter(
	COmxILFsm& aFsm,
	OMX_INDEXTYPE aParamIndex,
	const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::SetParameter"));

	return COmxILState::SetParameter(aFsm,
									 aParamIndex,
									 apComponentParameterStructure);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::PopulateBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE** appBufferHdr,
	OMX_U32 aPortIndex,
	OMX_PTR apAppPrivate,
	OMX_U32 aSizeBytes,
	OMX_U8* apBuffer,
	TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::PopulateBuffer"));

	return COmxILState::PopulateBuffer(aFsm,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::FreeBuffer(COmxILFsm& aFsm,
										OMX_U32 aPortIndex,
										OMX_BUFFERHEADERTYPE* apBuffer,
										TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::FreeBuffer"));

	return COmxILState::FreeBuffer(aFsm,
								   aPortIndex,
								   apBuffer,
								   aPortDepopulationCompleted);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::EmptyThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::EmptyThisBuffer"));

	return COmxILState::EmptyThisBufferV2(aFsm,
										  apBuffer);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::FillThisBuffer(COmxILFsm& aFsm,
											OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::FillThisBuffer"));

	return COmxILState::FillThisBufferV2(aFsm,
										 apBuffer);

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::ComponentTunnelRequest(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::ComponentTunnelRequest"));

	return COmxILState::ComponentTunnelRequest(aFsm,
											   aPort,
											   aTunneledComp,
											   aTunneledPort,
											   apTunnelSetup);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	TStateIndex nextState = EStateInvalid;
	switch(aCommand.iParam1)
		{
	case OMX_StateIdle:
		{
		nextState = ESubStatePauseToIdle;
		}
		break;
	case OMX_StateExecuting:
		{
		nextState = EStateExecuting;
		}
		break;
	case OMX_StatePause:
		{
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};

	// Initial command checks OK. The component is commanded to make a
	// transition to ESubStatePauseToIdle substate or OMX_StateExecuting.
	// Commit transition to the valid state
	OMX_ERRORTYPE omxRetValue = aFsm.FsmTransition(nextState);
	if (OMX_ErrorNone != omxRetValue)
		{
		return omxRetValue;
		}

	if (ESubStatePauseToIdle == nextState)
		{
		// Lets tell port manager since at this point it is mandated that all
		// buffers must be returned to their suppliers (IL Client and/or
		// tunnelled components).
		TBool allBuffersReturnedToSuppliers = EFalse;
		omxRetValue =
			aFsm.iPortManager.BufferFlushIndicationPauseOrExeToIdleCommand(
				allBuffersReturnedToSuppliers);
		if (OMX_ErrorNone != omxRetValue)
			{
			return omxRetValue;
			}

		if (allBuffersReturnedToSuppliers)
			{
			// Complete here the transition to OMX_StateIdle
			omxRetValue = aFsm.FsmTransition(EStateIdle);
			if (OMX_ErrorNone == omxRetValue)
				{
				// Notify the IL client that port population has completed sucessfully
				aFsm.iCallbacks.TransitionCompleteNotification(
					OMX_StateIdle);
				}
			}

		}
	else
		{
		// Notify the IL client that the transition to OMX_StateExecuting has
		// completed sucessfully
		omxRetValue = aFsm.iCallbacks.TransitionCompleteNotification(
			OMX_StateExecuting);

		if (OMX_ErrorNone == omxRetValue)
			{
			// Fire up the tunnelled buffer exchange, if any tunnelled ports are
			// found in the component...
			omxRetValue = aFsm.iPortManager.InitiateTunnellingDataFlow();
			}

		}

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILFsm::COmxILStatePause::CommandPortEnable(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStatePause::CommandPortEnable"));
	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandPortEnable,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// In this state, the port allocation has finished for enabled
	// ports. However, a port is being enabled here. The port being enabled
	// must populate (if tunnel supplier) or be populated (if non-tunnel
	// supplier or IL Client communication)
	TBool indicationIsFinal = EFalse;
	OMX_ERRORTYPE omxRetValue = aFsm.iPortManager.PortEnableIndication(
		aCommand.iParam1,
		indicationIsFinal);

	if (OMX_ErrorNone == omxRetValue)
		{
		// From section 3.2.2.6, "If the IL client enables a port while the
		// component is in any state other than OMX_StateLoaded or
		// OMX_WaitForResources, then that port shall allocate its buffers via
		// the same call sequence used on a transition from OMX_StateLoaded to
		// OMX_StateIdle."

		// Lets tell port manager in case the port being enabled is a tunnel
		// supplier...
		TBool componentPopulationCompleted = EFalse;
		omxRetValue =
			aFsm.iPortManager.TunnellingBufferAllocation(
				componentPopulationCompleted, aCommand.iParam1);

		}

	return omxRetValue;

	}


//
// COmxILStatePauseToIdle
//
OMX_ERRORTYPE
COmxILFsm::COmxILStatePauseToIdle::EmptyThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStatePauseToIdle::EmptyThisBuffer"));

	return ReturnThisBuffer(aFsm,
							apBuffer,
							OMX_DirInput);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePauseToIdle::FillThisBuffer(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILStatePauseToIdle::FillThisBuffer"));

	return ReturnThisBuffer(aFsm,
							apBuffer,
							OMX_DirOutput);

	}


OMX_ERRORTYPE
COmxILFsm::COmxILStatePauseToIdle::CommandStateSet(
	COmxILFsm& aFsm,
	const TOmxILCommand& aCommand)
	{
    DEBUG_PRINTF(_L8("COmxILStatePauseToIdle::CommandStateSet"));

	__ASSERT_DEBUG(aCommand.iCommandType == OMX_CommandStateSet,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	switch(aCommand.iParam1)
		{
	case OMX_StatePause:
		{
		// Here, return "same state" as the transition to OMX_StateIdle has not
		// completed yet.
		return OMX_ErrorSameState;
		}
	case OMX_StateInvalid:
		{
		// Notify the IL client... ignore the ret value...
		aFsm.iCallbacks.TransitionCompleteNotification(OMX_StateInvalid);
		// Invalidate component
		return OMX_ErrorInvalidState;
		}
	default:
		{
		return OMX_ErrorIncorrectStateTransition;
		}
		};

	}


