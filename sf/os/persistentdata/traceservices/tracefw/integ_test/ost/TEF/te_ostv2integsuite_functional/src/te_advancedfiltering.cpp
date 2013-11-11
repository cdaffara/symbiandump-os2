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
 @file te_tracecollectorclient.cpp
 @internalTechnology
*/
#include "te_advancedfiltering.h"
#include "te_ostv2integsuite_defs.h"
#include "te_instrumentationpoints.h"
#include "te_tracecontrolcmds.h"
#include "te_advancedfilteringTraces.h"
#include <opensystemtrace.h>
#include <uloggerclient.h>

using namespace Ulogger; //CA:added so can use old ulogger api
#ifdef TE_UPT_TRACE_ENABLED
_LIT(KTextvalue, "C:\\logs\\ULoggerStartFileTestLog.utf");
#endif
/*******************************************************************************/
/*********************************Primary Filter Test***************************/
/*******************************************************************************/

CTCMAdvancedGroupIDFilteringWrapper::~CTCMAdvancedGroupIDFilteringWrapper()
/**
 * Destructor
 */
	{
	}

CTCMAdvancedGroupIDFilteringWrapper::CTCMAdvancedGroupIDFilteringWrapper()
/**
 * Constructor
 */
	{
	}

CTCMAdvancedGroupIDFilteringWrapper* CTCMAdvancedGroupIDFilteringWrapper::NewLC()
	{
	CTCMAdvancedGroupIDFilteringWrapper* self = new (ELeave)CTCMAdvancedGroupIDFilteringWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMAdvancedGroupIDFilteringWrapper* CTCMAdvancedGroupIDFilteringWrapper::NewL()
	{
	CTCMAdvancedGroupIDFilteringWrapper* self=CTCMAdvancedGroupIDFilteringWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMAdvancedGroupIDFilteringWrapper::ConstructL()
	{
	}

TAny* CTCMAdvancedGroupIDFilteringWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMAdvancedGroupIDFilteringWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/,
					const TInt /*aAsyncErrorIndex*/)
	{
	  if(BlockResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
		TBool OstStatus=EFalse;
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(KTextvalue);
		OstStatus=ETrue;
#endif
		TInt result=0;
		RULogger logger;

		/**************First set primary filter*************/

		result=logger.Connect();
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
		INFO_PRINTF2(_L("connect err %d"), result);

		INFO_PRINTF1(_L("Test setting a single primary filter with the filter, rather than an array"));

		CArrayFixFlat<TUint8> *setfilterarray = new (ELeave)CArrayFixFlat<TUint8>(1); //CA:added so can use old ulogger api
		CleanupStack::PushL(setfilterarray); //CA:added
		TGroupId setfilter = KGroupId;
		setfilterarray->AppendL(setfilter); //CA:added so can use old ulogger api
		CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		CleanupStack::PushL(getfilter); //CA:added
		CArrayFixFlat<TUint8> *removefilterarray = new (ELeave)CArrayFixFlat<TUint8>(1); //CA:added so can use old ulogger api
		CleanupStack::PushL(removefilterarray); //CA:added
		TGroupId removefilter = setfilter;
		removefilterarray->AppendL(removefilter); //CA:added so can use old ulogger api

		result=logger.SetPrimaryFiltersEnabled(*setfilterarray,ETrue);//C.A. previously:Result=logger.EnableClassification(setfilter);

		if(result==KErrNone)
			INFO_PRINTF2(_L("Set single Primary filter has been set with single filter, %d, check log"), setfilter);
		else
			INFO_PRINTF2(_L("Set single Primary filter was not set, error returned, %d"), result);

		//logger.EnableClassificationFiltering();//C.A.: already enabled by default in old ulogger

		/**************Now get the GroupId filter set above************/

		logger.GetPrimaryFiltersEnabled(*getfilter);//C.A.: previously:logger.GetEnabledClassifications(*getfilter);
		INFO_PRINTF1(_L("Get Primary filter returns the following filters"));
		for(TInt i=0;i!=getfilter->Count();i++)
		{
			INFO_PRINTF2(_L("get filter returns %d "),getfilter->At(i));
		}
		if(getfilter->At(0)==setfilter)
		{
            result=0;
			INFO_PRINTF1(_L("Get primary filter successful"));
		}
		else
		{
			INFO_PRINTF2(_L("Get primary filter failed.\n NB: getfilter->Count is %d"), getfilter->Count());
			result=1;
		}
		getfilter->Reset();
		/*****************Now remove the above set filter***************/

		if(result==0)
		{

			INFO_PRINTF1(_L("Test disabling a single primary filter with the filter, rather than an array"));

			result=logger.SetPrimaryFiltersEnabled(*removefilterarray,EFalse);//C.A previously:Result=logger.DisableClassification(removefilter);
			INFO_PRINTF2(_L("Remove GroupId filters single filter with single filter not array, 0 if succesful: %d"), result);
			logger.GetPrimaryFiltersEnabled(*getfilter);//CA previously:logger.GetEnabledClassifications(*getfilter);
			if(getfilter->Count()==0&&result==KErrNone)
			{
				INFO_PRINTF1(_L("Remove primary filter successful"));
				result=0;
			}
			else
			{
				if(result!=0)
					INFO_PRINTF1(_L("Remove primary filter failed, as filter has not been set in the config file"));
				else
					INFO_PRINTF1(_L("Remove primary filter failed, as able to pass same filter option to RemovePrimaryFilter"));
				result=1;
			}
		}
		getfilter->Reset();

		/****Now if the above steps have passed proceed to testing enable/disable primary filtering***/

		if(result==1)
			SetBlockResult(EFail);
		else
		{
			INFO_PRINTF1(_L(" Testing GroupId filtering enabled/ disabled "));

			//reset config file
			configIni.ClearL(logger);


			/***********************************************************/
			/***enable GroupId, enable GroupId filtering*/
			/***********************************************************/

			INFO_PRINTF1(_L("\n\n Enable test GroupId, enable GroupId filtering.\n Testing error returns and ensuring a trace statement IS carried out "));
			SetBlockResult(TestGroupIdFilteringL(logger, ETrue, ETrue));

			/***********************************************************/
			/***enable GroupId, disable GroupId filtering*/
			/***********************************************************/

			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("\n\n Enable test GroupId, disable GroupId filtering.\n Testing error returns and ensuring a trace statement IS carried out + lots of kernel traces "));
				SetBlockResult(TestGroupIdFilteringL(logger, ETrue, EFalse));
				}

			/***********************************************************/
			/***disable GroupId, disable GroupId filtering*/
			/***********************************************************/

			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("\n\n Disable test GroupId, disable GroupId filtering.\n Testing error returns and ensuring a trace statement IS carried out + lots of kernel traces"));
				SetBlockResult(TestGroupIdFilteringL(logger, EFalse, EFalse));
				}

			/***********************************************************/
			/***disable GroupId, enable GroupId filtering*/
			/***********************************************************/

			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("\n\n Disable test GroupId, enable GroupId filtering.\n Testing error returns and ensuring a trace statement IS NOT carried out "));
				SetBlockResult(TestGroupIdFilteringL(logger, EFalse, ETrue));
				}

		}

		if (BlockResult()==EPass&&OstStatus)
			{
#ifdef TE_UPT_TRACE_ENABLED
			if(!ReadFromLogFile(KTextvalue))
				{
				TInt err=CompareTraces();
				if(err)
					{
					INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), err);
					SetBlockResult(EFail);
					}
				else
					SetBlockResult(EPass);
				}
