/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements CUpsServer.	See class and function definitions for
* more information.
*
*/


/**
 @file
*/

#include "upsserver.h"
#include "policycache.h"
#include "pluginmanager.h"
#include <ups/upsdbw.h>
#include <ups/cliententity.h>
#include <ups/dialogcreator.h>
#include <ups/fingerprint.h>
#include <ups/policy.h>
#include <ups/policyevaluator.h>
#include <e32property.h>
#include "authoriser.h"
#include <sacls.h>
#include "upsserver_p.h"

namespace UserPromptService
{

static const TInt upsPolicyRangeCount = 8;
static const TInt upsPolicyRanges[upsPolicyRangeCount] = 
				{
					0,
					// Range 0 - 0 to EBaseSession-1
					// Not used
					CScsServer::EBaseSession,
					// Range 1 - EBaseSession  to EBaseSession | EMngmntRead-1
					//
					// These codes used to create subsessions and to query the policy
					// authorisation settings.
					//
					// (ESessSubsessFromThreadId/EGetClientConfigLength/EGetClientConfigData)
					//
					CScsServer::EBaseSession | EMngmntRead,
					// Range 2 - EBaseSession | EMngmntRead to EBaseSession | EMngmntDelete - 1
					//
					// Management READ APIs
					//
					CScsServer::EBaseSession | EMngmntDelete,
					// Range 3 - EBaseSession | EMngmntDelete to EBaseSession | EMngmntUpdate - 1
					// Management DELETE API (ie. delete entire database or selected entries).
					//
					CScsServer::EBaseSession | EMngmntUpdate,
					// Range 4 - EBaseSession | EMngmntUpdate to EBaseSession | ESwiObserver - 1
					// Management  UPDATE API (ie. change an existing decision).
					//
					CScsServer::EBaseSession | ESwiObserver,
					// Range 5 - EBaseSession | ESwiObserver to EBaseSubSession - 1
					// SWI observer management API.
					//
					CScsServer::EBaseSubSession,
					// Range 6 - EBaseSubSession to EBaseMustAllow-1
					//
					// System Server APIs
					// Authorise - (ESubsessPreparePrompt/ESubsessExecutePrompt)
					CScsServer::EBaseMustAllow	
					// Range 7 - EBaseMustAllow to KMaxTInt inclusive
					//
					// SCS internal APIs to create subsessions, cancel requests etc.
				};

static const TUint8 upsPolicyElementsIndex[upsPolicyRangeCount] = 
				{
					CPolicyServer::ENotSupported, // Range 0 - Not used
					CPolicyServer::EAlwaysPass, // Range 1 - Subsess and auth policy
					0, // Range 2 - Management READ APIs
					1, // Range 3 - Management DELETE APIs
					2, // Range 4 - Management UPDATE APIs
					3, // Range 5 - SWI observer APIs
					4, // Range 6 - System Server APIs
					CPolicyServer::EAlwaysPass // Range 7 - SCS internal
				};

// 0x102836C3 == Swi::KUidSwiObserver.iUid from swiobserver.h BUT we can not include that because SWI is optional
// and we are not!
static const TSecureId KSwiObserverSid(0x102836C3);
static const CPolicyServer::TPolicyElement upsPolicyPolicyElements[5] = 
{
				{_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient},	
				{_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient},	
				{_INIT_SECURITY_POLICY_C1(ECapabilityAllFiles), CPolicyServer::EFailClient},
				{_INIT_SECURITY_POLICY_S0(KSwiObserverSid), CPolicyServer::EFailClient},
				{_INIT_SECURITY_POLICY_C1(ECapabilityProtServ), CPolicyServer::EFailClient}
};

static const CPolicyServer::TPolicy upsPolicy =
				{
				CPolicyServer::EAlwaysPass, // Allow all connects
				upsPolicyRangeCount,
				upsPolicyRanges,
				upsPolicyElementsIndex,
				upsPolicyPolicyElements,
				};

_LIT_SECURITY_POLICY_S0(KAllowUpsServer, KUpsServerUid.iUid); //< Only the UPS server can update the P&S flag used to tell clients to re-read client authorisation policies
_LIT_SECURITY_POLICY_C1(KAllowProtServ, ECapabilityProtServ); //< All our system server clients will have ProtServ, so limit reading of the (internal) flag to them.

CUpsServer* CUpsServer::NewLC()
/**
	Factory function allocates new, initialized instance of
	CUpsServer which is left on the cleanup stack.

	@return					New, initialized instance of CUpsServer
							which is left on the cleanup stack.
 */
	{
	CUpsServer* self = new(ELeave) CUpsServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUpsServer::CUpsServer()
/**
	Initializes the superclass with this server's version.
 */
	:	CScsServer(UserPromptService::Version(), upsPolicy),
		iPolicyCache(iFs),
		iDbHandle(iFs)
	{
	// empty.
	}

void CUpsServer::ConstructL()
/**
	Second-phase construction initializes the superclass and
	starts the server.
 */
	{
	CScsServer::ConstructL(UserPromptService::KShutdownPeriodUs);
	User::LeaveIfError(iFs.Connect());

	TInt r = RProperty::Define(KUpsServiceConfigProperty, RProperty::EInt, KAllowProtServ, KAllowUpsServer);
	if(r != KErrAlreadyExists)
		{
		User::LeaveIfError(r);
		}

	iSwiWatcher = CSwiWatcher::NewL(*this);

	SetupL();

	StartL(UserPromptService::KUpsServerName);
	}

void CUpsServer::SetupL()
/**
	Setup memory variables which are not already setup.
	Used during intial construction and after a call to FreeUncompressableMemory.
 */
	{
	if(!iPolicyCache.IsOpen())
		{
		iPolicyCache.OpenL();
		}
	if(!iPluginManager)
		{
		iPluginManager = CPluginManager::NewL();
		}

	// Create/Open the database
	if(!iDbHandle.IsOpen())
		{
		iDbHandle.OpenL();
		}

	if(!iFlurryQueue)
		{
		iFlurryQueue = CAuthoriserFifo::NewL();
		}
	if(!iFlurryQueueBeingProcessed)
		{
		iFlurryQueueBeingProcessed = CAuthoriserFifo::NewL();
		}
	}

void CUpsServer::FreeUncompressableMemory()
/**
	Frees memory which can not be compressed down to a known level for OOM testing.
 */
	{
	iDbHandle.Close();
	
	if(iPluginManager)
		{
		iPluginManager->Unload();
		delete iPluginManager;
		iPluginManager = 0;
		}

	iPolicyCache.Release();
	}


CUpsServer::~CUpsServer()
/**
	Cleanup the server, in particular close the RFs session.
 */
	{
	iDisputed.Close();
	
	(void) RProperty::Delete(KUpsServiceConfigProperty);
	
	delete iFlurryQueueBeingProcessed;
	iFlurryQueueBeingProcessed = 0;

	delete iFlurryQueue;
	iFlurryQueue = 0;

	FreeUncompressableMemory();

	delete iSwiWatcher;
	iSwiWatcher = 0;
	iFs.Close();
	}

CScsSession* CUpsServer::DoNewSessionL(const RMessage2& /*aMessage*/)
/**
	Implement CScsServer by allocating a new instance of CUpsSession.

	@param	aMessage		Standard server-side handle to message.	 Not used.
	@return					New instance of CUpsSession which is owned by the
							caller.
 */
	{
	return CUpsSession::NewL(*this);
	}


void CUpsServer::DoPreHeapMarkOrCheckL()
/**
	This function is called by the framework just before settingchecking a heap mark. We need to compress/free memory
	down to a state which should be the same both before and after the test operations.
*/
	{
#ifdef _DEBUG
	if(iAsyncRequests.Count() != 0)
		{
		User::Leave(KErrServerBusy);
		}
	iDisputed.Compress();
	iFlurryQueue->Compress();
	iFlurryQueueBeingProcessed->Compress();
	FreeUncompressableMemory();
#endif
	}

void CUpsServer::DoPostHeapMarkOrCheckL()
/**
	Called immediately after setting/checking the heap mark and therefore just after DoPreHeapMarkOrCheckL.
	This function needs to re-allocate uncompressable data structures which were destroyed by DoPreHeapMarkOrCheckL.
*/
	{
#ifdef _DEBUG
	SetupL();
#endif
	}

void CUpsServer::GateKeeperL(CAuthoriser *aAuthoriser)
	/**
	   If no dialog is in progress, the server will note one is in
	   progress, and tell the aAuthoriser it can continue (by calling
	   ClearedToDisplayL).

	   Whenever a CAuthoriser finishes it MUST call our AuthoriserDone
	   function. This will allow us to cleanup our queues and clear
	   the next dialog to display.

	   If a dialog is already in progress, the the aAuthoriser will be
	   added to iFlurryQueue for later processing.

	   @param aAuthoriser to queue
	 */
	{
	if(iCurrentDialog)
		{
		// Add to queue of requests requiring re-processing later. This includes requests which want to display
		// a dialog and ones which matches a recordId which is under dispute (i.e. Where ForcePrompt has been
		// called and returned yes, but the prompt hasn't been displayed yet).
		iFlurryQueue->PushL(aAuthoriser);
		return;
		}
	iCurrentDialog = aAuthoriser;
	iCurrentDialog->ClearedToDisplayL();
	}

void CUpsServer::AuthoriserDone(CAuthoriser *aAuthoriser)
/**
	See CUpsServer::AuthoriserDoneL for documentation.
*/
	{
	TRAP_IGNORE(AuthoriserDoneL(aAuthoriser));
	}

void CUpsServer::AuthoriserDoneL(CAuthoriser *aAuthoriser)
	/**
	   The CAuthoriser has either completed the request, been
	   cancelled, or failed somehow.

	   If it is in either FIFO it needs removing.

	   If it is the current display dialog, then we need to check the
	   FIFOs, and maybe swap them, and call WakeupNextPending.
	*/
	{
	// Remove from lists.
	// Note the FIFO object does NOT leave if the object is not found.
	iFlurryQueue->RemoveL(aAuthoriser);
	iFlurryQueueBeingProcessed->RemoveL(aAuthoriser);

	if(aAuthoriser == iCurrentDialog)
		{
		iCurrentDialog = 0;

		if(iFlurryQueueBeingProcessed->IsEmpty())
			{
			// Swap queues
			CAuthoriserFifo *tmp = iFlurryQueue;
			iFlurryQueue = iFlurryQueueBeingProcessed;
			iFlurryQueueBeingProcessed = tmp;
			}
		WakeupNextPendingL();
		}
	}

void CUpsServer::WakeupNextPendingL()
	/**
	   This function does the following:-

	   1) If iFlurryQueueBeingProcessed is empty it returns.
	   
	   2) Removes the first authoriser from iFlurryQueueBeingProcessed
	   and calls SetPending to mark it as no longer pending.
	   
	   3) Increases its priority to EPriorityUserInput (this makes
	   sure it will be handled ahead of any incoming requests)

	   4) Sets it active and completes it so it runs.

	   It will run BEFORE any incoming requests.
	   
	   The first thing it must do is call this function again. This
	   ensures all pending requests are re-processed in order.

	   Normally it will then re-lookup its fingerprints in the
	   database, if found it may complete client request. If it
	   decides it still needs to display a dialog it should call
	   GateKeeper again.
	 */
	{
	if(iFlurryQueueBeingProcessed->IsEmpty())
		{
		return;
		}
	
	CAuthoriser *authoriser = iFlurryQueueBeingProcessed->PopL();
	// Set priority of authoriser to EPriorityHigh-1. This is higher
	// than any incoming work, but lower than deferred deletes.
	authoriser->SetPriority(CActive::EPriorityHigh - 1);
	authoriser->Wakeup();
	}

void CUpsServer::DisputeRecordIdL(TUint32 aRecordId)
/**
	Add the specified record to the list of disputed record IDs.
*/
	{
	DEBUG_PRINTF2(_L8("CUpsServer::DisputeRecordIdL(%d)\n"), aRecordId);
	User::LeaveIfError(iDisputed.InsertInOrder(aRecordId));
	}

void CUpsServer::UnDisputeRecordIdL(TUint32 aRecordId)
/**
	Deletes the specified record from the list of disputed record IDs.
*/
	{
	DEBUG_PRINTF2(_L8("CUpsServer::UnDisputeRecordIdL(%d)\n"), aRecordId);
	TInt i = iDisputed.FindInOrderL(aRecordId);
	User::LeaveIfError(i);
	iDisputed.Remove(i);
	}

TBool CUpsServer::IsRecordIdDisputed(TUint32 aRecordId) const
/**
	Checks if the specified record is under dispute. A record is disputed
	if the evaluator ForcePromptL call for a match on that record return ETrue to
	force a prompt to be displayed.
*/
	{
	TBool disputed = iDisputed.FindInOrder(aRecordId) >= 0;
	//RDebug::Printf("CUpsServer::IsRecordIdDisputed(%d) - returning %s\n", 
	//				aRecordId,
	//				(disputed)?("EFalse"):("EFalse"));
	return disputed;
	}

//
// Implementation of CSwiWatcher
//
CSwiWatcher *CSwiWatcher::NewL(CUpsServer &aUpsServer)
	{
	CSwiWatcher *self = new(ELeave) CSwiWatcher(aUpsServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiWatcher::CSwiWatcher(CUpsServer &aUpsServer)
	: CActive(CActive::EPriorityHigh),
	  iUpsServer(aUpsServer)
	{
	CActiveScheduler::Add(this);
	}

void CSwiWatcher::ConstructL()
/**
	Subscribe for notification for writes to the SWI P&S property.
	We do not check the actual value because it is not guaranteed that we will run for every change.
*/
	{
	User::LeaveIfError(iSwiProperty.Attach(KUidSystemCategory, KSAUidSoftwareInstallKeyValue));
	iSwiProperty.Subscribe(iStatus);
	SetActive();
	}

void CSwiWatcher::RunL()
/**
	SWI has changed state, so unload any unused plugins.
	We do this for EVERY state change which we manage to run for, which is overkill, but the unload call
	is cheap/free if there are no plugins which require unloading.
*/
	{
	User::LeaveIfError(iStatus.Int());
	
	iSwiProperty.Subscribe(iStatus);
	SetActive();

	// Tell the plugin manager to unload plugins which are NOT in use. 
	iUpsServer.iPluginManager->Unload();
	}

void CSwiWatcher::DoCancel()
	{
	iSwiProperty.Cancel();
	}

CSwiWatcher::~CSwiWatcher()
	{
	Cancel();
	iSwiProperty.Close();
	}


} // End of namespace UserPromptService
// End of file
