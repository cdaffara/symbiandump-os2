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

#include "uloggerclient.h"
#include "uloggershared.h" 
#include "uloggertools.h"
#include "uloggercommands.h"
#include <e32math.h>
#include <e32base.h>
#include <badesca.h>


namespace Ulogger
{
//declarations

static const TUint KMessageSlots = 32;

///////////////////////////////////////////////////////////////////
//
// Definition of RULogger methods...
//
///////////////////////////////////////////////////////////////////

//constructor
EXPORT_C RULogger::RULogger()
	{

	}//</constructor>

//destructor
EXPORT_C RULogger::~RULogger()
	{
	Close();
	}//</destructor>

EXPORT_C TInt RULogger::Connect()
	{
	TInt retVal = StartServer();

	if((KErrNone == retVal) || (KErrAlreadyExists == retVal))
		{
		retVal = CreateSession(KServerName, this->Version(), KMessageSlots);
		}

	return retVal;	
	}

EXPORT_C TInt RULogger::RunAsService(TBool /*aRunAsService*/)
	{
	TInt retVal;

	retVal = KErrNotSupported; // Replace with SendReceive call once functionality implemented in server
//
// // Replace above with following, once functionality implemented in server:
//
//	if (aRunAsService)
//		retVal = SendReceive(ERunAsService);
//	else
//		retVal = SendReceive(EDontRunAsService);
//

	return retVal;
	}

EXPORT_C TInt RULogger::Start()
	{
	return SendReceive(EStart);	
	}

EXPORT_C TInt RULogger::Stop()
	{
	return SendReceive(EStop);
	}

EXPORT_C TInt RULogger::Restart()
	{
	return SendReceive(ERestart);
	}

EXPORT_C TInt RULogger::SetPrimaryFiltersEnabled(const CArrayFixFlat<TUint8>& aFilters, TBool aEnabled)
	{
	TInt retVal = KErrNone;

	//externalize filters array to string to send it in either a
	//ESetPrimaryFilter or ERemovePrimaryFilter message, but only if at least
	//one filter was actually specified in the filters array
	if(aFilters.Count())
		{
		//prepare data to send
		HBufC8 *des  = NULL;
		TRAP(retVal, des = ExternalizeToBufL((const CArrayFix<TUint8>&) aFilters, sizeof(TUint8)));
		if(KErrNone == retVal)
			{
			//send data
			TPtr8 dataPtr(des->Des());
			TIpcArgs args(&dataPtr, aFilters.Count());
			if (aEnabled)
				retVal = SendReceive(ESetPrimaryFilter, args);
			else
				retVal = SendReceive(ERemovePrimaryFilter, args);
			}

		delete des;
		}
	else
		retVal = KErrArgument;
		
	/**r
	if(retVal == KErrAlreadyExists)
		retVal = KErrNone;
		*/
	return retVal;
	}

EXPORT_C TInt RULogger::GetPrimaryFiltersEnabled(CArrayFixFlat<TUint8>& aFilters)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(256);
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args(&ptr);
		retVal = SendReceive(EGetPrimaryFilters, args);
		if (KErrNone == retVal)
			{
			TUint8 tmp = 1;
			TRAP(retVal, InternalizeFromBufL(*buf, aFilters, tmp));
			}

