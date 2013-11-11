// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LogServViewChangeManager.h"
#include <s32mem.h>
#include "logservpanic.h"
#include "LogServDatabaseChangeInterface.h"

// Constants
const TInt KLogServViewChangeDefinitionGranularity = 10;
const TInt KLogServViewChangeQueueGranularity = 3;
const TInt KLogServViewChangeBufferGranularity = 40;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewChangeManager (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServViewChangeManager::CLogServViewChangeManager(MLogServDatabaseChangeInterface& aChangeInterface)
:	iChangeInterface(aChangeInterface), iPendingChanges(KLogServViewChangeQueueGranularity)
	{
	}

CLogServViewChangeManager::~CLogServViewChangeManager()
	{
	delete iTransientChangeDefinition;
	//
	iPendingChanges.ResetAndDestroy();
	iPendingChanges.Close();
	}

void CLogServViewChangeManager::ConstructL()
	{
	iTransientChangeDefinition = CLogChangeDefinition::NewL(KLogServViewChangeDefinitionGranularity);
	}

CLogServViewChangeManager* CLogServViewChangeManager::NewL(MLogServDatabaseChangeInterface& aChangeInterface)
	{
	CLogServViewChangeManager* self = new(ELeave) CLogServViewChangeManager(aChangeInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewChangeManager::ChangeTransactionPrepare()
	{
	iTransientChangeDefinition->Reset();
	}

void CLogServViewChangeManager::ChangeTransactionSubmitL(TLogId aId, TLogDatabaseChangeType aType, TInt aViewIndex)
	{
	iTransientChangeDefinition->AddL(aId, aType, aViewIndex);
	}

void CLogServViewChangeManager::ChangeTransactionCommitL()
	{
	// We can only tell the client-side view change observer about the changes if:
	//
	// (2) we actually have something to tell the client views (some changes)
	// (3) we have an outstanding client-side change message pointer
	//
	const TInt count = iTransientChangeDefinition->Count();
	if	(count > 0) // (2)
		{
		// Do we have any existing pending changes? We can't alter the contents of the
		// first pending change, since the client may already be preparing a client-side
		// buffer of the requisite size. We can, however, combine the 2nd batch of changes
		// with this new set so there is less IPC required (although more memory).
		const TInt pendingChangeCount = iPendingChanges.Count();
		if	(pendingChangeCount >= 2)
			{
			CLogChangeDefinition* changeDef = CLogChangeDefinition::NewL();
			CleanupStack::PushL(changeDef);
			//
			CBufBase* buffer = iPendingChanges[1];
			RBufReadStream readStream(*buffer);
			//
			readStream >> *changeDef;
			
			// Add new changes
			TLogId logId = KLogNullId;
			TLogDatabaseChangeType type = ELogChangeTypeUndefined;
			TInt viewIndex = 0;
			//
			for(TInt i=0; i<count; i++)
				{
				type = iTransientChangeDefinition->At(i, logId, viewIndex);
				changeDef->AddL(logId, type, viewIndex);
				}

			// Write the whole lot out again
			buffer->Delete(0, buffer->Size());

			RBufWriteStream writeStream(*buffer);
			writeStream << *changeDef;
			buffer->Compress();
			CleanupStack::PopAndDestroy(changeDef);
			}
		else
			{
			CBufBase* buffer = CBufFlat::NewL(KLogServViewChangeBufferGranularity);
			CleanupStack::PushL(buffer);

			// Externalize changes
			RBufWriteStream stream(*buffer);
			stream << *iTransientChangeDefinition; 
			buffer->Compress();

			// Add to container
			User::LeaveIfError(iPendingChanges.Append(buffer));
			CleanupStack::Pop(buffer);
			}

		// Notify if necessary - handles (3) implicitly
		NotifyClient();
		}

	// Free some memory
	iTransientChangeDefinition->Reset();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewChangeManager::DeliverChangesL(const RMessage2& aMessage)
	{
	if	(iPendingChanges.Count())
		{
		CBufBase* headItem = iPendingChanges[0];
		//
		const TInt expectedChangesSize = aMessage.Int1();
		const TPtr8 pBufferContents(headItem->Ptr(0));

		// Check buffer size is as we expect
		if	(expectedChangesSize != pBufferContents.Size())
			::PanicClientL(aMessage, ELogViewBadClientSideChangeBufferSize);
		else
			{
			// Write back to client-side
			aMessage.WriteL(2, pBufferContents);

			// Remove the item
			iPendingChanges.Remove(0);
			delete headItem;
			}
		}
	else
		::PanicClientL(aMessage, ELogViewNoPendingChangesToDeliver);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewChangeManager::RequestChangeNotifications(const RMessage2& aMessage)
	{
    if	(iClientSideChangeMessage == RMessagePtr2())
		{
		// Notify if we have any cached changes...
		iClientSideChangeMessage = aMessage;
		NotifyClient();
		}
	else
		PanicClient(aMessage, ELogViewChangeRequestAlreadyIssued);
	}

void CLogServViewChangeManager::RequestChangeNotificationsCancel()
	{
    if	(iClientSideChangeMessage != RMessagePtr2())
		CompleteClientChangeMessage(KErrCancel);

	// Zap all the pending changes too
	iPendingChanges.ResetAndDestroy();
	iPendingChanges.GranularCompress();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewChangeManager::NotifyClient()
	{
	const TInt count = iPendingChanges.Count();
    if	(iClientSideChangeMessage != RMessagePtr2() && count)
		{
		CBufBase* headItem = iPendingChanges[0];
		const TInt messageSize = headItem->Size();
		CompleteClientChangeMessage(messageSize);
		}
	}

void CLogServViewChangeManager::CompleteClientChangeMessage(TInt aCompletionCode)
	{
	__ASSERT_ALWAYS(iClientSideChangeMessage != RMessagePtr2(), Panic(ELogViewNoClientChangeMessageOutstanding));
	iClientSideChangeMessage.Complete(aCompletionCode);
	}
