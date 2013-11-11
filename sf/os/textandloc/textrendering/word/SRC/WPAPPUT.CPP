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


#include <s32std.h>
#include <txtrich.h>
#include <frmprint.h>
#include <frmvis.h>
#include <prnsetup.h>
#include <apgdoor.h>
#include <coeutils.h>
#include <barsread.h>

#include <techview/eikchlst.h>
#include <eikembal.h>
#include <eikenv.h>
#include <eikproc.h>
#include <techview/eiktbar.h>
#include <techview/eikprtpv.h>
#include <techview/eikprtdg.h>
#include <techview/eikpword.h>
#include <techview/eikbtgpc.h>
#include <techview/eikon.rsg>
#include <eikon.mbg>

#include "WPAPPUI.H"
#include "WPDIALGS.H"
#include "WPDOC.H"
#include "WPTEST.H"
#include <word.rsg>
#include "WORD.HRH"

const TInt KZoomStateOne=660;
const TInt KZoomStateTwo=800;
const TInt KZoomStateThree=TZoomFactor::EZoomOneToOne;
const TInt KZoomStateFour=1500;
const TInt KDefaultZoomState=KZoomStateTwo;

const TInt KFirstPageNum=0;

const TInt KPrintPreviewBandsPerPage=6;

TBool CWordAppUi::ProcessCommandParametersL(TApaCommand aCommand ,TFileName& aDocumentName, const TDesC8& /*aTail*/)
//
	{return CEikAppUi::ProcessCommandParametersL(aCommand,aDocumentName);}


void CWordAppUi::CreateEdwinL()
	{
	CGraphicsDevice* device=iCoeEnv->ScreenDevice();
	iAppZoom.SetGraphicsDeviceMap(device);
	iAppZoom.SetZoomFactor(KDefaultZoomState);
	iRichEd=new(ELeave) CWordTextEditor(TGulBorder::ESingleBlack);
	const TInt flags=EEikEdwinOwnsWindow|EEikEdwinKeepDocument|EEikEdwinInclusiveSizeFixed|EEikEdwinUserSuppliedText|
						EEikEdwinNoAutoSelection|EEikEdwinAlwaysShowSelection;
	iRichEd->ConstructL(NULL,0,0,flags);
	iRichEd->SetObserver(this);
	iRichEd->SetEdwinObserver(this);
	iRichEd->CreateScrollBarFrameL();
	iRichEd->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	iRichEd->SetAdjacent(EGulAdjTop|EGulAdjRight);
	iRichEd->SetZoomFactorL(&iAppZoom);
	//
	SetEdwinL();
	// 
	iTest = CWordTest::NewL(iRichEd->TextView());	
	iTest->TurnOnCustomDrawing(iRichEd->TextLayout());
	iRichEd->ActivateL();
//	iRichEd->SetFocus(ETrue);
	AddToStackL(iRichEd);
	iRichEd->TextView()->SetParagraphFillTextOnly(ETrue);  // call when text view has been initalised
	SetReadOnly(iEmbeddedAndReadOnly);
	}


void CWordAppUi::HandleModelChangeL()
	{
	iModel=Document()->Model();
	SetUiData();
	ResetPrint();
	ResetEdwinL();  // may use print data if in wysiwyg mode.
	UpdateToolbarsL();
	SetPaginationOutOfDate(ETrue);
	iEikonEnv->BusyMsgCancel();
	if (iRichEd->IsReadOnly())
		iEikonEnv->InfoMsg(R_WORD_FILE_READ_ONLY);
	iRichEd->TextView()->SetCursorPlacement(ECursorVertical);
	}


void CWordAppUi::ReformatDueToStateChangeL()
// Called from numerous places where content has
// changed, requiring a reformat of the current document.
//
	{
	iRichEd->NotifyNewFormatL();
	UpdateToolbandButtonsL();
	SetPaginationOutOfDate(ETrue);
	}


void CWordAppUi::UpdateToolbarsL()
	{
	UpdateToolbandButtonsL();
	UpdateFileNameLabelL();
	ResetFlags();
	}

