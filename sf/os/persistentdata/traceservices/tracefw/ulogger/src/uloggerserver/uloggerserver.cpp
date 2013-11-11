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
//

#include <e32const.h>
#include <e32btrace.h>
#include <f32file.h>
#include "uloggerserver.h"
#include "uloggersession.h"
#include "uloggerclient.h"
#include "uloggerplugin.h"
#include "uloggershared.h"
#include "uloggerdatatypes.h"
#include "inputdata.h"
#include "uloggercommands.h"

#if defined(__LIGHTLOGGER_ENABLED)
#include "lightlogger.h" 
#endif


namespace Ulogger {


/*Default constructor*/
CULoggerServer::CULoggerServer(TInt aPriority)
: CServer2( aPriority )

{
	iIsBooting = ETrue;
	iConfigManager = NULL;
	iDataWatcher = NULL;
	iOutputFramework = NULL;
	iInputFramework = NULL;
	iPluginAllocator = NULL;
}


/** Destructor will be called before re-starting
*/
CULoggerServer::~CULoggerServer()
{
	delete iConfigManager;
	delete iOutputFramework;
	delete iInputFramework;
	delete iPluginAllocator;
	iPluginArray.ResetAndDestroy();
	iPluginArray.Close();
    if(iDataWatcher)
    	{
    	iDataWatcher->Cancel();
    	delete iDataWatcher; 
    	}
}

/** Static Factory Construction 
@return a pointer to the created object
@leave KErrNoMemory if not enough memory available
*/
CULoggerServer* CULoggerServer::NewLC(TInt aPriority)
{
	CULoggerServer* self = new (ELeave) CULoggerServer( aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
}

/**
Create and install new Framework & Active Scheduler
*/
void CULoggerServer::ConstructL()
{
	iRunAsService = EFalse; // Not used yet, to be used for run-as-service functionality
	iBufferSize = KMaxBufferSize;
	iBtraceOpen = EFalse;
	iDataNotification = 0;

	StartL( KServerName );	
	iConfigManager = CConfigFileManager::NewL();

}


/**
Create a new server session. 
@note Called by kernel after client's thread use any of the macros for the first time.
*/
CSession2* CULoggerServer::NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const
{
 	TVersion v(KULoggerSrvMajorVersionNumber,KULoggerSrvMinorVersionNumber,KULoggerSrvBuildVersionNumber);
    if (!User::QueryVersionSupported(v,aVersion))
         User::Leave(KErrNotSupported);

	RThread client;
	aMessage.Client(client);
	return CULoggerSession::NewL( client, *const_cast<CULoggerServer*>(this) );
}


/*Increment session counter*/
void CULoggerServer::IncrementSessions()
{	
	++iSessionCounter;
}

/*Decrement session counter*/
void CULoggerServer::DecrementSessions()
	{
	if(--iSessionCounter <= 0)
		{
		if (!iBtraceOpen)
			ShutDownServer();

//
// // Replace above if-statement with following to implement run-as-service functionality
//
//		if((!iBtraceOpen) && (!iRunAsService))
//			ShutDownServer();
//

		}
	}

/*!
Shuts down the server
*/
void CULoggerServer::ShutDownServer()
{
	CActiveScheduler::Stop();
}//</ShutDownServer>




/*!
Return the error value
*/
TInt CULoggerServer::RunError(TInt aError)
{
	Message().Complete(aError);
	ReStart();

	return KErrNone;
}




void CULoggerServer::PanicServer(TULoggerSrvPanics aPanic)
{
	User::Panic(KServerName, aPanic);
}//</PanicServer>



/*!
*/
void CULoggerServer::PanicClient(const RMessage2& aMessage, TULoggerSrvPanics aPanic)
{
	aMessage.Panic( KServerName, aPanic );
}//</PanicClient>




////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
ServerL
@Installs the active scheduler
@Opens the semaphore
*/
TInt CULoggerServer::StartServerL()
{
	#if defined(__LIGHTLOGGER_ENABLED)
	__CREATE_LOG(EFalse)
	__HIRES_RESOLUTION
	#endif	
	
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(activeScheduler) ;
    CActiveScheduler::Install(activeScheduler);
    CULoggerServer* server = NULL;
	TInt servererr = KErrNone;
	TRAP(servererr,
		{
		server = CULoggerServer::NewLC((TProcessPriority)EPriorityHigh);
		CleanupStack::Pop();
		});
	
	if(servererr==KErrNone||servererr==KErrNotFound )
		{
		RProcess::Rendezvous(KErrNone);
		servererr=KErrNone;
		CActiveScheduler::Start();
		}
	else
		{
		RProcess::Rendezvous(servererr);		
		}
	delete server;
   	CleanupStack::PopAndDestroy();
	return servererr;
	
}//</StartingFunctionL>


/**
Function to to start the server 
This should leave with KErrNone 
*/
TInt CULoggerServer::StartServer()
{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!cleanupStack)
	{
        	return KErrNoMemory;
	}
	TInt err;
    TRAP(err, StartServerL());
   
    delete cleanupStack;
    cleanupStack = NULL;

    return err;
}//</StartingFunction>


/**
Function to set the active output media 
This should leave with KErrNotFound if given plugin is not the correct one
or the file doesn't exists in the user and public path
*/

TInt CULoggerServer::SetActiveOutputPlugin(const TDesC8 &aPluginname)
{
	RArray<TPtrC8> activePluginsArray;
	TInt error= KErrNone;

	error = GetInstalledOutputPlugins(activePluginsArray);
	if(error==KErrNone)
		{
		for(TInt i=0;i<activePluginsArray.Count();i++)
			{
			if(aPluginname.Compare(activePluginsArray[i])==0)
				{
				error = iConfigManager->SetActiveOutputPlugin(aPluginname);
				break;
				}	
			else 
				error = KErrNotFound;
			}
		}
	if(error == KErrAlreadyExists)
		{
		error=KErrNone;
		}
	return error;
}


TInt CULoggerServer::SetActiveInputPlugin(const TDesC8 &aPluginname)
	{
	RArray<TPtrC8> activePluginsArray;
	TInt ret= KErrNone;
	//check configuration file in user path (\\ulogger\\uloggerconfig.ini)	

	ret = GetInstalledInputPluginsL(activePluginsArray);
	if(ret==KErrNone)
		{
		ret = KErrNotFound;
		for(TInt i=0;i<activePluginsArray.Count();i++)
			{
			if(aPluginname.Compare(activePluginsArray[i])==0)
				{
				ret = iConfigManager->SetActiveInputPlugin(aPluginname);
				break;	
				}
			}
		}
	return ret;
	}


/**
Function to set the plugin settings
This should leave with KErrNotFound if cannot file doesn't exist
*/
TInt CULoggerServer::SetPluginSettings(const TDesC8 &aPluginname, RArray<TPtrC8>& aListBuffer)
{	
	TInt i = 0;
	TInt ret=KErrNone;

	//if plug-in doesn't exists
	TPtrC8 pluginName(aPluginname);
	if(!CheckPluginExists(pluginName, EOutputPluginFilter) && !CheckPluginExists(pluginName, EInputPluginFilter))
		return KErrNotFound;
	
	while(i<aListBuffer.Count())
		{
		TPtrC8 key(aListBuffer[i]);
		TPtrC8 val(aListBuffer[++i]);
		ret = iConfigManager->SetPluginSetting(aPluginname, key, val);
		if(ret!=KErrNone)
			break;
		i++;		
		}					
	return ret;
}




/**
Function to get the plugin settings
This should leave with KErrNotFound if cannot find one
*/
TInt CULoggerServer::GetOptionsSettingsL(const TDesC8 &aMediaName, RArray<TPtrC8>& aListBuffer)
{
	//if plug-in doesn't exists
	TPtrC8 pluginName(aMediaName);
	if(!CheckPluginExists(pluginName, EOutputPluginFilter) && !CheckPluginExists(pluginName, EInputPluginFilter))
		return KErrNotFound;
	else
		{
		TInt ret = GetValuesL(aMediaName,aListBuffer);
		if(ret == KErrNotFound) //this error indicate that there is no section in config file, plug-in exists
			ret = KErrNone;
		return ret;
		}
}



/**
Function to get values from the configuration file (i.e. for filters,plugin settings)
This should leave with KErrNotFound if it cannot find the configuration file
*/
TInt CULoggerServer::GetValuesL(const TDesC8 &aSectionName, RArray<TPtrC8>& aListBuffer)
{	
	TPtrC8 key;
	TPtrC8 val;
	TInt err = KErrNotFound;	

	Ulogger::CConfigSettingsIter* iter = Ulogger::CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);
	
