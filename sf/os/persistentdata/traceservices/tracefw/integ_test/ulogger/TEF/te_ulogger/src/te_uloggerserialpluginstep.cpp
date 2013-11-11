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
#include "te_uloggerserialpluginstep.h"
#include "te_uloggermclsuitedefs.h"

using namespace Ulogger;

CULoggerSerialPluginStep::~CULoggerSerialPluginStep()
/**
 * Destructor
 */
	{
	}

CULoggerSerialPluginStep::CULoggerSerialPluginStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerSerialPluginStep);
	}

TVerdict CULoggerSerialPluginStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CULoggerSerialPluginStep"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}


TVerdict CULoggerSerialPluginStep::doTestStepL()
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
			INFO_PRINTF1(_L("About to test serial plugin"));  //Block start
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
			_LIT8(KTextmedia,"uloggerserialplugin");
			const TPtrC8 mediaptr(KTextmedia);
			// test the plugin can be activated
			Result=logger.ActivateOutputPlugin(mediaptr);	
			if(Result==0||Result==-11)	
			{	
				INFO_PRINTF1(_L("Serial Plugin active"));		
				_LIT(KTextsetting, "output_port");	
				_LIT(KTextvalue, "3");
				// test setting the plugin settings
				TPluginConfiguration pluginConfig;
				pluginConfig.SetKey(KTextsetting);
				pluginConfig.SetValue(KTextvalue);			
				Result=logger.SetPluginConfigurations(mediaptr,pluginConfig);
				if(Result==0)	
				{
					INFO_PRINTF1(_L("Serial Plugin Settings set correctly"));
				// test that the plugging settings can be retrieved
				RPointerArray<TPluginConfiguration> getPluginConfigs;
				if(Result==0)	
				{
					INFO_PRINTF1(_L("Serial Plugin Settings got correctly"));
					Result=logger.GetPluginConfigurations(mediaptr, getPluginConfigs);
					_LIT(KTextChecksetting, "output_port");
					_LIT(KTextCheckValue, "3");
					// test that the plugging settings retrieved are correct
					if (!getPluginConfigs[0]->Key().Compare(KTextChecksetting) && !getPluginConfigs[0]->Value().Compare(KTextCheckValue))
					{
						INFO_PRINTF1(_L("Retrieved serial plugin settings are correct"));
					}
					else
					{		
						INFO_PRINTF1(_L("Retrieved serial plugin settings are incorrect"));
						SetTestStepResult(EFail);
					}
					Result=logger.Start();
					INFO_PRINTF2(_L("ulogger start returns error %d"), Result);
#ifndef __WINSCW__				
					CUTraces Testtracer;
					for(TInt i=0; i!=256; i++)
					{
						if(!Testtracer.DoTheTrace(8))
						{
							INFO_PRINTF1(_L("UTrace primary filter writing via serial plugin failed"));
							SetTestStepResult(EFail);
						}
						if(!Testtracer.DoTheTrace(3))
						{
							INFO_PRINTF1(_L("UTrace general data filter writing via serial plugin failed"));
							SetTestStepResult(EFail);
						}
					}
#endif					
					logger.Stop();	
				}
				else
				{
					INFO_PRINTF1(_L("Failed to get serial Plugin Settings"));
					SetTestStepResult(EFail);
				}
				getPluginConfigs.ResetAndDestroy();
			}
			else
			{
				INFO_PRINTF1(_L("Serial Plugin Settings error"));
				SetTestStepResult(EFail);
			}					
		}
		else
		{			
			INFO_PRINTF1(_L("Serial Plugin error when activated"));
			SetTestStepResult(EFail);
		}		
		TBuf8<32>activeplugins;	
		_LIT8(KTextmedia1,"uloggerserialplugin");
		TBuf8<32> checkplugins(KTextmedia1);
		// test the active plugin name can be retrieved
		logger.GetActiveOutputPlugin(activeplugins);
		if (activeplugins.Compare(checkplugins)==0)
			INFO_PRINTF1(_L("Got correct active serial plugin"));
		else
		{
			INFO_PRINTF1(_L("Got incorrect active serial plugin"));
			SetTestStepResult(EFail);
		}
		// test activating the second plugin
		_LIT8(KTextmedia2,"te_fileplugin");
		const TPtrC8 mediaptr2(KTextmedia2);
		Result=logger.ActivateOutputPlugin(mediaptr2);	
		if(Result==0)			
			INFO_PRINTF1(_L("Second plugin activated correctly"));
		else
		{
			INFO_PRINTF1(_L("Second plugin activation error"));
			SetTestStepResult(EFail);
		}
		// test the first plugin can now be removed as it is no longer the active plugin
		Result=logger.RemovePluginConfigurations(mediaptr);
		if(Result==0)
		{	
			INFO_PRINTF1(_L("Removed serial Plugin correctly"));
		}
		else
		{
			INFO_PRINTF1(_L("Failed to remove serial Plugin"));
			SetTestStepResult(EFail);
		}
		logger.Stop();
		logger.Close();
		INFO_PRINTF1(_L("serial plugin tests complete!"));
		
		//  **************   Block end ****************
		}
		
	  	return TestStepResult();
	}



TVerdict CULoggerSerialPluginStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CULoggerSerialPluginStep"));
		return TestStepResult();
	}
