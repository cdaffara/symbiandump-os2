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
#include "ActiveReader.h"
#include "AcmConstants.h"
#include "AcmPanic.h"
#include "ReadObserver.h"
#include "AcmUtils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ActiveReaderTraces.h"
#endif

CActiveReader::CActiveReader(MReadObserver& aParent, RDevUsbcClient& aLdd, TEndpointNumber aEndpoint)
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
	OstTraceFunctionEntry0( CACTIVEREADER_CACTIVEREADER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEREADER_CACTIVEREADER_CONS_EXIT );
	}

CActiveReader::~CActiveReader()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADER_CACTIVEREADER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CACTIVEREADER_CACTIVEREADER_DES_EXIT );
	}

CActiveReader* CActiveReader::NewL(MReadObserver& aParent, 
								   RDevUsbcClient& aLdd,
								   TEndpointNumber aEndpoint)
/**
 * Standard two phase constructor.
 *
 * @param aParent The object that will be notified when read requests 
 * complete.
 * @param aLdd The LDD handle to be used for posting read requests.
 * @param aEndpoint The endpoint to read from.
 * @return Ownership of a new CActiveReader object.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADER_NEWL_ENTRY );
	CActiveReader* self = new(ELeave) CActiveReader(aParent, aLdd, aEndpoint);
	OstTraceFunctionExit0( CACTIVEREADER_NEWL_EXIT );
	return self;
	}

void CActiveReader::Read(TDes8& aDes, TInt aLen)
/**
 * Read the given length of data from the LDD.
 *
 * @param aDes A descriptor into which to read.
 * @param aLen The length to read.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADER_READ_ENTRY );
	iLdd.Read(iStatus, iEndpoint, aDes, aLen); 
	SetActive();
	OstTraceFunctionExit0( CACTIVEREADER_READ_EXIT );
	}

void CActiveReader::DoCancel()
/**
 * Cancel an outstanding read.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADER_DOCANCEL_ENTRY );
	iLdd.ReadCancel(iEndpoint);
	OstTraceFunctionExit0( CACTIVEREADER_DOCANCEL_EXIT );
	}

void CActiveReader::RunL()
/**
 * This function will be called when the read completes. It notifies the 
 * parent class of the completion.
 */
	{
	OstTraceFunctionEntry0( CACTIVEREADER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CACTIVEREADER_RUNL, "CActiveReader::RunL;RunL iStatus=%d", iStatus.Int() );
	iParent.ReadCompleted(iStatus.Int());
	OstTraceFunctionExit0( CACTIVEREADER_RUNL_EXIT );
	}

//
// End of file
