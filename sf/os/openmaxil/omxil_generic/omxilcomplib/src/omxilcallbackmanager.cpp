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
#include "omxilcallbackmanager.h"
#include "omxilfsm.h"
#include <openmax/il/common/omxilstatedefs.h>
#include <openmax/il/common/omxilutil.h>
#include "omxilportmanagerif.h"

const TInt COmxILCallbackManager::KMaxMsgQueueEntries;


COmxILCallbackManager*
COmxILCallbackManager::NewL(
	OMX_HANDLETYPE apComponentHandle,
	OMX_PTR apAppData,
	OMX_CALLBACKTYPE* apCallbacks)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::NewL"));
	COmxILCallbackManager* self = new (ELeave)COmxILCallbackManager(
		apComponentHandle,
		apAppData,
		apCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void
COmxILCallbackManager::ConstructL()
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::ConstructL"));

	CActiveScheduler::Add(this);
	User::LeaveIfError(iCommandQueue.CreateLocal(KMaxMsgQueueEntries));
	iCommandQueue.NotifyDataAvailable(iStatus);

	User::LeaveIfError(iPendingQueue.CreateLocal(KMaxMsgQueueEntries));

	SetActive();

	}

COmxILCallbackManager::COmxILCallbackManager(OMX_HANDLETYPE apComponentHandle,
											 OMX_PTR apAppData,
											 OMX_CALLBACKTYPE* apCallbacks)
	:
	CActive(CActive::EPriorityStandard),
	XOmxILCallbackManagerIfImpl(
		static_cast<OMX_COMPONENTTYPE*>(apComponentHandle),
		apAppData,
		apCallbacks),
	iCommandQueue(),
	iPendingQueue(),
	iFlushPendingQueue(EFalse),
	iCurrentState(OMX_StateLoaded),
	iPreviousState(OMX_StateLoaded)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::COmxILCallbackManager"));
	}


COmxILCallbackManager::~COmxILCallbackManager()
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::~COmxILCallbackManager"));

	Cancel();

	CleanUpQueue(iPendingQueue);

	CleanUpQueue(iCommandQueue);

	}


void
COmxILCallbackManager::CleanUpQueue(RCallbackManagerQueue& aQueue)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::CleanUpQueue"));

	if (aQueue.Handle() != 0)
		{
		CCallbackCommand* pCommand = 0;
		TInt err = KErrNone;
		while ((err = aQueue.Receive(pCommand)) == KErrNone)
			{
			DEBUG_PRINTF2(_L8("COmxILCallbackManager::CleanUpQueue : aQueue.Receive [%X]"), pCommand);
			delete pCommand;
			pCommand = 0;
			}

		if (KErrNone != err)
			{
			DEBUG_PRINTF2(_L8("COmxILCallbackManager::CleanUpQueue : aQueue.Receive returned error [%d]"), err);
			if (KErrNoMemory == err)
				{
				HandleInsufficientResources();
				}
			}

		}

	aQueue.Close();

	}


void
COmxILCallbackManager::SetPortManager(MOmxILPortManagerIf& apPortManager)
	{
	DoSetPortManager(apPortManager);
	}

void
COmxILCallbackManager::SetFsm(COmxILFsm& apFsm)
	{
	DoSetFsm(apFsm);
	}

