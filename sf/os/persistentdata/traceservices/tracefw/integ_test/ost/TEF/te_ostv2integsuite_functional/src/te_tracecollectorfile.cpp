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
 @file te_tracecollectorfile.cpp
 @internalTechnology
*/
#include "te_tracecollectorfile.h"
#include "te_ostv2integsuite_defs.h"

using namespace Ulogger; //CA:added so can use old ulogger api
CTCMFilePluginWrapper::~CTCMFilePluginWrapper()
/**
 * Destructor
 */
	{
	}

CTCMFilePluginWrapper::CTCMFilePluginWrapper()
/**
 * Constructor
 */
	{
	}

CTCMFilePluginWrapper* CTCMFilePluginWrapper::NewLC()
	{
	CTCMFilePluginWrapper* self = new (ELeave)CTCMFilePluginWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMFilePluginWrapper* CTCMFilePluginWrapper::NewL()
	{
	CTCMFilePluginWrapper* self=CTCMFilePluginWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMFilePluginWrapper::ConstructL()
	{
	}

TAny* CTCMFilePluginWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMFilePluginWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
	  {
		//  ************** Delete the Block, the block start ****************
		TInt result=0;
		SetBlockResult(EPass);
		INFO_PRINTF1(_L("About to test file plugin"));  //Block start
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
		_LIT8(KTextmedia,"uloggerfileplugin");
		const TPtrC8 mediaptr(KTextmedia);
		if(result==0||result==-11)	
		{	
			INFO_PRINTF1(_L("File Plugin active"));
			_LIT(KTextsetting, "output_path");
			_LIT(KTextvalue, "C:\\logs\\ULoggerFileLoggingTest.utf");
			// test setting the plugin settings
			TPluginConfiguration pluginConfig;
			pluginConfig.SetKey(KTextsetting);
			pluginConfig.SetValue(KTextvalue);			
			result=logger.SetPluginConfigurations(mediaptr,pluginConfig);
			if(result==0)	
			{
				INFO_PRINTF1(_L("File Plugin Settings set correctly"));
				// test that the plugging settings can be retrieved
				RPointerArray<TPluginConfiguration> getPluginConfigs;
				result=logger.GetPluginConfigurations(mediaptr, getPluginConfigs);
				if(result==0)	
				{
					INFO_PRINTF1(_L("File Plugin Settings got correctly"));
				}
				else
				{
					INFO_PRINTF1(_L("Failed to get File Plugin Settings"));
					SetBlockResult(EFail);
				}
				getPluginConfigs.ResetAndDestroy();
			}
			else
			{
				INFO_PRINTF1(_L("File Plugin Settings error"));
				SetBlockResult(EFail);
			}
		}
		else
		{
			INFO_PRINTF1(_L("File Plugin error when activated"));
			SetBlockResult(EFail);
		}		
		TBuf8<32>activeplugins;		
		_LIT8(KTextmedia1,"uloggerfileplugin");
		TBuf8<32> checkplugins(KTextmedia1);
		// test the active plugin name can be retrieved
		logger.GetActiveOutputPlugin(activeplugins);//C.A. previously:logger.ActiveOutputPlugin(activeplugins);
		if (activeplugins.Compare(checkplugins)==0)
			INFO_PRINTF1(_L("Got correct active file plugin"));
		else
		{
			INFO_PRINTF1(_L("Got incorrect active file plugin"));
			SetBlockResult(EFail);
		}
		// test activating the second plugin
		_LIT8(KTextmedia2,"te_fileplugin");
		const TPtrC8 mediaptr2(KTextmedia2);
		result=logger.ActivateOutputPlugin(mediaptr2);	//C.A. previously:Result=logger.SetActiveOutputPlugin(mediaptr2);	
		if(result==0||result==-11)			
			INFO_PRINTF1(_L("Second plugin activated correctly"));
		else
		{
			INFO_PRINTF1(_L("Second plugin activation error"));
			SetBlockResult(EFail);
		}
		// test the remove plugin settings method
		result=logger.RemovePluginConfigurations(mediaptr);
		if(result==0)
		{	
			INFO_PRINTF1(_L("Removed File Plugin correctly"));
		}
		else
		{
			INFO_PRINTF1(_L("Failed to remove File Plugin"));
			SetBlockResult(EFail);
		}
		// test the second plugin can be deactivated
		// This test should fail because the plugin has no settings to remove
		result=logger.RemovePluginConfigurations(mediaptr2);	
		if(result==0)
		{	
			INFO_PRINTF1(_L("Failed as somehow removed settings when there were none."));
			SetBlockResult(EFail);
		}
		else
		{
			INFO_PRINTF1(_L("Passed no settings to remove"));
		}
		
		//  **************   Block end ****************
	
		logger.Close();
		INFO_PRINTF1(_L("file plugin tests complete!"));
		}
	  return ETrue;
	}



