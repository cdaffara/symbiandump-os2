/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements CUpsSession.	 See class and function definitions for
* more information.
*
*/


/**
 @file
*/

#include "upsserver.h"
#include "policycache.h"
#include <ups/upsdbw.h>
#include <scs/ipcstream.h>
#include <scs/nullstream.h>
#include "pluginmanager.h"
#include "viewevaluator.h"
#include "updateevaluator.h"
#include "policychangeevaluator.h"

namespace UserPromptService
{

CUpsSession* CUpsSession::NewL(CUpsServer &aServer)
/**
	Factory function allocates new instance of CUpsSession.

	@return					New, initialized instance of CUpsSession
							which is owned by the caller.
 */
	{
	CUpsSession* self = new(ELeave) CUpsSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();			// CScsSession implementation
	CleanupStack::Pop(self);
	return self;
	}

CUpsSession::CUpsSession(CUpsServer &aServer)
/**
	This private constructor prevents direct instantiation.
 */
 :	CScsSession(aServer), iDbViewHandle(aServer.iDbHandle, this)
	{
	// empty.
	//RDebug::Printf("0x%x CUpsSession(server %x) (&iDbViewHandle %x)\n", this, &aServer, &iDbViewHandle);
	}

CUpsSession::~CUpsSession()
/**
	The base class destructor destroys any remaining subsessions
	or outstanding requests.
 */
	{
	//RDebug::Printf("0x%x ~CUpsSession (&iDbViewHandle %x)\n", this, &iDbViewHandle);
	CleanupView();
	iServiceConfig.Close();

	iDbViewHandle.Close();
	}

TBool CUpsSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	Note the subsession creation command is automatically sent to
	DoCreateSubsessionL, and not this function.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.	 Not used.
 */
	{
	TSessionFunction f = static_cast<TSessionFunction>(aFunction);
	//RDebug::Printf("0x%x CUpsSession::DoServiceL function %d\n", this, f);
	switch (f)
		{
		case EGetClientConfigLength:
			{
			iServiceConfig.Close();
			UpsServer()->iPolicyCache->ServiceConfigL(aMessage.SecureId(), iServiceConfig);

			TPckgBuf<TInt> lengthBuf;
			lengthBuf() = iServiceConfig.Count();
			aMessage.WriteL(0, lengthBuf);
			return ETrue; // Complete request with KErrNone
			}

		case EGetClientConfigData:
			{
			TInt count = iServiceConfig.Count();
			for(TInt i=0; i < count; ++i)
				{
				const TServiceConfig &sc = iServiceConfig[i];
				TPtrC8 ptr((TUint8 *) &sc, sizeof(TServiceConfig));
				aMessage.WriteL(0, ptr, i*sizeof(TServiceConfig));
				}
			iServiceConfig.Close();
			return ETrue; // Complete request with KErrNone
			}

		case EDeleteDatabase:
			TRAPD(err, UpsServer()->iDbHandle->DeleteDatabaseL(UpsServer()->iFs));
			UpsServer()->iDbHandle.Close();
			User::LeaveIfError(err);
			return ETrue; // Complete request with KErrNone

		case ECreateView:
			{
			if(iManagementView != 0)
				{
				User::Leave(KErrServerBusy);
				}
			
			CViewEvaluator *viewEvaluator = CViewEvaluator::NewLC(this, aMessage);
			viewEvaluator->TransferToScsFrameworkL();
			CleanupStack::Pop(viewEvaluator); // view now owned by SCS framework

			/**
			   	The CViewEvaluator is now responsible for completing the request,
	   			so we must NOT leave.
	   		*/
			viewEvaluator->StartEvaluatingView();
			return EFalse; // Do not complete client request - view evaulator will...
			}

		case ENextMatch:
			{
			if((iManagementView == 0) || (iRecord == 0))
				{
				User::Leave(KErrAbort);
				}

			// Copy the record to arg 0
			RIpcWriteStream ipcstream;
			ipcstream.Open(aMessage, 0);
			CleanupClosePushL(ipcstream);

			ipcstream << *iRecord;

			CleanupStack::PopAndDestroy(&ipcstream);

			// Update arg 1 with the length of the next record.
			PrefetchRecordAndWriteLengthToClientL(aMessage);
			return ETrue; // Complete client request with KErrNone
			}

		case ECloseView:
			{
			CleanupView();
			return ETrue; // Complete client request with KErrNone
			}

		case ERemoveDecisions:
			{
			// Read filter from the client arg 0
			RIpcReadStream ipcstream;
			ipcstream.Open(aMessage, 0);
			CleanupClosePushL(ipcstream);
			CDecisionFilter *filter = CDecisionFilter::NewLC();
			ipcstream >> *filter;

			UpsServer()->iDbHandle->RemoveDecisionsL(*filter);

			CleanupStack::PopAndDestroy(filter);
			CleanupStack::PopAndDestroy(&ipcstream);
			return ETrue; // Complete client request with KErrNone
			}

		case EUpdateDecision:
			{
			CUpdateEvaluator *updateEvaluator = CUpdateEvaluator::NewLC(this, aMessage);
			updateEvaluator->TransferToScsFrameworkL();
			CleanupStack::Pop(updateEvaluator); // view now owned by SCS framework

			/**
			   	The CViewEvaluator is now responsible for completing the request,
	   			so we must NOT leave.
	   		*/
			updateEvaluator->StartUpdate();
			return EFalse; // Do not complete client request - view evaulator will...
			}

		case EDeleteDecisionsForExe:
			{
			TUid exeSid;
			exeSid.iUid = aMessage.Int0();

			CDecisionFilter *filter = CDecisionFilter::NewLC();
			filter->SetClientSid(exeSid, EEqual);

			UpsServer()->iDbHandle->RemoveDecisionsL(*filter);

			CleanupStack::PopAndDestroy(filter);

			return ETrue; // Complete client request with KErrNone
			}
			
		case ENotifyPluginsMayHaveChanged:
			// Tell plugin manager to unload, and hence reload, plugins ASAP.
			UpsServer()->iPluginManager->Unload();
			return ETrue; // Complete client request with KErrNone
			
		case ENotifyPolicyFilesChanged:
			{
			CPolicyChangeEvaluator *changeEvaluator = CPolicyChangeEvaluator::NewLC(UpsServer()->iPolicyCache, this, aMessage);
			changeEvaluator->TransferToScsFrameworkL();
			CleanupStack::Pop(changeEvaluator); // Nnow owned by SCS framework

			/**
			   	The CPolicyChangeEvaluator is now responsible for completing the request,
	   			so we must NOT leave.
	   		*/
			changeEvaluator->StartUpdate();

			// Release our reference to the policy cache
			UpsServer()->iPolicyCache.Release();
			
			return EFalse; // Do not complete client request - policy change  evaluator will...
			}
		BULLSEYE_OFF
		default:
			break;
		BULLSEYE_RESTORE
		}

	BULLSEYE_OFF
	User::Leave(KErrNotSupported);
	/*lint -unreachable */
	return ETrue;
	BULLSEYE_RESTORE
	}

CScsSubsession* CUpsSession::DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage)
/**
	Override CScsSession by allocating a new subsession object.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.	 Not used.
	@return					New, initialized instance of CUpsSubsession, which is
							owned by the caller.
 */
	{
	TSessionFunction f = static_cast<TSessionFunction>(aFunction);

	switch (f)
		{
	case ESessSubsessFromThreadId:
		// create a subsession object curried on the supplied thread ID.
		return CUpsSubsession::NewL(*this, aMessage);

	default:
		User::Leave(KErrNotSupported);
		/*lint -unreachable */
		return 0;			// avoid compiler warning
		}
	}

