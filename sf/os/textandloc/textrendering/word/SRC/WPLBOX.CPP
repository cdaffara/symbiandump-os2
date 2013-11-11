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


#include <txtrich.h>

#include <frmtlay.h>

#include <prnsetup.h>

#include <barsread.h>
#include <badesca.h>

#include <eikenv.h>
#include <techview/eikrted.h>
#include <techview/eiktbar.h>
#include <techview/eiktxlbm.h>
#include <techview/eikclbd.h>
#include <techview/eiklbi.h>
#include <techview/eikcmbut.h>
#include <eikon.mbg>
#include <gulicon.h>

#include "WPAPPUI.H"
#include <word.rsg>
#include "WORD.HRH"
#include <word.mbg>
#include "WPLBOX.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtfmlyr_internal.h"
#endif

const TInt KBmpListVertItemGap=8;
const TInt KMaxTotalBorderBitmaps=8;

CDesCArray* CWordAppUi::BuildFontNameListLC() const
    {
	CDesCArray* fontList=new(ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(fontList);
	FontUtils::GetAvailableFontsL(*iCoeEnv->ScreenDevice(),*fontList,EGulNoSymbolFonts);
	return fontList;
    }

CDesCArray* CWordAppUi::BuildStyleNameListLC() const
	{
	const CStyleList& styleData=*iModel->StyleList();
	TInt count=styleData.Count();
	CDesCArray* styleList=new(ELeave) CDesCArrayFlat(count+1);  // +1 for Normal (global layers)
	CleanupStack::PushL(styleList);
	TParagraphStyleName name;
	iCoeEnv->ReadResource(name,R_WORD_NORMAL);
	styleList->AppendL(name);
	for (TInt ii=0;ii<count;ii++)
		{
		name.Zero();
		name.Append(styleData.At(ii).iStyle->iName);
		styleList->AppendL(name);
		}
	styleList->Sort();
	return styleList;
	}

CDesCArray* CWordAppUi::BuildAlignListLC(CArrayPtrFlat<CGulIcon>& aIcons) const
    {
	CDesCArray* text = new(ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(text);
	for (TInt ii = (TInt)CParaFormat::ELeftAlign; ii <= (TInt)CParaFormat::EJustifiedAlign; ii++)
		{
		TBuf<8> buf;
		buf.Num(ii);
		buf.Append(TChar(KColumnListSeparator));
		text->AppendL(buf);
		CGulIcon* icon = STATIC_CAST(CGulIcon*, iEikonEnv->CreateIconL(TPtrC(), AlignmentBitmapId(ii)));
		aIcons.AppendL(icon);
		}
	return text;
	}


TInt CWordAppUi::AlignmentBitmapId(TInt aId)const
	{
	switch(aId)
		{
		case(CParaFormat::ELeftAlign):
			return EMbmEikonLefta;
		case(CParaFormat::ECenterAlign):
			return EMbmEikonCenta;
		case(CParaFormat::ERightAlign):
			return EMbmEikonRighta;
		case(CParaFormat::EJustifiedAlign):
			return EMbmEikonJusta;
		default:
			return EMbmEikonLefta;
		}
	}


TInt CWordAppUi::FindFontName(CDesCArray* aFontList) const
	{
	TCharFormat charFormat;
	TCharFormatMask charUndeterminedMask;
    const TCursorSelection selection=iRichEd->Selection();
	Text()->GetCharFormat(charFormat,charUndeterminedMask,selection.LowerPos(),selection.Length());
	//
	TInt index=0;
	if (aFontList->Find(charFormat.iFontSpec.iTypeface.iName,index))
        index=0; // !! reset if can't find - daft API to Find()
	return index;
	}

TInt CWordAppUi::FindStyleName(CDesCArray* aStyleList) const
	{
	TInt index=0;
	TParagraphStyleName name;
	iCoeEnv->ReadResource(name , R_WORD_NORMAL);
    const TCursorSelection selection=iRichEd->Selection();
	TBool styleChange;
	const CParaFormatLayer* style=Text()->ParagraphStyle(styleChange,selection.LowerPos(),selection.Length());
	TUid id=style->Type();
	if (id==KNormalParagraphStyleUid)
		aStyleList->Find(name,index);
	else
		aStyleList->Find(((CParagraphStyle*)style)->iName,index);
	return index;
	}

TInt CWordAppUi::NearestHeightIndex() const
	{
	TCharFormat charFormat;
	TCharFormatMask charUndeterminedMask;
	const TCursorSelection selection=iRichEd->Selection();
	Text()->GetCharFormat(charFormat,charUndeterminedMask,selection.LowerPos(),selection.Length());
	const TInt heightInTwips=charFormat.iFontSpec.iHeight;
	return FontUtils::IndexOfNearestHeight(*iTwipsList,heightInTwips);
	}

TInt CWordAppUi::FindAlignmentL() const
	{
	TInt index=0;
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TParaFormatMask paraUndeterminedMask;
	const TCursorSelection selection=iRichEd->Selection();
	Text()->GetParaFormatL(paraFormat,paraUndeterminedMask,selection.LowerPos(),selection.Length());
	if (!paraUndeterminedMask.AttribIsSet(EAttAlignment))
		{
		switch (paraFormat->iHorizontalAlignment)
			{
		case CParaFormat::ELeftAlign:
			index=0;
			break;
		case CParaFormat::ECenterAlign:	
			index=1;
			break;
		case CParaFormat::ERightAlign:
			index=2;
			break;
		case CParaFormat::EJustifiedAlign:
			index=3;
			break;
		default:
			break;
			}
		}
	CleanupStack::PopAndDestroy();
	return index;
	}

CDesCArray* CWordAppUi::UpdateHeightListLC()
	{
	TCharFormat charFormat;
	TCharFormatMask charUndeterminedMask;
    const TCursorSelection selection=iRichEd->Selection();
	Text()->GetCharFormat(charFormat,charUndeterminedMask,selection.LowerPos(),selection.Length());
	CDesCArray* pointsList=new(ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(pointsList);
	iTwipsList=new(ELeave) CArrayFixFlat<TInt>(4);
	TInt err=FontUtils::GetAvailableHeightsInTwipsAndPointsL(*iCoeEnv->ScreenDevice(),charFormat.iFontSpec.iTypeface.iName,*iTwipsList,*pointsList);
	if (err==KErrNotSupported)
		{// This is a foreign font, so preserve current height only.
		TBuf<4> num;
		num.Num(FontUtils::PointsFromTwips(charFormat.iFontSpec.iHeight));
		pointsList->AppendL(num);
		iTwipsList->AppendL(charFormat.iFontSpec.iHeight);
		}
	return pointsList;
	}

void CWordAppUi::LaunchStylesPopupL()
	{
	if (iWordFlags & EStyleBoxPopped)
		return;
	CDesCArray* styleList = BuildStyleNameListLC();		
	CEikCommandButtonBase* nameButton = STATIC_CAST(CEikCommandButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonStyle));
	const TInt index = FindStyleName(styleList);
	DoCreatePopoutL(styleList, index, nameButton, EToolBand, EFalse);
	iWordFlags |= EStyleBoxPopped;
	}

void CWordAppUi::LaunchFontsPopupL()
	{
	if (iWordFlags & EFNameBoxPopped)
		return;
	CDesCArray* fontList = BuildFontNameListLC();		
	CEikCommandButtonBase* nameButton = STATIC_CAST(CEikCommandButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonFont));
	const TInt index = FindFontName(fontList);
	DoCreatePopoutL(fontList, index, nameButton, EToolBand, ETrue);
	iWordFlags |= EFNameBoxPopped;
	}

void CWordAppUi::LaunchAlignPopupL()
	{
	if (iWordFlags & EAlignBoxPopped)
		return;
	CArrayPtrFlat<CGulIcon>* icons = new(ELeave) CArrayPtrFlat<CGulIcon>(4);
	CleanupStack::PushL(icons);
	CDesCArray* text = BuildAlignListLC(*icons);		
	CEikCommandButtonBase* nameButton = STATIC_CAST(CEikCommandButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonAlign));
	const TInt index = FindAlignmentL();
	DoCreateBitmapPopoutL(text, icons, index, nameButton, EToolBand);
	iWordFlags |= EAlignBoxPopped;
	}

