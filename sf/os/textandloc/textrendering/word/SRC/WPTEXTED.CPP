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


#include "WPTEXTED.H"
#include <txtrich.h>
#include <eikenv.h>
#include <word.rsg>
#include <baclipb.h>
#include <conlist.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtfmlyr_internal.h"
#include <txtclipboard.h>
#endif

inline TBool IsSurrogate(TText a) { return 0xD800 == (a & 0xF800); }
inline TBool IsHighSurrogate(TText a) { return 0xD800 == (a & 0xFC00); }
inline TBool IsLowSurrogate(TText a) { return 0xDC00 == (a & 0xFC00); }
inline TChar JoinSurrogates(TText aHigh, TText aLow)
	{
	return ((aHigh - 0xd7f7) << 10) + aLow;
	}


inline TText16 GetHighSurrogate(TUint aChar)
	{
	return STATIC_CAST(TText16, 0xD7C0 + (aChar >> 10));
	}

inline TText16 GetLowSurrogate(TUint aChar)
	{
	return STATIC_CAST(TText16, 0xDC00 | (aChar & 0x3FF));
	}


class CTestCommand : public UndoSystem::CSingleCommand
	{
	TInt ExecuteL() const { return KErrNone; }
	};

TBool CWordUndoGatekeeper::AllowNotUndoableL(TInt aReason)
	{
	if (aReason != KErrNotSupported)
		return MNotUndoableGatekeeper::AllowNotUndoableL(aReason);
	// ask user if operation should continue
	return CEikonEnv::Static()->QueryWinL(R_WORD_NOT_UNDOABLE_TITLE, R_WORD_NOT_UNDOABLE_TEXT);
	}

CWordTextEditor::CWordTextEditor(const TGulBorder& aBorder):
	CEikRichTextEditor(aBorder)
	{
	}

TKeyResponse CWordTextEditor::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
	{
	return EKeyWasNotConsumed;
	}

CWordTextEditor::~CWordTextEditor()
	{
	if (iCommandManager)
		iCommandManager->Release();
	delete iEditorWithUndo;
	}

MUnifiedEditor& CWordTextEditor::UnifiedEditor()
	{
	ASSERT(iEditorWithUndo);
	return *iEditorWithUndo;
	}

const MUnifiedEditor& CWordTextEditor::UnifiedEditor() const
	{
	ASSERT(iEditorWithUndo);
	return *iEditorWithUndo;
	}

void CWordTextEditor::InitialiseUnifiedEditorL()
	{
	iFormAndEtextEditor.Set(*TextView(),*RichText());
	if (iCommandManager)
		{
		iCommandManager->Release();
		iCommandManager = 0;
		}
	delete iEditorWithUndo;
	iEditorWithUndo = 0;
	iCommandManager = UndoSystem::CCommandManager::NewL();
	iEditorWithUndo = CEditorWithUndo::NewL(iFormAndEtextEditor, iCommandManager);
	iEditorWithUndo->SetGatekeeper(&iGatekeeper);
	}

void CWordTextEditor::TestRunNotUndoableCommandL()
	{
	CTestCommand* command = new(ELeave) CTestCommand;
	CleanupStack::PushL(command);
	iCommandManager->ExecuteL(*command);
	CleanupStack::PopAndDestroy(command);
	}

void CWordTextEditor::InsertCharacterL(TChar aCharacter)
	{
	if ( aCharacter < 0x10000 )
		{
		//    Only surrogate high part or low part is inserted
		//    Ignore it
		if ( IsSurrogate( aCharacter ) )
		    return;
		
		TText c = (TText)aCharacter;
		TPtrC p(&c, 1);
		InsertTextL(iTextView->Selection().LowerPos(), p);
		}
	else
		{
		TText c[2];
		c[0] = GetHighSurrogate( aCharacter );
		c[1] = GetLowSurrogate( aCharacter );
		TPtrC p(c, 2);
		InsertTextL(iTextView->Selection().LowerPos(), p);
		}
	}

TBool CWordTextEditor::DeleteSelectionL()
	{
	TCursorSelection sel = iTextView->Selection();
	if (sel.Length())
		{
		DeleteTextL(sel.LowerPos(),sel.Length());
		return TRUE;
		}
	else
		return FALSE;
	}

void CWordTextEditor::DeleteLeftL()
	{
	if (!DeleteSelectionL())
		{
		TCursorSelection sel = iTextView->Selection();
		if (sel.iCursorPos > 0)
			DeleteTextL(sel.iCursorPos - 1, 1);
		}
	}

void CWordTextEditor::DeleteRightL()
	{
	if (!DeleteSelectionL())
		{
		TCursorSelection sel = iTextView->Selection();
		if (sel.iCursorPos < DocumentLength())
			DeleteTextL(sel.iCursorPos, 1);
		}
	}