	//check User configuration file if exists
	//check values for given section in the configuration file 
	//if value exists copy it to array 
	err = iConfigManager->GetSectionValues(aSectionName,*iter);
	if(err == KErrNone)
		{
		TBool found =EFalse;
		while(iter->Next(key,val))
			{
			aListBuffer.AppendL(key);
			aListBuffer.AppendL(val);
			found = ETrue;
			}
		if(!found)
			err = KErrNotFound;
		}
	CleanupStack::PopAndDestroy(); // list
	return err;
	
}


/**
Sets the Active Filter in the configuration file (primary and secondary filters)
@aCategory filters to be set
@aDupCategory duplicate filters which are not set
@aFilter value that describes primary or secondary filter
@return KErrNone, if successful, otherwise one of the other system-wide
        error codes.
*/
TInt CULoggerServer::SetActiveFilterL(RArray<TUint32> aCategory, TInt aFilterType)
{
	TInt error = KErrNone;
	if(aFilterType == 1)
		{
		if(aCategory.Count()==0 || aCategory.Count()>KMaxPrimaryFiltersLimit)
			error = KErrArgument;
		else
			{
			error = iConfigManager->SetActiveFilter(aCategory,KPrimaryFilterSection);
			if(error == KErrAlreadyExists)
				error = KErrNone;
			}
		}
	if(aFilterType == 2)
		{
		//we don't want to set more secondary filters than KMaxSecondaryFiltersLimit
		RArray<TUint32> tmpArray;
		CleanupClosePushL(tmpArray);
		error = GetActiveFilters(tmpArray, ESecondaryFilter);
		if(!error || error == KErrNotFound)
			{
			error = KErrNone; //in case it was KErrNotFound as it doesn't matter we didn't have any set already
			if(aCategory.Count()==0 ||tmpArray.Count() > KMaxSecondaryFiltersLimit ||
					(tmpArray.Count()+ aCategory.Count()) > KMaxSecondaryFiltersLimit)
				error = KErrArgument;
				
			}
		CleanupStack::PopAndDestroy(); //tmpArray
		if(!error)
			error = iConfigManager->SetActiveFilter(aCategory,KSecondaryFilterSection);
		}
	return error;
}
	

/**
Function to remove the Filter from config file
@aFilter  Filter to be removed
@aValue primary or secondary filter to be removed
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/
TInt CULoggerServer::RemoveActiveFilter(RArray<TUint32>& aValue, const TInt aFilter)
{
	TInt ret = KErrNone;
	//remove filter (primary or secondary) from BTrace current configuration
	if(aFilter == EPrimaryFilter && (aValue.Count()==0 || aValue.Count()>KMaxPrimaryFiltersLimit))
		return KErrArgument;
	else if(aFilter == ESecondaryFilter && (aValue.Count()==0 || aValue.Count()>KMaxSecondaryFiltersLimit))
		return KErrArgument;
		
	ret = iConfigManager->RemoveActiveFilter(aValue,aFilter);
	if(ret == KErrNotFound) //remove operation should succeed in this situation
		ret = KErrNone;
		
	return ret;
}


/**
Function to set Trace settings to the configuration file
@aValue  value to be set
@aSetting setting to which value to be set
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/
TInt CULoggerServer::SetTraceSettings(const TDesC8&  aValue, const TDesC8& aSetting)
	{
	TInt r = iConfigManager->SetTraceSettings(aValue, aSetting);
	return r;

	}

/**
Function to remove plugin settings from the configuration file
@aPluginName  Plugin name whose settings has to be removed
@return will return KErrNone or else leave with KErrNotFound  
	or any system wide error codes 
*/

TInt CULoggerServer::RemovePluginSettingsL(const TDesC8 &aPluginName)
	{
	if(aPluginName.Length() == 0)
		return KErrArgument;
	
	TInt ret = KErrNone;
	RArray<TPtrC8> settings;
	ret = GetOptionsSettingsL(aPluginName, settings);
	if(ret != KErrNone || settings.Count()==0)
		ret = KErrNotFound;
	settings.Close();
		
	if(ret == KErrNone)
		return iConfigManager->RemovePluginSettings(aPluginName);
	else
		return ret;
	}

/**
Resize the BTrace buffer size

@aSize The size to be set for the BTrace buffer.
@return KErrNone, if successful, otherwise one of the other system-wide
        error codes.
*/
TInt CULoggerServer::SetBufferSizeL(TInt aSize)
	{
	if(aSize > KMaxBufferSize || aSize < 1)
		return KErrArgument;

	
	//get dns to check
	GetBufandDataNotifyValuesL(KDataNotification,iDataNotification);
	if(aSize < iDataNotification)
		return KErrGeneral;
	
	iBufferSize = aSize;
	HBufC8* Size = HBufC8::NewLC(32);
	Size->Des().Num(iBufferSize);
	CleanupStack::Pop(1);
	TPtr8 ptr(Size->Des());
	TInt r = iConfigManager->SetTraceSettings((const TDesC8&)ptr, KBuffer);
	return r;
	}


/**
Change the mode of the BTrace Buffer

@aValue The mode to be set for the BTrace buffer.
@return KErrNone, if successful, otherwise one of the other system-wide
        error codes.
*/
TInt CULoggerServer::SetBufferMode(const TDesC8& aValue)
	{
	
	TInt r = SetTraceSettings(aValue, KBufferMode);
	return r;
	}