OMX_ERRORTYPE
COmxILCallbackManager::RegisterComponentHandle(OMX_HANDLETYPE aComponentHandle)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::RegisterComponentHandle"));

	__ASSERT_DEBUG(aComponentHandle,
				   User::Panic(KOmxILCallbackManagerPanicCategory, 1));

	CCompHandleRegistrationCommand* pHandleReg =
		new CCompHandleRegistrationCommand(aComponentHandle);
	if (!pHandleReg || (iCommandQueue.Send(pHandleReg) != KErrNone))
		{
		delete pHandleReg;
		DoRegisterComponentHandle(aComponentHandle);
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;

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
COmxILCallbackManager::RegisterILClientCallbacks(
	const OMX_CALLBACKTYPE* apCallbacks,
	const OMX_PTR apAppData)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::RegisterILClientCallbacks"));

	CClientCallbacksRegistrationCommand* pClientCBacksReg =
		new CClientCallbacksRegistrationCommand(
			apCallbacks,
			apAppData);
	if (!pClientCBacksReg || (iCommandQueue.Send(pClientCBacksReg) != KErrNone))
		{
		delete pClientCBacksReg;
		DoRegisterILClientCallbacks(apCallbacks, apAppData);
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILCallbackManager::RegisterTunnelCallback(
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection,
	OMX_HANDLETYPE aTunnelledComponentHandle,
	OMX_U32 aTunnelledPortIndex)
	{
	DEBUG_PRINTF2(_L8("COmxILCallbackManager::RegisterTunnelCallback : aTunnelledComponentHandle [%x]"), aTunnelledComponentHandle);

	CTunnelCallbackRegistrationCommand* pTunnelCBacksReg =
		new CTunnelCallbackRegistrationCommand(aLocalPortIndex,
											   aLocalPortDirection,
											   aTunnelledComponentHandle,
											   aTunnelledPortIndex);

	if (!pTunnelCBacksReg || (iCommandQueue.Send(pTunnelCBacksReg) != KErrNone))
		{
		delete pTunnelCBacksReg;
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILCallbackManager::DeregisterTunnelCallback(
	OMX_U32 aLocalPortIndex)
	{

	DEBUG_PRINTF(_L8("COmxILCallbackManager::DeregisterTunnelCallback"));

	return RegisterTunnelCallback(aLocalPortIndex,
								  OMX_DirMax,
								  0,
								  0);

	}

OMX_ERRORTYPE
COmxILCallbackManager::RegisterBufferMarkPropagationPort(
	OMX_U32 aPortIndex,
	OMX_U32 aPropagationPortIndex)
	{
	DEBUG_PRINTF(_L8("COmxILCallbackManager::RegisterBufferMarkPropagationPort"));

	CBufferMarkPropagationRegistrationCommand* pBufferMarkPropReg =
		new CBufferMarkPropagationRegistrationCommand(aPortIndex,
													  aPropagationPortIndex);
	if (!pBufferMarkPropReg || (iCommandQueue.Send(pBufferMarkPropReg) != KErrNone))
		{
		delete pBufferMarkPropReg;
		HandleInsufficientResources();
		}

	return OMX_ErrorNone;

	}

TBool
COmxILCallbackManager::BufferRemovalIndication(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection)
	{
	DEBUG_PRINTF(_L8("COmxILCallbackManager::BufferRemovalIndication"));

	CBufferRemovalCommand* pBufferRemovalCmd =
		new CBufferRemovalCommand(apBufferHeader, aDirection);

	if (!pBufferRemovalCmd ||
		(iCommandQueue.Send(pBufferRemovalCmd) != KErrNone))
		{
		delete pBufferRemovalCmd;
		HandleInsufficientResources();
		}

	// Always return false now as the buffer would be removed asynchronously
	return EFalse;

	}

OMX_ERRORTYPE
COmxILCallbackManager::TransitionCompleteNotification(OMX_STATETYPE aOmxState)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::TransitionCompleteNotification"));

	return EventNotificationImpl(OMX_EventCmdComplete,
								OMX_CommandStateSet,
								aOmxState,
								0);

	}


OMX_ERRORTYPE
COmxILCallbackManager::CommandCompleteNotification(OMX_COMMANDTYPE aOmxCommand,
												   OMX_U32 aOmxPortIndex)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::CommandCompleteNotification"));

	return EventNotification(OMX_EventCmdComplete,
							 aOmxCommand,
							 aOmxPortIndex,
							 0);

	}

#ifdef _OMXIL_COMMON_IL516C_ON
OMX_ERRORTYPE
COmxILCallbackManager::EjectBuffersRequest(OMX_U32 aLocalOmxPortIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILCallbackManager::EjectBuffersRequest : aLocalOmxPortIndex [%u]"), aLocalOmxPortIndex);

	CEjectBuffersRequestCommand* pEjectCBack =
		new CEjectBuffersRequestCommand(aLocalOmxPortIndex);
	if (!pEjectCBack || (iCommandQueue.Send(pEjectCBack) != KErrNone))
		{
		delete pEjectCBack;
		HandleInsufficientResources();
		}

	return OMX_ErrorNone;

	}
#endif

OMX_ERRORTYPE
COmxILCallbackManager::ErrorEventNotification(OMX_ERRORTYPE aOmxError)
	{
    DEBUG_PRINTF2(_L8("COmxILCallbackManager::ErrorEventNotification : aOmxError[%X] "), aOmxError);

	return EventNotification(OMX_EventError,
							 aOmxError,
							 0,
							 0);

	}

OMX_ERRORTYPE
COmxILCallbackManager::EventNotification(OMX_EVENTTYPE aEvent,
										 TUint32 aData1,
										 TUint32 aData2,
										 OMX_STRING aExtraInfo)
	{
	// The error code is ignored intentionally, as errors from this function cannot be handled by clients, since they don't have
	// another mechanism for reporting an error
	(void)EventNotificationImpl(aEvent, aData1, aData2, aExtraInfo);
	return OMX_ErrorNone;
	}


OMX_ERRORTYPE COmxILCallbackManager::EventNotificationImpl(OMX_EVENTTYPE aEvent,
														   TUint32 aData1,
														   TUint32 aData2,
														   OMX_STRING aExtraInfo)
	{
    DEBUG_PRINTF4(_L8("COmxILCallbackManager::EventNotificationImpl : aEvent[%X] aData1[%X] aData2[%u]"), aEvent, aData1, aData2);

	CEventCallbackCommand* pEventCBack =
		new CEventCallbackCommand(aEvent,
								  aData1,
								  aData2,
								  aExtraInfo);
	if (!pEventCBack || (iCommandQueue.Send(pEventCBack) != KErrNone))
		{
		delete pEventCBack;
		HandleInsufficientResources();
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;

	}


OMX_ERRORTYPE
COmxILCallbackManager::BufferDoneNotification(OMX_BUFFERHEADERTYPE* apBufferHeader,
											 OMX_U32 aLocalPortIndex,
											 OMX_DIRTYPE aLocalPortDirection)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::BufferDoneNotificaton"));

	return SendBufferDoneNotification(apBufferHeader,
									  aLocalPortIndex,
									  aLocalPortDirection,
									  CCallbackCommand::EPriorityNormal);

	}

OMX_ERRORTYPE
COmxILCallbackManager::ClockBufferDoneNotification(OMX_BUFFERHEADERTYPE* apBufferHeader,
												   OMX_U32 aLocalPortIndex,
												   OMX_DIRTYPE aLocalPortDirection)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::ClockBufferDoneNotification"));

	return SendBufferDoneNotification(apBufferHeader,
									  aLocalPortIndex,
									  aLocalPortDirection,
									  CCallbackCommand::EPriorityHigh);

	}

OMX_ERRORTYPE
COmxILCallbackManager::PortSettingsChangeNotification(
	OMX_U32 aLocalPortIndex,
	TUint aPortSettingsIndex,
	const TDesC8& aPortSettings)
	{
    DEBUG_PRINTF2(_L8("COmxILCallbackManager::PortSettingsChangeNotification : aLocalPortIndex[%d]"), aLocalPortIndex);

	HBufC8* pPortSettings = aPortSettings.Alloc();
	if (!pPortSettings)
		{
		HandleInsufficientResources();
		return OMX_ErrorNone;
		}

	CPortSettingsChangeCommand* pPortSettingsCmd =
		new CPortSettingsChangeCommand(aLocalPortIndex,
									   aPortSettingsIndex,
									   pPortSettings);
	if (!pPortSettingsCmd)
		{
		delete pPortSettings;
		HandleInsufficientResources();
		return OMX_ErrorNone;
		}

	if (iCommandQueue.Send(pPortSettingsCmd) != KErrNone)
		{
		delete pPortSettingsCmd; // Destructor will delete pPortSettings
		HandleInsufficientResources();
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILCallbackManager::SendBufferDoneNotification(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection,
	TInt aPriority)
	{
    DEBUG_PRINTF(_L8("COmxILCallbackManager::SendBufferDoneNotification"));

	__ASSERT_ALWAYS(apBufferHeader &&
					(OMX_DirInput == aLocalPortDirection ||
					 OMX_DirOutput == aLocalPortDirection),
					User::Panic(KOmxILCallbackManagerPanicCategory, 1));

	__ASSERT_ALWAYS(apBufferHeader->nOffset + apBufferHeader->nFilledLen
					<= apBufferHeader->nAllocLen,
					User::Panic(KOmxILCallbackManagerPanicCategory, 1));

	CBufferDoneCallbackCommand* pEventCBack =
		new CBufferDoneCallbackCommand(apBufferHeader,
									   aLocalPortIndex,
									   aLocalPortDirection,
									   aPriority);

	if (!pEventCBack || (iCommandQueue.Send(pEventCBack) != KErrNone))
		{
		delete pEventCBack;
		HandleInsufficientResources();
		}

	return OMX_ErrorNone;

	}


void
COmxILCallbackManager::RunL()
	{
    DEBUG_PRINTF2(_L8("COmxILCallbackManager::RunL : Handle[%X]"), ipHandle);

	ProcessQueue(iCommandQueue);

	// Setup for next callbacks
	iCommandQueue.NotifyDataAvailable(iStatus);
	SetActive();

	}


void
COmxILCallbackManager::ProcessQueue(RCallbackManagerQueue& aQueue)
	{
    DEBUG_PRINTF2(_L8("COmxILCallbackManager::ProcessQueue : Handle[%X]"), ipHandle);

	CCallbackCommand* pCommand = 0;

	TInt receiveRes = 0;
	TBool hasBeenDeferred = EFalse;
	while ((receiveRes = aQueue.Receive(pCommand)) == KErrNone)
		{
		if (pCommand)
			{
			DEBUG_PRINTF2(_L8("COmxILCallbackManager::ProcessQueue : aQueue.Receive [%X]"),
						  pCommand);
			hasBeenDeferred = EFalse;
			(*pCommand)(*this, hasBeenDeferred);
			if (hasBeenDeferred)
				{
				// Move the current command to the pending queue
				if (iPendingQueue.Send(pCommand) != KErrNone)
					{
					delete pCommand;
					pCommand = 0;
					HandleInsufficientResources();
					}
				}
			else
				{
				delete pCommand;
				pCommand = 0;
				}
			}

		}

	if (KErrNoMemory == receiveRes)
		{
		HandleInsufficientResources();
		}

	}

void
COmxILCallbackManager::DoCancel()
	{
    DEBUG_PRINTF2(_L8("COmxILCallbackManager::DoCancel : Handle[%X]"), ipHandle);

	iCommandQueue.CancelDataAvailable();

	}


//
// COmxILCallbackManager::RCallbackManagerQueue
//
TBool
COmxILCallbackManager::RCallbackManagerQueue::RemoveBufferDoneCbCommandsByBufferHeader(
	COmxILCallbackManager& aCbMgr,
	const OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection)
	{
    DEBUG_PRINTF2(_L8("RCallbackManagerQueue::RemoveBufferDoneCbCommandsByBufferHeader : Handle[%X]"), aCbMgr.ipHandle);

	if (KErrNone != DrainBackQueue())
		{
		aCbMgr.HandleInsufficientResources();
		return EFalse;
		}

	if(!iFrontQueue.IsEmpty())
		{
		TBool removed = EFalse;
		TDblQueIter<TFrontQueueElement> frontQueueIter(iFrontQueue);
		TFrontQueueElement* pLastElement = iFrontQueue.Last();
		TFrontQueueElement* pCurrentElement = 0;

		do
			{
			pCurrentElement = frontQueueIter++;
			__ASSERT_DEBUG(pCurrentElement && pCurrentElement->ipInfo,
						   User::Panic(KOmxILCallbackManagerPanicCategory, 1));
			removed = reinterpret_cast<CCallbackCommand*>(
				const_cast<TAny*>(pCurrentElement->ipInfo))->
				DoRemoveBufferDoneCbCommandByBufferHeader(aCbMgr,
														  apBufferHeader,
														  aDirection);
			if (removed)
				{
				pCurrentElement->iLink.Deque();
				delete reinterpret_cast<CCallbackCommand*>(
					const_cast<TAny*>(pCurrentElement->ipInfo));
				delete pCurrentElement;
				DEBUG_PRINTF2(_L8("RCallbackManagerQueue::RemoveBufferDoneCbCommandsByBufferHeader : Removed Buffer Done @ Header [%X]"), apBufferHeader);
				return ETrue;
				}
			}
		while (pCurrentElement != pLastElement);
		}

	return EFalse;

	}

TBool
COmxILCallbackManager::RCallbackManagerQueue::RemoveBufferDoneCbCommandsByPortIndex(
	COmxILCallbackManager& aCbMgr,
	OMX_U32 aLocalPortIndex)
	{
    DEBUG_PRINTF2(_L8("RCallbackManagerQueue::RemoveBufferDoneCbCommandsByPortIndex : Handle[%X]"), aCbMgr.ipHandle);

	TBool somethingRemoved = EFalse;

	if (KErrNone != DrainBackQueue())
		{
		aCbMgr.HandleInsufficientResources();
		return EFalse;
		}

	if(!iFrontQueue.IsEmpty())
		{
		TBool removed = EFalse;
		TDblQueIter<TFrontQueueElement> frontQueueIter(iFrontQueue);
		TFrontQueueElement* pLastElement = iFrontQueue.Last();
		TFrontQueueElement* pCurrentElement = 0;

		do
			{
			pCurrentElement = frontQueueIter++;
			__ASSERT_DEBUG(pCurrentElement && pCurrentElement->ipInfo,
						   User::Panic(KOmxILCallbackManagerPanicCategory, 1));
			removed = reinterpret_cast<CCallbackCommand*>(
				const_cast<TAny*>(pCurrentElement->ipInfo))->
				DoRemoveBufferDoneCbCommandByPortIndex(aCbMgr,
													   aLocalPortIndex);
			if (removed)
				{
				somethingRemoved = ETrue;
				pCurrentElement->iLink.Deque();
				delete reinterpret_cast<CCallbackCommand*>(
					const_cast<TAny*>(pCurrentElement->ipInfo));
				delete pCurrentElement;
				DEBUG_PRINTF2(_L8("RCallbackManagerQueue::RemoveBufferDoneCbCommandsByPortIndex : Removed Buffer Done @ Port Index [%d]"), aLocalPortIndex);
				}
			}
		while (pCurrentElement != pLastElement);
		}

	return somethingRemoved;

	}


TBool
COmxILCallbackManager::RCallbackManagerQueue::ExecuteBufferDoneCbCommandsByPortIndex(
	COmxILCallbackManager& aCbMgr,
	OMX_U32 aLocalPortIndex)
	{
    DEBUG_PRINTF2(_L8("RCallbackManagerQueue::ExecuteBufferDoneCbCommandsByPortIndex : Handle[%X]"), aCbMgr.ipHandle);

	TBool somethingExecuted = EFalse;

	if (KErrNone != DrainBackQueue())
		{
		aCbMgr.HandleInsufficientResources();
		return EFalse;
		}

	if(!iFrontQueue.IsEmpty())
		{
		TBool executed = EFalse;
		TDblQueIter<TFrontQueueElement> frontQueueIter(iFrontQueue);
		TFrontQueueElement* pLastElement = iFrontQueue.Last();
		TFrontQueueElement* pCurrentElement = 0;

		do
			{
			pCurrentElement = frontQueueIter++;
			__ASSERT_DEBUG(pCurrentElement && pCurrentElement->ipInfo,
						   User::Panic(KOmxILCallbackManagerPanicCategory, 1));
			executed = reinterpret_cast<CCallbackCommand*>(
				const_cast<TAny*>(pCurrentElement->ipInfo))->
				DoExecuteBufferDoneCbCommandByPortIndex(aCbMgr,
														aLocalPortIndex);
			if (executed)
				{
				somethingExecuted = ETrue;
				pCurrentElement->iLink.Deque();
				delete reinterpret_cast<CCallbackCommand*>(
					const_cast<TAny*>(pCurrentElement->ipInfo));
				delete pCurrentElement;
				DEBUG_PRINTF2(_L8("RCallbackManagerQueue::ExecuteBufferDoneCbCommandsByPortIndex : Executed Buffer Done @ Port Index [%d]"), aLocalPortIndex);
				}
			}
		while (pCurrentElement != pLastElement);
		}

	return somethingExecuted;

	}


//
// COmxILCallbackManager commands
//

TBool
COmxILCallbackManager::CCallbackCommand::DoRemoveBufferDoneCbCommandByBufferHeader(
	COmxILCallbackManager& /*aCbMgr*/,
	const OMX_BUFFERHEADERTYPE* /*apBufferHeader*/,
	OMX_DIRTYPE /*aDirection*/)
	{
	return EFalse;
	}

TBool
COmxILCallbackManager::CCallbackCommand::DoRemoveBufferDoneCbCommandByPortIndex(
	COmxILCallbackManager& /*aCbMgr*/,
	OMX_U32 /* aLocalPortIndex */)
	{
	return EFalse;
	}

TBool
COmxILCallbackManager::CCallbackCommand::DoExecuteBufferDoneCbCommandByPortIndex(
	COmxILCallbackManager& /*aCbMgr*/,
	OMX_U32 /* aLocalPortIndex */)
	{
	return EFalse;
	}

void
COmxILCallbackManager::CCompHandleRegistrationCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF2(_L8("CCompHandleRegistrationCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);
	aCbMgr.DoRegisterComponentHandle(ipHandle);
	}

void
COmxILCallbackManager::CClientCallbacksRegistrationCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF2(_L8("CClientCallbacksRegistrationCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);
	aCbMgr.DoRegisterILClientCallbacks(ipCallbacks, ipAppData);
	}

void
COmxILCallbackManager::CBufferMarkPropagationRegistrationCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF2(_L8("CBufferMarkPropagationRegistrationCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);

	OMX_ERRORTYPE omxError =
		aCbMgr.DoRegisterBufferMarkPropagationPort(
			iMarkPropagationInfo.iPortIndex,
			iMarkPropagationInfo.iPropagationPortIndex);

	if (OMX_ErrorInsufficientResources == omxError)
		{
		aCbMgr.HandleInsufficientResources();
		}
	}

void
COmxILCallbackManager::CBufferRemovalCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF2(_L8("CBufferRemovalCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);

	// Returned value not relevant
	aCbMgr.iPendingQueue.RemoveBufferDoneCbCommandsByBufferHeader(
		aCbMgr,
		ipBufferHeader,
		iDirection);

	// Returned value not relevant
	aCbMgr.iCommandQueue.RemoveBufferDoneCbCommandsByBufferHeader(
		aCbMgr,
		ipBufferHeader,
		iDirection);


	}

void
COmxILCallbackManager::CTunnelCallbackRegistrationCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF2(_L8("CTunnelCallbackRegistrationCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);

	if (!iTunnelInfo.iTunnelledComponentHandle)
		{
		// This is a tunnel deregistration command, then remove any pending
		// callbacks on that tunnel...
		aCbMgr.iCommandQueue.RemoveBufferDoneCbCommandsByPortIndex(
			aCbMgr,
			iTunnelInfo.iLocalPortIndex);
		}

	OMX_ERRORTYPE omxError =
		aCbMgr.DoRegisterTunnelCallback(iTunnelInfo.iLocalPortIndex,
										iTunnelInfo.iLocalPortDirection,
										iTunnelInfo.iTunnelledComponentHandle,
										iTunnelInfo.iTunnelledPortIndex);

	if (OMX_ErrorInsufficientResources == omxError)
		{
		aCbMgr.HandleInsufficientResources();
		}

	}


void
COmxILCallbackManager::CEventCallbackCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF2(_L8("CEventCallbackCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);

	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	switch(iData1)
		{
	case OMX_CommandStateSet:
		{
		aCbMgr.iPreviousState = aCbMgr.iCurrentState;
		aCbMgr.iCurrentState  = static_cast<OMX_STATETYPE>(iData2);

		DEBUG_PRINTF4(_L8("CEventCallbackCommand::operator() : Handle[%X] iPreviousState[%d] -> iCurrentState[%d]"), aCbMgr.ipHandle, aCbMgr.iPreviousState, aCbMgr.iCurrentState);

		if (OMX_StatePause == aCbMgr.iPreviousState &&
			OMX_StateIdle == aCbMgr.iCurrentState)
			{
			// Flush pending queue first...
			aCbMgr.ProcessQueue(aCbMgr.iPendingQueue);

			// ... and now signal command completion...
			omxError =
				aCbMgr.DoEventNotification(iEvent,
										   iData1,
										   iData2,
										   iExtraInfo);

			}
		else if (OMX_StatePause == aCbMgr.iPreviousState &&
			OMX_StateExecuting == aCbMgr.iCurrentState)
			{
			// Signal command completion first...
			omxError =
				aCbMgr.DoEventNotification(iEvent,
										   iData1,
										   iData2,
										   iExtraInfo);

			// ... and now flush...
			aCbMgr.ProcessQueue(aCbMgr.iPendingQueue);

			}
		else
			{
			// Signal command completion...
			omxError =
				aCbMgr.DoEventNotification(iEvent,
										   iData1,
										   iData2,
										   iExtraInfo);

			}

		}
		break;

	case OMX_CommandPortDisable:
	case OMX_CommandFlush:
		{
		// Process pending queue unconditionally...
		aCbMgr.iFlushPendingQueue = ETrue;

		// Flush first...
		if (OMX_ALL == iData2)
			{
			aCbMgr.ProcessQueue(aCbMgr.iPendingQueue);
			}
		else
			{
			aCbMgr.iPendingQueue.
				ExecuteBufferDoneCbCommandsByPortIndex(aCbMgr,
													   iData2);

			}

		aCbMgr.iFlushPendingQueue = EFalse;

		// ... and now signal command completion...
		omxError =
			aCbMgr.DoEventNotification(iEvent,
									   iData1,
									   iData2,
									   iExtraInfo);

		}
		break;

	default:
		{
		// Signal command completion...
		omxError =
			aCbMgr.DoEventNotification(iEvent,
									   iData1,
									   iData2,
									   iExtraInfo);

		}

		};

	if (OMX_ErrorInsufficientResources == omxError)
		{
		aCbMgr.HandleInsufficientResources();
		}

	}


void
COmxILCallbackManager::CBufferDoneCallbackCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred)
	{
    DEBUG_PRINTF2(_L8("CBufferDoneCallbackCommand::operator() : Handle[%X]"), aCbMgr.ipHandle);

	// Only send the buffer done callback if is not in Pause stae or if there
	// is a buffer flushing situation...

	if ((OMX_StatePause == aCbMgr.iCurrentState) &&
		(!aCbMgr.iFlushPendingQueue))
		{
		aHasBeenDeferred = ETrue;
		return;
		}

	OMX_ERRORTYPE omxError =
		aCbMgr.DoBufferDoneNotification(ipBufferHeader,
										iLocalPortIndex,
										iLocalPortDirection);

	if (OMX_ErrorInsufficientResources == omxError)
		{
		aCbMgr.HandleInsufficientResources();
		}

	}

TBool
COmxILCallbackManager::CBufferDoneCallbackCommand::DoRemoveBufferDoneCbCommandByBufferHeader(
	COmxILCallbackManager& aCbMgr,
	const OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection)
	{

	if (apBufferHeader == ipBufferHeader)
		{
		__ASSERT_DEBUG(aCbMgr.ipFsm,
					   User::Panic(KOmxILCallbackManagerPanicCategory, 1));
		__ASSERT_DEBUG(aDirection == OMX_DirInput ||
					   aDirection == OMX_DirOutput,
					   User::Panic(KOmxILCallbackManagerPanicCategory, 1));

		DEBUG_PRINTF4(_L8("DoRemoveBufferDoneCbCommandByBufferHeader() : Nofiying FSM : Handle[%X] aDirection[%X] apBufferHeader[%X]"), aCbMgr.ipHandle, aDirection, apBufferHeader);

		// Make sure the buffer contents are cleared...
		TOmxILUtil::ClearBufferContents(
			const_cast<OMX_BUFFERHEADERTYPE*>(apBufferHeader));

		if (aDirection == OMX_DirInput)
			{
			aCbMgr.ipFsm->EmptyThisBuffer(
				const_cast<OMX_BUFFERHEADERTYPE*>(apBufferHeader));
			}
		else
			{
			aCbMgr.ipFsm->FillThisBuffer(
				const_cast<OMX_BUFFERHEADERTYPE*>(apBufferHeader));
			}
		return ETrue;
		}

	return EFalse;

	}

//
// This method only prints some logging information for debugging purposes. For
// now, there's no other action to be performed as the deletion is done by the caller.
//
TBool
COmxILCallbackManager::CBufferDoneCallbackCommand::DoRemoveBufferDoneCbCommandByPortIndex(
	COmxILCallbackManager& /* aCbMgr */,
	OMX_U32 aLocalPortIndex)
	{

	if (iLocalPortIndex == aLocalPortIndex)
		{
		DEBUG_PRINTF2(_L8("CBufferDoneCallbackCommand::DoRemoveBufferDoneCbCommandByPortIndex() : FOUND -> PortIndex[%d]"), aLocalPortIndex);
		return ETrue;
		}

    DEBUG_PRINTF2(_L8("CBufferDoneCallbackCommand::DoRemoveBufferDoneCbCommandByPortIndex() : NOT FOUND -> PortIndex[%d]"), aLocalPortIndex);

	return EFalse;
	}


TBool
COmxILCallbackManager::CBufferDoneCallbackCommand::DoExecuteBufferDoneCbCommandByPortIndex(
	COmxILCallbackManager& aCbMgr,
	OMX_U32 aLocalPortIndex)
	{

	TBool executed = EFalse;

	if (iLocalPortIndex == aLocalPortIndex)
		{
		TBool hasBeenDeferred = EFalse;
		// The only use case for this method is during unconditional flushing
		// of the pending queue...
		__ASSERT_DEBUG(aCbMgr.iFlushPendingQueue,
					   User::Panic(KOmxILCallbackManagerPanicCategory, 1));
		this->operator()(aCbMgr, hasBeenDeferred);
		__ASSERT_DEBUG(!hasBeenDeferred,
					   User::Panic(KOmxILCallbackManagerPanicCategory, 1));
		executed = ETrue;
		}

	DEBUG_PRINTF3(_L8("CBufferDoneCallbackCommand::DoExecuteBufferDoneCbCommandByPortIndex() : %s FOUND  -> PortIndex[%d]"),
				  (executed ? "" : "NOT"), aLocalPortIndex);

	return executed;

	}


TBool
XOmxILCallbackManagerIfImpl::TBufferMarkPropagationInfo::Compare(
	const TBufferMarkPropagationInfo& aBmpi1,
	const TBufferMarkPropagationInfo& aBmpi2)
	{
	return (aBmpi1.iPortIndex == aBmpi2.iPortIndex ? ETrue : EFalse);
	}

TBool
XOmxILCallbackManagerIfImpl::TOutputPortBufferMarkInfo::Compare(
	const TOutputPortBufferMarkInfo& aOpbmi1,
	const TOutputPortBufferMarkInfo& aOpbmi2)
	{
	return (aOpbmi1.iPortIndex == aOpbmi2.iPortIndex ? ETrue : EFalse);
	}

COmxILCallbackManager::CPortSettingsChangeCommand::~CPortSettingsChangeCommand()
	{
	delete ipPortSettings;
	}

void
COmxILCallbackManager::CPortSettingsChangeCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF3(_L8("CPortSettingsChangeCommand::operator() : Handle[%X], iLocalPortIndex=[%d]"),
				  aCbMgr.ipHandle, iLocalPortIndex);

	OMX_ERRORTYPE omxError =
		aCbMgr.DoPortSettingsChangeNotification(iLocalPortIndex,
												iPortSettingsIndex,
												*ipPortSettings);

	if (OMX_ErrorInsufficientResources == omxError)
		{
		aCbMgr.HandleInsufficientResources();
		}

	}

#ifdef _OMXIL_COMMON_IL516C_ON	
void
COmxILCallbackManager::CEjectBuffersRequestCommand::operator()(
	COmxILCallbackManager& aCbMgr, TBool& /* aHasBeenDeferred */)
	{
    DEBUG_PRINTF3(_L8("CEjectBuffersRequestCommand::operator() : Handle[%X], iLocalOmxPortIndex=[%u]"),
				  aCbMgr.ipHandle, iLocalOmxPortIndex);

	OMX_ERRORTYPE omxError =
		aCbMgr.DoEjectBuffersRequest(iLocalOmxPortIndex);

	if (OMX_ErrorInsufficientResources == omxError)
		{
		aCbMgr.HandleInsufficientResources();
		}

	}
#endif //_OMXIL_COMMON_IL516C_ON
