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
 *@file
 *@internalComponent
 */

#include "ncmdhcpnotifwatcher.h"
#include "ncmconnectionmanager.h"
#include "ncmdhcpnotifobserver.h"

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_ncminternalsrv.h>
#else
#include "ncminternalsrv.h"

#endif // OVERDUMMY_NCMCC

// For OST trace
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmdhcpnotifwatcherTraces.h"
#endif

/**
 * Constructor.
 * @param aObserver The AO which handle the DHCP provisioning request from NCM internal server 
 * @param aNcmInteralSvr The reference of the NCM internal server client API handle
 */
CNcmDhcpNotifWatcher::CNcmDhcpNotifWatcher(MNcmDhcpNotificationObserver& aObserver,
        RNcmInternalSrv& aNcmInteralSvr)
	: CActive(CActive::EPriorityStandard), 
	  iObserver(aObserver), 
	  iNcmInteralSvr(aNcmInteralSvr) 
	{
    OstTraceFunctionEntryExt( CNCMDHCPNOTIFWATCHER_CNCMDHCPNOTIFWATCHER_ENTRY, this );
    
	CActiveScheduler::Add(this);		
	OstTraceFunctionExit1( CNCMDHCPNOTIFWATCHER_CNCMDHCPNOTIFWATCHER_EXIT, this );
	}


/**
 * Destructor.
 */
CNcmDhcpNotifWatcher::~CNcmDhcpNotifWatcher()
	{
	OstTraceFunctionEntry0( CNCMDHCPNOTIFWATCHER_CNCMDHCPNOTIFWATCHER_ENTRY_DESTROY );
	Cancel();
	OstTraceFunctionExit0( CNCMDHCPNOTIFWATCHER_CNCMDHCPNOTIFWATCHER_EXIT_DESTROY );
	}

/**
 * Start observing the connection stage changes.
 */
void CNcmDhcpNotifWatcher::StartL()
	{
	OstTraceFunctionEntry0( CNCMDHCPNOTIFWATCHER_START_ENTRY );
	    
	if(!IsActive())
	    {
        iNcmInteralSvr.DhcpProvisionNotify(iStatus);
        SetActive();
	    }
	else
	    {
	    // Report this to NCM connection manager
	    OstTrace0( TRACE_FATAL, CNCMDHCPNOTIFWATCHER_STARTL, "DHCP notifier already actived!" );
	    User::Leave(KErrInUse);
	    }
    
	OstTraceFunctionExit0( CNCMDHCPNOTIFWATCHER_START_EXIT );
	}

/**
 * Implements cancellation of an outstanding request. Cancels the connection progress notification request.
 */
void CNcmDhcpNotifWatcher::DoCancel()
	{
    OstTraceFunctionEntry0( CNCMDHCPNOTIFWATCHER_DOCANCEL_ENTRY );

	iNcmInteralSvr.DhcpProvisionNotifyCancel();
	OstTraceFunctionExit0( CNCMDHCPNOTIFWATCHER_DOCANCEL_EXIT );
	}

/**
 * Called when got DHCP provisioning request from NCM internal server.
 */
void CNcmDhcpNotifWatcher::RunL()
	{
	OstTraceFunctionEntry0( CNCMDHCPNOTIFWATCHER_RUNL_ENTRY );
	
	TInt completionCode = iStatus.Int();
	if (KErrCancel != completionCode)
	    {
        // Doing DHCP
        iObserver.MdnoHandleDhcpNotification(iStatus.Int());
        
        if (KErrNone == completionCode)
            {
            // Restart the watcher
            StartL();
            }
	    }
	OstTraceFunctionExit0( CNCMDHCPNOTIFWATCHER_RUNL_EXIT );
	}

