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
#include "omxilfsm.h"
#include "omxilstate.h"
#include <openmax/il/common/omxilconfigmanager.h>
#include "omxilcallbackmanager.h"
#include <openmax/il/common/omxilprocessingfunction.h>
#include "omxilportmanagerif.h"
#include "omxilcommand.h"
#include <openmax/il/common/omxilstatedefs.h>

#define RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(_a)						\
	{																	\
	const OMX_ERRORTYPE _err = _a;										\
	if (OMX_ErrorNone == _err) return _err;								\
	else return SendOmxErrorEventIfNeeded(_err);						\
	}


const TInt COmxILFsm::KMaxMsgQueueEntries;

COmxILFsm*
COmxILFsm::NewL(COmxILComponent& aComponent,
				COmxILProcessingFunction& aProcFunction,
				MOmxILPortManagerIf& aPortManager,
				COmxILConfigManager& aConfigManager,
				MOmxILCallbackManagerIf& aCallbacks)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::NewLC"));

	COmxILFsm* self = new (ELeave) COmxILFsm(aComponent,
											 aProcFunction,
											 aPortManager,
											 aConfigManager,
											 aCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (self);
	}

void
COmxILFsm::ConstructL()
	{
    DEBUG_PRINTF(_L8("COmxILFsm::ConstructL"));

	// Create the FSM states

	// Init the array
	for (TUint i=0; i < EStateMax; ++i)
		{
		iStates.AppendL(NULL);
		}

	// Add the standard states...
	iStates[EStateInvalid]			= new (ELeave)COmxILStateInvalid;
	iStates[EStateLoaded]			= new (ELeave)COmxILStateLoaded;
	iStates[EStateIdle]				= new (ELeave)COmxILStateIdle;
	iStates[EStateExecuting]		= new (ELeave)COmxILStateExecuting;
	iStates[EStatePause]			= new (ELeave)COmxILStatePause;
	iStates[EStateWaitForResources] = new (ELeave)COmxILStateWaitForResources;

	// Now add the substates
	iStates[ESubStateLoadedToIdle]	  = new (ELeave)COmxILStateLoadedToIdle;
	iStates[ESubStateIdleToLoaded]	  = new (ELeave)COmxILStateIdleToLoaded;
	iStates[ESubStateExecutingToIdle] = new (ELeave)COmxILStateExecutingToIdle;
	iStates[ESubStatePauseToIdle]	  = new (ELeave)COmxILStatePauseToIdle;

	iCallbacks.SetPortManager(iPortManager);
	iCallbacks.SetFsm(*this);

	}

COmxILFsm::COmxILFsm(COmxILComponent& aComponent,
					 COmxILProcessingFunction& aProcFunction,
					 MOmxILPortManagerIf& aPortManager,
					 COmxILConfigManager& aConfigManager,
					 MOmxILCallbackManagerIf& aCallbacks)
	:
	iComponent(aComponent),
	iProcFunction(aProcFunction),
	iPortManager(aPortManager),
	iConfigManager(aConfigManager),
	iCallbacks(aCallbacks),
	iStates(),
	iCurrentStateIndex(EStateMax),
	ipCurrentState(0)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::COmxILFsm"));
	}

COmxILFsm::~COmxILFsm()
	{
    DEBUG_PRINTF(_L8("COmxILFsm::~COmxILFsm"));

	iCurrentStateIndex = EStateMax;
	ipCurrentState = 0;
	iStates.ResetAndDestroy();

	}

OMX_ERRORTYPE
COmxILFsm::InitFsm()
	{
    DEBUG_PRINTF(_L8("COmxILFsm::InitFsm"));

	// Let's get ready to handle API calls...
	iCurrentStateIndex = EStateLoaded;
	ipCurrentState	   = iStates[iCurrentStateIndex];
	return OMX_ErrorNone;

	}

COmxILComponent*
COmxILFsm::GetComponent() const
	{
	return &iComponent;
	}

