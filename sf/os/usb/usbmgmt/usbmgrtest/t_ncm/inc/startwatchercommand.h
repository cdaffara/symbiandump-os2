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

#ifndef STARTWATCHERCOMMAND_H
#define STARTWATCHERCOMMAND_H

#include "commandengine.h"

class CDeviceWatcher;

NONSHARABLE_CLASS(CStartWatcherCommand) : public CNcmCommandBase
/**
Set the test mode to run NCM automaticly when usb cable plugin.
*/
	{
public:
	static CStartWatcherCommand* NewL(CUsbNcmConsole& aUsb, TUint aKey, CDeviceWatcher& aDeviceWatch);
	~CStartWatcherCommand();

public:
	//From CNcmCommandBase
	void DoCommandL();

private:
	CStartWatcherCommand(CUsbNcmConsole& aUsb, TUint aKey, CDeviceWatcher& aDeviceWatch);
	void ConstructL();
	
private:
	CDeviceWatcher& iDeviceWatcher;
	};

#endif // STARTWATCHERCOMMAND_H
