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
 @file te_uloggerfilepluginstep.cpp
 @internalTechnology
*/
#include "te_uloggerfilepluginstep.h"
#include "te_uloggermclsuitedefs.h"

using namespace Ulogger;

CULoggerFilePluginStep::~CULoggerFilePluginStep()
/**
 * Destructor
 */
	{
	}

CULoggerFilePluginStep::CULoggerFilePluginStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerFilePluginStep);
	}

TVerdict CULoggerFilePluginStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CULoggerFilePluginStep"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}


TVerdict CULoggerFilePluginStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
	  {
		//  ************** Delete the Block, the block start ****************
		TInt Result=0;
		SetTestStepResult(EPass);
		INFO_PRINTF1(_L("About to test file plugin"));  //Block start
		RULogger logger;
		logger.Connect();
		CClearConfig configIni;
		configIni.ClearL(logger);
		_LIT8(KTextmedia,"uloggerfileplugin");
		const TPtrC8 mediaptr(KTextmedia);
		if(Result==0||Result==-11)	
		{	
			INFO_PRINTF1(_L("File Plugin active"));
			_LIT(KTextsetting, "output_path");
			_LIT(KTextvalue, "C:\\logs\\ULoggerFileLoggingTest.log");
			// test setting the plugin settings
			TPluginConfiguration pluginConfig;
			pluginConfig.SetKey(KTextsetting);
			pluginConfig.SetValue(KTextvalue);			
			Result=logger.SetPluginConfigurations(mediaptr,pluginConfig);
			if(Result==0)	
			{
				INFO_PRINTF1(_L("File Plugin Settings set correctly"));
				// test that the plugging settings can be retrieved
				RPointerArray<TPluginConfiguration> getPluginConfigs;
				Result=logger.GetPluginConfigurations(mediaptr, getPluginConfigs);
				if(Result==0)	
				{
					INFO_PRINTF1(_L("File Plugin Settings got correctly"));
				}
				else
				{
					INFO_PRINTF1(_L("Failed to get File Plugin Settings"));
					SetTestStepResult(EFail);
				}
				getPluginConfigs.ResetAndDestroy();
			}
			else
			{
				INFO_PRINTF1(_L("File Plugin Settings error"));
				SetTestStepResult(EFail);
			}
		}
		else
		{
			INFO_PRINTF1(_L("File Plugin error when activated"));
			SetTestStepResult(EFail);
		}		
		TBuf8<32>activeplugins;		
		_LIT8(KTextmedia1,"uloggerfileplugin");
		TBuf8<32> checkplugins(KTextmedia1);
		// test the active plugin name can be retrieved
		logger.GetActiveOutputPlugin(activeplugins);
		if (activeplugins.Compare(checkplugins)==0)
			INFO_PRINTF1(_L("Got correct active file plugin"));
		else
		{
			INFO_PRINTF1(_L("Got incorrect active file plugin"));
			SetTestStepResult(EFail);
		}
		// test activating the second plugin
		_LIT8(KTextmedia2,"te_fileplugin");
		const TPtrC8 mediaptr2(KTextmedia2);
		Result=logger.ActivateOutputPlugin(mediaptr2);	
		if(Result==0||Result==-11)			
			INFO_PRINTF1(_L("Second plugin activated correctly"));
		else
		{
			INFO_PRINTF1(_L("Second plugin activation error"));
			SetTestStepResult(EFail);
		}
		// test the remove plugin settings method
		Result=logger.RemovePluginConfigurations(mediaptr);
		if(Result==0)
		{	
			INFO_PRINTF1(_L("Removed File Plugin correctly"));
		}
		else
		{
			INFO_PRINTF1(_L("Failed to remove File Plugin"));
			SetTestStepResult(EFail);
		}
		// test the second plugin can be deactivated
		// This test should fail because the plugin has no settings to remove
		Result=logger.RemovePluginConfigurations(mediaptr2);	
		if(Result==0)
		{	
			INFO_PRINTF1(_L("Failed as somehow removed settings when there were none."));
			SetTestStepResult(EFail);
		}
		else
		{
			INFO_PRINTF1(_L("Passed no settings to remove"));
		}
		
		//  **************   Block end ****************
	
		logger.Close();
		INFO_PRINTF1(_L("file plugin tests complete!"));
		}
	  return TestStepResult();
	}



TVerdict CULoggerFilePluginStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CULoggerFilePluginStep"));
		return TestStepResult();
	}



