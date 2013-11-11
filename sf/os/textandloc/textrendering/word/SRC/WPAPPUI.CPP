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


#include <s32mem.h>

#include <txtrich.h>

#include <frmprint.h>

#include <badesca.h>

#include <coedef.h>
#include <coeinput.h>

#include <prnsetup.h>

#include <techview/eikprtdg.h>
#include <techview/eikmenup.h>
#include <techview/eiktbar.h>
#include <techview/eikbtgpc.h>
#include <techview/eikclb.h>
#include <techview/eikon.rsg>
#include <eikon.mbg>
#include <techview/eiktxlbm.h>

#include "WPAPPUI.H"
#include "WPDIALGS.H"
#include "WPDOC.H"
#include "WPLBOX.H"
#include "WPSTYLE.H"
#include "WPDEF.H"
#include "WPTEST.H"
#include <word.rsg>

#include "WORD.HRH"


const TUid KUidSheet={0x10003A7D};
const TUid KUidPaintApp={0x1000007D};
// the next four are strictly temporary items!
_LIT(KSketchDllName,"PAINT.APP");
_LIT(KSheetDllName,"SHEET.APP");

CWordAppUi::CWordAppUi()
   	:iAlignment(RTmParFormat::EAlignNormal)
	{
	}

CWordAppUi::~CWordAppUi()
    {
	if (iDoorObserver)
		{
		MApaEmbeddedDocObserver::TExitMode exitMode=MApaEmbeddedDocObserver::EKeepChanges;
		if (!DocHasChanged())
			exitMode=MApaEmbeddedDocObserver::ENoChanges;
		if (Document()->Model()->Text()->DocumentLength()==0)
			exitMode=MApaEmbeddedDocObserver::EEmpty;
		iDoorObserver->NotifyExit(exitMode);
		}
	if (iRichEd)
		RemoveFromStack(iRichEd);
	delete iBullet;
	delete iToolBarUpdate;
	delete iRichEd;
	delete iTwipsList;
	delete iPopoutListBox;
	delete iConfirmationText;
	delete iTest;
	}

void CWordAppUi::ConstructL()
    {
	Document()->SetChanged(FALSE);
	//
	BaseConstructL();
	//
	iModel=Document()->Model();
	ResetPrint();  // initialise print related components
	CreateEdwinL();  // create & initialise edwin components (may use print data if in wysiwyg)
	UpdateToolbarsL();  // set the remainder components of the word view
	//
	iToolBarUpdate=CWordToolBarUpdate::NewL(*this);
	//
	iEikonEnv->AppUiFactory()->ToolBand()->ActivateL();
	iEikonEnv->AppUiFactory()->ToolBar()->ActivateL();
	iEikonEnv->AppUiFactory()->ToolBand()->DrawNow();
	iEikonEnv->AppUiFactory()->ToolBar()->DrawNow();
	iCoeEnv->WsSession().Flush();
	iConfirmationText = iEikonEnv->AllocReadResourceAsDes8L(R_WORD_CONFIRMATION_TEXTS);
	SetPaginationOutOfDate(TRUE);
	iRichEd->SetBookmark();
    }

TKeyResponse CWordAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	if (iRichEd->IsReadOnly())
		{
		iRichEd->CEikRichTextEditor::OfferKeyEventL(aKeyEvent, aType);
		return EKeyWasConsumed;
		}
	const TBool eventKey(aType == EEventKey);
	const TBool isEnter(aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyLineFeed);
	const TCursorSelection selection = iRichEd->Selection();
	if (eventKey && (aKeyEvent.iModifiers & EModifierCtrl) && (aKeyEvent.iModifiers & EModifierFunc))
		{// Paragraph style hot key
		TChar key = ((aKeyEvent.iCode >= 1) && (aKeyEvent.iCode <= 26))
			? aKeyEvent.iCode + 'A' - 1	// an alphabetic key was pressed.
			: aKeyEvent.iCode;			// a non-alphabetic key was pressed
		if (ProcessStyleHotKeyL(key) == EKeyWasConsumed)
			return EKeyWasConsumed;
		}
	if (eventKey && isEnter && (aKeyEvent.iModifiers & EModifierCtrl))
		{// Insert a breaking character
		InsertCharacterL(CEditableText::EPageBreak);
		return EKeyWasConsumed;
		}

	if ((aKeyEvent.iCode==EKeySpace || isEnter) && iRichEd->CheckForObjectL())
		{
		return EKeyWasConsumed;
		}

	// Allow the test harness to apply a custom keyboard map.
	TChar c;
	TKeyResponse r = iTest->OfferKeyEventL(aKeyEvent,aType,c);
	if (r == EKeyWasConsumed)
		{
		if (c != 0xFFFF)
			InsertCharacterL(c);
		return EKeyWasConsumed;
		}

	/*
	Handle all non-system keycodes via the unified editing interface, (except for space and enter
	on an embedded object, which allow it to be edited).
	*/
	if (eventKey && (aKeyEvent.iCode < 0xF700 || aKeyEvent.iCode >= 0xF900))
		{
		if (aKeyEvent.iCode == EKeyBackspace)
			iRichEd->DeleteLeftL();
		else if (aKeyEvent.iCode == EKeyDelete)
			iRichEd->DeleteRightL();
		else if (aKeyEvent.iCode == EKeySpace && (aKeyEvent.iModifiers & EModifierShift))
			InsertCharacterL(CEditableText::ENonBreakingSpace);
		else if (isEnter)
			{
			InsertCharacterL((aKeyEvent.iModifiers & EModifierShift)?
				CEditableText::ELineBreak : CEditableText::EParagraphDelimiter);
			}
		else
			InsertCharacterL(aKeyEvent.iCode);
		return EKeyWasConsumed;
		}
	iRichEd->CEikRichTextEditor::OfferKeyEventL(aKeyEvent, aType);
	if (eventKey && (iEikonEnv->AppUiFactory()->ToolBar()->IsVisible() || iEikonEnv->AppUiFactory()->ToolBand()->IsVisible()))
		{
		if ((aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete) && selection.Length())
			iToolBarUpdate->Start(CWordToolBarUpdate::EFullUpdate);
		else if (isEnter && !(aKeyEvent.iModifiers & EModifierShift))
			iToolBarUpdate->Start(CWordToolBarUpdate::EFullUpdate);
		}
	return EKeyWasConsumed;
	}

