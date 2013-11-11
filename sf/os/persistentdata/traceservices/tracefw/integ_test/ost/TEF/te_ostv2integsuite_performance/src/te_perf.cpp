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
// Example CTestStep derived implementation
//



/**
 @file te_perf.cpp
 @internalTechnology
*/
#include "te_perf.h"
#include "te_ostv2integsuite_defs.h"
#include <e32math.h>
#include "te_perfresultscollector.h"
#include <hal.h>
#include "te_lightlogger.h"
#include "lightlogger.h"
#include "te_tracecontrolcmds.h"
#include "te_perfcsvgenerator.h"
using namespace Ulogger; //CA:added so can use old ulogger api

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////// UPT OUTPUT PLUGIN CONFIG CLASS///////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
CPluginRunConfig::CPluginRunConfig()
	{
	/*Constructor*/
	//iPluginName=0; // initialize to nothing so can tell if test worked? pickup defaults here?
	//iPluginConfiguration=0; //set as default=file
	//iPluginKey=0;
	//iPluginValue=0;
	//iTestType;
	iDataSize=-1; //this is the amount of data to be sent through the output plugin
	iIteration=1;
	
	}

CPluginRunConfig::~CPluginRunConfig()
	{
	/*Destructor*/
	}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CTCMPerformanceWrapper::~CTCMPerformanceWrapper()
/**
 * Destructor
 */
	{
	delete iPluginRunConfig;
	}

CTCMPerformanceWrapper::CTCMPerformanceWrapper()
/**
 * Constructor
 */
	{
	TRunConfigurer::Init(iApiRunConfig);
	}


