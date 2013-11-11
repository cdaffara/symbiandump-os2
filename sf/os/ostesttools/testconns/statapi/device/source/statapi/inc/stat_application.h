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
 * Switches
 *
 ************************************************************************/
#ifndef __STATAPPLICATION_H__
#define __STATAPPLICATION_H__

/*************************************************************************
 *
 * System Includes
 *
 ************************************************************************/
#include <coeccntx.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/
#include <statgui.rsg>
#include "statapi.hrh"
#include "stat_window.h"
#include "stat_tcpip.h"
#include "stat_controller.h"

#ifdef SYMBIAN_DIST_SERIES60
#include <aknappui.h>
#endif

/*************************************************************************
 *
 * Definitions
 *
 ************************************************************************/
#ifdef SYMBIAN_DIST_SERIES60
#define CPlatAppUi	CAknAppUi
#else
#define CPlatAppUi CEikAppUi
#endif

/*************************************************************************
 *
 * CStatApplication
 *
 ************************************************************************/
class CStatApplication : public CEikApplication
{
private: 
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
};

/*************************************************************************
 *
 * CStatDocument
 *
 ************************************************************************/
class CStatDocument : public CEikDocument
{
public:
	static CStatDocument* NewL(CEikApplication& aApp);
	CStatDocument(CEikApplication& aApp);
	void ConstructL();

private: 
	CEikAppUi* CreateAppUiL();
};

/*************************************************************************
 *
 * CStatAppUi
 *
 ************************************************************************/
class CStatAppUi : public CPlatAppUi, public MCoeControlObserver
{
public:	
    void ConstructL();
	~CStatAppUi();

	// From CEikAppUi
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleCommandL( TInt aCommand );
	void HandleForegroundEventL(TBool aForeground);

	// From MCoeControlObserver
	void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

private:
	CStatController *iController;
	CStatWindow *iWindow;
};



#endif //__STATAPPLICATION_H__

