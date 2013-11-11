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
#include "te_tracecollectorclient.h"
#include "te_ostv2integsuite_defs.h"


using namespace Ulogger; //CA:added so can use old ulogger api
/*******************************************************************************/
/*********************************Primary Filter Test***************************/
/*******************************************************************************/
_LIT(KTextsetting, "output_path");

CTCMGroupIDFilteringWrapper::~CTCMGroupIDFilteringWrapper()
/**
 * Destructor
 */
	{
	}

CTCMGroupIDFilteringWrapper::CTCMGroupIDFilteringWrapper()
/**
 * Constructor
 */
	{
	}


CTCMGroupIDFilteringWrapper* CTCMGroupIDFilteringWrapper::NewLC()
	{
	CTCMGroupIDFilteringWrapper* self = new (ELeave)CTCMGroupIDFilteringWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMGroupIDFilteringWrapper* CTCMGroupIDFilteringWrapper::NewL()
	{
	CTCMGroupIDFilteringWrapper* self=CTCMGroupIDFilteringWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMGroupIDFilteringWrapper::ConstructL()
	{
	}

TAny* CTCMGroupIDFilteringWrapper::GetObject()
	{
	return NULL;
	}
TBool CTCMGroupIDFilteringWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************

		TInt result=0;
		RULogger logger;
		CArrayFixFlat<TUint8> *setfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		setfilter->AppendL(KGroupId);
		CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		CArrayFixFlat<TUint8> *removefilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		removefilter->AppendL(setfilter->At(0));

		/**************First set primary filter*************/

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
		CClearConfig configIni;
		configIni.ClearL(logger);
		INFO_PRINTF2(_L("connect err %d"), result);
		result=logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);//C.A. previously:result=logger.EnableClassifications(*setfilter);

		if(result==KErrNone)
			INFO_PRINTF2(_L("Primary filter has been set with single filter, %d, check log"), setfilter->At(0));
		else
			INFO_PRINTF2(_L("Primary filter was not set, error returned, %d"), result);

		/**************Now get the GroupId filter set above************/

		logger.GetPrimaryFiltersEnabled(*getfilter);//C.A. previously:logger.GetEnabledClassifications(*getfilter);
		INFO_PRINTF1(_L("Get Primary filter returns the following filters"));
		for(TInt i=0;i!=getfilter->Count();i++)
		{
			INFO_PRINTF2(_L("get filter returns %d "),getfilter->At(i));
		}
		if(getfilter->At(0)==setfilter->At(0))
		{
			result=0;
			INFO_PRINTF1(_L("Get primary filter successful"));
		}
		else
		{
			INFO_PRINTF1(_L("Get primary filter failed"));
			result=1;
		}
		getfilter->Reset();
		/*****************Now remove the above set filter***************/

		if(result==0)
		{
			result=logger.SetPrimaryFiltersEnabled(*removefilter,EFalse);//C.A. previously:result=logger.DisableClassifications(*removefilter);
			INFO_PRINTF2(_L("Remove GroupId filters single filter, 0 if succesful: %d"), result);
			logger.GetPrimaryFiltersEnabled(*getfilter);//C.A. previously:logger.GetEnabledClassifications(*getfilter);

			if(getfilter->Count()==0&&result==KErrNone)
			{
				INFO_PRINTF1(_L("Remove primary filter successful"));
				result=0;
			}
			else
			{
				if(result!=KErrNone)
					INFO_PRINTF1(_L("Remove primary filter failed, as filter has not been set in the config file"));
				else
					INFO_PRINTF1(_L("Remove primary filter failed, as able to pass same filter option to RemovePrimaryFilter"));
				result=1;
			}
		}
		getfilter->Reset();

		/****Now if the above steps have passed proceed to testing multiple filters***/

		if(result==1)
			SetBlockResult(EFail);
		else
		{
			CArrayFixFlat<TUint8> *setfilters = new (ELeave)CArrayFixFlat<TUint8>(10);
			CArrayFixFlat<TUint8> *getfilters = new (ELeave)CArrayFixFlat<TUint8>(10);
			for(TInt i=0;i!=KMaxNumberOfGroupIds;i++)
			{
				setfilters->AppendL((TUint8)(i));
			}

			/**************First set  multiple primary filter*************/

			result=logger.SetPrimaryFiltersEnabled(*setfilters,ETrue);//C.A. previously:result=logger.EnableClassifications(*setfilters);
			if(result==KErrNone)
				INFO_PRINTF1(_L("Primary filter has been set with multiple filters,from 0 to 255"));
			else
				INFO_PRINTF2(_L("Multiple primary filters have not been set succesfully, error code %d"), result);

			/**************Now get the GroupId filter set above************/
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

			logger.GetPrimaryFiltersEnabled(*getfilters);//C.A. previously:logger.GetEnabledClassifications(*getfilters);
			if(getfilters->Count()==setfilters->Count())
				INFO_PRINTF1(_L("setfilter and getfilter have the same count"));
			else
			{
				INFO_PRINTF1(_L("setfilter and getfilter have different counts"));
				INFO_PRINTF2(_L("setfilter has %d"), setfilters->Count());
				INFO_PRINTF2(_L("getfilter has %d"), getfilters->Count());
				result=1;
			}
			if(result==0)
			{
				for(TInt i=0;i!=getfilters->Count() && result==0;i++)
				{
					for(TInt j=0;j!=setfilters->Count() && result!=0;j++)
					{
						if(getfilters->At(i)==setfilters->At(j))
							result=0;
						else
							result=1;
					}
				}
			}
			if(result==0)
				INFO_PRINTF1(_L("Get multiple primary filters successful"));
			else
				INFO_PRINTF1(_L("Get multiple primary filters failed"));
			INFO_PRINTF1(_L("Get Primary filters returns multiple filters"));
		//	for(int i=0;i!=KMaxNumberOfGroupIds;i++)
		//		INFO_PRINTF2(_L(" %d"), getfilters->operator[](i));	//uncomment to verify get filters test correct
			getfilters->Reset();

			/*****************Given this is successful, now remove the above set filter***************/

			if(result==0)
			{
				logger.Connect();
				logger.SetPrimaryFiltersEnabled(*removefilter,EFalse);//C.A. previously:logger.DisableClassifications(*removefilter);
			//	getfilter.Append(removefilter);
			//	logger.EnableClassifications(getfilter);//uncomment these to negative test the test suite
				logger.GetPrimaryFiltersEnabled(*getfilters);//C.A. previously:logger.GetEnabledClassifications(*getfilters);
				for(TInt i=0; i!=getfilters->Count() && result==0;i++)
				{
					if(getfilters->At(i)==removefilter->At(0))
					{
						INFO_PRINTF1(_L("Remove primary filter in multiple test failed"));
						result=1;
					}
					else
						result=0;
				}
				if(result==0)
					INFO_PRINTF1(_L("Remove primary filter in multiple test successful"));
				else
					INFO_PRINTF1(_L("Remove primary filter in multiple test failed"));
			}
			INFO_PRINTF1(_L("Now, given that the above tests have passed, negative test the primary filter options"));
			INFO_PRINTF1(_L("First reset the removed filter, then proceed"));
			setfilter->At(0)=removefilter->At(0);
			logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setfilter);
			if(result==0)
			{
				INFO_PRINTF1(_L("Removed filter has been reset"));

				/************Now try to set a primary filter value greater than that allowed***/

				setfilter->At(0)=((TGroupId)(KMaxNumberOfGroupIds));
				INFO_PRINTF2(_L("setfilter is %d"), setfilter->At(0));
				TInt r=logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);//C.A. previously:TInt r=logger.EnableClassifications(*setfilter);
				if((r==KErrNone)&&(setfilter->At(0)==KMaxNumberOfGroupIds-256))
				{
					INFO_PRINTF3(_L("Primary filter has been set with filter %d,corresponding to the passed value %d, this is allowed as duplicate! Passed"), setfilter->At(0),KMaxNumberOfGroupIds );
					result=0;
				}
				else if((r==KErrNone)&&(setfilter->At(0)!=KMaxNumberOfGroupIds-256))
				{
					INFO_PRINTF3(_L("Primary filter has been set with filter %d,corresponding to the passed value %d, this is not allowed as not duplicate! Failed"), setfilter->At(0),KMaxNumberOfGroupIds );
					result=1;
				}
				else
				{
					INFO_PRINTF3(_L("Primary filter has not been set with filter %d, corresponding to the passed value %d. Passed"), setfilter->At(0), KMaxNumberOfGroupIds);
					result=0;
				}

				/************Now try to set more primary filter values greater than available***/

				{
					INFO_PRINTF1(_L("Now try to set more filter than available using a duplicate"));
					if(result==0)
					{
						setfilter->At(0)=((TGroupId)(KMaxNumberOfGroupIds-1));
						result=logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);//C.A. previously:result=logger.EnableClassifications(*setfilter);
						if((result==KErrNone)&&(setfilter->At(0)!=KMaxNumberOfGroupIds-1))
						{
							INFO_PRINTF1(_L("More primary filters than available can be set. Failed"));
							result=1;
						}
						else
						{
							INFO_PRINTF1(_L("More primary filters than available cannot be set OR the filter being set is a duplicate. Passed"));
							result=0;
						}
					}
				}
			}
			if(result==1)
				SetBlockResult(EFail);
			else
				SetBlockResult(EPass);
			if(setfilters)
			{
				delete setfilters;
				setfilters=NULL;
			}
			setfilter->Reset();
			if(getfilters)
			{
				delete getfilters;
				getfilters=NULL;
			}
		}

		//  **************   Block end ****************

		configIni.ClearL(logger);
		logger.Close();
		if(setfilter)
			delete setfilter;
		if(getfilter)
			delete getfilter;
		}
	  return ETrue;
	}



