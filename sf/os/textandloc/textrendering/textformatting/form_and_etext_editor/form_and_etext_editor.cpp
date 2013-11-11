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


#include "form_and_etext_editor.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtfmlyr_internal.h"
#endif

void TFormAndEtextEditor::AdjustSelectionL(TInt aPos, TInt aIncreaseInCharacterNumber,
										  TBool aFormatChanged)
	{
	TCursorSelection selected_text = iTextView->Selection();
	if (aIncreaseInCharacterNumber == 1 && selected_text.iCursorPos == aPos && !aFormatChanged)
		{
		iTextView->HandleCharEditL(CTextLayout::EFCharacterInsert, EFalse);
		return;
		}
	if (selected_text.iCursorPos >= aPos)
		selected_text.iCursorPos = Max(aPos,
			selected_text.iCursorPos + aIncreaseInCharacterNumber);
	if (selected_text.iAnchorPos >= aPos)
		selected_text.iAnchorPos = Max(aPos,
			selected_text.iAnchorPos + aIncreaseInCharacterNumber);
	iTextView->SetPendingSelection(selected_text);
	TCursorSelection sel(aPos, Max(aPos, aPos + aIncreaseInCharacterNumber));
	iTextView->HandleInsertDeleteL(sel, Max(0, -aIncreaseInCharacterNumber),
		aFormatChanged);
	}

EXPORT_C void TFormAndEtextEditor::InsertTextL(TInt aPos, const TDesC& aText,
		const TDesC* aStyle,
		const TTmCharFormatLayer* aCharFormat,
		const RTmParFormatLayer* aParFormat)
	{
	iGlobalText->InsertL(aPos, aText);
	TInt length = aText.Length();

	TBool style_set = EFalse;
	if (aStyle)
		style_set = SetStyleHelperL(aPos, length, *aStyle) == KErrNone;
	if (aCharFormat)
		{
		if (iRichText)
			iRichText->RemoveSpecificCharFormatL(aPos, length);
		SetCharFormatHelperL(aPos, length, *aCharFormat);
		}
	if (aParFormat)
		{
		if (iRichText && 0 < length)
			iRichText->RemoveSpecificParaFormatL(aPos, length - 1);
		SetParFormatHelperL(aPos, length, *aParFormat);
		}
	AdjustSelectionL(aPos, length, style_set || aParFormat);
	}

EXPORT_C void TFormAndEtextEditor::DeleteTextL(TInt aPos, TInt aLength)
	{
	if (iRichText)
		iRichText->DelSetInsertCharFormatL(aPos, aLength);
	else
		iGlobalText->DeleteL(aPos, aLength);
	AdjustSelectionL(aPos, -aLength, EFalse);
	}

EXPORT_C void TFormAndEtextEditor::SetBaseFormatL(const TTmCharFormat& /*aCharFormat*/,const RTmParFormat& /*aParFormat*/)
	{
	// This can't be done; CGlobalText doesn't own its base format and has const pointers only to the format objects.
	}

EXPORT_C void TFormAndEtextEditor::SetCharFormatL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat)
	{
	SetCharFormatHelperL(aPos, aLength, aFormat);
	TCursorSelection sel(aPos, aPos + aLength);
	iTextView->HandleRangeFormatChangeL(sel);
	}

EXPORT_C void TFormAndEtextEditor::SetParFormatL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat)
	{
	SetParFormatHelperL(aPos, aLength, aFormat);
	TCursorSelection sel(aPos, aPos + aLength);
	iTextView->HandleRangeFormatChangeL(sel, ETrue);
	}

EXPORT_C void TFormAndEtextEditor::DeleteCharFormatL(TInt aPos,TInt aLength)
	{
	if (iRichText)
		{
		iRichText->RemoveSpecificCharFormatL(aPos, aLength);
		TCursorSelection sel(aPos, aPos + aLength);
		iTextView->HandleRangeFormatChangeL(sel);
		}
	}

EXPORT_C void TFormAndEtextEditor::DeleteParFormatL(TInt aPos,TInt aLength)
	{
	if (iRichText && 0 < aLength)
		{
		iRichText->RemoveSpecificParaFormatL(aPos, aLength - 1);
		TCursorSelection sel(aPos, aPos + aLength);
		iTextView->HandleRangeFormatChangeL(sel, ETrue);
		}
	}

EXPORT_C TInt TFormAndEtextEditor::CreateStyleL(const RTmStyle& aStyle)
	{
	if (!iRichText)
		return KErrNotSupported;

	CStyleList* list = iRichText->StyleList();
	if (!list)
		return KErrNotSupported;

	RParagraphStyleInfo* style = new (ELeave) RParagraphStyleInfo;

	//+ clean up RParagraphStyleInfo!!! needs a Close function.
	CleanupStack::PushL(style);
	aStyle.GetRParagraphStyleInfoL(*style,
		*iGlobalText->GlobalParaFormatLayer(),
		*iGlobalText->GlobalCharFormatLayer(),
		*list);
	list->AppendL(style);
	CleanupStack::Pop(style);

	return KErrNone;
	}

