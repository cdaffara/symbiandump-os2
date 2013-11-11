/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TCustomWrap.h"
#include <basched.h>
#include <coecntrl.h>
#include <coeccntx.h>
#include <coemain.h>
#include <e32keys.h>
#include <eikon.hrh>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikenv.h>
#include <eikrted.h>
#include <eikedwin.h>
#include <eikdef.h>
#include <eikstart.h>
#include <txtrich.h>
#include "TCustomWrap.hrh"
#include <tcustomwrap.rsg>

// 
// ---------------- CCustomWrapAppControl  (a simple Edwin control) -----------

class CCustomWrapAppControl : public CCoeControl, public MCoeControlObserver
	{
public:
	~CCustomWrapAppControl();
	void ConstructL();
	void SetCustomWrapOnL();
	void SetCustomWrapCustomL();
	void SetCustomWrapOffL();
	void ReformatL();

private: // framework
	void Draw(const TRect& aRect) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType); // from the MCoeControlObserverInterface
//	void ActivateL();
	virtual TInt CountComponentControls() const;
	virtual CCoeControl* ComponentControl(TInt aIndex) const;    
	// the two functions above are needed in order for the control to work correctly.


private:
	void CreateEdwinL();
	

private:
	CCoeControl* iFocusControl; // The control that is currently in focus. 
	CEikEdwin* iEdwin; // I still need a focus control, even if just one EikEdwin...
	const MFormCustomWrap* iStandardCustomWrap;
	TTestCustomWrap iCustomCustomWrap;
	};

CCustomWrapAppControl::~CCustomWrapAppControl()
	{
	delete iEdwin;
	}

void CCustomWrapAppControl::ConstructL()
	{
	CreateWindowL();
    Window().SetShadowDisabled(ETrue);
	CreateEdwinL();
	iStandardCustomWrap = iEdwin->TextLayout()->CustomWrap();
	iFocusControl=iEdwin;
	SetExtentToWholeScreen();
	ActivateL();
	iFocusControl->SetFocus(ETrue);
	}

//void CCustomWrapAppControl::ActivateL()
//	{
//	CCoeControl::ActivateL();
//	iEdwin->ActivateL();
//	}

void CCustomWrapAppControl::HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
	{
	if (aEventType==EEventRequestFocus)
		{
		iFocusControl=aControl;
		iFocusControl->SetFocus(ETrue,EDrawNow);
		}
	} 

TInt CCustomWrapAppControl::CountComponentControls() const
	{
	return 1;
	}

CCoeControl* CCustomWrapAppControl::ComponentControl(TInt /*aIndex*/) const
	{
	return iEdwin;
	}

void CCustomWrapAppControl::CreateEdwinL()
	{
	const TSize screenSize(iCoeEnv->ScreenDevice()->SizeInPixels());
	// Rich text editor.
	iEdwin=new(ELeave) CEikRichTextEditor;
	STATIC_CAST(CEikRichTextEditor*,iEdwin)->ConstructL(this,0,0,0);
	//STATIC_CAST(CEikRichTextEditor*,iEdwin)->ConstructL(this,0,0,EEikEdwinInclusiveSizeFixed);
	iEdwin->SetObserver(this);
	iEdwin->CreateScrollBarFrameL();
	iEdwin->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn);
	iEdwin->SetExtent(TPoint(screenSize.iWidth/5,screenSize.iHeight/8),TSize(screenSize.iWidth/3,screenSize.iHeight*3/5));
	}

TKeyResponse CCustomWrapAppControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType==EEventKey)
		{
		iEdwin->OfferKeyEventL(aKeyEvent, aType);
		}
	return(EKeyWasConsumed);
	}

void CCustomWrapAppControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc=SystemGc();
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbWhite);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.DrawRect(aRect);
	}


void CCustomWrapAppControl::SetCustomWrapOnL()
	{
	iEdwin->TextLayout()->SetCustomWrap(iStandardCustomWrap);
	ReformatL();
	}

void CCustomWrapAppControl::SetCustomWrapCustomL()
	{
	iEdwin->TextLayout()->SetCustomWrap(&iCustomCustomWrap);
	ReformatL();
	}

void CCustomWrapAppControl::SetCustomWrapOffL()
	{
	iEdwin->TextLayout()->SetCustomWrap(0);
	ReformatL();
	}

void CCustomWrapAppControl::ReformatL()
	{
	iEdwin->NotifyNewFormatL();
	}

// 
//  ---------------------- CCustomWrapAppView definition ---------------- 
//

class CCustomWrapAppUi : public CEikAppUi
	{
public:
	void ConstructL();
	~CCustomWrapAppUi();
private: // from CEikAppUi -- framework
	void HandleCommandL(TInt aCommand);
private: 
	CCustomWrapAppControl* iCustomWrapAppControl;
	
	};

void CCustomWrapAppUi::ConstructL()
	{
	BaseConstructL();
	iCustomWrapAppControl=new(ELeave) CCustomWrapAppControl;
	iCustomWrapAppControl->ConstructL();
	AddToStackL(iCustomWrapAppControl);
	}

CCustomWrapAppUi::~CCustomWrapAppUi()
	{
	RemoveFromStack(iCustomWrapAppControl);
	delete iCustomWrapAppControl;
	}

void CCustomWrapAppUi::HandleCommandL(TInt aCommand)
	{
	switch(aCommand)
		{
		case EAppCmdExit:
			CBaActiveScheduler::Exit();
			break;
		case EAppCmdWrap0:
			iCustomWrapAppControl->SetCustomWrapOffL();
			break;
		case EAppCmdWrap1:
			iCustomWrapAppControl->SetCustomWrapOnL();
			break;
		case EAppCmdWrap2:
			iCustomWrapAppControl->SetCustomWrapCustomL();
			break;
		default:
			break;
		}
	} 

//
//  --------------------- CCustomWrapAppDoc class Definition ------------ 
//

class CCustomWrapAppDoc : public CEikDocument
	{
public:
	CCustomWrapAppDoc(CEikApplication& aApp);
private:
	CEikAppUi* CreateAppUiL();
	};


CCustomWrapAppDoc::CCustomWrapAppDoc(CEikApplication& aApp):CEikDocument(aApp)
	{
	// Nothing else to do, just call the base class constructor
	//
	}

CEikAppUi* CCustomWrapAppDoc::CreateAppUiL()
	{
	return new (ELeave) CCustomWrapAppUi;
	}
//
//  ------------------------------ CCustomWrapApp ----------------------- 
//


const TUid KCustomWrapUid = {0x10005d2f};

class CCustomWrapApp : public CEikApplication
	{
private:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;

	};

TUid CCustomWrapApp::AppDllUid() const 
	{
	return KCustomWrapUid;
	}

CApaDocument* CCustomWrapApp::CreateDocumentL()
	{
	return new (ELeave) CCustomWrapAppDoc(*this);
	}


////////////////////////////////////////////////////////////////////////////////////////////
//

	static CApaApplication* NewApplication()
		{
		return new CCustomWrapApp;
		}

	TInt E32Main()
		{
		return EikStart::RunApplication(&NewApplication);
		}


