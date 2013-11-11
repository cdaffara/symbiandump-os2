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


#include <frmvis.h>

#include <prninf.h>
#include <prnsetup.h>

#include <badesca.h>
#include <bamatch.h>

#include <e32std.h>

#include <coeutils.h>

#include <eikenv.h>
#include <eikfctry.h>
#include <techview/eikchlst.h>
#include <techview/eikfprev.h>
#include <techview/eikbtpan.h>
#include <techview/eikcmbut.h>
#include <techview/eikchkbx.h>
#include <techview/eikfpne.h>
#include <techview/eikcfdlg.h>
#include <techview/eikfsel.h>
#include <techview/eikprtdg.h>
#include <techview/eikinfo.h>
#include <techview/eikon.rsg>
#include <techview/eikfsel.h>
#include <eikfutil.h>
#include <techview/eikdclbm.h>
#include <techview/eikfbrow.h>
#include <techview/eikufsel.h>
#include <techview/eiklbbut.h>
#include <techview/eikchmap.h>
#include <techview/eikbtgpc.h>

#include "WPOUTLNE.H"
#include "WPDIALGS.H"
#include "WPPANIC.H"
#include "wpresources.h"
#include "WORD.HRH"
#include <word.rsg>
#include "WPMAIN.H"

const TInt KShortErrorMessageLength = 64;
const TInt KMaxDisplayedFullNameLen	= 35;

//
// class CWordBulletDialog
//

CWordBulletDialog::CWordBulletDialog(TBullet* aBullet,TRgb aBackColor, CPrinterDevice* ,const TDesC& aFontName)
	:iFontName(aFontName)
	{
	iWordBullet=aBullet;
	iBullet=*aBullet;
	iBackColor=aBackColor;
	}

CWordBulletDialog::~CWordBulletDialog()
	{
	}

void CWordBulletDialog::PreLayoutDynInitL()
	{
	CEikCharMap* charMap = STATIC_CAST(CEikCharMap*, Control(EWordCidBulletCharMap));
	charMap->SetChar(iBullet.iCharacterCode);
	charMap->DrawNow();
	}

void CWordBulletDialog::HandleControlStateChangeL(TInt aControlId)
	{
	if (aControlId == EWordCidBulletCharMap)
		{
		const TUint charCode = STATIC_CAST(CEikCharMap*, Control(EWordCidBulletCharMap))->GetChar();
		iBullet.iCharacterCode = (TText)charCode;
		}
	}

TBool CWordBulletDialog::OkToExitL(TInt aButtonId)
	{
	if (aButtonId==EWordCidFormatBullet)
		{
		CEikDialog* dialog=new(ELeave) CWordBulletFormatDialog(&iBullet,iBackColor,NULL,iFontName);
		dialog->ExecuteLD(R_WORD_DIALOG_FORMAT_BULLET);
		return EFalse;
		}
	*iWordBullet=iBullet;
	return ETrue;
	}

//
// class CWordBulletFormatDialog
//

CWordBulletFormatDialog::CWordBulletFormatDialog(TBullet* aBullet,TRgb aBackColor, CPrinterDevice* ,const TDesC& aFontName)
	:iFontName(aFontName)
	{
	iWordBullet=aBullet;
	iBullet=*aBullet;
	iBackColor=aBackColor;
	}

CWordBulletFormatDialog::~CWordBulletFormatDialog()
	{
	delete iTwipsList;
	}

SEikControlInfo CWordBulletFormatDialog::CreateCustomControlL(TInt aControlType)
	{
	SEikControlInfo info;
    info.iControl=NULL;
	info.iTrailerTextId=0;
	info.iFlags=EEikControlIsNonFocusing;
	if (aControlType==EWordCtBulletPreviewLabel)
		info.iControl=new(ELeave) CWordBulletPreviewLabel;
	return info;
	}

