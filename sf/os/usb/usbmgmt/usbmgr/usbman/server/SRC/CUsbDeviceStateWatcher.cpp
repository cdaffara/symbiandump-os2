/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Talks directly to the USB Logical Device Driver (LDD) and 
* watches any state changes
*
*/

/**
 @file
*/

#include "CUsbScheduler.h"
#include "CUsbDeviceStateWatcher.h"
#include "CUsbDevice.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbDeviceStateWatcherTraces.h"
#endif



/**
 * The CUsbDeviceStateWatcher::NewL method
 *
 * Constructs a new CUsbDeviceStateWatcher object
 *
 * @internalComponent
 * @param	aOwner	The device that owns the state watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 *
 * @return	A new CUsbDeviceStateWatcher object
 */
CUsbDeviceStateWatcher* CUsbDeviceStateWatcher::NewL(CUsbDevice& aOwner, RDevUsbcClient& aLdd)
	{
	OstTraceFunctionEntry0( CUSBDEVICESTATEWATCHER_NEWL_ENTRY );

	CUsbDeviceStateWatcher* r = new (ELeave) CUsbDeviceStateWatcher(aOwner, aLdd);
	OstTraceFunctionExit0( CUSBDEVICESTATEWATCHER_NEWL_EXIT );
	return r;
	}


/**
 * The CUsbDeviceStateWatcher::~CUsbDeviceStateWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbDeviceStateWatcher::~CUsbDeviceStateWatcher()
	{
	OstTrace1( TRACE_NORMAL, CUSBDEVICESTATEWATCHER_CUSBDEVICESTATEWATCHER, 
	        "CUsbDeviceStateWatcher::~CUsbDeviceStateWatcher;  this=(0x%08x)", (TUint32)this );
	Cancel();
	}


/**
 * The CUsbDeviceStateWatcher::CUsbDeviceStateWatcher method
 *
 * Constructor
 *
 * @param	aOwner	The device that owns the state watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 */
CUsbDeviceStateWatcher::CUsbDeviceStateWatcher(CUsbDevice& aOwner, RDevUsbcClient& aLdd)
	: CActive(CActive::EPriorityStandard), iOwner(aOwner), iLdd(aLdd)
	{
	CActiveScheduler::Add(this);
	}

/**
 * Called when the USB device changes its state.
 */
void CUsbDeviceStateWatcher::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		OstTrace1( TRACE_NORMAL, CUSBDEVICESTATEWATCHER_RUNL, 
		        "CUsbDeviceStateWatcher::RunL;Error=%d", iStatus.Int() );
		return;
		}

	OstTrace1( TRACE_NORMAL, CUSBDEVICESTATEWATCHER_RUNL_DUP1, "CUsbDeviceStateWatcher::RunL; - State Changed to %u", iState );
	
	if (!(iState & KUsbAlternateSetting))
		iOwner.SetDeviceState((TUsbcDeviceState) iState);

	OstTrace0( TRACE_NORMAL, CUSBDEVICESTATEWATCHER_RUNL_DUP2, 
	        "CUsbDeviceStateWatcher::RunL - About to call DeviceStatusNotify" );
	iLdd.AlternateDeviceStatusNotify(iStatus, iState);
	SetActive();
	OstTrace0( TRACE_NORMAL, CUSBDEVICESTATEWATCHER_RUNL_DUP3, "CUsbDeviceStateWatcher::RunL - Called DeviceStatusNotify" );
	}


/**
 * Automatically called when the state watcher is cancelled.
 */
void CUsbDeviceStateWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBDEVICESTATEWATCHER_DOCANCEL_ENTRY );
	iLdd.AlternateDeviceStatusNotifyCancel();
	OstTraceFunctionExit0( CUSBDEVICESTATEWATCHER_DOCANCEL_EXIT );
	}


/**
 * Instructs the state watcher to start watching.
 */
void CUsbDeviceStateWatcher::Start()
	{
	OstTraceFunctionEntry0( CUSBDEVICESTATEWATCHER_START_ENTRY );
	iLdd.AlternateDeviceStatusNotify(iStatus, iState);
	SetActive();
	OstTraceFunctionExit0( CUSBDEVICESTATEWATCHER_START_EXIT );
	}
