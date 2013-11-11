// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file te_uloggerapiteststep.cpp
 @internalTechnology
*/
#include "te_uloggerapiteststep.h"
#include "te_uloggermclsuitedefs.h"


using namespace Ulogger;

/*******************************************************************************/
/*********************************Primary Filter Test***************************/
/*******************************************************************************/

CULoggerAPIPrimTestStep::~CULoggerAPIPrimTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerAPIPrimTestStep::CULoggerAPIPrimTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerAPIPrimTestStep);
	}

TVerdict CULoggerAPIPrimTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger API/uloggerclient.h setting Primary filter functionality, for negative testing edit source"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CULoggerAPIPrimTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only logger.GetS called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test Result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
	
		TInt Result=0;
		RULogger logger;
		CArrayFixFlat<TUint8> *setfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		setfilter->AppendL(UTracePrimary);
		CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);				
		CArrayFixFlat<TUint8> *removefilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		removefilter->AppendL(setfilter->At(0));	

		/**************First set primary filter*************/

		Result=logger.Connect();
		INFO_PRINTF2(_L("connect err %d"), Result);
		CClearConfig configIni;
		configIni.ClearL(logger);
		INFO_PRINTF2(_L("connect err %d"), Result);
		Result=logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);
		
		if(Result==KErrNone)
			INFO_PRINTF2(_L("Primary filter has been set with single filter, %d, check log"), setfilter->At(0));
		else
			INFO_PRINTF2(_L("Primary filter was not set, error returned, %d"), Result);
	
		/**************Now get the primary filter set above************/
	
		logger.GetPrimaryFiltersEnabled(*getfilter);
		INFO_PRINTF1(_L("Get Primary filter returns the following filters"));
		for(TInt i=0;i!=getfilter->Count();i++)
		{
			INFO_PRINTF2(_L("get filter returns %d "),getfilter->At(i));
		}
		if(getfilter->At(0)==setfilter->At(0))
		{
			Result=0;
			INFO_PRINTF1(_L("Get primary filter successful"));	
		}
		else
		{
			INFO_PRINTF1(_L("Get primary filter failed"));
			Result=1;
		}
		getfilter->Reset();
		/*****************Now remove the above set filter***************/

		if(Result==0)
		{
			Result=logger.SetPrimaryFiltersEnabled(*removefilter, EFalse);
			INFO_PRINTF2(_L("Remove Primary filters single filter, 0 if succesful: %d"), Result);
			logger.GetPrimaryFiltersEnabled(*getfilter);
			if(getfilter->Count()==0&&Result==KErrNone)
			{
				INFO_PRINTF1(_L("Remove primary filter successful"));
				Result=0;
			}
			else
			{
				if(Result!=0)
					INFO_PRINTF1(_L("Remove primary filter failed, as filter has not been set in the config file"));
				else
					INFO_PRINTF1(_L("Remove primary filter failed, as able to pass same filter option to RemovePrimaryFilter"));
				Result=1;
			}
		}
		getfilter->Reset();
		
		/****Now if the above steps have passed proceed to testing multiple filters***/

		if(Result==1)
			SetTestStepResult(EFail);
		else
		{
			CArrayFixFlat<TUint8> *setfilters = new (ELeave)CArrayFixFlat<TUint8>(10);
			CArrayFixFlat<TUint8> *getfilters = new (ELeave)CArrayFixFlat<TUint8>(10);
			for(TInt i=0;i!=MaxNumberOfPrimaryFilters;i++)
			{
				setfilters->AppendL((TUint8)(i));
			}
		
			/**************First set  multiple primary filter*************/

			Result=logger.SetPrimaryFiltersEnabled(*setfilters,ETrue);
			if(Result==KErrNone)
				INFO_PRINTF1(_L("Primary filter has been set with multiple filters,from 0 to 255"));
			else
				INFO_PRINTF2(_L("Multiple primary filters have not been set succesfully, error code %d"), Result);
		
			/**************Now get the primary filter set above************/
			logger.Connect();
			logger.GetPrimaryFiltersEnabled(*getfilters);
			if(getfilters->Count()==setfilters->Count())
				INFO_PRINTF1(_L("setfilter and getfilter have the same count"));
			else
			{
				INFO_PRINTF1(_L("setfilter and getfilter have different counts"));
				INFO_PRINTF2(_L("setfilter has %d"), setfilters->Count());
				INFO_PRINTF2(_L("getfilter has %d"), getfilters->Count());
				Result=1;
			}
			if(Result==0)
			{
				for(TInt i=0;i!=getfilters->Count() && Result==0;i++)
				{
					for(TInt j=0;j!=setfilters->Count() && Result!=0;j++)
					{
						if(getfilters->At(i)==setfilters->At(j))
							Result=0;
						else
							Result=1;
					}
				}
			}
			if(Result==0)
				INFO_PRINTF1(_L("Get multiple primary filters successful"));
			else
				INFO_PRINTF1(_L("Get multiple primary filters failed"));
			INFO_PRINTF1(_L("Get Primary filters returns multiple filters"));
		//	for(int i=0;i!=MaxNumberOfPrimaryFilters;i++)
		//		INFO_PRINTF2(_L(" %d"), getfilters->operator[](i));	//uncomment to verify get filters test correct
			getfilters->Reset();
			
			/*****************Given this is successful, now remove the above set filter***************/
	
			if(Result==0)
			{
				logger.Connect();
				logger.SetPrimaryFiltersEnabled(*removefilter, EFalse);
			//	getfilter.Append(removefilter);
			//	logger.SetPrimaryFilter(getfilter);//uncomment these to negative test the test suite
				logger.GetPrimaryFiltersEnabled(*getfilters);
				for(TInt i=0; i!=getfilters->Count() && Result==0;i++)
				{
					if(getfilters->At(i)==removefilter->At(0))
					{
						INFO_PRINTF1(_L("Remove primary filter in multiple test failed"));
						Result=1;
					}
					else
						Result=0;
				}
				if(Result==0)
					INFO_PRINTF1(_L("Remove primary filter in multiple test successful"));
				else
					INFO_PRINTF1(_L("Remove primary filter in multiple test failed"));
			}
			INFO_PRINTF1(_L("Now, given that the above tests have passed, negative test the primary filter options"));
			INFO_PRINTF1(_L("First reset the removed filter, then proceed"));
			setfilter->At(0)=removefilter->At(0);
			logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);
			if(Result==0)
			{
				INFO_PRINTF1(_L("Removed filter has been reset"));
			
				/************Now try to set a primary filter value greater than that allowed***/
			
				setfilter->At(0)=((TPrimaryFilter)(MaxNumberOfPrimaryFilters));
				INFO_PRINTF2(_L("setfilter is %d"), setfilter->At(0));
				TInt r=logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);
				if((r==KErrNone)&&(setfilter->At(0)==MaxNumberOfPrimaryFilters-256))
				{
					INFO_PRINTF3(_L("Primary filter has been set with filter %d,corresponding to the passed value %d, this is allowed as duplicate! Passed"), setfilter->At(0),MaxNumberOfPrimaryFilters );
					Result=0;
				}
				else if((r==KErrNone)&&(setfilter->At(0)!=MaxNumberOfPrimaryFilters-256))
				{
					INFO_PRINTF3(_L("Primary filter has been set with filter %d,corresponding to the passed value %d, this is not allowed as not duplicate! Failed"), setfilter->At(0),MaxNumberOfPrimaryFilters );
					Result=1;
				}
				else
				{
					INFO_PRINTF3(_L("Primary filter has not been set with filter %d, corresponding to the passed value %d. Passed"), setfilter->At(0), MaxNumberOfPrimaryFilters);
					Result=0;
				}
				
				/************Now try to set more primary filter values greater than available***/

				{   
					INFO_PRINTF1(_L("Now try to set more filter than available using a duplicate"));
					if(Result==0)
					{
						setfilter->At(0)=((TPrimaryFilter)(MaxNumberOfPrimaryFilters-1));
						Result=logger.SetPrimaryFiltersEnabled(*setfilter,ETrue);
						if((Result==KErrNone)&&(setfilter->At(0)!=MaxNumberOfPrimaryFilters-1))
						{
							INFO_PRINTF1(_L("More primary filters than available can be set. Failed"));
							Result=1;
						}
						else
						{
							INFO_PRINTF1(_L("More primary filters than available cannot be set OR the filter being set is a duplicate. Passed"));
							Result=0;
						}
					}
				}
			}
			if(Result==1)
				SetTestStepResult(EFail);
			else
				SetTestStepResult(EPass);
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
	  return TestStepResult();
	}



