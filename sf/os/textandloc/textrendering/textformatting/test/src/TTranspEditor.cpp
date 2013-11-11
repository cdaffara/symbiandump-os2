/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TTranspEditor test source file. A base set of classes, neeeded for transparent editors
* functionality testing, is defined here.
* It is an "APP" test, where you can run TTranspEditor test application and check how the 
* trransparency/opaque drawing works. Currently you may see the effect of opaque drawing
* applied on texts, pictures, selections.
*
*/


#include <coecntrl.h>
#include <coeccntx.h>
#include <coemain.h>
#include <eikon.hrh>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikenv.h>
#include <eikdef.h>
#include <txtrich.h>
#include <frmtview.h>
#include <eikstart.h>
#include "TTranspEditor.h"
#include "TTranspEditor.hrh"
#include <ttranspeditor.rsg>

const TUid KAppUid = {0x13579ACE};

////////////////////////////////////////////////////////////////////////////////////////////
//Picture
//CTestPicture's instances can be inserted and displayed into the transparent text view,
//used in the test.

CTestPicture* CTestPicture::NewL()
	{
	CTestPicture* self = new (ELeave) CTestPicture;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestPicture::~CTestPicture()
	{
	delete iBitmap;
	}

CTestPicture::CTestPicture()
	{
	}

void CTestPicture::ConstructL()
	{
	_LIT(KDataMbmFile, "z:\\system\\data\\TTrEdData.mbm");
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Load(KDataMbmFile, 0));	
	}

void CTestPicture::Draw(CGraphicsContext& aGc, const TPoint&, const TRect& aRc, MGraphicsDeviceMap*) const
	{
	aGc.DrawBitmap(aRc, iBitmap, aRc);
	}

void CTestPicture::ExternalizeL(RWriteStream&) const
	{
	}

void CTestPicture::GetOriginalSizeInTwips(TSize& aSize) const
	{
	aSize = iBitmap->SizeInTwips();
	}

TInt CTestPicture::ScaleFactorWidth() const
	{
	return CPicture::ScaleFactorWidth() * 6;
	}

TInt CTestPicture::ScaleFactorHeight() const
	{
	return CPicture::ScaleFactorHeight() * 6;
	}

////////////////////////////////////////////////////////////////////////////////////////////
//Application

CApaDocument* CTranspEditorApp::CreateDocumentL()
	{
	return new (ELeave) CTranspEditorDoc(*this);
	}

TUid CTranspEditorApp::AppDllUid() const
	{
	return KAppUid;
	}

////////////////////////////////////////////////////////////////////////////////////////////
//View1
//It is used to create and display a background bitmap, needed for asserting opaque drawing
//functionality.

CTranspEditorView1* CTranspEditorView1::NewL()
	{
	CTranspEditorView1* self = new (ELeave) CTranspEditorView1;
	CleanupStack::PushL(self);
	self->ContructL();
	CleanupStack::Pop(self);
	return self;
	}

CTranspEditorView1::~CTranspEditorView1()
	{
	delete iBitmap;
	}

void CTranspEditorView1::ContructL()
	{
	_LIT(KDilbertMbmFile, "z:\\system\\data\\TTrEdDilbert.mbm");
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Load(KDilbertMbmFile, 0));

	CreateWindowL();
	TSize size = iEikonEnv->ScreenDevice()->SizeInPixels();
	const TRect KViewRect(size);
	SetRect(KViewRect);
	ActivateL();
	}

void CTranspEditorView1::Draw(const TRect&) const
	{
	CWindowGc& gc = SystemGc();
	TRect rc = Rect();
	TRect rc2(rc.iTl.iX + 1, rc.iTl.iY + 1, rc.iBr.iX - 1, rc.iBr.iY - 1);

	gc.Clear();

	TRgb cl(0xFF, 0x50, 0x00);
	gc.SetPenColor(cl);
	gc.DrawRect(rc);

	gc.DrawBitmap(rc2, iBitmap);
	}

/////////////////////////////////////////////////////////////////////////////////////////////
//View2
//Transparent text view.

CTranspEditorView2* CTranspEditorView2::NewL()
	{
	CTranspEditorView2* self = new (ELeave) CTranspEditorView2;
	CleanupStack::PushL(self);
	self->ContructL();
	CleanupStack::Pop(self);
	return self;
	}

CTranspEditorView2::~CTranspEditorView2()
	{
	delete iTextView;
	delete iLayout;
	delete iRichText;
	}

//Correspond to EAppCmdInsertText user interface command.
//Insert a text chunk into the transparent text view.
void CTranspEditorView2::InsertTextL()
	{
	_LIT(KText, "ABC123");
	iRichText->InsertL(iRichText->DocumentLength(), KText);

	TViewYPosQualifier a1;
	a1.SetFillScreen();
	a1.SetMakeLineFullyVisible();
	iTextView->HandleGlobalChangeL(a1);
	}

//Correspond to EAppCmdSwitchOpaque user interface command.
//Switch on/off opaque drawing mode.
void CTranspEditorView2::SwitchOpaque()
	{
	iOpaque = !iOpaque;
	iTextView->SetOpaque(iOpaque);
	DrawNow();
	}