OMX_ERRORTYPE
COmxILFsm::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
						  OMX_U32 aPortIndex,
						  OMX_PTR apAppPrivate,
						  OMX_U32 aSizeBytes,
						  OMX_U8* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::PopulateBuffer"));

	TBool portPopulationCompleted = EFalse;
	OMX_ERRORTYPE omxRetValue =
		ipCurrentState->PopulateBuffer(*this,
									   appBufferHdr,
									   aPortIndex,
									   apAppPrivate,
									   aSizeBytes,
									   apBuffer,
									   portPopulationCompleted);

	if (OMX_ErrorNone == omxRetValue)
		{
		if (portPopulationCompleted &&
			ESubStateLoadedToIdle == iCurrentStateIndex &&
			iPortManager.AllPortsPopulated())
			{
			// Complete here the transition to OMX_StateIdle
			omxRetValue = FsmTransition(EStateIdle);
			if (OMX_ErrorNone == omxRetValue)
				{
				// Notify the IL client that port population has
				// completed sucessfully
				omxRetValue = iCallbacks.TransitionCompleteNotification(
					OMX_StateIdle);

				}
			}
		}

	if (OMX_ErrorNone == omxRetValue ||
		OMX_ErrorInsufficientResources == omxRetValue)
		{
		// OMX_ErrorInsufficientResources is allowed in OMX_EmptyThisBuffer and
		// OMX_FillThisBuffer
		return omxRetValue;
		}
	else
		{
		return SendOmxErrorEventIfNeeded(omxRetValue);
		}

	}

OMX_ERRORTYPE
COmxILFsm::FsmTransition(TStateIndex aNewState)
	{
    DEBUG_PRINTF2(_L8("COmxILFsm::FsmTransition : %d"), aNewState);

	__ASSERT_ALWAYS(aNewState < EStateMax,
					User::Panic(KOmxILFsmPanicCategory, 1));

	if (aNewState != iCurrentStateIndex)
		{
		// We notify the processing function of all the state transitions, even
		// if they are not to a final OpenMAX IL state.
		OMX_ERRORTYPE omxRetValue;
		if (OMX_ErrorNone !=
			(omxRetValue =
			 iProcFunction.StateTransitionIndication(aNewState)))
			{
			// No need of propagating further error codes if the component is
			// transitioning to OMX_StateInvalid or if the PF itself is
			// invalidating the component...
			if (EStateInvalid != aNewState &&
				OMX_ErrorInvalidState != omxRetValue)
				{
				return omxRetValue;
				}
			}

		iCurrentStateIndex = aNewState;
		ipCurrentState = iStates[iCurrentStateIndex];

		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILFsm::FsmTransition(TUint32 aNewState)
	{

	return FsmTransition(static_cast<TStateIndex>(aNewState));

	}


OMX_ERRORTYPE
COmxILFsm::GetComponentVersion(OMX_STRING aComponentName,
							   OMX_VERSIONTYPE* apComponentVersion,
							   OMX_VERSIONTYPE* apSpecVersion,
							   OMX_UUIDTYPE* apComponentUUID) const
	{
    DEBUG_PRINTF(_L8("COmxILFsm::GetComponentVersion"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// This api should not be allowed in OMX_StateInvalid
	if (EStateInvalid == iCurrentStateIndex)
		{
		return SendOmxErrorEventIfNeeded(OMX_ErrorInvalidState);
		}

	if (!aComponentName ||
		!apComponentVersion ||
		!apSpecVersion ||
		!apComponentUUID)
		{
		return OMX_ErrorBadParameter;
		}

	// This API call is independent of the current state. Its handled by the
	// the config manager

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		iConfigManager.GetComponentVersion(
			aComponentName,
			apComponentVersion,
			apSpecVersion,
			apComponentUUID));
	}


OMX_ERRORTYPE
COmxILFsm::SendCommand(OMX_COMMANDTYPE aCommand,
					   TUint32 anParam1,
					   TAny* apCmdData)
	{
    DEBUG_PRINTF3(_L8("COmxILFsm::SendCommand : command [%d] Param1 [%d]"), aCommand, anParam1);

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// Do some very minor error checking here to try to save some time...
	if (OMX_CommandStateSet == aCommand &&
		anParam1 > OMX_StateWaitForResources)
		{
		return OMX_ErrorBadParameter;
		}

	TOmxILCommand command(aCommand, anParam1, apCmdData);
	OMX_ERRORTYPE sendCommandError;
	switch (aCommand)
		{
	case OMX_CommandStateSet:
		{
		sendCommandError = ipCurrentState->CommandStateSet(*this, command);
		}
		break;
	case OMX_CommandFlush:
		{
		sendCommandError = ipCurrentState->CommandFlush(*this, command);
		}
		break;
	case OMX_CommandPortDisable:
		{
		sendCommandError = ipCurrentState->CommandPortDisable(*this, command);
		}
		break;
	case OMX_CommandPortEnable:
		{
		sendCommandError = ipCurrentState->CommandPortEnable(*this, command);
		}
		break;
	case OMX_CommandMarkBuffer:
		{
		sendCommandError = ipCurrentState->CommandMarkBuffer(*this, command);
		}
		break;
	default:
		{
		// This is an invalid command type
		return OMX_ErrorBadParameter;
		}
		};

	if (OMX_ErrorNone == sendCommandError ||
		OMX_ErrorInsufficientResources == sendCommandError)
		{
		// OMX_ErrorInsufficientResources is allowed in OMX_SendCommand
		return sendCommandError;
		}
	else
		{
		return SendOmxErrorEventIfNeeded(sendCommandError);
		}

	}


OMX_ERRORTYPE
COmxILFsm::GetParameter(OMX_INDEXTYPE aParamIndex,
						TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILFsm::GetParameter"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apComponentParameterStructure)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->GetParameter(*this, aParamIndex,
									 apComponentParameterStructure));
	}


OMX_ERRORTYPE
COmxILFsm::SetParameter(OMX_INDEXTYPE aParamIndex,
						const TAny* apComponentParameterStructure)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::SetParameter"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apComponentParameterStructure)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
	ipCurrentState->SetParameter(*this, aParamIndex,
								 apComponentParameterStructure));
	}