void CWordAppUi::LaunchFontHeightsPopupL()
	{
	if (iWordFlags & EFHeightBoxPopped)
		return;
	CDesCArray* pointsList = UpdateHeightListLC();
	CEikCommandButtonBase* sizeButton = STATIC_CAST(CEikCommandButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonFontSize));
	const TInt index = NearestHeightIndex();
	DoCreatePopoutL(pointsList, index, sizeButton, EToolBand, ETrue);
	iWordFlags |= EFHeightBoxPopped;
	}

void CWordAppUi::LaunchBordersPopupL()
	{
	if (iWordFlags & EBordersPopped)
		return;
	CDesCArray* array = new(ELeave) CDesCArrayFlat(KMaxTotalBorderBitmaps);
	CleanupStack::PushL(array);
	CArrayPtrFlat<CGulIcon>* icons = new(ELeave) CArrayPtrFlat<CGulIcon>(KMaxTotalBorderBitmaps);
	CleanupStack::PushL(icons);
	for (TInt ii = (TInt)EMbmWordFullsbrd; ii <= (TInt)EMbmWordNullbrd; ii++)
		{
		TBuf<8> buf;
		buf.Num(ii - EMbmWordFullsbrd);
		buf.Append(TChar(KColumnListSeparator));
		array->AppendL(buf);
		CGulIcon* icon = NULL;
		icon = ((CGulIcon*)iEikonEnv->CreateIconL(BitmapStore(), ii));
		CleanupStack::PushL(icon);
		icons->AppendL(icon);
		}
	CEikCommandButtonBase* bordersButton = STATIC_CAST(CEikCommandButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonBorders));
	TPoint pos = bordersButton->PositionRelativeToScreen();
	TSize size((*icons)[0]->Bitmap()->SizeInPixels());
	const TInt width = size.iWidth + 6;
	size.iHeight += KBmpListVertItemGap;
	size.iHeight *= 4;
	size.iWidth = width << 1;
	pos.iY += bordersButton->Size().iHeight;  // draw popout below the borders button
	// determine current borders
	TParaFormatMask mask;
	CParaFormat* paraFormat = CParaFormat::NewLC();
	const TCursorSelection selection = iRichEd->TextView()->Selection();
	Text()->GetParaFormatL(paraFormat, mask, selection.LowerPos(), selection.Length());
	TInt index = 0;
	if (mask.AttribIsSet(EAttTopBorder) || mask.AttribIsSet(EAttBottomBorder) || mask.AttribIsSet(EAttLeftBorder) ||
				mask.AttribIsSet(EAttRightBorder))
		; // leave index as zero
	else if (!paraFormat->BordersPresent())
		index = ENullBorder-ESingleFull;
	else
		{
		TParaBorder left = paraFormat->ParaBorder(CParaFormat::EParaBorderLeft);
		TParaBorder right = paraFormat->ParaBorder(CParaFormat::EParaBorderRight);
		TParaBorder top = paraFormat->ParaBorder(CParaFormat::EParaBorderTop);
		TParaBorder bottom = paraFormat->ParaBorder(CParaFormat::EParaBorderBottom);
		if (top.iThickness == bottom.iThickness && left.iThickness == right.iThickness && top.iThickness == KThickBorderThicknessInTwips &&
				top.iLineStyle == bottom.iLineStyle && left.iLineStyle == right.iLineStyle &&
				top.iLineStyle == TParaBorder::ESolid)
			index = EDoubleFull - ESingleFull;
		else if (left.iThickness || right.iThickness)
			;
		else if (top.iThickness == bottom.iThickness && left.iThickness == right.iThickness && top.iThickness == KThinBorderThicknessInTwips &&
				top.iLineStyle == bottom.iLineStyle && left.iLineStyle == right.iLineStyle &&
				top.iLineStyle == TParaBorder::ESolid)
			index = ESingleTopAndBottom - ESingleFull;
		else if (top.iLineStyle == TParaBorder::ESolid && (top.iThickness == KThinBorderThicknessInTwips || top.iThickness == KThickBorderThicknessInTwips) &&
					bottom.iThickness == 0)
			{
			if (top.iThickness == KThinBorderThicknessInTwips)
				index = ESingleTop - ESingleFull;
			else
				index = EDoubleTop - ESingleFull;
			}
		else if (bottom.iLineStyle == TParaBorder::ESolid && (bottom.iThickness == KThinBorderThicknessInTwips || bottom.iThickness == KThickBorderThicknessInTwips) &&
					top.iThickness == 0)
			{
			if (bottom.iThickness == KThinBorderThicknessInTwips)
				index = ESingleBottom - ESingleFull;
			else
				index = EDoubleBottom - ESingleFull;
			}
		}
	CleanupStack::PopAndDestroy(); // paraFormat
	//
	DoCreateMcBitmapPopoutL(array, icons, index, bordersButton, width, TRect(pos, size));
	iWordFlags |= EBordersPopped;
	}

