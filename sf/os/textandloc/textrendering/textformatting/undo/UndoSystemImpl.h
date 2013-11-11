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


#ifndef UNDOSYSTEMIMPL_H_
#define UNDOSYSTEMIMPL_H_

#include <e32base.h>
#include "UndoSystem.h"

namespace UndoSystem
{
class CCommand;
class CSingleCommand;
class CBatchCommand;

/**
 * Undo panic codes
 *
 * @internalComponent
 */
enum TPanicCode
	{
	/**
	 * CSingleCommand::PrepareToAddInverseToLast() overridden to return ETrue, but
	 * CSingleCommand::AddInverseToLast() not overridden.
	 */
	KAddToLastOnlyHalfImplemented = 1,
	/**
	 * CCommandStack or CSingleCommandStack::Pop() called on an empty stack
	 */
	KCommandStackUnderflow,
	/**
	 * CCommandStack or CSingleCommandStack::Push() called without adequate
	 * space being reserved via PrepareToPushL()
	 */
	KCommandStackPushNotPrepared,
	/**
	 * No shared undo system passed when expected
	 */
	KEditorUndoNoCommandManager
	};

/**
 * Panic from Undo DLL
 *
 * @internalComponent
 */
void Panic(TPanicCode aCode);

/**
 * Stack of commands. A bookmark is maintianed so that we can tell if the stack
 * is in the same state it was at a previous time.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CCommandStack) : public CBase
	{
public:
	static CCommandStack* NewL();
	~CCommandStack();
	CCommandStack();
	void ConstructL();

	CCommand* Top() const;
	CCommand* Pop();
	/**
	 * Allows aNumberOfItems Push()s to be done before the next
	 * call to another non-const member function apart from Push().
	 * aNumberOfItems must be non-negative.
	 */
	void PrepareToPushL(TInt aNumberOfItems);
	void Push(CCommand* aCommand);
	void PruneTo(TInt aNumberOfItems);
	TInt Count() const;
	/**
	 * Removes all elements from the stack and deletes the bookmark.
	 */
	void Reset();
	/**
	 * Adds aStack to the top of this CCommandStack, emptying it completely.
	 * Enough items must have been reserved with PrepareToPushL(TInt).
	 */
	void Concatenate(CCommandStack& aStack);
	/**
	 * Sets the current position in the stack as the location of the bookmark.
	 * The bookmark will be lost if the bookmarked object is removed from the
	 * stack. Bookmark begins set at the zero position.
	 */
	void SetBookmark();
	/**
	 * Returns true iff we are at the current bookmark position.
	 */
	TBool IsAtBookmark() const;

private:
	CArrayFix<CCommand*>* iStack;
	/**
	 * One-past-top element in the array. Less than Count() after a call
	 * to PrepareToPushL() with non-zero argument.
	 */
	TInt iEnd;
	/**
	 * The bookmark into the stack. We are at the bookmark when iEnd ==
	 * iBookmark. No bookmark is represented by iBookmark < 0.
	 */
	TInt iBookmark;
	};

/**
 * Stack of single commands
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
class CSingleCommandStack : public CBase
	{
public:
	static CSingleCommandStack* NewL();

	inline CSingleCommand* Top() const;
	inline CSingleCommand* Pop();
	inline void PrepareToPushL(TInt aNumberOfItems);
	inline void Push(CSingleCommand* aCommand);
	inline void PruneTo(TInt aNumberOfItems);
	inline TInt Count() const;
	inline void Reset();
	inline void Concatenate(CSingleCommandStack& aStack);

private:
	CSingleCommandStack() {}

	CCommandStack iStack;
	};

/**
 * A stack of commands supporting batching and a flag for whether a batch
 * has had its undo option waived. A bookmark is also supported, so that we can
 * tell whether the history is in the same state as it was at a previous time.
 *
 * @internalComponent
 * @since App-frameworks6.1
 */