EXPORT_C TInt TFormAndEtextEditor::ChangeStyleL(const RTmStyle& aStyle)
	{
	if (!iRichText)
		return KErrNotSupported;

	CStyleList* list = iRichText->StyleList();
	if (!list)
		return KErrNotSupported;

	RParagraphStyleInfo* style = list->PtrByName(aStyle.iName);
	if (!style)
		return KErrNotFound;

	aStyle.GetRParagraphStyleInfoL(*style,
		*iGlobalText->GlobalParaFormatLayer(),
		*iGlobalText->GlobalCharFormatLayer(),
		*list);

	iTextView->HandleGlobalChangeL();
	return KErrNone;
	}

EXPORT_C TInt TFormAndEtextEditor::SetStyleL(TInt aPos,TInt aLength,const TDesC& aName)
	{
	TInt result = SetStyleHelperL(aPos, aLength, aName);
	if (result == KErrNone)
		{
		TCursorSelection sel(aPos, aPos + aLength);
		iTextView->HandleRangeFormatChangeL(sel, ETrue);
		}
	return result;
	}

EXPORT_C TInt TFormAndEtextEditor::RenameStyleL(const TDesC& aOldName,const TDesC& aNewName)
	{
	if (iRichText)
		{
		CStyleList* list = iRichText->StyleList();
		if (list)
			{
			RParagraphStyleInfo* style = list->PtrByName(aOldName);
			if (style)
				{
				TPtrC name = aNewName;
				if (name.Length() > KMaxParagraphStyleName)
					name.Set(name.Ptr(),KMaxParagraphStyleName);
				style->iStyle->iName = name;
				return KErrNone;
				}
			}
		}
	return KErrNotFound;
	}

EXPORT_C TInt TFormAndEtextEditor::DeleteStyleL(const TDesC& aName)
	{
	if (iRichText)
		{
		CStyleList* list = iRichText->StyleList();
		if (list)
			{
			RParagraphStyleInfo* style = list->PtrByName(aName);
			if (style)
				{
				list->Remove(style->iStyle);
				return KErrNone;
				}
			}
		}
	return KErrNotFound;
	}

EXPORT_C MTmOptionalInterface* TFormAndEtextEditor::Interface(TUint aId)
	{
	if (aId == KUidMUnifiedEditorStyleSupport)
		return static_cast<MUnifiedEditor::MStyleSupport*>(this);
	if (aId == KUidMUnifiedEditorPictureSupport)
		return static_cast<MUnifiedEditor::MPictureSupport*>(this);
	if (aId == KUidMUnifiedEditorClipboardSupport)
		return static_cast<MUnifiedEditor::MClipboardSupport*>(this);
	return 0;
	}

EXPORT_C TInt TFormAndEtextEditor::DocumentLength() const
	{
	return iGlobalText->DocumentLength();
	}

EXPORT_C void TFormAndEtextEditor::GetText(TInt aPos,TPtrC& aText) const
	{
	aText.Set(iGlobalText->Read(aPos));
	}

EXPORT_C void TFormAndEtextEditor::GetBaseFormatL(TTmCharFormat& aCharFormat,RTmParFormat& aParFormat) const
	{
	TCharFormat cf;
	TCharFormatMask cm;
	CParaFormat* pf = CParaFormat::NewL();
	CleanupStack::PushL(pf);
	TParaFormatMask pm;
	iGlobalText->GetCharFormat(cf,cm,0,0);
	iGlobalText->GetParaFormatL(pf,pm,0,0);
	aCharFormat = cf;
	aParFormat.CopyL(*pf);
	CleanupStack::PopAndDestroy(pf);
	}

EXPORT_C void TFormAndEtextEditor::GetCharFormat(TInt aPos,TFormatLevel aLevel,TTmCharFormatLayer& aFormat,
												 TInt& aRunLength) const
	{
	TPtrC text;
	TCharFormat format;
	TCharFormatMask mask;
	iGlobalText->GetChars(text,format,aPos);
	aRunLength = text.Length();
	aFormat.iMask.iFlags = 0xFFFFFFFF;
	if (iRichText && aLevel == ESpecific)
		{
		iRichText->GetSpecificCharFormatRight(format, mask, aPos);
		aFormat.iMask = mask;
		}
	aFormat.iFormat = format;
	}

EXPORT_C void TFormAndEtextEditor::GetParFormatL(TInt aPos,TFormatLevel aLevel,RTmParFormatLayer& aFormat,
												 TInt& aRunLength) const
	{
	CParaFormat* pf = CParaFormat::NewL();
	CleanupStack::PushL(pf);
	iGlobalText->GetParagraphFormatL(pf,aPos);
	int pos = aPos;
	TUint scan_mask = CPlainText::EScanToUnitEnd;
	aRunLength = iGlobalText->ScanParas(pos,scan_mask);
	aFormat.iMask.iFlags = 0xFFFFFFFF;
	if (aLevel == ESpecific)
		{
		TParaFormatMask mask;
		iRichText->GetSpecificParagraphFormatL(pf, mask, aPos);
		aFormat.iMask = mask;
		}
	aFormat.iFormat.CopyL(*pf);
	CleanupStack::PopAndDestroy(pf);
	}