void CWordAppUi::DestroyPopoutLBox()
	{
	if (iPopoutListBox)
        {
        iEikonEnv->RemoveFromStack(iPopoutListBox);
        delete iPopoutListBox;
        iPopoutListBox=NULL;
		delete iTwipsList;
		iTwipsList=NULL;
        }
	else if (iBitmapPopout)
		{
		iEikonEnv->RemoveFromStack(iBitmapPopout);
		delete iBitmapPopout;
		iBitmapPopout=NULL;
		}
	else if (iMcBitmapPopout)
		{
		iEikonEnv->RemoveFromStack(iMcBitmapPopout);
		delete iMcBitmapPopout;
		iMcBitmapPopout=NULL;
		}
 	}

void CWordAppUi::DoCreatePopoutL(CDesCArray* aText,TInt aIndex,CEikCommandButtonBase* aLaunchingButton,TToolBarType aType,TBool aLimitWidth)
	{
	CWordTextListBox* popout = new(ELeave) CWordTextListBox;
	CleanupStack::PushL(popout);
  	popout->ConstructL(NULL, CEikListBox::EPopout | CEikListBox::ELeftDownInViewRect);
	popout->CreateScrollBarFrameL();
	popout->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    popout->SetObserver(this);
	iEikonEnv->AddWindowShadow(popout);
	popout->Model()->SetItemTextArray(aText);
	CleanupStack::Pop(); // aText
	const TSize screenSize = iEikonEnv->ScreenDevice()->SizeInPixels();
	TSize butSize = aLaunchingButton->Size();
	TInt width = Max(butSize.iWidth, popout->MaxWidth());
	if (aType == EToolBar)
		{
		if (width + butSize.iWidth > screenSize.iWidth)
			width = screenSize.iWidth - butSize.iWidth; // assume word is always full screen
		}
	else
		{
		if (width > screenSize.iWidth)
			width = screenSize.iWidth;
		if (aLimitWidth)
			{
			width = Max(butSize.iWidth, popout->MaxWidth());
			if (width > 2 * butSize.iWidth)
				width = 2 * butSize.iWidth;
			}
		}
	TPoint pos = aLaunchingButton->PositionRelativeToScreen();
	if (aType == EToolBand)
		pos.iY += butSize.iHeight;
	TRect listBoxRect(pos, TSize(width, 0));
	popout->CalculatePopoutRect(0, pos.iY, listBoxRect);
	if (aType == EToolBar)
		pos.iX -= listBoxRect.Width();
	else if (listBoxRect.iTl.iY < pos.iY)
		{
		const TInt itemHeight = popout->ItemHeight();
		while (listBoxRect.iTl.iY < pos.iY)
			listBoxRect.iTl.iY += itemHeight;
		listBoxRect.iTl.iY = pos.iY;
		}
	if (aType == EToolBand && listBoxRect.iBr.iX > screenSize.iWidth)
		listBoxRect.Move(listBoxRect.iBr.iX - screenSize.iWidth, 0);
	popout->SetExtent(TPoint(pos.iX, listBoxRect.iTl.iY), listBoxRect.Size());
	popout->SetCurrentItemIndex(aIndex);
    iEikonEnv->AddDialogLikeControlToStackL(popout);
	popout->SetLaunchingButton(aLaunchingButton);
	popout->ActivateL();
	CleanupStack::Pop(); // popout
	iPopoutListBox = popout;
	aLaunchingButton->SetIgnoreNextPointerUp();
	iPopoutListBox->ClaimPointerGrab();
	}