/**
Resize the Data Notification size

@aSize The size to be set for the Data Notification size.
@return KErrNone, if successful, otherwise one of the other system-wide
        error codes.
*/
TInt CULoggerServer::SetDataNotificationSizeL(TInt aSize)
	{
	if(aSize > KMaxDnsSize || aSize < 0)
		return KErrArgument;
	TInt size, r;
	TRAP_IGNORE((r=GetBufandDataNotifyValuesL(KBuffer,size)));
	if(aSize > size)
		return KErrGeneral;
	
	iDataNotification = aSize;
	HBufC8* Size = HBufC8::NewLC(32);
	Size->Des().Num(aSize);
	TPtr8 ptr(Size->Des());
	r = SetTraceSettings((const TDesC8&)ptr, KDataNotification);
	CleanupStack::PopAndDestroy();
	return r;
	}
	
/**
Function to retrieve the Trace Section Values
This should leave with KErrNotFound if cannot find one
as this should be treated as a corrupt config file
*/
void CULoggerServer::GetOptionsSettingsL(RArray<TPtrC8>& aListBuffer,const TDesC8& aSetting)
{
	TPtrC8 key;
	TPtrC8 val;	
	TInt err=0;
	
	Ulogger::CConfigSettingsIter* iter = Ulogger::CConfigSettingsIter::NewL();
	CleanupStack::PushL(iter);
	err=iConfigManager->GetPluginSettings(*iter);
	
	if(err==KErrNone)
		{
		while(iter->Next(key,val))
			{
			if(key.Compare(aSetting)==0)
				{
				aListBuffer.AppendL(key);
				aListBuffer.AppendL(val);					
				}
			}
		}
	CleanupStack::PopAndDestroy(); // list
}

/**
Function to retrieve the active primary filters
This should leave with KErrNotFound if cannot find one
as this should be treated as a corrupt config file
*/
TInt CULoggerServer::GetBufandDataNotifyValuesL(const TDesC8& aSetting,TInt& value)
	{
	TInt err = KErrNotFound;
	RArray<TPtrC8> traceValues;
	TFileName filepath;
	
	GetOptionsSettingsL(traceValues,aSetting);
		
	value =-1;
	//if values exists for the given trace settings	
	if(traceValues.Count()>1)
		{// get values for buffer mode
		if(aSetting.Compare(KBufferMode)==0)	
			{
			err=KErrNone;
			if(traceValues[1].Compare(KCircular) == 0)
				value = ECircularBuffer;
			else if(traceValues[1].Compare(KStraight) == 0)
				value = EStraightBuffer;
			else
				err=KErrNotFound;
			}
		else
			{// get values for data notification and buffer size
			TLex8 lex_val(traceValues[1]);	
			err = lex_val.Val(value);
			}
		}
	return err;
	}
	


/**
Set the enable/disable secondary filters

@aSize The size to be set for the BTrace buffer.
@return KErrNone, if successful, otherwise one of the other system-wide
        error codes.
*/
TInt CULoggerServer::SetSecondaryFiltering(const TDesC8& aEnabled)
	{
	TInt error = iConfigManager->SetTraceSettings(aEnabled, KSecondaryGlobalFilter);
	if(error == KErrAlreadyExists)
		error = KErrNone;
	return error;

	}	


/**
Function to retrieve the active primary and secondary filters
This should return KErrNotFound if cannot find one or return any of the system wide error codes
*/
TInt CULoggerServer::GetActiveFilters(RArray<TUint32>& aListBuffer,TInt aFilterType)
	{
	TInt ret = KErrNotFound;
	RArray<TPtrC8> aValues;
	if(aFilterType == 1)
		{
		TRAPD(err, ret = GetValuesL(KPrimaryFilterSection,aValues));
		if (err != KErrNone)
			{
			return err;
			}
		
		if(ret==KErrNone)	
			{
			TInt i =0;
			TPtrC8 val;	
			TUint8 int_val;
			while(i<aValues.Count())
				{
				TLex8 lex_val(aValues[++i]);	
				ret = lex_val.Val(int_val,EDecimal);	
				if(ret == KErrNone)
					{
					ret = aListBuffer.Append(int_val);
					if (ret != KErrNone)
						{
						return ret;
						}
					}
				else
					{
					return KErrCorrupt;
					}
				i++;		
				}					
			}
		}
	else if(aFilterType == 2)
		{
		TRAPD(err, ret = GetValuesL(KSecondaryFilterSection,aValues));
		if (err != KErrNone)
			{
			return err;
			}
		if(ret==KErrNone)	
			{
			TInt i =0;
			TPtrC8 val;	
			TUint int_val;
			while(i<aValues.Count())
				{
				TLex8 lex_val(aValues[++i]);	
				ret = lex_val.Val(int_val,EDecimal);	
				if(ret == KErrNone)
					{
					ret = aListBuffer.Append(int_val);
					if (ret != KErrNone)
						{
						return ret;
						}
					}
				else
					{
					return KErrCorrupt;
					}
				i++;		
				}					
			}
		else
			{
			if(ret==KErrNotFound)//i.e. if there are no values in the array --> change to if(filters.Count = 0)?
				{
				ret=KErrNone;
				}
			}
		}
	return ret;
	}

/**
Function to retrieve the active secondary global filter
This should leave with KErrNotFound if cannot find one
*/

TInt CULoggerServer::GetSecondaryFiltering(TBool& aEnabled)
{
	RArray<TPtrC8> configSettings;			
	TRAPD(error, GetOptionsSettingsL(configSettings,KSecondaryGlobalFilter));
	if(configSettings[1]==KEnable) //First element contains KSecondaryGlobalFilter, second the value
		aEnabled = ETrue;
	if(configSettings[1]==KDisable)
		aEnabled = EFalse;
	return error; 
}

/**
Function to retrieve the plugin names from the ECOM framework
This should leave with KErrNotFound if cannot find one
*/
TInt CULoggerServer::GetInstalledOutputPlugins(RArray<TPtrC8>& aListBuffer)
	{
	// Read info about all implementations into infoArray
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	if(iPluginArray.Count()>0)
	{
     iPluginArray.ResetAndDestroy();
     iPluginArray.Close();
	}
	
	TRAPD(error,CPluginAllocator::ListAllImplementationsL(iPluginArray));

	if(iPluginArray.Count()>0)
	{
		for (TInt i=0; i< iPluginArray.Count(); i++)
			{
			TRAP(error,aListBuffer.AppendL(iPluginArray[i]->DataType()));
			}
	
		//filter plugins and to use only output plugins
		FilterPlugins(EOutputPluginFilter, aListBuffer);
	}
	
	return error;
	}
	


