/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <usbman.h>

static void MainL()
	{
	RUsb usb;
	User::LeaveIfError(usb.Connect());
	usb.Stop();
	usb.Close();
	}

GLDEF_C TInt E32Main()
    {
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());

	if (err != KErrNone)
		User::Panic(_L("StopUsb::E32Main - Panic"), err);

	delete cleanup;
	return KErrNone;
    }
///////////////////////