void CWordAppUi::DoCreateBitmapPopoutL(CDesCArray* aText, CArrayPtrFlat<CGulIcon>* aIcons, TInt aIndex,
									   CEikCommandButtonBase* aControl, TToolBarType aType)
	{
	CEikColumnListBox* popout = new(ELeave) CEikColumnListBox;
	CleanupStack::PushL(popout);
  	popout->ConstructL(NULL, CEikListBox::EPopout | CEikListBox::ELeftDownInViewRect | CEikListBox::ENoFirstLetterMatching);
	popout->CreateScrollBarFrameL();
	popout->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    popout->SetObserver(this);
	popout->SetItemHeightL((*aIcons)[0]->Bitmap()->SizeInPixels().iHeight + KBmpListVertItemGap);
	iEikonEnv->AddWindowShadow(popout);
	popout->Model()->SetItemTextArray(aText);
	CleanupStack::Pop(); // aText
	popout->ItemDrawer()->ColumnData()->SetIconArray(aIcons);
	CleanupStack::Pop(); // aIcons
	popout->ItemDrawer()->ColumnData()->SetColumnAlignmentL(0, CGraphicsContext::ECenter);
	popout->ItemDrawer()->ColumnData()->SetGraphicsColumnL(0, ETrue);
	TPoint pos = aControl->PositionRelativeToScreen();
	if (aType == EToolBand)
		pos.iY += aControl->Size().iHeight;
	TRect listBoxRect(pos, TSize(aControl->Size().iWidth, 0));
	popout->CalculatePopoutRect(0, pos.iY, listBoxRect);
	if (aType == EToolBar)
		pos.iX -= listBoxRect.Width();
	// assume list is not too wide or high to fit on screen
	const TSize screenSize = iCoeEnv->ScreenDevice()->SizeInPixels();
	const TSize listSize = listBoxRect.Size();
	if (listBoxRect.iTl.iX + listSize.iWidth > screenSize.iWidth)
		listBoxRect.iTl.iX = screenSize.iWidth - listSize.iWidth;
	popout->SetRect(listBoxRect);
	//	
	popout->ItemDrawer()->ColumnData()->SetColumnWidthPixelL(0, popout->View()->ViewRect().Width());
	popout->SetCurrentItemIndex(aIndex);
    iEikonEnv->AddDialogLikeControlToStackL(popout);
	popout->ActivateL();
	CleanupStack::Pop(); // popout
	iBitmapPopout = popout;
	iBitmapPopout->SetLaunchingButton(aControl);
	aControl->SetIgnoreNextPointerUp();
	iBitmapPopout->ClaimPointerGrab();
	}