TInt CULoggerServer::GetInstalledInputPluginsL(RArray<TPtrC8>& aListBuffer)
	{
	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__MARK_METHOD("CULoggerServer::GetInstalledControlPlugins")
	#endif
	
	if(iPluginArray.Count()>0)
		{
	     iPluginArray.ResetAndDestroy();
	     iPluginArray.Close();
		}
	// Read info about all implementations into infoArray
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	CPluginAllocator::ListAllImplementationsL(iPluginArray);
		
	if(iPluginArray.Count() > 0)
		{	
		for (TInt i=0; i< iPluginArray.Count(); i++)
			aListBuffer.AppendL(iPluginArray[i]->DataType());
		
		//filter plugins and to use only output plugins
		FilterPlugins(EInputPluginFilter, aListBuffer);
		}
	return KErrNone;
	}


/**
Function to retrieve the Active plugin name 
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/
TInt CULoggerServer::GetActiveOutputPlugin(RArray<TPtrC8>& aListBuffer)
	{
	TInt errCode = GetValuesL(KActiveSection,aListBuffer);
	//plugins are received as, e.g. '1 uloggerfileplugin' (.ini file syntax)
	//we have to remove number from array
	for(TInt i=0; i<aListBuffer.Count(); ++i)
		{
		TInt v;
		TLex8 lex(aListBuffer[i]);
		if(lex.Val(v) == KErrNone)
			{
			aListBuffer.Remove(i);
			i=-1; //start from begin, i will be incremented in loop
			}
		}
	return errCode;
	}


/**
Function to Deactivate or remove active output plugin 
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/

TInt CULoggerServer::DeActivateOutputPlugin(const TDesC8 &aPluginname)
{	
	if(iBtraceOpen != EFalse)
		{
		Stop();
		iBtraceOpen = EFalse;
		}
	
	TInt ret = iConfigManager->DeActivateOutputPlugin(aPluginname);
	return ret;
}

/**
Function to retrieve the Active control plugin name 
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/
TInt CULoggerServer::GetActiveInputPlugin(RArray<TPtrC8>& aListBuffer)
	{
	return GetValuesL(KActiveControlSection,aListBuffer);
	}

/**
Function to Deactivate active control plugin 
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/
TInt CULoggerServer::DeActivateInputPlugin(const TDesC8& aPluginname)
	{
	return iConfigManager->DeActivateInputPlugin(aPluginname);
	}

/**
Function to run ULogger server as service, i.e. to keep it constantly running
(but not necessarily logging) in the background, or to stop it being run as a
service (if EFalse is passed).

@param aRunAsService ETrue to start, EFalse to stop running as service
*/
void CULoggerServer::RunAsService(TBool /*aRunAsService*/)
	{
//
// // Uncomment following to implement run-as-service functionality
//
//	iRunAsService = aRunAsService;
//
//	if (!aRunAsService)
//		{
//		//make sure server is stopped if there are no sessions
//		IncrementSessions();
//		DecrementSessions();
//		}
	}

/**
Function to cd.. ulogger server
This should leave with KErrNotFound if cannot find one or any system wide error codes
*/
TInt CULoggerServer::Start()
	{
	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__MARK_METHOD("CULoggerServer::Start")
	#endif

	TRAPD(error, InitializeFrameworksL());
	
	//check errors
	if(error != KErrNone)
		return error;
	if(!iOutputFramework)
		return KErrNotReady;
	if(iBtraceOpen)
		return KErrInUse;

	TPtrC8 key;
	TPtrC8 val;
	TInt buffermode=0;
	RArray<TPtrC8> settings;
	RArray<TUint32> activefilters;
	//checking if there is at least one active channel
	error = GetActiveOutputPlugin(settings);
	if(error != KErrNone)
		return error;
	if(settings.Count()==0)
		{
		return KErrNotFound; //active output plug-in not found
		}

	if(iBtraceOpen == EFalse)
		{
		if(iIsBooting)
			error = TraceSettingsOnBootL();
		else
			error = iTrace.Open();

		if(error == KErrNone)
			{
			//----Reset the notification size
			TRAP_IGNORE(error=GetBufandDataNotifyValuesL(KDataNotification,iDataNotification));
			if(error!=KErrNone)
				return error;
			TRAP_IGNORE(error=GetBufandDataNotifyValuesL(KBuffer,iBufferSize));
			if(error!=KErrNone)
				return error;

			//----Reset BTrace filters
			ResetBtracePrimaryFilters(iTrace);

			//----Resize the trace buffer
			error = iTrace.ResizeBuffer(iBufferSize*1024);
			if(error == KErrNoMemory)
			{
				error = iTrace.ResizeBuffer(KMinBufferMTPLE*1024);
			}
			if(error != KErrNone)
				return error;
			//----Reset the buffer mode
			TRAP_IGNORE(GetBufandDataNotifyValuesL(KBufferMode,buffermode));
			if(buffermode==ECircularBuffer)
			{
				iTrace.SetMode(RBTrace::EFreeRunning|RBTrace::EEnable);	
			}
			else
			{
				iTrace.SetMode(RBTrace::EEnable);
			}
		
			//----Enable/disable primary filter
			error = GetActiveFilters(activefilters,EPrimaryFilter);
			if(error != KErrNone)
				return error;
			for(TInt i=0; i<activefilters.Count(); ++i)
				{
				TUint filter;
				filter= (TUint8)activefilters[i];
				iTrace.SetFilter(filter,ETrue);
				}

			//----Enable/disable secondary filter
			settings.Reset();
			TBool enabled = EFalse;
			error = GetSecondaryFiltering(enabled);
			if(!error)
				{
				if (enabled)//enable secondary filter, only traces with certain value will pass
					{
					iTrace.SetFilter2(0);

					//enable secondary filters chosen by user
					activefilters.Reset();
					if((error = GetActiveFilters(activefilters,ESecondaryFilter)) == KErrNone)
						for(TInt i=0; i<activefilters.Count(); ++i)
							iTrace.SetFilter2(activefilters[i], ETrue);
					else
						{
						if(error == KErrNotFound)
							{
							error = KErrNone;
							}
						else
							return error;
						}
					}
				else//disable secondary filter, all traces will pass
					{
					iTrace.SetFilter2(1);
					}				
				}
			
			//set flag to idicate that btrace is open
			iBtraceOpen = ETrue;

			//start input and output frameworks
			iInputFramework->StartReading(); //start waiting for remote commands
			AsynchDataRequest();
			}
		}
	else
		error = KErrInUse;

	return error;
	}

/**
Close the BTrace handle to the buffer
@return none
*/
TInt CULoggerServer::Stop()
{
	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__MARK_METHOD("CULoggerServer::Stop")
	#endif
	
	if(iBtraceOpen)
	{	
		TInt errCode = KErrNone;
		TRAP(errCode, ReadBufferL());	//flush the BTrace buffer
		
		ResetBtracePrimaryFilters(iTrace);//reset BTrace filters
		
		iTrace.CancelRequestData();	
		iTrace.Empty();
		iTrace.Close();
		iBtraceOpen = EFalse;
		iDataWatcher->Cancel();
		
		//stop waiting/reading for remote commands
		iInputFramework->StopReading();
		iOutputFramework->ReleaseOutputResources();
		return KErrNone;
		}
	else
		{
		return KErrNotReady;	
		}
		
}