void CWordBulletFormatDialog::PreLayoutDynInitL()
	{
	CWordBulletPreviewLabel* label=(CWordBulletPreviewLabel*)Control(EWordCidBulletCharacter);
	label->iBrushColor=iBackColor;
	label->iTextColor=iBullet.iColor;
	TFontSpec fontSpec;
	fontSpec.iHeight=iBullet.iHeightInTwips;
	fontSpec.iTypeface.iName=_L("Swiss");
	label->SetFontSpecL(fontSpec);
	SetPreviewLabelL(iBullet.iCharacterCode);
	SetGraySelectorColor(EWordCidBulletColor,iBullet.iColor);
	iTwipsList=new(ELeave) CArrayFixFlat<TInt>(4);
	iPointsList=new(ELeave) CDesCArrayFlat(4);
	FontUtils::GetAvailableHeightsInTwipsAndPointsL(*iCoeEnv->ScreenDevice(),iFontName,*iTwipsList,*iPointsList);
	CEikChoiceList* chlst=(CEikChoiceList*)Control(EWordCidBulletHeight);
	chlst->SetArrayL(iPointsList);
	chlst->SetArrayExternalOwnership(EFalse);
	const TInt index = FontUtils::IndexOfNearestHeight(*iTwipsList, iBullet.iHeightInTwips);
	chlst->SetCurrentItem(index);
	CEikButtonBase::TState state=(iBullet.iHangingIndent? CEikButtonBase::ESet : CEikButtonBase::EClear);
	SetCheckBoxState(EWordCidBulletIndent,state);
	}

void CWordBulletFormatDialog::SetPreviewLabelL(TUint aCharCode)
	{
	TChar bullet(aCharCode);
	TBuf<1> buf;
	buf.Append(bullet);
	SetLabelL(EWordCidBulletCharacter,buf);
	}

void CWordBulletFormatDialog::HandleControlStateChangeL(TInt aControlId)
	{
	switch (aControlId)
		{
	case EWordCidBulletColor:
		{
		const TRgb color=GraySelectorColor(EWordCidBulletColor);
		CWordBulletPreviewLabel* label=(CWordBulletPreviewLabel*)Control(EWordCidBulletCharacter);
		label->iTextColor=color;
		label->DrawNow();
		iBullet.iColor=color;
		break;
		}
	case EWordCidBulletHeight:
		{
		const TInt height=(*iTwipsList)[ChoiceListCurrentItem(EWordCidBulletHeight)];
		iBullet.iHeightInTwips=height;
		TFontSpec fontSpec;
		CWordBulletPreviewLabel* label=(CWordBulletPreviewLabel*)Control(EWordCidBulletCharacter);
		fontSpec.iHeight=height;
		fontSpec.iTypeface.iName=_L("Swiss");
		label->SetFontSpecL(fontSpec);
		label->DrawNow();
		break;
		}
	case EWordCidBulletIndent:
		iBullet.iHangingIndent=(CheckBoxState(EWordCidBulletIndent)==CEikButtonBase::ESet);
		break;
	default:
		;
		}
	}

TBool CWordBulletFormatDialog::OkToExitL(TInt /*aButtonId*/)
	{
	*iWordBullet=iBullet;
	return ETrue;
	}

//
// class CWordBulletPreviewLabel
//

CWordBulletPreviewLabel::CWordBulletPreviewLabel()
	{}

CWordBulletPreviewLabel::~CWordBulletPreviewLabel()
	{
	iCoeEnv->ReleaseScreenFont(CONST_CAST(CFont*,iPrevFont));
	}

void CWordBulletPreviewLabel::SetFontSpecL(TFontSpec& aFontSpec)
	{
	iCoeEnv->ReleaseScreenFont(CONST_CAST(CFont*,iPrevFont));
	iPrevFont=iCoeEnv->CreateScreenFontL(aFontSpec);
	}

TSize CWordBulletPreviewLabel::MinimumSize()
	{
	return TSize(40,40); // an attempt to make label big enough for most font heights
	}

void CWordBulletPreviewLabel::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc=SystemGc();
	gc.UseFont(iPrevFont);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(iBrushColor);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(iTextColor);
	const TInt ascent=iPrevFont->AscentInPixels();
	const TRect rect(Rect());
	const TInt baselineOffset=ascent+((rect.Height()-ascent)/2);
	gc.DrawText(*iText,rect,baselineOffset,CGraphicsContext::ECenter);
	}

//
// class CWordGoToDialog
//

