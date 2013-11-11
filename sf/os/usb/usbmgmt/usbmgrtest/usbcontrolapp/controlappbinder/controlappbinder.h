/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CONTROL_APP_BINDER_H
#define CONTROL_APP_BINDER_H

#include <e32cmn.h>
#include <e32base.h>

#include "usbcontrolappshared.h"


_LIT(KUsbControlAppBinderTitle, "BIND");

class CUsbControlAppBinderKeys;

NONSHARABLE_CLASS(CUsbControlAppBinderConsole) : public CBase
	{
friend class CUsbControlAppBinderKeys;

public:
	static CUsbControlAppBinderConsole* NewLC();
	~CUsbControlAppBinderConsole();

public:
	void Start();     
	void Stop() const; 

private:
	CUsbControlAppBinderConsole();
	void ConstructL();
	void Move(TInt aX, TInt aY);
	void Draw();

	void StartAwareApp();
	void StartControlApp();
	void StartViewerApp();
	
private:
	CConsoleBase* 				iConsole;	
	CUsbControlAppBinderKeys*	iKeys;

	// Handles for the individual components to run in parallel
	
	static RProcess iAwareApp;
	static RProcess iControlApp;
	static RProcess iViewerApp;
	
private:
	// Run on the far right-hand side of the H4 screen, which (for
	// the TextShell program) is 24 rows, 40 columns - each allows
	// 2 row/column for borders
	static const TInt KBinderWindowDepth = (KScreenDepth - 3);
	static const TInt KBinderWindowWidth = (KScreenWidth - (KViewerNumCharactersOnLine + 4));
	};



NONSHARABLE_CLASS(CUsbControlAppBinderKeys) : public CActive
	{
public:
	static CUsbControlAppBinderKeys* NewL(CUsbControlAppBinderConsole& aUsbControlAppBinderConsole);
	~CUsbControlAppBinderKeys();
	
private:
	CUsbControlAppBinderKeys(CUsbControlAppBinderConsole& aUsbControlAppBinderConsole);
	void ConstructL();
	void DoCancel();
	void RunL();
	
private:
	CUsbControlAppBinderConsole&	iUsbControlAppBinderConsole;
	};


#endif // CONTROL_APP_BINDER_H
	