void CWordTextEditor::HighlightL(TUint aEffects)
	{
	TTmCharFormatLayer format;
	format.iFormat.iEffects = aEffects;
	format.iFormat.iBackgroundColor = KRgbYellow;
	format.iMask.iFlags = TTmCharFormatMask::EBackground | TTmCharFormatMask::EBackgroundColor |
						  TTmCharFormatMask::EUnderline | TTmCharFormatMask::EStrikethrough |
						  TTmCharFormatMask::EShadow | TTmCharFormatMask::EUserDefinedEffects;
	TCursorSelection sel = iTextView->Selection();
	UnifiedEditor().SetCharFormatL(sel.LowerPos(),sel.Length(),format);
	}

void CWordTextEditor::RemoveHighlightL(TUint)
	{
	TCursorSelection sel = iTextView->Selection();
	TInt pos = sel.LowerPos();
	TInt length = sel.Length();

	TInt allowedFlags = ~(TTmCharFormatMask::EBackground | TTmCharFormatMask::EBackgroundColor |
						  TTmCharFormatMask::EUnderline | TTmCharFormatMask::EStrikethrough |
						  TTmCharFormatMask::EShadow | TTmCharFormatMask::EUserDefinedEffects);
	if (length == 0)
		{
		CRichText* rtext = RichText();
		TCharFormat tcf;
		TTmCharFormatMask notAllowed;
		notAllowed.iFlags = ~allowedFlags;
		TCharFormatMask tcfm;
		notAllowed.GetTCharFormatMask(tcfm);
		if (rtext)
			rtext->SetInsertCharFormatL(tcf, tcfm, pos);
		return;
		}

	iCommandManager->BeginBatchLC();
	MUnifiedEditor& ed = UnifiedEditor();
	while (0 < length)
		{
		TTmCharFormatLayer format;
		TInt runLength;
		ed.GetCharFormat(pos,
			MUnifiedEditor::ESpecific, format, runLength);
		format.iMask.iFlags &= allowedFlags;
		ed.DeleteCharFormatL(pos, length);
		ed.SetCharFormatL(pos, length, format);
		pos += runLength;
		length -= runLength;
		}

	CleanupStack::PopAndDestroy();	// batch
	}

CWordTextEditor::TAttributeValue CWordTextEditor::IsHighlighted(TUint aEffects) const
	{
	TCursorSelection sel = iTextView->Selection();
	TInt pos = sel.LowerPos();
	TInt length = sel.Length();
	const CRichText* rtext = RichText();
	if (length == 0 && rtext)
		{
		// need to find out if there is a zero length phrase. There is no
		// concept of this in MUnifiedEditor
		TCharFormat tcf;
		TCharFormatMask tcfm;
		rtext->GetCharFormat(tcf, tcfm, pos, 0);
		TTmCharFormat charFormat;
		charFormat = tcf;
		return (charFormat.iEffects & aEffects) == aEffects?
			EAttributeSet : EAttributeClear;
		}
	TUint retval = 0;
	while (retval != EAttributeIndeterminate && 0 < length)
		{
		TTmCharFormatLayer format;
		TInt runLength;
		UnifiedEditor().GetCharFormat(pos,
			MUnifiedEditor::EEffective, format, runLength);
		if ((format.iFormat.iEffects & aEffects) == aEffects)
			retval |= EAttributeSet;
		else
			retval |= EAttributeClear;
		pos += runLength;
		length -= runLength;
		}

	return static_cast<TAttributeValue>(retval);
	}

void CWordTextEditor::ToggleHighlightL(TUint aEffects)
	{
	if (IsHighlighted(aEffects) == EAttributeSet)
		RemoveHighlightL(aEffects);
	else
		HighlightL(aEffects);
	}

void CWordTextEditor::BoldItalicUnderlineEventL(TInt aFontFlag)
	{
	TCursorSelection sel = iTextView->Selection();
	if (sel.Length())
		{
		TTmCharFormatLayer old_format;
		TInt run_length;
		GetCharFormat(sel.LowerPos(),MUnifiedEditor::EEffective,old_format,run_length);
		TTmCharFormatLayer new_format;
		new_format.iMask.iFlags = 0;
		if (aFontFlag & EBold)
			{
			new_format.iMask.Set(TTmCharFormatMask::EBold);
			new_format.iFormat.iFontSpec.SetBold(!old_format.iFormat.iFontSpec.IsBold());
			}
		if (aFontFlag & EItalic)
			{
			new_format.iMask.Set(TTmCharFormatMask::EItalic);
			new_format.iFormat.iFontSpec.SetItalic(!old_format.iFormat.iFontSpec.IsItalic());
			}
		if (aFontFlag & EUnderline)
			{
			new_format.iMask.Set(TTmCharFormatMask::EUnderline);
			if (!(old_format.iFormat.iEffects & TTmCharFormat::EUnderline))
				new_format.iFormat.iEffects |= TTmCharFormat::EUnderline;
			}
		SetCharFormatL(sel.LowerPos(),sel.Length(),new_format);
		}
	else
		CEikGlobalTextEditor::BoldItalicUnderlineEventL(aFontFlag);
	}