CWordGoToDialog::CWordGoToDialog(TInt aMaxPages, TInt* aCurrentPage)
    {
	iMaxPages=aMaxPages;
	iCurrentPage=aCurrentPage;
    }
    
void CWordGoToDialog::PreLayoutDynInitL()
	{
	SetNumberEditorMinAndMax(EWordCidPageNumber,1,iMaxPages);
	SetNumberEditorValue(EWordCidPageNumber,*iCurrentPage);

	TBuf<10> buf;
	buf.Num(iMaxPages);
    SetLabelL(EWordCidPages, buf);
	}

TBool CWordGoToDialog::OkToExitL(TInt /*aButtonId*/)
	{
	*iCurrentPage=NumberEditorValue(EWordCidPageNumber);
	return(ETrue);
	}

//
//	CWordViewPreferencesDialog
//

CWordViewPreferencesDialog::CWordViewPreferencesDialog(TNonPrintingCharVisibility& aVisibility)
	: iVisibility(aVisibility)
	{}

void CWordViewPreferencesDialog::PreLayoutDynInitL()
	{
	CEikButtonBase::TState button=CEikButtonBase::EClear;
	//tabs
	button=(iVisibility.TabsVisible())?CEikButtonBase::ESet : CEikButtonBase::EClear;	
	SetCheckBoxState(EWordCidTab,button);
	//spaces
	button=(iVisibility.SpacesVisible())?CEikButtonBase::ESet : CEikButtonBase::EClear;		
	SetCheckBoxState(EWordCidSpaces,button);
	//paragraph ends
	button=(iVisibility.ParagraphDelimitersVisible())?CEikButtonBase::ESet : CEikButtonBase::EClear;		
	SetCheckBoxState(EWordCidParaEnds,button);
	//hyphens
	button=(iVisibility.NonBreakingHyphensVisible())?CEikButtonBase::ESet : CEikButtonBase::EClear;		
	SetCheckBoxState(EWordCidHardHyphens,button);
	//line breaks
	button=(iVisibility.LineBreaksVisible())?CEikButtonBase::ESet : CEikButtonBase::EClear;		
	SetCheckBoxState(EWordCidForcedLineBreaks,button);
	//nonbreaking spaces
	button=(iVisibility.NonBreakingSpacesVisible())?CEikButtonBase::ESet : CEikButtonBase::EClear;		
	SetCheckBoxState(EWordCidNonBreakingSpace,button);
	}

void CWordViewPreferencesDialog::WordSetCheckBoxState(CEikButtonBase::TState aState)
	{
	SetCheckBoxState(EWordCidTab,aState);
	SetCheckBoxState(EWordCidSpaces,aState);
	SetCheckBoxState(EWordCidParaEnds,aState);
	SetCheckBoxState(EWordCidHardHyphens,aState);
	SetCheckBoxState(EWordCidForcedLineBreaks,aState);
	SetCheckBoxState(EWordCidNonBreakingSpace,aState);
	} 												  

TBool CWordViewPreferencesDialog::OkToExitL(TInt /*aButtonId*/)
	{
	TBool visible;
	CEikButtonBase::TState button=CEikButtonBase::EClear;
	//tabs
	button=CheckBoxState(EWordCidTab);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iVisibility.SetTabsVisible(visible);
	//spaces
	button=CheckBoxState(EWordCidSpaces);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iVisibility.SetSpacesVisible(visible);
	//paragraph ends
	button=CheckBoxState(EWordCidParaEnds);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iVisibility.SetParagraphDelimitersVisible(visible);
	//hyphens
	button=CheckBoxState(EWordCidHardHyphens);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iVisibility.SetNonBreakingHyphensVisible(visible);
	//line breaks
	button=CheckBoxState(EWordCidForcedLineBreaks);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iVisibility.SetLineBreaksVisible(visible);
	//nonbreaking spaces
	button=CheckBoxState(EWordCidNonBreakingSpace);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iVisibility.SetNonBreakingSpacesVisible(visible);
	//
	return ETrue;
	}



//
//	CWordObjectPreferencesDialog & Revert To Style dialog
//

