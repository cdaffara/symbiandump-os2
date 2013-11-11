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


#ifndef EDITORPLAINTEXTUNDO_H_
#define EDITORPLAINTEXTUNDO_H_

#include "unified_editor.h"

namespace UndoSystem
{
class CCommandManager;
class MNotUndoableGatekeeper;
}

class CEditorPlainTextCommandFactory;

/**
MUnifiedEditor decorator that adds undo functionality to plain text editors.
It passes commands transparently to the editor it is constructed upon, while
storing the operations so that they can be undone and redone. Calling a
non-const method of MUnifiedEditor wipes all 'Redo' operations.
The undo and redo operations are only guaranteed to be accurate if the
underlying editor satisfies certain conditions.

@since App-frameworks6.1
@internalComponent
*/
class CEditorPlainTextWithUndo : public CBase, public MUnifiedEditor,
	private MUnifiedEditor::MClipboardSupport
	{
public:
	~CEditorPlainTextWithUndo();

	/**
	 * Creates a CEditorPlainTextWithUndo, using the aSharedUndoSystem.
	 * This allows many objects to stay in synchronization when each is
	 * accessed seperately.
	 * The aEditor is not owned.
	 * Undo functionality is accessed only via the aSharedUndoSystem
	 * object.
	 */
	IMPORT_C static CEditorPlainTextWithUndo* NewL(MUnifiedEditor& aEditor,
		UndoSystem::CCommandManager* aSharedUndoSystem);

	// From MUnifiedEditor
	void InsertTextL(TInt aPos, const TDesC& aText, const TDesC* aStyle,
		const TTmCharFormatLayer*, const RTmParFormatLayer*);
	void DeleteTextL(TInt aPos,TInt aLength);
	MTmOptionalInterface* Interface(TUint aId);

	// The following methods are forwarded to the base editor
	TInt DocumentLength() const;
	void GetText(TInt aPos, TPtrC& aText) const;
	void GetBaseFormatL(TTmCharFormat&, RTmParFormat&) const;
	void GetCharFormat(TInt aPos, TFormatLevel aLevel,
		TTmCharFormatLayer& aFormat,TInt& aRunLength) const;
	void GetParFormatL(TInt aPos, TFormatLevel aLevel,
		RTmParFormatLayer& aFormat, TInt& aRunLength) const;
	void SetBaseFormatL(const TTmCharFormat&, const RTmParFormat&);
	void SetCharFormatL(TInt aPos, TInt aLength, const TTmCharFormatLayer&);
	void SetParFormatL(TInt aPos, TInt aLength, const RTmParFormatLayer&);
	void DeleteCharFormatL(TInt aPos, TInt aLength);
	void DeleteParFormatL(TInt aPos, TInt aLength);

private:
	CEditorPlainTextWithUndo();
	void ConstructL(MUnifiedEditor& aEditorBasedOn,
		UndoSystem::CCommandManager& aSharedUndoSystem);

	// from MUnifiedEditor::MClipboardSupport
	void CopyToStoreL(CStreamStore&, CStreamDictionary&, TInt, TInt) const;
	void PasteFromStoreL(const CStreamStore&, const CStreamDictionary&, TInt);

	CEditorPlainTextCommandFactory*	iFactory;
	MUnifiedEditor*						iBaseEditor;
	UndoSystem::CCommandManager*	iCommandManager;
	};

#endif	// EDITORPLAINTEXTUNDO_H_