TVerdict CULoggerAPIPrimTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Completed Primary Filter test"));
	return TestStepResult();
	}



/*******************************************************************************/
/*********************************Secondary Filter Test***************************/
/*******************************************************************************/


CULoggerAPISecondTestStep::~CULoggerAPISecondTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerAPISecondTestStep::CULoggerAPISecondTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerAPISecondTestStep);
	}

TVerdict CULoggerAPISecondTestStep::doTestStepPreambleL()
/** * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger API/uloggerclient.h setting Secondary filter functionality, for negative testing edit source"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CULoggerAPISecondTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only logger.GetS called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test Result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************

			TInt Result=0;
			RULogger logger;
			RArray<TUint32> setfilter;
			setfilter.Append((TUint32) UTraceSecondary);
			RArray<TUint32> dupfilter;
			RArray<TUint32> getfilter;				
			RArray<TUint32> removefilter;
			removefilter.Append(setfilter[0]);
		//	removefilter=32; //uncomment to negative test the remove filter option		

			/**************First set Secondary filter*************/		

			logger.Connect();
			CClearConfig configIni;
			configIni.ClearL(logger);
			Result = logger.SetSecondaryFiltersEnabled(setfilter,ETrue);
			INFO_PRINTF2(_L("Secondary filter has been set with single filter, %d, check log"), setfilter[0]);
			INFO_PRINTF2(_L("code: %d"), Result);
			
	
			/**************Now get the Secondary filter set above************/	

			Result = logger.GetSecondaryFiltersEnabled(getfilter);	
			INFO_PRINTF1(_L("Get Secondary filter returns the following filters"));
			INFO_PRINTF2(_L("code: %d"), Result);
			for(TInt i=0;i!=getfilter.Count();i++)
			{
				INFO_PRINTF2(_L("%d "),getfilter[i]);
			}
			if(getfilter[0]==setfilter[0])
			{
				Result=0;
				INFO_PRINTF1(_L("Get Secondary filter successful"));	
			}
			else
			{
				INFO_PRINTF1(_L("Get Secondary filter failed"));
				Result=1;
			}
			for(TInt i=0;i!=getfilter.Count();)//i++)
			{
				getfilter.Remove(0);
			}		

			/*****************Now remove the above set filter***************/	

			if(Result==0)
			{
				Result=logger.SetSecondaryFiltersEnabled(removefilter, EFalse);
				INFO_PRINTF2(_L("Remove Secondary filters single filter, 0 if succesful, -1 if failed: %d"), Result);//logger.RemoveSingleSecondary);
				logger.GetSecondaryFiltersEnabled(getfilter);
				if(getfilter.Count()==0)
				{
					INFO_PRINTF1(_L("Remove Secondary filter successful"));
					Result=0;
				}
				else
				{
					if(Result!=0)
						INFO_PRINTF1(_L("Remove Secondary filter failed, as filter has not been set in the config file"));
					else
						INFO_PRINTF1(_L("Remove Secondary filter failed, as able to pass same filter option to RemoveSecondaryFilter"));
					Result=1;
				}
			}
			
			/****Now if the above steps have passed proceed to testing multiple filters***/		

			if(Result==1)
				SetTestStepResult(EFail);
			else
			{
				RArray<TUint32> setfilters;
				RArray<TUint32> getfilters;
				INFO_PRINTF2(_L("maxsecondary %d"),MaxNumberOfSecondaryFilters);
				for(TInt i=0;i<MaxNumberOfSecondaryFilters;i++)
				{
					setfilters.Append((TUint32)(i));
				}	
				INFO_PRINTF1(_L("initialising values for multiple secondary filters ok, now set the filters etc"));
		
				/**************First set  multiple Secondary filter*************/		

				Result=logger.SetSecondaryFiltersEnabled(setfilters,ETrue);
				if(Result==KErrNone)
				INFO_PRINTF2(_L("Secondary filter has been set with multiple filters,from 0 to %d"), MaxNumberOfSecondaryFilters);
				else
					INFO_PRINTF2(_L("Multiple secondary filters have not been set succesfully, error code %d"), Result);
			
				/**************Now get the Secondary filter set above************/
			
				INFO_PRINTF2(_L("before get, getfilter has %d"), getfilters.Count());
				logger.GetSecondaryFiltersEnabled(getfilters);
				if(getfilters.Count()==setfilters.Count())
				{
					INFO_PRINTF1(_L("setfilter and getfilter have the same count"));
					INFO_PRINTF2(_L("setfilter has %d"), setfilters.Count());
					INFO_PRINTF2(_L("getfilter has %d"), getfilters.Count());
				}
				else
				{
					INFO_PRINTF1(_L("setfilter and getfilter have different counts"));
					Result=1;
						INFO_PRINTF2(_L("setfilter has %d"), setfilters.Count());
						INFO_PRINTF2(_L("getfilter has %d"), getfilters.Count());
				}
				for(TInt i=0;i!=getfilters.Count() && Result==0;i++)
				{
					for(TInt j=0;j!=getfilters.Count() && Result!=0;j++)
					{
						if(getfilters[i]==setfilters[j])
							Result=0;
						else
							Result=1;
					}
				}
				if(Result==0)
					INFO_PRINTF1(_L("Get multiple Secondary filters successful"));
				else
					INFO_PRINTF1(_L("Get multiple Secondary filters failed"));
				INFO_PRINTF1(_L("Get Secondary filters returns multiple filters"));
			//	for(int i=0;i!=getfilters.Count();i++)//MaxNumberOfSecondaryFilters;i++)
			//		INFO_PRINTF2(_L(" %d"), getfilters[i]);	//uncomment to verify get filters test correct - WARNING-loadsa data if use true maximum for secondary filters-will break TEF!!
				for(TInt i=0;i<getfilters.Count();)//i++)
				{
					getfilters.Remove(0);
				}
			
				/*****************Given this is successful, now remove the above set filter***************/
		
				if(Result==0)
				{
					logger.SetSecondaryFiltersEnabled(removefilter, EFalse);
				//	getfilter.Append(removefilter);
				//	logger.SetSecondaryFilter(getfilter);//uncomment these to negative test the test suite
					logger.GetSecondaryFiltersEnabled(getfilters);
					for(TInt i=0; i!=getfilters.Count() && Result==0;i++)
					{
						if(getfilters[i]==removefilter[0])
						{
							INFO_PRINTF1(_L("Remove Secondary filter in multiple test failed"));
							Result=1;
						}
						else
							Result=0;
					}
					if(Result==0)
						INFO_PRINTF1(_L("Remove Secondary filter in multiple test successful"));
					else
					INFO_PRINTF1(_L("Remove Secondary filter in multiple test failed"));
				}
				INFO_PRINTF1(_L("Now, given that the above tests have passed, negative test the secondary filter options"));
				INFO_PRINTF1(_L("First reset the removed filter, then proceed"));
				setfilter[0]=removefilter[0];
				logger.SetSecondaryFiltersEnabled(setfilter,ETrue);
				if(Result==0)
				{
					INFO_PRINTF1(_L("Removed filter has been reset"));
				
					/************Now try to set a secondary filter value greater than that allowed***/
			
					setfilter[0]=((TSecondaryFilter)(MaxNumberOfSecondaryFilters-1));
					INFO_PRINTF2(_L("setfilter is %d"), setfilter[0]);
					TInt r= logger.SetSecondaryFiltersEnabled(setfilter,ETrue);
					if((r==KErrNone)&&(setfilter[0]==MaxNumberOfSecondaryFilters-1))
					{
						INFO_PRINTF3(_L("Secondary filter has been set with filter %d,corresponding to the passed value %d, this is allowed as is duplicate. Passed"), setfilter[0],MaxNumberOfSecondaryFilters-1 );
						Result=0;
					}
					if((r==KErrNone)&&(setfilter[0]!=MaxNumberOfSecondaryFilters-1))
					{
						INFO_PRINTF3(_L("Secondary filter has been set with filter %d,corresponding to the passed value %d, this is not allowed. Failed"), setfilter[0],MaxNumberOfSecondaryFilters-1 );
						Result=1;
					}
					else
					
					{
						INFO_PRINTF3(_L("Secondary filter has not been set with filter %d, corresponding to the passed value %d , this is as expected as otherwise duplicate. Passed"), setfilter[0], MaxNumberOfSecondaryFilters-1);
						Result=0;
					}
					/************Now try to set more secodndary filter values greater than available***/
					
					INFO_PRINTF1(_L("Now try to set more filter than available"));
					if(Result==0)
					{
						setfilter[0]=((TSecondaryFilter)(MaxNumberOfSecondaryFilters+3));
						Result=logger.SetSecondaryFiltersEnabled(setfilter,ETrue);
						if(Result==KErrNone)
						{
							INFO_PRINTF1(_L("More secondary filters than available can be set. Failed"));
							Result=1;
						}
						else
						{
							INFO_PRINTF1(_L("More secondary filters than available cannot be set. Passed"));
							Result=0;
						}
					}		
				}
				INFO_PRINTF1(_L("Now call clear config to check removing multiple second filters performance"));
				configIni.ClearL(logger);
				INFO_PRINTF1(_L("after clear config- multiple second filters performance"));
		
				if(Result==1)
					SetTestStepResult(EFail);
				else
					SetTestStepResult(EPass);		
			}
			logger.Close();
			//  **************   Block end ****************
		}
	  return TestStepResult();
	}