#endif
			}


		//  **************   Block end ****************

		configIni.ClearL(logger);
		logger.Close();
		CleanupStack::Pop(3);
		if(getfilter)
			delete getfilter;
		if(setfilterarray)
			delete setfilterarray;
		if(removefilterarray)
			delete removefilterarray;
		}
	  return ETrue;
	}


TVerdict CTCMAdvancedGroupIDFilteringWrapper::TestGroupIdFilteringL(RULogger& aLogger, const TBool aFilter, const TBool aFiltering)
/**
 * @return - TVerdict code
 * standard function to test error returns and GroupId filtering
 */
	{
 	TInt Result = 0;
	CArrayFixFlat<TUint8> *setfilterarray = new (ELeave)CArrayFixFlat<TUint8>(256); //CA:added so can use old ulogger api
	CleanupStack::PushL(setfilterarray); //CA:added
	setfilterarray->AppendL(KGroupId); //CA:added so can use old ulogger api
	//enable/disable filter
	aLogger.SetPrimaryFiltersEnabled(*setfilterarray,aFilter);

	//C.A. previously this but only need line above:
/*	if(aFilter)
		aLogger.EnableClassification(KGroupId);
	else
		aLogger.DisableClassification(KGroupId);*/


	//enable/disabled filtering
	if(aFiltering){}
		//Result = aLogger.EnableClassificationFiltering();//CA:enabled by default
	else //enable all GroupIds between 0 and 255
		//C.A. previously just :Result = aLogger.DisableClassificationFiltering();
		{
		TInt i;
		setfilterarray->Delete(0,setfilterarray->Count());
		for (i=0;i<=255;i++)
			{
			setfilterarray->AppendL((TUint8)i);
			}
		Result = aLogger.SetPrimaryFiltersEnabled(*setfilterarray,ETrue);
		}
	if(Result==KErrNone)
		SetBlockResult(EPass);
	else
		{
		INFO_PRINTF2(_L("Enable/disable GroupId filtering test FAILED, returning error %d"), Result);
		SetBlockResult(EFail);
		}

	if(BlockResult()==EPass)
		{
		INFO_PRINTF1(_L("Enable/disable GroupId filtering a second time to ensure correct error return"));
		//enable/disable filtering again
		if(aFiltering){}
			//Result = aLogger.EnableClassificationFiltering();//CA:enabled by default
		else //enable all GroupIds between 0 and 255
			//C.A. previously just :Result = aLogger.DisableClassificationFiltering();
			{
			//setfilterarray should not have changed i.e still contains all filters between 0 and 255
			Result = aLogger.SetPrimaryFiltersEnabled(*setfilterarray,ETrue);
			}
		if(Result==KErrNone)
			SetBlockResult(EPass);
		else
			{
			INFO_PRINTF2(_L("Duplicate enable/disable GroupId filtering test FAILED, returning error %d"), Result);
			SetBlockResult(EFail);
			}
		}
	if(BlockResult()==EPass)
		{
		//initialise filtering enabled boolean
//		TBool enabled = EFalse;
		INFO_PRINTF1(_L("Check IsClassificationFilteringEnabled() returns correct boolean accordingly"));
//		CA: filtering is always enabled and besides, can't check with this api
//		aLogger.IsClassificationFilteringEnabled(enabled);
//		if(enabled==aFiltering)
//			SetBlockResult(EPass);
//		else
//			{
//			INFO_PRINTF2(_L("IsClassificationFilteringEnabled test FAILED, returning bool %d"), enabled);
//			SetBlockResult(EFail);
//			}
		}
	if(BlockResult()==EPass)
		{
		INFO_PRINTF1(_L("Now call a trace statement to confirm correct full implementation"));
		CUptULogger loggerCommand;
		Result=loggerCommand.StartFileL(aLogger, aFiltering, ETrue, aFilter, ETrue);
		if(Result==KErrNone)
			SetBlockResult(EPass);
		else
			{
			INFO_PRINTF2(_L("Ulogger didnt start-> FAILED, returning error %d"), Result);
			SetBlockResult(EFail);
			}
		if(BlockResult()==EPass)
			{
			//initialise boolean return from trace call
			TBool ret = EFalse;
			//create a trace
			//need to change KPcNotIncluded to KPc so program counter can be included in tests
			TTraceContext testtrace(KComponentId, KGroupId, KContextId, KPcNotIncluded);
			ret = OstTrace(testtrace, KTraceId);
			OstTrace0(TEST_GROUP, OstTesting, "EOstTraceDef0" ) ;
			loggerCommand.Stop(aLogger);
			TBool OstStatus=0;
#ifdef TE_UPT_TRACE_ENABLED
			OstStatus = 1;
			INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
			OstStatus = 0;
			INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif // trace enabled
			if(ret&&OstStatus)
				{
				if((aFilter&&aFiltering)||(aFilter&&!aFiltering)||(!aFilter&&!aFiltering))
					{
					//create trace information object

					SetBlockResult(EPass);
					//need to change KPcNotIncluded to KPc so program counter can be included in tests
					StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId, KGroupId,KContextId, KPcNotIncluded);
					}
				else
					{
					INFO_PRINTF2(_L("trace call test FAILED, returning bool %d"), ret);
					SetBlockResult(EFail);
					}
				}
			else if(!ret)
				{
				if(!OstStatus||(!aFilter&&aFiltering))
					SetBlockResult(EPass);
				else
					{
					INFO_PRINTF2(_L("trace call test FAILED, returning bool %d"), ret);
					SetBlockResult(EFail);
					}
				}
			else
				{
				INFO_PRINTF2(_L("trace call test FAILED, returning bool %d"), ret);
				SetBlockResult(EFail);
				}
			}

		}
	CleanupStack::Pop(); //setfilterarray
	if(setfilterarray)
		delete setfilterarray;
		return BlockResult();
	}



