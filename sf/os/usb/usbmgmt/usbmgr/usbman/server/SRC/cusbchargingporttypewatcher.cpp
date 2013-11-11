/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "CUsbScheduler.h"
#include "cusbchargingporttypewatcher.h"
#include "CUsbDevice.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbchargingporttypewatcherTraces.h"
#endif


/**
 * The CUsbChargingPortTypeWatcher::NewL method
 *
 * Constructs a new CUsbChargingPortTypeWatcher object
 *
 * @internalComponent
 * @param	aOwner	The device that owns the charger type watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 *
 * @return	A new CUsbChargingPortTypeWatcher object
 */
CUsbChargingPortTypeWatcher* CUsbChargingPortTypeWatcher::NewL(CUsbDevice& aOwner, RDevUsbcClient& aLdd)
	{
	CUsbChargingPortTypeWatcher* r = new (ELeave) CUsbChargingPortTypeWatcher(aOwner, aLdd);
	return r;
	}


/**
 * The CUsbChargingPortTypeWatcher::~CUsbChargingPortTypeWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbChargingPortTypeWatcher::~CUsbChargingPortTypeWatcher()
	{
	Cancel();
	}


/**
 * The CUsbChargingPortTypeWatcher::CUsbChargingPortTypeWatcher method
 *
 * Constructor
 *
 * @param	aOwner	The device that owns the charger type watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 */
CUsbChargingPortTypeWatcher::CUsbChargingPortTypeWatcher(CUsbDevice& aOwner, RDevUsbcClient& aLdd)
	: CActive(CActive::EPriorityStandard), iOwner(aOwner), iLdd(aLdd)
	{
	CActiveScheduler::Add(this);
	}

/**
 * Called when the charger type changes.
 */
void CUsbChargingPortTypeWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBCHARGINGPORTTYPEWATCHER_RUNL_ENTRY );
	if (iStatus.Int() != KErrNone && iStatus.Int() != KErrDisconnected)
		{
		Start();
        OstTrace1( TRACE_ERROR, CUSBCHARGINGPORTTYPEWATCHER_RUNL, "CUsbChargingPortTypeWatcher::RunL;iStatus.Int()=%d", iStatus.Int() );
		OstTrace1( TRACE_NORMAL, DUP2_CUSBCHARGINGPORTTYPEWATCHER_RUNL, "CUsbChargingPortTypeWatcher::RunL;iChargingPortType=%d", iChargingPortType );
		OstTraceFunctionExit0( CUSBCHARGINGPORTTYPEWATCHER_RUNL_EXIT );
		return;
		}
    OstTrace1( TRACE_NORMAL, DUP1_CUSBCHARGINGPORTTYPEWATCHER_RUNL, "CUsbChargingPortTypeWatcher::RunL;iChargingPortType=%d", iChargingPortType );
	TUsbcChargingPortType portType = static_cast<TUsbcChargingPortType> (iChargingPortType);	
	Start();
	iOwner.SetChargingPortType(portType);
	OstTraceFunctionExit0( DUP1_CUSBCHARGINGPORTTYPEWATCHER_RUNL_EXIT );
	}


/**
 * Automatically called when the charger type watcher is cancelled.
 */
void CUsbChargingPortTypeWatcher::DoCancel()
	{
	iLdd.ChargingPortTypeNotifyCancel();
	}


/**
 * Instructs the charger type watcher to start watching.
 */
void CUsbChargingPortTypeWatcher::Start()
	{
	OstTraceFunctionEntry0( CUSBCHARGINGPORTTYPEWATCHER_START_ENTRY );
	iStatus = KRequestPending;
	iLdd.ChargingPortTypeNotify(iStatus, iChargingPortType);
	SetActive();
	OstTraceFunctionExit0( CUSBCHARGINGPORTTYPEWATCHER_START_EXIT );
	}