void CWordAppUi::DoCreateMcBitmapPopoutL(CDesCArray* aText, CArrayPtrFlat<CGulIcon>* aIcons, TInt aIndex,
									   CEikCommandButtonBase* aLaunchingButton, TInt aColumnWidth, const TRect& aRect)
	{
	CWordSnakingColumnListBox* popout = new(ELeave) CWordSnakingColumnListBox;
	CleanupStack::PushL(popout);
	popout->ConstructL(NULL, CEikListBox::EPopout | CEikListBox::ELeftDownInViewRect | CEikListBox::ENoFirstLetterMatching);
	popout->CreateScrollBarFrameL();
	popout->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    popout->SetObserver(this);
	popout->SetItemHeightL((*aIcons)[0]->Bitmap()->SizeInPixels().iHeight + KBmpListVertItemGap);
	iEikonEnv->AddWindowShadow(popout);
	popout->Model()->SetItemTextArray(aText);
	CleanupStack::Pop(); // aText
	popout->ItemDrawer()->ColumnData()->SetColumnWidthPixelL(0, aColumnWidth);
	popout->ItemDrawer()->ColumnData()->SetIconArray(aIcons);
	CleanupStack::Pop(EMbmWordNullbrd - EMbmWordFullsbrd + 2); // aBitmaps
	popout->ItemDrawer()->ColumnData()->SetColumnAlignmentL(0, CGraphicsContext::ECenter);
	popout->ItemDrawer()->ColumnData()->SetGraphicsColumnL(0, ETrue);
	popout->SetColumnWidth(aColumnWidth);
	// assume list is not too wide or high to fit on screen
	TRect rect(aRect);
	const TSize size = rect.Size() + popout->BorderSize();
	rect.SetSize(size);
	const TSize screenSize = iCoeEnv->ScreenDevice()->SizeInPixels();
	if (rect.iTl.iX + size.iWidth > screenSize.iWidth)
		rect.iTl.iX = screenSize.iWidth - size.iWidth;
	popout->SetRect(rect);
	//	
	popout->SetCurrentItemIndex(aIndex);
    iEikonEnv->AddDialogLikeControlToStackL(popout);
	popout->ActivateL();
	CleanupStack::Pop(); // popout
	iMcBitmapPopout = popout;
	iMcBitmapPopout->SetLaunchingButton(aLaunchingButton);
	aLaunchingButton->SetIgnoreNextPointerUp();
	iMcBitmapPopout->ClaimPointerGrab();
	}

