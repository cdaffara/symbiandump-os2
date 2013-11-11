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
 @file te_uloggerconfigfilestep.cpp
 @internalTechnology
*/
#include "te_uloggerconfigfilestep.h"
#include "te_uloggermclsuitedefs.h"



using namespace Ulogger;

/*******************************************************************************/
/*********************************Public File Test***************************/
/*******************************************************************************/

TInt CULoggerPublicConfigFileTestStep::aTestCount=0;

CULoggerPublicConfigFileTestStep::~CULoggerPublicConfigFileTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerPublicConfigFileTestStep::CULoggerPublicConfigFileTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerPublicConfigFileTestStep);
	aTestCount++;
	}

TVerdict CULoggerPublicConfigFileTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger public config file functionality"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CULoggerPublicConfigFileTestStep::doTestStepL()
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
		Result=logger.Connect();
		INFO_PRINTF2(_L("connect err %d"), Result);
	
		/**************First get primary filter*************/

		CArrayFixFlat<TUint8> *getprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);				
		logger.GetPrimaryFiltersEnabled(*getprimfilter);
		if(getprimfilter->At(0)==(TPrimaryFilter)255&&getprimfilter->Count()==1)
		{
			INFO_PRINTF1(_L("Get primary filter successful, correctly picked up from public file with correct count"));	
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF1(_L("Get primary filter failed, returned filter(s):"));
			for(TInt i=0;i!=getprimfilter->Count();i++)
				{
				INFO_PRINTF2(_L("%d "),getprimfilter->At(i));
				}
			SetTestStepResult(EFail);
		}
		/**************Now get secondary filter, if this has been successful*************/
	
		if(TestStepResult()==EPass)
		{
			RArray<TUint32> getsecondfilter;
			logger.GetSecondaryFiltersEnabled(getsecondfilter);	
			if(getsecondfilter[0]==(TSecondaryFilter)333&&getsecondfilter.Count()==1)
			{
				INFO_PRINTF1(_L("Get Secondary filter successful, correctly picked up from public file with correct count"));	
				SetTestStepResult(EPass);

			}
			else
			{
				INFO_PRINTF1(_L("Get Secondary filter failed, returned filter(s):"));
				for(TInt i=0;i!=getsecondfilter.Count();i++)
				{
					INFO_PRINTF2(_L("%d "),getsecondfilter[i]);
				}
				SetTestStepResult(EFail);
			}
			/**************Now do some tracing, if this has been successful*************/

			if(TestStepResult()==EPass)
			{
				logger.Start();
				TUTrace configtrace((TPrimaryFilter)255, (TSecondaryFilter)333, schema, context, pc);
				Result = configtrace.Trace();
				logger.Stop();
				if(Result==1)
				{
					INFO_PRINTF2(_L("Tracing successful, check outputlog PublicConfigULogger%d.log has one trace statement, primary filter=255 and secondary filter=333"), aTestCount);	
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF2(_L("Tracing failed, with error return %d"), Result);	
					SetTestStepResult(EFail);
				}
			}
		
		}
		//  **************   Block end ****************
		if(getprimfilter)
			delete getprimfilter;
		logger.Close();
	}
	  return TestStepResult();
	}



TVerdict CULoggerPublicConfigFileTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Completed Public Config File test"));
	return TestStepResult();
	}



/*******************************************************************************/
/*********************************Private File Test***************************/
/*******************************************************************************/


CULoggerPrivateConfigFileTestStep::~CULoggerPrivateConfigFileTestStep()
/**
 * Destructor
 */
	{
	}

CULoggerPrivateConfigFileTestStep::CULoggerPrivateConfigFileTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KULoggerPrivateConfigFileTestStep);
	}

TVerdict CULoggerPrivateConfigFileTestStep::doTestStepPreambleL()
/** * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Testing ULogger Private config file functionality"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CULoggerPrivateConfigFileTestStep::doTestStepL()
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
		Result=logger.Connect();
		INFO_PRINTF2(_L("connect err %d"), Result);
	
		/**************First get primary filter*************/

		CArrayFixFlat<TUint8> *getprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);				
		logger.GetPrimaryFiltersEnabled(*getprimfilter);
		if(getprimfilter->At(0)==3&&getprimfilter->Count()==1)
		{
			INFO_PRINTF1(_L("Get primary filter successful, correctly picked up from private file with correct count"));	
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF1(_L("Get primary filter failed, returned filter(s):"));
			for(TInt i=0;i!=getprimfilter->Count();i++)
				{
				INFO_PRINTF2(_L("%d "),getprimfilter->At(i));
				}
			SetTestStepResult(EFail);
		}
		/**************Now get secondary filter, if this has been successful*************/
	
		if(TestStepResult()==EPass)
		{
			RArray<TUint32> getsecondfilter;
			Result=logger.GetSecondaryFiltersEnabled(getsecondfilter);	
			if(Result==KErrNone&&getsecondfilter.Count()==0) //i.e. return empty array
			{
				INFO_PRINTF1(_L("Get Secondary filter successful, correctly picked up that there were no secondary filters from private config file"));	
				SetTestStepResult(EPass);

			}
			else
			{
				INFO_PRINTF1(_L("Get Secondary filter failed, returned filter(s):"));
				for(TInt i=0;i!=getsecondfilter.Count();i++)
				{
					INFO_PRINTF2(_L("%d "),getsecondfilter[i]);
				}
				SetTestStepResult(EFail);
			}
			/**************Now do some tracing, if this has been successful*************/

			if(TestStepResult()==EPass)
			{
				logger.Start();
				TUTrace configtrace((TUint8)3, schema, context, pc);
				Result = configtrace.Trace();
				logger.Stop();
				if(Result==1)
				{
					INFO_PRINTF1(_L("Tracing successful, check outputlog PrivateConfigULogger.log has trace statements only of order primary filter=3"));	
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF2(_L("Tracing failed, with error return %d"), Result);	
					SetTestStepResult(EFail);
				}
			}
		
		}
		//  **************   Block end ****************
		if(getprimfilter)
			delete getprimfilter;
		logger.Close();
	}
	  return TestStepResult();
	}



TVerdict CULoggerPrivateConfigFileTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Completed Private Config File test"));
	return TestStepResult();
	}