/**
Gets the Trace settings on boot
@return none
*/
TInt CULoggerServer::TraceSettingsOnBootL()
{
	TInt trace = FALSE;
	
	RArray<TUint32> category;
	TInt error = iTrace.Open();
	if(error != KErrNone)
		return error;
	
	TInt bufferSize = iTrace.BufferSize(); //To get the buffer size set at boot	
	if(bufferSize <= 0 && bufferSize > KMaxBufferSize)
		SetBufferSizeL(KMaxBufferSize); //Update config
	else
		SetBufferSizeL(bufferSize); //Update config
	for(TUint i=0; i<KMaxPrimaryFiltersLimit; i++)
	{
		trace = iTrace.Filter(i);
		if(trace == 1)         
			category.AppendL((TUint32)i);				
	}
	error = iConfigManager->SetActiveFilter(category,KPrimaryFilterSection);
	category.Close();
	iIsBooting=0;
	return error;
}



/**
Reads the BTrace buffer and transfers the collected logs to
the ACTIVE channel
*/
void CULoggerServer::ReadBufferL()
    {
    if(iBtraceOpen)
    	{   	
    	TUint8* data;
		TInt dataSize;
		//TUint oldMode = iTrace.Mode();
		//iTrace.SetMode(0); // turn off trace capture while we dump
		dataSize = iTrace.GetData(data);
		if(dataSize > 0)
			{

	 		TPtrC8 dataPtr(data, dataSize);
			
	 		#if defined(__LIGHTLOGGER_ENABLED) && defined(__PERFORMANCE_TEST)
 			__LOGTIMESTAMP_HIRES("Passing data to output framework - t2") //performance test
			#endif
 			
 			//send data through output channel
 			TInt error = KErrNone;
 			if((error = iOutputFramework->SendData(dataPtr)) == KErrNone)
 				iTrace.DataUsed(); //only if data was really sent via output plugin
 			else
 				User::Leave(error);
	 		
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__PERFORMANCE_TEST)
	 		__LOGTIMESTAMP_HIRES("Data logged - t3")	//performance test
			#endif
			}
		//iTrace.SetMode(oldMode);
			
		AsynchDataRequest();
	    } 
    }


void CULoggerServer::AsynchDataRequest()
	{
	if(!iDataWatcher->IsActive())
		{
		iTrace.RequestData(iDataWatcher->GetStatus(), iDataNotification*1024); 
		iDataWatcher->StartWatching(this);
		}
	}


void CULoggerServer::DataNotification()
{
	TRAPD(e, ReadBufferL());
	if(e != KErrNone)
		AsynchDataRequest();
}


void CULoggerServer::GetPluginAndSettingsL(TDes8& aPluginName, RPointerArray<TPluginConfiguration>* aSettings, TPluginFilter aPluginFilter)
	{	
	TPtrC8 key;
	TPtrC8 val;
	RArray<TPtrC8> activePluginsArray;
	RArray<TPtrC8> pluginsArray;
	RArray<TPtrC8> pluginSettings;
	TInt i,j;
	
	if(aPluginFilter == EOutputPluginFilter)
		GetActiveOutputPlugin(activePluginsArray); //get output plugin
	else
		GetActiveInputPlugin(activePluginsArray); //get control plugin
	
	for(i=0; i<activePluginsArray.Count(); ++i)
		{
		TBuf8<256> activeplugin;
		activeplugin.Copy(activePluginsArray[i]);
		activeplugin.LowerCase();
		
		GetInstalledOutputPlugins(pluginsArray);
		for(j=0;j<pluginsArray.Count();j++)
			{
			TBuf8<256> plugin;
			plugin.Copy(pluginsArray[j]);
			plugin.LowerCase();
			
			if(activeplugin.Compare(plugin)==0)
				{
				break;
				}
			}
		aPluginName.Copy(activePluginsArray[i]);
		GetOptionsSettingsL(aPluginName,pluginSettings);
		j=0;
		while(j < pluginSettings.Count())
			{
			TPluginConfiguration* keyAndVal = new TPluginConfiguration();
			keyAndVal->SetKey(pluginSettings[j]);
			keyAndVal->SetValue(pluginSettings[j+1]);
			aSettings->AppendL(keyAndVal);
			j=j+2;
			}
		}
	}


/*
 * Cleanup RPointerArray<TPluginConfiguration>* object by calling ResetAndDestroy to delete memory
 * allocated as TPluginConfigurations whose ownership has been passed to the RPointerArray.
 *
 */
void CULoggerServer::CleanupTPluginConfigArray(TAny* aPtr)
	{
	RPointerArray<TPluginConfiguration>* ptrArray = reinterpret_cast<RPointerArray<TPluginConfiguration>*>(aPtr);
	ptrArray->ResetAndDestroy();
	ptrArray->Close();
	}


void CULoggerServer::InitializeFrameworksL()
	{
	//<create plugin allocator (plugins)>
	//output settings
	RBuf8 outPluginName;
	outPluginName.CreateL(KMaxPluginName);
	CleanupClosePushL(outPluginName);
	RPointerArray<TPluginConfiguration> outputPluginSettings;
	CleanupStack::PushL(TCleanupItem(CleanupTPluginConfigArray, &outputPluginSettings));
	GetPluginAndSettingsL(outPluginName, &outputPluginSettings, EOutputPluginFilter);

	//control settings
	RBuf8 inputPluginName;
	inputPluginName.CreateL(KMaxPluginName);
	CleanupClosePushL(inputPluginName);
	RPointerArray<TPluginConfiguration> inputPluginSettings;
	CleanupStack::PushL(TCleanupItem(CleanupTPluginConfigArray, &inputPluginSettings));
	GetPluginAndSettingsL(inputPluginName, &inputPluginSettings, EInputPluginFilter);

	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__LOG("before creating CPluginAllocator")
	#endif
	
	//create plugin allocator (plugins)
	if(!iPluginAllocator)
		{
		iPluginAllocator = CPluginAllocator::NewL(outPluginName, inputPluginName);
		}

	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__LOG("before creating COutputFramework")
	#endif
	//Initialize output framework
	if(!iOutputFramework)
		{
		iOutputFramework = COutputFramework::NewL(*(iPluginAllocator->GetOutputPlugin()), outputPluginSettings);
		}

	//Initialize Control Framework
	if(!iInputFramework)
		{
		iInputFramework = CInputFramework::NewL(iPluginAllocator->GetInputPlugin(), inputPluginSettings, this);
		}

	//cleanup
	CleanupStack::PopAndDestroy(4, &outPluginName); // and outputPluginSettings, inputPluginName and inputPluginSettings

	iDataWatcher = CULoggerWatcher::NewL();	
	}


