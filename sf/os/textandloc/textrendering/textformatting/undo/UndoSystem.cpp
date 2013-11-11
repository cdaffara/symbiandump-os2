/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "UndoSystemImpl.h"
#include "AssertFileAndLine.h"

using namespace UndoSystem;

namespace UndoSystem
{
/**
 * Gatekeeper that overrides none of the default functionality.
 * @since App-frameworks6.1
 */
class CDefaultGatekeeper : public CBase, public MNotUndoableGatekeeper {};
}


//////////////////////////////
//							//
//	MNotUndoableGatekeeper  //
//							//
//////////////////////////////
EXPORT_C TBool MNotUndoableGatekeeper::RetryOutOfMemoryL(TInt)
	{
	return EFalse;
	}

EXPORT_C TBool MNotUndoableGatekeeper::AllowNotUndoableL(TInt aReason)
	{
	if (aReason != KErrNotSupported)
		User::Leave(aReason);
	return ETrue;
	}

///////////////////////
//					 //
//	CCommandManager  //
//					 //
///////////////////////

CCommandManager::~CCommandManager()
	{
	ResetUndo();
	delete iFuture;
	delete iPast;
	delete iDefaultGatekeeper;
	}

CCommandManager::CCommandManager() : iRefCount(1)
	{
	}

EXPORT_C void CCommandManager::NewReference()
	{
	++iRefCount;
	}

EXPORT_C void CCommandManager::Release()
	{
	if (--iRefCount == 0)
		delete this;
	}

void CCommandManager::ConstructL()
	{
	iFuture = CCommandHistory::NewL();
	iPast = CCommandHistory::NewL();
	iDefaultGatekeeper = new(ELeave) CDefaultGatekeeper();
	iCurrentGatekeeper = iDefaultGatekeeper;
	}

