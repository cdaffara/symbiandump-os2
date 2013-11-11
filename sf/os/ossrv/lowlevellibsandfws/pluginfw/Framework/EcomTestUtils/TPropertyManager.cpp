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

#include <e32base.h>
#include <e32def.h>
#include <e32property.h>
#include <e32debug.h>
#include "TPropertyManager.h"

_LIT(KSeparator, "|"); // Char used to separate arguments
_LIT(KPropertyManagerSrvName,"TPropertyManagerSrv");
_LIT(KPropertyManagerPanic,"TPropertyManager::SetProperty");

TInt PropertyManager::LaunchHelperProcess(TOperation aOperation,TDesC& aArgs)
	{
		TBuf<64> args;	
		args.AppendNum(aOperation);
		args.Append(KSeparator);
		args.Append(aArgs);
		
		// Property Manager Srv uid
		const TUid KSystemAgentExeUid = {0x10204FC5}; 
		const TUidType serverUid(KNullUid, KNullUid, KSystemAgentExeUid);
		RProcess server;
		TInt err = server.Create(KPropertyManagerSrvName, args, serverUid);
		if(err != KErrNone)
	        {
	        RDebug::Print(_L("Error Launching Property Manager..."));
			return err;
	        }
		
		RDebug::Print(_L("Property Manager Launched..."));
		TRequestStatus stat;
		server.Rendezvous(stat);
		if(stat != KRequestPending)
	        {
			server.Kill(0);		// abort startup
	        }
		else
	        {
			server.Resume();	// logon OK - start the server
	        }
		User::WaitForRequest(stat);		// wait for start or death
		// we can't use the 'exit reason' if the server panicked as this
		// is the panic 'reason' and may be '0' which cannot be distinguished
		// from KErrNone
		err = server.ExitType() == EExitPanic ? KErrGeneral : stat.Int();
		server.Close();
		
		RDebug::Print(_L("Property Manager Terminated..."));
		return err;

	}

EXPORT_C TInt PropertyManager::DefineProperty(TUid aCategory, TUint aKey, TInt aAttr,TInt aPreallocated)
	{	
	TBuf<64> args;
	args.AppendNum(aCategory.iUid);
	args.Append(KSeparator);
	args.AppendNum(aKey);
	args.Append(KSeparator);
	args.AppendNum(aAttr);
	args.Append(KSeparator);
	args.AppendNum(aPreallocated);
	
	return LaunchHelperProcess(EDefineProperty,args);
	}

EXPORT_C TInt PropertyManager::DeleteProperty(TUid aCategory, TUint aKey)
	{
	TBuf<64> args;
	args.AppendNum(aCategory.iUid);
	args.Append(KSeparator);
	args.AppendNum(aKey);
	
	return LaunchHelperProcess(EDeleteProperty,args);
	}

EXPORT_C TInt PropertyManager::SetProperty(TUid aCategory, TUint aKey, TInt aValue)
	{
	TBuf<64> args;
	args.AppendNum(aCategory.iUid);
	args.Append(KSeparator);
	args.AppendNum(aKey);
	args.Append(KSeparator);
	args.AppendNum(aValue);
	
	return LaunchHelperProcess(ESetPropertyInt,args);
	}

EXPORT_C TInt PropertyManager::SetProperty(TUid aCategory, TUint aKey, const TDesC8 &aValue)
	{
	TBuf<64> args;
	args.AppendNum(aCategory.iUid);
	args.Append(KSeparator);
	args.AppendNum(aKey);
	args.Append(KSeparator);
	
	//Need to check maximum length of this field....
	if((args.Size() + aValue.Size()) > args.MaxSize())
		{
		User::Panic(KPropertyManagerPanic,0);
		}
	
	//Convert the 8Bit descriptor into a 16 bit version
	TBuf<128> value;
	value.Copy(aValue);
	args.Append(value);
	
	return LaunchHelperProcess(ESetPropertyDes8,args);
	}

EXPORT_C TInt PropertyManager::SetProperty(TUid aCategory, TUint aKey, const TDesC16 &aValue)
	{	
	TBuf<64> args;
	args.AppendNum(aCategory.iUid);
	args.Append(KSeparator);
	args.AppendNum(aKey);
	args.Append(KSeparator);
	
	//Need to check maximum size of this field....
	if((args.Size() + aValue.Size()) < args.MaxSize())
		{
		User::Panic(KPropertyManagerPanic,0);
		}

	args.Append(aValue);
	
	return LaunchHelperProcess(ESetPropertyDes16,args);
	}