TKeyResponse CWordAppUi::ProcessStyleHotKeyL(const TChar& aKey)
// Look up the specified key in the style short cut list.
// Apply the relevant style if found.
//
	{
	const CParagraphStyle* styleToApply=NULL;
	if (aKey==iModel->NormalStyleShortCut())
		styleToApply=REINTERPRET_CAST(const CParagraphStyle*,Text()->GlobalParaFormatLayer());
	else
		{
		const CStyleShortCutList* shortCutList=iModel->StyleShortCutList();
		const TInt count=(shortCutList) ? shortCutList->Count() : 0;
		TInt index=0;
		TBool matched=FALSE;
		while (index<count && !matched)
			{
			TChar match=(*shortCutList)[index];
			if (match==aKey)
				break;
			index++;
			}
		if (index>=count)
			return EKeyWasNotConsumed;  // checked all hotkeys in the list and not matched any of them.
		//there could be a shortcut that hasnt a style
		if ( index > iModel->StyleList()->Count() )
			return EKeyWasNotConsumed; 
		styleToApply=iModel->StyleList()->At(index).iStyle;
		}
	TCursorSelection selection=iRichEd->Selection();
	Text()->ApplyParagraphStyleL(*styleToApply,selection.LowerPos(),selection.Length(),KApplyParaStyleMode);
	ReformatDueToStateChangeL();
	iRichEd->UpdateScrollBarsL();
	return EKeyWasConsumed;
	}


void CWordAppUi::SetAndDrawFocus(TBool aFocus)
	{
	if (iRichEd)
		iRichEd->SetFocus(aFocus,EDrawNow);
	}


TCoeInputCapabilities CWordAppUi::InputCapabilities() const
	{
	TCoeInputCapabilities inputCapabilities=CEikAppUi::InputCapabilities();
	if ((iRichEd!=NULL) && iRichEd->IsFocused() && !iRichEd->IsReadOnly())
		{
		inputCapabilities.MergeWith(iRichEd->InputCapabilities());
		}
	return inputCapabilities;
	}

TBool CWordAppUi::DocHasChanged() const
	{
	if (iDocument->HasChanged())
		return ETrue;
	if (iRichEd->IsAtBookmark())
		return EFalse;
	return ETrue;
	}

void CWordAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdExit:
		GetUiData(Document()->UiData());
		if (DocHasChanged() && !iContainerAppUi)
			CmdFileSaveL();
		Exit();
		break;
	case EEikCmdFileNew:
	case EEikCmdFileOpen:
	case EEikCmdFileSave:
	case EEikCmdFileSaveAs:
	case EEikCmdFileRevert:
	case EWordCmdFileSaveAsTemplate:
	case EWordCmdFileDeleteTemplate:
		if (iContainerAppUi)
			break;
		else
			{
			switch (aCommand)
				{
			case (EEikCmdFileNew):
				CmdFileNewL();
		        break;
			case (EEikCmdFileOpen):
				CmdFileOpenL();
				break;
			case (EEikCmdFileSave):
				CmdFileSaveL();
				break;
			case (EEikCmdFileSaveAs):
		        CmdFileSaveAsL();
			    break;
			case (EEikCmdFileRevert):
		        CmdFileRevertL();
			    break;
			case (EWordCmdFileSaveAsTemplate):
				CmdFileSaveAsTemplateL();
				break;
			case (EWordCmdFileDeleteTemplate):
				CmdFileDeleteTemplateL();
				break;
				}
			}
		break;
	case EEikCmdEditCopy:
		iRichEd->ClipboardCopyL();
		break;
	case EEikCmdFileExport:
		CmdFileTextExportL();
		break;
	case EWordCmdPageSetup:
		RunPageSetupDialogL();
		break;
	case EEikCmdPrintPaginate:
		RunPaginationDialogL();
		break;
	case EEikCmdPrintPrint:
	case EEikCmdPrintSetup:
	case EEikCmdPrintPreview:										    
	case EWordCmdGoTo:	
		{
		TBool cancel=FALSE;
 		if (PaginationOutOfDate())
			cancel=RunPaginationDialogL();
		if (cancel)
			break;
		switch (aCommand)
			{
			case EEikCmdPrintSetup:
				{
				TInt pages=iModel->PageTable()->Count();
				RunPrintRangeDialogL(iPrintSetup,pages);
				break;
				}
			case EEikCmdPrintPrint:
				RunPrintDialogL();
				break;
			case EEikCmdPrintPreview:
				RunPreviewDialogL();
				break;
			case EWordCmdGoTo:
				RunGoToDialogL();
				break;
			}
		break;
		}
	case EEikCmdSearchFind:
		iRichEd->RunFindDialogL();
		break;
	case EEikCmdSearchFindAgain:
		iRichEd->RunFindAgainDialogL();
		break;
	case EEikCmdEditSelectAll:
		{
		const TInt docLength=iRichEd->TextLength();
		if (docLength)
			iRichEd->SetSelectionL(docLength,0);
		break;
		}
	case EWordCmdReEditObject:
		iRichEd->ReEditObjectL();
		break;
	case EWordCmdToolbar:
	case EWordCmdToolband:
		UpdateToolBarAndBandL(aCommand);
		break;
	case EWordCmdWrapToScreen:
		CmdWrapToScreenL();
		break;
	case EWordCmdViewPreferences:
		RunViewPreferencesDialogL();
		break;
	case EWordCmdCount:
		iEikonEnv->InfoMsg(R_WORD_NO_WORDS,(iRichEd->CountWords()));	
		break;
	case EEikCmdZoomIn:
	case EEikCmdZoomOut:
		ZoomL(aCommand);
		break;
	case EWordCmdZoomSet:
		{
		TInt zoomFactor=iAppZoom.ZoomFactor();
		CEikDialog* dialog=new(ELeave) CWordTempManualZoomDialog(zoomFactor);
		if (dialog->ExecuteLD(R_WORD_DIALOG_ZOOM_SET))
			{
			iAppZoom.SetZoomFactor(zoomFactor);
			CFbsBitmap* lineCursor=iEikonEnv->CreateBitmapL(TPtrC(),EMbmEikonLncusr1);
			iRichEd->SetLineCursorBitmapL(lineCursor);
			iRichEd->NotifyNewFormatL();
			iRichEd->UpdateScrollBarsL();
			}
		break;
		}
	case EWordCmdOutline:
		RunOutlineDialogL();
		break;
	//
	// State changing commands
	// disallow when read-only
	//
	// FILE HANDLING
	case EEikCmdFileImport:
	case EWordCmdHtmlImport:
	//
	// CLIPBOARDING
	case EEikCmdEditCut:
	case EEikCmdEditPaste:
	//
	// CHARACTER FORMATTING
	case EEikCmdFontBold:
	case EEikCmdFontItalic:
	case EEikCmdFontUnderline:
	case EEikCmdFontChange:
	case EEikCmdInsertSymbol:
	//
	// PARAGRAPH FORMATTING
	case EEikCmdParaBorder:
	case EEikCmdParaSpacing:
	case EEikCmdParaIndent:
	case EEikCmdParaTabs:
	case EEikCmdParaAlign:
	case EWordCmdAlignLeft:
	case EWordCmdAlignRight:
	case EWordCmdAlignCenter:
	case EWordCmdAlignJustify:
	case EWordCmdFormatBullet:
	//
	// EDIT
	case EEikCmdSearchReplace:
	case EEikCmdEditPictureFormat:
	case EWordCmdInsertSketch:
	case EWordCmdInsertSheet:
	case EWordCmdInsertObject:
	case EWordCmdInsertPageBreak:
	case EWordCmdInsertLineBreak:
	case EEikCmdEditUndo:
	case EEikCmdEditRedo:
	//
	// TOOLS
	case EWordCmdObjectPreferences:
	//
	// TOP TOOLBAND
	case EWordButtonStyle:
	case EWordButtonFont:
	case EWordButtonFontSize:		
	case EWordButtonAlign:
	case EWordButtonBorders:
	case EWordCmdInsertBullet:
	case EWordButtonBullets:
	//
	// STYLES
	case EWordCmdRevertToStylePara:
	case EWordCmdRevertToStyleChar:
	case EWordCmdStyleGallery:
		{
		const TBool readOnly=(iContainerAppUi)? iEmbeddedAndReadOnly : !(iDocument->AppFileMode()&EFileWrite);
		if (readOnly)
			{
			iEikonEnv->InfoMsg(R_WORD_FILE_READ_ONLY);  // feedback this is a readonly file
			if (aCommand==EEikCmdFontBold || aCommand==EEikCmdFontItalic || aCommand==EEikCmdFontUnderline)
				UpdateCharFormatButtonsL();  // undo the pressing of the emphasis buttons.
			break;
			}
		switch (aCommand)
			{
		//
		// FILE HANDLING
			case EEikCmdFileImport:
				CmdFileTextImportL();
				break;
			case EWordCmdHtmlImport:
				CmdFileHtmlImportL();
				break;			
		//
		// CLIPBOARDING
			case EEikCmdEditCut:
				iRichEd->ClipboardCutL();
				UpdateToolbandButtonsL();
				break;
			case EEikCmdEditPaste:
				if (KErrNotFound == iRichEd->ClipboardPasteL())
					iEikonEnv->InfoMsg(R_WORD_NO_TEXT_TO_PASTE);
				else
					UpdateToolbandButtonsL();
				break;
		//
		// CHARACTER FORMATTING
			case EEikCmdFontBold:
				iRichEd->BoldItalicUnderlineEventL(CEikGlobalTextEditor::EBold); 
				break;
			case EEikCmdFontItalic:
				iRichEd->BoldItalicUnderlineEventL(CEikGlobalTextEditor::EItalic); 
				break;
			case EEikCmdFontUnderline:
				iRichEd->BoldItalicUnderlineEventL(CEikGlobalTextEditor::EUnderline); 
				break;
			case EEikCmdFontChange:
				iRichEd->RunFontDialogL(EGulNoSymbolFonts, iCoeEnv->ScreenDevice());
				UpdateCharFormatButtonsL();
				break;
			case EEikCmdInsertSymbol:
				iRichEd->RunCharMapDialogL();
				break;
		//
		// PARAGRAPH FORMATTING
			case EEikCmdParaBorder:
			case EEikCmdParaSpacing:
				iRichEd->RunParaDialogsL(aCommand);
				break;
			case EEikCmdParaIndent:
			case EEikCmdParaTabs:
				{
				TPageSpec pageSpec=iPrintSetup->PrinterDevice()->CurrentPageSpecInTwips();
				TInt width=pageSpec.iPortraitPageSize.iWidth;
				if (pageSpec.iOrientation==TPageSpec::ELandscape)
					width=pageSpec.iPortraitPageSize.iHeight;
				width-=(iPrintSetup->iPageMarginsInTwips.iMargins.iLeft+iPrintSetup->iPageMarginsInTwips.iMargins.iRight);
				if (aCommand==EEikCmdParaIndent)
					iRichEd->RunIndentsDialogL(width);
				else
					iRichEd->RunTabsDialogL(width);
				break;
				}
			case EEikCmdParaAlign:
				CmdChangeAlignmentL();
				break;
			case EWordCmdAlignLeft:
			case EWordCmdAlignRight:
			case EWordCmdAlignCenter:
			case EWordCmdAlignJustify:
				{
				SetParaAlignmentL(aCommand);	
				RTmParFormat::TAlignment alignment = RTmParFormat::EAlignNormal;
				switch (aCommand)
					{
					case EWordCmdAlignLeft:
						alignment = RTmParFormat::EAlignNormal;
						break;
					case EWordCmdAlignRight:
						alignment = RTmParFormat::EAlignReverse;
						break;
					case EWordCmdAlignCenter:
						alignment = RTmParFormat::EAlignCenter;
						break;
					case EWordCmdAlignJustify:
						alignment = RTmParFormat::EAlignJustify;
						break;
					}
				UpdateAlignButtonL(alignment);
				break;
				}
			case EWordCmdFormatBullet:
				RunSelectBulletDialogL();
				break;
		//
		// EDIT
			case EEikCmdSearchReplace:
				iRichEd->RunReplaceDialogL();
				break;
			case EEikCmdEditPictureFormat:
				iRichEd->EditPictureFormatL();
				break;
			case EWordCmdInsertSketch:
				iRichEd->InsertObjectL(KSketchDllName,KUidPaintApp,(SketchAsGlass())?CEikRichTextEditor::EGlassIfPossible : CEikRichTextEditor::EAlwaysIconic);
				break;
			case EWordCmdInsertSheet:
				iRichEd->InsertObjectL(KSheetDllName,KUidSheet,(GraphAsGlass())?CEikRichTextEditor::EGlassIfPossible : CEikRichTextEditor::EAlwaysIconic);
				break;
			case EWordCmdInsertObject:
				iRichEd->InsertObjectL(CEikRichTextEditor::EGlassIfPossible);
				break;
			case EWordCmdInsertPageBreak:
			case EWordCmdInsertLineBreak:
				InsertCharacterL((aCommand==EWordCmdInsertPageBreak)
					? CEditableText::EPageBreak
					: CEditableText::ELineBreak);
				break;
			case EEikCmdEditUndo:
				iRichEd->UndoL();
				break;
			case EEikCmdEditRedo:
				iRichEd->RedoL();
				break;
		//
		// TOOLS
			case EWordCmdObjectPreferences:
				RunObjectPreferencesDialogL();
				break;
		//
		// TOP TOOLBAND
			case EWordButtonStyle:
				LaunchStylesPopupL();
				break;
			case EWordButtonFont:
				LaunchFontsPopupL();
				break;
			case EWordButtonFontSize:		
				LaunchFontHeightsPopupL();
				break; 
			case EWordButtonAlign:
				LaunchAlignPopupL();
				break;
			case EWordButtonBorders:
				LaunchBordersPopupL();
				break;
			case EWordCmdInsertBullet:
				{
				if (REINTERPRET_CAST(CEikButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonBullets))->State() == CEikButtonBase::EClear)
					UpdateToolBandButton(EWordButtonBullets,CEikButtonBase::ESet); 
				else
					UpdateToolBandButton(EWordButtonBullets,CEikButtonBase::EClear);
				}  // drops into next case with a purpose
			case EWordButtonBullets:
				SetBulletL();
				break;
		//
		// STYLES
			case EWordCmdRevertToStylePara:
				{
				TCursorSelection selection=iRichEd->Selection();
				Text()->RemoveSpecificParaFormatL(selection.LowerPos(),selection.Length());
				ReformatDueToStateChangeL();
				break;
				}
			case EWordCmdRevertToStyleChar:
				{
				TCursorSelection selection=iRichEd->Selection();
				Text()->RemoveSpecificCharFormatL(selection.LowerPos(),selection.Length());
				ReformatDueToStateChangeL();
				break;
				}
			case EWordCmdStyleGallery:
				{
				TBool close=FALSE;
				CRichText* originalText=Text();
				const TCursorSelection selection=iRichEd->Selection();
				TChar& normalStyleShortCut=iModel->NormalStyleShortCut();
				TWordStyleGalleryData data(*originalText,*iModel->StyleList(),*iModel->StyleShortCutList(),
					normalStyleShortCut,*iPrintSetup,close,selection,*iRichEd);

				CEikDialog* dialog=CWordStyleGallery::NewL(data);
				dialog->ExecuteLD(R_WORD_DIALOG_STYLE_GALLERY);
				if (!close)
					break;
				ReformatDueToStateChangeL();
				SetDocChanged();
				break;
				}
			}

		break;
		}

		// test commands
		case EWordCmdHighlight:
			iRichEd->ToggleHighlightL(TTmCharFormat::EBackground);
			break;
		case EWordCmdShadow:
			iRichEd->ToggleHighlightL(TTmCharFormat::EShadow);
			break;
		case EWordCmdCustomHighlight:
			iRichEd->ToggleHighlightL((TUint)TFontPresentation::EFontHighlightFirstCustomStyle << TTmCharFormat::EUserDefinedShift);
			break;
		case EWordCmdStandardKeyboard:
			iTest->SetKeyboardL(CWordTest::EStandardKeyboard);
			break;
		case EWordCmdArabicKeyboard:
			iTest->SetKeyboardL(CWordTest::EArabicKeyboard);
			break;
		case EWordCmdGreekKeyboard:
			iTest->SetKeyboardL(CWordTest::EGreekKeyboard);
			break;
		case EWordCmdRussianKeyboard:
			iTest->SetKeyboardL(CWordTest::ERussianKeyboard);
			break;
		case EWordCmdChineseOn:
		    {
			const TUid KUidTFepChinese = { 0x1028716D };
			iEikonEnv->InstallFepL(KUidTFepChinese);
		    }
		break;
		case EWordCmdChineseOff:
			iEikonEnv->InstallFepL(KNullUid);			
			break;	
		case EWordCmdRuledPaper:
			iTest->SetRuledPaperL(iRichEd,!iTest->RuledPaper());
			break;
		case EWordCmdBackgroundBitmap:
			iTest->SetBackgroundBitmapL(iRichEd,!iTest->BackgroundBitmap());
			break;
		case EWordCmdUpperCase:
			iTest->ToggleCaseL(iRichEd);
			break;
		case EWordCmdBenchmark:
			iTest->BenchmarkL();
			break;
		case EWordCmdTruncateWithEllipsis:
			iTest->SetTruncateWithEllipsisL(iRichEd,!iTest->TruncateWithEllipsis());
			break;
		case EWordCmdPictureAlignTop:
			iTest->SetPictureAlignmentL(iRichEd,TFontPresentation::EAlignTop);
			break;
		case EWordCmdPictureAlignBottom:
			iTest->SetPictureAlignmentL(iRichEd,TFontPresentation::EAlignBottom);
			break;
		case EWordCmdPictureAlignCenter:
			iTest->SetPictureAlignmentL(iRichEd,TFontPresentation::EAlignCentered);
			break;
		case EWordCmdPictureAlignBaseline:
			iTest->SetPictureAlignmentL(iRichEd,TFontPresentation::EAlignBaseLine);
			break;
		case EWordCmdWrapOn:
			iTest->SetWrapL(iRichEd,TRUE);
			break;
		case EWordCmdWrapOff:
			iTest->SetWrapL(iRichEd,FALSE);
			break;
		case EWordCmdInsertPicture:
			{
			iTest->InsertPictureL(iRichEd);
			Document()->SetChanged(ETrue);
			break;	
			}		
		case EWordCmdCursorStyleFlash:
			iTest->ToggleCursorFlash();
			break;
		case EWordCmdCursorStyleVertical:
			iTest->SetCursorPlacement(ECursorVertical);
			break;
		case EWordCmdCursorStyleUnderlineNext:
			iTest->SetCursorPlacement(ECursorUnderlineNext);
			break;
		case EWordCmdCursorStyleUnderlinePrev:
			iTest->SetCursorPlacement(ECursorUnderlinePrev);
			break;
		case EWordCmdCursorStyleHeavy:
			iTest->SetCursorWeight(iTest->CursorWeight() == 3 ? 1 : 3);
			break;
		case EWordCmdCursorStyleRed:
			iTest->SetCursorXorColor(iTest->CursorXorColor() == KRgbWhite ? KRgbCyan : KRgbWhite);
			break;
		case EWordCmdUndoTestNotUndoable:
			iRichEd->TestRunNotUndoableCommandL();
			break;
		case EWordCmdPositioningHintNeutral:
			iTest->SetCursorPositioningHintL(TCursorPosition::EPosHintUndefined);
			break;
		case EWordCmdPositioningHintInsertLeftToRight:
			iTest->SetCursorPositioningHintL(TCursorPosition::EInsertStrongL2R);
			break;
		case EWordCmdPositioningHintInsertRightToLeft:
			iTest->SetCursorPositioningHintL(TCursorPosition::EInsertStrongR2L);
			break;
		case EWordCmdSelectionHighlightNormal:
			iRichEd->TextView()->SetSelectionVisibilityL(EFalse);
			iTest->SetSelectionHighlightStyle(0);
			iRichEd->TextView()->SetSelectionVisibilityL(ETrue);
			break;
		case EWordCmdSelectionHighlightCustom1:
			iRichEd->TextView()->SetSelectionVisibilityL(EFalse);
			iTest->SetSelectionHighlightStyle(1);
			iRichEd->TextView()->SetSelectionVisibilityL(ETrue);
			break;
		case EWordCmdSelectionHighlightCustom2:
			iRichEd->TextView()->SetSelectionVisibilityL(EFalse);
			iTest->SetSelectionHighlightStyle(2);
			iRichEd->TextView()->SetSelectionVisibilityL(ETrue);
			break;
		}
	}