EXPORT_C CCommandManager* CCommandManager::NewL()
	{
	CCommandManager* r = new(ELeave) CCommandManager;
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

TInt CCommandManager::ExecuteSingleCommandL(const CSingleCommand& aCommand, CCommandHistory& aUndo)
	{
	ASSERT(iCurrentGatekeeper);

	TInt retries = 0;
	CCommand* inverse = 0;
	TBool addingToLast = EFalse;
	while (!aUndo.UndoHasBeenWaived())
		{
		TRAPD(err, addingToLast = CreateAndPrepareToAddInverseL(aCommand, aUndo, inverse));
		if (err == KErrNone)
			break;
		if (err == KErrNoMemory)
			{
			if ( iCurrentGatekeeper->RetryOutOfMemoryL(retries++) )
				continue;
			}
		if ( !iCurrentGatekeeper->AllowNotUndoableL(err) )
			return KErrAbort;
		aUndo.SetUndoWaived();
		aUndo.Reset();
		break;
		}

	ASSERT(!(addingToLast && inverse));

	if (inverse)
		CleanupStack::PushL(inverse);

	TInt error = KErrNone;
	retries = 0;
	for (;;)
		{
		TRAPD(leaveError,
			error = aCommand.ExecuteL();
			);
		if (leaveError == KErrNone)
			break;
		if (leaveError != KErrNoMemory
			|| !iCurrentGatekeeper->RetryOutOfMemoryL(retries++))
			User::Leave(leaveError);
		}
	if (error < 0)
		{
		// execution caused an error: no inverse is required.
		if (inverse)
			CleanupStack::PopAndDestroy(inverse);
		return error;
		}

	if (addingToLast)
		{
		aCommand.AddInverseToLast(*aUndo.TopSingleCommand());
		}
	else if (inverse)
		{
		aUndo.AddCommand(inverse);
		CleanupStack::Pop(inverse);
		}

	return KErrNone;
	}

TInt CCommandManager::ExecuteBatchCommandL(CBatchCommand& aCommand, CCommandHistory& aUndo)
	{
	aUndo.BeginBatchLC();
	TInt err = KErrNone;
	for (CSingleCommand* single = aCommand.Top(); (err != KErrAbort) && single;
		delete aCommand.Pop(), single = aCommand.Top())
		{
		err = ExecuteSingleCommandL(*single, aUndo);
		if (aUndo.UndoHasBeenWaived())
			aUndo.Reset();
		}
	CleanupStack::PopAndDestroy();	// close batch
	return err == KErrAbort? KErrAbort : KErrNone;
	}

void CCommandManager::MoveHistoryL(CCommandHistory& aFrom, CCommandHistory& aTo)
	{
	ASSERT(iFuture);
	ASSERT(iPast);

	if (aFrom.IsAtBookmark())
		aTo.SetBookmark();

	TInt err = KErrNone;
	CCommand* command = aFrom.Top();
	if (!command)
		return;
	CSingleCommand* single = command->Single();
	if (single)
		{
		err = ExecuteSingleCommandL(*single, aTo);
		}
	else
		{
		ASSERT(command->Batch());
		err = ExecuteBatchCommandL(*command->Batch(), aTo);
		}

	if (0 <= err)
		delete aFrom.Pop();

	aFrom.Clean();
	aTo.Clean();
	}

TBool CCommandManager::CreateAndPrepareToAddInverseL(const CSingleCommand& aCommand,
	CCommandHistory& aUndo, CCommand*& aInverse)
	{
	aInverse = 0;

	// commands should not be sneaked beyond the bookmark
	if (!IsAtBookmark())
		{
		CSingleCommand* top = aUndo.TopSingleCommand();
		if ( top && aCommand.PrepareToAddInverseToLastL( *top ) )
			return ETrue;
		}

	CCommand* inverse = aCommand.CreateInverseL();
	// null return value indicates "nothing to do", not "inverse not possible"
	if (inverse)
		{
		CleanupStack::PushL(inverse);
		aUndo.PrepareToAddCommandL(inverse);
		CleanupStack::Pop(inverse);
		}
	aInverse = inverse;
	return EFalse;
	}

EXPORT_C void CCommandManager::BeginBatchLC()
	{
	iPast->BeginBatchLC();
	}

EXPORT_C void CCommandManager::UndoL()
	{
	ASSERT(!iPast->IsWithinBatch());
	MoveHistoryL(*iPast, *iFuture);
	}

EXPORT_C void CCommandManager::RedoL()
	{
	ASSERT(!iPast->IsWithinBatch());
	MoveHistoryL(*iFuture, *iPast);
	}

EXPORT_C TBool CCommandManager::CanUndo() const
	{
	ASSERT(iPast);
	return !iPast->IsEmpty();
	}

EXPORT_C TBool CCommandManager::CanRedo() const
	{
	ASSERT(iFuture);
	return !iFuture->IsEmpty();
	}

EXPORT_C void CCommandManager::ResetUndo()
	{
	ASSERT(iPast);
	ASSERT(!iPast->IsWithinBatch());
	ASSERT(iFuture);
	iPast->Reset();
	iFuture->Reset();
	}

EXPORT_C void CCommandManager::SetBookmark()
	{
	// Bookmarks cannot be set in the middle of batches.
	// More sophisticated logic could be written, but it is unlikely to raise
	// any issues.
	if (!iPast->IsWithinBatch())
		iPast->SetBookmark();
	}

EXPORT_C TBool CCommandManager::IsAtBookmark() const
	{
	return iPast->IsAtBookmark() | iFuture->IsAtBookmark();
	}

EXPORT_C TInt CCommandManager::ExecuteL(const CSingleCommand& aCommand)
	{
	TInt retval = ExecuteSingleCommandL(aCommand, *iPast);
	if (0 <= retval)
		iFuture->Reset();
	return retval;
	}

EXPORT_C MNotUndoableGatekeeper*
	CCommandManager::SetGatekeeper(MNotUndoableGatekeeper* a)
	{
	ASSERT(iCurrentGatekeeper);
	MNotUndoableGatekeeper* current = iCurrentGatekeeper;
	iCurrentGatekeeper = a? a : iDefaultGatekeeper;
	return current;
	}

EXPORT_C void CCommandManager::SetMaxItems(TInt aMaxItems)
	{
	ASSERT(iPast);
	iPast->SetMaxItems(aMaxItems);
	}

//////////////////////
//					//
//	CSingleCommand  //
//					//
//////////////////////

EXPORT_C TUid CSingleCommand::FamilyUid() const
	{
	return KNullUid;
	}

EXPORT_C CSingleCommand* CSingleCommand::Single()
	{
	return this;
	}

EXPORT_C CBatchCommand* CSingleCommand::Batch()
	{
	return 0;
	}

EXPORT_C TBool CSingleCommand::PrepareToAddInverseToLastL(CSingleCommand&) const
	{
	return EFalse;
	}

EXPORT_C void CSingleCommand::AddInverseToLast(CSingleCommand&) const
	{
	Panic(KAddToLastOnlyHalfImplemented);
	}

EXPORT_C CCommand* CSingleCommand::CreateInverseL() const
	{
	User::Leave(KErrNotSupported);
	return 0;
	}

/////////////////////
//				   //
//	CBatchCommand  //
//				   //
/////////////////////

CBatchCommand::CBatchCommand() {}
EXPORT_C CBatchCommand::~CBatchCommand()
	{
	if (iStack)
		{
		iStack->Reset();
		delete iStack;
		}
	}

void CBatchCommand::ConstructL()
	{
	iStack = CSingleCommandStack::NewL();
	}

EXPORT_C CBatchCommand* CBatchCommand::NewLC()
	{
	CBatchCommand* r = new(ELeave) CBatchCommand();
	CleanupStack::PushL(r);
	r->ConstructL();
	return r;
	}

EXPORT_C CBatchCommand* CBatchCommand::NewL()
	{
	CBatchCommand* r = NewLC();
	CleanupStack::Pop(r);
	return r;
	}

EXPORT_C CBatchCommand* CBatchCommand::Batch()
	{
	return this;
	}

EXPORT_C CSingleCommand* CBatchCommand::Single()
	{
	return 0;
	}

EXPORT_C CSingleCommand* CBatchCommand::Pop()
	{
	return iStack->Pop();
	}

EXPORT_C CSingleCommand* CBatchCommand::Top() const
	{
	return iStack->Top();
	}

EXPORT_C void CBatchCommand::PrepareToPushL(CCommand* aCommand)
	{
	CBatchCommand* batch = aCommand->Batch();
	if (!batch)
		{
		ASSERT(aCommand->Single());
		iStack->PrepareToPushL(1);
		}
	else
		iStack->PrepareToPushL(batch->iStack->Count());
	}

EXPORT_C void CBatchCommand::Push(CCommand* aCommand)
	{
	CSingleCommand* single = aCommand->Single();
	if (single)
		{
		iStack->Push(single);
		return;
		}
	CBatchCommand* batch = aCommand->Batch();
	ASSERT(batch);
	iStack->Concatenate(*batch->iStack);
	delete aCommand;
	}

EXPORT_C void CBatchCommand::PushL(CCommand* aCommand)
	{
	CleanupStack::PushL(aCommand);
	PrepareToPushL(aCommand);
	Push(aCommand);
	CleanupStack::Pop(aCommand);
	}