TVerdict CULoggerAPISecondTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Completed Secondary Filter test"));
	return TestStepResult();
	}


/*******************************************************************************/
/*********************************Logging Start/Stop Test***********************/
/*******************************************************************************/

CULoggerAPILoggingTestStep::~CULoggerAPILoggingTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerAPILoggingTestStep::CULoggerAPILoggingTestStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KULoggerAPILoggingTestStep);
	}

TVerdict CULoggerAPILoggingTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger API/uloggerclient.h logging with user side tracing functionality"));
 
 	if(NumberOfUTraceMacros!=18)
		{
			INFO_PRINTF1(_L("WARNING! Number of available UTrace Macros is not the same as accounted for in calls to UTrace!! Update ULoggerAPITest.cpp, (logging test step) accordingly!"));	
			SetTestStepResult(EFail);
		}
	else	
		SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CULoggerAPILoggingTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only logger.GetS called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test Result value will be EPass.
 */
	{
	
	  if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************

			INFO_PRINTF1(_L("About to test ulogger start with one primary and one secondary"));  //Block start
			TInt Result=0;
			RULogger logger;
			logger.Connect();
#ifdef SysStartTest
			TInt SysStartResult=logger.Start();
			if(SysStartResult!=KErrInUse)
				INFO_PRINTF2(_L("Logger start returning incorrect error code of %d when already started by SysStart, should be KErrInUse (-14)"), SysStartResult);
			logger.Stop();
#endif	
#ifndef SysStartTest
			CClearConfig configIni;
			configIni.ClearL(logger);
#endif
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
#ifndef SysStartTest
		
			if(Result==0)	
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);
				INFO_PRINTF1(_L("file set as active output plugin ok"));
				_LIT(KTextsetting, "output_path");
				_LIT(KTextvalue, "C:\\logs\\ULoggerLoggingTest.log");
				TPluginConfiguration pluginConfig;
				pluginConfig.SetKey(KTextsetting);
				pluginConfig.SetValue(KTextvalue);
				Result=logger.SetPluginConfigurations(mediaptr, pluginConfig);
				if(Result==0)
					INFO_PRINTF1(_L("output plugin settings set ok"));
				else
					INFO_PRINTF2(_L("setting output plugin settings failed with err %d"), Result);
			}
			else
				INFO_PRINTF2(_L("add file as output plugin failed with err %d"), Result);
