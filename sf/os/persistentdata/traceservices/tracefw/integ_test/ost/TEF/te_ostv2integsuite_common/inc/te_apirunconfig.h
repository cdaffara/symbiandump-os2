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
//



/**
 @file te_apirunconfig.h
 @internalTechnology
 @prototype
*/
#ifndef TE_APIRUNCONFIG_H
#define TE_APIRUNCONFIG_H

#include <opensystemtrace_types.h>



/**
 * This is used to pass configuration data to the actual trace calls.
 * This contains all the settings for the things we need to do, but not the results of what it did.
 * The results are kept in TApiRunResults
 * It needs to not have any methods and not to inherit from CBase because
 * we need to copy this data between user and kernel side.
 */
class TApiRunConfig
	{
public:
	//trace point parameters
	TInt				iApiId; //as an enum of utrace api methods and printf methods
	//Not too sure why these are here as they are as variable as any of the other trace data.
	THasThreadIdentification		iHasContext;
	THasProgramCounter 	iHasProgramCounter;

	/**
	 * Size of the binary data you want to pass through the TAny trace call...
	 * This is the EVP over and above the standard trace header
	 */
	TInt			    iSizeOfUptData;
	//The rest of the trace data is set up via global variables rather than here...

	/**
	 * Set up what kinds of tests to do
	 */
	TBool			iDoTestClassificationFiltering;
	TBool			iDoTestModuleIdFiltering;
	TBool			iDoTestClassificationFilterMatch;
	TBool			iDoTestModuleIdFilterMatch;
	TBool			iDoMinExecution;
	TBool			iDoTypicalExecution;
	TBool	 		iDoTrace;
	TBool			iDoStack;
	TBool			iDoFlush;
	};

/**
 * There is a one-to-one mapping between this and the TApiRunConfig
* It may also contain temporary data as a means of returning results back from a call
 * It needs to not have any methods and not to inherit from CBase because
 * we need to copy this data between user and kernel side.
 */
class TApiRunResults
	{
	public:
	/**
	 * The time it took to make the call for that set-up
	 */
	TUint32		iApiTraceTime;
	/**
	 * The number of times the api got called within the time limit
	 */
	TInt		iTimeCount;
	/**
	 * Resulting stack usage
	 */
	TInt		iApiStackUsage;
	/**
	 * Resulting heap usage
	 * Not implemented yet
	 */
	TInt		iApiHeapUsage; //not used yet
	/**
	 * This is used to check if the trace was compiled in or not
	 */
	TBool		iTraceEnabled;
	/**
	 * This is only used to store the result for passing values back from kernel side.
	 * It might not be the best place to keep it...
	 */
	TInt		iError;
	};


/**
 * This provides some convenience functions for
 * TApiRunConfig and TApiTestResult since they can't have methods because of the kernel side copying
 */
class TRunConfigurer
	{
public:
	static void Init(TApiRunConfig& aConfig);
	static void Init(TApiRunResults& aResults);

	static void SetRunParams(TApiRunConfig& aConfig, TInt aApiId, THasThreadIdentification aHasContext, THasProgramCounter aHasProgramCounter);
	static void SetRunConfigs(TApiRunConfig& aConfig, TBool aDoTestClassificationFiltering, TBool aDoTestModuleIdFiltering, TBool aDoTestClassificationFilterMatch, TBool aDoTestModuleIdFilterMatch, TBool aDoMinExecution, TBool aDoTypicalExecution, TBool aDoTrace, TBool aDoStack, TBool aDoFlush);
	};



/**
 * This is used to keep track of all the results for a particular API, i.e. cached,
 * non-cashed, stackbase heap etc.
 * Basically it will set up the tests as in the different types,
 * whereas TApiRunConfig contains things
 * that are only rellevant at the stage where we are actually calling the API.
 */
class TApiTestResult
	{
public:
	TApiTestResult();
public:
	/**
	 * Used to keep track of what call was made etc.
	 */
	TApiRunConfig iRunConfigs;
	/**
	 * Used to keep track of iApiStackUsage and iApiHeapUsage etc
	 */
	TApiRunResults iRunResults;

	/**
	 * Time results in nano seconds
	 * This is calculated from several different runs where the intermediate result is
	 * stored in TApiRunResults.iApiTraceTime
	 */
	TInt32		iMinimumExecutionTime;
	/**
	 * Time results in nano seconds
	 * This is calculated from several different runs where the intermediate result
	 * is stored in TApiRunResults.iApiTraceTime
	 */
	TInt32		iTypicalExecutionTime;
	/**
	 * Just to tell what the size of the APi call is, i.e. 32byte, 2*32byte or some other size...
	 * This is actually "static" as in, you would know what the size is depending on what API id you chose.
	 */
	TInt		iApiSize;
	};



#endif