CWordObjectPreferencesDialog::CWordObjectPreferencesDialog(TBool& aSketchAsGlass,TBool& aGraphAsGlass/*,TInt aButton*/)
	: iSketchAsGlass(aSketchAsGlass),iGraphAsGlass(aGraphAsGlass)/*,iButton(aButton)*/
	{}

void CWordObjectPreferencesDialog::PreLayoutDynInitL()
	{
//	//
//	// Change the dialog text as appropriate
//	if (!iObjectPreferenceDialog)
//		{
///		SetTitleL(R_WORD_STYLE_REVERT_TITLE);
//
///		}

	CEikButtonBase::TState button=CEikButtonBase::EClear;
	//
	// Sketch as glass
	button=(iSketchAsGlass) ? CEikButtonBase::EClear : CEikButtonBase::ESet;
	SetCheckBoxState(EWordCidShowSketch,button);
	// graph as glass
	button=(iGraphAsGlass) ? CEikButtonBase::EClear : CEikButtonBase::ESet;
	SetCheckBoxState(EWordCidShowGraph,button);
	}

void CWordObjectPreferencesDialog::WordSetCheckBoxState(CEikButtonBase::TState aState)
	{
	SetCheckBoxState(EWordCidShowSketch,aState);
	SetCheckBoxState(EWordCidShowGraph,aState);
	} 												  

TBool CWordObjectPreferencesDialog::OkToExitL(TInt /*aButtonId*/)
	{
	TBool visible;
	CEikButtonBase::TState button=CEikButtonBase::EClear;
	//
	// sketch as glass
	button=CheckBoxState(EWordCidShowSketch);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iSketchAsGlass=!visible;
	// graph as glass
	button=CheckBoxState(EWordCidShowGraph);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iGraphAsGlass=!visible;
	//
	return ETrue;
	}

//
//	CWordObjectPreferencesDialog & Revert To Style dialog
//

CWordRevertToStyleDialog::CWordRevertToStyleDialog(TBool& aSketchAsGlass,TBool& aGraphAsGlass)
	: iSketchAsGlass(aSketchAsGlass),iGraphAsGlass(aGraphAsGlass)
	{}

void CWordRevertToStyleDialog::PreLayoutDynInitL()
	{
//	//
//	// Change the dialog text as appropriate
//	if (!iObjectPreferenceDialog)
//		{
///		SetTitleL(R_WORD_STYLE_REVERT_TITLE);
//
///		}

	CEikButtonBase::TState button=CEikButtonBase::EClear;
	//
	// Sketch as glass
	button=(iSketchAsGlass) ? CEikButtonBase::ESet : CEikButtonBase::EClear;
	SetCheckBoxState(EWordCidRemoveChar,button);
	// graph as glass
	button=(iGraphAsGlass) ? CEikButtonBase::ESet : CEikButtonBase::EClear;
	SetCheckBoxState(EWordCidRemovePara,button);
	}

void CWordRevertToStyleDialog::WordSetCheckBoxState(CEikButtonBase::TState aState)
	{
	SetCheckBoxState(EWordCidRemoveChar,aState);
	SetCheckBoxState(EWordCidRemovePara,aState);
	} 												  

TBool CWordRevertToStyleDialog::OkToExitL(TInt /*aButtonId*/)
	{
	TBool visible;
	CEikButtonBase::TState button=CEikButtonBase::EClear;
	//
	// sketch as glass
	button=CheckBoxState(EWordCidRemoveChar);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iSketchAsGlass=visible;
	// graph as glass
	button=CheckBoxState(EWordCidRemovePara);
	visible=button? CEikButtonBase::ESet : CEikButtonBase::EClear; 
	iGraphAsGlass=visible;
	//
	return ETrue;
	}

//
// Manual zoom setting dialog
// 

CWordTempManualZoomDialog::CWordTempManualZoomDialog(TInt& aOrigZoomFactor)
	: iOrigZoomFactor(aOrigZoomFactor)
	{__ASSERT_DEBUG(iOrigZoomFactor>0,Panic(EWordZeroZoomFactor));}


void CWordTempManualZoomDialog::PreLayoutDynInitL()
	{
	// set the control with the current zoom factor
	__ASSERT_DEBUG(iOrigZoomFactor>0,Panic(EWordZeroZoomFactor));
	SetNumberEditorValue(EWordCidZoomSet,iOrigZoomFactor/10);
	}


