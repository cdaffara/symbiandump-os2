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

#include <e32std.h>
#include "ActiveReadOneOrMoreReader.h"
#include "AcmConstants.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "ReadOneOrMoreObserver.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ActiveReadOneOrMoreReaderTraces.h"
#endif

CActiveReadOneOrMoreReader::CActiveReadOneOrMoreReader(
								MReadOneOrMoreObserver& aParent, 
								RDevUsbcClient& aLdd,
								TEndpointNumber aEndpoint)
 :	CActive(KEcacmAOPriority), 
	iParent(aParent),
	iLdd(aLdd),
	iEndpoint(aEndpoint)
/**
 * Constructor.
 *
 * @param aParent The object that will be notified when read requests 
 * complete.
 * @param aLdd The LDD handle to be used for posting read requests.
 * @param aEndpoint The endpoint to read from.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADONEORMOREREADER_CACTIVEREADONEORMOREREADER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEREADONEORMOREREADER_CACTIVEREADONEORMOREREADER_CONS_EXIT );
	}

CActiveReadOneOrMoreReader::~CActiveReadOneOrMoreReader()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADONEORMOREREADER_CACTIVEREADONEORMOREREADER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CACTIVEREADONEORMOREREADER_CACTIVEREADONEORMOREREADER_DES_EXIT );
	}

CActiveReadOneOrMoreReader* CActiveReadOneOrMoreReader::NewL(
								MReadOneOrMoreObserver& aParent, 
								RDevUsbcClient& aLdd,
								TEndpointNumber aEndpoint)
/**
 * Standard two phase constructor.
 *
 * @param aParent The object that will be notified when read requests 
 * complete.
 * @param aLdd The LDD handle to be used for posting read requests.
 * @param aEndpoint The endpoint to read from.
 * @return Ownership of a new CActiveReadOneOrMoreReader object.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADONEORMOREREADER_NEWL_ENTRY );
	CActiveReadOneOrMoreReader* self = 
		new(ELeave) CActiveReadOneOrMoreReader(aParent, aLdd, aEndpoint);
	OstTraceFunctionExit0( CACTIVEREADONEORMOREREADER_NEWL_EXIT );
	return self;
	}

void CActiveReadOneOrMoreReader::ReadOneOrMore(TDes8& aDes, TInt aLength)
/**
 * Read as much data as the LDD can supply up to the given limit.
 *
 * @param aDes A descriptor into which the data will be read.
 * @param aLength The length to read.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADONEORMOREREADER_READONEORMORE_ENTRY );
	OstTrace1( TRACE_NORMAL, CACTIVEREADONEORMOREREADER_READONEORMORE, 
			"CActiveReadOneOrMoreReader::ReadOneOrMore;aLength=%d", aLength );
	iLdd.ReadOneOrMore(iStatus, iEndpoint, aDes, aLength);
	SetActive();
	OstTraceFunctionExit0( CACTIVEREADONEORMOREREADER_READONEORMORE_EXIT );
	}

void CActiveReadOneOrMoreReader::DoCancel()
/**
 * Cancel an outstanding request.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADONEORMOREREADER_DOCANCEL_ENTRY );
	iLdd.ReadCancel(iEndpoint);
	OstTraceFunctionExit0( CACTIVEREADONEORMOREREADER_DOCANCEL_EXIT );
	}

void CActiveReadOneOrMoreReader::RunL()
/**
 * This function will be called when the request completes. It notifies the 
 * parent class of the completion.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADONEORMOREREADER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CACTIVEREADONEORMOREREADER_RUNL, 
			"CActiveReadOneOrMoreReader::RunL;iStatus=%d", iStatus.Int() );
	iParent.ReadOneOrMoreCompleted(iStatus.Int());
	OstTraceFunctionExit0( CACTIVEREADONEORMOREREADER_RUNL_EXIT );
	}

//
// End of file
