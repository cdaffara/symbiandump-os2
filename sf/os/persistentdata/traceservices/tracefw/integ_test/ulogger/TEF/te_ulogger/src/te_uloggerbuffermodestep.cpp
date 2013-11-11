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
 @file te_uloggerbuffermodestep.cpp
 @internalTechnology
*/
#include "te_uloggerbuffermodestep.h"

#include <e32cons.h>
#include <e32def.h>
#include "te_setfilterparameters.h"

using namespace Ulogger;

CULoggerBufferModeStep::~CULoggerBufferModeStep()
/**
 * Destructor
 */
	{
	}

CULoggerBufferModeStep::CULoggerBufferModeStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerBufferModeStep);
	}

TVerdict CULoggerBufferModeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CULoggerBufferModeStep"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}

void CULoggerBufferModeStep::writeToBuffer(const TPtrC iLit, TInt iTraceNumber,RULogger* iLogger)
	{
		_LIT8(KTextmedia,"uloggerfileplugin");
		const TPtrC8 mediaptr(KTextmedia);
		_LIT(KTextsetting, "output_path");
		// test setting the plugin settings
		TPluginConfiguration setPluginConfigs;
		setPluginConfigs.SetKey(KTextsetting);
		setPluginConfigs.SetValue(iLit);			
		iLogger->SetPluginConfigurations(mediaptr, setPluginConfigs);
		CUTraces Testtracer;
		iLogger->Start();		
		TBool ret=0;	
//		for(TInt i=0; i!=iTraceNumber/5; i++)
//			ret = Testtracer.DoTheTrace(0);
//		for(TInt i=0; i!=iTraceNumber/5; i++)
//			ret = Testtracer.DoTheTrace(1);
//		for(TInt i=0; i!=iTraceNumber/5; i++)
//			ret = Testtracer.DoTheTrace(2);
//		for(TInt i=0; i!=iTraceNumber/5; i++)
//			ret = Testtracer.DoTheTrace(3);
//		for(TInt i=0; i!=iTraceNumber/5; i++)
//			ret = Testtracer.DoTheTrace(9);
		for(TInt i=0; i!=iTraceNumber; i++)
		{
		//	ret = Testtracer.DoTheTrace(0);
		//	ret = Testtracer.DoTheTrace(1);
		//	ret = Testtracer.DoTheTrace(2);
			ret = Testtracer.DoTheTrace(3);
		//	ret = Testtracer.DoTheTrace(9);
		}
		if (ret == 0)
		{
			INFO_PRINTF2(_L("Trace %d not carried out"), iTraceNumber);
			SetTestStepResult(EFail);
		}
		ret = TUTrace::Trace((TPrimaryFilter) (192), UTraceSecondary, schema, context, pc);
		if (ret == 0)
		{
			INFO_PRINTF1(_L("Trace 192 not carried out"));
			SetTestStepResult(EFail);
		}
		iLogger->Stop();
	}

TVerdict CULoggerBufferModeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  	if(TestStepResult()==EPass)
       	{
		//  ************** Delete the Block, the block start ****************
		
			TInt Result=0;			
			INFO_PRINTF1(_L("About to test buffer Mode"));  //Block start
			RULogger logger;
			logger.Connect();
			CClearConfig configIni;
			configIni.ClearL(logger);
			TInt iBufferCircular = 0;
			TInt iBufferStraight = 1;
			TInt iGetBuffer = 2;	
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);
			setprimfilter->AppendL((TPrimaryFilter) (192));
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);	
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
			SetTestStepResult(EPass);
			if(Result==0)			
			{
				INFO_PRINTF1(_L("BTrace to File plugin added"));			
				_LIT8(KTextmedia,"uloggerfileplugin");
				const TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);	
			
				// write to a small circular buffer with a tight loop 
				logger.SetBufferSize(4);
				logger.SetNotificationSize(3.9);
				Result=logger.SetBufferMode(iBufferCircular);
				if(Result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));	
					logger.GetBufferMode(iGetBuffer);
					if(iGetBuffer==iBufferCircular)
						INFO_PRINTF1(_L("Circular Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set circular - FAIL"));
						SetTestStepResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), Result);
					SetTestStepResult(EFail);
				}
				_LIT(KTextvalue1, "C:\\logs\\ULoggerBuffer4kDNS4kCircular.log");
				const TPtrC valueptr1(KTextvalue1);
				writeToBuffer(valueptr1, UTrace_Count, &logger);
		
				// write to a small straight buffer with a tight loop 
				Result=logger.SetBufferMode(iBufferStraight);
				if(Result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));	
					logger.GetBufferMode(iGetBuffer);
					if(iGetBuffer==iBufferStraight)
						INFO_PRINTF1(_L("Straight Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set straight - FAIL"));
						SetTestStepResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), Result);
					SetTestStepResult(EFail);
				}			
				_LIT(KTextvalue2, "C:\\logs\\ULoggerBuffer4kDNS4kStraight.log");
				const TPtrC valueptr2(KTextvalue2);
				writeToBuffer(valueptr2, UTrace_Count, &logger);			
			
				// write to a large circular buffer with a tight loop 
				logger.SetBufferSize(100);
				logger.SetNotificationSize(4);
				Result=logger.SetBufferMode(iBufferCircular);
				if(Result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));
					logger.GetBufferMode(iGetBuffer);
					if(iGetBuffer==iBufferCircular)
						INFO_PRINTF1(_L("Circular Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set circular - FAIL"));
						SetTestStepResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), Result);
					SetTestStepResult(EFail);	
				}
				_LIT(KTextvalue5, "C:\\logs\\ULoggerBuffer100kDNS4kCircular.log");
				const TPtrC valueptr5(KTextvalue5);
				writeToBuffer(valueptr5, UTrace_Count, &logger);		
			
				// write to a large straight buffer with a tight loop 
				Result=logger.SetBufferMode(iBufferStraight);
				if(Result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));	
					logger.GetBufferMode(iGetBuffer);
					if(iGetBuffer==iBufferStraight)
						INFO_PRINTF1(_L("Straight Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set Straight - FAIL"));
						SetTestStepResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), Result);
					SetTestStepResult(EFail);	
				}
				_LIT(KTextvalue6, "C:\\logs\\ULoggerBuffer100kDNS4kStraight.log");
				const TPtrC valueptr6(KTextvalue6);
				writeToBuffer(valueptr6, UTrace_Count, &logger);
					
			}	
			else
			{
				INFO_PRINTF1(_L("File Plugin error when added"));	
				SetTestStepResult(EFail);			
			}
			logger.Close();
			INFO_PRINTF1(_L("Buffer Mode tests complete!"));
		
		//  **************   Block end ****************
       	}
  		return TestStepResult();
	}



TVerdict CULoggerBufferModeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CULoggerBufferModeStep"));
		return TestStepResult();
	}
