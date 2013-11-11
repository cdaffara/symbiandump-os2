/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



 /*************************************************************************
 *
 * System Includes
 *
 *************************************************************************/
#include <e32std.h> 
#include <eikstart.h> 

/*************************************************************************
 *
 * Local Includes
 *
 *************************************************************************/
#include <statgui.rsg>
#include "stat_application.h"
#include "stat_window.h"
#ifdef SYMBIAN_DIST_SERIES60
#include <avkon.hrh>
#endif

/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/
const TUid KUidStat = { 0x10009B04 }; 
#define IS_COMMAND_CHAR(c) ((c == 'A') || (c == 'C'))

/*************************************************************************
 *
 * Prototypes
 *
 *************************************************************************/
char *lstrchr( char *str, char chr );

/*************************************************************************
 *
 * Application entry-point
 *
 *************************************************************************/
LOCAL_C CApaApplication* NewApplication()
{
	return new CStatApplication;
}

GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication(NewApplication);
}


/*************************************************************************
 *
 * Top-level Application Implementation
 *
 *************************************************************************/
TUid CStatApplication::AppDllUid() const
{
	return KUidStat;
}

CApaDocument* CStatApplication::CreateDocumentL()
{
	return new (ELeave) CStatDocument(*this);
}

/*************************************************************************
 *
 * Document Class - no real work in this application
 *
 *************************************************************************/
CStatDocument::CStatDocument(CEikApplication& aApp) : CEikDocument(aApp)
{
}

CEikAppUi* CStatDocument::CreateAppUiL()
{
	return new(ELeave) CStatAppUi;
}

/*************************************************************************
 *
 * CStatAppUI
 *
 *************************************************************************/
void CStatAppUi::ConstructL()
{
    BaseConstructL();

	// create the STAT controller
	iController = CStatController::NewL(); 
	iWindow = new(ELeave) CStatWindow();
	iWindow->ConstructL( ClientRect(), iController );
	iWindow->SetObserver( this );
}

CStatAppUi::~CStatAppUi()
{
	delete iWindow;
	iWindow = NULL;
	delete iController;
	iController = NULL;
}

TKeyResponse CStatAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	return iWindow->OfferKeyEventL( aKeyEvent, aType );
}

void CStatAppUi::HandleControlEventL( CCoeControl* /*aControl*/, TCoeEvent aEventType )
{
	if( aEventType == EEventRequestExit )
		Exit();
}

void CStatAppUi::HandleCommandL( TInt aCommand )
{
	switch( aCommand ) {

	case EStatCmdAction:
		iWindow->HandleControlEventL( (CCoeControl*)KActionButton, MCoeControlObserver::EEventStateChanged );
		break;
	
	case EAknSoftkeyExit:
		iWindow->HandleControlEventL( (CCoeControl*)KExitButton, MCoeControlObserver::EEventStateChanged );
		break;

	case EStatCmdToggleLogging:
		iWindow->HandleControlEventL( (CCoeControl*)KLogButton, MCoeControlObserver::EEventStateChanged );
		break;
	}
}

void CStatAppUi::HandleForegroundEventL( TBool aForeground )
{
	iWindow->SetForeground( aForeground );
}


