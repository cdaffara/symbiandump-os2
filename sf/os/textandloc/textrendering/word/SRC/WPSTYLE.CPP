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


#include <e32math.h>

#include <s32mem.h>

#include <txtrich.h>
#include <txtstyle.h>
#include <txtfrmat.h>

#include <badesca.h>
#include <barsread.h>
#include <prnsetup.h>
#include <clock.h>  // RMessageWindow::EMaxTextLength

#include <techview/eikmnbut.h>
#include <techview/eikbtpan.h>
#include <techview/eiklbbut.h>
#include <techview/eikdlgtb.h>
#include <techview/eiktxlbx.h>
#include <techview/eiktxlbm.h>
#include <eikenv.h>
#include <techview/eikrted.h>
#include <techview/eikchkbx.h>
#include <techview/eikbutb.h>
#include <techview/eikfontd.h>
#include <techview/eikparad.h>
#include <techview/eiktabdg.h>
#include <eikfctry.h>
#include <techview/eikon.rsg>
#include <techview/eikclb.h>
#include <techview/eikclbd.h>
#include <techview/eikbtgpc.h>

#include "WPSTYLE.H"
#include "WORD.HRH"
#include <word.rsg>
#include "WPAPPUI.H"
#include "WPDEF.H"
#include "WPDIALGS.H"
#include "WPPANIC.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtfmlyr_internal.h"
#endif

const TInt KNormalIndex=1;
const TInt KMaxStyleHotKeyDescription=20;
const TInt KDescriptionGranularity=100;
const TInt KStyleColumnSpacerWidth=5;
const TUint KNoHotkey=0;

enum TDescriptor
	{	EDesLeftMargin=EAttLeftMargin,
		EDesRightMargin,
		EDesIndent,
		EDesDummy0,
		EDesDummy1,
		EDesLineSpacing,
		EDesDummy2,
		EDesSpaceBefore,
		EDesSpaceAfter,
		EDesKeepTogether,
		EDesKeepWithNext,
		EDesStartNewPage,
		EDesWidowOrphan,
		EDesDummy3,
		EDesBorderMargin,
		EDesBorderTop,
		EDesBorderBottom,
		EDesBorderLeft,
		EDesBorderRight,
		EDesBullet,
		EDesDummy4,
		EDesTabStop,
		//
		EDesItalic,
		EDesBold,
		EDesSuperscript,
		EDesSubscript,
		EDesUnderline,
		EDesStrikethrough,
		EDesLeftAlign,
		EDesCenterAlign,
		EDesRightAlign,
		EDesJustifiedAlign
	};

				

TWordStyleGalleryData::TWordStyleGalleryData(CRichText& aText,CStyleList& aStyleList,CStyleShortCutList& aHotKeyList,
		TChar& aNormalStyleHotKey,CPrintSetup& aPrintSetup,TBool& aApplyStyle,const TCursorSelection& aSelection,
		MUnifiedEditor& aEditor):
	iText(aText),
	iStyleList(aStyleList),
	iHotKeyList(aHotKeyList),
	iNormalStyleHotKey(aNormalStyleHotKey),
	iPrintSetup(aPrintSetup),
	iApplyStyle(aApplyStyle),
	iSelection(aSelection),
	iEditor(aEditor)
	{
	}


CWordStyleGallery::CWordStyleGallery(TWordStyleGalleryData& aData)
	: iData(aData)
	{
	}

CWordStyleGallery::~CWordStyleGallery()
	{
	delete iStyleNameKeyList;
	delete iStyleNameList;
	delete iStyleDescriptor;
	}

void CWordStyleGallery::ConstructL()
	{
	iStyleNameList=new(ELeave) CDesCArrayFlat(4);
	iStyleNameKeyList=new(ELeave) CDesCArrayFlat(4);	
	}