void CUpsSession::PrefetchRecordAndWriteLengthToClientL(const RMessagePtr2& aMessage)
/**
	Retrieve the next record from the iManagementView and return its length to the 
	client in arg 1.
*/
	{
	if(iManagementView == 0)
		{
		User::Leave(KErrEof);
		}

	TPckgBuf<TUint32> nextRecordLenghthBuf;

	delete iRecord;
	iRecord = 0;
	TRAPD(err, iRecord = iManagementView->NextDecisionL());
	if((err != KErrNone) || (iRecord == 0))
		{
		nextRecordLenghthBuf() = 0;
		}
	else
		{
		RNullWriteStream nullstream;
		nullstream << *iRecord;
		nextRecordLenghthBuf() = nullstream.BytesWritten();
		}

	aMessage.WriteL(1, nextRecordLenghthBuf);
	}

void CUpsSession::CleanupView()
/**
	Cleanup view objects
*/
	{
	delete iManagementView;
	iManagementView = 0;
	delete iRecord;
	iRecord = 0;
	}

void CUpsSession::DbHandleAboutToBeDeleted()
/**
	Master DB handle is about to be deleted, so we must delete our view now.
	This will also cause the next RUpsManageMent::NextMatchL call to leave.
*/
	{
	if(iManagementView)
		{
		iManagementView->Cancel();
		CleanupView();
		}
	}

} // End of namespace UserPromptService
// End of file