#endif		
			if(Result==0)
			{
			/************** Start Logging to File****************/

			Result=logger.Start();
			INFO_PRINTF2(_L("ulogger start returns error %d"), Result);
			INFO_PRINTF1(_L("now step over some UTrace statements"));
			int tracetag =0;
			TBool ret=1;
			CUTraces Testtracer;
			INFO_PRINTF1(_L("******Testing NONSTATIC Trace Object Methods******"));
			/*Comment out this line to switch off tracing as a negative test*/
			for(tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
			{
					ret = 0;
					ret = Testtracer.DoTheTrace(tracetag);
					INFO_PRINTF3(_L("UTrace macro %d has trace size %d"), tracetag, Testtracer.SizeOfTrace(tracetag));
			}
			bool UTraceStatus=0;	
#ifdef SYMBIAN_TRACE_ENABLE 
	UTraceStatus = 1;
		INFO_PRINTF1(_L("UTRACING HAS BEEN ENABLED!"));
#else
		UTraceStatus = 0;
		INFO_PRINTF1(_L("UTRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
			if((ret&&UTraceStatus)||(!ret&&!UTraceStatus))
			{
				Result=0;
				INFO_PRINTF1(_L("UTracing carried out succesfully"));
				if(ret==1) 
					INFO_PRINTF1(_L("logfile should contain appropriate tracing"));
				else
					INFO_PRINTF1(_L("logfile should be empty of trace statements"));
			}
			else
			{
				Result=1;
				INFO_PRINTF2(_L("UTracing Failed on UTrace macro %d"), tracetag);
			}
			
			/*************Now test the set method************************/
			
			INFO_PRINTF1(_L("******Testing NONSTATIC Trace Set Methods******"));
			TUTrace TestTraceObject(UTracePrimary, UTraceSecondary, schema, context, pc);
			for(tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
			{
				ret = 0;
				ret = Testtracer.DoTheSetTrace(tracetag, TestTraceObject);
				INFO_PRINTF3(_L("UTrace set macro %d has trace size %d"), tracetag, Testtracer.SizeOfTrace(tracetag));
			}
			if((ret&&UTraceStatus)||(!ret&&!UTraceStatus))
			{
				Result=0;
				INFO_PRINTF1(_L("UTracing carried out succesfully for set method"));
				if(ret==1) 
					INFO_PRINTF1(_L("logfile should contain appropriate tracing"));
				else
					INFO_PRINTF1(_L("logfile should be empty of trace statements"));
			}
		
			else
			{
				Result=1;
				INFO_PRINTF2(_L("UTracing Failed on UTrace macro %d for set method "), tracetag);
			}
			
				/*************Now test the static method************************/
				
			INFO_PRINTF1(_L("******Testing STATIC Trace Methods******"));

			
			for(tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
			{
					ret = 0;
					ret = Testtracer.DoTheStaticTrace(tracetag);
					INFO_PRINTF3(_L("UTrace static macro %d has trace size %d"), tracetag, Testtracer.SizeOfTrace(tracetag));
			}
			if((ret&&UTraceStatus)||(!ret&&!UTraceStatus))
			{
				Result=0;
				INFO_PRINTF1(_L("UTracing carried out succesfully for static method"));
				if(ret==1) 
					INFO_PRINTF1(_L("logfile should contain appropriate tracing"));
				else
					INFO_PRINTF1(_L("logfile should be empty of trace statements"));

			}
			
		
			else
			{
				Result=1;
				INFO_PRINTF2(_L("UTracing Failed on UTrace macro  %d for static method"), tracetag);
			}
			
			/************** Stop Logging to File****************/

			Result=logger.Stop();
			if(Result==0)
			{
				ret = Testtracer.DoTheTrace(0);
				INFO_PRINTF1(_L("logfile should contain appropriate tracing without an extra UTrace(0) trace at the end, if it is present ULogger.Stop() has failed in functionality"));
			}
			else
			{
				INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with err %d"), Result);
			}
		}
		else
			INFO_PRINTF1(_L("adding file as output plugin failed"));
		if(Result==0)
				SetTestStepResult(EPass);
		else
				SetTestStepResult(EFail);
		INFO_PRINTF1(_L("Simple logging to file has been tested - check output log"));
	
		/***************************negative test***********************/

		if(TestStepResult()==EPass)	
		{
			INFO_PRINTF1(_L("Now do some negative logging testing."));
			if(Result==KErrNone)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				if(Result==0)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KnegTextvalue, "C:\\logs\\ULoggerNegativeLoggingTest.log");
					TPluginConfiguration negPluginConfig;
					negPluginConfig.SetKey(KTextsetting);
					negPluginConfig.SetValue(KnegTextvalue);
					Result=logger.SetPluginConfigurations(mediaptr, negPluginConfig);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));	
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
				INFO_PRINTF1(_L("Log over traces with different filters set - there should only be one line in logfile corrseponding to the set filters."));
				if(Result==0)
				{
					logger.Start();

					/*****do the tracing**************/

					bool ret = TUTrace::Trace(UTracePrimary, UTraceSecondary, schema, context, pc);
					INFO_PRINTF2(_L("tracing ret %d"), ret);
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+1),(TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
					INFO_PRINTF2(_L("tracing ret %d"), ret);
	
					/************** Stop Logging to File****************/

					Result=logger.Stop();
					if(Result!=0)
					{
					INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with err %d"), Result);
					}					
					INFO_PRINTF1(_L("Set some more filters and remove the original set in ulogger, then trace over the original types of traces - there should be no extra lines in logfile above"));
					
					/**************Now Change the filter settings********/
					CArrayFixFlat<TUint8> *removeprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					removeprimfilter->AppendL(setprimfilter->At(0));
					RArray<TUint32> removesecondfilter;
					removesecondfilter.Append(setsecondfilter[0]);
					Result=logger.SetPrimaryFiltersEnabled(*removeprimfilter, EFalse);
					if(Result==0)
					{
						INFO_PRINTF1(_L("Call to removeprimary has passed"));
					}
					Result=logger.SetSecondaryFiltersEnabled(removesecondfilter, EFalse);
					if(Result==0)
					{
						INFO_PRINTF1(_L("Call to removesecondary has passed"));
					}
					CArrayFixFlat<TUint8> *newsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					newsetprimfilter->AppendL((TUint8)(UTracePrimary+2));
					RArray<TUint32> newsetsecondfilter;
					newsetsecondfilter.Append((TUint32) (UTraceSecondary+2));
					logger.SetPrimaryFiltersEnabled(*newsetprimfilter,ETrue);
					logger.SetSecondaryFiltersEnabled(newsetsecondfilter,ETrue);
					CUTraces Testtracer;
					ret=1;	
					logger.Start();
					
					/*****do the tracing**************/

					/*Comment out this line to switch off tracing as a negative test*/
					int tracetag=0;
					for(tracetag=0; tracetag!=NumberOfUTraceMacros&&ret;tracetag++)
					{
						ret = 0;
						ret = Testtracer.DoTheTrace(tracetag);
						INFO_PRINTF3(_L("UTrace macro %d has trace size %d"), tracetag, Testtracer.SizeOfTrace(tracetag));
					}
					if(ret==1)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("BTrace filters are not being reset with Start()/stop(), so logging over filters that have been unset is occuring. Fail"));
					}
				
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2),(TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
					INFO_PRINTF2(_L("tracing ret %d"), ret);
										
					/************** Stop Logging to File****************/

					Result=logger.Stop();
					if(Result!=0)
					{
					INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with err %d"), Result);
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
			if(Result==0&&TestStepResult()==EPass)
					SetTestStepResult(EPass);
			else
					SetTestStepResult(EFail);
			INFO_PRINTF1(_L("Simple negative logging to file has been tested - check output log"));
		}
	
		/***************************multiple type test***********************/
   		 // From negative test: primary has been set to UTracePrimary+2 and secondary to UTraceSecondary+2
		if(TestStepResult()==EPass)	
		{	
			INFO_PRINTF1(_L("Now do some multiple type logging testing."));
			//CClearConfig configIni;
			//configIni.ClearL(logger);
		
			if(Result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				if(Result==0)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KmultTextvalue, "C:\\logs\\ULoggerMultipleLoggingTest.log");
					TPluginConfiguration pluginConfig;
					pluginConfig.SetKey(KTextsetting);
					pluginConfig.SetValue(KmultTextvalue);				
					Result=logger.SetPluginConfigurations(mediaptr, pluginConfig);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));	
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
				if(Result==0)
				{
					TInt i;
					CArrayFixFlat<TUint8> *multsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					multsetprimfilter->AppendL(UTracePrimary);
					RArray<TUint32> multsetsecondfilter;
					multsetsecondfilter.Append((TUint32) (UTraceSecondary));
					logger.SetPrimaryFiltersEnabled(*multsetprimfilter,ETrue);
					TInt errCode = logger.SetSecondaryFiltersEnabled(multsetsecondfilter,ETrue);			
					
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
					logger.GetPrimaryFiltersEnabled(*primFilters);
					
					for(i=0; i<primFilters->Count(); ++i)
						{
						TBuf<16> buf;
						buf.AppendFormat(_L("%d "), primFilters->At(i));
						INFO_PRINTF1(buf);
						}
					CleanupStack::PopAndDestroy(); //primFiletrs
					
					INFO_PRINTF1(_L("Secondary Filetrs:"));
					RArray<TUint32> secFilters;
					errCode = logger.GetSecondaryFiltersEnabled(secFilters);
					if(errCode != KErrNone)
						INFO_PRINTF2(_L("Getting secondary filters failed: %d"), errCode);
					
					for(i=0; i<secFilters.Count(); ++i)
						{
						TBuf<16> buf;
						buf.AppendFormat(_L("%d"), secFilters[i]);
						INFO_PRINTF1(buf);
						}
					secFilters.Close();
					
					INFO_PRINTF1(_L("Secondary Filetring is:"));
					TBool secF = EFalse;
					logger.GetSecondaryFilteringEnabled(secF);
					if(secF)
						INFO_PRINTF1(_L("ENABLED"));
					else
						INFO_PRINTF1(_L("DISABLED"));
						
					Result=logger.Start();
					if(Result!=KErrNone)
						INFO_PRINTF2(_L("ulogger server didn't start, with error %d"), Result);
					
					/*****do the tracing**************/
			
					bool ret = TUTrace::Trace(UTracePrimary,UTraceSecondary, schema, context, pc);
					if(ret==0)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec and prim) is not occuring. Fail"));
					}
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+1),(TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
					if(ret==1)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have not been set is occuring. Fail"));
					}
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary),(TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
					if(ret==0)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec+2 and prim) is not occuring. Fail"));
					}
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2),(TSecondaryFilter)(UTraceSecondary), schema, context, pc);
					if(ret==0)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec and prim+2) is not occuring. Fail"));
					}
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2),(TSecondaryFilter)(UTraceSecondary+2), schema, context, pc);
					if(ret==0)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have been set (both sec+2 and prim+2) is not occuring. Fail"));
					}
					
					INFO_PRINTF1(_L("checking UTracePrimary i UTraceSecondary"));
					TBuf<32> b;
					b.AppendFormat(_L("%d and %d"), UTracePrimary+2, UTraceSecondary+1);
					INFO_PRINTF1(b);
					
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+2),(TSecondaryFilter)(UTraceSecondary+1), schema, context, pc);
					if(ret==1)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have not been set (the sec) is occuring. Fail"));
					}
					ret = TUTrace::Trace((TPrimaryFilter)(UTracePrimary+1),(TSecondaryFilter)(UTraceSecondary), schema, context, pc);
					if(ret==1)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("logging over filters that have not been set (the prim) is occuring. Fail"));
					}
					ret = TUTrace::TracePrimary((TPrimaryFilter)(UTracePrimary+1), schema, context, pc);
					if(ret==1)
					{
						SetTestStepResult(EFail);
						INFO_PRINTF1(_L("fast logging over filters that have not been set (the prim) is occuring. Fail"));
					}
				
					/************** Stop Logging to File****************/

					Result=logger.Stop();
					if(Result!=KErrNone)
					{
						INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with error %d"), Result);
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
			if(Result==0&&TestStepResult()==EPass)
					SetTestStepResult(EPass);
			else
					SetTestStepResult(EFail);
			INFO_PRINTF1(_L("Multiple logging to file with user side tracing has been tested - check output log. It should contain four lines of tracing of type, (sec,prim), (sec+2, prim), (sec, prim+2), (sec+2, prim+2)"));
		}
		if(setprimfilter)
		{
			delete(setprimfilter);
			setprimfilter=NULL;
		}
		INFO_PRINTF1(_L("Now shutdown the ulogger server session"));
	    logger.Close();
