/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a Symbian OS server that exposes the RUsb API
* 
*
*/



/**
 @file
*/

#ifndef __CUSBSERVER_H__
#define __CUSBSERVER_H__


_LIT(KUsbSvrPncCat, "CUsbServer");

enum TUsbPanicServer
	{
	EICSInvalidCount 			= 0x00,
	EDCSInvalidCount 			= 0x10,
	ELSTNSNotIdle 				= 0x20,
	ENullPersonalityPointer 	= 0x30,
	EMaxClassUidsBufTooSmall	= 0x40
	};

//
// Forward declarations
//
class CUsbDevice;
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
class CUsbOtg;
class CUsbHost;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

/**
 * The CUsbServer class
 *
 * Implements a Symbian OS server that exposes the RUsb API
 */
 NONSHARABLE_CLASS(CUsbServer) : public CPolicyServer
	{
public:
	static CUsbServer* NewLC();
	virtual ~CUsbServer();

	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	void Error(TInt aError);

	inline CUsbDevice& Device() const;

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	inline CUsbOtg& Otg() const;
	inline CUsbHost& Host() const;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

	void IncrementSessionCount();
	void DecrementSessionCount();
	inline TInt SessionCount() const;
	void LaunchShutdownTimerIfNoSessions();

protected:
	CUsbServer();
	void ConstructL();
	
private:
	CUsbDevice* iUsbDevice;
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	CUsbOtg* iUsbOtg;
	CUsbHost* iUsbHost;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

	TInt iSessionCount;
	enum {KShutdownDelay = 2 * 1000 * 1000};	// 2 seconds
	class CShutdownTimer : public CTimer
		{
	public:
		CShutdownTimer();
		void ConstructL();
		virtual void RunL();
		};
	CShutdownTimer* iShutdownTimer;
	};

#include "CUsbServer.inl"

#endif
