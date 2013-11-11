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


#ifndef EDITORUNDO_H_
#define EDITORUNDO_H_

#include "unified_editor.h"

namespace UndoSystem
/**
@internalTechnology
*/
{
class CCommandManager;
class MNotUndoableGatekeeper;
}

class CEditorCommandFactory;

/**
MUnifiedEditor decorator that adds undo functionality.
It passes commands transparently to the editor it is constructed upon, while
storing the operations so that they can be undone and redone. Calling a
non-const method of MUnifiedEditor wipes all 'Redo' operations.
The undo and redo operations are only guaranteed to be accurate if the
underlying editor satisfies certain conditions.

@since App-frameworks6.1
@internalAll
*/
class CEditorWithUndo : public CBase,
	public MUnifiedEditor,
	private MUnifiedEditor::MStyleSupport,
	private MUnifiedEditor::MPictureSupport,
	private MUnifiedEditor::MClipboardSupport

	{
public:
	~CEditorWithUndo();

	/**
	 * Creates a CEditorWithUndo. It does not own the aEditor.
	 */
	IMPORT_C static CEditorWithUndo* NewL(MUnifiedEditor& aEditor);
	/**
	 * Creates a CEditorWithUndo, using the aSharedUndoSystem. This allows
	 * many objects to stay in synchronization when each is accessed
	 * seperately.
	 * The aEditor is not owned.
	 */
	IMPORT_C static CEditorWithUndo* NewL(MUnifiedEditor& aEditor,
		UndoSystem::CCommandManager* aSharedUndoSystem);
	/**
	 * Undoes one operation or batch of operations. If one operation in the
	 * middle of a batch leaves, this function will leave, but the underlying
	 * editor will not necessarily be in the same state as it was in before
	 * the call. However, all operations will still be stored, and so the
	 * previous state is still recoverable with a call to RedoL() or a
	 * further call to UndoL will complete the operation (resource acquisition
	 * permitting).
	 */
	IMPORT_C void UndoL();
	/**
	 * Redoes one operation or batch of operations. If one operation in the
	 * middle of a batch leaves, this function will leave, but the underlying
	 * editor will not necessarily be in the same state as it was in before
	 * the call. However, all operations will still be stored, and so the
	 * previous state is still recoverable with a call to UndoL() or a
	 * further call to RedoL will complete the operation (resource acquisition
	 * permitting).
	 */
	IMPORT_C void RedoL();
	/**
	 * Returns ETrue iff UndoL() would have an effect
	 */
	IMPORT_C TBool CanUndo() const;
	/**
	 * Returns ETrue iff RedoL() would have an effect
	 */
	IMPORT_C TBool CanRedo() const;
	/**
	 * Wipes all undo and redo operations
	 */
	IMPORT_C void ResetUndo();
	/**
	 * Sets limits on the 'undo depth'. This is the numbet of times that
	 * successive calls to UndoL() have an effect. When a depth of
	 * aMaxItems is reached, the undo depth is reset to aMinItems.
	 */
	IMPORT_C void SetMaxItems(TInt aMaxItems);
	/**
	 * Sets a gatekeper for the undo system. This will be called whenever an
	 * operation is attempted that cannot be undone for any reason.
	 * The gatekeeper therefore has an oportunity to suppress execution and
	 * keep the current undo operations stored.
	 * NULL may be passed to restore default behaviour.
	 * Returns the old gatekeeper.
	 */
	IMPORT_C UndoSystem::MNotUndoableGatekeeper*
		SetGatekeeper(UndoSystem::MNotUndoableGatekeeper*);

	// From MUnifiedEditor
	MTmOptionalInterface* Interface(TUint aId);
	void InsertTextL(TInt aPos, const TDesC& aText, const TDesC* aStyle,
		const TTmCharFormatLayer*, const RTmParFormatLayer*);
	void DeleteTextL(TInt aPos,TInt aLength);
	void SetBaseFormatL(const TTmCharFormat&, const RTmParFormat&);
	void SetCharFormatL(TInt aPos, TInt aLength, const TTmCharFormatLayer&);
	void SetParFormatL(TInt aPos, TInt aLength, const RTmParFormatLayer&);
	void DeleteCharFormatL(TInt aPos, TInt aLength);
	void DeleteParFormatL(TInt aPos, TInt aLength);
	TInt DocumentLength() const;
	void GetText(TInt aPos, TPtrC& aText) const;
	void GetBaseFormatL(TTmCharFormat&, RTmParFormat&) const;
	void GetCharFormat(TInt aPos, TFormatLevel aLevel,
		TTmCharFormatLayer& aFormat,TInt& aRunLength) const;
	void GetParFormatL(TInt aPos, TFormatLevel aLevel,
		RTmParFormatLayer& aFormat, TInt& aRunLength) const;

private:
	// from MStyleSupport
	TInt CreateStyleL(const RTmStyle&);
	TInt ChangeStyleL(const RTmStyle&);
	TInt SetStyleL(TInt aPos, TInt aLength, const TDesC&);
	TInt RenameStyleL(const TDesC& aOldName, const TDesC& aNewName);
	TInt DeleteStyleL(const TDesC& aName);
	TInt StyleCount() const;
	void GetStyle(TInt aPos, TPtrC& aName, TInt& aRunLength) const;
	TInt GetStyleByNameL(const TDesC& aName, RTmStyle&) const;
	TInt GetStyleByIndexL(TInt aIndex, RTmStyle&) const;

	// from MPictureSupport
	void InsertPictureL(TInt aPos, const TPictureHeader&);
	void DropPictureL(TInt aPos);
	void Picture(TInt aPos, TPictureHeader&) const;

	// from MClipboardSupport
	void CopyToStoreL(CStreamStore& aStore, CStreamDictionary& aDictionary,
		TInt aPos, TInt aLength) const;
	void PasteFromStoreL(const CStreamStore& aStore,
		const CStreamDictionary& aDictionary, TInt aPos);

	CEditorWithUndo();
	void ConstructL(MUnifiedEditor& aEditorBasedOn,
		UndoSystem::CCommandManager* aSharedUndoSystem);

	CEditorCommandFactory*			iFactory;
	MUnifiedEditor*						iBaseEditor;
	UndoSystem::CCommandManager*	iCommandManager;
	};

#endif	// EDITORUNDO_H_