#ifdef SysStartTest
		if(SysStartResult!=KErrInUse)
			SetTestStepResult(EFail);
#endif
		//  **************   Block end ****************
		}	
	  return TestStepResult();
	}
	



TVerdict CULoggerAPILoggingTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Completed Logging Test Step"));
	return TestStepResult();
	}
	
/*******************************************************************************/
/*********************************Kernel Side Logging Test**********************/
/*******************************************************************************/

CULoggerAPIKernelTestStep::~CULoggerAPIKernelTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerAPIKernelTestStep::CULoggerAPIKernelTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerAPIKernelTestStep);
	}

TVerdict CULoggerAPIKernelTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger API/uloggerclient.h logging with kernel side tracing functionality"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CULoggerAPIKernelTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only logger.GetS called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test Result value will be EPass.
 */
	{
#ifndef __WINSCW__
	  if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************

			INFO_PRINTF1(_L("About to test ulogger start with one primary and one secondary, kernel side"));  //Block start
			TInt Result=0;
			CUKernelTraces TestKernelTracer;
			RULogger logger;
			logger.Connect();
			CClearConfig configIni;
			configIni.ClearL(logger);
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);		
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
		
			if(Result==0)	
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);
				if(Result==0||Result==-11)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KTextvalue, "C:\\logs\\ULoggerKernelLoggingTest.log");
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KTextvalue);
					Result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));
			TInt r = KErrNone;
			r=User::LoadLogicalDevice(_L("eusbc.ldd"));
			if(r == KErrAlreadyExists)
			r = KErrNone;
			RDevUsbcClient usbDevice;
			if(Result==0)
			{
				/************** Start Logging to File****************/
			
				logger.Start();
				INFO_PRINTF1(_L("now step over some UTrace statements"));
				INFO_PRINTF1(_L("Device driver loading............."));
				if (r == KErrNone)
				{
					INFO_PRINTF1(_L("Device driver loaded successfully!!!!!!!!!!!!!!!!!!"));
					r = usbDevice.Open(0);
					INFO_PRINTF2(_L("Device driver opening %d"), r);
					if(r == KErrNone)
					{
						r=TestKernelTracer.DoTheKernelTrace(0, usbDevice);
						if(r==KErrNone)
							INFO_PRINTF2(_L("EndpointZeroRequestError %d"), r);
						else
							INFO_PRINTF2(_L("EndpointZeroRequestError failed %d"), r);
					}
				else
						INFO_PRINTF1(_L("Device driver opening failed"));	
				}
				else
				INFO_PRINTF2(_L("Device Driver didn't load, %d"), r);
#ifdef SYMBIAN_TRACE_ENABLE
		INFO_PRINTF1(_L("UTRACING HAS BEEN ENABLED!"));
#else
		INFO_PRINTF1(_L("UTRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
				/************** Stop Logging to File****************/
	
				Result=logger.Stop();
				if(Result!=KErrNone)
					INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with err %d"), Result);
			}
			else
				INFO_PRINTF1(_L("adding file as output plugin failed"));
			if(Result==0)
					SetTestStepResult(EPass);
			else
					SetTestStepResult(EFail);
			INFO_PRINTF1(_L("Simple logging to file has been tested with kernel side tracing- check output log"));
		
			/***************************negative test***********************/
		
			if(TestStepResult()==EPass)	
			{
				INFO_PRINTF1(_L("Now do some negative logging testing."));
				if(Result==KErrNone)
				{
					INFO_PRINTF1(_L("file set as output plugin ok"));
					_LIT8(KTextmedia,"uloggerfileplugin");
					TPtrC8 mediaptr(KTextmedia);
					if(Result==0)
					{
						INFO_PRINTF1(_L("file set as active output plugin ok"));
						_LIT(KTextsetting, "output_path");
						_LIT(KnegTextvalue, "C:\\logs\\ULoggerKernelNegativeLoggingTest.log");
						TPluginConfiguration negSetPluginConfigs;
						negSetPluginConfigs.SetKey(KTextsetting);
						negSetPluginConfigs.SetValue(KnegTextvalue);
						Result=logger.SetPluginConfigurations(mediaptr, negSetPluginConfigs);
						if(Result==0)
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
					Result=logger.SetPrimaryFiltersEnabled(*removeprimfilter, EFalse);
					if(Result==0)
					{
						INFO_PRINTF1(_L("Call to removeprimary has passed"));
					}
					Result=logger.SetSecondaryFiltersEnabled(removesecondfilter, EFalse);
					if(Result==0)
					{
						INFO_PRINTF1(_L("Call to removesecondary has passed"));
					}
					CArrayFixFlat<TUint8> *newsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					newsetprimfilter->AppendL((TUint8)(UTracePrimary+2));
					RArray<TUint32> newsetsecondfilter;
					newsetsecondfilter.Append((TUint32)(UTraceSecondary+2));
					logger.SetPrimaryFiltersEnabled(*newsetprimfilter,ETrue);
					logger.SetSecondaryFiltersEnabled(newsetsecondfilter,ETrue);
					if(Result==0)
					{
						logger.Start();
						INFO_PRINTF1(_L("now step over some UTrace statements"));
						r=TestKernelTracer.DoTheKernelTrace(0, usbDevice);
						if(r==KErrNone)
							INFO_PRINTF2(_L("EndpointZeroRequestError %d"), r);
						else
							INFO_PRINTF2(_L("EndpointZeroRequestError failed  %d"), r);
					}
					Result=logger.Stop();
					if(Result!=0)
					{
						INFO_PRINTF1(_L("Call to Ulogger.Stop() has failed"));
					}
					if(newsetprimfilter)
					{
						delete(newsetprimfilter);
						newsetprimfilter=NULL;
					}
				}
				else
					INFO_PRINTF1(_L("Incorrect error code when trying to add file as a plug-in when it already exists"));
				if(Result==0&&TestStepResult()==EPass)
						SetTestStepResult(EPass);
				else
						SetTestStepResult(EFail);
				INFO_PRINTF1(_L("Simple negative logging to file has been tested - check output log"));
		}

		/***************************multiple type test***********************/
	   	 // From negative test: primary has been set to UTracePrimary+2 and secondary to UTraceSecondary+2
		if(TestStepResult()==EPass)	
		{
			INFO_PRINTF1(_L("Now do some multiple type logging testing."));
			if(Result==0)
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				if(Result==0)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KmultTextvalue, "C:\\logs\\ULoggerKernelMultipleLoggingTest.log");
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KmultTextvalue);
					Result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));	
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
				if(Result==0)
				{
				/******************reset filters and change call for multiple logging*****/
				
					CArrayFixFlat<TUint8> *multsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					multsetprimfilter->AppendL(UTracePrimary);
					RArray<TUint32> multsetsecondfilter;
					multsetsecondfilter.Append((TUint32) (UTraceSecondary));
					logger.SetPrimaryFiltersEnabled(*multsetprimfilter,ETrue);
					logger.SetSecondaryFiltersEnabled(multsetsecondfilter,ETrue);
					INFO_PRINTF1(_L("Now given the two sets of set filters, step over traces of each type, some with matching primary but not secondary and vice versa"));
					Result=logger.Start();
					if(Result!=KErrNone)
						INFO_PRINTF2(_L("ulogger server didn't start, with error %d"), Result);
					/*****do the tracing**************/
					r=TestKernelTracer.DoTheKernelTrace(1, usbDevice);
					if(r!=KErrNone)
					{
						INFO_PRINTF2(_L("GetStringDescriptorLangId failed with error %d"), r);
					}
					Result=logger.Stop();
					if(Result!=KErrNone)
					{
						INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with error %d"), Result);
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
			if(Result==0&&TestStepResult()==EPass)
					SetTestStepResult(EPass);
			else
					SetTestStepResult(EFail);
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
#else
		INFO_PRINTF1(_L("Kernel tests not run on emulator"));
#endif
	  return TestStepResult();
	}



TVerdict CULoggerAPIKernelTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CULoggerAPIKernelTestStep"));
	return TestStepResult();
	}