TBool CWordTempManualZoomDialog::OkToExitL(TInt /*aButtonId*/)
	{
	iOrigZoomFactor=(NumberEditorValue(EWordCidZoomSet))*10;
	return ETrue;
	}


//
// Custom File Open dialog
//


CWordFileOpenDialog::CWordFileOpenDialog(TDes* aFileName,TBool& aReadOnly,TInt aTitleId)
	: CEikFileOpenDialog(aFileName,aTitleId),
	  iReadOnly(aReadOnly)
    {
	__DECLARE_NAME(_S("CWordFileOpenDialog"));
    }


void CWordFileOpenDialog::HandleControlStateChangeL(TInt)
	{
	}

TBool CWordFileOpenDialog::OkToExitL(TInt aButtonId)
    {
	if (CEikFileOpenDialog::OkToExitL(aButtonId))
		{
		iReadOnly=CheckBoxState(EWordCidReadOnly)==CEikButtonBase::ESet;
		return ETrue;
		}
	else return EFalse;
    }

//
// Custom File New dialog with templates
//

CWordTemplateNewFileDialog::CWordTemplateNewFileDialog(TDes* aFileName, TFileName& aTemplateFileName, TUidType aUid)
	: CEikFileSaveAsDialog(aFileName,NULL,R_EIK_TBUF_CREATE_NEW_FILE), iTemplateFileName(aTemplateFileName),
		iUid(aUid)
	{
	}

TBool CWordTemplateNewFileDialog::OkToExitL(TInt aButtonId)
	{
	if(!CEikFileSaveAsDialog::OkToExitL(aButtonId))
		return EFalse;
	// Check to see if the template is still there
	CEikUnifiedFileNameSelector* templateFileNameSel = (CEikUnifiedFileNameSelector*) (Control(EEikCtUnifiedFileSel));
	iTemplateFileName = templateFileNameSel->FullName();
	if(ConeUtils::FileExists(iTemplateFileName))
		return ETrue;
	else
		{
		iEikonEnv->InfoMsg(R_WORD_TEMPLATE_NOT_AVAILABLE);
		return EFalse;
		}
	}

void CWordTemplateNewFileDialog::PreLayoutDynInitL()
	{
	CEikFileSaveAsDialog::PreLayoutDynInitL();
	TFileName templateFileName;
	TWordFilePath::GetNormalTemplateFolderName(templateFileName);
	TFileName tempFileName;
	TFindFile ff(iEikonEnv->FsSession());

	iEikonEnv->ReadResource(tempFileName, R_WORD_NORMAL_TEMPLATE_FILE_NAME);
	templateFileName.Append(tempFileName);
	templateFileName[0] = 'Y';
	TParsePtr parse(templateFileName);
	TInt err = ff.FindByDir(parse.NameAndExt(),parse.DriveAndPath());
	if(err)
		{
		// no normal template, search ROM for blank
		TWordFilePath::GetNormalTemplateFolderName(templateFileName);
		iEikonEnv->ReadResource(tempFileName, R_WORD_BLANK_TEMPLATE_FILE_NAME);
		templateFileName.Append(tempFileName);
		templateFileName[0] = 'Z';
		TParsePtr blankParse(templateFileName);
		User::LeaveIfError(ff.FindByDir(blankParse.NameAndExt(),blankParse.DriveAndPath()));
		}
	templateFileName = ff.File();
	
	CEikUnifiedFileNameSelector* templateFileNameSel = (CEikUnifiedFileNameSelector*) (Control(EEikCtUnifiedFileSel));
	templateFileNameSel->SetUidType(iUid);
	templateFileNameSel->SetROMIncluded(ETrue);
	templateFileNameSel->SetFullNameL(templateFileName);	
	}

SEikControlInfo CWordTemplateNewFileDialog::CreateCustomControlL(TInt aControlType)
	{
	SEikControlInfo controlInfo;
	controlInfo.iControl=NULL;
	controlInfo.iTrailerTextId=0;
	controlInfo.iFlags=0;

	switch(aControlType)
		{
	case EWordUnifiedFileNameSelector:
		{
		CWordUnifiedFileSelector* newControl = new(ELeave) CWordUnifiedFileSelector();
		controlInfo.iControl = newControl;
		controlInfo.iFlags = EEikControlHasEars;
		break;
		}
	default:
		break;
		}
	return controlInfo;
	}

