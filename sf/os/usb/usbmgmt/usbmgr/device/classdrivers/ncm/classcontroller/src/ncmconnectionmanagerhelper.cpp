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

#include <e32debug.h>

#include "ncmconnectionmanagerhelper.h"
#include "ncmiapconnectionobserver.h"
#include "ncmconnectionmanager.h"

// For OST tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmconnectionmanagerhelperTraces.h"
#endif

/**
 * Constructor.
 * @param aObserver The object which is interested in the IAP connection completion
 * @param aConnection The reference of the NCM connection to be used
 */
CNcmConnectionManHelper::CNcmConnectionManHelper(MNcmConnectionObserver& aObserver,
        RConnection& aConnection)
    : CActive(CActive::EPriorityStandard)
	, iObserver(aObserver)
	, iConnection(aConnection) 
	{
	OstTraceFunctionEntryExt( CNCMCONNECTIONMANHELPER_CNCMCONNECTIONMANHELPER_ENTRY, this );
	CActiveScheduler::Add(this);		
	OstTraceFunctionExit1( CNCMCONNECTIONMANHELPER_CNCMCONNECTIONMANHELPER_EXIT, this );
	}


/**
 * Destructor.
 */
CNcmConnectionManHelper::~CNcmConnectionManHelper()
	{	
	OstTraceFunctionEntry1( CNCMCONNECTIONMANHELPER_CNCMCONNECTIONMANHELPER_ENTRY_DESTRUCTOR, this );
	if (IsActive())
	    {
	    Cancel();
	    }
	else
	    {
	    Stop();
	    }
	OstTraceFunctionExit1( CNCMCONNECTIONMANHELPER_CNCMCONNECTIONMANHELPER_ENTRY_DESTRUCTOR_EXIT, this );
	}

/**
 * Start the connection.
 * @param aPref NCM connect preference
 */
void CNcmConnectionManHelper::Start(TConnPref& aPref)
	{	
	OstTraceFunctionEntryExt( CNCMCONNECTIONMANHELPER_START_ENTRY, this );
	if (!IsActive())
		{
		iConnection.Start(aPref, iStatus);
		SetActive();
		}
	OstTraceFunctionExit1( CNCMCONNECTIONMANHELPER_START_EXIT, this );
	}

/**
 * Stop the connection.
 */
void CNcmConnectionManHelper::Stop()
	{
    OstTraceFunctionEntry1( CNCMCONNECTIONMANHELPER_STOP_ENTRY, this );
   
	TInt err = iConnection.Stop(RConnection::EStopAuthoritative);
	OstTrace1( TRACE_NORMAL, CNCMCONNECTIONMANHELPER_STOP_RETURN_CODE, "Return code from Stop() is:;err=%d", err );
	OstTraceFunctionExit1( CNCMCONNECTIONMANHELPER_STOP_EXIT, this );
	}
/**
 * Implements cancellation of an outstanding request.
 */
void CNcmConnectionManHelper::DoCancel()
	{
    OstTraceFunctionEntry0( CNCMCONNECTIONMANHELPER_DOCANCEL_ENTRY );

	TInt err = iConnection.Stop(RConnection::EStopAuthoritative);

    OstTrace1( TRACE_NORMAL, CNCMCONNECTIONMANHELPER_DOCNCEL, "Return code from Stop() is:;err=%d", err );
	OstTraceFunctionExit0( CNCMCONNECTIONMANHELPER_DOCANCEL_EXIT );
	}

/**
 * Called when connection completed.
 */
void CNcmConnectionManHelper::RunL()
	{	
	OstTraceFunctionEntry0( CNCMCONNECTIONMANHELPER_RUNL_ENTRY );
	iObserver.MicoHandleConnectionComplete(iStatus.Int());
	OstTraceFunctionExit0( CNCMCONNECTIONMANHELPER_RUNL_EXIT );
	}