OMX_ERRORTYPE
COmxILFsm::GetConfig(OMX_INDEXTYPE aConfigIndex,
					 TAny* apComponentConfigStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILFsm::GetConfig"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apComponentConfigStructure)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->GetConfig(*this,
								  aConfigIndex,
								  apComponentConfigStructure));

	}


OMX_ERRORTYPE
COmxILFsm::SetConfig(OMX_INDEXTYPE aIndex,
					 const TAny* apComponentConfigStructure)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::SetConfig"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apComponentConfigStructure)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->SetConfig(*this, aIndex, apComponentConfigStructure));

	}


OMX_ERRORTYPE
COmxILFsm::GetExtensionIndex(
	OMX_STRING aParameterName,
	OMX_INDEXTYPE* apIndexType) const
	{
    DEBUG_PRINTF(_L8("COmxILFsm::GetExtensionIndex"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apIndexType || !aParameterName)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->GetExtensionIndex(*this,
										  aParameterName,
										  apIndexType));
	}


OMX_ERRORTYPE
COmxILFsm::GetState(OMX_STATETYPE* apState) const
	{
    DEBUG_PRINTF(_L8("COmxILFsm::GetState"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apState)
		{
		return OMX_ErrorBadParameter;
		}

	*apState = ipCurrentState->GetState();

	return OMX_ErrorNone;

	}


OMX_ERRORTYPE
COmxILFsm::ComponentTunnelRequest(OMX_U32 aPort,
								  OMX_HANDLETYPE aTunneledComp,
								  OMX_U32 aTunneledPort,
								  OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::ComponentTunnelRequest"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// Here, since NULL is a valid parameter for aTunneledComp, checking of
	// input parameters is completely done by the ports.
	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->ComponentTunnelRequest(*this,
											   aPort,
											   aTunneledComp,
											   aTunneledPort,
											   apTunnelSetup));

	}