/*******************************************************************************/
/*********************************Secondary Filter Test***************************/
/*******************************************************************************/


CTCMComponentIDFilteringWrapper::~CTCMComponentIDFilteringWrapper()
/**
 * Destructor
 */
	{
	}

CTCMComponentIDFilteringWrapper::CTCMComponentIDFilteringWrapper()
/**
 * Constructor
 */
	{
	}


CTCMComponentIDFilteringWrapper* CTCMComponentIDFilteringWrapper::NewLC()
	{
	CTCMComponentIDFilteringWrapper* self = new (ELeave)CTCMComponentIDFilteringWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMComponentIDFilteringWrapper* CTCMComponentIDFilteringWrapper::NewL()
	{
	CTCMComponentIDFilteringWrapper* self=CTCMComponentIDFilteringWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMComponentIDFilteringWrapper::ConstructL()
	{
	}

TAny* CTCMComponentIDFilteringWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMComponentIDFilteringWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
            INFO_PRINTF1(_L("do stepL"));
			TInt result=0;
			RULogger logger;
			RArray<TUint32> setfilter;
			setfilter.Append((TUint32) KComponentId);
			RArray<TUint32> dupfilter;
			RArray<TUint32> getfilter;
			RArray<TUint32> removefilter;
			removefilter.Append(setfilter[0]);
		//	removefilter=32; //uncomment to negative test the remove filter option

			/**************First set Secondary filter*************/

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

			CClearConfig configIni;
			configIni.ClearL(logger);
			result = logger.SetSecondaryFiltersEnabled(setfilter,ETrue);//C.A. previously:result = logger.EnableModuleUids(setfilter);
			INFO_PRINTF2(_L("Secondary filter has been set with single filter, %d, check log"), setfilter[0]);
			INFO_PRINTF2(_L("code: %d"), result);


			/**************Now get the Secondary filter set above************/

			result = logger.GetSecondaryFiltersEnabled(getfilter);//C.A. previously:result = logger.GetEnabledModuleUids(getfilter);
			INFO_PRINTF1(_L("Get Secondary filter returns the following filters"));
			INFO_PRINTF2(_L("code: %d"), result);
			for(TInt i=0;i!=getfilter.Count();i++)
			{
				INFO_PRINTF2(_L("%d "),getfilter[i]);
			}
			if(getfilter[0]==setfilter[0])
			{
				result=0;
				INFO_PRINTF1(_L("Get Secondary filter successful"));
			}
			else
			{
				INFO_PRINTF1(_L("Get Secondary filter failed"));
				result=1;
			}
			for(TInt i=0;i!=getfilter.Count();)//i++)
			{
				getfilter.Remove(0);
			}

			/*****************Now remove the above set filter***************/

			if(result==0)
			{
				result=logger.SetSecondaryFiltersEnabled(removefilter,EFalse);//C.A. previously:result=logger.DisableModuleUids(removefilter);
				INFO_PRINTF2(_L("Remove Module Uid filters single filter, 0 if succesful, -1 if failed: %d"), result);//logger.RemoveSingleSecondary);
				logger.GetSecondaryFiltersEnabled(getfilter);//C.A. previously:logger.GetEnabledModuleUids(getfilter);
				if(getfilter.Count()==0)
				{
					INFO_PRINTF1(_L("Remove ModuleUid filter successful"));
					result=0;
				}
				else
				{
					if(result!=0)
						INFO_PRINTF1(_L("Remove ModuleUid filter failed, as filter has not been set in the config file"));
					else
						INFO_PRINTF1(_L("Remove ModuleUid filter failed, as able to pass same filter option to RemoveSecondaryFilter"));
					result=1;
				}
			}

			/****Now if the above steps have passed proceed to testing multiple filters***/

			if(result==1)
				SetBlockResult(EFail);
			else
			{
				RArray<TUint32> setfilters;
				RArray<TUint32> getfilters;
				INFO_PRINTF2(_L("maxsecondary %d"),KMaxNumberOfComponentIds);
				for(TInt i=0;i<KMaxNumberOfComponentIds;i++)
				{
					setfilters.Append((TUint32)(i));
				}
				INFO_PRINTF1(_L("initialising values for multiple secondary filters ok, now set the filters etc"));

				/**************First set  multiple Secondary filter*************/

				result=logger.SetSecondaryFiltersEnabled(setfilters,ETrue);//C.A. previously:result=logger.EnableModuleUids(setfilters);
				if(result==KErrNone)
				INFO_PRINTF2(_L("Secondary filter has been set with multiple filters,from 0 to %d"), KMaxNumberOfComponentIds);
				else
					INFO_PRINTF2(_L("Multiple secondary filters have not been set succesfully, error code %d"), result);

				/**************Now get the Secondary filter set above************/

				INFO_PRINTF2(_L("before get, getfilter has %d"), getfilters.Count());
				logger.GetSecondaryFiltersEnabled(getfilters);//C.A. previously:logger.GetEnabledModuleUids(getfilters);
				if(getfilters.Count()==setfilters.Count())
				{
					INFO_PRINTF1(_L("setfilter and getfilter have the same count"));
					INFO_PRINTF2(_L("setfilter has %d"), setfilters.Count());
					INFO_PRINTF2(_L("getfilter has %d"), getfilters.Count());
				}
				else
				{
					INFO_PRINTF1(_L("setfilter and getfilter have different counts"));
					result=1;
						INFO_PRINTF2(_L("setfilter has %d"), setfilters.Count());
						INFO_PRINTF2(_L("getfilter has %d"), getfilters.Count());
				}
				for(TInt i=0;i!=getfilters.Count() && result==0;i++)
				{
					for(TInt j=0;j!=getfilters.Count() && result!=0;j++)
					{
						if(getfilters[i]==setfilters[j])
							result=0;
						else
							result=1;
					}
				}
				if(result==0)
					INFO_PRINTF1(_L("Get multiple Secondary filters successful"));
				else
					INFO_PRINTF1(_L("Get multiple Secondary filters failed"));
				INFO_PRINTF1(_L("Get Secondary filters returns multiple filters"));
			//	for(int i=0;i!=getfilters.Count();i++)//KMaxNumberOfComponentIds;i++)
			//		INFO_PRINTF2(_L(" %d"), getfilters[i]);	//uncomment to verify get filters test correct - WARNING-loadsa data if use true maximum for secondary filters-will break TEF!!
				for(TInt i=0;i<getfilters.Count();)//i++)
				{
					getfilters.Remove(0);
				}

				/*****************Given this is successful, now remove the above set filter***************/

				if(result==0)
				{
					logger.SetSecondaryFiltersEnabled(removefilter,EFalse);//C.A. previously:logger.DisableModuleUids(removefilter);
				//	getfilter.Append(removefilter);
				//	logger.EnableModuleUids(getfilter);//uncomment these to negative test the test suite
					logger.GetSecondaryFiltersEnabled(getfilters);//C.A. previously:logger.GetEnabledModuleUids(getfilters);
					for(TInt i=0; i!=getfilters.Count() && result==0;i++)
					{
						if(getfilters[i]==removefilter[0])
						{
							INFO_PRINTF1(_L("Remove Secondary filter in multiple test failed"));
							result=1;
						}
						else
							result=0;
					}
					if(result==0)
						INFO_PRINTF1(_L("Remove Secondary filter in multiple test successful"));
					else
					INFO_PRINTF1(_L("Remove Secondary filter in multiple test failed"));
				}
				INFO_PRINTF1(_L("Now, given that the above tests have passed, negative test the secondary filter options"));
				INFO_PRINTF1(_L("First reset the removed filter, then proceed"));
				setfilter[0]=removefilter[0];
				logger.SetSecondaryFiltersEnabled(setfilter,ETrue);//C.A. previously:logger.EnableModuleUids(setfilter);
				if(result==0)
				{
					INFO_PRINTF1(_L("Removed filter has been reset"));

					/************Now try to set a secondary filter value greater than that allowed***/

					setfilter[0]=((TComponentId)(KMaxNumberOfComponentIds-1));
					INFO_PRINTF2(_L("setfilter is %d"), setfilter[0]);
					TInt r= logger.SetSecondaryFiltersEnabled(setfilter,ETrue);//C.A. previously:TInt r= logger.EnableModuleUids(setfilter);
					if((r==KErrNone)&&(setfilter[0]==KMaxNumberOfComponentIds-1))
					{
						INFO_PRINTF3(_L("Secondary filter has been set with filter %d,corresponding to the passed value %d, this is allowed as is duplicate. Passed"), setfilter[0],KMaxNumberOfComponentIds-1 );
						result=0;
					}
					if((r==KErrNone)&&(setfilter[0]!=KMaxNumberOfComponentIds-1))
					{
						INFO_PRINTF3(_L("Secondary filter has been set with filter %d,corresponding to the passed value %d, this is not allowed. Failed"), setfilter[0],KMaxNumberOfComponentIds-1 );
						result=1;
					}
					else

					{
						INFO_PRINTF3(_L("Secondary filter has not been set with filter %d, corresponding to the passed value %d , this is as expected as otherwise duplicate. Passed"), setfilter[0], KMaxNumberOfComponentIds-1);
						result=0;
					}
					/************Now try to set more secodndary filter values greater than available***/

					INFO_PRINTF1(_L("Now try to set more filter than available"));
					if(result==0)
					{
						setfilter[0]=((TComponentId)(KMaxNumberOfComponentIds+3));
						result=logger.SetSecondaryFiltersEnabled(setfilter,ETrue);//C.A. previously:result=logger.EnableModuleUids(setfilter);
						if(result==KErrNone)
						{
							INFO_PRINTF1(_L("More secondary filters than available can be set. Failed"));
							result=1;
						}
						else
						{
							INFO_PRINTF1(_L("More secondary filters than available cannot be set. Passed"));
							result=0;
						}
					}
				}
				INFO_PRINTF1(_L("Now call clear config to check removing multiple second filters performance"));
				configIni.ClearL(logger);
				INFO_PRINTF1(_L("after clear config- multiple second filters performance"));

				if(result==1)
					SetBlockResult(EFail);
				else
					SetBlockResult(EPass);
			}
			logger.Close();
			//  **************   Block end ****************
		}
	  return ETrue;
	}


