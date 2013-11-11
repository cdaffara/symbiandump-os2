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



/**
 @file te_uloggerostv2integsuite_defs.cpp
 @internalTechnology
*/

#include <e32cons.h>
#include <e32def.h>
#include "te_ostv2integsuite_defs.h"

//C.A. previously:using namespace UTF;
using namespace Ulogger; //CA:added so can use old ulogger api
CClearConfig::~CClearConfig()
{
	/*Destructor*/
}

CClearConfig::CClearConfig()
{
	/*Constructor*/
}

void CClearConfig::ClearL(RULogger& loggerSession)
{	
	loggerSession.Stop();//C.A. previously:loggerSession.StopOutputting();
	loggerSession.DeActivateInputPlugin();
	CArrayPtrFlat<HBufC8> *allplugins = new (ELeave)CArrayPtrFlat<HBufC8>(1);
	loggerSession.GetInstalledOutputPlugins(*allplugins);//C.A. previously:loggerSession.InstalledOutputPlugins(*allplugins);
	for(TInt i=0;i<(allplugins->Count());i++)
	if(allplugins->Count())
	{
		TBuf8<50> dataBuf;
		dataBuf.Copy(allplugins->At(i)->Des());
		loggerSession.RemovePluginConfigurations(dataBuf);
	}
	CArrayFixFlat<TUint8> *getfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
	loggerSession.GetPrimaryFiltersEnabled(*getfilter);//C.A. previously:loggerSession.GetEnabledClassifications(*getfilter);
	TInt Result=loggerSession.SetPrimaryFiltersEnabled(*getfilter,EFalse);//C.A. previously:TInt Result=loggerSession.DisableClassifications(*getfilter);
	RArray<TUint32> get2filter;	
	loggerSession.GetSecondaryFiltersEnabled(get2filter);//C.A. previously:loggerSession.GetEnabledModuleUids(get2filter);
	loggerSession.SetSecondaryFiltersEnabled(get2filter,EFalse);//C.A. previously:loggerSession.DisableModuleUids(get2filter);
	//C.A. previously:loggerSession.EnableClassificationFiltering();
	loggerSession.SetSecondaryFilteringEnabled(ETrue);//C.A. previously:loggerSession.EnableModuleUidFiltering();
	loggerSession.SetBufferSize(1024);
	loggerSession.SetNotificationSize(512);
	loggerSession.SetBufferMode(1);//C.A. previously:loggerSession.SetBufferMode(EStraight);
}

//NB: currently this method does not set the default values for serial and usb! hanging the server
void  CClearConfig::DefaultL(RULogger& loggerSession)
	{
		ClearL(loggerSession);
		//Now set default configurations shipped with ulogger
		CArrayFixFlat<TUint8> *setfilter=new (ELeave)CArrayFixFlat<TUint8>(1);//CA:added so can use old ulogger api
		setfilter->AppendL(3);//CA:added so can use old ulogger api
		//setfilter->AppendL(13);//reintroduce this filter when pick up uloggerv2 - needed for SWAP analysis
		//set Classification filter
		loggerSession.SetPrimaryFiltersEnabled(*setfilter,ETrue);//CA:added so can use old ulogger api
		//C.A. previously:loggerSession.EnableClassification(3);
		//C.A. previously:loggerSession.EnableClassification(13);
		
		//set file plugin
		_LIT8(KTextmedia,"uloggerfileplugin");
		const TPtrC8 mediaPtr(KTextmedia);
		_LIT(KTextsetting, "output_path");
		_LIT(KTextvalue, "c:\\logs\\utracedata.utf");
		TPluginConfiguration pluginConfig;
		pluginConfig.SetKey(KTextsetting);
		pluginConfig.SetValue(KTextvalue);			
		loggerSession.SetPluginConfigurations(mediaPtr,pluginConfig);
		
		//set serial plugin 
	
		_LIT8(KSerialTextmedia,"uloggerserialplugin");
		const TPtrC8 serialMediaPtr(KSerialTextmedia);
		_LIT(KSerialTextsetting, "output_port");	
		_LIT(KSerialTextvalue, "3");
		TPluginConfiguration serialPluginConfig;
		serialPluginConfig.SetKey(KSerialTextsetting);
		serialPluginConfig.SetValue(KSerialTextvalue);			
		loggerSession.SetPluginConfigurations(serialMediaPtr,serialPluginConfig);
		
		//set usb plugin
		//add into TCl code >>TclDefault class/method
		
		/* this needs to be implemented
		 * -->usb plugin not available on the MCL
		 * either implement another version of this for MCL
		 * or change MCL version of the config file
		 * _LIT8(KUsbTextmedia,"uloggerusbplugin");
		 * const TPtrC8 usbMediaPtr(KUsbTextmedia);
		 * _LIT(KUsbTextsetting, "mode");	
		 * _LIT(KUsbTextvalue, "raw");
		 * TPluginConfiguration usbPluginConfig;
		 * pluginConfig.SetKey(KUsbTextsetting);
		 * pluginConfig.SetValue(KUsbTextvalue);			
		 * loggerSession.SetPluginConfigurations(usbMediaPtr,usbPluginConfig);
		 */
		
		
		//activate file plugin as active
		loggerSession.ActivateOutputPlugin(mediaPtr);//C.A. previously:loggerSession.SetActiveOutputPlugin(mediaPtr);				
		
	}