CWordStyleGallery* CWordStyleGallery::NewLC(TWordStyleGalleryData& aData)
	{
	CWordStyleGallery* self = new (ELeave) CWordStyleGallery(aData);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
CWordStyleGallery* CWordStyleGallery::NewL(TWordStyleGalleryData& aData)
	{
	CWordStyleGallery* self = CWordStyleGallery::NewLC(aData);
	CleanupStack::Pop(self);
	return self;
	}

void CWordStyleGallery::StyleDescriptionL(TInt aIndex)
	{
	TParagraphStyleName name=(*iStyleNameList)[aIndex];
	TInt index=iData.iStyleList.IndexByName(name);
	const CParaFormatLayer* layer=(index==KErrNotFound)
		? iData.iText.GlobalParaFormatLayer()
		: iData.iStyleList.At(index).iStyle;
	FillBufferL(*layer);
	}


void CWordStyleGallery::FillBufferL(const CParaFormatLayer& aParaFormatLayer)
	{
	CEikGlobalTextEditor* edwin=(CEikGlobalTextEditor*)Control(EWordCidStyleInfo);
	TCharFormat charFormat;
	TCharFormatMask charMask;
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TParaFormatMask paraMask;
	//
	//
	iCurrentDescription=HBufC::NewL(KDescriptionGranularity);
	TPtr buf=iCurrentDescription->Des();
	TBool isNormalStyle=(aParaFormatLayer.Type()==KNormalParagraphStyleUid);
	if (!isNormalStyle)
		{
		const CParagraphStyle* flayer=REINTERPRET_CAST(const CParagraphStyle*,aParaFormatLayer.SenseBase());
		TUid type=flayer->Type();
		if (type==KNormalParagraphStyleUid)
			buf.Append(iNormalName);
		else
			buf.Append(flayer->iName);
		buf.Append('+');
		}
	//
	if (isNormalStyle)
		iData.iText.GlobalCharFormatLayer()->Sense(charFormat,charMask);
	else
		REINTERPRET_CAST(const CParagraphStyle&,aParaFormatLayer).CharFormatLayer()->Sense(charFormat,charMask);
	aParaFormatLayer.SenseL(paraFormat,paraMask);
	//
	if (!charMask.IsNull())
		{
		if (charMask.AttribIsSet(EAttFontTypeface))
			DoAppendDescriptionL(charFormat.iFontSpec.iTypeface.iName);
		if (charMask.AttribIsSet(EAttFontHeight))
			{
			TInt height=charFormat.iFontSpec.iHeight;
			height=(height+10)/20;
			TBuf<20> num;
			num.Num(height);
			TBuf<10> unit;
			iCoeEnv->ReadResource(unit,R_WORD_FONT_UNITS);
			num.Append(unit);
			DoAppendDescriptionL(num);
			}
		if (charMask.AttribIsSet(EAttFontPosture))
			{
			if (charFormat.iFontSpec.iFontStyle.Posture()==EPostureItalic)
				DoAppendDescriptionL((*iStyleDescriptor)[EDesItalic]);
			}
		if (charMask.AttribIsSet(EAttFontStrokeWeight))
			{
			if (charFormat.iFontSpec.iFontStyle.StrokeWeight()==EStrokeWeightBold)
				DoAppendDescriptionL((*iStyleDescriptor)[EDesBold]);
			}
		if (charMask.AttribIsSet(EAttFontPrintPos))
			{
			TFontPrintPosition pos=charFormat.iFontSpec.iFontStyle.PrintPosition();
			switch(pos)
				{
			case EPrintPosSuperscript:
				DoAppendDescriptionL((*iStyleDescriptor)[EDesSuperscript]);
				break;
			case EPrintPosSubscript:
				DoAppendDescriptionL((*iStyleDescriptor)[EDesSubscript]);
				break;
			default:
				break;
				}
			}
		if (charMask.AttribIsSet(EAttFontUnderline))
			{
			if (charFormat.iFontPresentation.iUnderline==EUnderlineOn)
				DoAppendDescriptionL((*iStyleDescriptor)[EDesUnderline]);
			}
		if (charMask.AttribIsSet(EAttFontStrikethrough))
			{
			if (charFormat.iFontPresentation.iStrikethrough==EStrikethroughOn)
				DoAppendDescriptionL((*iStyleDescriptor)[EDesStrikethrough]);
			}
		}
	if (!paraMask.IsNull())
		{
		AppendDescriptionL(EAttLeftMargin,paraMask);
		AppendDescriptionL(EAttRightMargin,paraMask);
		AppendDescriptionL(EAttIndent,paraMask);
		if (paraMask.AttribIsSet(EAttAlignment))
			DoAppendDescriptionL((*iStyleDescriptor)[EDesLeftAlign+paraFormat->iHorizontalAlignment]);
		AppendDescriptionL(EAttLineSpacing,paraMask);
		AppendDescriptionL(EAttSpaceBefore,paraMask);
		AppendDescriptionL(EAttSpaceAfter,paraMask);
		AppendDescriptionL(EAttKeepTogether,paraMask);
		AppendDescriptionL(EAttKeepWithNext,paraMask);
		AppendDescriptionL(EAttStartNewPage,paraMask);
		AppendDescriptionL(EAttWidowOrphan,paraMask);
		AppendDescriptionL(EAttBorderMargin,paraMask);
		AppendDescriptionL(EAttTopBorder,*paraFormat,paraMask);
		AppendDescriptionL(EAttBottomBorder,*paraFormat,paraMask);
		AppendDescriptionL(EAttLeftBorder,*paraFormat,paraMask);
		AppendDescriptionL(EAttRightBorder,*paraFormat,paraMask);
		AppendDescriptionL(EAttBullet,paraMask);
		AppendDescriptionL(EAttTabStop,*paraFormat,paraMask);
		}
	CleanupStack::PopAndDestroy();  // paraFormat
	edwin->SetTextL(iCurrentDescription);
	delete iCurrentDescription;
	iCurrentDescription=NULL;
	DrawEdwinL();
	}


void CWordStyleGallery::AppendDescriptionL(TTextFormatAttribute aAttribute,const CParaFormat& aFormat,const TParaFormatMask& aMask)
	{
	if (aMask.AttribIsSet(aAttribute) && 
	aFormat.ParaBorder((CParaFormat::TParaBorderSide)(aAttribute-EAttTopBorder)).iLineStyle!=TParaBorder::ENullLineStyle)
		{
		DoAppendDescriptionL((*iStyleDescriptor)[(TDescriptor)aAttribute]);
		}
	}


void CWordStyleGallery::AppendDescriptionL(TTextFormatAttribute aAttribute,const TParaFormatMask& aMask)
	{
	if (aMask.AttribIsSet(aAttribute))
		DoAppendDescriptionL((*iStyleDescriptor)[(TDescriptor)aAttribute]);
	}


void CWordStyleGallery::DoAppendDescriptionL(const TDesC& aText)
	{
	TInt newLength=iCurrentDescription->Length()+aText.Length();
	if (iCurrentDescription->Length()>0)
		newLength+=2;  // add 2 characters for the separator.
	if (newLength>iCurrentDescription->Des().MaxLength())
		{
		TInt alloc=KDescriptionGranularity*((newLength+KDescriptionGranularity-1)/KDescriptionGranularity);
		iCurrentDescription=iCurrentDescription->ReAllocL(alloc);
		}
	TPtr ptr=iCurrentDescription->Des();
	if (ptr.Length()>0)
		ptr.Append(_L(", "));
	ptr.Append(aText);
	}

void CWordStyleGallery::DrawEdwinL()
	{
	CEikGlobalTextEditor* edwin=(CEikGlobalTextEditor*)Control(EWordCidStyleInfo);
	TViewYPosQualifier yPosQualifier;
	yPosQualifier.SetMakeLineFullyVisible();
	edwin->TextView()->HandleGlobalChangeL(yPosQualifier); // !! Reconsider this
	edwin->DrawNow();
	}

void CWordStyleGallery::PreLayoutDynInitL()					    
	{
	DoCalcColumnWidthsL();
	}


void CWordStyleGallery::PostLayoutDynInitL()					    
	{
	DoCalcColumnWidthsL();
	iStyleDescriptor=iCoeEnv->ReadDesCArrayResourceL(R_WORD_STYLE_GALLERY_DESCRIPTIONS);
	//
	iCoeEnv->ReadResource(iNormalName , R_WORD_NORMAL);
	iCoeEnv->ReadResource(iHotKey , R_WORD_HOTKEY);
	TInt pos=0;
	iStyleNameList->Reset();						  
	iStyleNameList->AppendL(iNormalName);

	TBool styleChange;
	TInt keycount=iData.iHotKeyList.Count();

	const CParaFormatLayer* style=iData.iText.ParagraphStyle(styleChange,iData.iSelection.LowerPos(),iData.iSelection.Length());
	TInt count=iData.iStyleList.Count();
	for (TInt i=0;i<count;i++)
		iStyleNameList->AppendL(iData.iStyleList.At(i).iStyle->iName);
	iStyleNameList->Sort();
	TUid id=style->Type();
	for (TInt k=0;k<iStyleNameList->Count();k++)
		{
		if (id==KNormalParagraphStyleUid)
			{
			if (iNormalName==(*iStyleNameList)[k])
				{
				pos=k;
				break;
				}
			}
		else if(((CParagraphStyle*)style)->iName==(*iStyleNameList)[k])
			{
			pos=k;
			break;
			}
		}

	// Make sure all styles have a hot key, even if it's not assigned
	// since any pasted styles will be missing hot key info


	if (keycount < count)
		{
			TInt idx;
			TInt keysToAdd = count-keycount;

			for (idx=0; idx<keysToAdd; idx++)
				iData.iHotKeyList.AppendL(KNoHotkey);
			
		}
	else if ( keycount > count )
		{
		//too many, they should be the same so remove extras
			TInt idx;
			for (idx=keycount-1; idx>=count; idx--)
				iData.iHotKeyList.Delete(idx);

		}
	MergeArraysL();
	ListBox()->CreateScrollBarFrameL();
	ListBox()->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	ListBox()->Model()->SetItemTextArray(iStyleNameKeyList);
	ListBox()->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	ListBox()->HandleItemAdditionL();
	SetListBoxCurrentItem(EWordCidStyleNameList,pos);
	SetDeleteButton();

	CEikGlobalTextEditor* edwin=(CEikGlobalTextEditor*)Control(EWordCidStyleInfo);
	CCharFormatLayer* clayer=CONST_CAST(CCharFormatLayer*,(edwin->GlobalText())->GlobalCharFormatLayer());
	CParaFormatLayer* player=CONST_CAST(CParaFormatLayer*,(edwin->GlobalText())->GlobalParaFormatLayer());

    TCharFormatMask defaultCharFormatMask;
    TCharFormat defaultCharFormat;
	TInt fontAttributes=TTypeface::EProportional;
	defaultCharFormat.iFontSpec.iTypeface.SetAttributes(fontAttributes);
	defaultCharFormat.iFontSpec.iHeight=160;

	defaultCharFormatMask.SetAttrib(EAttFontHeight);
	defaultCharFormatMask.SetAttrib(EAttFontTypeface);
	clayer->SetL(defaultCharFormat,defaultCharFormatMask);	   
//
	CParaFormat pFormat;
	TParaFormatMask pMask;
	pFormat.iLineSpacingInTwips=160;
	pMask.SetAttrib(EAttLineSpacing);
	player->SetL(&pFormat,pMask);
//
	StyleDescriptionL(pos);
	SetLineNonFocusing(EWordCidStyleInfo);
	//
	}

void CWordStyleGallery::MergeArraysL()
	{
	iStyleNameKeyList->Reset();
	TInt count=iStyleNameList->Count();
	//
	for (TInt ii=0;ii<count;ii++)
		AppendL(ii);
	}

CEikColumnListBox* CWordStyleGallery::ListBox() const
	{
	return STATIC_CAST(CEikColumnListBox*,Control(EWordCidStyleNameList));
	}

void CWordStyleGallery::DoCalcColumnWidthsL()
	{
	CColumnListBoxData* columnData = ListBox()->ItemDrawer()->ColumnData();
	const CFont* font;
	//
	// column 0
	font = iEikonEnv->NormalFont();
	columnData->SetColumnFontL(0, font);
	columnData->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	//
	// column 1
	columnData->SetColumnWidthPixelL(1, KStyleColumnSpacerWidth);
	//
	// column 2
	font = iEikonEnv->LegendFont();
	columnData->SetColumnFontL(2, font);
	TBuf<KMaxStyleHotKeyDescription> hotkey;
	iCoeEnv->ReadResource(hotkey, R_WORD_HOTKEY);
	TInt width2 = font->TextWidthInPixels(hotkey) + font->MaxCharWidthInPixels();
	columnData->SetColumnWidthPixelL(2, width2);
	columnData->SetColumnAlignmentL(2, CGraphicsContext::ELeft);
	//
	// more column 0
	TInt width0 = ListBox()->View()->ViewRect().Width() - KStyleColumnSpacerWidth - width2 - CEikScrollBar::DefaultScrollBarBreadth(); // only works if called from PostLayout...
	columnData->SetColumnWidthPixelL(0, width0);
	}

void CWordStyleGallery::SetDeleteButton()
	{
	TInt index = ListBoxCurrentItem(EWordCidStyleNameList);
	TBool dimmed = EFalse;
	if (iNormalName == (*iStyleNameList)[index])
		dimmed = ETrue;
	else
		{
		CParagraphStyle* paraStyle = iData.iStyleList.PtrByName((*iStyleNameList)[index])->iStyle;
		TUid type = paraStyle->Type();
		dimmed = (type == KSystemParagraphStyleUid);
		}
	ButtonGroupContainer().DimCommand(EEikBidDelete, dimmed);
	}

TBool CWordStyleGallery::OkToExitL(TInt aButtonId)
	{
	switch(aButtonId)
		{
		case EWordCidAddStyle:
		case EWordCidModifyStyle:
			StyleL(aButtonId);
			return EFalse;
		case EEikBidDelete:
			{
			if (ButtonGroupContainer().IsCommandDimmed(EEikBidDelete))
				return EFalse;
			TInt index = ListBoxCurrentItem(EWordCidStyleNameList);			   
			TParagraphStyleName name = (*iStyleNameList)[index]; 
			//			
			TBuf<RMessageWindow::EMaxTextLength> title;
			TBuf<RMessageWindow::EMaxTextLength> res;
			iCoeEnv->ReadResource(res, R_WORD_DELETE_STYLE_TITLE);
			title.Format(res, &name);
			//
			if (iEikonEnv->QueryWinL(title, _L("")))
				{// Can never delete the normal style
				CParagraphStyle* paraStyle = iData.iStyleList.PtrByName(name)->iStyle;
				TInt offset = iData.iStyleList.IndexByPtr(paraStyle);
				__ASSERT_ALWAYS(offset >= 0, Panic(EWordGalleryInvalidStyle));
				iData.iText.NotifyStyleDeletedL(paraStyle);
				iData.iStyleList.Remove(paraStyle);
				iStyleNameList->Delete(index);
				iStyleNameKeyList->Delete(index);
				iData.iHotKeyList.Delete(offset);
				ListBox()->Reset();
				if (index == iStyleNameList->Count())
					index = index-KNormalIndex;
				ListBox()->SetCurrentItemIndex(index);
				ListBox()->UpdateScrollBarsL();				  //don't have to this.listbox should provide Handledeleteitem method
				ListBox()->ScrollToMakeItemVisible(index);
				ListBox()->DrawNow();
				SetTextForCancelButtonL();
				iData.iApplyStyle = ETrue;
				}
			SetDeleteButton();
			StyleDescriptionL(index);
			return EFalse;
			}
		case EEikBidOk:
			{
			TInt index = ListBoxCurrentItem(EWordCidStyleNameList);
			MUnifiedEditor::MStyleSupport* style_support = iData.iEditor.StyleSupport();
			if (style_support)
				{
				TPtrC styleName = (*iStyleNameList)[index];
				// normal style name will not be recognised by the editor object,
				// as it is our own invention. A null descriptor should be used.
				const TPtrC nullDes;
				if (styleName == iNormalName)
					styleName.Set(nullDes);
				style_support->SetStyleL(iData.iSelection.LowerPos(), iData.iSelection.Length(), styleName);
				}
			iData.iApplyStyle=ETrue;
			}
		}
	//
	return ETrue;
	}

void CWordStyleGallery::HandleControlStateChangeL(TInt aControlId)  
	{
	if (aControlId==EWordCidStyleNameList)
		{
		TInt index=ListBoxCurrentItem(EWordCidStyleNameList);
		StyleDescriptionL(index);
		SetDeleteButton();
		}
	}

void CWordStyleGallery::StyleL(TInt aControlId)
	{	
	TInt index=ListBoxCurrentItem(EWordCidStyleNameList);			   
	CParagraphStyle* paraStyle=NULL;
	CEikDialog* dialog=NULL;
	TBool normal=EFalse;
	TChar newHotkey=KNoHotkey;
	if (aControlId==EWordCidAddStyle)
		{
		paraStyle=CParagraphStyle::NewL(*(iData.iText.GlobalParaFormatLayer()),*(iData.iText.GlobalCharFormatLayer()));
		dialog=new(ELeave) CWordStyleFormatDialog(iNormalName,iData.iNormalStyleHotKey,iData.iStyleList,
									aControlId,iData.iText,&iData.iPrintSetup,iData.iHotKeyList,newHotkey,paraStyle);
		}
	else
		{
		if ((*iStyleNameList)[index]==iNormalName)
			{
			normal=ETrue;
			dialog=new(ELeave) CWordStyleFormatDialog(iNormalName,iData.iNormalStyleHotKey,iData.iStyleList,
									aControlId,iData.iText,&iData.iPrintSetup,iData.iHotKeyList,newHotkey,NULL,normal);
			}
		else
			{
			paraStyle=iData.iStyleList.PtrByName((*iStyleNameList)[index])->iStyle;
			dialog=new(ELeave) CWordStyleFormatDialog(iNormalName,iData.iNormalStyleHotKey,iData.iStyleList,
									aControlId,iData.iText,&iData.iPrintSetup,iData.iHotKeyList,newHotkey,paraStyle);
			}
		if (normal)
			newHotkey=iData.iNormalStyleHotKey;
		else
			{
			TInt ii=iData.iStyleList.IndexByPtr(paraStyle);
			__ASSERT_DEBUG(ii!=KErrNotFound,Panic(EStyleIntegrityError));
			newHotkey=iData.iHotKeyList[ii];
			}
		}
	//
	if (dialog->ExecuteLD(R_WORD_DIALOG_STYLE_FORMAT))
		{
		SetTextForCancelButtonL();	
		// remove the hotkey from any old style if it has been re-assigned
		if (newHotkey==iData.iNormalStyleHotKey)
			iData.iNormalStyleHotKey=0;
		else
			{
			TInt hotkeyCount=iData.iHotKeyList.Count();
			for (TInt ii=0;ii<hotkeyCount;ii++)
				{
				if (newHotkey==iData.iHotKeyList[ii] && newHotkey!=KNoHotkey)
					{
					iData.iHotKeyList[ii]=0;
					break;
					}					
				}
			}
		//
		if (aControlId==EWordCidAddStyle)
			{
			RParagraphStyleInfo styleSet(paraStyle);
			iData.iStyleList.AppendL(&styleSet);
			}
		else
			{
			iStyleNameList->Delete(index);
			iStyleNameKeyList->Delete(index);
			}
		if (normal)
			iStyleNameList->AppendL(iNormalName);
		else
			iStyleNameList->AppendL(paraStyle->iName);
		iStyleNameList->Sort();
		//
		TParagraphStyleName name=(normal) ? iNormalName : paraStyle->iName;
		for (TInt ii=0;ii<iStyleNameList->Count();ii++)
			{
			if (name==(*iStyleNameList)[ii])
				{
				index=ii;
				break;
				}
			}

		if (aControlId==EWordCidAddStyle)
			iData.iHotKeyList.AppendL(newHotkey);
		else
			{
			if (normal)
				iData.iNormalStyleHotKey=newHotkey;
			else
				{
				CParagraphStyle* style=iData.iStyleList.PtrByName((*iStyleNameList)[index])->iStyle;
				TInt offset=iData.iStyleList.IndexByPtr(style);
				iData.iHotKeyList[offset]=newHotkey;
				}
			}
		MergeArraysL();
		ListBox()->HandleItemAdditionL();
		ListBox()->SetCurrentItemIndexAndDraw(index);
		StyleDescriptionL(index);
		SetDeleteButton();
		}
	else
		{
		if (aControlId==EWordCidAddStyle)
			delete paraStyle;
		}
	}

void CWordStyleGallery::AppendL(TInt aIndex)
	{
	TBuf<100> name;
	name = (*iStyleNameList)[aIndex];
	//
	// truncate name if necessary
	CColumnListBoxData* columnData = ListBox()->ItemDrawer()->ColumnData();
	TInt columnWidth = columnData->ColumnWidthPixel(0);
	const CFont* columnFont = columnData->ColumnFont(0);
	if (columnFont->TextWidthInPixels(name) > columnWidth)
		TextUtils::ClipToFit(name, *columnFont, columnWidth);
	//
	TChar key = iData.iNormalStyleHotKey;
	if (name != iNormalName)
		{
		CParagraphStyle* pp = iData.iStyleList.PtrByName(name)->iStyle;
		TInt index = iData.iStyleList.IndexByPtr(pp);
		key = iData.iHotKeyList[index];
		}
	if (key > 0)
		{
		name.Append(KColumnListSeparator);
		name.Append(KColumnListSeparator); // one for empty spacer column
		name.Append(iHotKey);
		name.Append(key);		
		name.Append(KColumnListSeparator);
		}
	iStyleNameKeyList->AppendL(name);
	}

void CWordStyleGallery::SetTextForCancelButtonL()
	{
	TBuf<20> buf;
	iCoeEnv->ReadResource(buf, R_WORD_CLOSE);
	const TInt pos = ButtonGroupContainer().PositionById(EEikBidCancel);	
	ButtonGroupContainer().SetCommandL(pos, EEikBidCancel, buf);
	iData.iApplyStyle = ETrue;
	}

///////////////////Add Modify button dialogs//////////////////////

CWordStyleFormatDialog::CWordStyleFormatDialog(const TParagraphStyleName& aNormalName,const TChar aNormalStyleHotkey,const CStyleList& aStyleList,const TInt aButton,CRichText& aText,CPrintSetup* aPrintSetup,CArrayFix<TChar>& aHotKeyList,TChar& aChar,CParagraphStyle* aParagraphStyle,TBool aNormalStyle)
	:
	iStyleList(aStyleList),
	iText(&aText),
	iButton(aButton),
	iParagraphStyle(aParagraphStyle),
	iNormalStyle(aNormalStyle),
	iPrintSetup(aPrintSetup),
	iHotKeyList(&aHotKeyList),
	iNormalStyleHotkey(aNormalStyleHotkey),
	iNormalName(aNormalName),
	iChar(aChar)
	{
		if (aParagraphStyle) iOutline = aParagraphStyle->OutlineLevel();
		else iOutline = 0;
	}

CWordStyleFormatDialog::~CWordStyleFormatDialog()
	{
	delete iBullet;
	}

void CWordStyleFormatDialog::PreLayoutDynInitL()
	{
	//
	// Dynamically change the title of this dialog as appropriate
	TInt id = (iButton == EWordCidAddStyle)?
		R_WORD_STYLEFORMAT_TITLE_ADD : R_WORD_STYLEFORMAT_TITLE_MODIFY;
	if (id == R_WORD_STYLEFORMAT_TITLE_ADD)
		SetTitleL(id);
	else
		{
		TBuf<24> buf;
		iCoeEnv->ReadResource(buf,R_WORD_STYLEFORMAT_TITLE_MODIFY);
		TBuf<24+KMaxParagraphStyleName> title; // 56
		TParagraphStyleName name;
		name = (iNormalStyle)? iNormalName : iParagraphStyle->iName;
		title.Format(buf,&name);
		SetTitleL(title);
		}
	//
	TParaFormatMask paraMask;
	CParaFormat* paraFormat=CParaFormat::NewLC();
	ButtonGroupContainer().ButtonById(EWordCidFormatList)->
		SetButtonLayout(CEikCommandButton::ETextLeftPictureRight);
	if (iNormalStyle)
		{
		TParagraphStyleName name;
		iCoeEnv->ReadResource(name , R_WORD_NORMAL);
		SetEdwinTextL(EWordCidStyleName,&name);
		SetLineDimmedNow(EWordCidStyleName,ETrue);
		SetLineDimmedNow(EWordCidOutline,ETrue);
		iText->GlobalParaFormatLayer()->SenseL(paraFormat,paraMask);
		}
	else
		{
		SetEdwinTextL(EWordCidStyleName,&iParagraphStyle->iName);
		TUid type=iParagraphStyle->Type();
		if (type==KSystemParagraphStyleUid)
			SetLineDimmedNow(EWordCidStyleName,ETrue);
		iParagraphStyle->SenseL(paraFormat,paraMask);
		}
	//hotkey
	if (iButton==EWordCidModifyStyle)
		{
		CEikCharEditor* editor=(CEikCharEditor*)Control(EWordCidHotKey);
		editor->SetChar(iChar);
		}
	//
	CEikButtonBase::TState state;
	state=(paraFormat->iBullet)? CEikButtonBase::ESet : CEikButtonBase::EClear ;
	SetCheckBoxState(EWordCidBullet,state);

	// Set values for Outline level

	SetNumberEditorValue(EWordCidOutline,iOutline);

	CleanupStack::PopAndDestroy();
	}


TBool CWordStyleFormatDialog::OkToExitL(TInt aButtonId)
	{
	if (aButtonId == EWordCidFormatList)
		{
		CEikLabeledButton* labelButton = STATIC_CAST(CEikLabeledButton*,ButtonGroupContainer().ControlOrNull(EWordCidFormatList));
		if (labelButton != NULL)
			{
			CEikMenuButton* menuButton = STATIC_CAST(CEikMenuButton*, labelButton->Button());
			menuButton->LaunchPopupMenuL(this);				
			return EFalse;
			}
		}
	if (aButtonId == EEikBidOk)
		{	
		TParagraphStyleName name;
		GetEdwinText(name,EWordCidStyleName);
		if (!name.Length())
			{
			iEikonEnv->InfoMsg(R_WORD_NOSTYLENAME);
			return EFalse;
			}

		if (iParagraphStyle)
		{
		iOutline = NumberEditorValue(EWordCidOutline);
		iParagraphStyle->SetOutlineLevel(iOutline);
		}
		
		
		// check for normal name as well
		TParagraphStyleName normalName;
		iCoeEnv->ReadResource(normalName , R_WORD_NORMAL);
		 // you cannot modify names of the built in styles
		if (iButton == EWordCidModifyStyle)
			{
			if (iNormalStyle)
				return ETrue;
			else
				{
				TUid type = iParagraphStyle->Type();
				if (type == KSystemParagraphStyleUid)
					return ETrue;
				else   // userdefined style
					{  	// now check the style list to see if the specified style already exists
					CStyleList* styleList = iText->StyleList();
					for (TInt i = 0; i < styleList->Count(); i++)
						{
						if ((name == (*styleList)[i].iStyle->iName) || (name == normalName))
							{
							if (name != iParagraphStyle->iName)
								{
								iEikonEnv->InfoMsg(R_WORD_STYLENAMEEXISTS);
								return EFalse;
								}
							}
						}
					}
				}
			}
		else
			{
			CStyleList* list = iText->StyleList();
			for (TInt i = 0; i < list->Count(); i++)
				if ((name == (*list)[i].iStyle->iName) || (name == normalName))
					{
					iEikonEnv->InfoMsg(R_WORD_STYLENAMEEXISTS);
					return EFalse;
					}
			}
		GetEdwinText(iParagraphStyle->iName, EWordCidStyleName);
		}					   
	return ETrue;
	}	  						  

SEikControlInfo CWordStyleFormatDialog::CreateCustomControlL(TInt aControlType)
	{
	SEikControlInfo info;
	info.iTrailerTextId=0;
    info.iFlags=0;
	if (aControlType==EWordCtStyleShortcutKeyEditor)
		info.iControl=new(ELeave) CWordStyleHotKeyEditor(this);
	else
		info.iControl=NULL;
	return info;
	}

void CWordStyleFormatDialog::HandleControlStateChangeL(TInt aControlId)
	{
	if (aControlId==EWordCidBullet)
		SetBulletL();
	if (aControlId==EWordCidHotKey)
		iChar = ((CEikCharEditor*)Control(EWordCidHotKey))->GetChar();
	}


void CWordStyleFormatDialog::HandleInteractionRefused(TInt aControlId)
	{	
	if (aControlId==EWordCidOutline)
		iEikonEnv->InfoMsg(R_WORD_NORMAL_OUTLINE);	
	else
		iEikonEnv->InfoMsg(R_EIK_TBUF_NOT_AVAILABLE);
	}


TBool CWordStyleFormatDialog::ValidateHotkey(TKeyEvent& aKeyEvent)
// return false if the hotkey is invalid
// otherwise set aKeyEvent to the hotkey I have validated (ie uppercase it)
	{
	// check that the key is a valid character
	TChar hotkey=aKeyEvent.iCode;
	if (!hotkey.IsAlphaDigit())
		{
		iEikonEnv->InfoMsg(R_WORD_HOTKEY_INVALID);
		return EFalse;
		}
	//
	// upper-case the key
	hotkey.UpperCase();
	//
	// check that it isn't already in use
	HotkeyConflict(hotkey);
	//
	// set the hotkey and return
	aKeyEvent.iCode = hotkey;
	return ETrue;
	}


TBool CWordStyleFormatDialog::HotkeyConflict(TChar aHotkey)
	{
	TBool hotkeyConflict=EFalse;
	TInt index=-1;
	if (aHotkey==iNormalStyleHotkey)
		hotkeyConflict=ETrue;
	else
		{
		TInt hotkeyCount=iHotKeyList->Count();
		for (index=0;index<hotkeyCount;index++)
			{
			if (aHotkey==(*iHotKeyList)[index])
				{
				hotkeyConflict=ETrue;
				break;
				}
			}
		}
	//
	if (hotkeyConflict)
		{
		//there isnt an entry in the stylelist
		if (index >iStyleList.Count() )
			hotkeyConflict=EFalse;
		else
			{
				TBuf<80> msg;
				TBuf<48> fmtString;

				iCoeEnv->ReadResource(fmtString,R_WORD_HOTKEYEXISTS);
				TParagraphStyleName name=(index>=0)
					? iStyleList.At(index).iStyle->iName
					: iNormalName;
				msg.Format(fmtString,&name);
				iEikonEnv->InfoMsg(msg);
			}
		}
	//
	return hotkeyConflict;
	}


void CWordStyleFormatDialog::SetBulletL()
	{
	TParaFormatMask paraMask;
	CParaFormat* paraFormat=CParaFormat::NewLC();
	if (iNormalStyle)
		iText->GlobalParaFormatLayer()->SenseL(paraFormat,paraMask);
	else 
		iParagraphStyle->SenseL(paraFormat,paraMask);
	delete paraFormat->iBullet;

	if(((CEikCheckBox*)Control(EWordCidBullet))->State()==CEikButtonBase::EClear)
		{
		paraFormat->iBullet=NULL;
		paraFormat->iIndentInTwips=0;
		paraMask.ClearAttrib(EAttBullet);
		paraMask.ClearAttrib(EAttIndent);
		}
	else
		{
		paraMask.SetAttrib(EAttBullet);
		paraMask.SetAttrib(EAttIndent);
		TBullet* bullet=new(ELeave) TBullet;
		if (!iBullet)
			CreateDefaultBulletL();
		*bullet=*iBullet;									 
		paraFormat->iBullet=bullet;
		if (iBullet->iHangingIndent)
			paraFormat->iIndentInTwips=283; // about 1/2cm - completely arbitrary value !!
		else
			paraFormat->iIndentInTwips=0;
		}
	if (iNormalStyle)
		((CParaFormatLayer*)iText->GlobalParaFormatLayer())->SetL(paraFormat,paraMask);
	else
		iParagraphStyle->SetL(paraFormat,paraMask);
	TViewYPosQualifier yPosQ;
	yPosQ.SetMakeLineFullyVisible();
	CleanupStack::PopAndDestroy();
	}

void CWordStyleFormatDialog::CreateDefaultBulletL()
	{
	iBullet=new(ELeave) TBullet;
	iBullet->iCharacterCode=0x2022;
	iBullet->iHeightInTwips=iEikonEnv->NormalFont()->FontSpecInTwips().iHeight;
	iBullet->iTypeface.iName=_L("Swiss");
	}

void CWordStyleFormatDialog::ProcessCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
	case EEikCmdFontChange:	
		RunFontDialogL();
		break;
	case EEikCmdParaIndent:		
	case EEikCmdParaSpacing:	
	case EEikCmdParaBorder:			
	case EEikCmdParaTabs:		
	case EEikCmdParaAlign:		
		RunDialogL((TStyleFormatDialog)aCommand);					   
		break;
	case EWordCmdFormatBullet:
		RunSelectBulletDialogL();
		break;
		}
	}
		
