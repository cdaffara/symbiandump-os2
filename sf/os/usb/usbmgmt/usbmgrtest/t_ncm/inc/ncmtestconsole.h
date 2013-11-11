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


#ifndef NCMTESTCONSOLE_H
#define NCMTESTCONSOLE_H

#include <usbman.h>
#include <e32cons.h>
#include <usb/usblogger.h>
#include <comms-infras/commsdebugutility.h>
#include <in_sock.h>

void Panic(TInt aPanic);

#ifdef __FLOG_ACTIVE
_LIT8(KSubSys, "NCM");
_LIT8(KLogComponent, "TEST");
#endif


class CConsoleBase;
class CNcmCommandEngine;
class CDeviceWatcher;
class CServiceWatcher;
class CSharedStateWatcher;

_LIT(KUsbNcmConsoleTitle, "NCM Test Console");

//Display item index
enum TDisplayItemIndex
	{	
	EUsbServiceStateItem = 0,		
	EUsbDeviceStateItem,			
	ENcmConnStatusItem,			
	ENcmConnIpItem,				
	EUplinkConnStatusItem,			
	EUplinkConnIpItem,				
	ENcmBtPanItem,				
	ENcmAgentStateItem,			
	EAuthorisationSettingItem,		
	ENcmConnectionStateItem,
	ELastItem						//not used, just for count
	};

#define DISPLAY_USB_SERVICE_STATE_LEN	11			//EUsbServiceStateItem
#define DISPLAY_USB_DEVICE_STATE_LEN	11			//EUsbDeviceStateItem
#define DISPLAY_NCM_CONN_STATE_LEN	16			//ENcmConnStatusItem
#define DISPLAY_NCM_CONN_IP_LEN		15			//ENcmConnIpItem
#define DISPLAY_UPLINK_CONN_STATE_LEN	16			//EUplinkConnStatusItem
#define DISPLAY_UPLINK_CONN_IP_LEN		15			//EUplinkConnIpItem
#define DISPLAY_NCM_BTPAN_LEN			10			//ENcmBtPanItem
#define DISPLAY_AGENT_STATE_LEN			30			//ENcmAgentStateItem
#define DISPLAY_AUTH_SETTING_LEN		15			//EAuthorisationSettingItem
#define DISPLAY_NCM_CONNECTION_STATE    16

//The SID of Ncm state P&S key used
const TUid KC32ExeSid = {0x101F7989};

NONSHARABLE_CLASS(CUsbNcmConsoleEvent)
/**
Event reported by running commands and watcher classes 
*/
	{
public:
	static CUsbNcmConsoleEvent* NewL();
	~CUsbNcmConsoleEvent();
private:
	CUsbNcmConsoleEvent();
	void ConstructL();
public:
	//Event queue link
	TDblQueLink	iLink;
	//Event description
	RBuf		iEvent;
	};


NONSHARABLE_CLASS(CUsbNcmConsole) : public CActive
/**
The Main Console of NCM manual test
*/
	{
friend class CNcmCommandEngine;

public:
	static CUsbNcmConsole* NewLC(TBool aDemo);
	~CUsbNcmConsole();

public:
	void StartL();
	void Stop() const;
	
	RUsb& Usb();

public:
	//commands use it to report event to main console
	void NotifyEvent(CUsbNcmConsoleEvent* aEvent);
	
	//Set the Ncm IAP and monitor the related connection status
	void SetLocalIapId(TInt aId);
	//Set the Uplink IAP and monitor the related connection status
	void SetUplinkIapId(TInt aId);

	//Get Ncm Iap ID
	TInt LocalIapId() const;
	//Get Uplink Iap ID
	TInt UplinkIapId() const;
	
	//Called by commands and watchers to notify main console refresh the screen
	void ScheduleDraw(TUint aKey);
	
	//Set the permanent info to display on screen 
	void SetDisplayItem(TInt aIndex, TDes& aInfo);
	//Get the permanent info on screen
	const TDes& GetDisplayItem(TInt aIndex);

	//Display all Iaps configured in commsdb
	TInt DrawAvailableIapsL();
	
	//Attach the connection specified by aIap
	TBool AttachConnectionL(TInt aIap, RConnection& aConnection);
	
private:
	CUsbNcmConsole(TBool aDemo);
	void ConstructL();
	
	//Display all kinds of info.
	void DrawL();
	//Init the items which are displayed on screen permanently
	void CreateDisplayItemL();

	//Call back function 
	static TInt DoCommandCallback(TAny *aThisPtr);

private:
	//From Active
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
    CConsoleBase* iConsole;
	RUsb iUsb;
	
	//The info displayed on screen permanently
	RArray<RBuf> iDisplayArray;

	TVersion	iVersion;
	
	//The Uplink IAP id	
	TInt	iLocalIapId;
	
	CNcmCommandEngine* iKeys;

	//monitor the USB device state
	CDeviceWatcher* iDeviceWatcher;
	//monitor the USB service state
	CServiceWatcher* iServiceWatcher;

	//Get the agent state of Ncm
	CSharedStateWatcher*    iSharedStateWatcher; 
	//Indicate current operation is display the help or not.
	TInt 		iHelp;
	
	//If 'DEMO' is used as application parameter, iDemo will be set true. The Ncm
	//Control app will be started automaticly and start Ncm automicly when usb cable
	//plugin. This is used to demo for licencee.
	TBool iDemo;	
//	CAsyncCallBack* iStartWatcherCallback;
	
	// Event list
	TDblQue<CUsbNcmConsoleEvent>	iEventList;
	};


#endif // NCMTESTCONSOLE_H