NONSHARABLE_CLASS(CCommandHistory) : public CBase
	{
public:
	~CCommandHistory();
	static CCommandHistory* NewL();

	/**
	 * Returns the command at the top of the stack.
	 */
	CCommand* Top() const;
	/**
	 * Returns the single command at the top of the stack.
	 */
	CSingleCommand* TopSingleCommand() const;
	/**
	 * Allocates enough resources for one call to AddCommand(CCommand* aCommand).
	 */
	void PrepareToAddCommandL(CCommand* aCommand);
	/**
	 * Adds the command to the top of the stack. PrepareToAddCommandL() must
	 * have been called successfully since the last call to AddCommand or NewL.
	 */
	void AddCommand(CCommand*);
	/**
	 * Returns ETrue iff SetUndoWaived() has been called during this batch. If
	 * there is no batch currently open, this function will always return
	 * EFalse.
	 */
	TBool UndoHasBeenWaived() const;
	/**
	 * Sets a flag to indicated that undo is not required for this current
	 * batch. This function has no effect if there is no batch currently open.
	 *
	 * @see UndoHasBeenWaived
	 */
	void SetUndoWaived();
	/**
	 * Sets the maximum height of the stack to aMaxItems. If it is exceeded,
	 * the stack is truncated to aPrunedItems.
	 */
	void SetMaxItems(TInt aMaxItems);
	/**
	 * Returns ETrue iff the stack has no elements.
	 */
	TBool IsEmpty() const { return Top()? EFalse : ETrue; }
	/**
	 * Begins a new batch. All commands added subsequently will be exeuted all
	 * at once. Close the batch with CleanupStack::PopAndDestroy();
	 */
	void BeginBatchLC();
	/**
	 * Returns ETrue iff there is a batch currently open.
	 */
	TBool IsWithinBatch() const;
	/**
	 * Removes and returns the top element. There must be no batch currently
	 * open before a call to Pop().
	 */
	CCommand* Pop();
	/**
	 * Removes all elements from the stack. If a batch is currently open, any
	 * commands added subsequently will not be added to it. Deletes the
	 * bookmark.
	 */
	void Reset();
	/**
	 * Removes and destroys the top element iff it is an empty batch. Must not
	 * be called if a batch is currently open.
	 */
	void Clean();
	/**
	 * Sets the bookmark to the current position.
	 */
	void SetBookmark();
	/**
	 * Returns true iff the bookmark is at the current position.
	 */
	TBool IsAtBookmark();

private:

	CCommandHistory();
	void ConstructL();
	void Prune();

	static void CloseBatch(void* aCCommandHistory);
	static void DownBatchLevel(void* aCCommandHistory);

	CCommandStack* iStack;
	CBatchCommand* iCurrent;	// either == 0 or points to the most
								// recent CBatchCommand within iStack

	TInt iMaxItems;

	TBool iBatchUndoHasBeenWaived;
	};

//
// CSingleCommandStack inlines
//
CSingleCommand* CSingleCommandStack::Top() const
	{ return static_cast<CSingleCommand*>(iStack.Top()); }

CSingleCommand* CSingleCommandStack::Pop()
	{ return static_cast<CSingleCommand*>(iStack.Pop()); }

void CSingleCommandStack::PrepareToPushL(TInt aNumberOfItems)
	{ iStack.PrepareToPushL(aNumberOfItems); }

void CSingleCommandStack::Push(CSingleCommand* aCommand)
	{ iStack.Push(aCommand); }

void CSingleCommandStack::PruneTo(TInt aNumberOfItems)
	{ iStack.PruneTo(aNumberOfItems); }

TInt CSingleCommandStack::Count() const
	{ return iStack.Count(); }

void CSingleCommandStack::Reset()
	{ iStack.Reset(); }

void CSingleCommandStack::Concatenate(CSingleCommandStack& aStack)
	{ iStack.Concatenate(aStack.iStack); }

}

#endif