void CWordAppUi::ResetEdwinL()
	{
	iRichEd->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOff);
	if (WysiwygModeIsSet())
		iRichEd->SetWysiwygModeOn(PageLayoutWidth(),iModel->PrintSetup()->PrinterDevice());
	else
		iRichEd->SetWysiwygModeOff();
	SetEdwinL();
	TInt adjacent=EGulAdjNone;
	if (iEikonEnv->AppUiFactory()->ToolBar()->IsVisible())
		adjacent |= EGulAdjRight;
	if (iEikonEnv->AppUiFactory()->ToolBand()->IsVisible())
		adjacent |= EGulAdjTop;
	iRichEd->SetAdjacent(adjacent);
	TViewYPosQualifier yPosQualifier;
	yPosQualifier.SetMakeLineFullyVisible();
	yPosQualifier.SetFillScreen();
	yPosQualifier.SetHotSpot(TViewYPosQualifier::EFViewBottomOfLine);
	TInt docPos=iRichEd->TextView()->Selection().iCursorPos;
	TInt yPos=iRichEd->TextView()->ViewRect().iBr.iY;
	iRichEd->TextView()->SetViewL(docPos,yPos,yPosQualifier,CTextView::EFViewDiscardAllNoRedraw);  // !! casues format
	iRichEd->SetRect(ClientRect());  // !! causes format but no draw
	if (WysiwygModeIsSet())
		iRichEd->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto,CEikScrollBarFrame::EAuto);
	else
		iRichEd->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	//
	// for use when running as a main document.
	// Read-only status is determined by looking at the AppFileMode.
	if (iContainerAppUi)
		SetReadOnly(iEmbeddedAndReadOnly);
	else
		SetReadOnly((TBool)!(iDocument->AppFileMode()&EFileWrite));
	//
	iRichEd->ForceScrollBarUpdateL();
	iRichEd->NotifyNewFormatL(); // re-flow layout
	iRichEd->DrawNow();  // !! causes draw
	UpdateToolbandButtonsL(EVisibilityDependant);
	UpdateFileNameLabelL();
	iCoeEnv->WsSession().Flush();
	}

void CWordAppUi::SetEdwinL()
	{
	iModel=Document()->Model();
	iRichEd->SetDocumentContentL(*Text(),CEikEdwin::EUseText);
	iRichEd->SetRect(ClientRect());	
	iRichEd->InitialiseUnifiedEditorL();
	CFbsBitmap* lineCursor=iEikonEnv->CreateBitmapL(TPtrC(),EMbmEikonLncusr1);
	iRichEd->SetLineCursorBitmapL(lineCursor);
	}

void CWordAppUi::ResetPrint()
	{
	iPrintSetup=iModel->PrintSetup();
	CTextPageRegionPrinter& printer=PageRegionPrinter();
	printer.SetFirstPageOfDoc(KFirstPageNum);
	printer.SetDocument(Text());
	printer.SetPrinterDevice(iPrintSetup->PrinterDevice());
	printer.SetPageList(iModel->PageTable());	 
	printer.SetPageMarginsInTwips(iPrintSetup->iPageMarginsInTwips.iMargins);	
	printer.SetPageSpecInTwips(iPrintSetup->PrinterDevice()->CurrentPageSpecInTwips());	
	printer.SetParagraphFillTextOnly(ETrue);
	}

void CWordAppUi::ResetFlags()
	{
	iAlignment = RTmParFormat::EAlignNormal;
	iWordFlags &= EWysiwygModeOn;
	}

void CWordAppUi::RunGoToDialogL()
	{
	TInt pageNo=1;	  // etext should provide a function that gives current page number.
	TInt aMaxPages=iModel->PageTable()->Count();
	CEikDialog* dialog=new(ELeave) CWordGoToDialog(aMaxPages,&pageNo);
	if (!dialog->ExecuteLD(R_WORD_DIALOG_GOTO))
        return;
	TInt total=0;
	CArrayFix<TInt>* pageTable=iModel->PageTable();
	for (TInt ii=0;ii<pageNo-1;ii++)
		total+=(*pageTable)[ii];
    iRichEd->SetCursorPosL(total,EFalse);
	}