CParaFormatLayer* CWordStyleFormatDialog::GetParaLayerAndFormattingL(CParaFormat& aFormat,TParaFormatMask& aMask)
//
	{
	CParaFormatLayer* layer=(iNormalStyle)
		? CONST_CAST(CParaFormatLayer*,iText->GlobalParaFormatLayer())
		: iParagraphStyle;
	DoGetParaFormatL(aFormat,aMask,*layer);
	return layer;
	}


void CWordStyleFormatDialog::DoGetParaFormatL(CParaFormat& aFormat,TParaFormatMask& aMask,const CParaFormatLayer& aLayer)
//
	{
	aLayer.SenseEffectiveL(&aFormat);
	aLayer.SenseL(&aFormat,aMask);
	}


void CWordStyleFormatDialog::RunSelectBulletDialogL()
	{
	if (!iBullet)
		CreateDefaultBulletL();
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TParaFormatMask paraMask;
	TCharFormat charFormat;
	TCharFormatMask charMask;
//	TCharFormatMask charUndeterminedMask;
	if(iNormalStyle)
		{
		iText->GlobalParaFormatLayer()->SenseL(paraFormat,paraMask);
		iText->GlobalCharFormatLayer()->Sense(charFormat,charMask);
		}
	else
		{
		iParagraphStyle->SenseEffectiveL(paraFormat);
		iParagraphStyle->CharFormatLayer()->SenseEffective(charFormat);
		}
	TRgb backColor=paraFormat->iFillColor;
	if (paraMask.AttribIsSet(EAttFillColor))
		backColor=KRgbWhite; // what else could be done ???
	CEikDialog* dialog=new(ELeave) CWordBulletDialog(iBullet,backColor,NULL,charFormat.iFontSpec.iTypeface.iName);
	if (dialog->ExecuteLD(R_WORD_DIALOG_BULLET))
		{
		SetCheckBoxState(EWordCidBullet,CEikButtonBase::ESet);
		SetBulletL();
		}
	CleanupStack::PopAndDestroy();
	}