		delete buf;
		buf = NULL;
		}
	else
		{
		return KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::SetSecondaryFiltersEnabled(const RArray<TUint32>& aFilters, TBool aEnabled)
	{
	TInt retVal = KErrNone;

	//ensure max secondary filters limit isn't exceeded, otherwise return error
	if (aFilters.Count() > KMaxSecondaryFiltersLimit)
		{
		retVal = KErrArgument;
		}
	else
		{
		//externalize filters array to string to send it in either a
		//ESetSecondaryFilter or ERemoveSecondaryFilter message, but only if at least
		//one filter was actually specified in the filters array
		if(aFilters.Count())
			{
			//prepare data to send
			HBufC8 *des = NULL;
			
			TRAP(retVal, des = ExternalizeToBufL(aFilters, sizeof(TUint32)));
			if(KErrNone == retVal)
				{
				//send data
				TPtr8 dataPtr(des->Des());
				TIpcArgs args(&dataPtr, aFilters.Count());
				if (aEnabled)
					retVal = SendReceive(ESetSecondaryFilter, args);
				else
					retVal = SendReceive(ERemoveSecondaryFilter, args);
				}

			//clean up string if necessary
			delete des;
			}
		else
			retVal = KErrArgument;
		}
	
	return retVal;
	}

EXPORT_C TInt RULogger::GetSecondaryFiltersEnabled(RArray<TUint32>& aFilters)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(KMaxSecondaryFiltersLimit*sizeof(TUint32));
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args(&ptr);
		retVal = SendReceive(EGetSecondaryFilters, args);
		if(KErrNone == retVal)
			{
			TUint32 tmp=1;
			TRAP(retVal, InternalizeFromBufL(*buf, aFilters, tmp));
			}

		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::SetSecondaryFilteringEnabled(TBool aEnabled)
	{
	TInt retVal;

	if (aEnabled)
		retVal = SendReceive(EEnableSecondaryFiltering);
	else
		retVal = SendReceive(EDisableSecondaryFiltering);

	return retVal;
	}

EXPORT_C TInt RULogger::GetSecondaryFilteringEnabled(TBool& aEnabled)
	{
	//this code is a copy of GetModuleUidFiltering && GetBuffer && Notification etc etc....
	//should put all of it in a separate method...
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(200);
	if(buf)
		{
		//send request to server
		TPtr8 ptr(buf->Des());
		TIpcArgs args(&ptr);
		retVal = SendReceive(EGetSecondaryFiltering, args);

		if(!retVal)
			{
			TLex8 val(ptr);
			retVal = val.Val(aEnabled);
			}
		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::ActivateOutputPlugin(const TDesC8& aPluginName)
	{
	TIpcArgs iArgs; //Ipc arguments
	iArgs.Set(0, &aPluginName);
	return SendReceive(ESetActivePlugin, iArgs);
	}

EXPORT_C TInt RULogger::GetActiveOutputPlugin(TDes8& aPluginName)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(2048); // Need to push on stack in that case
	// does it matter if its 8 or 16 buf?
	if(buf)
		{
		//send get active output plug-in message
		TPtr8 ptr(buf->Des());
		TIpcArgs args;
		args.Set(0, &ptr);
		retVal = SendReceive(EGetActivePlugin, args);
		
		//parse response buffer
		TInt pos = 0;
		if(pos != KErrNotFound)
			{
			pos = buf->Des().Find(KSeparator); // Find separator
			//just don't send the separator?
			if(pos >= 0)
				{
				TPtrC8 ptrVal(buf->Des().Left(pos));
				if (aPluginName.MaxLength() < pos)
					retVal = KErrOverflow;
				else
					aPluginName.Copy(ptrVal);
				}
			}

		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::GetInstalledOutputPlugins(CArrayPtrFlat<HBufC8>& aPluginNames)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(2048);
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args;
		args.Set(0, &ptr);
		retVal = SendReceive(EGetInstalledPlugins, args);

		//parse buffer
		TInt pos = KErrNone;
		TInt err = KErrNone;
		while(pos != KErrNotFound)
			{
			pos = buf->Des().Find(KSeparator);
			if(pos > 0)	
				{
				TPtrC8 ptrVal(buf->Des().Left(pos));
				HBufC8 *bufDes = HBufC8::New(ptrVal.Length()+8);
				if(bufDes)
					{
					bufDes->Des().Copy(ptrVal);
					TRAP(err, aPluginNames.AppendL(bufDes));
					}
				buf->Des().Delete(0,pos+1);
				}
			}

		delete buf;
		buf = NULL;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::ActivateInputPlugin(const TDesC8& aPluginName)
	{
	TIpcArgs iArgs;
	iArgs.Set(0, &aPluginName);
	return SendReceive(ESetActiveInputPlugin, iArgs);
	}

EXPORT_C TInt RULogger::GetActiveInputPlugin(TDes8& aPluginName)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(1024);// Push on stack in that case
	if(buf)
		{
		//send get active input plug-in message
		TPtr8 ptr(buf->Des());
		TIpcArgs args;
		args.Set(0, &ptr);
		retVal = SendReceive(EGetActiveInputPlugin, args);

		//parse response buffer
		TInt pos = KErrNone;
		if(pos != KErrNotFound)
			{
			pos = buf->Des().Find(KSeparator);
			if(pos >= 0)
				{
				buf->Des().Delete(0, pos+1); // Get rid of media number + separator
				pos = buf->Des().Find(KSeparator); // Find separator after plug-in name

				if(pos >= 0)	
					{
					TPtrC8 ptrVal(buf->Des().Left(pos));
					if (aPluginName.MaxLength() < pos)
						retVal = KErrOverflow;
					else
						aPluginName.Copy(ptrVal);
					}
				}
			}

		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::DeActivateInputPlugin()
	{
	TInt retVal = KErrNone;

	//get currently active input plug-in name
	TBuf8<KMaxPath> activePluginName;
	retVal = GetActiveInputPlugin(activePluginName);

	//deactivate currently active input plug-in
	if (KErrNone == retVal)
		{
		TIpcArgs iArgs;
		iArgs.Set(0, &activePluginName);
		retVal = SendReceive(EDeactivateInputPlugin, iArgs);
		}

	return retVal;
	}

EXPORT_C TInt RULogger::GetInstalledInputPlugins(CArrayPtrFlat<HBufC8>& aPluginNames)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(2048);//Push on stack in that case
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args;
		args.Set(0, &ptr);
		retVal = SendReceive(EGetInputPlugins, args);

		//parse buffer
		TInt pos = KErrNone;
		TInt err = KErrNone;
		while(pos != KErrNotFound)
			{
			pos = buf->Des().Find(KSeparator);
			if(pos > 0)	
				{
				TPtrC8 ptrVal(buf->Des().Left(pos));
				HBufC8 *bufDes = HBufC8::New(ptrVal.Length()+8);//push on stack
				if(bufDes)
					{
					bufDes->Des().Copy(ptrVal);
					TRAP(err, aPluginNames.AppendL(bufDes));
					}
				buf->Des().Delete(0,pos+1);
				}
			}

		delete buf;
		buf = NULL;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::SetPluginConfigurations(const TDesC8& aPluginName, const TPluginConfiguration& aConfiguration)
	{
	TInt retVal = KErrNone;

	if(aConfiguration.Key().Length()==0||aConfiguration.Value().Length()==0)
		return KErrArgument;

	//count total_length + separators
	TInt length = 1;
	length+=aConfiguration.Key().Length()+aConfiguration.Value().Length()+2;

	HBufC *configs = HBufC::New(length);// Push on stack in that case
	if(configs)
		{
		TPtr formatConfigs(configs->Des());
		formatConfigs.AppendFormat(KConfigFormat, &(aConfiguration.Key()), &(aConfiguration.Value()));
		HBufC8 *settings= HBufC8::NewLC(configs->Length());//Push on stack in that case
		if(settings)
			{
			settings->Des().Copy(configs->Des());
			TPtr8 arg2(settings->Des());

			TIpcArgs args; //Ipc arguments
			args.Set(0, &aPluginName);
			args.Set(1, &arg2);
			retVal = SendReceive(ESetPluginSettings,args);
			CleanupStack::PopAndDestroy(settings); 
			}
		else
			return KErrNoMemory;

		delete configs;
		}
	else
		return KErrNoMemory;

	return retVal;
	}

EXPORT_C TInt RULogger::GetPluginConfigurations(const TDesC8& aPluginName, RPointerArray<TPluginConfiguration>& aConfigurations)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(2048);//Push on stack in that case
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		HBufC8 *mediaBuf = HBufC8::New(1+aPluginName.Length());// push on stack if we need to alloc this. Don't hardcode!!
		if(mediaBuf)
			{
			mediaBuf->Des().Copy(aPluginName);
			TPtr8 mediaPtr(mediaBuf->Des());

			//send data
			TIpcArgs args;
			args.Set(0, &mediaPtr);
			args.Set(1, &ptr);
			retVal = SendReceive(EGetPluginSettings, args);

			//parse received buffer
			TInt pos = buf->Des().Find(KSeparator);
			while((pos != KErrNotFound) && (retVal == KErrNone))
				{
				TPluginConfiguration* pluginConfig = new TPluginConfiguration();// Push on stack in that case
				//memory leak? not always deleted?
				if (pluginConfig)
					{
					TPtrC8 ptrKey(buf->Des().Left(pos));
					pluginConfig->SetKey(ptrKey);
					buf->Des().Delete(0, pos+1);
					pos = buf->Des().Find(KSeparator);
					if (pos != KErrNotFound)
						{
						TPtrC8 ptrVal(buf->Des().Left(pos));
						pluginConfig->SetValue(ptrVal);
						buf->Des().Delete(0, pos+1);
						retVal = aConfigurations.Append(pluginConfig);
						}
					else
						{
						delete pluginConfig; // Ignore trailing key
						}
					}
				else
					{
					retVal = KErrNoMemory;
					}
				pos = buf->Des().Find(KSeparator);
				}

			delete mediaBuf;
			mediaBuf = NULL;
			}
		else
			{
			retVal = KErrNoMemory;
			}

		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::RemovePluginConfigurations(const TDesC8& aPluginName)	
	{
	TInt retVal = KErrNone;
	HBufC8 *mediaBuf = HBufC8::New(1+aPluginName.Length());// Push on stack at least
	if(mediaBuf)
		{
		mediaBuf->Des().Copy(aPluginName);
		TPtr8 mediaPtr(mediaBuf->Des());

		//send data
		TIpcArgs args;
		args.Set(0, &mediaPtr);	
		retVal = SendReceive(ERemovePluginSettings, args);
		delete mediaBuf;
		mediaBuf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::SetBufferSize(TInt aSize)
	{
	TInt retVal;

	if( (aSize > KMaxBufferSize) || (aSize < 1) )
		{
		retVal = KErrArgument;
		}
	else
		{
		retVal = SendReceive(EResizeTraceBuffer, TIpcArgs(aSize));
		}

	return retVal;
	}

EXPORT_C TInt RULogger::GetBufferSize(TInt& aSize)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(32);// push on stack if we need to alloc this. Don't hardcode!!
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args(&ptr);
		retVal = SendReceive(EGetTraceBufferSize,args);	
		if(retVal == KErrNone )
			{
			TLex8 val(ptr);
			retVal = val.Val(aSize);
			}
		delete buf;
		buf = NULL;
		}
	else
		return KErrNoMemory;
	
	return retVal;
	}

EXPORT_C TInt RULogger::SetNotificationSize(TInt aSize)
	{
	TInt retVal;

	if( (aSize > KMaxBufferSize) || (aSize < 0) )					
		{
		retVal =  KErrArgument;
		}
	else
		{
		retVal = SendReceive(ESetDataNotificationSize, TIpcArgs(aSize));
		}

	return retVal;
	}

EXPORT_C TInt RULogger::GetNotificationSize(TInt& aSize)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(32);// push on stack if we need to alloc this. Don't hardcode!!
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args(&ptr);
		retVal = SendReceive(EGetDataNotificationSize,args);	
		if(retVal == KErrNone )
			{
			TLex8 val(ptr);
			retVal = val.Val(aSize);
			}
		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::SetBufferMode(TInt aMode)
	{
	TInt retVal;

	if( (aMode == EStraightBuffer) || (aMode==ECircularBuffer) )					
		{
		retVal = SendReceive(ESetBufferMode, TIpcArgs(aMode));
		}
	else
		{
		retVal = KErrArgument;
		}

	return retVal;
	}

EXPORT_C TInt RULogger::GetBufferMode(TInt& aMode)
	{
	TInt retVal = KErrNone;

	HBufC8 *buf = HBufC8::New(32);// push on stack if we need to alloc this. Don't hardcode!!
	if(buf)
		{
		TPtr8 ptr(buf->Des());
		TIpcArgs args(&ptr);
		retVal = SendReceive(EGetBufferMode,args);	
		if(retVal == KErrNone)
			{
			TLex8 val(ptr);
			retVal = val.Val(aMode);
			}
		delete buf;
		buf = NULL;
		}
	else
		{
		retVal = KErrNoMemory;
		}

	return retVal;
	}

EXPORT_C TVersion RULogger::Version()
	{
	return TVersion(KULoggerSrvMajorVersionNumber,KULoggerSrvMinorVersionNumber,KULoggerSrvBuildVersionNumber);
	}

/**
Internal Function - Starts the server
*/
TInt RULogger::StartServer()
	{
	TInt retVal = KErrNone;

	TFullName serverName;
	TFindServer serverFinder(KServerName);
	retVal = serverFinder.Next(serverName);
	if(KErrNone == retVal)
		{
		retVal = KErrAlreadyExists;
		}
	else
		{
		//RProcess
		RProcess server;
		retVal = server.Create(KServerName, _L(""), EOwnerProcess);
		if(KErrNone == retVal)
			{
			// Synchronise with the server
			TRequestStatus reqStatus;
			server.Rendezvous(reqStatus);

			// Start the test harness
			server.Resume();
			// Server will call the reciprocal static synchronise call
			User::WaitForRequest(reqStatus);
			server.Close();
			retVal = reqStatus.Int();
			}
		}

	return retVal;
	}//</StartServer>

} // namespace