OMX_ERRORTYPE
COmxILFsm::UseBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
					 OMX_U32 aPortIndex,
					 OMX_PTR apAppPrivate,
					 OMX_U32 aSizeBytes,
					 OMX_U8* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::UseBuffer"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!appBufferHdr || !aSizeBytes || !apBuffer)
		{
		return OMX_ErrorBadParameter;
		}

	return PopulateBuffer(appBufferHdr,
						  aPortIndex,
						  apAppPrivate,
						  aSizeBytes,
						  apBuffer);

	}


OMX_ERRORTYPE
COmxILFsm::AllocateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
						  OMX_U32 aPortIndex,
						  OMX_PTR apAppPrivate,
						  OMX_U32 aSizeBytes)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::AllocateBuffer"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!appBufferHdr || !aSizeBytes)
		{
		return OMX_ErrorBadParameter;
		}


	return PopulateBuffer(appBufferHdr,
						  aPortIndex,
						  apAppPrivate,
						  aSizeBytes,
						  0);

	}


OMX_ERRORTYPE
COmxILFsm::FreeBuffer(OMX_U32 aPortIndex,
					  OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::FreeBuffer"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apBuffer)
		{
		return OMX_ErrorBadParameter;
		}

	TBool portDepopulationCompleted = EFalse;
	OMX_ERRORTYPE omxRetValue =
		ipCurrentState->FreeBuffer(*this,
								   aPortIndex,
								   apBuffer,
								   portDepopulationCompleted);

	if (OMX_ErrorNone == omxRetValue)
		{
		if (portDepopulationCompleted)
			{
			if (ESubStateIdleToLoaded == iCurrentStateIndex)
				{
				if (iPortManager.AllPortsDePopulated())
					{
					// Complete here the transition to OMX_StateLoaded
					omxRetValue = FsmTransition(EStateLoaded);
					if (OMX_ErrorNone == omxRetValue)
						{
						// Notify the IL client that port depopulation has
						// completed sucessfully
						omxRetValue =
							iCallbacks.TransitionCompleteNotification(
							OMX_StateLoaded);
						}
					}
				}
			}
		}

	if (OMX_ErrorNone == omxRetValue)
		{
		return OMX_ErrorNone;
		}
	else
		{
		return SendOmxErrorEventIfNeeded(omxRetValue);
		}

	}


OMX_ERRORTYPE
COmxILFsm::EmptyThisBuffer(OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF2(_L8("COmxILFsm::EmptyThisBuffer : BUFFER [%X]"), apBuffer);

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apBuffer)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->EmptyThisBuffer(*this, apBuffer));

	}


OMX_ERRORTYPE
COmxILFsm::FillThisBuffer(OMX_BUFFERHEADERTYPE* apBuffer)
	{
    DEBUG_PRINTF2(_L8("COmxILFsm::FillThisBuffer : BUFFER [%X]"), apBuffer);

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apBuffer)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		ipCurrentState->FillThisBuffer(*this, apBuffer));

	}


OMX_ERRORTYPE
COmxILFsm::SetCallbacks(const OMX_CALLBACKTYPE* apCallbacks,
						const OMX_PTR apAppData)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::SetCallbacks"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	if (!apCallbacks)
		{
		return OMX_ErrorBadParameter;
		}

	// This api should only be allowed in OMX_StateLoaded
	if (EStateLoaded != iCurrentStateIndex)
		{
		return OMX_ErrorIncorrectStateOperation;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		iCallbacks.RegisterILClientCallbacks(apCallbacks, apAppData));

	}


