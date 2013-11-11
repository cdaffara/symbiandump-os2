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


#ifndef UNDOSYSTEM_H_
#define UNDOSYSTEM_H_

#include <e32base.h>

namespace UndoSystem
{
class CCommand;
class CSingleCommand;
class CBatchCommand;
class CCommandManager;
class CCommandHistory;
class CCommandStack;
class CSingleCommandStack;
class CDefaultGatekeeper;

/**
Base class for gatekeepers. A Gatekeeper is responsible for finding more
memory during certain out of memory conditions, and for deciding whether
an operation that cannot be undone should be allowed to be excuted.

@since App-frameworks6.1
@internalAll
*/
class MNotUndoableGatekeeper

	{
public:
	/**
	 * Tries to find more memory. aNumRetries will be 0 on the first call to
	 * this function for the processing of a given command. It will increase
	 * by 1 each time it is called for the same command. The calls will stop
	 * when either the processing for the command completes successfully, when
	 * processing for the command fails for some other reason, or when this
	 * function returns EFalse or leaves.
	 *
	 * Default behaviour is to return EFalse always.
	 */
	IMPORT_C virtual TBool RetryOutOfMemoryL(TInt aNumRetries);
	/**
	 * Decides whether to allow an operation that is undoable.
	 *
	 * aReason is the code that the attempt to create an inverse command
	 * left with.
	 *
	 * A return value of EFalse indicates that the command should not be
	 * executed, and all stored operations should be retained. KErrAbort will
	 * be returned to the caller of CCommandManager. A return value of ETure
	 * indicates that the command should be executed, and all stored
	 * operations deleted. The function may also leave. Any leave will pass
	 * back to the caller of CCommandManager, leaving the command unexecuted
	 * and the stored operations intact.
	 *
	 * Default behaviour is to return ETrue if aReason is KErrNotSupported,
	 * and leave with aReason otherwise.
	 */
	IMPORT_C virtual TBool AllowNotUndoableL(TInt aReason);
	};

/**
General undo system. Together with CSingleCommand and CBatchCommand, this
class provides a framework for building undo systems. A bookmark is
maintained so that we can determine if the undo system has returned the
target to a previously bookmarked state. This is useful for determining if
saving is necessary on exit.

@see CSingleCommand, CBatchCommand
@since App-frameworks6.1
@internalAll
*/
class CCommandManager : public CBase

	{
public:
	/**
	 * Allows a new owner to share this CCommandManager. Release() will need
	 * to be called one extra time per call to NewReference().
	 */
	IMPORT_C void NewReference();
	/**
	 * Allows the caller to finish with this CCommandManager. The caller must
	 * not access this object after calling Release().
	 */
	IMPORT_C void Release();

	/**
	 * Creates a new CCommandManager. One call to Release() will be required
	 * to dispose of this object, unless NewReference() is called, in which case
	 * one additional call to Release() is required per call to NewReference().
	 */
	IMPORT_C static CCommandManager* NewL();

	/**
	 * Begins a batch. Commands entered into the batch will be undone and redone
	 * in one go. If undo is cancelled for one command in the batch, it will be
	 * considered cancelled for the entire batch.
	 * End the batch with CleanupStack::PopAndDestroy();
	 */
	IMPORT_C void BeginBatchLC();
	/**
	 * Returns ETrue iff UndoL() will have an effect.
	 */
	IMPORT_C TBool CanUndo() const;
	/**
	 * Returns ETrue iff RedoL() will have an effect.
	 */
	IMPORT_C TBool CanRedo() const;
	/**
	 * Executes a single command, allowing it to be undone later, if
	 * appropriate.
	 */
	IMPORT_C TInt ExecuteL(const CSingleCommand&);
	/**
	 * Sets a gatekeper for the undo system. This will be called whenever an
	 * operation is attempted that cannot be undone for any reason.
	 * The gatekeeper therefore has an oportunity to suppress execution and
	 * keep the current undo operations stored.
	 * NULL may be passed to restore default behaviour.
	 * Returns previous gatekeeper.
	 */
	IMPORT_C MNotUndoableGatekeeper* SetGatekeeper(MNotUndoableGatekeeper*);
	/**
	 * Sets limits on the 'undo depth'. This is the number of times that
	 * successive calls to UndoL() have an effect.
	 */
	IMPORT_C void SetMaxItems(TInt aMaxItems);
	/**
	 * Undoes one operation or batch of operations. If one operation in the
	 * middle of a batch leaves, this function will leave, but the underlying
	 * editor will not necessarily be in the same state as it was in before
	 * the call. However, all operations will still be stored, and so the
	 * previous state is still recoverable with a further call to UndoL().
	 */
	IMPORT_C void UndoL();
	/**
	 * Redoes one operation or batch of operations. If one operation in the
	 * middle of a batch leaves, this function will leave, but the underlying
	 * editor will not necessarily be in the same state as it was in before
	 * the call. However, all operations will still be stored, and so the
	 * previous state is still recoverable with a further call to RedoL().
	 */
	IMPORT_C void RedoL();
	/**
	 * Deletes all stored operations. Deletes the bookmark.
	 */
	IMPORT_C void ResetUndo();
	/**
	 * Sets the bookmark to the current position in the history.
	 */
	IMPORT_C void SetBookmark();
	/**
	 * Returns true iff we are currently at the bookmarked position.
	 */
	IMPORT_C TBool IsAtBookmark() const;

private:

	TInt ExecuteSingleCommandL(const CSingleCommand& aCommand, CCommandHistory& aUndo);
	TInt ExecuteBatchCommandL(CBatchCommand& aCommand, CCommandHistory& aUndo);
	void MoveHistoryL(CCommandHistory& aFrom, CCommandHistory& aTo);
	TBool CreateAndPrepareToAddInverseL(const CSingleCommand& aCommand,
		CCommandHistory& aUndo, CCommand*& aInverse);

	CCommandManager();
	void ConstructL();
	~CCommandManager();

	CCommandHistory*		iFuture;
	CCommandHistory*		iPast;
	MNotUndoableGatekeeper*	iCurrentGatekeeper;
	CDefaultGatekeeper*		iDefaultGatekeeper;
	TInt					iRefCount;
	};

/**
Abstract base class for all commands that can be stored in the undo system

@since App-frameworks6.1
@internalComponent
*/
class CCommand : public CBase

	{
public:
	/**
	 * Casts this CCommand to CSingleCommand* if possible
	 */
	virtual CSingleCommand*	Single() = 0;
	/**
	 * Casts this CCommand to CBatchCommand* if possible
	 */
	virtual CBatchCommand*	Batch() = 0;
	};

/**
Abstract base class for all commands. A CSingleCommand is able to be
completed atomically, that is, leave their target unchanged if its
execution leaves.

@since App-frameworks6.1
@internalAll
*/
class CSingleCommand : public CCommand

	{
public:
	/**
	 * Executes this command. This function may leave or return an error code.
	 * in either case, there must have been no effect on its target(s).
	 */
	virtual TInt ExecuteL() const = 0;
	/**
	 * Prepares to add the inverse of this command to aLastCommand.
	 * Returns ETrue iff this was possible. Returning ETrue implies that
	 * a future call to AddInverseToLast with the same parameter will
	 * succeed without leaving.
	 * The defualt implementation is to return EFalse.
	 */
	IMPORT_C virtual TBool PrepareToAddInverseToLastL(CSingleCommand& aLastCommand) const;

	/**
	 * Adds this command's inverse to aLastCommand. This function will
	 * only be called after PrepareToAddInverseToLastL has been called
	 * with the same argument, ETrue having been returned.
	 * Default implementation is to panic.
	 */
	IMPORT_C virtual void AddInverseToLast(CSingleCommand& aLastCommand) const;

	/**
	 * Creates an inverse of this command.
	 * A return value of 0 indicates that this command has no effect, and so a
	 * return is not needed. To indicate that an inverse command cannot be
	 * created, this function should leave with KErrNotSupported.
	 * Default implementation is to leave with KErrNotSupported.
	 */
	IMPORT_C virtual CCommand* CreateInverseL() const;

	/**
	 * Returns a UID for the family of CSingleCommands that this belongs to.
	 * This would usually be the DLL UID or KNullUid. It can be used to
	 * determine whether a downcast is safe.
	 */
	IMPORT_C virtual TUid FamilyUid() const;
	/**
	 * Returns this. Not to be overridden further.
	 */
	IMPORT_C CSingleCommand* Single();
	/**
	 * Returns 0. Not to be overridden further.
	 */
	IMPORT_C CBatchCommand* Batch();
	};

/**
Batch of commands.

@since App-frameworks6.1
@internalComponent
*/
class CBatchCommand : public CCommand

	{
public:
	IMPORT_C ~CBatchCommand();

	/**
	 * Creates an empty batch.
	 */
	IMPORT_C static CBatchCommand* NewL();
	/**
	 * Creates an empty batch on the cleanup stack.
	 */
	IMPORT_C static CBatchCommand* NewLC();

	/**
	 * Returns 0.
	 */
	IMPORT_C CSingleCommand*	Single();
	/**
	 * Returns this.
	 */
	IMPORT_C CBatchCommand*		Batch();
	/**
	 * Returns the single command that is at the top of the stack. If a batch
	 * is at the top, then it will be the top of that.
	 * A return value of 0 indicates that the batch is empty. Some empty
	 * batches within the batch may be deleted.
	 */
	IMPORT_C CSingleCommand*	Top() const;
	/**
	 * Returns the single command that is at the top of the stack as for Top().
	 * The ownership of the object is passed to the caller. This method must
	 * not be called on an empty batch.
	 */
	IMPORT_C CSingleCommand*	Pop();
	/**
	 * Ensures that enough resources to perform a Push(CCommand* aCommand)
	 * without leaving are allocated. The contents of the batch are unaltered.
	 */
	IMPORT_C void				PrepareToPushL(CCommand* aCommand);
	/**
	 * Pushes the command onto the batch. This command will be executed before
	 * the commands currently in the batch. This function must only be called
	 * if PrepareToPushL() has been called successfully since the last call to
	 * Push() or NewL().
	 *
	 * aCommand may not be accessed after this call has completed.
	 */
	IMPORT_C void				Push(CCommand* aCommand);
	/**
	 * Performs PrepareToPushL(aCommand) then Push(aCommand). If it leaves,
	 * aCommand is destroyed.
	 *
	 * @see PrepareToPushL, Push
	 */
	IMPORT_C void				PushL(CCommand* aCommand);
	/**
	 * Returns ETrue iff the batch is empty.
	 */
	TBool IsEmpty() const 
		{
		if (Top())
			return EFalse;
		else
			return ETrue;
		}

private:
	CSingleCommandStack* iStack;
	CBatchCommand();
	void ConstructL();
	};
}

#endif	// UNDOSYSTEM_H_
