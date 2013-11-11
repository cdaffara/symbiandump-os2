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

#ifndef DEVICEWATCHER_H
#define DEVICEWATCHER_H

#include <e32base.h>
#include <d32usbc.h>

class CUsbNcmConsole;

//The mode of CDeviceWatcher
enum TDeviceWatchMode
	{
	EWatchMode,				//Only monitor the device state
	EWatchAndStartMode		//monitor the device state and start/stop NCM and NCM control app 
							//when usb cable plugin/unplug
	};

NONSHARABLE_CLASS(CDeviceWatcher) : public CActive
/**
Monitor the USB device state.
*/
	{	
public:
	
	static CDeviceWatcher* NewL(CUsbNcmConsole& aUsb);
	~CDeviceWatcher();

	void SetWatchMode(TDeviceWatchMode aWatchMode);
	TDeviceWatchMode GetWatchMode() const;

private:
	CDeviceWatcher(CUsbNcmConsole& aUsb);
	void ConstructL();
	
private:
	//From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	void DisplayDeviceState(TUint aDeviceState);

private:
	//Main console
	CUsbNcmConsole&  iTestConsole;
	RDevUsbcClient iLdd;

	//Usb device state
	TUint		iDeviceState;
	//Usb old device state
	TUint		iOldDeviceState;
	//Indicate whether start/stop behavior are used. ETrue means start/stop is needed.
	TBool				iStartNcm;	
	TDeviceWatchMode	iWatchMode;
	};


#endif //DEVICEWATCHER_H