/*******************************************************************************/
/*********************************Secondary Filter Test***************************/
/*******************************************************************************/


CTCMAdvancedComponentIDFilteringWrapper::~CTCMAdvancedComponentIDFilteringWrapper()
/**
 * Destructor
 */
	{
	}

CTCMAdvancedComponentIDFilteringWrapper::CTCMAdvancedComponentIDFilteringWrapper()
/**
 * Constructor
 */
	{
	}

CTCMAdvancedComponentIDFilteringWrapper* CTCMAdvancedComponentIDFilteringWrapper::NewLC()
	{
	CTCMAdvancedComponentIDFilteringWrapper* self = new (ELeave)CTCMAdvancedComponentIDFilteringWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMAdvancedComponentIDFilteringWrapper* CTCMAdvancedComponentIDFilteringWrapper::NewL()
	{
	CTCMAdvancedComponentIDFilteringWrapper* self=CTCMAdvancedComponentIDFilteringWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMAdvancedComponentIDFilteringWrapper::ConstructL()
	{
	}

TAny* CTCMAdvancedComponentIDFilteringWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMAdvancedComponentIDFilteringWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/,
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
		TBool OstStatus=EFalse;
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(KTextvalue);
		OstStatus=ETrue;
#endif
		TInt result=0;
		RULogger logger;
		TComponentId setfilter = KComponentId;
		RArray<TUint32> dupfilter;
		RArray<TUint32> getfilter;
		RArray<TUint32> setfilterarray,removefilterarray; //CA added for old ulogger api
		setfilterarray.Append(setfilter); //CA added for old ulogger api
		TComponentId removefilter=setfilter;
		removefilterarray.Append(removefilter);
	//	removefilter=32; //uncomment to negative test the remove filter option

		/**************First set Secondary filter*************/

		result=logger.Connect();
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
		result = logger.SetSecondaryFiltersEnabled(setfilterarray,ETrue);//CA previously:Result = logger.EnableModuleUid(setfilter);
		INFO_PRINTF2(_L("Setting single Secondary filter with filter rather than array has been set with single filter, %d, check log"), setfilter);
		INFO_PRINTF2(_L("code: %d"), result);


		/**************Now get the Secondary filter set above************/

		result = logger.GetSecondaryFiltersEnabled(getfilter);//CA previously:Result = logger.GetEnabledModuleUids(getfilter);
		INFO_PRINTF1(_L("Get Secondary filter returns the following filters"));
		INFO_PRINTF2(_L("code: %d"), result);
		for(TInt i=0;i!=getfilter.Count();i++)
		{
			INFO_PRINTF2(_L("%d "),getfilter[i]);
		}
		if(getfilter[0]==setfilter)
		{
			result=0;
			INFO_PRINTF1(_L("Get single Secondary filter successful"));
		}
		else
		{
			INFO_PRINTF2(_L("Get Secondary filter failed. \n NB: getfilter.Count = %d"), getfilter.Count());
			result=1;
		}
		for(TInt i=0;i!=getfilter.Count();)//i++)
		{
			getfilter.Remove(0);
		}

		/*****************Now remove the above set filter***************/

		if(result==0)
		{
			result=logger.SetSecondaryFiltersEnabled(removefilterarray,EFalse);//CA previously:Result=logger.DisableModuleUid(removefilter);
			INFO_PRINTF2(_L("Remove Module Uid filters single filter with single filter rather than array, 0 if succesful, -1 if failed: %d"), result);//logger.RemoveSingleSecondary);
			logger.GetSecondaryFiltersEnabled(getfilter);//CA previously:logger.GetEnabledModuleUids(getfilter);
			if(getfilter.Count()==0)
			{
				INFO_PRINTF1(_L("Remove ComponentId filter successful"));
				result=0;
			}
			else
			{
				if(result!=0)
					INFO_PRINTF1(_L("Remove ComponentId filter failed, as filter has not been set in the config file"));
				else
					INFO_PRINTF1(_L("Remove ComponentId filter failed, as able to pass same filter option to RemoveSecondaryFilter"));
				result=1;
			}
		}

		/****Now if the above steps have passed proceed to testing enable/disable ComponentId filtering***/

		if(result==1)
			SetBlockResult(EFail);
		else
			{
			INFO_PRINTF1(_L(" Testing ComponentId filtering enabled/ disabled "));

			//reset config file
			configIni.ClearL(logger);


			/***********************************************************/
			/***enable ComponentID, enable ComponentID filtering*/
			/***********************************************************/

			INFO_PRINTF1(_L("\n\n Enable test ComponentID, enable ComponentID filtering.\n Testing error returns and ensuring a trace statement IS carried out "));
			SetBlockResult(TestComponentIdFilteringL(logger, ETrue, ETrue));

			/***********************************************************/
			/***enable ComponentId, disable ComponentId filtering*/
			/***********************************************************/

			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("\n\n Enable test ComponentId, disable ComponentId filtering.\n Testing error returns and ensuring a trace statement IS carried out "));
				SetBlockResult(TestComponentIdFilteringL(logger, ETrue, EFalse));
				}

			/***********************************************************/
			/***disable ComponentId, disable ComponentId filtering*/
			/***********************************************************/

			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("\n\n Disable test ComponentId, disable ComponentId filtering.\n Testing error returns and ensuring a trace statement IS carried out "));
				SetBlockResult(TestComponentIdFilteringL(logger, EFalse, EFalse));
				}

			/***********************************************************/
			/***disable ComponentId, enable ComponentId filtering*/
			/***********************************************************/

			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("\n\n Disable test ComponentId, enable ComponentId filtering.\n Testing error returns and ensuring a trace statement IS NOT carried out "));
				SetBlockResult(TestComponentIdFilteringL(logger, EFalse, ETrue));
				}
			}

		if (BlockResult()==EPass&&OstStatus)
			{
			if (BlockResult()==EPass)
				{
#ifdef TE_UPT_TRACE_ENABLED
				if(!ReadFromLogFile(KTextvalue))
					{
					TInt err=CompareTraces();
					if(err)
						{
						INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), err);
						SetBlockResult(EFail);
						}
					else
						SetBlockResult(EPass);
					}