CTCMPerformanceWrapper* CTCMPerformanceWrapper::NewLC()
	{
	CTCMPerformanceWrapper* self = new (ELeave)CTCMPerformanceWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMPerformanceWrapper* CTCMPerformanceWrapper::NewL()
	{
	CTCMPerformanceWrapper* self=CTCMPerformanceWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMPerformanceWrapper::ConstructL()
	{
	iPluginRunConfig = new(ELeave) CPluginRunConfig;
	}

TAny* CTCMPerformanceWrapper::GetObject()
	{
	return NULL;
	}
TBool CTCMPerformanceWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& aSection, 
					const TInt /*aAsyncErrorIndex*/)
	{
	if(BlockResult()==EPass)
	  	{
		TBool doapicalltests;
		TBool dooutputplugintests;
		TBool dothroughputtests;
		TBool dobuffersizetests;//****to be implemented
		TBool dobuffermodetests;//****to be implemented

		//collect which testypes to run
		if(!GetBoolFromConfig(aSection,_L("runapicalltests"), doapicalltests))
			doapicalltests=ETrue;
		if(!GetBoolFromConfig(aSection,_L("runoutputplugintests"), dooutputplugintests))
			dooutputplugintests=ETrue;
		if(!GetBoolFromConfig(aSection,_L("runthroughputtests"), dothroughputtests))
			dothroughputtests=ETrue;
		if(!GetBoolFromConfig(aSection,_L("runbuffersizetests"), dobuffersizetests))
			dobuffersizetests=ETrue;
		if(!GetBoolFromConfig(aSection,_L("runbuffermodetests"), dobuffermodetests))
			dobuffermodetests=ETrue;

		TInt pluginOnlyTests=0;
	  	//run api tests
		if(doapicalltests)
			SetBlockResult(DoApiTestsL(aSection));

		if(dooutputplugintests)
			{
			pluginOnlyTests=1;
			SetBlockResult(DoPluginTestsL(pluginOnlyTests, aSection));
			}
		
		if(dothroughputtests)
			{
			pluginOnlyTests=2;
			SetBlockResult(DoPluginTestsL(pluginOnlyTests, aSection));
			}

		/*
		 * Implement these in the next iteration
		 *
		 * if(dobuffersizetests)
		SetBlockResult(DoBufferSizeTests());
		if(dobuffermodetests)
		SetBlockResult(DoBufferModeTests());
		*
		*
		*
		*/

	  	}
	 	return ETrue;
	}

TVerdict CTCMPerformanceWrapper::DoApiTestsL(const TTEFSectionName& aSection)
	{
	//initialize and create test run parameters
  	//*******this is quite ugly and could be tidied up into a new routine so isnt in main test step
  	TInt minapiidvalue=0;
  	TInt maxapiidvalue=0;
	TInt pcBool=0;
	THasProgramCounter minpcvalue=ENoProgramCounter;
	THasProgramCounter maxpcvalue=ENoProgramCounter;
	TInt contextBool=0;
	THasThreadIdentification mincontextvalue=ENoThreadIdentification;
	THasThreadIdentification maxcontextvalue=ENoThreadIdentification;
	TBool usesampleevp = EFalse;
	TInt mindatasizevalue=0;
	TInt maxdatasizevalue=0;
	TInt dataincrement=1;
	TBool minexecutionvalue=EFalse;
	TBool typicalexecutionvalue=EFalse;
	TBool testclassificationfiltering=EFalse;
	TBool testmoduleidfiltering=EFalse;
	TBool mintestclassificationmatch=ETrue;
	TBool maxtestclassificationmatch=ETrue;
	TBool mintestmoduleidmatch=EFalse;
	TBool maxtestmoduleidmatch=EFalse;
	TBool appendvalue=ETrue;
	TInt testtype = 0;

	TBool traceenabledvalue=EFalse;

	if(BlockResult() == EPass)
		{
		//TInt error=KErrNotSupported;

	 	//fetch user defined fields from config file (need to ensure there are defaults in the test suite)
	 	//any missing fields, etc, assign default values
	 	//*******remove magic numbers and set as KLit's
	  	INFO_PRINTF1(_L("Fetch user-defined values from config file"));
	
		//initial api
		if(!GetIntFromConfig(aSection,_L("minapiid"),minapiidvalue))
			minapiidvalue=0;
	
		//final api
		if(!GetIntFromConfig(aSection,_L("maxapiid"),maxapiidvalue))
			maxapiidvalue=minapiidvalue;
	
  		//pc parameters
  		if(!GetIntFromConfig(aSection,_L("pc"),pcBool))
			{
  			minpcvalue=EAddProgramCounter;
  			maxpcvalue=ENoProgramCounter;
			}
		else
			{
  			if(pcBool == 1)
   				{
  				maxpcvalue=minpcvalue=EAddProgramCounter;
   				}
   			else
   				{
   				maxpcvalue=minpcvalue=ENoProgramCounter;
   				}
   			}
		
		//context paramaters
		if(!GetIntFromConfig(aSection,_L("context"),contextBool))
			{
			mincontextvalue=EAddThreadIdentification;
			maxcontextvalue=ENoThreadIdentification;
			}
		else
			{
			if(contextBool == 1)
				{
				maxcontextvalue=mincontextvalue=EAddThreadIdentification;
				}
			else
				maxcontextvalue=mincontextvalue=ENoThreadIdentification;
			}
		//loop over data or use sample evp
		if(!GetBoolFromConfig(aSection,_L("testsampleevp"),usesampleevp))
			usesampleevp=ETrue;
		 
		//data size parameters
		if(!GetIntFromConfig(aSection,_L("minevp"),mindatasizevalue))
			mindatasizevalue=0;
		if(!GetIntFromConfig(aSection,_L("maxevp"),maxdatasizevalue))
			maxdatasizevalue=mindatasizevalue;
		if(maxdatasizevalue!=mindatasizevalue)
			{
			if(!GetIntFromConfig(aSection,_L("evpincrement"),dataincrement))
				dataincrement=(maxdatasizevalue-mindatasizevalue)/(10-1);
			}
	
		//minimum execution test
		if(!GetBoolFromConfig(aSection,_L("minexecutiontime"),minexecutionvalue))
			minexecutionvalue=ETrue;
	
		//typical execution test
		if(!GetBoolFromConfig(aSection,_L("reasonablemaxexecutiontime"),typicalexecutionvalue))
			typicalexecutionvalue=ETrue;
	
		//testprimary filter
		if(!GetBoolFromConfig(aSection,_L("testwithclassificationfilteringenabled"),testclassificationfiltering))
			testclassificationfiltering=ETrue;
	
		//testsecondary filter
		if(!GetBoolFromConfig(aSection,_L("testwithmoduleidfilteringenabled"),testmoduleidfiltering))
			testmoduleidfiltering=ETrue;
	
		//testclassificationfiltermatch
		if(!GetBoolFromConfig(aSection,_L("testwithclassificationfiltermatchestrace "),mintestclassificationmatch))
			{
			mintestclassificationmatch=ETrue;
			maxtestclassificationmatch=EFalse;
			}
		else
			maxtestclassificationmatch=mintestclassificationmatch;
		
		//testmoduleidfiltermatch
			if(!GetBoolFromConfig(aSection,_L("testwithmoduleidfiltermatchestrace"),mintestmoduleidmatch))
				{
				mintestmoduleidmatch=ETrue;
				maxtestmoduleidmatch=EFalse;
				}
			else
				maxtestmoduleidmatch=mintestmoduleidmatch;
			
			
		if(!GetBoolFromConfig(aSection,_L("append"),appendvalue))
			appendvalue = EFalse;
	
		traceenabledvalue = EFalse;
	
	#ifdef TE_UPT_TRACE_ENABLED //or could ifdef on SYMBIAN_TRACE_EXECUTABLE_INCLUDE --especially if there's likely to be other configs
		traceenabledvalue = ETrue;
	#endif
	
		//mass of nested for loops... :(
	
		//do the test
	
		INFO_PRINTF1(_L("Do the API tests"));
		CApiCallTest apitest;
		CUptResults manageresults;
	
	
		// write data to csv
		INFO_PRINTF1(_L("Write the api results to csv"));
		
	
		//create the ULogger session
		RULogger logger;
		CUptULogger loggerCommand;
		TInt result= 0;
        result =logger.Connect();
        INFO_PRINTF2(_L("Connection to Ulogger: %d, attempt: 1"), result);
        if (result!=KErrNone)
            {
            INFO_PRINTF1(_L("Connection to Ulogger failed. Will try to connect max. 10 times"));
            for (TInt i=2; i<12;i++)
                {
                User::After(1000);
                result = logger.Connect();
                INFO_PRINTF3(_L("Connection to Ulogger: %d, attempt: %d"), result, i);
                if (result==KErrNone)
                    {
                    break;
                    }
                }
            }
		CClearConfig configIni;
		configIni.ClearL(logger);
	
		//put in test over API CALLS
		testtype=0;
		
		if(usesampleevp==0)
			{
#define LOOP_EVP			
			}
	for(TInt runapiid=minapiidvalue; runapiid<=maxapiidvalue; runapiid++)
			{
			for(TInt runpc=minpcvalue; runpc<=maxpcvalue; runpc++)
				{
				for(TInt runcontext=mincontextvalue; runcontext<=maxcontextvalue; runcontext++)
					{
					TInt rundatasize=mindatasizevalue;
#ifdef LOOP_EVP			
						for(rundatasize=mindatasizevalue; rundatasize<=maxdatasizevalue; rundatasize=rundatasize+dataincrement)
							{
#endif
							{
							for(TBool runclassificationfiltermatch=mintestclassificationmatch; runclassificationfiltermatch<=maxtestclassificationmatch; runclassificationfiltermatch++)
								{
								for(TBool runmoduleidfiltermatch=mintestmoduleidmatch; runmoduleidfiltermatch<=maxtestmoduleidmatch; runmoduleidfiltermatch++)
									{
	
									//now set the parameters to pass to the test step
									TRunConfigurer::SetRunParams(iApiRunConfig, runapiid, (THasThreadIdentification) runcontext, (THasProgramCounter) runpc);
									TRunConfigurer::SetRunConfigs(iApiRunConfig, testclassificationfiltering, testmoduleidfiltering, runclassificationfiltermatch, runmoduleidfiltermatch, minexecutionvalue, typicalexecutionvalue, EFalse, EFalse, EFalse);
		
									//start the ULogger session with filter settings as specified by the user
									__TEST_LOG("about to start ulogger session");
			
									TInt error=loggerCommand.StartFileL(logger,testclassificationfiltering, testmoduleidfiltering, runclassificationfiltermatch, runmoduleidfiltermatch);
									if(error)
										{
										INFO_PRINTF2(_L("start logger returns %d"), error);
										SetBlockResult(EFail);
										}
									
									TApiTestResult apitestresult;
									apitestresult.iRunConfigs = iApiRunConfig; //shallow opy should suffice as we use it between kernel and user side as well.
									apitestresult.iRunResults.iTraceEnabled = traceenabledvalue;
		
									//do the test
									INFO_PRINTF2(_L("Testing API %d"), iApiRunConfig.iApiId);
									error = apitest.CachedTraceTimeL(iApiRunConfig, apitestresult);
									if(error)
										{
										INFO_PRINTF2(_L("ERROR: CachedTraceTimeL failed, error %d"), error);
										SetBlockResult(EFail);
										}
									error = apitest.NonCachedTraceTimeL(iApiRunConfig, apitestresult);
									if(error)
										{
										INFO_PRINTF2(_L("ERROR: NonCachedTraceTimeL failed, error %d"), error);
										SetBlockResult(EFail);
										}
									error = apitest.StackUsageL(iApiRunConfig, apitestresult);							
									if(error)
										{
										INFO_PRINTF2(_L("ERROR: StackUsageL failed, error %d"), error);
										SetBlockResult(EFail);
										}
									
									//(apitest.DoTestL(*iApiRunConfig))->Copy(apitestresult);
									//stop the ULogger session
									__TEST_LOG("about to stop ulogger session");
									loggerCommand.StopAndDeleteFile(logger);
			
									//collect the results
									//INFO_PRINTF1(_L("Collect the api results"));
			
									//store the API test result
									manageresults.StoreApiResultsL(apitestresult);
									//error = manageresults.StoreApiResultsL(apitestresult);
									//if(error)
										//{
										//INFO_PRINTF1(_L("Write the api results"));
										//SetBlockResult(EFail);
										//}
									__UHEAP_MARK;
									manageresults.WriteUptResultsL(KApiCSVFile, appendvalue, testtype);
									__UHEAP_MARKEND;
									}
			
								}
#ifdef LOOP_EVP			
								}
#endif
							}
					}
					}
				}
		result = logger.Connect();
        INFO_PRINTF2(_L("Connection to Ulogger: %d, attempt: 1"), result);
        if (result!=KErrNone)
            {
            INFO_PRINTF1(_L("Connection to Ulogger failed. Will try to connect max. 10 times"));
            for (TInt i=2; i<12;i++)
                {
                User::After(1000);
                result = logger.Connect();
                INFO_PRINTF3(_L("Connection to Ulogger: %d, attempt: %d"), result, i);
                if (result==KErrNone)
                    {
                    break;
                    }
                }
            }
		configIni.ClearL(logger);
		}
	//put in all the error returns
	//put in test step result returns
	INFO_PRINTF1(_L("End of test run"));
	return BlockResult();
	}

TVerdict CTCMPerformanceWrapper::DoPluginTestsL(const TInt& aPluginOnly, const TTEFSectionName& aSection)
	{
	//define default plugin configurations
	_LIT(KDefaultPlugin,"uloggerfileplugin");
	_LIT(KOutputPath, "output_path");
	_LIT(KLogDefaultFileName, "C:\\logs\\defaultplugin.utf");
	_LIT(KOutputCSVFile,"c:\\te_plugintestresults.csv");

	//create lightlogger file for the timestamps
	//NB this is a temporary solution until the next iteration
	//where ulogger source itself will be instrumented with test code

	__CREATE_LOG(false);
	__LOG("About to record plugin timestamps");
	__HIRES_RESOLUTION;

	//initialize and create test run parameters
	TInt plugindata=0;
	TInt pluginiterations=0;
	TBool appendvalue;
	TInt testtype = 0;
	TPtrC plugintempname(KDefaultPlugin);
	TPtrC plugintempkey(KOutputPath);
	TPtrC plugintempvalue(KLogDefaultFileName);
	TPtrC8 plugintestype;

	//first identify the testtype
	if(aPluginOnly==1)
		{
		testtype=1;
		plugintestype.Set(KOutputPluginTest);
		__LOG("NB: recording plugin only timestamps");
		}
	else
		{
		testtype=2;
		plugintestype.Set(KThroughputTest);
		__LOG("NB: recording full throughput timestamps");
		}


	//TInt ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aSettings); if>0
	//currently this test suite  assumes that there is only one set of configuration settings


	//Collect the output plugin test run configuration

	//plugin name
	if(!GetStringFromConfig(aSection,_L("plugintestused"),plugintempname))
		{
		;
		}

	//the following three lines are an irritating get around to sort out the fact that
	//the routine for GetStringFromConfig() only works with TPtrC16 and ULogger expects TPtrC8
	TBuf8<0x100> pluginname;
	pluginname.Copy(plugintempname);


	//plugin configurations
	if(!GetStringFromConfig(aSection,_L("plugintestconfigurationkey"),plugintempkey))
		{
		;
		}
	//the routine for GetStringFromConfig() only works with TPtrC16 and ULogger expects TPtrC8
	TBuf8<0x100> pluginkey;
	pluginkey.Copy(plugintempkey);

	if(!GetStringFromConfig(aSection,_L("plugintestconfigurationvalue"),plugintempvalue))
		{
		;
		}
	//the routine for GetStringFromConfig() only works with TPtrC16 and ULogger expects TPtrC8
	TBuf8<0x100> pluginvalue;
	pluginvalue.Copy(plugintempvalue);

	//datasize to pass through plugin in bytes
	if(!GetIntFromConfig(aSection,_L("plugintestdatasize"),plugindata))
		plugindata=1024;

	//number of test iterations for stress test over time
	if(!GetIntFromConfig(aSection,_L("plugintestiterations"), pluginiterations))
		pluginiterations=1;

	//option for appending to results file
	if(!GetBoolFromConfig(aSection,_L("append"),appendvalue))
		appendvalue=ETrue;

	TBool traceon=ETrue;
#ifndef TE_UPT_TRACE_ENABLED //or could ifdef on SYMBIAN_TRACE_EXECUTABLE_INCLUDE --especially if there's likely to be other configs
	traceon=EFalse;
	INFO_PRINTF1(_L("WARNING! Tracing is disabled so output plugin tests not run "));
#endif

	//mass of nested for loops... :(
	if(traceon)
		{
		//do the test
		INFO_PRINTF1(_L("Do the plugin tests"));
		CPluginTestResult plugintestresult;
		CPluginTests plugintest;
		CUptResults manageresults;


		//create the ULogger session
		RULogger logger;
		CUptULogger loggerCommand;

		//set the run parameters

		iPluginRunConfig->iPluginName.Set(pluginname);
		iPluginRunConfig->iPluginConfiguration.SetKey(pluginkey);
		iPluginRunConfig->iPluginConfiguration.SetValue(pluginvalue);
		iPluginRunConfig->iPluginKey.Set(pluginkey);
		iPluginRunConfig->iPluginValue.Set(pluginvalue);
		iPluginRunConfig->iTestType.Set(plugintestype);
		iPluginRunConfig->iDataSize=plugindata;
		TBool classificationFiltering = ETrue;
		TBool moduleIdFiltering = ETrue;
		TBool classificationFilterMatch = ETrue;
		TBool moduleIdFilterMatch = ETrue;
	
		//we force this to be hardcoded as it makes no sense to have a mismatch here
		//i.e. to have trace compiled in but filters disabled

		//iterates over tests for stress testing
		TInt error=KErrNone;

		for(TInt iterate=1; iterate<=pluginiterations; iterate++)
			{

				iPluginRunConfig->iIteration=iterate;

				//start the ULogger session with filter settings as specified by the user
				//__TEST_LOG("about to start ulogger session");
				INFO_PRINTF1(_L("about to start logger"));

				error=loggerCommand.StartGeneralL(logger, iPluginRunConfig->iPluginName, iPluginRunConfig->iPluginConfiguration, classificationFiltering, moduleIdFiltering, classificationFilterMatch, moduleIdFilterMatch);

				INFO_PRINTF2(_L("start logger returns %d"), error);

				//do the test
				plugintestresult.Copy(*plugintest.DoTestL(*iPluginRunConfig));

				//stop the ULogger session
				//__TEST_LOG("about to stop ulogger session");
				loggerCommand.Stop(logger);

				//collect the results
				INFO_PRINTF1(_L("Collect the plugin results"));

				//store the plugin test result
				manageresults.StorePluginResultsL(plugintestresult);
				// write data to csv
				INFO_PRINTF1(_L("Write the plugin results to csv"));
				manageresults.WriteUptResultsL(KOutputCSVFile, appendvalue, testtype);

			}
		SetBlockResult(EFail);
		if(error==KErrNone)
			SetBlockResult(EPass);

	}

	return BlockResult();
}


TVerdict CTCMPerformanceWrapper::DoBufferSizeTests()
	{
	TInt error=KErrNone;
	SetBlockResult(EFail);
	if(error==KErrNone)
		SetBlockResult(EPass);
	return BlockResult();
	}

TVerdict CTCMPerformanceWrapper::DoBufferModeTests()
	{
	TInt error=KErrNone;
	SetBlockResult(EFail);
	if(error==KErrNone)
		SetBlockResult(EPass);
	return BlockResult();
	}


