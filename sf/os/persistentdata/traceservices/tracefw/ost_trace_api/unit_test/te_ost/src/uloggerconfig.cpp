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
//

#include "uloggerconfig.h"


CClearConfig::~CClearConfig()
{
	/*Destructor*/
}

CClearConfig::CClearConfig()
{
	/*Constructor*/
}

void CClearConfig::ClearL(RULogger& aLoggerSession)
{	
	aLoggerSession.Stop();
	aLoggerSession.DeActivateInputPlugin();
	CArrayPtrFlat<HBufC8> *allplugins = new (ELeave)CArrayPtrFlat<HBufC8>(1);
	//loggerSession.InstalledOutputPlugins(*allplugins);
	
	aLoggerSession.GetInstalledOutputPlugins(*allplugins);
	for(TInt i=0;i<(allplugins->Count());i++)
	if(allplugins->Count())
	{
		TBuf8<50> dataBuf;
		dataBuf.Copy(allplugins->At(i)->Des());
		aLoggerSession.RemovePluginConfigurations(dataBuf);
	}
	CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
	
	aLoggerSession.GetPrimaryFiltersEnabled(*getfilter);
	
	TInt Result = aLoggerSession.SetPrimaryFiltersEnabled(*getfilter, EFalse);
	RArray<TUint32> get2filter;	
	aLoggerSession.GetSecondaryFiltersEnabled(get2filter);
	aLoggerSession.SetSecondaryFiltersEnabled(get2filter, EFalse);
	//-- aLoggerSession.  EnableClassificationFiltering(); MF - this is the default
	aLoggerSession.SetSecondaryFilteringEnabled(ETrue);
	aLoggerSession.SetBufferSize(1024);
	aLoggerSession.SetNotificationSize(512);
	aLoggerSession.SetBufferMode(1);// MF previously EStraight;
	
}

//NB: currently this method does not set the default values for serial and usb! hanging the server
void  CClearConfig::DefaultL(RULogger& aLoggerSession)
	{
		ClearL(aLoggerSession);
		//Now set default configurations shipped with ulogger
		
		CArrayFixFlat<TUint8>* array = new(ELeave) CArrayFixFlat<TUint8>(4);
		CleanupStack::PushL(array);
		array->AppendL(3);
		array->AppendL(13);
		//set Classification filter
		aLoggerSession.SetPrimaryFiltersEnabled(*array, ETrue);		
		CleanupStack::PopAndDestroy(array);
		
		//set file plugin
		_LIT8(KTextmedia,"uloggerfileplugin");
		const TPtrC8 mediaPtr(KTextmedia);
		_LIT(KTextsetting, "output_path");
		_LIT(KTextvalue, "c:\\logs\\utracedata.utf");
		TPluginConfiguration pluginConfig;
		pluginConfig.SetKey(KTextsetting);
		pluginConfig.SetValue(KTextvalue);			
		aLoggerSession.SetPluginConfigurations(mediaPtr,pluginConfig);
		
		//set serial plugin 
	
		_LIT8(KSerialTextmedia,"uloggerserialplugin");
		const TPtrC8 serialMediaPtr(KSerialTextmedia);
		_LIT(KSerialTextsetting, "output_port");	
		_LIT(KSerialTextvalue, "3");
		TPluginConfiguration serialPluginConfig;
		serialPluginConfig.SetKey(KSerialTextsetting);
		serialPluginConfig.SetValue(KSerialTextvalue);			
		aLoggerSession.SetPluginConfigurations(serialMediaPtr,serialPluginConfig);		
		
		//activate file plugin as active
		aLoggerSession.ActivateOutputPlugin(mediaPtr);				
		
	}

