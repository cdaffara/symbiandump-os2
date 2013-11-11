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

namespace UndoSystemImpl
{
const TInt KCommandStackGranularity = 10;
_LIT(KUndoPanic, "Undo System");
}

using namespace UndoSystemImpl;


void UndoSystem::Panic(TPanicCode aCode)
	{
	User::Panic(KUndoPanic, aCode);
	}

/////////////////////
//				   //
//	CCommandStack  //
//				   //
/////////////////////

CCommandStack::CCommandStack() : iEnd(0), iBookmark(0) {}

CCommandStack::~CCommandStack()
	{
	if (iStack)
		{
		Reset();
		delete iStack;
		}
	}

void CCommandStack::ConstructL()
	{
	iStack = new(ELeave) CArrayFixSeg<CCommand*>(KCommandStackGranularity);
	}

CCommandStack* CCommandStack::NewL()
	{
	CCommandStack* r = new(ELeave) CCommandStack();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

TInt CCommandStack::Count() const
	{
	ASSERT(iEnd <= iStack->Count());
	return iEnd;
	}

void CCommandStack::Reset()
	{
	PruneTo(0);
	iBookmark = -1;
	}

CCommand* CCommandStack::Top() const
	{
	ASSERT(0 <= iEnd);
	ASSERT(iEnd <= iStack->Count());
	return iEnd == 0? 0 : iStack->At(iEnd - 1);
	}

CCommand* CCommandStack::Pop()
	{
	ASSERT(iEnd <= iStack->Count());
	CCommand* top = Top();
	__ASSERT_ALWAYS(top, UndoSystem::Panic(KCommandStackUnderflow));
	TInt numSlots = iStack->Count() - iEnd + 1;
	ASSERT(numSlots <= iStack->Count());
	iStack->Delete(iStack->Count() - numSlots, numSlots);
	iEnd = iStack->Count();
	if (iEnd < iBookmark)
		iBookmark = -1;
	return top;
	}

void CCommandStack::PruneTo(TInt aNumberOfItems)
	{
	ASSERT(iEnd <= iStack->Count());
	iStack->Delete(iEnd, iStack->Count() - iEnd);
	iEnd = iStack->Count();
	if (aNumberOfItems < iEnd)
		{
		TInt numberToDestroy = iEnd - aNumberOfItems;
		for (TInt i = 0; i != numberToDestroy; ++i)
			{
			delete iStack->At(i);
			}
		iStack->Delete(0, numberToDestroy);
		iStack->Compress();
		iEnd = aNumberOfItems;
		iBookmark -= aNumberOfItems;
		}
	}

void CCommandStack::PrepareToPushL(TInt aNumberOfItems)
	{
	iStack->ResizeL(iEnd + aNumberOfItems);
	}

void CCommandStack::Push(CCommand* aCommand)
	{
	ASSERT(aCommand);
	__ASSERT_ALWAYS(iEnd < iStack->Count(), UndoSystem::Panic(KCommandStackPushNotPrepared));
	iStack->At(iEnd) = aCommand;
	++iEnd;
	}

void CCommandStack::Concatenate(CCommandStack& aStack)
	{
	TInt thisCount = iEnd;
	iEnd += aStack.iEnd;
	__ASSERT_DEBUG(iEnd <= iStack->Count(),
		UndoSystem::Panic(KCommandStackPushNotPrepared));
	for (TInt i = 0; i != aStack.iEnd; ++i, ++thisCount)
		iStack->At(thisCount) = (aStack.iStack->At(i));
	aStack.iEnd = 0;
	aStack.iStack->Reset();
	}

void CCommandStack::SetBookmark()
	{
	iBookmark = iEnd;
	}

TBool CCommandStack::IsAtBookmark() const
	{
	return iBookmark == iEnd? ETrue : EFalse;
	}

CSingleCommandStack* CSingleCommandStack::NewL()
	{
	CSingleCommandStack* r = new(ELeave) CSingleCommandStack;
	CleanupStack::PushL(r);
	r->iStack.ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

///////////////////////
//					 //
//	CCommandHistory  //
//					 //
///////////////////////

CCommandHistory::CCommandHistory()
	: iMaxItems(KMaxTInt >> 1)
	{
	}

CCommandHistory::~CCommandHistory()
	{
	delete iStack;
	}

void CCommandHistory::ConstructL()
	{
	iStack = CCommandStack::NewL();
	iCurrent = 0;
	}

CCommandHistory* CCommandHistory::NewL()
	{
	CCommandHistory* r = new(ELeave) CCommandHistory();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}

void CCommandHistory::Prune()
	{
	if (iMaxItems < iStack->Count())
		iStack->PruneTo(iMaxItems);
	}

void CCommandHistory::SetMaxItems(TInt aMaxItems)
	{
	ASSERT(0 < aMaxItems);
	iMaxItems = aMaxItems;
	Prune();
	}

void CCommandHistory::CloseBatch(void* a)
	{
	CCommandHistory* aThat = reinterpret_cast<CCommandHistory*>(a);
	aThat->iCurrent = 0;
	if (aThat->iBatchUndoHasBeenWaived)
		aThat->Reset();
	}

void CCommandHistory::DownBatchLevel(void*) {}

TBool CCommandHistory::IsWithinBatch() const
	{
	return iCurrent? ETrue : EFalse;
	}

void CCommandHistory::BeginBatchLC()
	{
	if (iCurrent)
		{
		CleanupStack::PushL(TCleanupItem(DownBatchLevel, this));
		}
	else
		{
		iStack->PrepareToPushL(1);
		iBatchUndoHasBeenWaived = EFalse;
		CleanupStack::PushL(TCleanupItem(CloseBatch, this));
		CBatchCommand* batch = CBatchCommand::NewL();
		iCurrent = batch;
		iStack->Push(batch);
		}
	}

TBool CCommandHistory::UndoHasBeenWaived() const
	{
	return iCurrent? iBatchUndoHasBeenWaived : EFalse;
	}

void CCommandHistory::SetUndoWaived()
	{
	iBatchUndoHasBeenWaived = ETrue;
	}

void CCommandHistory::Reset()
	{
	iStack->Reset();
	iCurrent = 0;
	}

void CCommandHistory::PrepareToAddCommandL(CCommand* aCommand)
	{
	if (iCurrent)
		iCurrent->PrepareToPushL(aCommand);
	else
		iStack->PrepareToPushL(1);
	}

void CCommandHistory::AddCommand(CCommand* aCommand)
	{
	if (iCurrent)
		iCurrent->Push(aCommand);
	else
		iStack->Push(aCommand);
	if (!iCurrent)
		Prune();
	}

CSingleCommand* CCommandHistory::TopSingleCommand() const
	{
	CCommand* top = Top();

	if (!top)
		return 0;

	// if the top of the undo stack is an empty batch, then we are starting a new
	// batch command and so do not want to combine. Otherwise, if the top is a batch
	// with elements, we want to see if we can combine with the top one.
	CBatchCommand* batch = top->Batch();
	return batch? batch->Top() : top->Single();
	}

CCommand* CCommandHistory::Top() const
	{
	return iStack->Top();
	}

CCommand* CCommandHistory::Pop()
	{
	ASSERT(!iCurrent);
	return iStack->Pop();
	}

void CCommandHistory::Clean()
	{
	ASSERT(!iCurrent);
	CCommand* command = Top();
	if (!command)
		return;
	CBatchCommand* batch = command->Batch();
	if (!batch)
		return;
	if (batch->IsEmpty())
		{
		delete Pop();
		}
	}

void CCommandHistory::SetBookmark()
	{
	iStack->SetBookmark();
	}

TBool CCommandHistory::IsAtBookmark()
	{
	return iStack->IsAtBookmark();
	}

