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

#include <prnsetup.h>

#include <eikenv.h>
#include <techview/eiklabel.h>
#include <techview/eikimage.h>
#include <techview/eikrted.h>
#include <techview/eiktbar.h>
#include <techview/eikcmbut.h>
#include <techview/eikfnlab.h>
#include <eikon.mbg>
#include <techview/eikbtgpc.h>
#include <eikfutil.h>

#include "WPAPPUI.H"
#include "WPSTYLE.H"
#include "WPDEF.H"
#include "WORD.HRH"
#include <word.rsg>
#include <word.mbg>
#include "WPPANIC.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "txtfmlyr_internal.h"
#endif

void CWordAppUi::UpdateToolBarAndBandL(TInt aCommand)
	{
	__ASSERT_ALWAYS(aCommand == EWordCmdToolband || aCommand == EWordCmdToolbar, Panic(EInvalidCommandParameter));
	
	TBool getsHidden = EFalse;
	if(aCommand == EWordCmdToolband)
		{
		getsHidden = !ToggleToolbarControlL(*iEikonEnv->AppUiFactory()->ToolBand());
		}
	else
		{
		iEikonEnv->AppUiFactory()->ToolBar()->SetComponentsToInheritVisibility(ETrue);
		getsHidden = iEikonEnv->AppUiFactory()->ToolBar()->IsVisible();
		iEikonEnv->AppUiFactory()->ToolBar()->MakeVisible(!getsHidden);
		}

	if (getsHidden)
		{
		if (aCommand == EWordCmdToolbar)
			iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordCmdFileName)->DrawNow();  // redraw the filename label only
		else
			{
			TRect bottomBorder = iRichEd->Rect();
			bottomBorder.iTl.iY = bottomBorder.iBr.iY - 1;
			STATIC_CAST(RWindow*, iRichEd->DrawableWindow())->Invalidate(bottomBorder);
			}
		}
	TInt adjacent = EGulAdjNone;
	if (iEikonEnv->AppUiFactory()->ToolBar()->IsVisible())
		adjacent |= EGulAdjRight;
	if (iEikonEnv->AppUiFactory()->ToolBand()->IsVisible())
		adjacent |= EGulAdjTop;
	iRichEd->SetAdjacent(adjacent);
	iRichEd->SetRect(ClientRect());
	iRichEd->ForceScrollBarUpdateL();
	iRichEd->DrawNow();
	}


TBool CWordAppUi::ToggleToolbarControlL(CEikToolBar& aToolBar)
// Toggles the visibility of the specified toolbar conttrol.
// Returns ETrue if the toolbar becomes visible as a result of this action,
// otherwise returns EFalse.
//
	{
	const TBool visible = aToolBar.IsVisible();
	if (!visible)
		{// Update buttons if the toolbar is about to appear
		UpdateToolbandButtonsL(EVisibilityIndependant);
		}
	aToolBar.MakeVisible(!visible);
	return !visible;
	}

void CWordAppUi::UpdateToolbandButtonsL(TToolBarUpdate aUpdate)
	{
	if (!aUpdate && !iEikonEnv->AppUiFactory()->ToolBand()->IsVisible())
		return;
	UpdateCharFormatButtonsL();
	UpdateParaFormatButtonsL();
	}


void CWordAppUi::UpdateCharFormatButtonsL()
	{
	TCharFormat format;
	TCharFormatMask mask;
	const TCursorSelection selection=iRichEd->Selection();
	Text()->GetCharFormat(format,mask,selection.LowerPos(),selection.Length());
	//
	UpdateToolBandButton(EEikCmdFontBold,format.iFontSpec.iFontStyle.StrokeWeight()==EStrokeWeightBold? CEikButtonBase::ESet : CEikButtonBase::EClear);
	UpdateToolBandButton(EEikCmdFontItalic,format.iFontSpec.iFontStyle.Posture()==EPostureItalic? CEikButtonBase::ESet : CEikButtonBase::EClear);
	UpdateToolBandButton(EEikCmdFontUnderline,format.iFontPresentation.iUnderline==EUnderlineOn? CEikButtonBase::ESet : CEikButtonBase::EClear);
	//
	UpdateToolBandFontNameButtonL(format.iFontSpec.iTypeface.iName);
	UpdateToolBandFontHeightButtonL(format.iFontSpec.iHeight);
	}


