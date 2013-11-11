/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */


#ifndef SERVICEWATCHER_H
#define SERVICEWATCHER_H

#include <usbstates.h>
#include <e32base.h>

class CUsbNcmConsole;

NONSHARABLE_CLASS(CServiceWatcher) : public CActive
/**
Monitor USB service state
*/
	{
public:
	static CServiceWatcher* NewL(CUsbNcmConsole& aUsb);
	~CServiceWatcher();

private:
	CServiceWatcher(CUsbNcmConsole& aUsb);
	void ConstructL();

	//Display USB service state on main console
	void DisplayServiceState(TUsbServiceState aServiceState);

private:
	//From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CUsbNcmConsole&	iTestConsole;
	TUsbServiceState	iServiceState;
	};


#endif  // SERVICEWATCHER_H
