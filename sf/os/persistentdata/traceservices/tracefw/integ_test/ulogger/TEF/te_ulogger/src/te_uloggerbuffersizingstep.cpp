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
 @file te_uloggerbuffersizingstep.cpp
 @internalTechnology
*/
#include "te_uloggerbuffersizingstep.h"

#include <e32cons.h>
#include <e32def.h>
#include "te_setfilterparameters.h"

using namespace Ulogger;

CULoggerBufferSizingStep::~CULoggerBufferSizingStep()
/**
 * Destructor
 */
	{
	}

CULoggerBufferSizingStep::CULoggerBufferSizingStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerBufferSizingStep);
	}

TVerdict CULoggerBufferSizingStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CULoggerBufferSizingStep"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}

void CULoggerBufferSizingStep::writeToBuffer(const TPtrC iLit, TInt iTraceNumber,RULogger* iLogger)
	{
		_LIT8(KTextmedia,"uloggerfileplugin");
		TPtrC8 mediaptr(KTextmedia);
		_LIT(KTextsetting, "output_path");
		// test setting the plugin settings
		TPluginConfiguration setPluginConfigs;
		setPluginConfigs.SetKey(KTextsetting);
		setPluginConfigs.SetValue(iLit);			
		iLogger->SetPluginConfigurations( mediaptr, setPluginConfigs);
		CUTraces Testtracer;
		iLogger->Start();		
		for(TInt i=0; i!=iTraceNumber; i++)
		{
			TBool ret;
			if (iTraceNumber==100)
				ret = Testtracer.DoTheTrace(7);
			else
				ret = Testtracer.DoTheTrace(8);
			if (ret == 0)
			{
				INFO_PRINTF2(_L("Trace %d not logged"), iTraceNumber);
				SetTestStepResult(EFail);
			}
		}
		iLogger->Stop();
	}

TVerdict CULoggerBufferSizingStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	    if (TestStepResult()==EPass)
		{
		//AddOutputPlugin
		//  ************** Delete the Block, the block start ****************
				
			INFO_PRINTF1(_L("About to test buffer sizing"));  //Block start
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
			SetTestStepResult(EPass);
			INFO_PRINTF1(_L("File plugin added"));
			_LIT8(KTextmedia,"uloggerfileplugin");
			TPtrC8 mediaptr(KTextmedia);
		logger.ActivateOutputPlugin(mediaptr);	
			// write to an average buffer size with a smaller dns as many bytes
			// as the buffer can hold		
			logger.SetBufferSize(1024);
			logger.SetNotificationSize(64);		
			_LIT(KTextvalue1, "C:\\logs\\ULoggerBufferAverage.log");
			const TPtrC valueptr1(KTextvalue1);
			writeToBuffer(valueptr1, 64, &logger);
			// write to an average buffer size with a smaller dns more bytes
			// than the buffer can hold
			_LIT(KTextvalue2, "C:\\logs\\ULoggerBufferAverageOverflow.log");
			const TPtrC valueptr2(KTextvalue2);
			writeToBuffer(valueptr2, 65, &logger);
			// write to a small buffer size with a smaller dns as many bytes
			// as the buffer can hold
			logger.SetBufferSize(64);
			logger.SetNotificationSize(16);
			_LIT(KTextvalue5, "C:\\logs\\ULoggerBufferSmall.log");
			const TPtrC valueptr5(KTextvalue5);
			writeToBuffer(valueptr5, 4, &logger);
			// write to a small buffer size with a smaller dns more bytes
			// than the buffer can hold
			_LIT(KTextvalue6, "C:\\logs\\ULoggerBufferSmallOverflow.log");
			const TPtrC valueptr6(KTextvalue6);
			writeToBuffer(valueptr6, 5, &logger);		
			// write to a large buffer size with a smaller dns as many bytes
			// as the buffer can hold
			logger.SetBufferSize(65535);
			logger.SetNotificationSize(1024);
			_LIT(KTextvalue9, "C:\\logs\\ULoggerBufferBig.log");
			const TPtrC valueptr9(KTextvalue9);
			writeToBuffer(valueptr9, 4095, &logger);
			// write to a large buffer size with a smaller dns more bytes
			// than the buffer can hold		
			_LIT(KTextvalue10, "C:\\logs\\ULoggerBufferBigOverflow.log");
			const TPtrC valueptr10(KTextvalue10);
			writeToBuffer(valueptr10, 4097, &logger);
			// write to a larg buffer size with an equal dns as many bytes
			// as the buffer can hold		
			logger.SetNotificationSize(4900);
			logger.SetBufferSize(5000);	
			_LIT(KTextvalue11, "C:\\logs\\ULoggerBufferOverflow.log");
			const TPtrC valueptr11(KTextvalue11);
			writeToBuffer(valueptr11, 1000, &logger);			
			logger.Close();
			INFO_PRINTF1(_L("Buffer Sizing tests complete!"));
		
		//  **************   Block end ****************
		}
	  return TestStepResult();
	}



TVerdict CULoggerBufferSizingStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CULoggerBufferSizingStep"));
		return TestStepResult();
	}