//
// class CWordTextListBox
//

TInt CWordTextListBox::MaxWidth()
	{
	iView->CalcDataWidth();
	TInt width=iView->DataWidth();
	width+=(HorizontalMargin()<<1)+iBorder.SizeDelta().iWidth;
	return width;
	}

//
// class CWordSnakingColumnListBox
//

void CWordSnakingColumnListBox::ConstructL(const CCoeControl* aParent,TInt aFlags)
	{
	CTextListBoxModel* model = new(ELeave) CTextListBoxModel;
	iModel = model;
	model->ConstructL();
	CColumnListBoxData* data = CColumnListBoxData::NewL();
	CleanupStack::PushL(data);
	iItemDrawer = new(ELeave) CColumnListBoxItemDrawer(model, iEikonEnv->NormalFont(), data);
	CleanupStack::Pop();	//data
	iItemDrawer->SetDrawMark(EFalse);
	CEikListBox::ConstructL(aParent, aFlags|ELeftDownInViewRect);
	}

CTextListBoxModel* CWordSnakingColumnListBox::Model() const
	{
	return(CTextListBoxModel*)iModel;
	}

CColumnListBoxItemDrawer* CWordSnakingColumnListBox::ItemDrawer() const
	{
	return(CColumnListBoxItemDrawer*)iItemDrawer;
	}

TSize CWordSnakingColumnListBox::BorderSize() const
	{
	TSize size(HorizontalMargin()<<1,VerticalMargin()<<1);
	size+=iBorder.SizeDelta();
	return size;
	}