/*******************************************************************************/
/*********************************Logging Start/Stop Test***********************/
/*******************************************************************************/

CTCMLoggingWrapper::~CTCMLoggingWrapper()
/**
 * Destructor
 */
	{
	}

CTCMLoggingWrapper::CTCMLoggingWrapper()
/**
 * Constructor
 */
	{
	}



CTCMLoggingWrapper* CTCMLoggingWrapper::NewLC()
	{
	CTCMLoggingWrapper* self = new (ELeave)CTCMLoggingWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMLoggingWrapper* CTCMLoggingWrapper::NewL()
	{
	CTCMLoggingWrapper* self=CTCMLoggingWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMLoggingWrapper::ConstructL()
	{
	}

TAny* CTCMLoggingWrapper::GetObject()
	{
	return NULL;
	}
TBool CTCMLoggingWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
		TBool OstStatus=0;
	  if (BlockResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************

			INFO_PRINTF1(_L("About to test ulogger start with one primary and one secondary"));  //Block start
			TInt result=0;
			RULogger logger;
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
#ifdef SysStartTest
			TInt SysStartResult=logger.StartOutputting();
			if(SysStartResult!=KErrInUse)
				INFO_PRINTF2(_L("Logger start returning incorrect error code of %d when already started by SysStart, should be KErrInUse (-14)"), SysStartResult);
			logger.StopOutputting();
#endif
#ifndef SysStartTest
			CClearConfig configIni;
			configIni.ClearL(logger);
#endif
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(22);
			setprimfilter->AppendL(KGroupId);
			setprimfilter->AppendL(TRACE_PERFORMANCE); 
			setprimfilter->AppendL(TRACE_STATE);
			setprimfilter->AppendL(TRACE_FLOW);
			/**REMOVE THIS SECTION WHEN TRACE COMPILER WORK FOR CORRECT GIDS IS DONE**/
			setprimfilter->AppendL(TRACE_NORMAL);
			/*************************************************************************/
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) KComponentId);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setprimfilter);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(setsecondfilter);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
#ifndef SysStartTest

			_LIT(KTextvalue, "C:\\logs\\ULoggerLoggingTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
			EmptyFile(KTextvalue);
#endif
			if(result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				result=logger.ActivateOutputPlugin(mediaptr);//C.A. previously:result=logger.SetActiveOutputPlugin(mediaptr);
				INFO_PRINTF1(_L("file set as active output plugin ok"));
				TPluginConfiguration pluginConfig;
				pluginConfig.SetKey(KTextsetting);
				pluginConfig.SetValue(KTextvalue);
				result=logger.SetPluginConfigurations(mediaptr, pluginConfig);
				if(result==0)
					INFO_PRINTF1(_L("output plugin settings set ok"));
				else
					INFO_PRINTF2(_L("setting output plugin settings failed with err %d"), result);
			}
			else
				INFO_PRINTF2(_L("add file as output plugin failed with err %d"), result);
#endif
			if(result==0)
			{
			/************** Start Logging to File****************/

			result=logger.Start();//C.A. previously:result=logger.StartOutputting();
			INFO_PRINTF2(_L("ulogger start returns error %d"), result);
			INFO_PRINTF1(_L("now step over some Ost statements"));
			int tracetag =0;
			TBool ret=KErrNone;
			iApiRunConfig.iDoStack=EFalse;

			#ifdef TE_UPT_TRACE_ENABLED
			OstStatus = 1;
			INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
  			#else
			OstStatus = 0;
			INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
  			#endif

			/*************Now test the static method************************/

			INFO_PRINTF1(_L("******Testing STATIC Trace Methods******"));

			iApiRunConfig.iDoStack=EFalse;
			ret=KErrNone;
			/*Comment out this line to switch off tracing as a negative test*/
			for(tracetag=EUptTrace; tracetag!=(EOstTraceEventStop+1)&&ret==KErrNone;tracetag++)
			{
                iApiRunConfig.iApiId = tracetag;
                ret = 1;
                TApiRunResults apiRunResults;
                TRunConfigurer::Init(apiRunResults);
                ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
                INFO_PRINTF3(_L("Ost macro %d has executed with error %d"), tracetag, ret);

                if(ret==KErrNone&&OstStatus)
                    {
                    StoreTraceInfo(ETrue,(TUPTApiUsed)tracetag,0, 0,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
                    }
			}

			if((ret==KErrNone&&OstStatus)||(ret!=KErrNone&&!OstStatus&&tracetag!=EUptIsTraceActive))
			{
				result=0;
				INFO_PRINTF1(_L("Tracing carried out succesfully for static method"));
				if(ret==KErrNone)
					{
					INFO_PRINTF1(_L("logfile should contain appropriate tracing"));
					}
				else
					INFO_PRINTF1(_L("logfile should be empty of trace statements"));

			}
			else
			{
				result=1;
				INFO_PRINTF2(_L("Tracing Failed on Ost macro  %d for static method"), tracetag);
			}

			/************** Stop Logging to File****************/

			result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
			if(result==KErrNone&&OstStatus)
				{
				if(!ReadFromLogFile(KTextvalue))
					{
					result=CompareTraces();
					if(result)
						INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), result);
					//prob set the test step result here
					}
				}

			if(result==0)
			{
			iApiRunConfig.iApiId = EUptTrace;
			ret = 0;
			TApiRunResults apiRunResults;
			TRunConfigurer::Init(apiRunResults);
			ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
			INFO_PRINTF1(_L("logfile should contain appropriate tracing without an extra Ost(0) trace at the end, if it is present ULogger.StopOutputting() has failed in functionality"));
			if(!ret)
				{
				result=CompareTraces();
				if(result)
					INFO_PRINTF2(_L("ERROR: Error when comparing traces, extra trace might have been added at the end %i"), result);
				}
			}
			else
			{
				INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with err %d"), result);
			}
		}
		else
			INFO_PRINTF1(_L("adding file as output plugin failed"));
		if(result==0)
				SetBlockResult(EPass);
		else
				SetBlockResult(EFail);
		INFO_PRINTF1(_L("Simple logging to file has been tested - check output log"));

		/***************************negative test***********************/
		_LIT(KnegTextvalue, "C:\\logs\\ULoggerNegativeLoggingTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(KnegTextvalue);
#endif
		if(BlockResult()==EPass)
		{
			INFO_PRINTF1(_L("Now do some negative logging testing."));
			if(result==KErrNone)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				if(result==0)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					TPluginConfiguration negPluginConfig;
					negPluginConfig.SetKey(KTextsetting);
					negPluginConfig.SetValue(KnegTextvalue);
					result=logger.SetPluginConfigurations(mediaptr, negPluginConfig);
					if(result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
				INFO_PRINTF1(_L("Log over traces with different filters set - there should only be one line in logfile corrseponding to the set filters."));
				if(result==0)
				{
				logger.Start();//C.A. previously:logger.StartOutputting();

					/*****do the tracing**************/

				bool ret = 1;
				TTraceContext header(KComponentId, KGroupId, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
				ret = OstTrace(header, KTraceId);

				iLoggedTraces.Reset();
				iSentTraces.Reset();
				if (ret)
					{
					//create trace information object
					StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId, KGroupId,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					}

				INFO_PRINTF2(_L("tracing ret %d"), ret);
				TTraceContext attr1(KComponentId+1, KGroupId+1, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
				ret = OstTrace(attr1, KTraceId);

				INFO_PRINTF2(_L("tracing ret %d"), ret);

				/************** Stop Logging to File****************/

				result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
				if(result!=0)
					{
					INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with err %d"), result);
					}
					INFO_PRINTF1(_L("Set some more filters and remove the original set in ulogger, then trace over the original types of traces - there should be no extra lines in logfile above"));

				/**************Now Change the filter settings********/
				CArrayFixFlat<TUint8> *removeprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
				removeprimfilter->AppendL(setprimfilter->At(0));
				RArray<TUint32> removesecondfilter;
				removesecondfilter.Append(setsecondfilter[0]);
				result=logger.SetPrimaryFiltersEnabled(*removeprimfilter,EFalse);//C.A. previously:result=logger.DisableClassifications(*removeprimfilter);
				if(result==0)
				{
					INFO_PRINTF1(_L("Call to removeprimary has passed"));
				}
				result=logger.SetSecondaryFiltersEnabled(removesecondfilter,EFalse);//C.A. previously:result=logger.DisableModuleUids(removesecondfilter);
				if(result==0)
				{
						INFO_PRINTF1(_L("Call to removesecondary has passed"));
				}
					CArrayFixFlat<TUint8> *newsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					newsetprimfilter->AppendL((TUint8)(KGroupId+2));
					RArray<TUint32> newsetsecondfilter;
					newsetsecondfilter.Append((TUint32) (KComponentId+2));
					logger.SetPrimaryFiltersEnabled(*newsetprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*newsetprimfilter);
					logger.SetSecondaryFiltersEnabled(newsetsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(newsetsecondfilter);
					ret=1;
					logger.Start();//C.A. previously:logger.StartOutputting();

					/*****do the tracing**************/

					/*Comment out this line to switch off tracing as a negative test*/
					int tracetag=0;
					for(tracetag=EUptTrace; tracetag!=EUptIsTraceActive&&ret==KErrNone;tracetag++)
					{
					iApiRunConfig.iApiId = tracetag;
					ret = 1;
					TApiRunResults apiRunResults;
					TRunConfigurer::Init(apiRunResults);
					ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
					INFO_PRINTF3(_L("Ost macro %d has been executed with error %d"), tracetag, ret);
					}
					if(ret==KErrNone)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("BTrace filters are not being reset with StartOutputting()/StopOutputting(), so logging over filters that have been unset is occuring. Fail"));
					}
					TTraceContext attr2(KComponentId+2, KGroupId+2, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr2, KTraceId);
					if (ret)
						{
						//create trace information object
						StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId+2, KGroupId+2,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
						}

					INFO_PRINTF2(_L("tracing ret %d"), ret);

					/************** Stop Logging to File****************/

					logger.Stop();//C.A. previously:result=logger.StopOutputting();

					if(result!=0)
					{
					INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with err %d"), result);
					}

					else if(result==KErrNone&&OstStatus)
						{
						if(!ReadFromLogFile(KnegTextvalue))
							{
							result=CompareTraces();
							if(result)
								INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), result);
							//prob set the test step result here
							}
						}
					if(newsetprimfilter)
					{
						delete(newsetprimfilter);
						newsetprimfilter=NULL;
					}
				}
			}
			else
				INFO_PRINTF1(_L("Incorrect error code when trying to add file as a plug-in when it already exists"));
			if(result==0&&BlockResult()==EPass)
					SetBlockResult(EPass);
			else
					SetBlockResult(EFail);
			INFO_PRINTF1(_L("Simple negative logging to file has been tested - check output log"));
		}

		/***************************multiple type test***********************/
   		 // From negative test: primary has been set to KClassification+2 and secondary to KComponentId+2
		_LIT(KmultTextvalue, "C:\\logs\\ULoggerMultipleLoggingTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(KmultTextvalue);
#endif
		if(BlockResult()==EPass)
		{
			INFO_PRINTF1(_L("Now do some multiple type logging testing."));
			//CClearConfig configIni;
			//configIni.ClearL(logger);

			if(result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				if(result==0)
				{
					INFO_PRINTF1(_L("file set as active output plugin ok"));
					TPluginConfiguration pluginConfig;
					pluginConfig.SetKey(KTextsetting);
					pluginConfig.SetValue(KmultTextvalue);
					result=logger.SetPluginConfigurations(mediaptr, pluginConfig);
					if(result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
				if(result==0)
				{
					TInt i;
					CArrayFixFlat<TUint8> *multsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					multsetprimfilter->AppendL(KGroupId);
					RArray<TUint32> multsetsecondfilter;
					multsetsecondfilter.Append((TUint32) (KComponentId));
					logger.SetPrimaryFiltersEnabled(*multsetprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*multsetprimfilter);
					TInt errCode = logger.SetSecondaryFiltersEnabled(multsetsecondfilter,ETrue);//C.A. previously:TInt errCode = logger.EnableModuleUids(multsetsecondfilter);

					if(errCode != KErrNone)
						INFO_PRINTF2(_L("Setting secondary filters failed: %d"), errCode);
					INFO_PRINTF1(_L("checking !!! secondary filters"));
					for(i=0; i<multsetsecondfilter.Count(); ++i)
						{
						TBuf<16> buf;
						buf.AppendFormat(_L("%d"), multsetsecondfilter[i]);
						INFO_PRINTF1(buf);
						}

					INFO_PRINTF1(_L("Now given the two sets of set filters, step over traces of each type, some with matching primary but not secondary and vice versa"));

					INFO_PRINTF1(_L("Primary Filetrs:"));
					CArrayFixFlat<TUint8>* primFilters = new (ELeave) CArrayFixFlat<TUint8>(1);
					CleanupStack::PushL(primFilters);
					logger.GetPrimaryFiltersEnabled(*primFilters);//C.A. previously:logger.GetEnabledClassifications(*primFilters);

					for(i=0; i<primFilters->Count(); ++i)
						{
						TBuf<16> buf;
						buf.AppendFormat(_L("%d "), primFilters->At(i));
						INFO_PRINTF1(buf);
						}
					CleanupStack::PopAndDestroy(); //primFiletrs

					INFO_PRINTF1(_L("ModuleUids:"));
					RArray<TUint32> secFilters;
					errCode = logger.GetSecondaryFiltersEnabled(secFilters);//C.A. previously:errCode = logger.GetEnabledModuleUids(secFilters);
					if(errCode != KErrNone)
						INFO_PRINTF2(_L("Getting ModuleUids failed: %d"), errCode);

					for(i=0; i<secFilters.Count(); ++i)
						{
						TBuf<16> buf;
						buf.AppendFormat(_L("%d"), secFilters[i]);
						INFO_PRINTF1(buf);
						}
					secFilters.Close();

					INFO_PRINTF1(_L("Secondary Filetring is:"));
					TBool secF = EFalse;
					logger.GetSecondaryFilteringEnabled(secF);//C.A. previously:logger.IsModuleUidFilteringEnabled(secF);
					if(secF)
						INFO_PRINTF1(_L("ENABLED"));
					else
						INFO_PRINTF1(_L("DISABLED"));

					result=logger.Start();//C.A. previously:result=logger.StartOutputting();
					if(result!=KErrNone)
						INFO_PRINTF2(_L("ulogger server didn't start, with error %d"), result);

					/*****do the tracing**************/

#ifdef TE_UPT_TRACE_ENABLED
		OstStatus = 1;
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		OstStatus = 0;
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
					iLoggedTraces.Reset();
					iSentTraces.Reset();
					bool ret=0;
					TTraceContext attributes(KComponentId, KGroupId, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attributes, KTraceId);

					if(ret==0&&OstStatus)
						{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec and prim) is not occuring. Fail"));
						}
					else
						{
						if (ret==1&&OstStatus)
							{
							//create trace information object
							StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId, KGroupId,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
							}
						}

					TTraceContext attr1(KComponentId + 1, KGroupId + 1, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr1, KTraceId);
					if(ret==1)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have not been set is occuring. Fail"));
					}
					TTraceContext attr2(KComponentId + 2, KGroupId, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr2, KTraceId);
					if(ret==0&&OstStatus)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec+2 and prim) is not occuring. Fail"));
					}
					else
						{
						if (ret==1&&OstStatus)
							{
							//create trace information object
							StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId+2, KGroupId,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
							}
						}

					TTraceContext attr3(KComponentId, KGroupId + 2, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr3, KTraceId);
					if(ret==0&&OstStatus)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec and prim+2) is not occuring. Fail"));
					}

					else
						{
						if (ret==1&&OstStatus)
							{
							//create trace information object
							StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId, KGroupId+2,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
							}
						}

					TTraceContext attr4(KComponentId + 2, KGroupId + 2, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr4, KTraceId);
					if(ret==0&&OstStatus)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec+2 and prim+2) is not occuring. Fail"));
					}
					else
						{
						if (ret==1&&OstStatus)
							{
							//create trace information object
							StoreTraceInfo(EFalse,(TUPTApiUsed)0,KComponentId+2, KGroupId+2,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
							}
						}

					INFO_PRINTF1(_L("checking KGroupId i KComponentId"));
					TBuf<32> b;
					b.AppendFormat(_L("%d and %d"), KGroupId+2, KComponentId+1);
					INFO_PRINTF1(b);
					TTraceContext attr5(KComponentId + 1, KGroupId + 2, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr5, KTraceId);
					if(ret==1)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have not been set (the sec) is occuring. Fail"));
					}
					TTraceContext attr6(KComponentId, KGroupId + 1, iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					ret = OstTrace(attr6, KTraceId);
					if(ret==1)
					{
						SetBlockResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have not been set (the prim) is occuring. Fail"));
					}


					/************** Stop Logging to File****************/

					result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
					if(result!=KErrNone)
					{
						INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with error %d"), result);
					}
					else
						if(OstStatus)
						{
						if(!ReadFromLogFile(KmultTextvalue))
							{
							result=CompareTraces();
							if(result)
								INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), result);
								//prob set the test step result here
							}
						}

					if(multsetprimfilter)
					{
						delete(multsetprimfilter);
						multsetprimfilter=NULL;
					}
				}
			configIni.ClearL(logger);
			}
			else
				INFO_PRINTF1(_L("adding file as output plugin failed"));
			if(result==0&&BlockResult()==EPass)
					SetBlockResult(EPass);
			else
					SetBlockResult(EFail);
			INFO_PRINTF1(_L("Multiple logging to file with user side tracing has been tested - check output log. It should contain four lines of tracing of type, (sec,prim), (sec+2, prim), (sec, prim+2), (sec+2, prim+2)"));
		}
		if(setprimfilter)
		{
			delete(setprimfilter);
			setprimfilter=NULL;
		}
		INFO_PRINTF1(_L("Now shutdown the ulogger server session"));
	    logger.Close();

		iLoggedTraces.Reset();
		iSentTraces.Reset();
