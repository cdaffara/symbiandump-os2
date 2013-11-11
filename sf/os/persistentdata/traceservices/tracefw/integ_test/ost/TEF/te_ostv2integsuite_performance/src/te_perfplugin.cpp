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
// Ost Performance Tests Output Plugin Test Object
//



/**
 @file te_perfplugin.cpp
 @internalTechnology
 @prototype
*/


#include "te_perfplugin.h"
#include "te_tracecontrolcmds.h"
#include "te_instrumentationpoints.h"
#include "te_perf.h"
#include "te_dataparameters.h"
#include "lightlogger.h"


CPluginTestResult::CPluginTestResult()
	{
	/*Constructor*/
	//iPluginName;
	//iPluginConfiguration;
	//iPluginKey;
	//iPluginValue;
	//iTestType;
	/*
	TInt32 throughput=-1; //in bytes/sec, calculated from the amount of data sent divided by the// difference between the initial and final timestamps
	TInt32 dataSize=0; //may not need
	TInt iteration=0;
	TInt64 latency=-1; //in nanoseconds-> to be implemented once ULogger instrumented with testcode
	*/
	}

CPluginTestResult::~CPluginTestResult()
	{
	/*Destructor*/
	}

TInt CPluginTestResult::Copy(const CPluginTestResult& aPluginTestResult)
	{
	TInt err=0;

	//iPluginTestResult->iPluginName.Set(aPluginRunConfig.iPluginName);
	//iPluginTestResult->iPluginConfiguration=aPluginRunConfig.iPluginConfiguration;
	//iPluginTestResult->iPluginKey.Set(aPluginRunConfig.iPluginKey);
	//iPluginTestResult->iPluginValue.Set(aPluginRunConfig.iPluginValue);
	//iPluginTestResult->iTestType.Set(aPluginRunConfig.iTestType);
	iThroughput=aPluginTestResult.iThroughput; //in bytes/sec, calculated from the amount of data sent divided by the// difference between the initial and final timestamps
	iDataSize=aPluginTestResult.iDataSize; //may not need
	iIteration=aPluginTestResult.iIteration;
	iLatency=aPluginTestResult.iLatency;

	return err;
	}


CPluginTests::CPluginTests()
	{
	/*Constructor*/

	__CREATE_LOG(false);
	}

CPluginTests::~CPluginTests()
	{
	/*Destructor*/
	delete iPluginTestResult;
	}


/** This user-side method takes the parameters passed by the user to carry out a single plugin performance test, independent of UTrace
and to store the test results
@param aPluginName is the identity of the plugin to be tested defined as a TPtrc8 for consistency with ULogger's parameter definitions
@param aPluginSetting is the settings of the plugin to be tested defined as a TPluginConfiguration for consistency with ULogger's parameter definitions
@param aTestDataSize is the size of the data in bytes, specified as TInt, that the user wants to pass through the plugin.
		If aTestDataSize=zero then,
			 {if aTestDuration=zero; default settings are picked up,
			 if aTestDuration!=zero; the test assumes the user wants a continuous stream of data}
@param aTestDuration is the time duration in seconds, specified as a TInt, that the user wants data to pass through the plugin.
		If aTestDuration=zero then,
			 {if aTestDataSize=zero; default settings are picked up,
			 if aTestDataSize!=zero; the test assumes the user wants the full set of data to be passed regardless of the time taken}
@return CPluginTestResult is the  Plugin test result struct object returned to the calling program, which contains all the test results from the relevent test run
 */
CPluginTestResult* CPluginTests::DoTestL(const CPluginRunConfig& aPluginRunConfig)
//const COutputPluginTestResult* COutputPluginTests::DoTestL(const TPtrC8& aPluginName, const TPluginConfiguration& aPluginSetting, const TInt& aTestDataSize, const TInt& aTestIteration)
	{
	//TTraceTester tracecall;
	if (iPluginTestResult == NULL)
        {
        iPluginTestResult = new(ELeave) CPluginTestResult;   
        }

	TInt numberOfTraces=aPluginRunConfig.iDataSize/16; //update magic number to a size of (trace)/bytes
	//do a baseline to remove loop effects for throughput tests?????

	//removed logging of timestamp because of a warning...
//	if(aPluginRunConfig.iTestType.Compare(KThroughputTest))
//		__LOGTIMESTAMP("Timestamp before call to trace - t1");

	TTraceContext attrs(KComponentId, KGroupId, KContextId, KPc);
	for(TInt iterate=0; iterate<numberOfTraces; iterate++)
		{
		//use this commented routine if want a more generic version for maintainability
		//tracecall.SendTraceL()
		OstTrace(attrs, KTraceId);
		}
	iPluginTestResult->iPluginName.Set(aPluginRunConfig.iPluginName);
	iPluginTestResult->iPluginConfiguration=aPluginRunConfig.iPluginConfiguration;
	iPluginTestResult->iPluginKey.Set(aPluginRunConfig.iPluginKey);
	iPluginTestResult->iPluginValue.Set(aPluginRunConfig.iPluginValue);
	iPluginTestResult->iTestType.Set(aPluginRunConfig.iTestType);
	iPluginTestResult->iDataSize=aPluginRunConfig.iDataSize; //may not need
	iPluginTestResult->iIteration=aPluginRunConfig.iIteration;

	//iOutputPluginTestResult->iLatency; //in nanoseconds-> to be implemented once ULogger instrumented with testcode
	return iPluginTestResult;
	}