void CWordAppUi::SetBulletL()
	{
	TParaFormatMask mask;
	CParaFormat* paraFormat = CParaFormat::NewLC();
	const TCursorSelection cursorSelection = iRichEd->TextView()->Selection();
	const TInt lower = cursorSelection.LowerPos();
	const TInt selectionLength = cursorSelection.Length();
	CRichText* text = Text();
	text->GetParaFormatL(paraFormat, mask, lower, selectionLength);
	delete paraFormat->iBullet;
	mask.ClearAll();
	mask.SetAttrib(EAttBullet);
	mask.SetAttrib(EAttIndent);
	if (((CEikButtonBase*)iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonBullets))->State() == CEikButtonBase::EClear)
		{
		paraFormat->iBullet = NULL;
		paraFormat->iIndentInTwips = 0;
		}
	else
		{
		TBullet* bullet = new(ELeave) TBullet;
		if (!iBullet)
			CreateDefaultBulletL();
		*bullet = *iBullet;
		paraFormat->iBullet = bullet;
		if (iBullet->iHangingIndent)
			paraFormat->iIndentInTwips = 283; // about 1/2cm - completely arbitrary value !!
		else
			paraFormat->iIndentInTwips = 0;
		}
	iRichEd->ApplyParaFormatL(paraFormat, mask);
	CleanupStack::PopAndDestroy();
	}

void CWordAppUi::RunSelectBulletDialogL()
	{
	if (!iBullet)
		CreateDefaultBulletL();
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TParaFormatMask paraUndeterminedMask;
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	const TCursorSelection selection=iRichEd->TextView()->Selection();
	Text()->GetParaFormatL(paraFormat,paraUndeterminedMask,selection.LowerPos(),selection.Length());
	Text()->GetCharFormat(charFormat,charFormatMask,selection.LowerPos(),selection.Length());
	TRgb backColor=paraFormat->iFillColor;
	if (paraUndeterminedMask.AttribIsSet(EAttFillColor))
		backColor=KRgbWhite; // what else could be done ???
	if (paraFormat->iBullet)
		*iBullet=*paraFormat->iBullet;
	CEikDialog* dialog=new(ELeave) CWordBulletDialog(iBullet,backColor,NULL,charFormat.iFontSpec.iTypeface.iName);
	if (dialog->ExecuteLD(R_WORD_DIALOG_BULLET))
		{
		CEikButtonBase* but = (CEikButtonBase*)iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonBullets);
		but->SetState(CEikButtonBase::ESet);
		SetBulletL();
		but->DrawNow();
		}
	CleanupStack::PopAndDestroy();
	}

void CWordAppUi::CreateDefaultBulletL()
	{
	iBullet=new(ELeave) TBullet;
	iBullet->iCharacterCode=0x2022;
	iBullet->iHeightInTwips=iEikonEnv->NormalFont()->FontSpecInTwips().iHeight;
	iBullet->iTypeface.iName=_L("Swiss");
	}

void CWordAppUi::SetParaAlignmentL(TInt aIndex)
	{
	RTmParFormatLayer format;
	format.iMask.iFlags = 0;
	format.iMask.Set(TTmParFormatMask::EAlignment);
	switch(aIndex)
		{
		case 0:
		case EWordCmdAlignLeft:
			format.iFormat.iAlignment = RTmParFormat::EAlignNormal;
			break;
		case 1:
		case EWordCmdAlignCenter:	
			format.iFormat.iAlignment = RTmParFormat::EAlignCenter;
			break;
		case 2:
		case EWordCmdAlignRight:
			format.iFormat.iAlignment = RTmParFormat::EAlignReverse;
			break;
		case 3:
		case EWordCmdAlignJustify:
			format.iFormat.iAlignment = RTmParFormat::EAlignJustify;
			break;
		default:
			format.iFormat.iAlignment = RTmParFormat::EAlignNormal;
		}
	TCursorSelection sel = iRichEd->Selection();
	iRichEd->SetParFormatL(sel.LowerPos(),sel.Length(),format);
	UpdateAlignButtonL(format.iFormat.iAlignment);
	}


void CWordAppUi::CmdChangeAlignmentL()
	{
	CParaFormat format;
	TParaFormatMask undeterminedMask;
	const TCursorSelection selection=iRichEd->Selection();
	Text()->GetParaFormatL(&format,undeterminedMask,selection.LowerPos(),selection.Length());
	//
	TInt index=format.iHorizontalAlignment+1;  // toggle the alignment
	//
	if (undeterminedMask.AttribIsSet(EAttAlignment))
		index=4;  // ...to force default case
	//
	if (index>=4)
		index=0;  // where alignment > justifed, make it left aligned.
	SetParaAlignmentL(index);
	}


