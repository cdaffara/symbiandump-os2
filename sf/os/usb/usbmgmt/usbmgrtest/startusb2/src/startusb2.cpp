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

#include <e32test.h>
#include <e32twin.h>
#include <c32comm.h>
#include <d32comm.h>
#include <f32file.h>
#include <hal.h>
#include <usbman.h>

static TInt MainL()
	{
	RDebug::Print(_L("Main() - Starting!"));

	RFs theFs;

	TInt r = theFs.Connect();
	if (r != KErrNone)
		{
		RDebug::Print(_L("Main() - Failed to connect to the fs. Error = %d"), r);
		return r;
		}

	RDebug::Print(_L("Main() - Connected to file server"));

	r = StartC32();
	if (r!=KErrNone && r !=KErrAlreadyExists)
		{
		RDebug::Print(_L("Main() - Failed to start C32. Error = %d"), r);
		return r;
		}

	RDebug::Print(_L("E32Main: Started c32"));

	RUsb usb;
	TInt err = usb.Connect();
	if (err != KErrNone)
		{
		RDebug::Print(_L("MainL() - Unable to Connect to USB server"));
		theFs.Close();
		return err;
		}
	RDebug::Print(_L("MainL() - Connected to USB server"));

	TUsbServiceState state;

	err = usb.GetCurrentState(state);
	if (err != KErrNone)
		{
		RDebug::Print(_L("MainL() - Failed to fetch service state from usbman, error %d"), err);
		}
	else
		{
		RDebug::Print(_L("MainL() - Usb service state = 0x%x"), state);
		}

	TRequestStatus status;
	usb.Start(status);
	User::WaitForRequest(status);

	RDebug::Print(_L("Start completed with status %d"), status.Int());

	theFs.Close();
	RDebug::Print(_L("MainL() - Exiting normally"));
	return KErrNone;
	}

GLDEF_C TInt E32Main()
    {
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,err=MainL());

	if (err != KErrNone)
		User::Panic(_L("StartUsb::E32Main - Panic"), err);

	delete cleanup;
	return err;
    }
///////////////////////