void CWordAppUi::UpdateToolBandButton(TInt aCommand,CEikButtonBase::TState aNewState)
// Sets the state of the specified latching button in the toolband.
//
	{
	CEikButtonBase* button = STATIC_CAST(CEikButtonBase*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(aCommand));
	if (button->State() != aNewState)
		{
		button->SetState(aNewState);
		button->DrawNow();
		}
	}

void CWordAppUi::UpdateParaFormatButtonsL()
	{
	RTmParFormatLayer format;
	TInt run_length;
	iRichEd->GetParFormatL(iRichEd->CursorPos(),MUnifiedEditor::EEffective,format,run_length);
	RTmParFormat::TAlignment alignment = format.iFormat.iAlignment;
	TBool have_bullet = format.iFormat.Bullet() != NULL;
	TBool have_borders = format.iFormat.HaveBorders();
	format.Close();

	UpdateStyleButtonL();
	UpdateAlignButtonL(alignment);
	UpdateToolBandButton(EWordButtonBullets,have_bullet ? CEikButtonBase::ESet : CEikButtonBase::EClear);
	UpdateBorderButtonL(have_borders);
	}

void CWordAppUi::UpdateToolBandFontNameButtonL(TDesC& aNewFontName)
// 
	{
	TBuf<KMaxTypefaceNameLength> typefaceName;
	typefaceName=aNewFontName;
	UpdateToolBandFontNameButtonL(typefaceName);
	}

void CWordAppUi::UpdateToolBandFontNameButtonL(TDes& aNewFontName)
// Only change the label text if the current font is different from that specified.
//
	{
	CEikCommandButton* button = STATIC_CAST(CEikCommandButton*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonFont));
	CEikLabel* buttonLabel=button->Label();
	TInt maxWidthInPixels=buttonLabel->Size().iWidth;
	TextUtils::ClipToFit(aNewFontName,*iEikonEnv->LegendFont(),maxWidthInPixels,KClippedTextCharacter);
	if (aNewFontName!=*buttonLabel->Text())
		{
		buttonLabel->SetTextL(aNewFontName);
		buttonLabel->DrawNow();
		}
	}

void CWordAppUi::UpdateToolBandFontHeightButtonL(const TInt aNewFontHeight)
// Only change the label text if necessary
//
	{
	const TInt points = FontUtils::PointsFromTwips(aNewFontHeight);
	TBuf<16> noPoints;
	noPoints.Num(points);    
	TBuf<4> temp;
	iCoeEnv->ReadResource(temp, R_WORD_FONT_UNITS);
	noPoints.Append(temp);
	CEikCommandButton* button = STATIC_CAST(CEikCommandButton*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonFontSize));
	CEikLabel* buttonLabel = button->Label();
	if (*buttonLabel->Text() != noPoints)
		{
		buttonLabel->SetTextL(noPoints);
		buttonLabel->DrawNow();
		}
	}

void CWordAppUi::UpdateStyleButtonL()
// Update the label on the style name button, but only if, after clipping,
// the text is different to the current setting.
//
	{
	const TCursorSelection selection = iRichEd->Selection();
	TBool styleChange;
	const CParaFormatLayer* style = Text()->ParagraphStyle(styleChange, selection.LowerPos(), selection.Length());
	TUid type = style->Type();
	TParagraphStyleName name;
	if (type != KNormalParagraphStyleUid)
		name = ((CParagraphStyle*)style)->iName;
	else
		iCoeEnv->ReadResource(name, R_WORD_NORMAL);
	CEikCommandButton* button = STATIC_CAST(CEikCommandButton*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonStyle));
	CEikLabel* buttonLabel = button->Label();
	TInt maxWidthInPixels = buttonLabel->Size().iWidth;
	TextUtils::ClipToFit(name, *iEikonEnv->LegendFont(), maxWidthInPixels, KClippedTextCharacter);
	if (*buttonLabel->Text() != name)
		{
		buttonLabel->SetTextL(name);
		buttonLabel->DrawNow();
		}
	}