void CWordAppUi::ZoomL(TInt aZoomCmd)
	{
	TInt zoomFactor=iAppZoom.ZoomFactor();
	
	if (zoomFactor>KZoomStateFour)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateOne : KZoomStateFour;
	
	else if (zoomFactor==KZoomStateFour)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateOne : KZoomStateThree;
	
	else if (zoomFactor>KZoomStateThree)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateFour : KZoomStateThree;
	
	else if (zoomFactor==KZoomStateThree)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateFour : KZoomStateTwo;

	else if (zoomFactor>KZoomStateTwo)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateThree : KZoomStateTwo;

	else if (zoomFactor==KZoomStateTwo)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateThree : KZoomStateOne;
	
	else if (zoomFactor>KZoomStateOne)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateTwo : KZoomStateOne;
	
	else if (zoomFactor==KZoomStateOne)
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateTwo : KZoomStateFour;
	
	else
		zoomFactor=(aZoomCmd==EEikCmdZoomIn) ? KZoomStateOne : KZoomStateFour;
	//
	iAppZoom.SetZoomFactor(zoomFactor);
	TInt bitmapId=EMbmEikonLncusr1;
	switch (zoomFactor)
		{
	case KZoomStateOne:
	case KZoomStateTwo:
		break;
	case KZoomStateThree:
		bitmapId=EMbmEikonLncusr3;
		break;
	case KZoomStateFour:
		bitmapId=EMbmEikonLncusr4;
		break;
	default:
		;
		}
	CFbsBitmap* lineCursor=iEikonEnv->CreateBitmapL(TPtrC(),bitmapId);
	iRichEd->SetLineCursorBitmapL(lineCursor);
	iRichEd->NotifyNewFormatL();
	}

void CWordAppUi::InsertCharacterL(TChar aCharacter)
	{
	iRichEd->InsertCharacterL(aCharacter);
	UpdateToolbandButtonsL();
	SetPaginationOutOfDate(ETrue);
	//
	if (aCharacter==CEditableText::EPageBreak)
		RunPaginationDialogL();
	}

void CWordAppUi::CmdWrapToScreenL()
// Toggle the state of the screen mapping.
// Swaps between wrap to screen and wrap to page width
//
	{
	TBool wysiwygModeNowOn=ToggleWysiwygMode();
	CEikScrollBarFrame::TScrollBarVisibility horizVisibility=(wysiwygModeNowOn)
		? CEikScrollBarFrame::EAuto
		: CEikScrollBarFrame::EOff;
	iRichEd->ScrollBarFrame()->SetScrollBarVisibilityL(horizVisibility,CEikScrollBarFrame::EAuto);
	//
	if (wysiwygModeNowOn)
		UpdatePageLayoutL();  // wrap to the page width
	else
		{// wrap to screen width
		iRichEd->SetWysiwygModeOff();
		iRichEd->NotifyNewFormatL();
		}
	}


void CWordAppUi::UpdatePageLayoutL()
	{
	if (WysiwygModeIsSet())
		{
		iRichEd->SetWysiwygModeOn(PageLayoutWidth(),iModel->PrintSetup()->PrinterDevice());
		iRichEd->NotifyNewFormatL();
		}
	}

TInt CWordAppUi::PageLayoutWidth() const
	{
	const CPrintSetup* print=Document()->Model()->PrintSetup();
	const TPageSpec pageSpec=print->PrinterDevice()->CurrentPageSpecInTwips();
	TInt width=(pageSpec.iOrientation? pageSpec.iPortraitPageSize.iHeight : pageSpec.iPortraitPageSize.iWidth);
	const TMargins margins=print->iPageMarginsInTwips.iMargins;
	width-=(margins.iLeft+margins.iRight);
	return width;
	}

void CWordAppUi::RunViewPreferencesDialogL()
	{
	TNonPrintingCharVisibility visibility=iRichEd->TextLayout()->NonPrintingCharsVisibility();
	CWordViewPreferencesDialog* dialog=new(ELeave) CWordViewPreferencesDialog(visibility);
	if (dialog->ExecuteLD(R_WORD_DIALOG_VIEW_PREFERENCES))
		{
		visibility.SetPageBreaksVisible(ETrue);  // for now this is the default
		iRichEd->TextLayout()->SetNonPrintingCharsVisibility(visibility);
		iRichEd->NotifyNewFormatL();
		}
	}