void CULoggerServer::PrepareControlDataPayloadL(RBuf8& aPayloadBuf, const RArray<TPtrC8>& aArray)
	{
	aPayloadBuf.CreateL(aArray.Count()*32);
	for(TInt i=0; i<aArray.Count(); i++)
		{
		if(aPayloadBuf.MaxSize() < aPayloadBuf.Length()+aArray[i].Length())
			{
			aPayloadBuf.ReAllocL(aPayloadBuf.Length()+(aArray[i].Length()*10));
			}
		aPayloadBuf.Append(aArray[i]);
		if(i < aArray.Count()-1) //skip last sparator as it will be added automatically
			{
			aPayloadBuf.Append(DATA_SEPARATOR);
			}
		}
	}


void CULoggerServer::PrepareControlDataPayloadL(RBuf8& aPayloadBuf, const RArray<TUint32>& aArray)
	{
	aPayloadBuf.CreateL(aArray.Count()*4);
	for(TInt i=0; i<aArray.Count(); i++)
		{
		TBuf8<64> b;
		b.Num(aArray[i]);
		if(aPayloadBuf.MaxSize() < aPayloadBuf.Length()+b.Length())
			{
			aPayloadBuf.ReAllocL(aPayloadBuf.Length()+(b.Length()*10));
			}
		aPayloadBuf.Append(b);
		if(i < aArray.Count()-1) //skip last sparator as it will be added automatically
			{
			aPayloadBuf.Append(DATA_SEPARATOR);
			}
		}
	}


TInt CULoggerServer::RestartOutputting()
	{
	TInt err=KErrNone;
	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__MARK_METHOD("CULoggerServer::ForceRefreshSettingsL")
	#endif
	if(iBtraceOpen != EFalse)
		{
		err = Stop();
		iBtraceOpen = EFalse;
		}

	 if(iDataWatcher)
		 {
		 iDataWatcher->Cancel();
		 delete iDataWatcher;
		 iDataWatcher = NULL;
		 }
	 if(iOutputFramework)
		 {
		 delete iOutputFramework;
		 iOutputFramework = NULL;
		 }
	 if(iInputFramework)
		 {
		 delete iInputFramework;
		 iInputFramework = NULL;
		 }
	 if(iPluginAllocator)
		 {
		 delete iPluginAllocator;
		 iPluginAllocator = NULL;
		 }
	if(iConfigManager)
		{
		iConfigManager->RefreshConfigFiles();
		}

	err = Start(); //always start
	return err;
	}


TInt CULoggerServer::ResetBtracePrimaryFilters(RBTrace	&aBTrace)
	{
	for(TUint i=0; i<256; i++)
		aBTrace.SetFilter(i, EFalse);
	aBTrace.SetFilter2(0); 
	return KErrNone;
	}

