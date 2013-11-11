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




#ifndef FORM_AND_ETEXT_EDITOR_H_
#define FORM_AND_ETEXT_EDITOR_H_ 1

#include <frmtview.h>
#include <txtrich.h>
#include "unified_editor.h"

/**
TFormAndEtextEditor is an editor interface class that can be constructed as a facade
over a FORM object and an ETEXT object, which may be either a CGlobalText or a CRichText
object. It implements the unified editing interface, MUnifiedEditor,
@internalAll
*/
class TFormAndEtextEditor: public MUnifiedEditor,
	public MUnifiedEditor::MStyleSupport,
	public MUnifiedEditor::MPictureSupport,
	public MUnifiedEditor::MClipboardSupport

	{
public:
	TFormAndEtextEditor(): iTextView(NULL), iGlobalText(NULL), iRichText(NULL) { }
	TFormAndEtextEditor(CTextView& aTextView,CGlobalText& aRichText):
		iTextView(&aTextView), iGlobalText(&aRichText), iRichText(NULL) { }
	TFormAndEtextEditor(CTextView& aTextView,CRichText& aRichText):
		iTextView(&aTextView), iGlobalText(&aRichText), iRichText(&aRichText) { }
	void Set(CTextView& aTextView,CGlobalText& aGlobalText)
		{ iTextView = &aTextView; iGlobalText = &aGlobalText; iRichText = NULL; }
	void Set(CTextView& aTextView,CRichText& aRichText)
		{ iTextView = &aTextView; iGlobalText = &aRichText; iRichText = &aRichText; }

	// virtual functions from MUnifiedEditor
	IMPORT_C MTmOptionalInterface* Interface(TUint aId);
	IMPORT_C TInt DocumentLength() const;
	IMPORT_C void GetText(TInt aPos,TPtrC& aText) const;
	IMPORT_C void GetBaseFormatL(TTmCharFormat& aCharFormat,RTmParFormat& aParFormat) const;
	IMPORT_C void GetCharFormat(TInt aPos,TFormatLevel aLevel,
								TTmCharFormatLayer& aFormat,TInt& aRunLength) const;
	IMPORT_C void GetParFormatL(TInt aPos,TFormatLevel aLevel,
								RTmParFormatLayer& aFormat,TInt& aRunLength) const;
	IMPORT_C void InsertTextL(TInt aPos,const TDesC& aText,
							  const TDesC* aStyle = NULL,
							  const TTmCharFormatLayer* aCharFormat = NULL,
							  const RTmParFormatLayer* aParFormat = NULL);
	IMPORT_C void DeleteTextL(TInt aPos,TInt aLength);
	IMPORT_C void SetBaseFormatL(const TTmCharFormat& aCharFormat,const RTmParFormat& aParFormat);
	IMPORT_C void SetCharFormatL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat);
	IMPORT_C void SetParFormatL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat);
	IMPORT_C void DeleteCharFormatL(TInt aPos,TInt aLength);
	IMPORT_C void DeleteParFormatL(TInt aPos,TInt aLength);

	// virtual functions from MUnifiedEditor::MStyleSupport
	IMPORT_C TInt CreateStyleL(const RTmStyle& aStyle);
	IMPORT_C TInt ChangeStyleL(const RTmStyle& aStyle);
	IMPORT_C TInt SetStyleL(TInt aPos,TInt aLength,const TDesC& aName);
	IMPORT_C TInt RenameStyleL(const TDesC& aOldName,const TDesC& aNewName);
	IMPORT_C TInt DeleteStyleL(const TDesC& aName);
	IMPORT_C TInt StyleCount() const;
	IMPORT_C void GetStyle(TInt aPos,TPtrC& aName,TInt& aRunLength) const;
	IMPORT_C TInt GetStyleByNameL(const TDesC& aName,RTmStyle& aStyle) const;
	IMPORT_C TInt GetStyleByIndexL(TInt aIndex,RTmStyle& aStyle) const;

	// virtual functions from MUnifiedEditor::MPictureSupport
	IMPORT_C void InsertPictureL(TInt aPos, const TPictureHeader& aPictureIn);
	IMPORT_C void DropPictureL(TInt aPos);
	IMPORT_C void Picture(TInt aPos, TPictureHeader& aPictureOut) const;

	// virtual functions from MUnifiedEditor::MClipboardSupport
	IMPORT_C void CopyToStoreL(CStreamStore& aStore, CStreamDictionary& aDictionary,
		TInt aPos, TInt aLength) const;
	IMPORT_C void PasteFromStoreL(const CStreamStore& aStore,
		const CStreamDictionary& aDictionary, TInt aPos);

private:
	void SetCharFormatHelperL(TInt aPos, TInt aLength, const TTmCharFormatLayer& aFormat);
	void SetParFormatHelperL(TInt aPos, TInt aLength, const RTmParFormatLayer& aFormat);
	TInt SetStyleHelperL(TInt aPos, TInt aLength, const TDesC& aName);
	void AdjustSelectionL(TInt aPos, TInt aIncreaseInCharacterNumber, TBool aFormatChanged);

	CTextView* iTextView;
	CGlobalText* iGlobalText;
	CRichText* iRichText;
	};

#endif // FORM_AND_ETEXT_EDITOR_H_