void CWordAppUi::RunObjectPreferencesDialogL()
	{
	TBool sketchAsGlass=SketchAsGlass();
	TBool graphAsGlass=GraphAsGlass();
	CWordObjectPreferencesDialog* dialog=new(ELeave) CWordObjectPreferencesDialog(sketchAsGlass,graphAsGlass);
	if (dialog->ExecuteLD(R_WORD_DIALOG_OBJECT_PREFERENCES))
		{
		SetSketchAsGlass(sketchAsGlass);
		SetGraphAsGlass(graphAsGlass);
		}
	}

void CWordAppUi::RunOutlineDialogL()
	{
	TInt newCursorPosition = 0;

	CEikDialog* dialog = new(ELeave) CWordOutlineDialog(Document()->Model()->Text(), iRichEd, newCursorPosition);
	
	if (dialog->ExecuteLD(R_WORD_DIALOG_OUTLINE))
		{
		iRichEd->SetCursorPosL(newCursorPosition,EFalse);
		if (iEikonEnv->AppUiFactory()->ToolBar()->IsVisible() || iEikonEnv->AppUiFactory()->ToolBand()->IsVisible())
			iToolBarUpdate->Start(CWordToolBarUpdate::EFullUpdate);
		}
	 }


TFileName CWordAppUi::BitmapStore() const
	{
	return _L("*");
	}


TBool CWordAppUi::RunPaginationDialogL()
	{
	TPageTable* pageTable=iModel->PageTable();
	const TBool paginationCancelled=iRichEd->RunPaginateDialogL(iPrintSetup,pageTable);
	if (!paginationCancelled)
		{
		iPrintParameters.iFirstPage=0;
		iPrintParameters.iLastPage=pageTable->Count()-1;
		SetPaginationOutOfDate(EFalse);
		Text()->SetPageTable(pageTable);
		iRichEd->DrawNow();  // need to display any page breaks immediatley.
		}
	return paginationCancelled;
	}


void CWordAppUi::RunPreviewDialogL()
//
//
	{
	TBool paginationCancelled=EFalse;
	if (PaginationOutOfDate())
		paginationCancelled=RunPaginationDialogL();
	if (paginationCancelled)
		return;
	//
	TInt totalNumPages=iModel->PageTable()->Count();
	CTextPageRegionPrinter& printer=PageRegionPrinter();
	printer.SetPrintPreview(ETrue);
	if (CEikPrintPreviewDialog::RunDlgLD(*iPrintSetup, printer, totalNumPages, this, KPrintPreviewBandsPerPage))
		DoPrintL();
	}


TBool CWordAppUi::PageSetupChangedL(CPrintSetup* aPrintSetup,TInt& aNumPagesInDoc)
// Return ETrue if pagination is cancelled, otherwise return EFalse.
//
	{
	CTextPageRegionPrinter& printer=PageRegionPrinter();
	printer.SetPageMarginsInTwips(aPrintSetup->iPageMarginsInTwips.iMargins);
	printer.SetPageSpecInTwips(aPrintSetup->PrinterDevice()->CurrentPageSpecInTwips());
	SetPaginationOutOfDate(ETrue);
	SetDocChanged();
	if (RunPaginationDialogL())
		return ETrue;
	aNumPagesInDoc=iModel->PageTable()->Count();
	return EFalse;
	}


TBool CWordAppUi::RunPrintRangeDialogL(CPrintSetup* aPrintSetup,TInt& aNumPagesInDoc)
// Return ETrue if pagination is cancelled, otherwise return EFalse.
//
	{			 
	TUid oldUid = aPrintSetup->PrinterDevice()->Model().iUid;
	TUid uid = oldUid;
	CEikPrintRangeDialog* dialog = new(ELeave) CEikPrintRangeDialog(iPrintParameters, aPrintSetup, uid);
	//
	// fix for SW1-157.
	// an Eikon defect that cannot be fixed without breaking binary compatibility
	//
	dialog->iPageRange.iLowerLimit = 1;  // 
	dialog->iPageRange.iUpperLimit = iModel->PageTable()->Count();
	dialog->ExecuteLD(R_EIK_DIALOG_PRINT_RANGE_SETUP);
	if (oldUid != uid)	
		{
		aPrintSetup->CreatePrinterDeviceL(uid, iEikonEnv->FsSession());
		CTextPageRegionPrinter& printer = PageRegionPrinter();
		printer.SetPrinterDevice(aPrintSetup->PrinterDevice());
		UpdatePageLayoutL();
		SetDocChanged();
		SetPaginationOutOfDate(ETrue);
		if (RunPaginationDialogL())
			return ETrue;
		}
	aNumPagesInDoc = iModel->PageTable()->Count();
	return EFalse;  // no cancellation during pagination
	}