void CWordAppUi::DynInitMenuPaneL(TInt aMenuId,CEikMenuPane* aMenuPane)
	{
	TBool readOnly=iRichEd->IsReadOnly();
	switch (aMenuId)
		{
	//
	//
	case R_WORD_FILE_MENU:
		{
		if (iContainerAppUi)
			{
			aMenuPane->DeleteMenuItem(EEikCmdFileNew);
			aMenuPane->DeleteMenuItem(EEikCmdFileOpen);
			}
		break;
		}
	//
	//
	case R_WORD_FILEOPTIONS_MENU:
		{
		if (iContainerAppUi)
			{
			aMenuPane->DeleteMenuItem(EEikCmdFileSave);
			aMenuPane->DeleteMenuItem(EEikCmdFileSaveAs);
			aMenuPane->DeleteMenuItem(EEikCmdFileRevert);
			aMenuPane->DeleteMenuItem(EWordCmdFileSaveAsTemplate);
			aMenuPane->DeleteMenuItem(EWordCmdFileDeleteTemplate);
			}
		else if (readOnly)
			{
			aMenuPane->SetItemDimmed(EEikCmdFileSave,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdFileRevert,TRUE);
			}
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EEikCmdFileImport,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdHtmlImport,ETrue);
			}
		break;
		}
	//
	//
	case R_WORD_EDIT_MENU:
		{// read-only dependancies
		if (readOnly || (!iRichEd->CanUndo()) )
			aMenuPane->SetItemDimmed(EEikCmdEditUndo,TRUE);
		if (readOnly || (!iRichEd->CanRedo()) )
			aMenuPane->SetItemDimmed(EEikCmdEditRedo,TRUE);
		TInt selection=iRichEd->Selection().Length();
		if (selection==0)
			aMenuPane->SetItemDimmed(EEikCmdEditCopy,TRUE);
		if (readOnly || selection==0)
			aMenuPane->SetItemDimmed(EEikCmdEditCut,TRUE);
		if (readOnly)
			aMenuPane->SetItemDimmed(EEikCmdEditPaste,TRUE);
		break;
		}
	//
	//
	case R_WORD_EDITFIND_MENU:
		{
		TInt documentLength=iRichEd->TextLength();
		if (documentLength==0)
			{
			aMenuPane->SetItemDimmed(EEikCmdSearchFind,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdSearchFindAgain,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdGoTo,TRUE);
			}
		if (documentLength==0 || readOnly)
			aMenuPane->SetItemDimmed(EEikCmdSearchReplace,TRUE);
		break;
		}
	//
	//
	case R_WORD_EDITOPTIONS_MENU:
		if (readOnly)
			aMenuPane->SetItemDimmed(EEikCmdEditPictureFormat,TRUE);
		break;
	//
	//
	case R_WORD_VIEW_MENU:
		{// No read-only dependant menu options here
		if (!WysiwygModeIsSet())
			aMenuPane->SetItemButtonState(EWordCmdWrapToScreen, EEikMenuItemSymbolOn);
		if (iEikonEnv->AppUiFactory()->ToolBand()->IsVisible())
			aMenuPane->SetItemButtonState(EWordCmdToolband, EEikMenuItemSymbolOn);
		if (iEikonEnv->AppUiFactory()->ToolBar()->IsVisible())
			aMenuPane->SetItemButtonState(EWordCmdToolbar, EEikMenuItemSymbolOn);
		break;
		}
	//
	//
	case R_WORD_INSERT_MENU:
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EWordCmdInsertSketch,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdInsertSheet,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdInsertObject,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdInsertPageBreak,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdInsertLineBreak,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdInsertSymbol,TRUE);
			}
		break;
	//
	//
	case R_WORD_TEXT_MENU:
		{
		TCharFormat charFormat;
		TCharFormatMask charUndeterminedMask;
		((CRichText*)iRichEd->Text())->GetCharFormat(charFormat,charUndeterminedMask,iRichEd->CursorPos(),0);

		if (charFormat.iFontSpec.iFontStyle.StrokeWeight()==EStrokeWeightBold)
			aMenuPane->SetItemButtonState(EEikCmdFontBold,EEikMenuItemSymbolOn);
		if (charFormat.iFontSpec.iFontStyle.Posture()==EPostureItalic)
			aMenuPane->SetItemButtonState(EEikCmdFontItalic,EEikMenuItemSymbolOn);
		if (charFormat.iFontPresentation.iUnderline==EUnderlineOn)
			aMenuPane->SetItemButtonState(EEikCmdFontUnderline,EEikMenuItemSymbolOn);
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EEikCmdFontBold,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdFontItalic,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdFontUnderline,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdFontChange,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdRevertToStyleChar,TRUE);
			}
		break;
		}
	//
	//
	case R_WORD_PARAGRAPH_MENU:
		{
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EWordCmdRevertToStylePara,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdParaIndent,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdParaTabs,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdParaSpacing,TRUE);
			aMenuPane->SetItemDimmed(EEikCmdParaBorder,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdStyleGallery,TRUE);
			}
		break;
		}
	//
	//
	case R_WORD_PARA_ALIGNMENT_MENU:
		{
		CParaFormat* paraFormat=CParaFormat::NewLC();
		TParaFormatMask paraUndeterminedMask;
		const TCursorSelection selection=iRichEd->Selection();
		Text()->GetParaFormatL(paraFormat,paraUndeterminedMask,selection.LowerPos(),selection.Length());
		if (!paraUndeterminedMask.AttribIsSet(EAttAlignment))
			{
			switch (paraFormat->iHorizontalAlignment)
				{
			case CParaFormat::ELeftAlign:
				aMenuPane->SetItemButtonState(EWordCmdAlignLeft,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(0);
				break;
			case CParaFormat::ERightAlign:
				aMenuPane->SetItemButtonState(EWordCmdAlignRight,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(1);
				break;
			case CParaFormat::ECenterAlign:	
				aMenuPane->SetItemButtonState(EWordCmdAlignCenter,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(2);
				break;
			case CParaFormat::EJustifiedAlign:
				aMenuPane->SetItemButtonState(EWordCmdAlignJustify,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(3);
				break;
			default:
				break;
				}
			}
		CleanupStack::PopAndDestroy();
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EWordCmdAlignLeft,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdAlignRight,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdAlignCenter,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdAlignJustify,TRUE);
			}
		break;
		}
	//
	//
	case R_WORD_PARA_BULLETS_MENU:
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EWordCmdInsertBullet,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdFormatBullet,TRUE);
			}
		break;
	//
	//
	case R_WORD_TOOLS_MENU:
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EWordCmdObjectPreferences,TRUE);
			}
		break;
	//
	//
	case R_EIK_SIDEBAR_IRDA_MENU:
	case R_WORD_TOOLS_INFRARED:
		aMenuPane->SetItemDimmed(EEikCmdIrdaReceive, TRUE);
		aMenuPane->SetItemDimmed(EEikCmdIrdaSend, TRUE);
		break;
	//
	//
	case R_EIK_SIDEBAR_EDIT_MENU:
		{
		TInt selection=iRichEd->Selection().Length();
		if (selection==0)
			aMenuPane->SetItemDimmed(EEikCmdEditCopy,TRUE);
		if (readOnly || selection==0)
			aMenuPane->SetItemDimmed(EEikCmdEditCut,TRUE);
		if (readOnly)
			aMenuPane->SetItemDimmed(EEikCmdEditPaste,TRUE);
		}
		break;

	case R_WORD_TEST_MENU:
		{
		TPtrC text;
		TCharFormat format;
		iRichEd->RichText()->GetChars(text,format,iRichEd->Selection().LowerPos());
		TFontPresentation::TFontHighlightStyle highlight = format.iFontPresentation.iHighlightStyle;
		if (highlight == TFontPresentation::EFontHighlightNormal)
			aMenuPane->SetItemButtonState(EWordCmdHighlight,EEikMenuItemSymbolOn);
		else if (highlight == TFontPresentation::EFontHighlightShadow)
			aMenuPane->SetItemButtonState(EWordCmdShadow,EEikMenuItemSymbolOn);
		else if (highlight == TFontPresentation::EFontHighlightFirstCustomStyle)
			aMenuPane->SetItemButtonState(EWordCmdCustomHighlight,EEikMenuItemSymbolOn);
		if (iTest->RuledPaper())
			aMenuPane->SetItemButtonState(EWordCmdRuledPaper,EEikMenuItemSymbolOn);
		if (iTest->BackgroundBitmap())
			aMenuPane->SetItemButtonState(EWordCmdBackgroundBitmap,EEikMenuItemSymbolOn);
		if (text.Length() && TChar(text[0]).IsUpper())
			aMenuPane->SetItemButtonState(EWordCmdUpperCase,EEikMenuItemSymbolOn);
		if (iTest->TruncateWithEllipsis())
			aMenuPane->SetItemButtonState(EWordCmdTruncateWithEllipsis,EEikMenuItemSymbolOn);
		}
		break;

	case R_WORD_PICTURE_ALIGNMENT_MENU:
		{
		TCharFormat format;
		TCursorSelection sel = iRichEd->Selection();
		TCharFormatMask varies;
		Text()->GetCharFormat(format,varies,sel.LowerPos(),sel.Length());
		if (!varies.AttribIsSet(EAttFontPictureAlignment))
			{
			switch (format.iFontPresentation.iPictureAlignment)
				{
				case TFontPresentation::EAlignTop:
					aMenuPane->SetItemButtonState(EWordCmdPictureAlignTop,EEikMenuItemSymbolOn);
					aMenuPane->SetSelectedItem(0);
					break;
				case TFontPresentation::EAlignBottom:
					aMenuPane->SetItemButtonState(EWordCmdPictureAlignBottom,EEikMenuItemSymbolOn);
					aMenuPane->SetSelectedItem(1);
					break;
				case TFontPresentation::EAlignCentered:	
					aMenuPane->SetItemButtonState(EWordCmdPictureAlignCenter,EEikMenuItemSymbolOn);
					aMenuPane->SetSelectedItem(2);
					break;
				case TFontPresentation::EAlignBaseLine:
					aMenuPane->SetItemButtonState(EWordCmdPictureAlignBaseline,EEikMenuItemSymbolOn);
					aMenuPane->SetSelectedItem(3);
					break;
				default:
					break;
				}
			}
		if (readOnly)
			{
			aMenuPane->SetItemDimmed(EWordCmdPictureAlignTop,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdPictureAlignBottom,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdPictureAlignCenter,TRUE);
			aMenuPane->SetItemDimmed(EWordCmdPictureAlignBaseline,TRUE);
			}
		break;
		}

	case R_WORD_KEYBOARD_MENU:
		switch (iTest->Keyboard())
			{
			case CWordTest::EStandardKeyboard:
				aMenuPane->SetItemButtonState(EWordCmdStandardKeyboard,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(0);
				break;
			case CWordTest::EGreekKeyboard:
				aMenuPane->SetItemButtonState(EWordCmdGreekKeyboard,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(1);
				break;
			case CWordTest::ERussianKeyboard:
				aMenuPane->SetItemButtonState(EWordCmdRussianKeyboard,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(2);
				break;
			case CWordTest::EArabicKeyboard:
				aMenuPane->SetItemButtonState(EWordCmdArabicKeyboard,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(3);
				break;
			}
		break;

	case R_WORD_CURSOR_STYLE_MENU:
		if (iTest->CursorFlashing())
			aMenuPane->SetItemButtonState(EWordCmdCursorStyleFlash,EEikMenuItemSymbolOn);
		if (iTest->CursorWeight() >= 3)
			aMenuPane->SetItemButtonState(EWordCmdCursorStyleHeavy,EEikMenuItemSymbolOn);
		if (iTest->CursorXorColor() == KRgbCyan)
			aMenuPane->SetItemButtonState(EWordCmdCursorStyleRed,EEikMenuItemSymbolOn);
		switch (iTest->CursorPlacement())
			{
			case ECursorVertical:
				aMenuPane->SetItemButtonState(EWordCmdCursorStyleVertical,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(1);
				break;
			case ECursorUnderlineNext:
				aMenuPane->SetItemButtonState(EWordCmdCursorStyleUnderlineNext,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(2);
				break;
			case ECursorUnderlinePrev:
				aMenuPane->SetItemButtonState(EWordCmdCursorStyleUnderlinePrev,EEikMenuItemSymbolOn);
				aMenuPane->SetSelectedItem(3);
				break;
			}
		break;
	case R_WORD_SELECTION_HIGHLIGHT_MENU:
		aMenuPane->SetItemButtonState(
			aMenuPane->MenuItemCommandId(iTest->SelectionHighlightStyle()),
			EEikMenuItemSymbolOn);
		break;

	case R_WORD_POSITIONING_HINT_TEST_MENU:
		switch(iTest->CursorPositioningHint())
			{
		case TCursorPosition::EPosHintUndefined:
			aMenuPane->SetItemButtonState(
				EWordCmdPositioningHintNeutral,
				EEikMenuItemSymbolOn);
			aMenuPane->SetSelectedItem(1);
			break;
		case TCursorPosition::EInsertStrongL2R:
			aMenuPane->SetItemButtonState(
				EWordCmdPositioningHintInsertLeftToRight,
				EEikMenuItemSymbolOn);
			aMenuPane->SetSelectedItem(2);
			break;
		case TCursorPosition::EInsertStrongR2L:
			aMenuPane->SetItemButtonState(
				EWordCmdPositioningHintInsertRightToLeft,
				EEikMenuItemSymbolOn);
			aMenuPane->SetSelectedItem(3);
			break;
			}
		break;
	default:
		break;
		}
	}
	
void CWordAppUi::HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
	{
	if (iWordFlags&EFNameBoxPopped && (aEventType==EEventRequestCancel ||aEventType==EEventRequestExit))
		{
		if (aEventType==EEventRequestExit)
			{
			TCharFormatMask charFormatMask;
			TCharFormat charFormat;
			const TCursorSelection selection=iRichEd->Selection();
			Text()->GetCharFormat(charFormat,charFormatMask,selection.LowerPos(),selection.Length());
			TInt index=iPopoutListBox->CurrentItemIndex();
			charFormat.iFontSpec.iTypeface.iName=iPopoutListBox->Model()->ItemText(index);

			charFormat.iFontSpec.iTypeface.SetAttributes(FontUtils::TypefaceAttributes(*iCoeEnv->ScreenDevice(),charFormat.iFontSpec.iTypeface.iName));

			iTwipsList=new(ELeave) CArrayFixFlat<TInt>(4);
			FontUtils::GetAvailableHeightsInTwipsL(*iCoeEnv->ScreenDevice(),charFormat.iFontSpec.iTypeface.iName,*iTwipsList);
			index=FontUtils::IndexOfNearestHeight(*iTwipsList,charFormat.iFontSpec.iHeight);
			charFormat.iFontSpec.iHeight=(*iTwipsList)[index];
			charFormatMask.ClearAll();
			charFormatMask.SetAttrib(EAttFontTypeface);	 
			iRichEd->ApplyCharFormatL(charFormat,charFormatMask);
			//
			// Sense format of lower pos of selection to display appropriate info on toolband button labels
			Text()->GetCharFormat(charFormat,charFormatMask,selection.LowerPos(),selection.Length());
			UpdateToolBandFontNameButtonL(charFormat.iFontSpec.iTypeface.iName);
			UpdateToolBandFontHeightButtonL(charFormat.iFontSpec.iHeight);
			SetPaginationOutOfDate(TRUE);
			}
		iWordFlags&=~EFNameBoxPopped;
		DestroyPopoutLBox();
		return;
		}
	else if (iWordFlags&EStyleBoxPopped && (aEventType==EEventRequestCancel ||aEventType==EEventRequestExit))
		{
		if (aEventType==EEventRequestExit)
			{
			const TCursorSelection selection=iRichEd->Selection();
			TInt index=iPopoutListBox->CurrentItemIndex();
			TParagraphStyleName name;
			iCoeEnv->ReadResource(name, R_WORD_NORMAL);
			CDesCArray* array=(CDesCArray*)(iPopoutListBox->Model()->ItemTextArray());
			if (name==(*array)[index])
				Text()->ApplyParagraphStyleL(*(CParagraphStyle*)Text()->GlobalParaFormatLayer(),selection.LowerPos(),selection.Length(),KApplyParaStyleMode);
			else
				{
				const CParagraphStyle& paraStyle=*(Text()->StyleList()->PtrByName((*array)[index])->iStyle);
		   		Text()->ApplyParagraphStyleL(paraStyle,selection.LowerPos(),selection.Length(),KApplyParaStyleMode);
				}
			ReformatDueToStateChangeL();
			}
		iWordFlags&=~EStyleBoxPopped;
		DestroyPopoutLBox();
		return;
		}
	else if (iWordFlags&EAlignBoxPopped && (aEventType==EEventRequestCancel ||aEventType==EEventRequestExit))
		{
		if (aEventType==EEventRequestExit)
			{
			const TInt index = iBitmapPopout->CurrentItemIndex();
			SetParaAlignmentL(index);
			ReformatDueToStateChangeL();
			}
		iWordFlags&=~EAlignBoxPopped;
		DestroyPopoutLBox();
		return;
		}
	else if (iWordFlags&EFHeightBoxPopped && (aEventType==EEventRequestCancel ||aEventType==EEventRequestExit))
		{
		if (aEventType==EEventRequestExit)
			{
			TCharFormatMask charFormatMask;
			TCharFormat charFormat;
			TInt index=iPopoutListBox->CurrentItemIndex();
			charFormatMask.SetAttrib(EAttFontHeight);	 
			charFormat.iFontSpec.iHeight=(*iTwipsList)[index];
			iRichEd->ApplyCharFormatL(charFormat,charFormatMask);
			UpdateToolBandFontHeightButtonL(charFormat.iFontSpec.iHeight);
			}
		iWordFlags&=~EFHeightBoxPopped;
		DestroyPopoutLBox();
		return;
		}
	else if (iWordFlags&EBordersPopped && (aEventType==EEventRequestCancel ||aEventType==EEventRequestExit))
		{
		if (aEventType==EEventRequestExit)
			{
			TParaBorder top;
			TParaBorder bottom;
			TParaBorder left;
			TParaBorder right;
			const TInt index=iMcBitmapPopout->CurrentItemIndex();
			switch (index)
				{
			case (ESingleFull-ESingleFull):
				top.iThickness=KThinBorderThicknessInTwips;
				bottom.iThickness=KThinBorderThicknessInTwips;
				left.iThickness=KThinBorderThicknessInTwips;
				right.iThickness=KThinBorderThicknessInTwips;
				break;
			case (ESingleTop-ESingleFull):
				top.iThickness=KThinBorderThicknessInTwips;
				break;
			case (ESingleBottom-ESingleFull):
				bottom.iThickness=KThinBorderThicknessInTwips;
				break;
			case (ESingleTopAndBottom-ESingleFull):
				top.iThickness=KThinBorderThicknessInTwips;
				bottom.iThickness=KThinBorderThicknessInTwips;
				break;
			case (EDoubleFull-ESingleFull):
				top.iThickness=KThickBorderThicknessInTwips;
				bottom.iThickness=KThickBorderThicknessInTwips;
				left.iThickness=KThickBorderThicknessInTwips;
				right.iThickness=KThickBorderThicknessInTwips;
				break;
			case (EDoubleTop-ESingleFull):
				top.iThickness=KThickBorderThicknessInTwips;
				break;
			case (EDoubleBottom-ESingleFull):
				bottom.iThickness=KThickBorderThicknessInTwips;
				break;
				}
			switch (index)
				{
			case (ESingleFull-ESingleFull):
			case (EDoubleFull-ESingleFull):
				top.iLineStyle=TParaBorder::ESolid;
				bottom.iLineStyle=TParaBorder::ESolid;
				left.iLineStyle=TParaBorder::ESolid;
				right.iLineStyle=TParaBorder::ESolid;
				break;
			case (ESingleTop-ESingleFull):
			case (EDoubleTop-ESingleFull):
				top.iLineStyle=TParaBorder::ESolid;
				break;
			case (ESingleBottom-ESingleFull):
			case (EDoubleBottom-ESingleFull):
				bottom.iLineStyle=TParaBorder::ESolid;
				break;
			case (ESingleTopAndBottom-ESingleFull):
				top.iLineStyle=TParaBorder::ESolid;
				bottom.iLineStyle=TParaBorder::ESolid;
				break;
				}
			TParaFormatMask mask;
			mask.SetAttrib(EAttTopBorder);
			mask.SetAttrib(EAttBottomBorder);
			mask.SetAttrib(EAttLeftBorder);
			mask.SetAttrib(EAttRightBorder);
			CParaFormat* paraFormat=CParaFormat::NewLC();
			paraFormat->SetParaBorderL(CParaFormat::EParaBorderTop,top);
			paraFormat->SetParaBorderL(CParaFormat::EParaBorderBottom,bottom);
			paraFormat->SetParaBorderL(CParaFormat::EParaBorderLeft,left);
			paraFormat->SetParaBorderL(CParaFormat::EParaBorderRight,right);
			iRichEd->ApplyParaFormatL(paraFormat,mask);
			CleanupStack::PopAndDestroy(); // paraFormat
			}
		iWordFlags&=~EBordersPopped;
		DestroyPopoutLBox();
		return;
		}
    if (aEventType==EEventStateChanged)
		{
		SetPaginationOutOfDate(TRUE);
		if (aControl==iRichEd)
			iToolBarUpdate->Start(CWordToolBarUpdate::ECharFormatUpdate);
		}
	if (iPaginateDialogCalled && aEventType==EEventRequestExit)
		{ // !! possibly better - the dialog should define its own observer class
		delete aControl;
		SetPaginationOutOfDate(FALSE);
		iPaginateDialogCalled=FALSE;
		return;
		}  
	}


void CWordAppUi::HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType)
	{
	if (aEdwin==iRichEd && (aEventType==MEikEdwinObserver::EEventFormatChanged || 
							aEventType==MEikEdwinObserver::EEventNavigation))
		{
		if (aEventType==MEikEdwinObserver::EEventFormatChanged)
			{
			SetPaginationOutOfDate(TRUE);
			SetDocChanged();
			}
		if (iEikonEnv->AppUiFactory()->ToolBar()->IsVisible() || iEikonEnv->AppUiFactory()->ToolBand()->IsVisible())
			iToolBarUpdate->Start(CWordToolBarUpdate::EFullUpdate);
		}
	}

void CWordAppUi::RunPageSetupDialogL()
	{
	if (CEikPageSetupDialog::RunDlgLD(iPrintSetup))
		{
		CTextPageRegionPrinter& printer=PageRegionPrinter();
		printer.SetPageMarginsInTwips(iPrintSetup->iPageMarginsInTwips.iMargins);
		printer.SetPageSpecInTwips(iPrintSetup->PrinterDevice()->CurrentPageSpecInTwips());
		SetPaginationOutOfDate(TRUE);
		SetDocChanged();
		UpdatePageLayoutL();
		iRichEd->DrawNow();  // need to display any page breaks immediatley.
		}
	}