#ifdef SysStartTest
		if(SysStartResult!=KErrInUse)
			SetBlockResult(EFail);
#endif
		//  **************   Block end ****************
		}
	  return ETrue;
	}

/*******************************************************************************/
/*********************************Kernel Side Logging Test**********************/
/*******************************************************************************/

CTCMKernelLoggingWrapper::~CTCMKernelLoggingWrapper()
/**
 * Destructor
 */
	{
	}

CTCMKernelLoggingWrapper::CTCMKernelLoggingWrapper()
/**
 * Constructor
 */
	{
	}



CTCMKernelLoggingWrapper* CTCMKernelLoggingWrapper::NewLC()
	{
	CTCMKernelLoggingWrapper* self = new (ELeave)CTCMKernelLoggingWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMKernelLoggingWrapper* CTCMKernelLoggingWrapper::NewL()
	{
	CTCMKernelLoggingWrapper* self=CTCMKernelLoggingWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMKernelLoggingWrapper::ConstructL()
	{
	}

TAny* CTCMKernelLoggingWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMKernelLoggingWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{

	  if (BlockResult()==EPass)
		{
		TApiRunResults apiRunResults;
		TRunConfigurer::Init(apiRunResults);

		//  ************** Delete the Block, the block start ****************

			INFO_PRINTF1(_L("About to test ulogger start with one primary and one secondary, kernel side"));  //Block start
			TInt result=0;
			RULogger logger;
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
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(KGroupId);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) KComponentId);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setprimfilter);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(setsecondfilter);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step

			_LIT(KTextvalue, "C:\\logs\\ULoggerKernelLoggingTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
			EmptyFile(KTextvalue);
#endif
			if(result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				result=logger.ActivateOutputPlugin(mediaptr);//C.A. previously:result=logger.SetActiveOutputPlugin(mediaptr);
				if(result==0||result==-11)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KTextvalue);
					result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));

			if(result==0)
			{
				/************** Start Logging to File****************/
				iLoggedTraces.Reset();
				iSentTraces.Reset();
				TInt ret=KErrNone;
				logger.Start();//C.A. previously:logger.StartOutputting();
				/*test kernel side nonstatic*/
				TInt tracetag=0;
				INFO_PRINTF1(_L("Test kernel side nonstatic Ost methods"));
				for(tracetag=EUptKernelTrace; tracetag!=EUptKernelIsTraceActive&&ret==KErrNone;tracetag++)
				{
				//to remove the big traces (>80 bytes) for now.
				//if((tracetag!=EUptKernelTraceTAnyBigStruct)&&(tracetag!=EUptKernelTraceTAnyHugeStruct)&&(tracetag!=EUptKernelTraceTemplatedBigStruct)&&(tracetag!=EUptKernelTraceTemplatedHugeStruct))
					{
					iApiRunConfig.iApiId = tracetag;
					ret = 1;
					ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
					INFO_PRINTF3(_L("Ost kernel macro %d has been executed with return value %d"), tracetag, ret);

					//storing trace information for checking later
					if(ret==KErrNone)
						{
						StoreTraceInfo(ETrue,(TUPTApiUsed)tracetag,0, 0,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
						}
					}
				}

				ret=KErrNone;
				/*test kernel side static*/
				INFO_PRINTF1(_L("Test kernel side static Ost methods"));
				for(tracetag=EUptKernelTrace; tracetag!=EUptKernelIsTraceActive&&ret==KErrNone;tracetag++)
				{
				//to remove the big traces (>80 bytes) for now.
				//if((tracetag!=EUptKernelTraceTAnyBigStruct)&&(tracetag!=EUptKernelTraceTAnyHugeStruct)&&(tracetag!=EUptKernelTraceTemplatedBigStruct)&&(tracetag!=EUptKernelTraceTemplatedHugeStruct))
					{
					iApiRunConfig.iApiId = tracetag;
					ret = 1;
					ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
					INFO_PRINTF3(_L("Ost kernel macro %d has been executed with return value %d"), tracetag, ret);

					//storing trace information for checking later
					if(ret==KErrNone)
						{
						StoreTraceInfo(ETrue,(TUPTApiUsed)tracetag,0, 0,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
						}
					}

				}

				ret=KErrNone;



#ifdef SYMBIAN_TRACE_ENABLE
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
				/************** Stop Logging to File****************/

				result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
				if(result!=KErrNone)
					INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with err %d"), result);
			}
			else
				INFO_PRINTF1(_L("adding file as output plugin failed"));
			if(result==0)
				{
				SetBlockResult(EPass);
				//check trace content here
				if(!ReadFromLogFile(KTextvalue))
					{
					result=CompareTraces();
					if(result!=KErrNone)
						{
						INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), result);
						SetBlockResult(EFail);
						}
					else
						SetBlockResult(EPass);
					//prob set the test step result here
					}
				}
			else
					SetBlockResult(EFail);
			INFO_PRINTF1(_L("Simple logging to file has been tested with kernel side tracing- check output log"));

			/***************************negative test***********************/
			_LIT(KnegTextvalue, "C:\\logs\\ULoggerKernelNegativeLoggingTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
			EmptyFile(KnegTextvalue);
#endif
			//empty out arrays of previously sent and logged traces
			iLoggedTraces.Reset();
			iSentTraces.Reset();

			if(BlockResult()==EPass)
			{
				INFO_PRINTF1(_L("Now do some negative logging testing."));
				if(result==KErrNone)
				{
					INFO_PRINTF1(_L("file set as output plugin ok"));
					_LIT8(KTextmedia,"uloggerfileplugin");
					TPtrC8 mediaptr(KTextmedia);
					if(result==0)
					{
						INFO_PRINTF1(_L("file set as active output plugin ok"));
						TPluginConfiguration negSetPluginConfigs;
						negSetPluginConfigs.SetKey(KTextsetting);
						negSetPluginConfigs.SetValue(KnegTextvalue);
						result=logger.SetPluginConfigurations(mediaptr, negSetPluginConfigs);
						if(result==0)
							INFO_PRINTF1(_L("output plugin settings set ok"));
						else
							INFO_PRINTF1(_L("setting output plugin settings failed"));
					}
					else
						INFO_PRINTF1(_L("file not set as active output plugin - failed"));

					/**************Now Change the filter settings********/

					CArrayFixFlat<TUint8> *removeprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					removeprimfilter->AppendL(setprimfilter->At(0));
					RArray<TUint32> removesecondfilter;
					removesecondfilter.Append(setsecondfilter[0]);
					result=logger.SetPrimaryFiltersEnabled(*removeprimfilter,EFalse);//C.A. previously:result=logger.DisableClassifications(*removeprimfilter);
					if(result==0)
					{
						INFO_PRINTF1(_L("Call to removeprimary has passed"));
					}
					result=logger.SetSecondaryFiltersEnabled(removesecondfilter,EFalse);//C.A. previously:result=logger.DisableModuleUids(removesecondfilter);
					if(result==0)
					{
						INFO_PRINTF1(_L("Call to removesecondary has passed"));
					}
					CArrayFixFlat<TUint8> *newsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					newsetprimfilter->AppendL((TUint8)(KGroupId+2));
					RArray<TUint32> newsetsecondfilter;
					newsetsecondfilter.Append((TUint32)(KComponentId+2));
					logger.SetPrimaryFiltersEnabled(*newsetprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*newsetprimfilter);
					logger.SetSecondaryFiltersEnabled(newsetsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(newsetsecondfilter);
					if(result==0)
					{
						logger.Start();//C.A. previously:logger.StartOutputting();
						INFO_PRINTF1(_L("now step over some Ost statements for negative testing on the kernel side"));
						iApiRunConfig.iApiId = UPTKernelNegativeFunctionalityTest;
						result = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
					}
					result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
					if(result!=0)
					{
						INFO_PRINTF1(_L("Call to Ulogger.StopOutputting() has failed"));
					}
					if(newsetprimfilter)
					{
						delete(newsetprimfilter);
						newsetprimfilter=NULL;
					}
				}
				else
					INFO_PRINTF1(_L("Incorrect error code when trying to add file as a plug-in when it already exists"));
				if(result==0&&BlockResult()==EPass)
						SetBlockResult(EPass);
				else
						SetBlockResult(EFail);
				INFO_PRINTF1(_L("Simple negative logging to file has been tested - check output log"));
		}

		/***************************multiple type test***********************/
	   	 // From negative test: primary has been set to KClassification+2 and secondary to KComponentId+2

		_LIT(KmultTextvalue, "C:\\logs\\ULoggerKernelMultipleLoggingTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(KmultTextvalue);
#endif
		if(BlockResult()==EPass)
		{
			INFO_PRINTF1(_L("Now do some multiple type logging testing."));
			if(result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				if(result==0)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KmultTextvalue);
					result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
				if(result==0)
				{
				/******************reset filters and change call for multiple logging*****/

					CArrayFixFlat<TUint8> *multsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					multsetprimfilter->AppendL(KGroupId);
					RArray<TUint32> multsetsecondfilter;
					multsetsecondfilter.Append((TUint32) (KComponentId));
					logger.SetPrimaryFiltersEnabled(*multsetprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*multsetprimfilter);
					logger.SetSecondaryFiltersEnabled(multsetsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(multsetsecondfilter);
					INFO_PRINTF1(_L("Now given the two sets of set filters, step over traces of each type, some with matching primary but not secondary and vice versa"));
					result=logger.Start();//C.A. previously:result=logger.StartOutputting();
					if(result!=KErrNone)
						INFO_PRINTF2(_L("ulogger server didn't start, with error %d"), result);
					/*****do the tracing**************/
					iApiRunConfig.iApiId = UPTKernelMultipleFunctionalityTest;
					result = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
					result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
					if(result!=KErrNone)
					{
						INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with error %d"), result);
					}
					if(multsetprimfilter)
					{
						delete(multsetprimfilter);
						multsetprimfilter=NULL;
					}
				}
			}
			else
				INFO_PRINTF1(_L("adding file as output plugin failed"));
			if(result==0&&BlockResult()==EPass)
					SetBlockResult(EPass);
			else
					SetBlockResult(EFail);
			INFO_PRINTF1(_L("Multiple logging to file with kernel side tracing has been tested - check output log. It should contain four lines of tracing of type, (sec,prim), (sec+2, prim), (sec, prim+2), (sec+2, prim+2)"));
			configIni.ClearL(logger);
		}
		if(setprimfilter)
		{
			delete(setprimfilter);
			setprimfilter=NULL;
		}
		INFO_PRINTF1(_L("Now shutdown the ulogger server session"));
	    logger.Close();
		//  **************   Block end ****************
		}
	  return ETrue;
	}



/*******************************************************************************/
/*********************************Printf Test***************************/
/*******************************************************************************/

CTCMPrintfLoggingWrapper::~CTCMPrintfLoggingWrapper()
/**
 * Destructor
 */
	{
	}

CTCMPrintfLoggingWrapper::CTCMPrintfLoggingWrapper()
/**
 * Constructor
 */
	{
	}


CTCMPrintfLoggingWrapper* CTCMPrintfLoggingWrapper::NewLC()
	{
	CTCMPrintfLoggingWrapper* self = new (ELeave)CTCMPrintfLoggingWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMPrintfLoggingWrapper* CTCMPrintfLoggingWrapper::NewL()
	{
	CTCMPrintfLoggingWrapper* self=CTCMPrintfLoggingWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMPrintfLoggingWrapper::ConstructL()
	{
	}

TAny* CTCMPrintfLoggingWrapper::GetObject()
	{
	return NULL;
	}
TBool CTCMPrintfLoggingWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
		{
		TApiRunResults apiRunResults;
		TRunConfigurer::Init(apiRunResults);
		User::After(1000);
		//  ************** Delete the Block, the block start ****************
			INFO_PRINTF1(_L("About to test ulogger printf tracing with one primary and one secondary"));  //Block start
			TInt result=0;
			RULogger logger;
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
			CClearConfig configIni;
			configIni.ClearL(logger);
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(KGroupId);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) KComponentId);
			result = logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setprimfilter);
			INFO_PRINTF2(_L("Settting primary filter %d"), result);
			result = logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(setsecondfilter);
			result =0;
			INFO_PRINTF2(_L("Settting secondary filter %d"), result);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step

			_LIT(KTextsetting, "output_path");
			_LIT(KTextvalue, "C:\\logs\\ULoggerPrintfTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
			EmptyFile(KTextvalue);
#endif
			if(result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				result=logger.ActivateOutputPlugin(mediaptr);//C.A. previously:result=logger.SetActiveOutputPlugin(mediaptr);
				if(result==0||result==-11)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					TPluginConfiguration pluginConfig;
					pluginConfig.SetKey(KTextsetting);
					pluginConfig.SetValue(KTextvalue);
					result=logger.SetPluginConfigurations(mediaptr, pluginConfig);
					if(result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));
			if(result==0)
			{
				/************** Start Logging to File****************/
				/************* Testing NONSTATIC printf methods**********************/

				result=logger.Start();//C.A. previously:result=logger.StartOutputting();
				INFO_PRINTF2(_L("ulogger start returns error %d"), result);
				INFO_PRINTF1(_L("now step over some Ost printf statements"));
				int tracetag =0;
				TBool ret=1;

				INFO_PRINTF1(_L("******Testing user side NONSTATIC Trace Printf Methods******"));

				ret=KErrNone;

				bool OstStatus=0;
#ifdef TE_UPT_TRACE_ENABLED
		OstStatus = 1;
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		OstStatus = 0;
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif

//				if((ret&&OstStatus)||(!ret&&!OstStatus))
//				{
//					result=0;
//					INFO_PRINTF1(_L("Tracing printfs carried out succesfully"));
//					if(ret==1)
//						INFO_PRINTF1(_L("logfile should contain appropriate printf tracing"));
//					else
//						INFO_PRINTF1(_L("logfile should be empty of printf trace statements"));
//				}
//				else
//				{
//					result=1;
//					INFO_PRINTF2(_L("Tracing Failed on Ost printf macro %d"), tracetag);
//				}

				/************* Now Test STATIC printf methods**********************/

				INFO_PRINTF1(_L("******Testing user side STATIC Trace Printf Methods******"));

				ret=KErrNone;
				/*test user side static printf's*/
				for(tracetag=EUptPrintTDesC8_big; tracetag!=EUptPrintfTRefByValueTDesC16+1&&ret==KErrNone;tracetag++)
				{
				iApiRunConfig.iApiId = tracetag;
				ret = 1;
				ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
				INFO_PRINTF3(_L("Ost printf function %d has been executed with return value %d"), tracetag, ret);

				if(ret==KErrNone&&OstStatus)
					{
					StoreTraceInfo(ETrue,(TUPTApiUsed)tracetag,0, 0,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
					}
				}

#ifdef TE_UPT_TRACE_ENABLED
		OstStatus = 1;
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		OstStatus = 0;
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif

		result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
		if (result!=KErrNone)
			INFO_PRINTF1(_L("Ulogger has not been stopped succesfully"));

				if((!ret&&OstStatus)||(!ret&&!OstStatus))
				{
					result=0;
					INFO_PRINTF1(_L("Tracing printfs carried out succesfully"));
					if(ret==0)
						{
						INFO_PRINTF1(_L("logfile should contain appropriate printf tracing"));
						if(OstStatus)
						    {
                            if(!ReadFromLogFile(KTextvalue))
                                {
                                result=CompareTraces();
                                if(result)
                                    INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), result);
                                //prob set the test step result here
                                }
						    }
						}
					else
						INFO_PRINTF1(_L("logfile should be empty of printf trace statements"));
				}
				else
				{
					result=1;
					INFO_PRINTF2(_L("Tracing Failed on Ost printf macro %d"), tracetag);
				}
			}
			if (!result)
			    SetBlockResult(EPass);
			else
			    SetBlockResult(EFail);
		}
	  return ETrue;
	}