//This bit should be part of the input framework, or it should reuse the other methods better
ControlData* CULoggerServer::ProcessCommandL(TCommand aOpCode, RArray<TPtrC8> &aArguments)
	{
	#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
	__MARK_METHOD("CULoggerServer::ProcessCommand")
	#endif	
	
	TInt errCode = KErrNone;
	TInt minSize = 8;
	CInputData *inputData = new (ELeave) CInputData(minSize);
	ControlData* data = NULL;
	RBuf8 payloadBuf;
	TBuf8<32> result;

	switch(aOpCode)
		{
		case EStart:
			{
			if(!ServerState())
				errCode = this->Start();
			else
				errCode = KErrInUse;
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case EStop:
			{
			//ulogger shouldn't be restarted here as we must send acknowledgment back to PC
			//Stop will be done in DoPostProcessing method after sending ack.
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;
			
		case ESetPrimaryFilter:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ESetPrimaryFilter")
			#endif
			
			RArray<TUint32> filterArray;
			
			errCode = KErrNone;
			//process parameters
			TInt i;
			for(i=0; i<aArguments.Count(); i++)
				{
				TUint8 val;
				TLex8 lex(aArguments[i]);
				if((errCode=lex.Val(val, EDecimal)) != KErrNone)
					{
					//report bad values, don't set anything
					if(errCode == KErrGeneral)
						errCode = KErrArgument;
					result.Num(errCode);
					data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
					/*
					payloadBuf.Create(128);
					payloadBuf.Copy(aArguments[i]);
					inputData->AppendNewData(ack, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
					*/
					filterArray.Close();
					break;
					}
				filterArray.AppendL(val);
				}
			
			if(errCode == KErrNone)
				{
				errCode = SetActiveFilterL(filterArray, EPrimaryFilter);
				result.Num(errCode);
			
				//create acknowledment
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			
				//create string as a payload
				/*
				payloadBuf.CreateL(dupfilterArray.Count()*4);
				for(i=0; i<dupfilterArray.Count(); i++)
					{
					TBuf8<32> b;
					b.Num(dupfilterArray[i]);
					if(payloadBuf.MaxSize() < payloadBuf.Length()+b.Length())
						payloadBuf.ReAllocL(payloadBuf.Length()+(b.Length()*10));
					payloadBuf.Append(b);
					if(i < dupfilterArray.Count()-1) //skip last sparator as it will be added automatically
						payloadBuf.Append(DATA_SEPARATOR);
					}
				if(payloadBuf.Length() > 0)
					inputData->AppendNewData(ack, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
				*/
				filterArray.Close();
				}
			}
		break; //ESetPrimaryFilter
		
		case ESetSecondaryFilter:
			{
			RArray<TUint32> filterArray;
			//process parameters
			TInt i;
			for(i=0; i<aArguments.Count(); i++)
				{
				TUint32 val;
				TLex8 lex(aArguments[i]);
				if((errCode=lex.Val(val,EDecimal)) != KErrNone)
					{
					//report bad values, don't set anything
					if(errCode == KErrGeneral)
						errCode = KErrArgument;
					result.Num(errCode);
					data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
					/*
					payloadBuf.CreateL(128);
					payloadBuf.Copy(aArguments[i]);
					man->AppendNewData(ack, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
					*/
					filterArray.Close();
					break;
					}
				filterArray.AppendL(val);
				}
			if(errCode != KErrNone)
				break;
			
			//create ack
			errCode = SetActiveFilterL(filterArray, ESecondaryFilter);
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
		
			filterArray.Close();
			}
		break; //ESetSecondaryFilter

		case EEnableSecondaryFiltering:
			{
			errCode = SetSecondaryFiltering(KEnable);
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case EDisableSecondaryFiltering:
			{
			errCode = SetSecondaryFiltering(KDisable);
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case EResizeTraceBuffer:
			{
			if(aArguments.Count() > 0)
				{
				TInt bufSize;
				TLex8 lex(aArguments[0]);
				if((errCode = lex.Val(bufSize)) == KErrNone)
					errCode = SetBufferSizeL(bufSize);
				}
			else
				errCode = KErrArgument;

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case ESetBufferMode:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ESetBufferMode")
			#endif

			if(aArguments.Count() > 0)
				{
				if(aArguments[0].Compare(KCircular)==0)
					errCode  = SetBufferMode(KCircular);
					else if(aArguments[0].Compare(KStraight)==0)
						errCode  = SetBufferMode(KStraight);
						else
							errCode = KErrArgument;
				}
			else
				errCode = KErrArgument;

			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("errCode")
			__LOGNUM(errCode)
			#endif

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case ESetDataNotificationSize:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ESetDataNotificationSize")
			#endif

			if(aArguments.Count() > 0)
				{
				TInt dns;
				TLex8 lex(aArguments[0]);
				if((errCode = lex.Val(dns)) == KErrNone)
					errCode = SetDataNotificationSizeL(dns);
					else
						errCode = KErrArgument;
				}
			else
				errCode = KErrArgument;

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case ERemovePluginSettings:
			{
			if(aArguments.Count() > 0)
				errCode = RemovePluginSettingsL(aArguments[0]);
			else
				errCode = KErrArgument;
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case EGetInstalledPlugins:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetInstalledOutputPlugins")
			#endif

			RArray<TPtrC8> tmpArray;
			errCode = GetInstalledOutputPlugins(tmpArray);
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());

			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			for(TInt p=0; p<tmpArray.Count(); p++) __LOGBUF(tmpArray[p])
			#endif

			//create payload
			PrepareControlDataPayloadL(payloadBuf, tmpArray);
			//add playload in control data chunk
			if(payloadBuf.Length() > 0)
				inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
			}
		break; //EGetInstalledOutputPlugins

		case EGetActivePlugin: //output
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetActivePlugin")
			#endif

			RArray<TPtrC8> tmpArray;
			errCode = GetActiveOutputPlugin(tmpArray);
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());

			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			for(TInt p=0; p<tmpArray.Count(); p++) __LOGBUF(tmpArray[p])
			#endif

			//create payload
			PrepareControlDataPayloadL(payloadBuf, tmpArray);
			//add playload in control data chunk
			if(payloadBuf.Length() > 0)
				inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
			}
		break; //EGetActivePlugin

		case EGetPluginSettings:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetPluginSettings")
			#endif
			RArray<TPtrC8> tmpArray;
			if(aArguments.Count() > 0)
				{	
				errCode = GetOptionsSettingsL(aArguments[0],tmpArray);

				#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
				for(TInt p=0; p<tmpArray.Count(); p++) __LOGBUF(tmpArray[p])
				#endif
				}
			else
				errCode = KErrArgument;

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			//create payload
			PrepareControlDataPayloadL(payloadBuf, tmpArray);
			//add playload in control data chunk
			if(payloadBuf.Length() > 0)
				inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
			}
		break; //EGetPluginSettings

		case EGetPrimaryFilters:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetPluginSettings")
			#endif

			RArray<TUint32> tmpArray;
			errCode = GetActiveFilters(tmpArray,EPrimaryFilter);

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());

			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			for(TInt p=0; p<tmpArray.Count(); p++) __LOGNUM(tmpArray[p])
			#endif

			//create payload
			PrepareControlDataPayloadL(payloadBuf, tmpArray);

			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetPluginSettings - test 1")
			#endif

			
			if(payloadBuf.Length() > 0)
				inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
			tmpArray.Close();
			
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetPluginSettings - test 2")
			#endif
			}
		break; //EGetPrimaryFilters
		
		case EGetSecondaryFilters:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetSecondaryFilters")
			#endif
			
			TBool skipReadingFilters = EFalse;
			RArray<TPtrC8> tmpArrayTxt;
			TBool enabled = EFalse;
			errCode = errCode = this->GetSecondaryFiltering(enabled);
			if(errCode == KErrNone)
				{
				if(EFalse == enabled)
						{
						result.Num(errCode);
					data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
						skipReadingFilters = ETrue;
						//create payload
					PrepareControlDataPayloadL(payloadBuf, tmpArrayTxt);
						if(payloadBuf.Length() > 0)
						inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
						}
				}

			RArray<TUint32> tmpArray;
			if(!skipReadingFilters)
				{
				errCode = this->GetActiveFilters(tmpArray,ESecondaryFilter);
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				//create payload
				PrepareControlDataPayloadL(payloadBuf, tmpArray);
				if(payloadBuf.Length() > 0)
					inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
				}
			
			tmpArrayTxt.Close();
			tmpArray.Close();
			}
		break; //EGetSecondaryFilters
		
		case EGetSecondaryFiltering:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetSecondaryFiltering")
			#endif
			
			TBool enabled = EFalse;
			errCode = GetSecondaryFiltering(enabled);
			if(errCode == KErrNone)
				{
			result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				
				//create payload
				TBuf8<32> b;
				b.Num(enabled);
				inputData->AppendNewData(data, (const void*)b.Ptr(), b.Length());
				}
			else
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				}
			}
		break; //EGetSecondaryFiltering
		
		//this functionality is removed now
		/*
		case ESetSecondaryFilterCurrent:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ESetSecondaryFilterCurrent")
			#endif
			
			errCode = this->SetSecondaryFiltering(KCurrent);
			result.Num(errCode);
			ack = man->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break; //ESetSecondaryFilterCurrent
		*/
		case EGetTraceBufferSize:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetTraceBufferSize")
			#endif
			
			TInt bufferSize = 0;	
			errCode = GetBufandDataNotifyValuesL(KBuffer,bufferSize);
			if(errCode == KErrNone)
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				TBuf8<32> b;
				b.Num(bufferSize);
				inputData->AppendNewData(data, (const void*)b.Ptr(), b.Length());
				}
			else
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				}
			}
		break; //EGetTraceBufferSize
		
		case EGetBufferMode:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetBufferMode")
			#endif
			
			TInt bufferMode = 3;	
			errCode = GetBufandDataNotifyValuesL(KBufferMode,bufferMode);
			if(errCode == KErrNone)
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());

				result.Zero();
				if(bufferMode == 0)
					result.Append(KCircular);
				else if(bufferMode == 1)
					result.Append(KStraight);
					else
						result.Append(_L8("Invalid mode"));

				inputData->AppendNewData(data, (const void*)result.Ptr(), result.Length());
				}
			else
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				}
			}
		break; //EGetBufferMode
		
		case EGetDataNotificationSize:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetDataNotificationSize")
			#endif
			
			TInt dataNotification = 0;	
			errCode = GetBufandDataNotifyValuesL(KDataNotification,dataNotification);
			if(errCode == KErrNone)
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				TBuf8<32> b;
				b.Num(dataNotification);
				inputData->AppendNewData(data, (const void*)b.Ptr(), b.Length());
				}
			else
				{
				result.Num(errCode);
				data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
				}
			}
		break; //EGetDataNotificationSize
		
		case ERemovePrimaryFilter:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ERemovePrimaryFilter")
			for(TInt k=0; k<aArguments.Count(); k++)
				{
				__LOGBUF(aArguments[k])
				}
			#endif
			
			if(aArguments.Count() > 0)
				{
				RArray<TUint32> filterArray;
				for(TInt i=0; i<aArguments.Count(); i++)
					{
					TUint32 val;
					TLex8 lex(aArguments[i]);
					if(lex.Val(val, EDecimal) == KErrNone)
						filterArray.AppendL(val);
					}
				errCode = RemoveActiveFilter(filterArray,EPrimaryFilter);
				filterArray.Close();
				}
			else
				errCode = KErrArgument;
			
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break; //ERemovePrimaryFilter
		
		case ERemoveSecondaryFilter:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EDisableMultipleModuleUids")
			for(TInt k=0; k<aArguments.Count(); k++)
				{
				__LOGBUF(aArguments[k])
				}
			#endif
			
			if(aArguments.Count() > 0)
				{
				RArray<TUint32> filterArray;
				for(TInt i=0; i<aArguments.Count(); i++)
					{
					TUint32 val;
					TLex8 lex(aArguments[i]);
					if(lex.Val(val, EDecimal) == KErrNone)
						filterArray.AppendL(val);
					}
				errCode = RemoveActiveFilter(filterArray,ESecondaryFilter);
				filterArray.Close();
				}
			else
				errCode = KErrArgument;
			
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("errCode:")
			__LOGNUM(errCode)
			#endif
			
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break; //ERemoveSecondaryFilter
		
		case ESetPluginSettings:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ESetPluginSettings")
			#endif
				
			if(aArguments.Count() > 2)
				{
				TBuf8<128> plugin;
				plugin.Copy(aArguments[0]);
				aArguments.Remove(0);
				errCode = SetPluginSettings(plugin, aArguments);
				}
			else
				errCode = KErrArgument;
						
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break; //ESetChannelSettings
		
		case ESetActivePlugin:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ESetActiveOutputPlugin")
			#endif

			if(aArguments.Count() > 0)
				errCode = SetActiveOutputPlugin(aArguments[0]);
			else
				errCode = KErrArgument;

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break; //ESetActiveOutputPlugin

		case ESetActiveInputPlugin:
			{
			if(aArguments.Count() > 0)
				errCode = SetActiveInputPlugin(aArguments[0]);
			else
				errCode = KErrArgument;

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;
		
		case EDeactivateInputPlugin:
			{
			if(aArguments.Count() > 0)
				errCode = DeActivateInputPlugin(aArguments[0]);
			else
				errCode = KErrArgument;

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;

		case EGetActiveInputPlugin:
			{
			RArray<TPtrC8> tmpArray;
			errCode = GetActiveInputPlugin(tmpArray);
			result.Num(errCode);

			//remove ini specific data (number)
			//example (1 uloggerusbplugin) must look like (uloggerusbplugin)
			TInt i=0;
			while(i<tmpArray.Count())
				{
				TInt v = 0;
				TLex8 l(tmpArray[i]);
				if(l.Val(v) == KErrNone)
					tmpArray.Remove(i);
				else
					++i;
				}

			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			PrepareControlDataPayloadL(payloadBuf, tmpArray);
			if(payloadBuf.Length() > 0)
				inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
			tmpArray.Close();
			}
		break;
		
		case EGetInputPlugins:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("EGetControlPlugins")
			#endif
			
			RArray<TPtrC8> tmpArray;
			errCode = GetInstalledInputPluginsL(tmpArray);

			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());

			//create payload
			PrepareControlDataPayloadL(payloadBuf, tmpArray);
			//add playload to control data chunk
			if(payloadBuf.Length() > 0)
				inputData->AppendNewData(data, (const void*)payloadBuf.Ptr(), payloadBuf.Length());
			tmpArray.Close();
			}
		break;

		case ERestart:
			{
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("ERestartOutputting")
			#endif

			//ulogger shouldn't be restarted here as we must send acknowledgment back to PC
			//restart will be done in DoPostProcessing method after sending ack.
			result.Num(errCode);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
			}
		break;
		
		default:
			#if defined(__LIGHTLOGGER_ENABLED) && defined(__VERBOSE_MODE)
			__LOG("input command not supported")
			#endif
		
			result.Num(KErrNotSupported);
			data = inputData->CreatePackage((void*)result.Ptr(), result.Length());
		break;
	}//end of switch

	
	//cleanup
	if(inputData)
		{
		delete inputData;
		inputData = NULL;
		}
	payloadBuf.Close();

	return data;
	}



