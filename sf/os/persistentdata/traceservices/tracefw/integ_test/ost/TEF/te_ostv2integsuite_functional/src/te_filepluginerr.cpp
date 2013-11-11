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

#include "te_filepluginerr.h"
#include "te_tracecollectorfile.h"
#include "te_ostv2integsuite_defs.h"
#include "te_dataparameters.h"
using namespace Ulogger; //CA:added so can use old ulogger api

///negative testing
CTCMFilePluginErrWrapper::~CTCMFilePluginErrWrapper()
{
}

CTCMFilePluginErrWrapper::CTCMFilePluginErrWrapper()
{
}


CTCMFilePluginErrWrapper* CTCMFilePluginErrWrapper::NewLC()
	{
	CTCMFilePluginErrWrapper* self = new (ELeave)CTCMFilePluginErrWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTCMFilePluginErrWrapper* CTCMFilePluginErrWrapper::NewL()
	{
	CTCMFilePluginErrWrapper* self=CTCMFilePluginErrWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTCMFilePluginErrWrapper::ConstructL()
	{
	}

TAny* CTCMFilePluginErrWrapper::GetObject()
	{
	return NULL;
	}

TBool CTCMFilePluginErrWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	  if (BlockResult()==EPass)
		  {
		  TInt result = 0;
		  INFO_PRINTF1(_L("Testing..."));
		  RULogger logger;
		  result =logger.Connect();
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
		  _LIT8(KTextmedia,"te_fileplugin");
		  const TPtrC8 mediaptr(KTextmedia);
		  INFO_PRINTF1(_L("Setting test plugin as active..."));
		  result = logger.ActivateOutputPlugin(mediaptr);//C.A. previously:result = logger.SetActiveOutputPlugin(mediaptr);
		  if(result != KErrNone && result != KErrAlreadyExists)
			  {
			  INFO_PRINTF1(_L("ERROR: Setting test plugin as active error."));
			  SetBlockResult(EFail);
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
		  		SetBlockResult(EFail);
		  		}
		  	_LIT(KTextsetting2, "output_path");
		  	_LIT(KTextvalue2, "C:\\logs\\ULoggerFileLoggingTest.utf");
		  	// test setting the plugin settings
		  	TPluginConfiguration pluginConfig2;
		  	pluginConfig2.SetKey(KTextsetting2);
		  	pluginConfig2.SetValue(KTextvalue2);			
		  	result=logger.SetPluginConfigurations(mediaptr,pluginConfig2);
		  	if(result!=KErrNone)	
		  		{
		  		INFO_PRINTF1(_L("ERROR: Setting test plugin configuration (path) error."));
		  		SetBlockResult(EFail);
		  		}	  	
		  	
		  	INFO_PRINTF1(_L("setting filters..."));
		  	CArrayFixFlat<TUint8> *a = new (ELeave) CArrayFixFlat<TUint8>(1);
		  	a->AppendL(KGroupId);
		  	result=logger.SetPrimaryFiltersEnabled(*a,ETrue);//C.A. previously:result=logger.EnableClassifications(*a);
		  	//C.A. already done by default, but previously:result=logger.EnableClassificationFiltering();
		  	//result=logger.DisableModuleUidFiltering(); ;//as only setting primary filters- we do not care about secondary
		  	result=logger.SetNotificationSize(0);
		  	result=logger.SetBufferSize(1020);
		  	delete a;
		  	
		  	INFO_PRINTF1(_L("starting ulogger with primary filters only configured..."));
		  	result = logger.Stop();//C.A. previously:result = logger.StopOutputting();
		  	result = logger.Start();//C.A. previously:result = logger.StartOutputting();
		  	if(result != KErrNone)
		  		{
		  		INFO_PRINTF2(_L("ERROR: Starting Ulogger error: %d"), result);
		  		SetBlockResult(EFail);
		  		}
		  	User::After(1*1000*1000);
		  	
		  	INFO_PRINTF1(_L("stopping ulogger..."));
		  	result = logger.Stop();//C.A. previously:result = logger.StopOutputting();

		  	if(result != KErrNone)
		  		{
		  		INFO_PRINTF2(_L("ERROR: Stopping error %d (server probably panicked)"), result);
		  		SetBlockResult(EFail);
		  		}
		  }
		  else
			  {
			  INFO_PRINTF1(_L("ERROR: Test plugin not found."));
		  	  SetBlockResult(EFail);
			  }
		  }
	return ETrue;
	}


