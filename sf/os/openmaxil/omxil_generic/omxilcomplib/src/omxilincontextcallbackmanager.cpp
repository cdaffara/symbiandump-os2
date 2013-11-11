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
#include "omxilincontextcallbackmanager.h"
#include "omxilfsm.h"
#include <openmax/il/common/omxilstatedefs.h>
#include <openmax/il/common/omxilutil.h>

COmxILInContextCallbackManager*
COmxILInContextCallbackManager::NewL(
	OMX_HANDLETYPE apComponentHandle,
	OMX_PTR apAppData,
	OMX_CALLBACKTYPE* apCallbacks)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::NewL"));
	COmxILInContextCallbackManager* self = new (ELeave)COmxILInContextCallbackManager(
		apComponentHandle,
		apAppData,
		apCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void
COmxILInContextCallbackManager::ConstructL()
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::ConstructL"));

	User::LeaveIfError(iLock.CreateLocal());

	}

COmxILInContextCallbackManager::COmxILInContextCallbackManager(OMX_HANDLETYPE apComponentHandle,
											 OMX_PTR apAppData,
											 OMX_CALLBACKTYPE* apCallbacks)
	:
	CBase(),
	XOmxILCallbackManagerIfImpl(
		static_cast<OMX_COMPONENTTYPE*>(apComponentHandle),
		apAppData,
		apCallbacks),
	iLock(),
	iPendingQueue(),
	iFlushPendingQueue(EFalse),
	iCurrentState(OMX_StateLoaded),
	iPreviousState(OMX_StateLoaded)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::COmxILInContextCallbackManager"));
	}

COmxILInContextCallbackManager::~COmxILInContextCallbackManager()
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::~COmxILInContextCallbackManager Pending Queue count [%d]"),
				  iPendingQueue.Count());

	iLock.Close();

	iPendingQueue.Close();

	}

void
COmxILInContextCallbackManager::LockCallbackManager()
	{
	iLock.Wait();
	}

void
COmxILInContextCallbackManager::UnlockCallbackManager()
	{
	iLock.Signal();
	}

void
COmxILInContextCallbackManager::SetPortManager(MOmxILPortManagerIf& apPortManager)
	{
	LockCallbackManager();
	DoSetPortManager(apPortManager);
	UnlockCallbackManager();
	}

void
COmxILInContextCallbackManager::SetFsm(COmxILFsm& apFsm)
	{
	LockCallbackManager();
	DoSetFsm(apFsm);
	UnlockCallbackManager();
	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::RegisterComponentHandle(OMX_HANDLETYPE aComponentHandle)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::RegisterComponentHandle"));
	LockCallbackManager();
	OMX_ERRORTYPE omxError = DoRegisterComponentHandle(aComponentHandle);
	UnlockCallbackManager();
	return omxError;
	}