void CWordStyleFormatDialog::RunDialogL(TStyleFormatDialog aFunction)
	{
	TParaFormatMask mask;
	TParaFormatMask undeterminedMask;  // always NULL for these calls
	CParaFormat* format=CParaFormat::NewLC();  // !! could use 1 on the stack for some
	CParaFormatLayer* layer=GetParaLayerAndFormattingL(*format,mask);
//	CEikDialog* dialog=NULL;
	switch(aFunction)
		{
		case EIndentsDialog:
		case ETabsDialog:
			{
			TPageSpec pageSpec=iPrintSetup->PrinterDevice()->CurrentPageSpecInTwips();
			TInt width=pageSpec.iPortraitPageSize.iWidth;
			if (pageSpec.iOrientation==TPageSpec::ELandscape)
				width=pageSpec.iPortraitPageSize.iHeight;
			width-=(iPrintSetup->iPageMarginsInTwips.iMargins.iLeft+iPrintSetup->iPageMarginsInTwips.iMargins.iRight);
			if (aFunction==ETabsDialog)
				{
				(void)CEikTabsDialog::RunDlgLD(format, mask, undeterminedMask, width);
				}
			else
				{
				(void)CEikIndentDialog::RunDlgLD(format, mask, undeterminedMask, width);
				}
			break;
			}
		case EBordersDialog:
			(void)CEikBorderDialog::RunDlgLD(format, mask, undeterminedMask);
			break;
		case EAlignmentDialog:
			(void)CEikAlignDialog::RunDlgLD(format,mask,undeterminedMask);
			break;
		case ELineSpacingDialog:
			(void)CEikSpaceDialog::RunDlgLD(format,mask,undeterminedMask);
			break;
		}
	layer->SetL(format,mask);
	CleanupStack::PopAndDestroy();  // format
	}