void CWordAppUi::UpdateAlignButtonL(RTmParFormat::TAlignment aNewAlignment)
// Update the alignment button.
//
	{
	if (iAlignment == aNewAlignment)
		return;
	CEikBitmapButton* align = STATIC_CAST(CEikBitmapButton*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonAlign));
	switch(aNewAlignment)
		{
		case RTmParFormat::EAlignCenter:
			align->SetPictureFromFileL(TPtrC(), EMbmEikonCenta, EMbmEikonCenta, CEikCommandButtonBase::EFirst);
			break;
		case RTmParFormat::EAlignJustify:
			align->SetPictureFromFileL(TPtrC(), EMbmEikonJusta, EMbmEikonJusta, CEikCommandButtonBase::EFirst);
			break;
		case RTmParFormat::EAlignReverse:
			align->SetPictureFromFileL(TPtrC(), EMbmEikonRighta, EMbmEikonRighta, CEikCommandButtonBase::EFirst);
			break;
		case RTmParFormat::EAlignNormal:
		default:
			align->SetPictureFromFileL(TPtrC(), EMbmEikonLefta, EMbmEikonLefta, CEikCommandButtonBase::EFirst);
			break;
		}
	iAlignment = aNewAlignment;
	align->Picture()->DrawNow();
	}


void CWordAppUi::UpdateBorderButtonL(TBool aBordersPresent)
// Changes the border button, but only if the desired state is different from the current state.
//
	{
	CEikBitmapButton* button = STATIC_CAST(CEikBitmapButton*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordButtonBorders));
	if (aBordersPresent != iBorderPresent)
		{// Change the button
		if (aBordersPresent)
			button->SetPictureFromFileL(BitmapStore(), EMbmWordBorderf, EMbmWordBorderfm, CEikCommandButtonBase::EFirst);
		else
			button->SetPictureFromFileL(BitmapStore(), EMbmWordBordern, EMbmWordBordernm, CEikCommandButtonBase::EFirst);
		iBorderPresent = aBordersPresent;
		button->Picture()->DrawNow();
		}
	}


void CWordAppUi::UpdateFileNameLabelL()
// Update the label text on both the toolbar and toolband filename controls
//
	{
	CEikFileNameLabel* label = STATIC_CAST(CEikFileNameLabel*, iEikonEnv->AppUiFactory()->ToolBar()->ControlOrNull(EWordCmdFileName));
	if(label != NULL)
		{
		label->UpdateL();
		label->DrawNow();
		}
	label = STATIC_CAST(CEikFileNameLabel*, iEikonEnv->AppUiFactory()->ToolBand()->ControlById(EWordCmdFileName));
	if (!iContainerAppUi)
		label->UpdateL();
	label->DrawNow();
	}


//
// class CWordToolBarUpdate
//

CWordToolBarUpdate* CWordToolBarUpdate::NewL(CWordAppUi& aAppUi)
// Create a new idle object.
	{
	CWordToolBarUpdate* self=new(ELeave) CWordToolBarUpdate(aAppUi);
	CActiveScheduler::Add(self);
	return(self);
	}

CWordToolBarUpdate::CWordToolBarUpdate(CWordAppUi& aAppUi)
// Constructor.
	: CActive(EActivePriorityDefault), iAppUi(aAppUi)
	{
	iCount=(TInt)(CWordAppUi::EStyleName);
	}

CWordToolBarUpdate::~CWordToolBarUpdate()
// Destructor.
	{
	Cancel();
	}