TInt CWordTextEditor::DocumentLength() const
	{
	return UnifiedEditor().DocumentLength();
	}

void CWordTextEditor::GetText(TInt aPos,TPtrC& aText) const
	{
	UnifiedEditor().GetText(aPos,aText);
	}

void CWordTextEditor::GetBaseFormatL(TTmCharFormat& aCharFormat,RTmParFormat& aParFormat) const
	{
	UnifiedEditor().GetBaseFormatL(aCharFormat,aParFormat);
	}

void CWordTextEditor::GetCharFormat(TInt aPos,TFormatLevel aLevel,
		TTmCharFormatLayer& aFormat,TInt& aRunLength) const
	{
	UnifiedEditor().GetCharFormat(aPos,aLevel,aFormat,aRunLength);
	}

void CWordTextEditor::GetParFormatL(TInt aPos,TFormatLevel aLevel,
		RTmParFormatLayer& aFormat,TInt& aRunLength) const
	{
	UnifiedEditor().GetParFormatL(aPos,aLevel,aFormat,aRunLength);
	}

void CWordTextEditor::InsertTextL(TInt aPos,const TDesC& aText,
		 const TDesC* aStyle,
		 const TTmCharFormatLayer* aCharFormat,
		 const RTmParFormatLayer* aParFormat)
	{
	DeleteSelectionL();
	UnifiedEditor().InsertTextL(aPos,aText,aStyle,aCharFormat,aParFormat);
	UpdateScrollBarsL();
	}

void CWordTextEditor::DeleteTextL(TInt aPos,TInt aLength)
	{
	UnifiedEditor().DeleteTextL(aPos,aLength);
	UpdateScrollBarsL();
	}

void CWordTextEditor::SetBaseFormatL(const TTmCharFormat& aCharFormat,const RTmParFormat& aParFormat)
	{
	UnifiedEditor().SetBaseFormatL(aCharFormat,aParFormat);
	UpdateScrollBarsL();
	}

void CWordTextEditor::SetCharFormatL(TInt aPos,TInt aLength,const TTmCharFormatLayer& aFormat)
	{
	UnifiedEditor().SetCharFormatL(aPos,aLength,aFormat);
	UpdateScrollBarsL();
	}

void CWordTextEditor::SetParFormatL(TInt aPos,TInt aLength,const RTmParFormatLayer& aFormat)
	{
	UnifiedEditor().SetParFormatL(aPos,aLength,aFormat);
	UpdateScrollBarsL();
	}

void CWordTextEditor::DeleteCharFormatL(TInt aPos,TInt aLength)
	{
	UnifiedEditor().DeleteCharFormatL(aPos,aLength);
	UpdateScrollBarsL();
	}

void CWordTextEditor::DeleteParFormatL(TInt aPos,TInt aLength)
	{
	UnifiedEditor().DeleteParFormatL(aPos,aLength);
	UpdateScrollBarsL();
	}

MTmOptionalInterface* CWordTextEditor::Interface(TUint aId)
	{
	return UnifiedEditor().Interface(aId);
	}

void CWordTextEditor::HandleResourceChange(TInt aType)
	{
	CEikBorderedControl::HandleResourceChange(aType);
	switch (aType)
		{
	case KEikMessageCapsLock:
		break;
	case KEikMessageVirtualCursorStateChange:
		{
		TEikVirtualCursor& cursor=iEikonEnv->VirtualCursor();
		if(!(iEdwinUserFlags&EIgnoreVirtualCursor) && IsFocused()
			&& cursor.CursorState(*iEikonEnv)==TEikVirtualCursor::EOn)
			{
			// This doesn't leave
			cursor.SetCursorStateL(TEikVirtualCursor::ESuspended,*iEikonEnv);
			}
		}
		break;
	default:
		break;
		}

	}

void CWordTextEditor::FocusChanged(TDrawNow /*aDrawNow*/)
	{
	if (!iTextView)
		return;
	const TBool focused=IsFocused();
	const TCursor::TVisibility textCursor=(focused? TCursor::EFCursorFlashing : TCursor::EFCursorInvisible);
	const TCursor::TVisibility lineCursor=((iEdwinUserFlags&ELineCursor && focused)? 
										TCursor::EFCursorVisible : TCursor::EFCursorInvisible);
	TRAPD(ignored,iTextView->SetCursorVisibilityL(lineCursor,textCursor));
	if (!focused && iEdwinUserFlags&EAlwaysShowSelection)
		;
	else
		{
		TRAP(ignored,iTextView->SetSelectionVisibilityL(focused)); // !! inefficient
		}
	//TRAP(ignored,SetVirtualCursorStateL(focused));	// virtual cursor not supported in RefUi word.
	}