/**
   The IL Client callback registration is handled in this implementation
   asynchronously. Note that this implementation assumes that the IL Client
   will update the callbacks information once all expected callbacks from this
   component have already been received and therefore, the callback change will
   be safe leading to no race condition at the IL Client side.

   @param apCallbacks The IL Client callback structure.

   @param apAppData Pointer to an application provided value so that the
	    application can have a component specific context when receiving
	    the callback.

   @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE
COmxILInContextCallbackManager::RegisterILClientCallbacks(const OMX_CALLBACKTYPE* apCallbacks,
												 const OMX_PTR apAppData)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::RegisterILClientCallbacks"));

	LockCallbackManager();
	OMX_ERRORTYPE omxError = DoRegisterILClientCallbacks(apCallbacks, apAppData);
	UnlockCallbackManager();
	return omxError;

	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::RegisterTunnelCallback(
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection,
	OMX_HANDLETYPE aTunnelledComponentHandle,
	OMX_U32 aTunnelledPortIndex)
	{
	DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::RegisterTunnelCallback : aTunnelledComponentHandle [%x]"), aTunnelledComponentHandle);

	LockCallbackManager();
	OMX_ERRORTYPE omxError = DoRegisterTunnelCallback(aLocalPortIndex,
													  aLocalPortDirection,
													  aTunnelledComponentHandle,
													  aTunnelledPortIndex);
	UnlockCallbackManager();
	return omxError;

	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::DeregisterTunnelCallback(
	OMX_U32 aLocalPortIndex)
	{

	DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::DeregisterTunnelCallback"));

	LockCallbackManager();
	OMX_ERRORTYPE omxError =  DoRegisterTunnelCallback(aLocalPortIndex,
													   OMX_DirMax,
													   0,
													   0);
	UnlockCallbackManager();
	return omxError;

	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::RegisterBufferMarkPropagationPort(
	OMX_U32 aPortIndex,
	OMX_U32 aPropagationPortIndex)
	{
	DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::RegisterBufferMarkPropagationPort"));

	LockCallbackManager();
	OMX_ERRORTYPE omxError = DoRegisterBufferMarkPropagationPort(
		aPortIndex,
		aPropagationPortIndex);
	UnlockCallbackManager();
	return omxError;

	}

TBool
COmxILInContextCallbackManager::BufferRemovalIndication(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection)
	{
	DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::BufferRemovalIndication"));

	return RemoveBuffersByBufferHeader(
		iPendingQueue, apBufferHeader, aDirection);

	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::TransitionCompleteNotification(OMX_STATETYPE aOmxState)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::TransitionCompleteNotification"));

	// No need to lock callback manager. Anyway, we should not have it locked
	// when calling an IL Client callback method
	OMX_ERRORTYPE omxError = EventNotification(OMX_EventCmdComplete,
											   OMX_CommandStateSet,
											   aOmxState,
											   0);

	return omxError;

	}


OMX_ERRORTYPE
COmxILInContextCallbackManager::CommandCompleteNotification(OMX_COMMANDTYPE aOmxCommand,
												   OMX_U32 aOmxPortIndex)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::CommandCompleteNotification"));

	// No need to lock callback manager. Anyway, we should not have it locked
	// when calling an IL Client callback method

	OMX_ERRORTYPE omxError = EventNotification(OMX_EventCmdComplete,
											   aOmxCommand,
											   aOmxPortIndex,
											   0);

	return omxError;

	}


OMX_ERRORTYPE
COmxILInContextCallbackManager::ErrorEventNotification(OMX_ERRORTYPE aOmxError)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::ErrorEventNotification : aOmxError[%X] "), aOmxError);

	// No need to lock callback manager. Anyway, we should not have it locked
	// when calling an IL Client callback method
	OMX_ERRORTYPE omxError = EventNotification(OMX_EventError,
											   aOmxError,
											   0,
											   0);

	return omxError;

	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::EventNotification(OMX_EVENTTYPE aEvent,
												  TUint32 aData1,
												  TUint32 aData2,
												  OMX_STRING aExtraInfo)
	{
    DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::EventNotification : aEvent[%u] aData1[%u] aData2[%u]"),
				  aEvent, aData1, aData2);

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	switch(aData1)
		{
	case OMX_CommandStateSet:
		{
		LockCallbackManager();
		iPreviousState = iCurrentState;
		iCurrentState  = static_cast<OMX_STATETYPE>(aData2);

		DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::EventNotification() : Handle[%X] iPreviousState[%d] -> iCurrentState[%d]"), ipHandle, iPreviousState, iCurrentState);

		if (OMX_StatePause == iPreviousState &&
			OMX_StateIdle == iCurrentState)
			{
			// Release lock before any callback gets called...
			UnlockCallbackManager();

			// Flush pending queue first...
			FlushQueue(iPendingQueue);

			// ... and now signal command completion...
			omxError =
				DoEventNotification(aEvent,
									aData1,
									aData2,
									aExtraInfo);

			}
		else if (OMX_StatePause == iPreviousState &&
				 OMX_StateExecuting == iCurrentState)
			{
			// Release lock before any callback...
			UnlockCallbackManager();

			// Signal command completion first...
			omxError =
				DoEventNotification(aEvent,
									aData1,
									aData2,
									aExtraInfo);

			// ... and now flush...
			FlushQueue(iPendingQueue);

			}
		else
			{
			// Release lock before any callback...
			UnlockCallbackManager();

			// Signal command completion...
			omxError =
				DoEventNotification(aEvent,
									aData1,
									aData2,
									aExtraInfo);

			}

		}
		break;

	case OMX_CommandPortDisable:
	case OMX_CommandFlush:
		{
		// Flush first...
		if (OMX_ALL == aData2)
			{
			FlushQueue(iPendingQueue);
			}
		else
			{
			FlushBuffersByPortIndex(iPendingQueue,
									aData2);
			}

		// ... and now signal command completion...
		omxError =
			DoEventNotification(aEvent,
								aData1,
								aData2,
								aExtraInfo);

		}
		break;

	default:
		{
		// Signal command completion...
		omxError =
			DoEventNotification(aEvent,
								aData1,
								aData2,
								aExtraInfo);

		}

		};

	if (OMX_ErrorInsufficientResources == omxError)
		{
		HandleInsufficientResources();
		}

	return omxError;

	}


OMX_ERRORTYPE
COmxILInContextCallbackManager::BufferDoneNotification(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::BufferDoneNotificaton : BUFFER [%X]"),
				  apBufferHeader);

	__ASSERT_ALWAYS(apBufferHeader &&
					(OMX_DirInput == aLocalPortDirection ||
					 OMX_DirOutput == aLocalPortDirection),
					User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	__ASSERT_ALWAYS(apBufferHeader->nOffset + apBufferHeader->nFilledLen
					<= apBufferHeader->nAllocLen,
					User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	__ASSERT_DEBUG(ipHandle && ipCallbacks,
				   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

	LockCallbackManager();
	if (OMX_StatePause == iCurrentState)
		{
		if (KErrNone != iPendingQueue.Append(TOmxILBuffer(
												 apBufferHeader,
												 aLocalPortIndex,
												 aLocalPortDirection)))
			{
			// Not much we can do here...
			UnlockCallbackManager();
			HandleInsufficientResources();
			}
		else
			{
			DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::BufferDoneNotificaton : DEFERRED buffer header[%X] port [%X] queue items [%d]"),
					  apBufferHeader, aLocalPortIndex, iPendingQueue.Count());
			UnlockCallbackManager();
			}

		return OMX_ErrorNone;

		}
	UnlockCallbackManager();

	ProcessBufferDoneNotification(apBufferHeader,
								  aLocalPortIndex,
								  aLocalPortDirection);

	return OMX_ErrorNone;

	}

void
COmxILInContextCallbackManager::ProcessBufferDoneNotification(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::ProcessBufferDoneNotification : BUFFER [%X]"),
				  apBufferHeader);

	// Look for buffer marks to be signalled or propagated (maintain callback
	// manager unlocked here)
	SignalOrPropagateBufferMarks(apBufferHeader,
								 aLocalPortDirection);

	LockCallbackManager();

	// find out whether the port is tunnelled or not
	TBool tunnelled = EFalse;
	OMX_COMPONENTTYPE* pTunnelledComponent = 0;
	const TUint tunnelCount = iRegisteredTunnels.Count();
	for (TUint i=0; i<tunnelCount; ++i)
		{
		if (iRegisteredTunnels[i].iLocalPortIndex ==
			aLocalPortIndex)
			{
			tunnelled = ETrue;
			pTunnelledComponent =
				static_cast<OMX_COMPONENTTYPE*>(
					iRegisteredTunnels[i].
					iTunnelledComponentHandle);

			__ASSERT_DEBUG(pTunnelledComponent,
						   User::Panic(KOmxILCallbackManagerIfImplPanicCategory, 1));

			break;
			}
		}

	// Unlock callback manager before calling the callback
	UnlockCallbackManager();

	if (tunnelled)
		{
		// From OMX_Core.h "Callbacks should not return an error to the
		// component, so if an error occurs, the application shall handle it
		// internally". Callback return error ignored here.
		if (OMX_DirInput == aLocalPortDirection)
			{
			OMX_FillThisBuffer(pTunnelledComponent, apBufferHeader);
			}
		else
			{
			OMX_EmptyThisBuffer(pTunnelledComponent, apBufferHeader);
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
		fp2CBackHandler(ipHandle,
						ipAppData,
						apBufferHeader);

		}

	}

void
COmxILInContextCallbackManager::SignalOrPropagateBufferMarks(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_U32 aLocalPortIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::SignalOrPropagateBufferMarks() : BUFFER [%X]"),
				  apBufferHeader);

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

			LockCallbackManager();

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

			UnlockCallbackManager();

			}
		}
	else
		{
		LockCallbackManager();

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
				apBufferHeader->pMarkData			 = markInfo.ipMarkData;

				// Remove the mark info from the local store
				iBufferMarks.Remove(index);
				}

			}

		UnlockCallbackManager();

		}


	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::ClockBufferDoneNotification(OMX_BUFFERHEADERTYPE* apBufferHeader,
											 OMX_U32 aLocalPortIndex,
											 OMX_DIRTYPE aLocalPortDirection)
	{
    DEBUG_PRINTF(_L8("COmxILInContextCallbackManager::ClockBufferDoneNotification"));

	return BufferDoneNotification(apBufferHeader,
								  aLocalPortIndex,
								  aLocalPortDirection);

	}

OMX_ERRORTYPE
COmxILInContextCallbackManager::PortSettingsChangeNotification(
	OMX_U32 aLocalPortIndex,
	TUint aPortSettingsIndex,
	const TDesC8& aPortSettings)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::PortSettingsChangeNotification : aLocalPortIndex[%d]"), aLocalPortIndex);

	LockCallbackManager();
	OMX_ERRORTYPE omxError = DoPortSettingsChangeNotification(aLocalPortIndex,
															  aPortSettingsIndex,
															  aPortSettings);
	UnlockCallbackManager();
	return omxError;

	}

#ifdef _OMXIL_COMMON_IL516C_ON
OMX_ERRORTYPE
COmxILInContextCallbackManager::EjectBuffersRequest(
	OMX_U32 aLocalOmxPortIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::EjectBuffersRequest : aLocalOmxPortIndex[%d]"), aLocalOmxPortIndex);

	OMX_ERRORTYPE omxError = DoEjectBuffersRequest(aLocalOmxPortIndex);
	return omxError;

	}
#endif

void
COmxILInContextCallbackManager::FlushQueue(
	RCbMgrBufferQueue& aQueue)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::FlushQueue : Handle[%X]"), ipHandle);

	LockCallbackManager();

	TInt i = 0;
	while(i < aQueue.Count())
		{
		TOmxILBuffer buffer(aQueue[i]);
		aQueue.Remove(i);
		DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::FlushQueue() : FOUND  -> buffer header [%X] PortIndex[%d], queue items [%d]"), buffer.ipBufferHeader, buffer.iLocalPortIndex, aQueue.Count());

		UnlockCallbackManager();
		ProcessBufferDoneNotification(buffer.ipBufferHeader,
									  buffer.iLocalPortIndex,
									  buffer.iLocalPortDirection);
		LockCallbackManager();
		// There is a window where new items could have been added to the
		// queue. Restart loop just in case...
		i = 0;
		}

	UnlockCallbackManager();
	}

TBool
COmxILInContextCallbackManager::RemoveBuffersByBufferHeader(
	RCbMgrBufferQueue& aQueue,
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	const OMX_DIRTYPE aDirection)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::RemoveBuffersByBufferHeader : Handle[%X]"), ipHandle);

	LockCallbackManager();

	TInt i = 0;
	while(i < aQueue.Count())
		{
		TOmxILBuffer buffer(aQueue[i]);
		if (apBufferHeader == buffer.ipBufferHeader)
			{
			__ASSERT_DEBUG(aDirection == OMX_DirInput ||
						   aDirection == OMX_DirOutput,
						   User::Panic(KOmxILCallbackManagerPanicCategory, 1));

			DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::RemoveBuffersByBufferHeader() : Nofiying FSM : Handle[%X] aDirection[%X] apBufferHeader[%X]"), ipHandle, aDirection, apBufferHeader);

			// Make sure the buffer contents are cleared...
			TOmxILUtil::ClearBufferContents(apBufferHeader);

			aQueue.Remove(i);
			UnlockCallbackManager();

			if (aDirection == OMX_DirInput)
				{
				ipFsm->EmptyThisBuffer(
					const_cast<OMX_BUFFERHEADERTYPE*>(apBufferHeader));
				}
			else
				{
				ipFsm->FillThisBuffer(
					const_cast<OMX_BUFFERHEADERTYPE*>(apBufferHeader));
				}

			return ETrue;
			}
		else
			{
			++i;
			}
		}

	UnlockCallbackManager();

	return EFalse;
	}


void
COmxILInContextCallbackManager::FlushBuffersByPortIndex(
	RCbMgrBufferQueue& aQueue,
	const OMX_U32 aLocalPortIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILInContextCallbackManager::FlushBuffersByPortIndex : Handle[%X]"), ipHandle);

	LockCallbackManager();

	TInt i = 0;
	while(i < aQueue.Count())
		{
		TOmxILBuffer buffer(aQueue[i]);
		DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::FlushBuffersByPortIndex() : LOOKING  -> buffer header [%X] PortIndex[%d], queue items [%d]"), buffer.ipBufferHeader, aLocalPortIndex, aQueue.Count());
		if (aLocalPortIndex == buffer.iLocalPortIndex)
			{
			aQueue.Remove(i);
			DEBUG_PRINTF4(_L8("COmxILInContextCallbackManager::FlushBuffersByPortIndex() : FOUND  -> buffer header [%X] PortIndex[%d], queue items [%d]"), buffer.ipBufferHeader, aLocalPortIndex, aQueue.Count());
			UnlockCallbackManager();
			ProcessBufferDoneNotification(buffer.ipBufferHeader,
										  buffer.iLocalPortIndex,
										  buffer.iLocalPortDirection);
			LockCallbackManager();
			// There is a window where new items could have been added to the
			// queue. Restart loop just in case...
			i = 0;
			}
		else
			{
			++i;
			}
		}

	UnlockCallbackManager();

	}