void CWordToolBarUpdate::Start(TUpdateType aUpdateType)
// Start the object.
	{
	if (aUpdateType==EFullUpdate)
		{
		iCount=(TInt)(CWordAppUi::EStyleName);
		iUpdateType=EFullUpdate;
		}
	else
		{
		if (iUpdateType==ECharFormatUpdate)
			iCount=(TInt)(CWordAppUi::EFontName);
		else
			{
			if (iCount>(TInt)(CWordAppUi::EBorder))
				{
				iCount=(TInt)(CWordAppUi::EFontName);
				iUpdateType=ECharFormatUpdate;
				}
			else
				iCount=Min((TInt)(CWordAppUi::EFontName),iCount);
			}
		}
	if (IsActive())
		return;
	TRequestStatus* pS=(&iStatus);
	User::RequestComplete(pS,0);
	SetActive();
	}

void CWordToolBarUpdate::RunL()
// Called when nothing of a higher priority can be scheduled.
	{
	if (!STATIC_CAST(CEikonEnv*, iAppUi.iCoeEnv)->AppUiFactory()->ToolBand()->IsVisible())
		return;
	switch(iCount)
		{
	case CWordAppUi::EStyleName:
		iAppUi.UpdateStyleButtonL();
		break;
	case CWordAppUi::EFontName:
		{
		//
		// Get the current character format
		TCharFormat format;
		TCharFormatMask mask;
		const TCursorSelection selection=iAppUi.iRichEd->Selection();
		iAppUi.Text()->GetCharFormat(format,mask,selection.LowerPos(),selection.Length());
		//
		// Store values for later use
		iFontHeightInTwips=format.iFontSpec.iHeight;
		iStrokeWeight=format.iFontSpec.iFontStyle.StrokeWeight();
		iPosture=format.iFontSpec.iFontStyle.Posture();
		iUnderline=format.iFontPresentation.iUnderline;
		//
		// Update the font name button
		TBuf<KMaxTypefaceNameLength> typefaceName;
		typefaceName=format.iFontSpec.iTypeface.iName;
		iAppUi.UpdateToolBandFontNameButtonL(typefaceName);
		break;
		}
	case CWordAppUi::EFontHeight:
		iAppUi.UpdateToolBandFontHeightButtonL(iFontHeightInTwips);
		break;
	case CWordAppUi::EBold:
		iAppUi.UpdateToolBandButton(EEikCmdFontBold,iStrokeWeight==EStrokeWeightBold? CEikButtonBase::ESet : CEikButtonBase::EClear);
		break;
	case CWordAppUi::EItalic:
		iAppUi.UpdateToolBandButton(EEikCmdFontItalic,iPosture==EPostureItalic? CEikButtonBase::ESet : CEikButtonBase::EClear);
		break;
	case CWordAppUi::EUnderline:
		iAppUi.UpdateToolBandButton(EEikCmdFontUnderline,iUnderline==EUnderlineOn? CEikButtonBase::ESet : CEikButtonBase::EClear);
		break;
	case CWordAppUi::EAlignment:
		{
		RTmParFormatLayer format;
		TInt run_length;
		iAppUi.iRichEd->GetParFormatL(iAppUi.iRichEd->CursorPos(),MUnifiedEditor::EEffective,format,run_length);
		RTmParFormat::TAlignment alignment = format.iFormat.iAlignment;
		iBulletPresent = format.iFormat.Bullet() != NULL;
		iBordersPresent = format.iFormat.HaveBorders();
		format.Close();
		iAppUi.UpdateAlignButtonL(alignment);
		break;
		}
	case CWordAppUi::EBullet:
		iAppUi.UpdateToolBandButton(EWordButtonBullets,iBulletPresent ? CEikButtonBase::ESet : CEikButtonBase::EClear);
		break;
	case CWordAppUi::EBorder:
		iAppUi.UpdateBorderButtonL(iBordersPresent);
		break;
		};
	++iCount;
	if ((iUpdateType==EFullUpdate && iCount<=(TInt)(CWordAppUi::EBorder)) ||
		(iUpdateType==ECharFormatUpdate && iCount<=(TInt)(CWordAppUi::EUnderline)))
		{
		TRequestStatus* pS=(&iStatus);
		User::RequestComplete(pS,0);
		SetActive();
		}
	}

void CWordToolBarUpdate::DoCancel()
// Cancel
	{
	}