//Correspond to EAppCmdSwitchSelect user interface command.
//Switch on/off text selection.
void CTranspEditorView2::SwitchSelectL()
	{
	iSelect = !iSelect;
	if(iSelect)
		{
		TTmDocPosSpec docPosSpec(10, TTmDocPosSpec::ELeftToRight);
		TTmPosInfo2 posInfo;
		TBool res = iTextView->FindDocPosL(docPosSpec, posInfo);
		if(res)
			{
			TCursorSelection sel(posInfo.iDocPos.iPos, 0);
			iTextView->SetSelectionL(sel);
			DrawNow();
			}
		}
	else
		{
		iTextView->ClearSelectionL();
		DrawNow();
		}
	}

//Correspond to EAppCmdInsertPicture user interface command.
//Create and insert a picture into the transparent text view.
void CTranspEditorView2::InsertPictureL()
	{
	CTestPicture* pic = CTestPicture::NewL();
	CleanupStack::PushL(pic);

	TSize size;
	pic->GetOriginalSizeInTwips(size);

	TPictureHeader pictheader;
	pictheader.iSize = size;
	pictheader.iPicture = pic;

	iRichText->InsertL(iRichText->DocumentLength(), pictheader);
	CleanupStack::Pop(pic);

	TViewYPosQualifier a1;
	a1.SetFillScreen();
	a1.SetMakeLineFullyVisible();
	iTextView->HandleGlobalChangeL(a1);
	}

//Correspond to EAppCmdSetCharFormat user interface command.
//Change the format of the selected text.
void CTranspEditorView2::SetCharFormatL()
	{
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormat.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
	charFormatMask.SetAttrib(EAttFontPosture);
	TCursorSelection select = iTextView->Selection();
	if(select.Length() != 0)
		{
		iRichText->ApplyCharFormatL(charFormat, charFormatMask, select.LowerPos(), select.Length());
		iTextView->HandleRangeFormatChangeL(select);
		}
	else
		{
		iRichText->SetInsertCharFormatL(charFormat, charFormatMask, select.iCursorPos);
		}
	}

//Construct a transparent text view, which is displayed on top of the background bitmap, 
//which makes the transparency effect very well visible.
void CTranspEditorView2::ContructL()
	{
	CreateWindowL(); 
	TSize size = iEikonEnv->ScreenDevice()->SizeInPixels();
	const TRect KViewRect(15, 15, size.iWidth - 15, size.iHeight - 15);
	SetRect(KViewRect);

	iRichText = CRichText::NewL(iEikonEnv->SystemParaFormatLayerL(), iEikonEnv->SystemCharFormatLayerL());
	iLayout = CTextLayout::NewL(iRichText, KViewRect.Width() - 2);

	TRect rc(KViewRect);
	rc.Shrink(1, 1);
	rc.Move(-rc.iTl.iX + 1, -rc.iTl.iY + 1);
	iTextView = CTextView::NewL(iLayout, 
								rc, 
								iEikonEnv->ScreenDevice(),
								iEikonEnv->ScreenDevice(),
								&Window(), 
								&iEikonEnv->RootWin(), 
								&iEikonEnv->WsSession());

	iTextView->EnablePictureFrameL(ETrue);
	const TRgb KTransparencyColor(85, 85, 85);
	Window().SetTransparencyFactor(KTransparencyColor);

	ActivateL();
	}

void CTranspEditorView2::Draw(const TRect&) const
	{
	CWindowGc& gc = SystemGc();
	TRect rc = Rect();
	TRect rc2(rc.iTl.iX + 1, rc.iTl.iY + 1, rc.iBr.iX - 1, rc.iBr.iY - 1);

	gc.Clear();

	const TRgb KPenColor(0x00, 0x00, 0xFF);
	gc.SetPenColor(KPenColor);
	gc.DrawRect(rc);

	TRAPD(err, iTextView->DrawL(rc2));
    if(err != KErrNone)
		{
		SystemGc().Clear(rc2);
		iEikonEnv->NotifyIdleErrorWhileRedrawing(err);
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////
//Document

CTranspEditorDoc::CTranspEditorDoc(CEikApplication& aApp) :
	CEikDocument(aApp)
	{
	}

CEikAppUi* CTranspEditorDoc::CreateAppUiL()
	{
	return new (ELeave) CTranspEditorUi;
	}

////////////////////////////////////////////////////////////////////////////////////////////
//UI

void CTranspEditorUi::ConstructL()
	{
	BaseConstructL();

	iTranspEditorView1 = CTranspEditorView1::NewL();
	AddToStackL(iTranspEditorView1);

	iTranspEditorView2 = CTranspEditorView2::NewL();
	AddToStackL(iTranspEditorView2);
	}

CTranspEditorUi::~CTranspEditorUi()
	{
	RemoveFromStack(iTranspEditorView2);
	delete iTranspEditorView2;

	RemoveFromStack(iTranspEditorView1);
	delete iTranspEditorView1;
	}

void CTranspEditorUi::HandleCommandL(TInt aCommand)
	{
	switch(aCommand)
		{
		case EAppCmdExit:
			CBaActiveScheduler::Exit();
			break;
		case EAppCmdInsertText:
			iTranspEditorView2->InsertTextL();
			break;
		case EAppCmdSwitchOpaque:
			iTranspEditorView2->SwitchOpaque();
			break;
		case EAppCmdSwitchSelect:
			iTranspEditorView2->SwitchSelectL();
			break;
		case EAppCmdInsertPicture:
			iTranspEditorView2->InsertPictureL();
			break;
		case EAppCmdSetCharFormat:
			iTranspEditorView2->SetCharFormatL();
			break;
		default:
			break;
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////
//

	static CApaApplication* NewApplication()
		{
		return new CTranspEditorApp;
		}

	TInt E32Main()
		{
		return EikStart::RunApplication(&NewApplication);
		}


