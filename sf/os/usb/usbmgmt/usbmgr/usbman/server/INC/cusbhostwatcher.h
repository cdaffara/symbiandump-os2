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

/**
 @file
 @internalComponent
 @released
*/


#ifndef CUSBHOSTWATCHER_H
#define CUSBHOSTWATCHER_H

#include "usbhoststack.h"
#include <usb/usbshared.h>

enum TUsbHostMonitor
	{
	EHostEventMonitor 	= 0,
	EHostMessageMonitor = 1,
	ENumMonitor			= 2,
	};	

const TUint KHostEventMonitor = EHostEventMonitor;
const TUint KHostMessageMonitor = EHostMessageMonitor;

class MUsbHostObserver;

class CActiveUsbHostWatcher : public CActive
	{
public:

	CActiveUsbHostWatcher(RUsbHostStack& aUsbHostStack, 
						  MUsbHostObserver& aOwner,
						  TUint aWatcherId);
	virtual ~CActiveUsbHostWatcher();

public:
	virtual void Post() = 0;

protected: // from CActive
	virtual void RunL();
	virtual void DoCancel() = 0;

protected: // unowned
	RUsbHostStack& iUsbHostStack;
	MUsbHostObserver& iOwner;
	TUint iWatcherId;
	};


class CActiveUsbHostEventWatcher: public CActiveUsbHostWatcher
	{
public:
	static CActiveUsbHostEventWatcher* NewL(RUsbHostStack& aUsbHostStack, 
											MUsbHostObserver& aOwner,
											TDeviceEventInformation& aDeviceEventInfo);
	virtual ~CActiveUsbHostEventWatcher();
private:
	CActiveUsbHostEventWatcher(RUsbHostStack& aUsbHostStack, 
							   MUsbHostObserver& aOwner,
							   TDeviceEventInformation& aHostEventInfo);

private: // from CActive
	virtual void DoCancel();
public:
	// from CActiveUsbHostWatcher
	virtual void Post();

private:
	TDeviceEventInformation& iHostEventInfo;
};


class CActiveUsbHostMessageWatcher: public CActiveUsbHostWatcher
	{
public:
	static CActiveUsbHostMessageWatcher* NewL(RUsbHostStack& aUsbHostStack, 
											  MUsbHostObserver& aOwner,
											  TInt& aHostMessage);
	virtual ~CActiveUsbHostMessageWatcher();
private:
	CActiveUsbHostMessageWatcher(RUsbHostStack& aUsbHostStack, 
								 MUsbHostObserver& aOwner,
								 TInt& aHostMessage);

private: // from CActive
	virtual void DoCancel();
public:
	// from CActiveUsbHostWatcher
	virtual void Post();
	
private:
	TInt& iHostMessage;
	};

#endif //CUSBHOSTWATCHER_H
