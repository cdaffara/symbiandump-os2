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
 @file te_tracecollectorserial.cpp
 @internalTechnology
*/
#include "te_tracecollectorserial.h"
#include "te_ostv2integsuite_defs.h"

using namespace Ulogger; //CA:added so can use old ulogger api
CTCMSerialPluginWrapper::~CTCMSerialPluginWrapper()
/**
 * Destructor
 */
	{
	}

CTCMSerialPluginWrapper::CTCMSerialPluginWrapper()
/**
 * Constructor
 */
	{
	}

CTCMSerialPluginWrapper* CTCMSerialPluginWrapper::NewLC()
	{
	CTCMSerialPluginWrapper* self = new (ELeave)CTCMSerialPluginWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMSerialPluginWrapper* CTCMSerialPluginWrapper::NewL()
	{
	CTCMSerialPluginWrapper* self=CTCMSerialPluginWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMSerialPluginWrapper::ConstructL()
	{
	}

TAny* CTCMSerialPluginWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMSerialPluginWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	    if (BlockResult()==EPass)
		{		
		//  ************** Delete the Block, the block start ****************
		
			TInt Result=0;
			SetBlockResult(EPass);
			INFO_PRINTF1(_L("About to test serial plugin"));  //Block start
			RULogger logger;
			Result = logger.Connect();
			INFO_PRINTF2(_L("Connection to Ulogger: %d, attempt: 1"), Result);
                if (Result!=KErrNone)
                {
                INFO_PRINTF1(_L("Connection to Ulogger failed. Will try to connect max. 10 times"));
                for (TInt i=2; i<12;i++)
                   {
                   User::After(1000);
                   Result = logger.Connect();
                   INFO_PRINTF3(_L("Connection to Ulogger: %d, attempt: %d"), Result, i);
                   if (Result==KErrNone)
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
			_LIT8(KTextmedia,"uloggerserialplugin");
			const TPtrC8 mediaptr(KTextmedia);
			// test the plugin can be activated
			Result=logger.ActivateOutputPlugin(mediaptr);//C.A. previously:Result=logger.SetActiveOutputPlugin(mediaptr);	
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
				if(Result==KErrNone)	
				{
					INFO_PRINTF1(_L("Setting Serial Plugin Settings returns KErrNone but verify set correctly in next step"));
				// test that the plugging settings can be retrieved
				RPointerArray<TPluginConfiguration> getPluginConfigs;
				if(Result==KErrNone)	
				{
					INFO_PRINTF1(_L("Getting Serial Plugin Settings returns KErrNone but verify got correctly in next step"));
					Result=logger.GetPluginConfigurations(mediaptr, getPluginConfigs);
					_LIT(KTextChecksetting, "output_port");
					_LIT(KTextCheckValue, "3");
					// test that the plugging settings retrieved are correct
					if(getPluginConfigs.Count()>0)
						{
						if (!getPluginConfigs[0]->Key().Compare(KTextChecksetting) && !getPluginConfigs[0]->Value().Compare(KTextCheckValue))
							{
							INFO_PRINTF1(_L("Retrieved serial plugin settings are correct"));
							}
							else
								{		
								INFO_PRINTF1(_L("Retrieved serial plugin settings are incorrect"));
								SetBlockResult(EFail);
								}
							Result=logger.Start();//C.A. previously:Result=logger.StartOutputting();
							INFO_PRINTF2(_L("ulogger start returns error %d"), Result);
							TBool OstStatus=0;
#ifndef __WINSCW__	
#ifdef TE_UPT_TRACE_ENABLED
OstStatus = 1;
INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
OstStatus = 0;
INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif // trace enabled
							TTraceContext attrs(KComponentId, KGroupId, KContextId, KPc);						
							if(!OstTrace(attrs, KInitialClientFormat, KAny16)&&OstStatus)
								{
								INFO_PRINTF1(_L("Ost first trace via serial plugin failed"));
								SetBlockResult(EFail);
								}
							TTraceContext attrs1(KComponentId, KGroupId, KContextId, KPc);
							if(!OstTrace(attrs1, KInitialClientFormat, KAny16)&&OstStatus)
								{
								INFO_PRINTF1(_L("Ost second trace writing via serial plugin failed"));
								SetBlockResult(EFail);
								}
#endif	//winscw			
							logger.Stop();//C.A. previously:logger.StopOutputting();	
						}
					else
						{		
						INFO_PRINTF1(_L("Either SetPluginConfiguration failed or GetPluginConfiguration failed as returned a zero size array"));
						SetBlockResult(EFail);
						}
				}
				else
				{
					INFO_PRINTF1(_L("Failed to get serial Plugin Settings"));
					SetBlockResult(EFail);
				}
				getPluginConfigs.ResetAndDestroy();
			}
			else
			{
				INFO_PRINTF1(_L("Serial Plugin Settings error"));
				SetBlockResult(EFail);
			}					
		}
		else
		{			
			INFO_PRINTF1(_L("Serial Plugin error when activated"));
			SetBlockResult(EFail);
		}		
		TBuf8<32>activeplugins;	
		_LIT8(KTextmedia1,"uloggerserialplugin");
		TBuf8<32> checkplugins(KTextmedia1);
		// test the active plugin name can be retrieved
		logger.GetActiveOutputPlugin(activeplugins);//C.A. previously:logger.ActiveOutputPlugin(activeplugins);
		if (activeplugins.Compare(checkplugins)==0)
			INFO_PRINTF1(_L("Got correct active serial plugin"));
		else
		{
			INFO_PRINTF1(_L("Got incorrect active serial plugin"));
			SetBlockResult(EFail);
		}
		// test activating the second plugin
		_LIT8(KTextmedia2,"te_fileplugin");
		const TPtrC8 mediaptr2(KTextmedia2);
		Result=logger.ActivateOutputPlugin(mediaptr2);	//C.A. previously:Result=logger.SetActiveOutputPlugin(mediaptr2);	
		if(Result==0)			
			INFO_PRINTF1(_L("Second plugin activated correctly"));
		else
		{
			INFO_PRINTF1(_L("Second plugin activation error"));
			SetBlockResult(EFail);
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
			SetBlockResult(EFail);
		}
		logger.Stop();//C.A. previously:logger.StopOutputting();
		logger.Close();
		INFO_PRINTF1(_L("serial plugin tests complete!"));
		
		//  **************   Block end ****************
		}
		
	  	return ETrue;
	}