//
// class CWordUnifiedFileSelector file selector with sorting
// 

void CWordUnifiedFileSelector::SortArray(MDesCArray* aFileNames)
	{
	TFileName blank;
	iEikonEnv->ReadResource(blank, R_WORD_BLANK_TEMPLATE_FILE_NAME);
	TInt count = aFileNames->MdcaCount();
	for (TInt i = 0 ; i < count; i++)
		{
		if (aFileNames->MdcaPoint(i) == blank)
			{
			if (i == 0)
				break;
			CDesCArray* array = STATIC_CAST(CDesCArray*,aFileNames);
			TRAPD(err,array->InsertL(0, blank)); // top
			if (err == KErrNone)
				{
				array->Delete(++i);
				}
			break;
			}
		}
	}

void CWordUnifiedFileSelector::BuildFileNameArrayL(const TParse& aParse, MDesCArray* aFileNames)
	{
	CEikUnifiedFileNameSelector::BuildFileNameArrayL(aParse, aFileNames);
	SortArray(aFileNames);
	}

//
// class CWordDeleteTemplateDialog
//

CWordDeleteTemplateDialog::CWordDeleteTemplateDialog(TFileName& aTemplateFileName, TUidType aUid)
	: iTemplateFileName(aTemplateFileName), iUid(aUid)
	{
	}

TBool CWordDeleteTemplateDialog::OkToExitL(TInt /*aButtonId*/)
	{
	CEikUnifiedFileNameSelector* templateFileNameSel = STATIC_CAST(CEikUnifiedFileNameSelector*,(Control(EEikCtUnifiedFileSel)));
	iTemplateFileName = templateFileNameSel->FullName();
	
	if (!iTemplateFileName.Length())
		return ETrue;

	// Confirm this is what the user really wants
	TParsePtrC parse(iTemplateFileName);
	TFileName confirmPre;
	TBuf<30> confirmPost;
	iEikonEnv->ReadResource(confirmPre, R_WORD_TEMPLATE_DELETE_CONFIRMATION_PRE);
	iEikonEnv->ReadResource(confirmPost, R_WORD_TEMPLATE_DELETE_CONFIRMATION_POST);
	TInt gap = 60 - confirmPre.Length() - confirmPost.Length();
	if (parse.NameAndExt().Length() > gap)
		{
		TFileName name = parse.NameAndExt();
		confirmPre.Append(name.Left(gap - 1));
		confirmPre.Append((TChar)KEllipsis);
		}
	else
		confirmPre.Append(parse.NameAndExt());
	confirmPre.Append(confirmPost);
	confirmPost = _L("");
		
	if (!iEikonEnv->QueryWinL(confirmPre, confirmPost))
		iTemplateFileName = TFileName();
	return ETrue;
	}

void CWordDeleteTemplateDialog::PreLayoutDynInitL()
	{
	CEikUnifiedFileNameSelector* templateFileNameSel = STATIC_CAST(CEikUnifiedFileNameSelector*,(Control(EEikCtUnifiedFileSel)));
	templateFileNameSel->SetUidType(iUid);
	templateFileNameSel->SetROMIncluded(EFalse);
	templateFileNameSel->SetFullNameL(iTemplateFileName);
	}

//
// class CWordSaveAsTemplateDialog
//

CWordSaveAsTemplateDialog::CWordSaveAsTemplateDialog(TFileName& aTemplateFileName, TUidType aUid)
	: iTemplateFileName(aTemplateFileName), iUid(aUid)
	{
	}

