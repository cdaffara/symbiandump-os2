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
 * @file
 * @internalComponent
 */

#include "ncmiapprogresswatcher.h"
#include "ncmiapprogressstateobserver.h"
#include "ncmconnectionmanager.h"

// For OST Tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmiapprogresswatcherTraces.h"
#endif

/**
 * Constructor.
 * @param aObserver The object which is interested in the IAP progress stage change
 * @param aConnection The reference of the NCM connection to be used
 */
CNcmIapProgressWatcher::CNcmIapProgressWatcher(MNcmIapProgressStateObserver& aObserver, RConnection& aConnection)
	: CActive(CActive::EPriorityStandard)
	, iObserver(aObserver)
	, iConnection(aConnection) 
	{
    OstTraceFunctionEntryExt( CNCMIAPPROGRESSWATCHER_CNCMIAPPROGRESSWATCHER_ENTRY, this );
    
	CActiveScheduler::Add(this);		
	OstTraceFunctionExit1( CNCMIAPPROGRESSWATCHER_CNCMIAPPROGRESSWATCHER_EXIT, this );
	}


/**
 * Destructor.
 */
CNcmIapProgressWatcher::~CNcmIapProgressWatcher()
	{
	OstTraceFunctionEntry0( CNCMIAPPROGRESSWATCHER_CNCMIAPPROGRESSWATCHER_ENTRY_DESTRUCTOR );
	
	Cancel();
	OstTraceFunctionExit0( CNCMIAPPROGRESSWATCHER_CNCMIAPPROGRESSWATCHER_ENTRY_DESTRUCTOR_EXIT );
	}

/**
 * Start observing the connection stage changes.
 */
void CNcmIapProgressWatcher::Start()
	{
	OstTraceFunctionEntry0( CNCMIAPPROGRESSWATCHER_START_ENTRY );
	
	if (!IsActive())
		{
		iConnection.ProgressNotification(iProgressInfoBuf, iStatus);
		SetActive();
		}
	OstTraceFunctionExit0( CNCMIAPPROGRESSWATCHER_START_EXIT );
	}

/**
 * Implements cancellation of an outstanding request. Cancels the connection progress notification request.
 */
void CNcmIapProgressWatcher::DoCancel()
	{
    OstTraceFunctionEntry0( CNCMIAPPROGRESSWATCHER_DOCANCEL_ENTRY );

	iConnection.CancelProgressNotification();
	OstTraceFunctionExit0( CNCMIAPPROGRESSWATCHER_DOCANCEL_EXIT );
	}

/**
 * Called when connection stage changed.
 */
void CNcmIapProgressWatcher::RunL()
	{
	OstTraceFunctionEntry0( CNCMIAPPROGRESSWATCHER_RUNL_ENTRY );
	
	OstTrace1( TRACE_NORMAL, CNCMIAPPROGRESSWATCHER_RUNL_COMPLETION_CODE, ";iStatus.Int()=%d", iStatus.Int() );
	OstTraceExt2( TRACE_NORMAL, CNCMIAPPROGRESSWATCHER_RUNL_INFO, ";iProgressInfoBuf().iStage=%d;iProgressInfoBuf().iError=%d", iProgressInfoBuf().iStage, iProgressInfoBuf().iError );
	

	TInt completionCode = iStatus.Int();
    if (KErrNone == completionCode)
        {
        switch (iProgressInfoBuf().iStage)
            {
            case KConnectionUninitialised:
                {
                iObserver.MipsoHandleClassFatalError(KErrDisconnected);
                break;
                }
            case ENcmPktDrvFatalError:
                {
                iObserver.MipsoHandleClassFatalError(iProgressInfoBuf().iError);
                break;
                }
            default:
                {
                // Request the next notification
                OstTrace0( TRACE_NORMAL, CNCMIAPPROGRESSWATCHER_RUNL_RESTART_WATCHER, "Request the next notification" );                
                Start();
                }
            }
        }
	OstTraceFunctionExit0( CNCMIAPPROGRESSWATCHER_RUNL_EXIT );
	}
