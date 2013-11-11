/**
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
* Usb Battery Charging test code
* 
*
*/



#ifndef __CDUMMYUSBDEVICE_H__
#define __CDUMMYUSBDEVICE_H__

#include <usb/chargingtest/CUsbBatteryChargingTestPlugin.h>
#include <usbstates.h>
#include <ecom/ecom.h>
#include <e32std.h>
#include "musbmanextensionpluginobserver.h"
#include "cusbmanextensionplugin.h"
#include <e32test.h>
#include <e32property.h> //Publish & Subscribe header

class MUsbDeviceNotify;

_LIT(KUsbChargingTestPanic, "UsbChargingTestPanic");
enum TUsbChargingTestPanic
	{
	EUsbChargingTestPanicIncorrectPlugin = 0,
	EUsbChargingTestPanicBadInputData	 = 1,
	EUsbChargingTestPanicBadCommand		 = 2,
	EUsbChargingTestPanicBadAsyncOp		 = 3,
	EUsbChargingTestPanicBadCheck		 = 4,
	};

enum TUsbChargingTestCommand
	{
	EUsbChargingTestCommandNone		    = 0, // "none"
	EUsbChargingTestCommandDeviceState  = 1, // "devicestate"
	EUsbChargingTestCommandUserSetting  = 2, // "usersetting"
	};

enum TUsbChargingTestCheck
	{
	EUsbChargingTestCheckNone			= 0, // "none"
	EUsbChargingTestCheckPluginState	= 1, // "pluginstate"
	EUsbChargingTestCheckMilliAmps		= 2, // "milliamps" - requested current
	EUsbChargingTestCheckCharging		= 3, // "charging" - available current
	};

enum TUsbChargingTestAsyncOp
	{
	EUsbChargingTestAsyncOpNone			= 0, // "none"
	EUsbChargingTestAsyncOpDelay		= 1, // "delay" - microseconds
	};

class CDummyUsbDevice : public CActive, public MUsbmanExtensionPluginObserver
	{
public:
	static CDummyUsbDevice* NewL();
	virtual ~CDummyUsbDevice();

	void RegisterObserverL(MUsbDeviceNotify& aObserver);
	void DeRegisterObserver(MUsbDeviceNotify& aObserver);
	
	void DefinePropertyL(const TInt32 aCategory, TUint aKey,RProperty::TType eType);
	TInt GetChargingCurrentFromProperty(TInt &aCurrent);
	TInt WriteToRepositoryProperty(TInt iCommandValue);
	
	TInt StartPropertyBatteryCharging();

	void DoTestsL();

public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

protected:
	CDummyUsbDevice();
	void ConstructL();

private:
	void InstantiateExtensionPluginsL();
	void UpdatePluginInfo();
	void OpenFileL();
	TInt GetNextLine();
	void InterpretLine();
	TInt GetCommand(const TDesC8& aDes);
	TInt GetAsyncOp(const TDesC8& aDes);
	TInt GetCheck(const TDesC8& aDes);
	void DoCommand();
	void DoAsyncOp();
	void DoCheck();

private: // from MUsbmanExtensionPluginObserver
	RDevUsbcClient& MuepoDoDevUsbcClient();
	void MuepoDoRegisterStateObserverL(MUsbDeviceNotify& aObserver);

private:
	RPointerArray<MUsbDeviceNotify> iObservers;
	RPointerArray<CUsbmanExtensionPlugin> iExtensionPlugins;
	// we know there'll only be one plugin, so keep a single pointer to it...
	CUsbmanExtensionPlugin* iPlugin;
	TUsbDeviceState  iDeviceState;
	RDevUsbcClient iDummyLdd;
	REComSession* iEcom;	//	Not to be deleted, only closed!

	TPluginTestInfo iInfo;

	RTest iTest;
	RTimer iTimer;

	TPtr8 iPtr; // data file
	TPtr8 iLine; // current line
	TInt  iLineNumber;
	TInt iFileOffset;
	TText8* iText;

	TInt	iCommand;
	TInt					iCommandValue;
	TInt	iAsyncOp;
	TInt					iAsyncOpValue;
	TInt	iCheck;
	TInt					iCheckValue;
	
	RProperty iPropertyWriteToRepositoryAck;
	RProperty iPropertyReadChargingCurrentAck;
	RProperty iProperty;
	};

#endif // __CDUMMYUSBDEVICE_H__