TInt CWordTextEditor::ClipboardPasteL()
	{
	MUnifiedEditor::MClipboardSupport* ci = iEditorWithUndo->ClipboardSupport();
	if (!ci)
		return KErrNotSupported;
	CClipboard* cb = CClipboard::NewForReadingLC(iCoeEnv->FsSession());
	if (cb->StreamDictionary().At(KClipboardUidTypePlainText) == KNullStreamId)
		{
		CleanupStack::PopAndDestroy(cb);
		return KErrNotFound;
		}
	iCommandManager->BeginBatchLC();		// batch together the delete and insert

	DeleteSelectionL();
	ci->PasteFromStoreL(cb->Store(), cb->StreamDictionary(),
		iTextView->Selection().iCursorPos);

	CleanupStack::PopAndDestroy();		// close batch
	CleanupStack::PopAndDestroy(cb);
	UpdateScrollBarsL();

	return KErrNone;
	}

void CWordTextEditor::ClipboardCutL()
	{
	ClipboardCopyL();
	DeleteSelectionL();
	}

void CWordTextEditor::ClipboardCopyL() const
	{
	MUnifiedEditor::MClipboardSupport* ci = iEditorWithUndo->ClipboardSupport();
	if (!ci)
		return;
	TCursorSelection sel = iTextView->Selection();
	if (!sel.Length())
		return;
	CClipboard* cb = CClipboard::NewForWritingLC(iCoeEnv->FsSession());
	ci->CopyToStoreL(cb->Store(), cb->StreamDictionary(), sel.LowerPos(), sel.Length());
	cb->CommitL();
	CleanupStack::PopAndDestroy(cb);
	}

void CWordTextEditor::SetBookmark()
	{
	iCommandManager->SetBookmark();
	}

TBool CWordTextEditor::IsAtBookmark() const
	{
	return iCommandManager->IsAtBookmark();
	}

void CWordTextEditor::ResetUndo()
	{
	iCommandManager->ResetUndo();
	}

void CWordTextEditor::InsertFromHtmlFileL(const TDesC& aFileName)
	{
	//get converter
	CCnaConverterList* convList=CCnaConverterList::NewLC();
	const TUid KUidCHtmlToCrtConverter = {0x1000a90e};
	CConverterBase* conv = convList->NewConverterL(KUidCHtmlToCrtConverter);

	if (conv)
		{
		//do conversion
		CleanupStack::PushL(conv);
		
		_LIT(KTempRichTextDestinationFile, "_:\\System\\Temp\\TempRichText");
		TFileName richtextdestFile(KTempRichTextDestinationFile);
		richtextdestFile[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
		
		conv->ConvertL(aFileName, richtextdestFile);	
		CleanupStack::PopAndDestroy(conv);

		CancelFepTransaction();
		const TInt oldLength = iText->DocumentLength();
		const TInt cursorPos=CursorPos();
		CDirectFileStore* directFileStore=CDirectFileStore::OpenLC(iCoeEnv->FsSession(), richtextdestFile, EFileShareReadersOnly);
		TStreamId streamId(directFileStore->Root());
		RStoreReadStream readStream;
		CStreamDictionary* dictionary=CStreamDictionary::NewLC();
		readStream.OpenLC(*directFileStore, streamId);
		readStream>>*dictionary;
		User::LeaveIfError(iText->PasteFromStoreL(*directFileStore, *dictionary, cursorPos));

		CleanupStack::PopAndDestroy(3);//readStream, dictionary, directFileStore
		iCoeEnv->FsSession().Delete(richtextdestFile);

		const TInt KFullFormattingUpperThreshold=2000;//already defined in \uikon\coctlsrc\eikedwin.cpp

		CheckValidityOfChars(oldLength,(iText->DocumentLength()-oldLength));
		const TInt newLength=iText->DocumentLength();
		const TInt newCursorPos=cursorPos+newLength-oldLength;
		iTextView->SetPendingSelection(TCursorSelection(newCursorPos,newCursorPos));
		if (newLength>KFullFormattingUpperThreshold &&
			oldLength<=KFullFormattingUpperThreshold)
			SetAmountToFormatL();
		else
			iTextView->HandleInsertDeleteL(TCursorSelection(newCursorPos,cursorPos),0,ETrue);
		DrawContents();
		UpdateScrollBarsL();
		ReportEventL(MCoeControlObserver::EEventStateChanged);
		iEikonEnv->BusyMsgCancel();
		}
	else
		{
		iEikonEnv->InfoMsg(R_WORD_NO_HTML_CONVERTER);
		}

	CleanupStack::PopAndDestroy(convList);
	}