#endif
				}
			}

		//  **************   Block end ****************

		configIni.ClearL(logger);
		logger.Close();
		}
	  return ETrue;
	}



TVerdict CTCMAdvancedComponentIDFilteringWrapper::TestComponentIdFilteringL(RULogger& aLogger, const TBool aFilter, const TBool aFiltering)
/**
 * @return - TVerdict code
 * standard function to test error returns and ComponentId filtering
 */
	{
	RArray<TUint32> setfilterarray; //CA added for old ulogger api
	setfilterarray.Append(KComponentId); //CA added for old ulogger api
	TInt Result = 0;

	//enable/disable filter
	aLogger.SetSecondaryFiltersEnabled(setfilterarray,aFilter);

//	CA previously:
/*	if(aFilter)
		aLogger.EnableModuleUid(KComponentId);
	else
		aLogger.DisableModuleUid(KComponentId);*/

	//enable/disabled filtering
	Result = aLogger.SetSecondaryFilteringEnabled(aFiltering);
//	CA previously:
/*	if(aFiltering)
		Result = aLogger.EnableModuleUidFiltering();
	else
		Result = aLogger.DisableModuleUidFiltering();*/

	if(Result==KErrNone)
		SetBlockResult(EPass);
	else
		{
		INFO_PRINTF2(_L("Enable/disable ComponentId filtering test FAILED, returning error %d"), Result);
		SetBlockResult(EFail);
		}

	if(BlockResult()==EPass)
		{
		INFO_PRINTF1(_L("Enable/disable ComponentId filtering a second time to ensure correct error return"));
		//enable/disable filtering again
		Result = aLogger.SetSecondaryFilteringEnabled(aFiltering);
		//	CA previously:
		/*	if(aFiltering)
				Result = aLogger.EnableModuleUidFiltering();
			else
				Result = aLogger.DisableModuleUidFiltering();*/
		if(Result==KErrNone)
			SetBlockResult(EPass);
		else
			{
			INFO_PRINTF2(_L("Duplicate enable/disable ComponentId filtering test FAILED, returning error %d"), Result);
			SetBlockResult(EFail);
			}
		}

	if(BlockResult()==EPass)
		{
		//initialise filtering enabled boolean
		TBool enabled = EFalse;
		INFO_PRINTF1(_L("Check IsModuleUidFilteringEnabled() returns correct boolean accordingly"));
		aLogger.GetSecondaryFilteringEnabled(enabled);//CA previously:aLogger.IsModuleUidFilteringEnabled(enabled);
		if(enabled==aFiltering)
			SetBlockResult(EPass);
		else
			{
			INFO_PRINTF2(_L("IsModuleUidFilteringEnabled test FAILED, returning bool %d"), enabled);
			SetBlockResult(EFail);
			}
		}

	if(BlockResult()==EPass)
		{
		CUptULogger loggerCommand;
		INFO_PRINTF1(_L("Now call a trace statement to confirm correct full implementation with classification correctly enabled"));
		Result=loggerCommand.StartFileL(aLogger, ETrue, aFiltering, ETrue, aFilter);
		if(Result==KErrNone)
			SetBlockResult(EPass);
		else
			{
			INFO_PRINTF2(_L("Ulogger didnt start-> FAILED, returning error %d"), Result);
			SetBlockResult(EFail);
			}
			{
            TBool OstStatus=0;
#ifdef TE_UPT_TRACE_ENABLED
            OstStatus = 1;
            INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
            OstStatus = 0;
            INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif // trace enabled			
			//initialise boolean return from trace call
			TBool ret = EFalse;
			//create a trace
			//need to change KPcNotIncluded to KPc so program counter can be included in tests
			TTraceContext testtrace(KComponentId, KGroupId, KContextId, KPcNotIncluded);
			ret = OstTrace(testtrace, KTraceId);
	         if(ret&&OstStatus)
	                {
	                if((aFilter&&aFiltering)||(aFilter&&!aFiltering)||(!aFilter&&!aFiltering))
	                    {
	                    SetBlockResult(EPass);
	                    //need to change KPcNotIncluded to KPc so program counter can be included in tests
	                    StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId, KGroupId,KContextId, KPcNotIncluded);
	                    }
	                else
	                    {
	                    INFO_PRINTF2(_L("trace call test FAILED, returning bool %d"), ret);
	                    SetBlockResult(EFail);
	                    }
	                }
	            else if(!ret)
	                {
	                if(!OstStatus||(!aFilter&&aFiltering))
	                    SetBlockResult(EPass);
	                else
	                    {
	                    INFO_PRINTF2(_L("trace call test FAILED, returning bool %d"), ret);
	                    SetBlockResult(EFail);
	                    }
	                }
	            else
	                {
	                INFO_PRINTF2(_L("trace call test FAILED, returning bool %d"), ret);
	                SetBlockResult(EFail);
	                }
			
	        //Do the same for a couple of ostv1 derived macros
            //initialise return from trace call
            TInt macroret = KErrNone;
            //create a trace
            //need to change KPcNotIncluded to KPc so program counter can be included in tests
            TInt tracetag=0;
            iApiRunConfig.iDoStack=EFalse;
            iApiRunConfig.iDoStack=EFalse;
            for(tracetag=EOstTraceDef0; ((macroret==KErrNone)&&(tracetag!=EOstTraceExt5+1)); tracetag++)
                {
                iApiRunConfig.iApiId = tracetag;
                macroret = 1;
                TApiRunResults apiRunResults;
                TRunConfigurer::Init(apiRunResults);
                macroret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
                INFO_PRINTF3(_L("Ost macro %d has executed with error %d"), tracetag, macroret);                
                
                //if tracing is enabled, store the information that should have been
                //traced to compare later
                if(OstStatus)
                    {
                    if((aFilter&&aFiltering)||(aFilter&&!aFiltering)||(!aFilter&&!aFiltering))
                        {
                        SetBlockResult(EPass);
                        //need to change KPcNotIncluded to KPc so program counter can be included in tests
                        StoreTraceInfo(ETrue,(TUPTApiUsed)tracetag,0, 0,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
                        }
                    }
                }
            
            //stop outputting trace
			loggerCommand.Stop(aLogger);
			if(BlockResult()==EPass)
				{
				INFO_PRINTF1(_L("Now call a trace statement to confirm correct full implementation with classification not configured"));
				Result=loggerCommand.StartFileL(aLogger, ETrue, aFiltering, EFalse, aFilter);
				if(Result==KErrNone)
					SetBlockResult(EPass);
				else
					{
					INFO_PRINTF2(_L("Ulogger didnt start-> FAILED, returning error %d"), Result);
					SetBlockResult(EFail);
					}
				if(BlockResult()==EPass)
					{
					//call a trace that should not be output
					ret = OstTrace(testtrace, KTraceId);
					loggerCommand.Stop(aLogger);
					INFO_PRINTF2(_L("Trace call returns a value of %d"), ret);

					if(!ret)
						SetBlockResult(EPass);
					else
						{
						INFO_PRINTF2(_L("trace call test FAILED as trace carried out without requisite classification, returning bool %d"), ret);
						SetBlockResult(EFail);
						}
					}
				}
			}

		}

	return BlockResult();
	}
