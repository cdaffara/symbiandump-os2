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
#include "omxilcallbackmanagerifimpl.h"
#include "omxilfsm.h"
#include "omxilportmanagerif.h"
#include <openmax/il/common/omxilstatedefs.h>




XOmxILCallbackManagerIfImpl::XOmxILCallbackManagerIfImpl(OMX_HANDLETYPE apComponentHandle,
														 OMX_PTR apAppData,
														 OMX_CALLBACKTYPE* apCallbacks)
	:
	ipHandle(static_cast<OMX_COMPONENTTYPE*>(apComponentHandle)),
	ipAppData(apAppData),
	ipCallbacks(apCallbacks),
	iRegisteredTunnels(),
	iBufferMarkPropagationPorts(),
	iBufferMarks(),
	ipPortManager(0),
	ipFsm(0)
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::XOmxILCallbackManagerIfImpl"));
	}


XOmxILCallbackManagerIfImpl::~XOmxILCallbackManagerIfImpl()
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::~XOmxILCallbackManagerIfImpl"));
	// These pointers are cleared to make sure that any further calls on this
	// object will fail (e.g., from a threaded PF)
	ipHandle	= 0;
	ipAppData	= 0;
	ipCallbacks = 0;

	iRegisteredTunnels.Close();
	iBufferMarkPropagationPorts.Close();
	iBufferMarks.Close();

	ipPortManager = 0;

	ipFsm = 0;

	}

void
XOmxILCallbackManagerIfImpl::DoSetPortManager(MOmxILPortManagerIf& apPortManager)
	{
	ipPortManager = &apPortManager;
	}

void
XOmxILCallbackManagerIfImpl::DoSetFsm(COmxILFsm& apFsm)
	{
	ipFsm = &apFsm;
	}

OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoRegisterComponentHandle(OMX_HANDLETYPE aComponentHandle)
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::DoRegisterComponentHandle"));

	__ASSERT_DEBUG(aComponentHandle,
				   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	ipHandle = static_cast<OMX_COMPONENTTYPE *>(aComponentHandle);

	return OMX_ErrorNone;

	}


OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoRegisterILClientCallbacks(const OMX_CALLBACKTYPE* apCallbacks,
														 const OMX_PTR apAppData)
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::DoRegisterILClientCallbacks"));

	ipAppData   = const_cast<OMX_PTR>(apAppData);
	ipCallbacks = const_cast<OMX_CALLBACKTYPE*>(apCallbacks);

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoRegisterTunnelCallback(
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection,
	OMX_HANDLETYPE aTunnelledComponentHandle,
	OMX_U32 aTunnelledPortIndex)
	{
	DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::DoRegisterTunnelCallback"));

	OMX_ERRORTYPE omxError = OMX_ErrorNone;

	if (aTunnelledComponentHandle)
		{
		// Register tunnelled port
		TInt err = iRegisteredTunnels.Append(
			TTunnelRegistrationInfo(aLocalPortIndex,
									aLocalPortDirection,
									aTunnelledComponentHandle,
									aTunnelledPortIndex));
		if (KErrNone != err)
			{
			switch (err)
				{
			case KErrNoMemory:
				{
				omxError = OMX_ErrorInsufficientResources;
				}
				break;
			default:
				{
				omxError = OMX_ErrorUndefined;
				}
				};
			}

		}
	else
		{
		// Deregister tunnelled port
		const TUint tunnelCount = iRegisteredTunnels.Count();
		for (TUint i=0; i<tunnelCount; ++i)
			{
			if (iRegisteredTunnels[i].iLocalPortIndex ==
				aLocalPortIndex)
				{
				iRegisteredTunnels.Remove(i);
				break;
				}
			}
		}

	return omxError;

	}

OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoRegisterBufferMarkPropagationPort(
	OMX_U32 aPortIndex,
	OMX_U32 aPropagationPortIndex)
	{
	DEBUG_PRINTF3(_L8("XOmxILCallbackManagerIfImpl::DoRegisterBufferMarkPropagationPort : aPortIndex[%d] aPropagationPortIndex[%d] "),aPortIndex, aPropagationPortIndex);

	TInt err = iBufferMarkPropagationPorts.Append(
		TBufferMarkPropagationInfo(aPortIndex,
								   aPropagationPortIndex));

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	if (KErrNone != err)
		{
		switch (err)
			{
		case KErrNoMemory:
			{
			omxError = OMX_ErrorInsufficientResources;
			}
			break;
		default:
			{
			omxError = OMX_ErrorUndefined;
			}
			};
		}

	return omxError;

	}



OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoTransitionCompleteNotification(OMX_STATETYPE aOmxState)
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::DoTransitionCompleteNotification"));

	return DoEventNotification(OMX_EventCmdComplete,
							   OMX_CommandStateSet,
							   aOmxState,
							   0);

	}


OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoCommandCompleteNotification(OMX_COMMANDTYPE aOmxCommand,
														   OMX_U32 aOmxPortIndex)
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::DoCommandCompleteNotification"));

	return DoEventNotification(OMX_EventCmdComplete,
							   aOmxCommand,
							   aOmxPortIndex,
							   0);

	}


OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoErrorEventNotification(OMX_ERRORTYPE aOmxError)
	{
    DEBUG_PRINTF2(_L8("XOmxILCallbackManagerIfImpl::DoErrorEventNotification : aOmxError[%X] "), aOmxError);

	return DoEventNotification(OMX_EventError,
							   aOmxError,
							   0,
							   0);

	}

OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoEventNotification(OMX_EVENTTYPE aEvent,
												 TUint32 aData1,
												 TUint32 aData2,
												 OMX_STRING aExtraInfo)
	{
    DEBUG_PRINTF5(_L8("XOmxILCallbackManagerIfImpl::DoEventNotification : Handle[%X] aEvent[%u] aData1[%u] aData2[%u]"), ipHandle, aEvent, aData1, aData2);

	__ASSERT_DEBUG(ipHandle && ipCallbacks, User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));


	ipCallbacks->EventHandler(ipHandle,
							  ipAppData,
							  aEvent,
							  aData1,
							  aData2,
							  aExtraInfo);
	return OMX_ErrorNone;

	}


OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoBufferDoneNotification(OMX_BUFFERHEADERTYPE* apBufferHeader,
													  OMX_U32 aLocalPortIndex,
													  OMX_DIRTYPE aLocalPortDirection)
	{
    DEBUG_PRINTF5(_L8("XOmxILCallbackManagerIfImpl::DoBufferDoneNotification : HANDLE [%X] BUFFER [%X] PORT[%d] DIR[%d]"), ipHandle, apBufferHeader, aLocalPortIndex, aLocalPortDirection);

	__ASSERT_ALWAYS(apBufferHeader &&
					(OMX_DirInput == aLocalPortDirection ||
					 OMX_DirOutput == aLocalPortDirection),
					User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	__ASSERT_ALWAYS(apBufferHeader->nOffset + apBufferHeader->nFilledLen
					<= apBufferHeader->nAllocLen,
					User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	__ASSERT_DEBUG(ipHandle && ipCallbacks, User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	// Look for buffer marks to be signalled or propagated
	SignalOrPropagateBufferMarks(apBufferHeader,
								 aLocalPortDirection);

	// find out whether the port is tunnelled or not
	TBool tunnelled = EFalse;
	TUint tunnelInfoArrayIndex = 0;
	const TUint tunnelCount = iRegisteredTunnels.Count();
	for (TUint i=0; i<tunnelCount; ++i)
		{
		if (iRegisteredTunnels[i].iLocalPortIndex ==
			aLocalPortIndex)
			{
			tunnelled = ETrue;
			tunnelInfoArrayIndex = i;
			break;
			}
		}

	if (tunnelled)
		{
		OMX_COMPONENTTYPE* ipTunnelledComponent =
			static_cast<OMX_COMPONENTTYPE*>(
				iRegisteredTunnels[tunnelInfoArrayIndex].
				iTunnelledComponentHandle);

		__ASSERT_DEBUG(ipTunnelledComponent,
					   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

		// From OMX_Core.h "Callbacks should not return an error to the
		// component, so if an error occurs, the application shall handle it
		// internally". Callback return error ignored here.
		if (OMX_DirInput == aLocalPortDirection)
			{
			OMX_FillThisBuffer(ipTunnelledComponent, apBufferHeader);
			}
		else
			{
			OMX_EmptyThisBuffer(ipTunnelledComponent, apBufferHeader);
			}

		}
	else
		{
		OMX_ERRORTYPE (*fp2CBackHandler)
			(OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE*) =
			(aLocalPortDirection == OMX_DirInput ?
			 ipCallbacks->EmptyBufferDone :
			 ipCallbacks->FillBufferDone);

		// From OMX_Core.h "Callbacks should not return an error to the
		// component, so if an error occurs, the application shall handle it
		// internally". Callback return error ignored here.
#ifdef OMX_DEBUG_TRACING_ON
		if (aLocalPortDirection == OMX_DirInput)
			{
			OMX_TRACE_EMPTYBUFFERDONE_IN(ipHandle, ipAppData, apBufferHeader);
			}
		else
			{
			OMX_TRACE_FILLBUFFERDONE_IN(ipHandle, ipAppData, apBufferHeader);
			}
#endif
		fp2CBackHandler(ipHandle,
						ipAppData,
						apBufferHeader);
#ifdef OMX_DEBUG_TRACING_ON
        if (aLocalPortDirection == OMX_DirInput)
            {
            OMX_TRACE_EMPTYBUFFERDONE_OUT(ipHandle, ipAppData, apBufferHeader, 0);
            }
        else
            {
            OMX_TRACE_FILLBUFFERDONE_OUT(ipHandle, ipAppData, apBufferHeader, 0);
            }
#endif

		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoPortSettingsChangeNotification(
	OMX_U32 aLocalPortIndex,
	TUint aPortSettingsIndex,
	const TDesC8& aPortSettings)
	{
    DEBUG_PRINTF2(_L8("XOmxILCallbackManagerIfImpl::DoPortSettingsChangeNotification : aLocalPortIndex[%d]"), aLocalPortIndex);

	__ASSERT_DEBUG(ipHandle &&
				   ipCallbacks &&
				   ipPortManager,
				   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	HBufC8* pPortSettings = HBufC8::New(aPortSettings.Length());
	if (!pPortSettings)
		{
		return OMX_ErrorInsufficientResources;
		}
	*pPortSettings = aPortSettings;

	// This is an event that the port may want to convey to the IL Client...
	OMX_EVENTTYPE eventForILClient = OMX_EventMax;
	OMX_ERRORTYPE omxRetError =
		ipPortManager->PortSettingsChangeIndication(aLocalPortIndex,
													aPortSettingsIndex,
													*pPortSettings,
													eventForILClient);

	delete pPortSettings;
	pPortSettings = NULL;

	// Inform the IL Client that some value in one of the ports' configuration
	// structures has changed...
	if (OMX_EventMax != eventForILClient)
		{
		// Only allow these two port events...
		__ASSERT_ALWAYS(eventForILClient == OMX_EventPortSettingsChanged ||
						eventForILClient == OMX_EventPortFormatDetected,
						User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

		// From OMX_Core.h "Callbacks should not return an error to the component,
		// so if an error occurs, the application shall handle it
		// internally". Callback return error ignored here.
		ipCallbacks->EventHandler(ipHandle,
								  ipAppData,
								  eventForILClient,
								  aLocalPortIndex,
								  0,
								  0);
		}

	return OMX_ErrorNone;

	}

#ifdef _OMXIL_COMMON_IL516C_ON
OMX_ERRORTYPE
XOmxILCallbackManagerIfImpl::DoEjectBuffersRequest(
	OMX_U32 aLocalPortIndex)
	{
    DEBUG_PRINTF2(_L8("XOmxILCallbackManagerIfImpl::DoEjectBuffersRequest : "
					  "aLocalPortIndex[%d]"), aLocalPortIndex);

	__ASSERT_DEBUG(ipHandle &&
				   ipCallbacks &&
				   ipPortManager,
				   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	// find out whether the port is tunnelled or not
	const TUint tunnelCount = iRegisteredTunnels.Count();
	for (TUint i=0; i<tunnelCount; ++i)
		{
		if (iRegisteredTunnels[i].iLocalPortIndex ==
			aLocalPortIndex)
			{

			OMX_COMPONENTTYPE* ipTunnelledComponent =
				static_cast<OMX_COMPONENTTYPE*>(
					iRegisteredTunnels[i].
					iTunnelledComponentHandle);

			__ASSERT_DEBUG(ipTunnelledComponent,
						   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

			DEBUG_PRINTF3(_L8("XOmxILCallbackManagerIfImpl::DoEjectBuffersRequest : "
							  "iTunnelledComponent [%X] iTunnelledPortIndex[%u]"),
						  ipTunnelledComponent,
						  iRegisteredTunnels[i].iTunnelledPortIndex);

			OMX_PARAM_U32TYPE ejectBufferRequest;
			ejectBufferRequest.nSize	  = sizeof(OMX_PARAM_U32TYPE);
			ejectBufferRequest.nVersion	  = TOmxILSpecVersion();
			ejectBufferRequest.nPortIndex = iRegisteredTunnels[i].iTunnelledPortIndex;
			ejectBufferRequest.nU32		  = 1;	// .... we can define a constant to be used here

			// Returned error ignored here. Cannot handle an error from the
			// tunnelled component
			OMX_SetConfig(ipTunnelledComponent,
						  OMX_IndexConfigPortBufferReturnRequest,
						  &ejectBufferRequest);

			break;
			}
		}

	return OMX_ErrorNone;

	}
#endif

void
XOmxILCallbackManagerIfImpl::SignalOrPropagateBufferMarks(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_U32 aLocalPortIndex)
	{
    DEBUG_PRINTF(_L8("XOmxILCallbackManagerIfImpl::SignalOrPropagateBufferMarks()"));

	// Look for buffer marks to be signalled or propagated
	if (apBufferHeader->hMarkTargetComponent)
		{
		// See if this component is the buffer mark target component...
		if (apBufferHeader->hMarkTargetComponent == ipHandle)
			{
			// Inform the IL Client that a marked buffer has been processed...
			ipCallbacks->EventHandler(ipHandle,
									  ipAppData,
									  OMX_EventMark,
									  0,
									  0,
									  apBufferHeader->pMarkData);

			// At this point, the mark has been delivered to the IL
			// Client...Remove the mark from the processed header...
			apBufferHeader->hMarkTargetComponent = 0;
			apBufferHeader->pMarkData = 0;

			}
		else
			{
			// Propagate the mark...

			// First find the buffer mark propagation port...
			const TInt index = iBufferMarkPropagationPorts.Find(
				TBufferMarkPropagationInfo(aLocalPortIndex),
				TIdentityRelation<TBufferMarkPropagationInfo>(
					&TBufferMarkPropagationInfo::Compare));

			// Note that sink components don't propagate marks...
			if (index != KErrNotFound)
				{
				const TBufferMarkPropagationInfo& propInfo =
					iBufferMarkPropagationPorts[index];

				// Let's check for the special case: The case for a source
				// component where the output port is both the port that marks
				// the headers and the port that propagates them ... Therefore
				// no need to store the mark for later propagation...
				if (propInfo.iPropagationPortIndex != aLocalPortIndex)
					{
					// Now, store temporarily the mark so the next time we send
					// a buffer done callback in that propagation port, we mark
					// that header accordingly...
					// Unsuccessful insertion is ignored.
					iBufferMarks.Append(
						TOutputPortBufferMarkInfo(
							propInfo.iPropagationPortIndex,
							apBufferHeader->hMarkTargetComponent,
							apBufferHeader->pMarkData));

					// At this point the mark has been set for propagation to
					// an output port. Remove the mark from the processed
					// header...
					apBufferHeader->hMarkTargetComponent = 0;
					apBufferHeader->pMarkData = 0;
					}
				}
			}
		}
	else
		{
		if(iBufferMarks.Count() != 0)
			{
			// Let's see if we have a mark waiting to go out...This will find the
			// first mark in the local list of marks ...
			const TInt index = iBufferMarks.Find(
				TOutputPortBufferMarkInfo(aLocalPortIndex),
				TIdentityRelation<TOutputPortBufferMarkInfo>(
					&TOutputPortBufferMarkInfo::Compare));
			if (index != KErrNotFound)
				{
				const TOutputPortBufferMarkInfo& markInfo =
					iBufferMarks[index];

				// Mark the header...
				apBufferHeader->hMarkTargetComponent = markInfo.ipMarkTargetComponent;
				apBufferHeader->pMarkData                        = markInfo.ipMarkData;

				// Remove the mark info from the local store
				iBufferMarks.Remove(index);
				}
			}

		}


	}

void
XOmxILCallbackManagerIfImpl::HandleInsufficientResources()
	{
    DEBUG_PRINTF3(_L8("XOmxILCallbackManagerIfImpl::HandleInsufficientResources : ipCallbacks[%X] ipHandle[%X]"), ipCallbacks, ipHandle);

	if (ipCallbacks && ipHandle)
		{
		// This is a best-effort action, let's try to inform the IL Client of
		// the lack of resources...
		ipCallbacks->EventHandler(ipHandle,
								  ipAppData,
								  OMX_EventError,
					  			  (OMX_U32)OMX_ErrorInsufficientResources,
								  0,
								  0);
		}
	}