OMX_ERRORTYPE
COmxILFsm::UseEGLImage(OMX_BUFFERHEADERTYPE** /*appBufferHdr*/,
					   OMX_U32 /*aPortIndex*/,
					   OMX_PTR /*aAppPrivate*/,
					   void* /*eglImage*/)
	{
    DEBUG_PRINTF(_L8("COmxILFsm::UseEGLImage"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	return OMX_ErrorNotImplemented;
	}

OMX_ERRORTYPE
COmxILFsm::ComponentRoleEnum(OMX_U8* aRole,
							 OMX_U32 aIndex) const
	{
    DEBUG_PRINTF(_L8("COmxILFsm::ComponentRoleEnum"));

	__ASSERT_DEBUG(iCurrentStateIndex != EStateMax,
				   User::Panic(KOmxILFsmPanicCategory, 1));

	// This api should not be allowed in OMX_StateInvalid
	if (EStateInvalid == iCurrentStateIndex)
		{
		return SendOmxErrorEventIfNeeded(OMX_ErrorInvalidState);
		}

	if (!aRole)
		{
		return OMX_ErrorBadParameter;
		}

	RETURN_OMX_ERROR_AND_EVENT_IF_NEEDED(
		iConfigManager.ComponentRoleEnum(aRole,
										 aIndex));

	}

/**
   This method is here to fullfill the following functionalities:

   -# It is used to make sure that the component error codes are returned to
      the IL Client in a way that conforms with Table 3-9 of the OpenMAX IL
      1.1.1 spec. This table specifies which error codes must be sent with
      EventHandler. If an error code is to be sent via EventHandler, the API
      return code must be OMX_ErrorNone.

   -# This method is also used to invalidate the component whenever an internal
      component action returns OMX_ErrorInvalidState. For example, this is
      useful when code executed by a port or by the processing function cannot
      recover from an internal error. Returning OMX_ErrorInvalidState in that
      kind of situation will invalidate the component in
      SendOmxErrorEventIfNeeded and the event will be conveyed to the IL Client
      as mandated by the spec.

   @param aError An OpenMAX IL error code.
 */
OMX_ERRORTYPE
COmxILFsm::SendOmxErrorEventIfNeeded(OMX_ERRORTYPE aError)
	{
	DEBUG_PRINTF2(_L8("COmxILFsm::SendOmxErrorEventIfNeeded - aError = 0x%X"), aError);

	OMX_ERRORTYPE returnError = aError;
	switch(aError)
		{
	case OMX_ErrorInsufficientResources:
		{
		DEBUG_PRINTF(_L8("COmxILFsm::SendOmxErrorEventIfNeeded aError[OMX_ErrorInsufficientResources]"));
		iCallbacks.ErrorEventNotification(aError);
		}
		break;
	case OMX_ErrorInvalidState:
		{
		DEBUG_PRINTF(_L8("COmxILFsm::SendOmxErrorEventIfNeeded aError[OMX_ErrorInvalidState]"));
		iCallbacks.ErrorEventNotification(aError);
		if (EStateInvalid != iCurrentStateIndex)
			{
			returnError = OMX_ErrorNone;
			}
		}
		break;
	case OMX_ErrorUnderflow:
	case OMX_ErrorOverflow:
	case OMX_ErrorHardware:
	case OMX_ErrorStreamCorrupt:
	case OMX_ErrorResourcesLost:
	case OMX_ErrorSameState:
	case OMX_ErrorResourcesPreempted:
	case OMX_ErrorPortUnresponsiveDuringAllocation:
	case OMX_ErrorPortUnresponsiveDuringDeallocation:
	case OMX_ErrorPortUnresponsiveDuringStop:
	case OMX_ErrorIncorrectStateTransition:
	case OMX_ErrorPortUnpopulated:
	case OMX_ErrorDynamicResourcesUnavailable:
	case OMX_ErrorMbErrorsInFrame:
	case OMX_ErrorFormatNotDetected:
		{
		DEBUG_PRINTF2(_L8("COmxILFsm::SendOmxErrorEventIfNeeded aError[%X]"), aError);
		iCallbacks.ErrorEventNotification(aError);
		returnError = OMX_ErrorNone;
		}
		break;
		};

	if(OMX_ErrorInvalidState == aError &&
	   EStateInvalid != iCurrentStateIndex)
		{
		// Invalidate this component. This instance of the component should be
		// destroyed by the IL Client after this. No need to check error code.
		FsmTransition(EStateInvalid);
		}

	return returnError;

	}

OMX_ERRORTYPE
COmxILFsm::SendOmxErrorEventIfNeeded(OMX_ERRORTYPE aError) const
	{
	DEBUG_PRINTF(_L8("COmxILFsm::SendOmxErrorEventIfNeeded"));

	return const_cast<COmxILFsm*>(this)->SendOmxErrorEventIfNeeded(aError);

	}
