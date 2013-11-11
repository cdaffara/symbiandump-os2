/**
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

#ifndef __CUSBOTGWATCHER_H__
#define __CUSBOTGWATCHER_H__

#include <e32def.h>

#ifndef __OVER_DUMMYUSBDI__
#include <d32otgdi.h>
#else
#include <usbhost/dummyotgdi/d32otgdi.h>
#endif

#include <e32property.h> //Publish & Subscribe header

class MUsbOtgObserver;
class RUsbOtgDriver;
class CUsbOtg;

/**
 * The CUsbOtgIdPinWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches ID-Pin changes.
 * Publishes TInt property as ID-Pin status
 */
class CUsbOtgBaseWatcher : public CActive
	{
public:
	CUsbOtgBaseWatcher(RUsbOtgDriver& aLdd);
	virtual ~CUsbOtgBaseWatcher();

	// From CActive
	virtual void RunL() = 0;
	virtual void DoCancel() = 0;
	
	virtual void Start();

protected:
	virtual void Post() = 0;

protected:
	RUsbOtgDriver& iLdd;
	};

/**
 * The CUsbOtgIdPinWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches ID-Pin changes.
 * Publishes TInt property as ID-Pin status
 */
NONSHARABLE_CLASS(CUsbOtgIdPinWatcher) : public CUsbOtgBaseWatcher
	{
public:
	static CUsbOtgIdPinWatcher* NewL(RUsbOtgDriver& aLdd);
	virtual ~CUsbOtgIdPinWatcher();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	
protected:
	CUsbOtgIdPinWatcher(RUsbOtgDriver& aLdd);
	void ConstructL();
	virtual void Post();

private:
	RUsbOtgDriver::TOtgIdPin iOtgIdPin;
	};

/**
 * The CUsbOtgVBusWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches ID-Pin changes.
 * Publishes TInt property as ID-Pin status
 */
NONSHARABLE_CLASS(CUsbOtgVbusWatcher) : public CUsbOtgBaseWatcher
	{
public:
	static CUsbOtgVbusWatcher* NewL(RUsbOtgDriver& aLdd);
	virtual ~CUsbOtgVbusWatcher();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	
protected:
	CUsbOtgVbusWatcher(RUsbOtgDriver& aLdd);
	void ConstructL();
	virtual void Post();

private:
	RUsbOtgDriver::TOtgVbus iOtgVbus;
	};	

/**
 * The CUsbOtgVBusWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches ID-Pin changes.
 * Publishes TInt property as ID-Pin status
 */
NONSHARABLE_CLASS(CUsbOtgStateWatcher) : public CUsbOtgBaseWatcher
	{
	public:
		static CUsbOtgStateWatcher* NewL(RUsbOtgDriver& aLdd);
		virtual ~CUsbOtgStateWatcher();

		// From CActive
		virtual void RunL();
		virtual void DoCancel();
		
	protected:
		CUsbOtgStateWatcher(RUsbOtgDriver& aLdd);
		void ConstructL();
		virtual void Post();

	private:
		RUsbOtgDriver::TOtgState iOtgState;
	};

/**
 * The CUsbOtgEventWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches ID-Pin changes.
 * Publishes TInt property as ID-Pin status
 */
NONSHARABLE_CLASS(CUsbOtgEventWatcher) : public CUsbOtgBaseWatcher
	{
	public:
		static CUsbOtgEventWatcher* NewL(CUsbOtg& aOwner, RUsbOtgDriver& aLdd, 
										 RUsbOtgDriver::TOtgEvent& aOtgEvent);
		virtual ~CUsbOtgEventWatcher();

		// From CActive
		virtual void RunL();
		virtual void DoCancel();
		
	protected:
		CUsbOtgEventWatcher(CUsbOtg& aOwner, RUsbOtgDriver& aLdd, RUsbOtgDriver::TOtgEvent& aOtgEvent);
		void ConstructL();
		virtual void Post();
		
	private:
		void LogEventText(RUsbOtgDriver::TOtgEvent /*aState*/);

	private:
		CUsbOtg& iOwner;
		RUsbOtgDriver::TOtgEvent& iOtgEvent;
	};	
	
/**
 * The CUsbOtgConnectionIdleWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches Connection Idle changes.
 * Publishes TInt property as Connection Idle status
 */
NONSHARABLE_CLASS(CUsbOtgConnectionIdleWatcher) : public CUsbOtgBaseWatcher
	{
public:
	static CUsbOtgConnectionIdleWatcher* NewL(RUsbOtgDriver& aLdd);
	virtual ~CUsbOtgConnectionIdleWatcher();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	
protected:
	CUsbOtgConnectionIdleWatcher(RUsbOtgDriver& aLdd);
	void ConstructL();
	virtual void Post();

private:
	RUsbOtgDriver::TOtgConnection iConnectionIdle;
	};


/**
 * The CUsbOtgWatcher class
 *
 * Talks directly to the USB OTG Logical Device Driver (LDD) and 
 * watches any messages/errors which occur
 */
NONSHARABLE_CLASS(CUsbOtgWatcher) : public CActive
	{
public:
	static CUsbOtgWatcher* NewL(MUsbOtgObserver& aOwner, RUsbOtgDriver& aLdd, TUint& aOtgMessage);
	virtual ~CUsbOtgWatcher();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();

	virtual void Start();

protected:
	CUsbOtgWatcher(MUsbOtgObserver& aOwner, RUsbOtgDriver& aLdd, TUint& aOtgMessage);
	void Post();

private:
	MUsbOtgObserver& iOwner;
	RUsbOtgDriver& iLdd;
	TUint& iOtgMessage;
	};

NONSHARABLE_CLASS(CRequestSessionWatcher) : public CActive
	{
public:
	static CRequestSessionWatcher* NewL(MUsbOtgObserver& aOwner);
	~CRequestSessionWatcher();

private:
	CRequestSessionWatcher(MUsbOtgObserver& aOwner);
	void ConstructL();
	
	// From CActive
	virtual void RunL();
	virtual void DoCancel();

private:
	RProperty iProp;
	MUsbOtgObserver& iOwner;
	};

#endif

