/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#ifndef __WPTEXTED_H__
#define __WPTEXTED_H__

#include <techview/eikrted.h>
#include "form_and_etext_editor.h"
#include "EditorUndo.h"
#include "UndoSystem.h"

class CWordUndoGatekeeper : public CBase, public UndoSystem::MNotUndoableGatekeeper
/**
@internalComponent
*/
	{
	// from MNotUndoableGatekeeper
	TBool AllowNotUndoableL(TInt aReason);
	};

class CWordTextEditor : public CEikRichTextEditor, public MUnifiedEditor
/**
@internalComponent
*/
	{
public:
	enum TAttributeValue { EAttributeUndefined = 0, EAttributeSet = 1,
		EAttributeClear = 2, EAttributeIndeterminate = 3 };
	CWordTextEditor(const TGulBorder& aBorder);
	~CWordTextEditor();
	MUnifiedEditor& UnifiedEditor();
	const MUnifiedEditor& UnifiedEditor() const;

	void InitialiseUnifiedEditorL();
	void InsertCharacterL(TChar aCharacter);
	TBool DeleteSelectionL();
	void DeleteLeftL();
	void DeleteRightL();
	void HighlightL(TUint aEffects);
	void RemoveHighlightL(TUint aEffects);
	TAttributeValue IsHighlighted(TUint aEffects) const;
	void ToggleHighlightL(TUint aEffects);
	inline void RedoL();
	inline TBool CanRedo() const;

	TInt ClipboardPasteL();
	void ClipboardCutL();
	void ClipboardCopyL() const;

	void InsertFromHtmlFileL(const TDesC& aFileName);

public:	// functions overloading non-virtual functions in base classes
	inline void UndoL();
	inline TBool CanUndo() const;
	void BoldItalicUnderlineEventL(TInt aFontFlag);

public:	// from MUnifiedEditor
 	TInt DocumentLength() const;
	void GetText(TInt aPos,TPtrC& aText) const;
	void GetBaseFormatL(TTmCharFormat& aCharFormat,RTmParFormat& aParFormat) const;
	void GetCharFormat(TInt aPos,TFormatLevel aLevel,TTmCharFormatLayer& aFormat,TInt& aRunLength) const;
	void GetParFormatL(TInt aPos,TFormatLevel aLevel,RTmParFormatLayer& aFormat,TInt& aRunLength) const;
	void InsertTextL(TInt aPos,const TDesC& aText,const TDesC* aStyle=NULL,
						const TTmCharFormatLayer* aCharFormat=NULL,const RTmParFormatLayer* aParFormat=NULL);
	void DeleteTextL(TInt aPos,TInt aLength);
	void SetBaseFormatL(const TTmCharFormat& aCharFormat,const RTmParFormat& aParFormat);
	void SetCharFormatL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat);
	void SetParFormatL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat);
	void DeleteCharFormatL(TInt aPos,TInt aLength);
	void DeleteParFormatL(TInt aPos,TInt aLength);
	MTmOptionalInterface* Interface(TUint aId);
	void TestRunNotUndoableCommandL();
	void SetBookmark();
	TBool IsAtBookmark() const;
	void ResetUndo();
private: // from CCoeControl
	void HandleResourceChange(TInt aType);
	void FocusChanged(TDrawNow aDrawNow);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

private:
	TFormAndEtextEditor iFormAndEtextEditor;
	CEditorWithUndo* iEditorWithUndo;
	UndoSystem::CCommandManager* iCommandManager;
	CWordUndoGatekeeper iGatekeeper;
	};

inline void CWordTextEditor::RedoL()
	{
	if (iEditorWithUndo)
		iEditorWithUndo->RedoL();
	}
inline TBool CWordTextEditor::CanRedo() const
	{return iEditorWithUndo && iEditorWithUndo->CanRedo();}
inline void CWordTextEditor::UndoL()
	{
	if (iEditorWithUndo)
		iEditorWithUndo->UndoL();
	}
inline TBool CWordTextEditor::CanUndo() const
	{return iEditorWithUndo && iEditorWithUndo->CanUndo();}

#endif
