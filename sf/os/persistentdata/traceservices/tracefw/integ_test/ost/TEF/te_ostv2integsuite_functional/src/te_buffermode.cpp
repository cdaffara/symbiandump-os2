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
 @file te_buffermode.cpp
 @internalTechnology
*/
#include "te_buffermode.h"

#include <e32cons.h>
#include <e32def.h>
#include "te_dataparameters.h"
#include "te_ostv2integsuite_defs.h"

using namespace Ulogger; //CA:added so can use old ulogger api
CTCMBufferModeWrapper::~CTCMBufferModeWrapper()
/**
 * Destructor
 */
	{
	}

CTCMBufferModeWrapper::CTCMBufferModeWrapper()
/**
 * Constructor
 */
	{
	}

void CTCMBufferModeWrapper::writeToBuffer(const TPtrC aFileName, TInt aTraceNumber,RULogger* aLogger)
	{
		_LIT8(KTextmedia,"uloggerfileplugin");
		const TPtrC8 mediaptr(KTextmedia);
		_LIT(KTextsetting, "output_path");
		// test setting the plugin settings
		TPluginConfiguration setPluginConfigs;
		setPluginConfigs.SetKey(KTextsetting);
		setPluginConfigs.SetValue(aFileName);
		aLogger->SetPluginConfigurations(mediaptr, setPluginConfigs);
		//empty out logfile before writing to it.
#ifdef TE_UPT_TRACE_ENABLED
		EmptyFile(aFileName);
#endif
		aLogger->Start();	//C.A. previously:aLogger->StartOutputting();
		TBool ret=0;
		TBool OstStatus=0;

#ifdef TE_UPT_TRACE_ENABLED
		OstStatus = 1;
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		OstStatus = 0;
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif

		for(TInt i=0; i!=aTraceNumber; i++)
			{
			TTraceContext attrs(KComponentId+i+1, KGroupId, KContextId, KPc);
			ret = OstTrace(attrs, KInitialClientFormat, KAny16);
			if (ret == 0 && OstStatus == 1)
			{
				INFO_PRINTF2(_L("Trace %d not carried out"), aTraceNumber);
				SetBlockResult(EFail);
			}

			else if (ret == 1 && OstStatus == 0)
				SetBlockResult(EFail);
			}

		TTraceContext attrs1(KComponentId, 192, KContextId, KPc);
		ret = OstTrace(attrs1, KInitialClientFormat, KAny16);

		if (ret == 0 && OstStatus == 1)
		{
			INFO_PRINTF2(_L("Trace %d not carried out"), aTraceNumber);
			SetBlockResult(EFail);
		}

		else if (ret == 1 && OstStatus == 0)
			SetBlockResult(EFail);

		if (ret == 0 && OstStatus == 1)
		{
			INFO_PRINTF1(_L("Trace 192 not carried out"));
			SetBlockResult(EFail);
		}
		aLogger->Stop();

	}


