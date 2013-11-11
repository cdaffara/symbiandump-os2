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
*
*/

/**
 @file
*/

#include "CUsbScheduler.h"
#include "CUsbServer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbSchedulerTraces.h"
#endif



/**
 * The CUsbScheduler::NewL method
 *
 * Creates a new Active scheduler
 *
 * @internalComponent
 */
CUsbScheduler* CUsbScheduler::NewL()
	{
	OstTraceFunctionEntry0( CUSBSCHEDULER_NEWL_ENTRY );
	
	CUsbScheduler* self = new(ELeave) CUsbScheduler;
	OstTraceFunctionExit0( CUSBSCHEDULER_NEWL_EXIT );
	return self;
	}

/**
 * The CUsbScheduler::~CUsbScheduler method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbScheduler::~CUsbScheduler()
	{
	// Note that though we store a pointer to the server,
	// we do not own it (it is owned by the cleanup stack)
	// and our pointer is only used to error the server
	// if we have been given a reference to it.
	}

/**
 * The CUsbScheduler::SetServer method
 *
 * Give us a reference to the server
 *
 * @internalComponent
 * @param	aServer	A reference to the server
 */
void CUsbScheduler::SetServer(CUsbServer& aServer)
	{
	iServer = &aServer;	
	}

/**
 * The CUsbScheduler::Error method
 *
 * Inform the server that an error has occurred
 *
 * @internalComponent
 * @param	aError	Error that has occurred
 */
void CUsbScheduler::Error(TInt aError) const
	{
	OstTrace1( TRACE_NORMAL, CUSBSCHEDULER_ERROR, "CUsbScheduler::Error;aError=%d", aError );
	

	if (iServer)
		{
		iServer->Error(aError);
		}
	}

