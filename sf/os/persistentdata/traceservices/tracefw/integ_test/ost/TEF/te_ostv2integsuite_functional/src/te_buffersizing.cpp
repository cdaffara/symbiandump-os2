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
 @file te_buffersizing.cpp
 @internalTechnology
*/
#include "te_buffersizing.h"

#include <e32cons.h>
#include <e32def.h>
#include "te_dataparameters.h"

using namespace Ulogger; //CA:added so can use old ulogger api

CTCMBufferSizingWrapper::~CTCMBufferSizingWrapper()
/**
 * Destructor
 */
	{
	}

CTCMBufferSizingWrapper::CTCMBufferSizingWrapper()
/**
 * Constructor
 */
	{
	}

void CTCMBufferSizingWrapper::writeToBuffer(const TPtrC aLit, TInt aTraceNumber,RULogger* aLogger)
	{
		_LIT8(KTextmedia,"uloggerfileplugin");
		TPtrC8 mediaptr(KTextmedia);
		_LIT(KTextsetting, "output_path");
		// test setting the plugin settings
		TPluginConfiguration setPluginConfigs;
		setPluginConfigs.SetKey(KTextsetting);
		setPluginConfigs.SetValue(aLit);
		aLogger->SetPluginConfigurations( mediaptr, setPluginConfigs);

		TBool OstStatus=0;

#ifdef TE_UPT_TRACE_ENABLED
		OstStatus = 1;
		INFO_PRINTF1(_L("TRACING HAS BEEN ENABLED!"));
#else
		OstStatus = 0;
		INFO_PRINTF1(_L("TRACING HAS BEEN DISABLED! All Log files should be empty/not exist"));
#endif
		if (OstStatus)
			EmptyFile(aLit);

		aLogger->Start();//C.A. previously:aLogger->StartOutputting();
		for(TInt i=0; i!=aTraceNumber; i++)
		{
			TBool ret=1;
			//need to change KPcNotIncluded to KPc so program counter can be included in tests
			TTraceContext attrs(KComponentId+i+1, KGroupId, KContextId, KPcNotIncluded);
			ret = OstTrace(attrs, KInitialClientFormat, KAny16);
			if (ret == 0&&OstStatus==1)
			{
				INFO_PRINTF2(_L("Trace %d not logged"), aTraceNumber);
				SetBlockResult(EFail);
			}
		}
		aLogger->Stop();//C.A. previously:aLogger->StopOutputting();

	}



CTCMBufferSizingWrapper* CTCMBufferSizingWrapper::NewLC()
	{
	CTCMBufferSizingWrapper* self = new (ELeave)CTCMBufferSizingWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMBufferSizingWrapper* CTCMBufferSizingWrapper::NewL()
	{
	CTCMBufferSizingWrapper* self=CTCMBufferSizingWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMBufferSizingWrapper::ConstructL()
	{
	}

TAny* CTCMBufferSizingWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMBufferSizingWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{if (BlockResult()==EPass)
		{
		//AddOutputPlugin
		//  ************** Delete the Block, the block start ****************

			INFO_PRINTF1(_L("About to test buffer sizing"));  //Block start
			RULogger logger;
			TInt result=0;
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
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(KGroupId);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) KComponentId);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);//C.A. previously:logger.EnableClassifications(*setprimfilter);
//			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);//C.A. previously:logger.EnableModuleUids(setsecondfilter);
			SetBlockResult(EPass);
			INFO_PRINTF1(_L("File plugin added"));
			_LIT8(KTextmedia,"uloggerfileplugin");
			TPtrC8 mediaptr(KTextmedia);
			logger.ActivateOutputPlugin(mediaptr);//C.A. previously:logger.SetActiveOutputPlugin(mediaptr);
			// write to an average buffer size with a smaller dns as many bytes
			// as the buffer can hold
			logger.SetBufferSize(1024);
			logger.SetNotificationSize(64);
			_LIT(KTextvalue1, "C:\\logs\\ULoggerBufferAverage.utf");
			const TPtrC valueptr1(KTextvalue1);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),1024,64);

			writeToBuffer(valueptr1, 64, &logger);
			// write to an average buffer size with a smaller dns more bytes
			// than the buffer can hold
			_LIT(KTextvalue2, "C:\\logs\\ULoggerBufferAverageOverflow.utf");
			const TPtrC valueptr2(KTextvalue2);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),1024,64);

			writeToBuffer(valueptr2, 65, &logger);
			// write to a small buffer size with a smaller dns as many bytes
			// as the buffer can hold
			logger.SetBufferSize(64);
			logger.SetNotificationSize(16);
			_LIT(KTextvalue5, "C:\\logs\\ULoggerBufferSmall.utf");
			const TPtrC valueptr5(KTextvalue5);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),64,16);

			writeToBuffer(valueptr5, 4, &logger);
			// write to a small buffer size with a smaller dns more bytes
			// than the buffer can hold
			_LIT(KTextvalue6, "C:\\logs\\ULoggerBufferSmallOverflow.utf");
			const TPtrC valueptr6(KTextvalue6);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),64,16);

			writeToBuffer(valueptr6, 5, &logger);
			// write to a large buffer size with a smaller dns as many bytes
			// as the buffer can hold
			logger.SetBufferSize(65535);
			logger.SetNotificationSize(1024);
			_LIT(KTextvalue9, "C:\\logs\\ULoggerBufferBig.utf");
			const TPtrC valueptr9(KTextvalue9);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),65535,1024);

			writeToBuffer(valueptr9, 4095, &logger);
			// write to a large buffer size with a smaller dns more bytes
			// than the buffer can hold
			_LIT(KTextvalue10, "C:\\logs\\ULoggerBufferBigOverflow.utf");
			const TPtrC valueptr10(KTextvalue10);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),65535,1024);

			writeToBuffer(valueptr10, 4097, &logger);
			// write to a larg buffer size with an equal dns as many bytes
			// as the buffer can hold
			logger.SetNotificationSize(4900);
			logger.SetBufferSize(5000);
			_LIT(KTextvalue11, "C:\\logs\\ULoggerBufferOverflow.utf");
			const TPtrC valueptr11(KTextvalue11);

			INFO_PRINTF3(_L("Writing to buffer... Buffer size: %d, DNS: %d"),4900,5000);

			writeToBuffer(valueptr11, 1000, &logger);
			logger.Close();
			INFO_PRINTF1(_L("Buffer Sizing tests complete!"));

		//  **************   Block end ****************
		}
	  return ETrue;
	}

