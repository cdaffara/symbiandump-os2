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
 @file te_uloggerserialpluginstep.cpp
 @internalTechnology
*/

#include "te_uloggerfilepluginsteperr.h"
#include "te_uloggerfilepluginstep.h"
#include "te_uloggermclsuitedefs.h"

///negative testing
CULoggerFilePluginStepErr::~CULoggerFilePluginStepErr()
{
}

CULoggerFilePluginStepErr::CULoggerFilePluginStepErr()
{
	SetTestStepName(KULoggerFilePluginStepErr);
}

TVerdict CULoggerFilePluginStepErr::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Negative testing of ulogger - returning err code from a plugin."));
	return TestStepResult();
	}

TVerdict CULoggerFilePluginStepErr::doTestStepL()
	{
	  if (TestStepResult()==EPass)
		  {
		  TInt result = 0;
		  INFO_PRINTF1(_L("Testing..."));
		  RULogger logger;
		  logger.Connect();
		  CClearConfig configIni;
		  configIni.ClearL(logger);
		  _LIT8(KTextmedia,"te_fileplugin");
		  const TPtrC8 mediaptr(KTextmedia);
		  INFO_PRINTF1(_L("Setting test plugin as active..."));
		  result = logger.ActivateOutputPlugin(mediaptr);
		  if(result != KErrNone && result != KErrAlreadyExists)
			  {
			  INFO_PRINTF1(_L("ERROR: Setting test plugin as active error."));
			  SetTestStepResult(EFail);
			  }
		  
		  if(result==0||result==-11)	
		  	{	
		  	INFO_PRINTF1(_L("Tesr File Plugin active"));
		  	_LIT(KTextsetting, "ErrorCode");
		  	_LIT(KTextvalue, "10");
		  	// test setting the plugin settings
		  	TPluginConfiguration pluginConfig;
		  	pluginConfig.SetKey(KTextsetting);
		  	pluginConfig.SetValue(KTextvalue);			
		  	result=logger.SetPluginConfigurations(mediaptr,pluginConfig);
		  	if(result!=KErrNone)	
		  		{
		  		INFO_PRINTF1(_L("ERROR: Setting test plugin configuration error."));
		  		SetTestStepResult(EFail);
		  		}
		  	_LIT(KTextsetting2, "output_path");
		  	_LIT(KTextvalue2, "C:\\logs\\ULoggerFileLoggingTest.log");
		  	// test setting the plugin settings
		  	TPluginConfiguration pluginConfig2;
		  	pluginConfig2.SetKey(KTextsetting2);
		  	pluginConfig2.SetValue(KTextvalue2);			
		  	result=logger.SetPluginConfigurations(mediaptr,pluginConfig2);
		  	if(result!=KErrNone)	
		  		{
		  		INFO_PRINTF1(_L("ERROR: Setting test plugin configuration (path) error."));
		  		SetTestStepResult(EFail);
		  		}	  	
		  	
		  	INFO_PRINTF1(_L("setting filters..."));
		  	CArrayFixFlat<TUint8>*a = new (ELeave) CArrayFixFlat<TUint8>(1);
		  	a->AppendL(4);
		  	logger.SetPrimaryFiltersEnabled(*a, ETrue);
		  	logger.SetNotificationSize(0);
		  	logger.SetBufferSize(1020);
		  	delete a;
		  	
		  	INFO_PRINTF1(_L("starting ulogger..."));
		  	logger.Stop();
		  	result = logger.Start();
		  	if(result != KErrNone)
		  		{
		  		INFO_PRINTF2(_L("ERROR: Starting Ulogger error: %d"), result);
		  		SetTestStepResult(EFail);
		  		}
		  	User::After(1*1000*1000);
		  	
		  	INFO_PRINTF1(_L("stopping ulogger..."));
		  	result = logger.Stop();
		  	if(result != KErrNone)
		  		{
		  		INFO_PRINTF2(_L("ERROR: Stopping error %d (server probably panicked)"), result);
		  		SetTestStepResult(EFail);
		  		}
		  }
		  else
			  {
			  INFO_PRINTF1(_L("ERROR: Test plugin not found."));
		  	  SetTestStepResult(EFail);
			  }
		  }
	return TestStepResult();
	}

TVerdict CULoggerFilePluginStepErr::doTestStepPostambleL()
	{
		INFO_PRINTF1(_L("Negative testing of ulogger (err code from plugin) passed"));
		return TestStepResult();
	}