void CWordStyleFormatDialog::RunFontDialogL()
	{
	TCharFormat charFormat;
	TCharFormatMask charMask;
	CCharFormatLayer* charLayer=NULL;
	if (iNormalStyle)
		{
		charLayer=CONST_CAST(CCharFormatLayer*,iText->GlobalCharFormatLayer());
		charLayer->SenseEffective(charFormat);  // used to seed the dialog
		charLayer->Sense(charFormat,charMask);  // gets the mask
		}
	else
		{
		charLayer=iParagraphStyle->CharFormatLayer();
		charLayer->SenseEffective(charFormat);
		charLayer->Sense(charFormat,charMask);
		}
	if (!CEikFontDialog::RunDlgLD(charFormat, charMask, EGulNoSymbolFonts, iCoeEnv->ScreenDevice()))
		return;
	charLayer->SetL(charFormat,charMask);
	}

void CWordStyleFormatDialog::SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
	{
	}


//
// class CWordStyleHotKeyEditor
//

CWordStyleHotKeyEditor::CWordStyleHotKeyEditor(MWordHotkeyValidator* aValidator)
:iValidator(aValidator)
	{}


TKeyResponse CWordStyleHotKeyEditor::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType!=EEventKey)
		return EKeyWasNotConsumed;
	//
	TChar hotkey=aKeyEvent.iCode;
	if (hotkey==EKeyUpArrow || hotkey==EKeyDownArrow || hotkey==EKeyLeftArrow || hotkey==EKeyRightArrow)
		return EKeyWasNotConsumed;
	//
	TKeyEvent event=aKeyEvent;
	if (!iValidator->ValidateHotkey(event))
		return EKeyWasConsumed;
	//
	return CEikCharEditor::OfferKeyEventL(event,aType);
	}