void CWordAppUi::RunPrintDialogL()
	{
	TUid oldUid = iPrintSetup->PrinterDevice()->Model().iUid; 	
	TUid uid = oldUid;
	CEikPrintRangeDialog* dialog = new(ELeave) CEikPrintRangeDialog(iPrintParameters, iPrintSetup, uid, ETrue);
	//
	// fix for SW1-157.
	// an Eikon defect that cannot be fixed without breaking binary compatibility
	//
	dialog->iPageRange.iLowerLimit = 1;  // 
	dialog->iPageRange.iUpperLimit = iModel->PageTable()->Count();;
	if (dialog->ExecuteLD(R_EIK_DIALOG_PRINT_RANGE_SETUP))
		{
		if (oldUid != uid)
			{
			iPrintSetup->CreatePrinterDeviceL(uid, iEikonEnv->FsSession());
			CTextPageRegionPrinter& printer = PageRegionPrinter();
			printer.SetPrinterDevice(iPrintSetup->PrinterDevice());
			UpdatePageLayoutL();
			SetDocChanged();
			SetPaginationOutOfDate(ETrue);
			}
		}
	else
		return;
	TBool cancel = EFalse;
	if (PaginationOutOfDate())
		cancel = RunPaginationDialogL();
	if (cancel)
		return;
	DoPrintL();
	}


void CWordAppUi::DoPrintL()
// Run the print progress dialog
//
	{
	CTextPageRegionPrinter& printer=PageRegionPrinter();
	printer.SetPrintPreview(EFalse);
	CEikPrintProgressDialog::RunDlgLD(iPrintSetup, &printer, iPrintParameters);
	iPrintSetup->FreeModelList();
	}


TBool CWordAppUi::ConfirmationDialogL(TConfirmationText aTitle,TConfirmationText aText)const
// Generate a query window with the specified text.
//
	{
	TResourceReader reader;
	return iEikonEnv->QueryWinL(reader.ReadTPtrC(aTitle,MUTABLE_CAST(HBufC8*,iConfirmationText)),
								reader.ReadTPtrC(aText,MUTABLE_CAST(HBufC8*,iConfirmationText)) );
	}


void CWordAppUi::GetUiData(TWordUiData& aData)const
//
	{
	aData.iNpcVisibility=0;
	if (!(iRichEd && iRichEd->TextLayout()))
		return;
	TNonPrintingCharVisibility visibility=iRichEd->TextLayout()->NonPrintingCharsVisibility();
	if (visibility.TabsVisible())
		aData.iNpcVisibility|=ENpcTabsVisible;
	if (visibility.SpacesVisible())
		aData.iNpcVisibility|=ENpcSpacesVisible;
	if (visibility.ParagraphDelimitersVisible())
		aData.iNpcVisibility|=NpcParagraphsVisible;
	if (visibility.LineBreaksVisible())
		aData.iNpcVisibility|=NpcForcedLineBreaksVisible;
	if (visibility.PotentialHyphensVisible())
		aData.iNpcVisibility|=NpcSoftHyphensVisible;
	if (visibility.NonBreakingHyphensVisible())
		aData.iNpcVisibility|=NpcHardHyphensVisible;
	if (visibility.NonBreakingSpacesVisible())
		aData.iNpcVisibility|=NpcHardSpacesVisible;
	//
	// sketch/graph default appearance
	aData.iPicturePrefs=0;
	aData.iPicturePrefs=iPicturePrefs;
	//
	// cursor pos
	aData.iCursorPos=iRichEd->CursorPos();
	//
	// toolbar visibilities
	aData.iToolbandVisibility = iEikonEnv->AppUiFactory()->ToolBand()->IsVisible();
	aData.iToolbarVisibility = iEikonEnv->AppUiFactory()->ToolBar()->IsVisible();
	//
	// zoom factor
	aData.iZoomFactor=iAppZoom.ZoomFactor();
	//
	// WYSIWYG flag
	aData.iWysiwygOn=WysiwygModeIsSet();
	}


