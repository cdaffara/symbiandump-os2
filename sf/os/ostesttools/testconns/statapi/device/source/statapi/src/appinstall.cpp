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



#include <e32base.h>
#include <e32cons.h>
#include <hal.h>
#include <iniparser.h>

// Epoc Includes
#include <swi/launcher.h>


#include "appinstall.h"
#include "testsecurityinstuihandler.h"


CAppInstall::CAppInstall()
	{
	}

TInt CAppInstall::Install(const TDesC& aFileName)
	{
	
	TInt error = KErrNone;

	TFileName fileName(aFileName);
	
	fileName.PtrZ();	
	
	Swi::CInstallPrefs* iInstallPrefs = Swi::CInstallPrefs::NewL();
	
	CTestSecurityInstUIHandler* iUIHandler = CTestSecurityInstUIHandler::NewL();
	
	error = Swi::Launcher::Install(*iUIHandler,fileName,*iInstallPrefs);

	delete iUIHandler;
	iUIHandler = 0;
	delete iInstallPrefs;
	iInstallPrefs = 0;
	
	return error;
	
	}

TInt CAppInstall::Uninstall(const TDesC& aUid)
	{
	
	TInt error = KErrNone;

	TLex conv(aUid);
	
	TUint32 id;
	
	error = conv.Val(id,EHex);
	
	if(error!=KErrNone)
		{
		return error;
		}
		
	TUid uid = TUid::Uid(id);
		
	
	CTestSecurityInstUIHandler* iUIHandler = CTestSecurityInstUIHandler::NewL();

	error = Swi::Launcher::Uninstall(*iUIHandler, uid);
	
	delete iUIHandler;
	iUIHandler = 0;

	return error;
		
	}