EXPORT_C TInt TFormAndEtextEditor::StyleCount() const
	{
	if (iRichText)
		return iRichText->StyleCount();
	else
		return 0;
	}

EXPORT_C void TFormAndEtextEditor::GetStyle(TInt aPos,TPtrC& aName,TInt& aRunLength) const
	{
	if (iRichText)
		{
		int pos = aPos;
		TUint scan_mask = CPlainText::EScanToUnitEnd;
		aRunLength = iRichText->ScanParas(pos,scan_mask);
		TBool varies;
		const CParaFormatLayer* format = iRichText->ParagraphStyle(varies,aPos,aRunLength);
		if (format->Type() == KNormalParagraphStyleUid)
			aName.Set(KNullDesC);
		else
			{
			const CParagraphStyle* style = static_cast<const CParagraphStyle*>(format);
			aName.Set(style->iName);
			}
		}
	}

EXPORT_C TInt TFormAndEtextEditor::GetStyleByNameL(const TDesC& aName,RTmStyle& aStyle) const
	{
	if (iRichText)
		{
		const CStyleList* list = iRichText->StyleList();
		if (list)
			{
			RParagraphStyleInfo* style = list->PtrByName(aName);
			if (style)
				{
				aStyle.CopyL(*style);
				return KErrNone;
				}
			}
		}
	return KErrNotFound;
	}

EXPORT_C TInt TFormAndEtextEditor::GetStyleByIndexL(TInt aIndex,RTmStyle& aStyle) const
	{
	if (iRichText)
		{
		const CStyleList* list = iRichText->StyleList();
		if (list)
			{
			aStyle.CopyL((*list)[aIndex]);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

EXPORT_C void TFormAndEtextEditor::CopyToStoreL(CStreamStore& aStore,
												CStreamDictionary& aDictionary,
												TInt aPos,
												TInt aLength) const
	{
	iGlobalText->CopyToStoreL(aStore, aDictionary, aPos, aLength);
	}

EXPORT_C void TFormAndEtextEditor::PasteFromStoreL(const CStreamStore& aStore,
												   const CStreamDictionary& aDictionary,
												   TInt aPos)
	{
	TInt length = iGlobalText->PasteFromStoreL(aStore, aDictionary, aPos);
	AdjustSelectionL(aPos, length, ETrue);
	}

// Set the character format without reformatting and redisplaying.
void TFormAndEtextEditor::SetCharFormatHelperL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat)
	{
	TCharFormat format;
	TCharFormatMask mask;
	aFormat.iFormat.GetTCharFormat(format);
	aFormat.iMask.GetTCharFormatMask(mask);
	iGlobalText->ApplyCharFormatL(format,mask,aPos,aLength);
	}

// Set the paragraph format without reformatting and redisplaying.
void TFormAndEtextEditor::SetParFormatHelperL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat)
	{
	CParaFormat* format = CParaFormat::NewL();
	CleanupStack::PushL(format);
	TParaFormatMask mask;
	aFormat.iFormat.GetCParaFormatL(*format);
	aFormat.iMask.GetTParaFormatMask(mask);
	iGlobalText->ApplyParaFormatL(format,mask,aPos,aLength);
	CleanupStack::PopAndDestroy(format);
	}

// Set the style without reformatting and redisplaying.
TInt TFormAndEtextEditor::SetStyleHelperL(TInt aPos,TInt aLength,const TDesC& aName)
	{
	if (!iRichText)
		return KErrNotFound;

	const CStyleList* list = iRichText->StyleList();
	if (!list)
		return KErrNotFound;

	if (aName.Length() == 0)
		{
		// A null name means remove the style
		iRichText->ApplyParagraphStyleL(
			*static_cast<CParagraphStyle*>(
				const_cast<CParaFormatLayer*>(iRichText->GlobalParaFormatLayer())),
			aPos, aLength,
			CParagraphStyle::ERetainSpecificCharFormat);
		return KErrNone;
		}

	CParagraphStyle* style = NULL;
	RParagraphStyleInfo* info = list->PtrByName(aName);
	if (info)
		style = info->iStyle;
	else
		return KErrNotFound;
	iRichText->ApplyParagraphStyleL(*style, aPos, aLength,
		CParagraphStyle::ERetainSpecificCharFormat);
	return KErrNone;
	}

EXPORT_C void TFormAndEtextEditor::InsertPictureL(TInt aPos, const TPictureHeader& aPictureIn)
	{
	iRichText->InsertL(aPos, aPictureIn);
	AdjustSelectionL(aPos, 1, ETrue);
	}

EXPORT_C void TFormAndEtextEditor::DropPictureL(TInt aPos)
	{
	iRichText->CancelInsertCharFormat();
	iRichText->DropPictureOwnership(aPos);
	AdjustSelectionL(aPos, -1, ETrue);
	}

EXPORT_C void TFormAndEtextEditor::Picture(TInt aPos, TPictureHeader& aPictureOut) const
	{
	aPictureOut = iRichText->PictureHeader(aPos);
	}