CTCMBufferModeWrapper* CTCMBufferModeWrapper::NewLC()
	{
	CTCMBufferModeWrapper* self = new (ELeave)CTCMBufferModeWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMBufferModeWrapper* CTCMBufferModeWrapper::NewL()
	{
	CTCMBufferModeWrapper* self=CTCMBufferModeWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMBufferModeWrapper::ConstructL()
	{
	}

TAny* CTCMBufferModeWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMBufferModeWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	if(BlockResult()==EPass)
       	{
       	//  ************** Delete the Block, the block start ****************
			const TInt ECircular=0;//C.A. added because TBuffermode enum doesnt exist in old ulogger api:
			const TInt EStraight=1;//C.A. as above:
			TInt result=0;
			INFO_PRINTF1(_L("About to test buffer Mode"));  //Block start
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
			logger.SetSecondaryFilteringEnabled(EFalse);//C.A. previously:logger.DisableModuleUidFiltering();
//			C.A. previously:
/*			TBufferMode bufferCircular = ECircular;
			TBufferMode bufferStraight = EStraight;
			TBufferMode getBuffer = ECircular;	*/
			TInt bufferCircular = ECircular;
			TInt bufferStraight = EStraight;
			TInt getBuffer = ECircular;
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(KGroupId);
			setprimfilter->AppendL((TGroupId) (192));
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) KComponentId);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setprimfilter);
			//logger.EnableModuleUids(setsecondfilter);
			SetBlockResult(EPass);
			if(result==0)
			{
				INFO_PRINTF1(_L("BTrace to File plugin added"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				const TPtrC8 mediaptr(KTextmedia);
				result=logger.ActivateOutputPlugin(mediaptr);//C.A. previously:Result=logger.SetActiveOutputPlugin(mediaptr);

				// write to a small circular buffer with a tight loop
				logger.SetBufferSize(4);
				logger.SetNotificationSize(3.9);
				result=logger.SetBufferMode(bufferCircular);
				if(result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));
					logger.GetBufferMode(getBuffer);
					if(getBuffer==bufferCircular)
						INFO_PRINTF1(_L("Circular Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set circular - FAIL"));
						SetBlockResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), result);
					SetBlockResult(EFail);
				}
				_LIT(KTextvalue1, "C:\\logs\\ULoggerBuffer4kDNS4kCircular.utf");
				const TPtrC valueptr1(KTextvalue1);
				INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),4,4);
				writeToBuffer(valueptr1, Ost_Count, &logger);

				// write to a small straight buffer with a tight loop
				result=logger.SetBufferMode(bufferStraight);
				if(result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));
					logger.GetBufferMode(getBuffer);
					if(getBuffer==bufferStraight)
						INFO_PRINTF1(_L("Straight Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set straight - FAIL"));
						SetBlockResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), result);
					SetBlockResult(EFail);
				}
				_LIT(KTextvalue2, "C:\\logs\\ULoggerBuffer4kDNS4kStraight.utf");
				const TPtrC valueptr2(KTextvalue2);
				INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),4,4);
				writeToBuffer(valueptr2, Ost_Count, &logger);

				// write to a large circular buffer with a tight loop
				logger.SetBufferSize(100);
				logger.SetNotificationSize(4);
				result=logger.SetBufferMode(bufferCircular);
				if(result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));
					logger.GetBufferMode(getBuffer);
					if(getBuffer==bufferCircular)
						INFO_PRINTF1(_L("Circular Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set circular - FAIL"));
						SetBlockResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), result);
					SetBlockResult(EFail);
				}
				_LIT(KTextvalue5, "C:\\logs\\ULoggerBuffer100kDNS4kCircular.utf");
				const TPtrC valueptr5(KTextvalue5);
				INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),100,4);
				writeToBuffer(valueptr5, Ost_Count, &logger);

				// write to a large straight buffer with a tight loop
				result=logger.SetBufferMode(bufferStraight);
				if(result==KErrNone)
				{
					INFO_PRINTF1(_L("Buffer mode set without error"));
					logger.GetBufferMode(getBuffer);
					if(getBuffer==bufferStraight)
						INFO_PRINTF1(_L("Straight Buffer mode got correctly"));
					else
					{
						INFO_PRINTF1(_L("get buffer mode different from set Straight - FAIL"));
						SetBlockResult(EFail);
					}
				}
				else
				{
					INFO_PRINTF2(_L("Buffer mode set failed with error %d"), result);
					SetBlockResult(EFail);
				}
				_LIT(KTextvalue6, "C:\\logs\\ULoggerBuffer100kDNS4kStraight.utf");
				const TPtrC valueptr6(KTextvalue6);
				INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),100,4);
				writeToBuffer(valueptr6, Ost_Count, &logger);

			}
			else
			{
				INFO_PRINTF1(_L("File Plugin error when added"));
				SetBlockResult(EFail);
			}
			logger.Close();
			INFO_PRINTF1(_L("Buffer Mode tests complete!"));

		//  **************   Block end ****************
       	}
  		return ETrue;
	}