void CULoggerServer::DoPostProcessing(TCommand aCmd)
	{
	switch(aCmd)
		{
		case EStop:
			Stop();
			break;
		case ERestart:
			RestartOutputting();
			break;
		default:
			break;
		}
	}

/**
This function checks if array of given names contains any of real plugin names (either output of input).
After call aPluginList will be filtered to contain only proper plugin names (strings that really could be used 
to refer to a real plugin.
@param aFilter Either EOutputPluginFilter or EInputPluginFilter.
@param aPluginList Reference to an array which contain list of names that function has to check.
 */
void CULoggerServer::FilterPlugins(TPluginFilter aFilter, RArray<TPtrC8>& aPluginList)
	{
	TInt errCode = 0;
	CPlugin::TPluginInterface interfaceId;
	//assign filter value
	interfaceId = (aFilter == EOutputPluginFilter ? MOutputPlugin::iInterfaceId : MInputPlugin::iInterfaceId);
	
	//filter plugins
	TInt i=0;
	while(i<aPluginList.Count())
		{
		CPlugin *plugin = NULL;
		TRAP(errCode, plugin=CPlugin::NewL(aPluginList[i]));
		if(plugin && errCode==KErrNone)
			{
			TAny *ptr = NULL;
			TRAP(errCode, ptr=plugin->GetInterfaceL(interfaceId));
			if(!ptr || errCode!=KErrNone)
				{
				aPluginList.Remove(i--);
				}
			//cleanup
			if(plugin)
				{	
				delete plugin;
				plugin = NULL;
				}
			}
		++i;
		}
	}



TBool CULoggerServer::ServerState()
	{
	return iBtraceOpen;
	}


TBool CULoggerServer::CheckPluginExists(TPtrC8& aPluginName, TPluginFilter aPluginFilter)
	{
	RArray<TPtrC8> pluginsArray;
	if(aPluginFilter == EOutputPluginFilter)
		GetInstalledOutputPlugins(pluginsArray);
	else if(aPluginFilter == EInputPluginFilter)
		GetInstalledInputPluginsL(pluginsArray);

	for(TInt i=0; i<pluginsArray.Count(); ++i)
		if(aPluginName.Compare(pluginsArray[i])==0)
			{
			pluginsArray.Close();
			return ETrue;
			}
		
	pluginsArray.Close();
	return EFalse;
	}


}//end of namespace

#ifndef __ULOGGER_SERVER_UNIT_TEST
//Main Entry
TInt E32Main()
{
	return Ulogger::CULoggerServer::StartServer();
}
#endif //__ULOGGER_SERVER_UNIT_TEST
