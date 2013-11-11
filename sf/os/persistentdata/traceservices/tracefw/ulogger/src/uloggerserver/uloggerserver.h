// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger Server header file
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef ULOGGERSERVER_H
#define ULOGGERSERVER_H

#include "outputframework.h" // COutputFramework
#include "uloggerconfigmanager.h" // CConfig, CConfigSettingsIter, MConfigObserver
#include "uloggershared.h"
#include "uloggerwatcher.h"
#include "pluginallocator.h"
#include "inputframework.h"
#include <d32btrace.h> // RBTrace

namespace Ulogger {


enum TFilter
{
	EPrimaryFilter = 1,
	ESecondaryFilter
};


enum TULoggerSrvPanics
{
	EUnknownPanic = 1
};


/** CULoggerServer server class.
This class is responsible for maintaining the server state.
*/
class CULoggerServer : public CServer2, 
					   public MDataWatcherObserver, 
					   public MInputFrameworkObserver
	{
	public:

	static CULoggerServer* NewLC(TInt aPriority);
	~CULoggerServer();

	//own methods
	static TInt StartServer();
	void IncrementSessions();
	void DecrementSessions();

	//from CActive
	TInt RunError(TInt aError);

	//from MInputFrameworkObserver
	ControlData* ProcessCommandL(TCommand aOpCode, RArray<TPtrC8>& aArguments);
	void DoPostProcessing(TCommand aCmd);

	//From MDataWatcher
	void DataNotification();

	//server stuff
	TBool ServerState();

	//actions
	void RunAsService(TBool aRunAsService); // Not implemented yet
	TInt Start(); //this is actually only called from inside the server, or by the test framework.
	TInt Stop();
	TInt RestartOutputting();
	void ReadBufferL(); //this is actually only called from inside the server, or by the test framework.


	//filters
	TInt GetActiveFilters(RArray<TUint32>& aListBuffer,TInt aFilterType);
	TInt SetActiveFilterL(RArray<TUint32> aCategory, TInt aFilterType);
	TInt RemoveActiveFilter(RArray<TUint32>& aValue, const TInt aFilterType);
	TInt SetSecondaryFiltering(const TDesC8& aEnabled);
	TInt GetSecondaryFiltering(TBool& aEnabled);

	//plugins
	TInt SetPluginSettings(const TDesC8& aPluginname, RArray<TPtrC8>& aListBuffer);
	TInt RemovePluginSettingsL(const TDesC8& aPluginName);
	TInt GetActiveOutputPlugin(RArray<TPtrC8>& aListBuffer);
	TInt SetActiveOutputPlugin(const TDesC8& aPluginname);
	TInt GetInstalledOutputPlugins(RArray<TPtrC8>& aListBuffer);
	TInt DeActivateInputPlugin(const TDesC8& aPluginname);
	TInt GetActiveInputPlugin(RArray<TPtrC8>& aListBuffer);
	TInt SetActiveInputPlugin(const TDesC8& aPluginname);
	TInt GetInstalledInputPluginsL(RArray<TPtrC8>& aListBuffer);

	//buffers
	TInt GetOptionsSettingsL(const TDesC8& aMedianame, RArray<TPtrC8>& aListBuffer);
	TInt GetBufandDataNotifyValuesL(const TDesC8& aSetting,TInt& value);
	TInt SetBufferMode(const TDesC8& aValue);
	TInt SetBufferSizeL(TInt aSize);
	TInt SetDataNotificationSizeL(TInt aSize);

private:
	//server related stuff
	CULoggerServer(TInt aPriority);
	void ConstructL();
	static TInt StartServerL();
	static void PanicServer(TULoggerSrvPanics aPanic);
	static void PanicClient(const RMessage2& aMessage, TULoggerSrvPanics aPanic);
	void ShutDownServer();
	void RequestCancel();
	//from CServer2
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

	//on boot things
	TInt TraceSettingsOnBootL();
	TInt ResetBtracePrimaryFilters(RBTrace& aBTrace);

	//action related things

	//other configs
	void GetOptionsSettingsL(RArray<TPtrC8>& aListBuffer,const TDesC8& aSetting);
	TInt SetTraceSettings(const TDesC8&  aValue, const TDesC8& aSetting);

	//framework stuff
	void InitializeFrameworksL();
	void PrepareControlDataPayloadL(RBuf8& aPayloadBuf, const RArray<TPtrC8>& aArray);
	void PrepareControlDataPayloadL(RBuf8& aPayloadBuf, const RArray<TUint32>& aArray);
	
	//config file stuff
//	CConfig* CreateConfig(TFileName aFilePath);
//	void InitializeConfigFileL();
//	TInt CheckConfigFile(const TDesC& aFileName,const TDesC& aDirPath,TFileName& aFilePath);
//	void CopyFileToSystemDrive(TFileName& aFilePath);
	
	//plugin stuff
	TInt DeActivateOutputPlugin(const TDesC8& aPluginname);
	void FilterPlugins(TPluginFilter aFilter, RArray<TPtrC8>& aPluginList);
	TBool CheckPluginExists(TPtrC8& aPluginName, TPluginFilter aPluginFilter);
	void GetPluginAndSettingsL(TDes8& aPluginName, RPointerArray<TPluginConfiguration>* aSettings, TPluginFilter aPluginFilter);
	void AsynchDataRequest();
	
	//filter related stuff
	ControlData* SetPrimaryFilters(RArray<TPtrC8> &aArguments);
	TInt GetValuesL(const TDesC8& aSectionName, RArray<TPtrC8>& aListBuffer);

	// cleanup methods
	static void CleanupTPluginConfigArray(TAny* aPtr);

private:
	//data
	TInt iRunAsService;
	TInt iSessionCounter;
	
	 /** config API */
	//CConfig* iConfig;
	CConfigFileManager* iConfigManager;
    /** Output Framework */
	COutputFramework* iOutputFramework;
	/** Input Framework main object. */
	CInputFramework* iInputFramework;
	/** Data Watcher */
	CULoggerWatcher* iDataWatcher;
	/** Plugin Allocator */
	CPluginAllocator* iPluginAllocator;

	RImplInfoPtrArray iPluginArray;

	//BTrace
	RBTrace	  iTrace;
	TInt	  iBufferSize;
	TBool     iBtraceOpen;
	TInt      iDataNotification;
	TFileName iUserFilePath;
	TFileName iConfigFilePath;
	TBool     iUserFile;
	TBool     iConfigFile;
	TBool	  iIsBooting;
	};

}//namespace

#endif // ULOGGERSERVER_H