TBool CWordSaveAsTemplateDialog::OkToExitL(TInt /*aButtonId*/)
	{
	CEikFileNameEditor* templateFileNameEd = STATIC_CAST(CEikFileNameEditor*,(Control(EEikCidFileNameEd)));
	CEikDriveNameSelector *driveNameSelector = STATIC_CAST(CEikDriveNameSelector*,(Control(EEikCidDriveNameSel)));
	templateFileNameEd->GetFullNameL(iTemplateFileName);

	TFileName drivename = driveNameSelector->FullName();
	TParse parse;
	User::LeaveIfError(parse.Set(drivename, &iTemplateFileName,NULL));
	iTemplateFileName = parse.FullName();
	TBool doNotReplaceExistingFile = EFalse;
	TUint attributes = 0;
	if (ConeUtils::FileExists(iTemplateFileName))
		{
		TBuf<32> infoDialogTitle;
		iEikonEnv->ReadResource(infoDialogTitle, R_EIK_TBUF_DIALOG_TITLE_CONFIRM_FILE_REPLACE);
		TBuf<KShortErrorMessageLength> formatStr;
		User::LeaveIfError(iEikonEnv->FsSession().Att(iTemplateFileName, attributes));
		if (attributes & KEntryAttReadOnly)
			iEikonEnv->LeaveWithInfoMsg(R_EIK_TBUF_CANNOT_REPLACE_READONLY_FILE);
		else
			iEikonEnv->ReadResource(formatStr, R_EIK_TBUF_FILE_REPLACE_CONFIRM1);
		TBuf<KMaxDisplayedFullNameLen> abbrevName;
		User::LeaveIfError(EikFileUtils::Parse(iTemplateFileName));
		TParsePtr parse(iTemplateFileName);
		EikFileUtils::AbbreviateFileName(parse.NameAndExt(), abbrevName);
		TBuf<KShortErrorMessageLength + KMaxDisplayedFullNameLen> textMsg;
		textMsg.Format(formatStr, &abbrevName);
		CEikDialog* infoDialog = new(ELeave) CEikInfoDialog(infoDialogTitle, textMsg,CEikInfoDialog::EIgnoreEnter);
		if (!(infoDialog->ExecuteLD(R_EIK_DIALOG_SINGLE_FILE_REPLACE)))
			doNotReplaceExistingFile = ETrue;
		}
	return !doNotReplaceExistingFile;
	}

void CWordSaveAsTemplateDialog::PreLayoutDynInitL()
	{
	TFileName templateFileName;
	TWordFilePath::GetNormalTemplateFolderName(templateFileName);

	CEikFileNameEditor* templateFileNameEd = STATIC_CAST(CEikFileNameEditor*,(Control(EEikCidFileNameEd)));
	CEikDriveNameSelector *driveNameSelector = STATIC_CAST(CEikDriveNameSelector*,(Control(EEikCidDriveNameSel)));
	
	templateFileNameEd->SetFullNameL(templateFileName);
	
	// set drive to be system drive
	templateFileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	
	driveNameSelector->ShowRom(EFalse);
	driveNameSelector->SetFullNameL(templateFileName);
	}

// Outline Dialog

CWordOutlineDialog::CWordOutlineDialog(CRichText* aText, CEikEdwin* aEdWin, TInt& aNewCursorPosition)
	:iText(aText), 
	iEdWin(aEdWin),
	iNewCursorPosition(aNewCursorPosition)
{
}



TBool CWordOutlineDialog::OkToExitL(TInt aButtonId)
	{
	switch(aButtonId)
		{
	case EEikBidOk:
		iNewCursorPosition = iOutlineNav->OutlineModel()->CurrentCharPosition();
		return ETrue;
	case EWordBidOpenAll:
		iOutlineNav->ExpandAllItemsL();
		return EFalse;
	case EWordBidCloseAll:
		iOutlineNav->CollapseAllItemsL();
		return EFalse;
	default:
		return EFalse;
		}
	}


SEikControlInfo CWordOutlineDialog::CreateCustomControlL(TInt aControlType)
	{
	SEikControlInfo controlInfo;
	controlInfo.iControl=NULL;
	controlInfo.iTrailerTextId=0;
	controlInfo.iFlags=0;

	COutlineHListBox* newControl;

	switch(aControlType)
		{
	case EWordCtOutlineNav:
		newControl = new(ELeave)COutlineHListBox(iText,iEdWin->CursorPos());
		controlInfo.iControl = newControl;
		iOutlineNav = newControl;
		break;
	default:
		break;
		}
	return controlInfo;
	}
