// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// UTrace Performance Tests UTrace Call Controller
//



/**
 @file te_perftracetester.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTUTRACETESTER_H
#define TE_UPTUTRACETESTER_H


#include <opensystemtrace.h>
#include "te_instrumentation_enum.h"
#include "te_perf.h"


/**
 * Use this class to call to get the traces.
 * It will behind the scene work out if its a kernel or user side call
 */
class TUptTraceTester
{
public:
	/**
	 * @param aApiRunConfig Configuration parameters that both set and return values
	 * @return Returns if there was an error or not.
	 */
	TInt SendTraceL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults); //pass the configuration here

	/**
	 * This returns the average execution time calculated over
	 * KMeasurementTime and KNumberOfTraceCallIterations
	 * @param aApiRunConfig Configuration parameters that both set and return values
	 * @return Returns if there was an error or not.
	 */
	TInt TimeTraceL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults);

	/**
	 * Makes a trace call to either kernel or user side
	 * and returns the time in the parameter aApiRunConfig.iApiTraceTime
	 */
	TInt TimeTraceCallL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults);

	/**
	 * Makes a call to user side, returning the time in the
	 * parameter aApiRunConfig.iApiTraceTime
	 */
	TInt TimeUsersideTraceCallsL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults);

	/**
	 */
	TInt GenerateEvpDataField(const TInt aDataSize, TInt8* aData);

};

#endif