void CWordAppUi::SetUiData()
	{
	const TWordUiData& data=Document()->UiData();
	//
	// Non-printing-char prefs
	TNonPrintingCharVisibility visibility;
	visibility.SetPageBreaksVisible(ETrue);
	visibility.SetTabsVisible(data.iNpcVisibility&ENpcTabsVisible);
	visibility.SetSpacesVisible(data.iNpcVisibility&ENpcSpacesVisible);
	visibility.SetParagraphDelimitersVisible(data.iNpcVisibility&NpcParagraphsVisible);
	visibility.SetLineBreaksVisible(data.iNpcVisibility&NpcForcedLineBreaksVisible);
	visibility.SetPotentialHyphensVisible(data.iNpcVisibility&NpcSoftHyphensVisible);
	visibility.SetNonBreakingHyphensVisible(data.iNpcVisibility&NpcHardHyphensVisible);
	visibility.SetNonBreakingSpacesVisible(data.iNpcVisibility&NpcHardSpacesVisible);
	iRichEd->TextLayout()->SetNonPrintingCharsVisibility(visibility);
	//
	// sketch/graph default appearance
	iPicturePrefs=data.iPicturePrefs;
	//
	// cursor pos
	iRichEd->TextView()->SetPendingSelection(TCursorSelection(data.iCursorPos,data.iCursorPos));
	//
	// toolbar visibilites
	iEikonEnv->AppUiFactory()->ToolBand()->MakeVisible(data.iToolbandVisibility);
	iEikonEnv->AppUiFactory()->ToolBar()->MakeVisible(data.iToolbarVisibility);
	//
	// zoom factor
	iAppZoom.SetZoomFactor(data.iZoomFactor);
	//
	// WYSIWYG flag
	if (data.iWysiwygOn)
		iWordFlags|=EWysiwygModeOn;
	else
   		iWordFlags&=~EWysiwygModeOn;
	}


TWordUiData::TWordUiData()
	:iNpcVisibility(0),
	iPicturePrefs(0),
	iToolbandVisibility(ETrue),
	iToolbarVisibility(ETrue),
	iCursorPos(0),
	iZoomFactor(KDefaultZoomState),
	iWysiwygOn(EFalse)
	{}


void TWordUiData::Reset()
	{
	iNpcVisibility=0;
	iPicturePrefs=0;
	iToolbandVisibility=ETrue;
	iToolbarVisibility=ETrue;
	iCursorPos=0;
	iZoomFactor=KDefaultZoomState;
	iWysiwygOn=EFalse;
	}


void TWordUiData::InternalizeL(RReadStream& aStream)
	{
	TCardinality version;
	version.InternalizeL(aStream);
	if (version!=KVersionAppConfigurationData)
		User::Leave(KErrCorrupt); // !! need better error

	iNpcVisibility=0;
	iNpcVisibility=aStream.ReadUint8L();
	iPicturePrefs=0;
	iPicturePrefs=aStream.ReadUint8L();
	iToolbandVisibility=(TBool)aStream.ReadInt8L();
	iToolbarVisibility=(TBool)aStream.ReadInt8L();
	iWysiwygOn=(TBool)aStream.ReadInt8L();

	iCursorPos=aStream.ReadInt32L();
	iZoomFactor=aStream.ReadInt32L();
	}


void TWordUiData::ExternalizeL(RWriteStream& aStream) const
	{
	const TCardinality version=KVersionAppConfigurationData;
	version.ExternalizeL(aStream);

	aStream.WriteUint8L((TUint8)iNpcVisibility);
	aStream.WriteUint8L((TUint8)iPicturePrefs);
	aStream.WriteInt8L(iToolbandVisibility);
	aStream.WriteInt8L(iToolbarVisibility);
	aStream.WriteInt8L(iWysiwygOn);

	aStream.WriteInt32L(iCursorPos);
	aStream.WriteInt32L(iZoomFactor);
	}