/*******************************************************************************/
/*********************************Printf Test***************************/
/*******************************************************************************/

CULoggerAPIPrintfTestStep::~CULoggerAPIPrintfTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerAPIPrintfTestStep::CULoggerAPIPrintfTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerAPIPrintfTestStep);
	}

TVerdict CULoggerAPIPrintfTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Testing ULogger API/uloggerclient.h logging with user side printf tracing functionality"));
 		if(NumberOfPrintfMacros!=16)
		{
			INFO_PRINTF1(_L("WARNING! Number of available UTrace Printf Macros is not the same as accounted for in calls to UTrace!! Update ULoggerAPITest.cpp, (logging test step) accordingly!"));	
			SetTestStepResult(EFail);
		}
		else	
			SetTestStepResult(EPass);
		return TestStepResult();
	}


TVerdict CULoggerAPIPrintfTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only logger.GetS called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test Result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
			INFO_PRINTF1(_L("About to test ulogger printf tracing with one primary and one secondary"));  //Block start
			TInt Result=0;
			RULogger logger;
			logger.Connect();
			CClearConfig configIni;
			configIni.ClearL(logger);
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
		
			if(Result==0)	
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);
				if(Result==0||Result==-11)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KTextvalue, "C:\\logs\\ULoggerPrintfTest.log");
					TPluginConfiguration pluginConfig;
					pluginConfig.SetKey(KTextsetting);
					pluginConfig.SetValue(KTextvalue);
					Result=logger.SetPluginConfigurations(mediaptr, pluginConfig);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));
			if(Result==0)
			{
				/************** Start Logging to File****************/
				/************* Testing NONSTATIC printf methods**********************/

				Result=logger.Start();
				INFO_PRINTF2(_L("ulogger start returns error %d"), Result);
				INFO_PRINTF1(_L("now step over some UTrace printf statements"));
				int tracetag =0;
				TBool ret=1;
				CUTraces Testtracer;
				INFO_PRINTF1(_L("******Testing NONSTATIC Trace Printf Methods******"));
		
				/*Comment out this line to switch off tracing as a negative test*/
		
				for(tracetag=0; tracetag!=NumberOfPrintfMacros&&ret;tracetag++)
				{
					ret = 0;
					ret = Testtracer.DoThePrintf(tracetag);
					INFO_PRINTF3(_L("UTrace printf macro %d has trace size %d - UPDATE!!!!"), tracetag, 0);
				}
				bool UTraceStatus=0;	
#ifdef SYMBIAN_TRACE_ENABLE
		UTraceStatus = 1;
		INFO_PRINTF1(_L("UTRACING HAS BEEN ENABLED!"));
#else
		UTraceStatus = 0;
		INFO_PRINTF1(_L("UTRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
				if((ret&&UTraceStatus)||(!ret&&!UTraceStatus))
				{
					Result=0;
					INFO_PRINTF1(_L("UTracing printfs carried out succesfully"));
					if(ret==1) 
						INFO_PRINTF1(_L("logfile should contain appropriate printf tracing"));
					else
						INFO_PRINTF1(_L("logfile should be empty of printf trace statements"));
				}
				else
				{
					Result=1;
					INFO_PRINTF2(_L("UTracing Failed on UTrace printf macro %d"), tracetag);
				}
			
				/************* Now Test STATIC printf methods**********************/
				
				INFO_PRINTF1(_L("******Testing STATIC Trace Printf Methods******"));
	
				/*Comment out this line to switch off tracing as a negative test*/
				for(tracetag=0; tracetag!=NumberOfPrintfMacros&&ret;tracetag++)
				{
					ret = 0;
					ret = Testtracer.DoTheStaticPrintf(tracetag);
					INFO_PRINTF3(_L("UTrace printf macro %d has trace size %d - UPDATE!!!!"), tracetag, 0);
				}
#ifdef SYMBIAN_TRACE_ENABLE
		UTraceStatus = 1;
		INFO_PRINTF1(_L("UTRACING HAS BEEN ENABLED!"));
#else
		UTraceStatus = 0;
		INFO_PRINTF1(_L("UTRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
				if((ret&&UTraceStatus)||(!ret&&!UTraceStatus))
				{
					Result=0;
					INFO_PRINTF1(_L("UTracing printfs carried out succesfully"));
					if(ret==1) 
						INFO_PRINTF1(_L("logfile should contain appropriate printf tracing"));
					else
						INFO_PRINTF1(_L("logfile should be empty of printf trace statements"));
				}
				else
				{
					Result=1;
					INFO_PRINTF2(_L("UTracing Failed on UTrace printf macro %d"), tracetag);
				}
			}
			SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CULoggerAPIPrintfTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("completed user side printf logging tests"));
	
	return TestStepResult();
	}





/******************************************************************************/
/*********************************Kernel Side Printf Test***************************/
/*******************************************************************************/

CULoggerAPIKernelPrintfTestStep::~CULoggerAPIKernelPrintfTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerAPIKernelPrintfTestStep::CULoggerAPIKernelPrintfTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerAPIKernelPrintfTestStep);
	}

TVerdict CULoggerAPIKernelPrintfTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger API/uloggerclient.h kernel side printf logging functionality"));
	return TestStepResult();
	}


TVerdict CULoggerAPIKernelPrintfTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only logger.GetS called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test Result value will be EPass.
 */
	{
#ifndef __WINSCW__
	  if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
			INFO_PRINTF1(_L("About to test ulogger start with one primary and one secondary, kernel side printf logging"));  //Block start
			TInt Result=0;
			CUKernelTraces TestKernelTracer;
			RULogger logger;
			logger.Connect();
			CClearConfig configIni;
			configIni.ClearL(logger);
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);	
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
		
			//Result=logger.AddOutputPlugin(KTextplugin);
			if(Result==0)	
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);
				if(Result==0||Result==-11)
				{	INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KTextvalue, "C:\\logs\\ULoggerKernelPrintfTest.log");
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KTextvalue);
					Result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));
			TInt r = KErrNone;
			r=User::LoadLogicalDevice(_L("eusbc.ldd"));
			if(r == KErrAlreadyExists)
			r = KErrNone;
			RDevUsbcClient usbDevice;
			if(Result==0)
			{
				/************** Start Logging to File****************/
		
				logger.Start();
				INFO_PRINTF1(_L("now step over some UTrace statements"));
				INFO_PRINTF1(_L("Device driver loading............."));
				if (r == KErrNone)
				{
					INFO_PRINTF1(_L("Device driver loaded successfully!!!!!!!!!!!!!!!!!!"));
					r = usbDevice.Open(0);
					INFO_PRINTF2(_L("Device driver opening %d"), r);
					if(r == KErrNone)
					{
						r = TestKernelTracer.DoTheKernelPrintf(0, usbDevice);
						if(r==KErrNone)
							INFO_PRINTF2(_L("SetStringDescriptor %d"), r);
						else
							INFO_PRINTF2(_L("SetStringDescriptor failed %d"), r);
					}
					else
						INFO_PRINTF1(_L("Device driver opening failed"));
				}
				else
					INFO_PRINTF2(_L("Device Driver didn't load, %d"), r);
#ifdef SYMBIAN_TRACE_ENABLE
		INFO_PRINTF1(_L("UTRACING HAS BEEN ENABLED!"));
#else
		INFO_PRINTF1(_L("UTRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
				/************** Stop Logging to File****************/
	
				Result=logger.Stop();
				if(Result!=KErrNone)
					INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with err %d"), Result);
			}
			else
				INFO_PRINTF1(_L("adding file as output plugin failed"));
			if(Result==0)
					SetTestStepResult(EPass);
			else
					SetTestStepResult(EFail);
			INFO_PRINTF1(_L("Simple printf logging to file has been tested with kernel side tracing- check output log"));	

			/***************************negative test***********************/
	
			if(TestStepResult()==EPass)	
			{
				INFO_PRINTF1(_L("Now do some negative logging testing."));
				if(Result==KErrNone)
				{
					INFO_PRINTF1(_L("file set as output plugin ok"));
					_LIT8(KTextmedia,"uloggerfileplugin");
					TPtrC8 mediaptr(KTextmedia);
					if(Result==0||Result==-11)
					{	INFO_PRINTF1(_L("file set as active output plugin ok"));
						_LIT(KTextsetting, "output_path");
						_LIT(KnegTextvalue, "C:\\logs\\ULoggerKernelPrintfNegativeLoggingTest.log");
						TPluginConfiguration negSetPluginConfigs;
						negSetPluginConfigs.SetKey(KTextsetting);
						negSetPluginConfigs.SetValue(KnegTextvalue);
						Result=logger.SetPluginConfigurations(mediaptr, negSetPluginConfigs);
						if(Result==0)
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
					Result=logger.SetPrimaryFiltersEnabled(*removeprimfilter, EFalse);
					if(Result==0)
					{
						INFO_PRINTF1(_L("Call to removeprimary has passed"));
					}
					Result=logger.SetSecondaryFiltersEnabled(removesecondfilter, EFalse);
					if(Result==0)
					{
						INFO_PRINTF1(_L("Call to removesecondary has passed"));
					}
					CArrayFixFlat<TUint8> *newsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
					newsetprimfilter->AppendL((TUint8)(UTracePrimary+2));
					RArray<TUint32> newsetsecondfilter;
					newsetsecondfilter.Append((TUint32) (UTraceSecondary+2));
					logger.SetPrimaryFiltersEnabled(*newsetprimfilter,ETrue);
					logger.SetSecondaryFiltersEnabled(newsetsecondfilter,ETrue);
					if(Result==0)
					{
						logger.Start();
						INFO_PRINTF1(_L("now step over some UTrace statements"));
						r = TestKernelTracer.DoTheKernelPrintf(0, usbDevice);		
						if(r==KErrNone)
							INFO_PRINTF2(_L("SetStringDescriptor %d"), r);
						else
							INFO_PRINTF2(_L("SetStringDescriptor failed  %d"), r);
					}
					Result=logger.Stop();
					if(Result!=0)
					{
						INFO_PRINTF1(_L("Call to Ulogger.Stop() has failed"));
					}
					if(newsetprimfilter)
					{
						delete(newsetprimfilter);
						newsetprimfilter=NULL;
					}
				}
				else
					INFO_PRINTF1(_L("Incorrect error code when trying to add file as a plug-in when it already exists"));
				if(Result==0&&TestStepResult()==EPass)
						SetTestStepResult(EPass);
				else
						SetTestStepResult(EFail);
				INFO_PRINTF1(_L("Simple negative printf logging to file has been tested - check output log"));
			}	

		/***************************multiple type test***********************/
	   	 // From negative test: primary has been set to UTracePrimary+2 and secondary to UTraceSecondary+2
			if(TestStepResult()==EPass)	
			{
				INFO_PRINTF1(_L("Now do some multiple type printf logging testing."));
				if(Result==0)
				{
					INFO_PRINTF1(_L("file set as output plugin ok"));
					_LIT8(KTextmedia,"uloggerfileplugin");
					TPtrC8 mediaptr(KTextmedia);				
					if(Result==0||Result==-11)
					{	INFO_PRINTF1(_L("file set as active output plugin ok"));
						_LIT(KTextsetting, "output_path");
						_LIT(KmultTextvalue, "C:\\logs\\ULoggerKernelPrintfMultipleLoggingTest.log");
						TPluginConfiguration setPluginConfigs;
						setPluginConfigs.SetKey(KTextsetting);
						setPluginConfigs.SetValue(KmultTextvalue);
						Result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
						if(Result==0)
							INFO_PRINTF1(_L("output plugin settings set ok"));
						else
							INFO_PRINTF1(_L("setting output plugin settings failed"));	
					}
					else
						INFO_PRINTF1(_L("file not set as active output plugin - failed"));
					if(Result==0)
					{
						/******************reset filters and change call for multiple logging*****/
					
						CArrayFixFlat<TUint8> *multsetprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
						multsetprimfilter->AppendL(UTracePrimary);
						RArray<TUint32> multsetsecondfilter;
						multsetsecondfilter.Append((TUint32) (UTraceSecondary));
						logger.SetPrimaryFiltersEnabled(*multsetprimfilter,ETrue);
						logger.SetSecondaryFiltersEnabled(multsetsecondfilter,ETrue);
						INFO_PRINTF1(_L("Now given the two sets of set filters, step over traces of each type, some with matching primary but not secondary and vice versa"));
						Result=logger.Start();
						if(Result!=KErrNone)
							INFO_PRINTF2(_L("ulogger server didn't start, with error %d"), Result);
						
						/*****do the tracing**************/
						r = TestKernelTracer.DoTheKernelPrintf(1, usbDevice);
						if(r!=KErrNone)
						{
							INFO_PRINTF2(_L("RemoveStringDescriptor failed with error %d"), r);
						}
						Result=logger.Stop();
						if(Result!=KErrNone)
						{
							INFO_PRINTF2(_L("Call to Ulogger.Stop() has failed with error %d"), Result);
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
				if(Result==0&&TestStepResult()==EPass)
						SetTestStepResult(EPass);
				else
						SetTestStepResult(EFail);
				INFO_PRINTF1(_L("Multiple printf logging to file with kernel side tracing has been tested - check output log. It should contain four lines of tracing of type, (sec,prim), (sec+2, prim), (sec, prim+2), (sec+2, prim+2)"));
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
#else
		INFO_PRINTF1(_L("Kernel tests not run on emulator"));
#endif
	  return TestStepResult();
	}



TVerdict CULoggerAPIKernelPrintfTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("completed kernel printf functionality"));
		return TestStepResult();
	}