/******************************************************************************/
/*********************************Kernel Side Printf Test***************************/
/*******************************************************************************/

CTCMKernelPrintfLoggingWrapper::~CTCMKernelPrintfLoggingWrapper()
/**
 * Destructor
 */
	{
	}

CTCMKernelPrintfLoggingWrapper::CTCMKernelPrintfLoggingWrapper()
/**
 * Constructor
 */
	{
	}


CTCMKernelPrintfLoggingWrapper* CTCMKernelPrintfLoggingWrapper::NewLC()
	{
	CTCMKernelPrintfLoggingWrapper* self = new (ELeave)CTCMKernelPrintfLoggingWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMKernelPrintfLoggingWrapper* CTCMKernelPrintfLoggingWrapper::NewL()
	{
	CTCMKernelPrintfLoggingWrapper* self=CTCMKernelPrintfLoggingWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMKernelPrintfLoggingWrapper::ConstructL()
	{
	}

TAny* CTCMKernelPrintfLoggingWrapper::GetObject()
	{
	return NULL;
	}
TBool CTCMKernelPrintfLoggingWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
		INFO_PRINTF1(_L("About to test ulogger start with one primary and one secondary, kernel side printf logging"));  //Block start
		TInt result=0;
		RULogger logger;
		result = logger.Connect();
		CClearConfig configIni;
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
		CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		setprimfilter->AppendL(KGroupId);
		RArray<TUint32> setsecondfilter;
		setsecondfilter.Append((TUint32) KComponentId);
		logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setprimfilter);
		logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(setsecondfilter);
		INFO_PRINTF1(_L("about to set file as output plugin"));
		//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
		_LIT(KTextvalue, "C:\\logs\\ULoggerKernelPrintfTest.utf");
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(KTextvalue);
#endif
		//result=logger.AddOutputPlugin(KTextplugin);
		if(result==0)
			{
			INFO_PRINTF1(_L("file set as output plugin ok"));
			_LIT8(KTextmedia,"uloggerfileplugin");
			TPtrC8 mediaptr(KTextmedia);
			result=logger.ActivateOutputPlugin(mediaptr);//C.A. previously:result=logger.SetActiveOutputPlugin(mediaptr);
			if(result==0||result==-11)
				{
				INFO_PRINTF1(_L("file set as active output plugin ok"));
				TPluginConfiguration setPluginConfigs;
				setPluginConfigs.SetKey(KTextsetting);
				setPluginConfigs.SetValue(KTextvalue);
				result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
				if(result==0)
					INFO_PRINTF1(_L("output plugin settings set ok"));
				else
					INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
			else
				INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
		else
			INFO_PRINTF1(_L("add file as output plugin failed"));
		//TInt r = KErrNone;

		if(result==0)
			{
			TApiRunResults apiRunResults;
			TRunConfigurer::Init(apiRunResults);

			/************** Start Logging to File****************/

			logger.Start();//C.A. previously:logger.StartOutputting();
			TInt ret=KErrNone;
			INFO_PRINTF1(_L("******Testing kernel side STATIC Trace Printf Methods******"));
			/*test kernel side non static printf's*/
			TInt tracetag = EUptKernelPrintfchar;
			iApiRunConfig.iApiId = tracetag;
			ret = 1;
			ret = iTestTracer.SendTraceL(iApiRunConfig, apiRunResults);
			INFO_PRINTF3(_L("Ost kernel macro %d has been executed with return value %d"), tracetag, ret);
			//empty out array of previously sent and logged traces
			iLoggedTraces.Reset();
			iSentTraces.Reset();
			if (ret==KErrNone)
				{
				StoreTraceInfo(ETrue,(TUPTApiUsed)tracetag,0, 0,iApiRunConfig.iHasContext, iApiRunConfig.iHasProgramCounter);
				}

#ifdef TE_UPT_TRACE_ENABLED
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
			/************** Stop Logging to File****************/

			result=logger.Stop();//C.A. previously:result=logger.StopOutputting();
			if(result!=KErrNone)
				INFO_PRINTF2(_L("Call to Ulogger.StopOutputting() has failed with err %d"), result);

#ifdef TE_UPT_TRACE_ENABLED
			if(!ReadFromLogFile(KTextvalue))
					{
					result=CompareTraces();
					if(result)
						INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), result);
						//prob set the test step result here
					}
#endif
			}
		else
			INFO_PRINTF1(_L("adding file as output plugin failed"));
		if(result==0)
			SetBlockResult(EPass);
		else
			SetBlockResult(EFail);
		INFO_PRINTF1(_L("Simple printf logging to file has been tested with kernel side tracing- check output log"));

		configIni.ClearL(logger);

		if(setprimfilter)
			{
			delete(setprimfilter);
			setprimfilter=NULL;
			}
		INFO_PRINTF1(_L("Now shutdown the ulogger server session"));
		logger.Close();
		}


	//  **************   Block end ****************

	return ETrue;
	}

