// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : server.cpp
// Part of     : ipc server
// To implement Utilities class for ipc objects
// 
//

#include <e32base.h>
#include <e32math.h>
#include "ipcutils.h"

//-------------------------------------------------------------------------------
// Function Name : 	GetPid
// Description   : 	Utility function to get the process id of the senders request
//	                via RMessage2
//-------------------------------------------------------------------------------	

TInt IpcUtils::GetPid(const RMessage2& aMessage)
	{
	RThread thread;
	RProcess process;
	TInt ret;
	ret = aMessage.Client(thread);
	if(ret != KErrNone)
		{
		return -1;
		}
	ret = thread.Process(process);
	if(ret != KErrNone)
		{
		return -1;
		}
	ret = process.Id();
	thread.Close();
	process.Close();
	return ret;
	}


//-------------------------------------------------------------------------------
// Function Name : 	GetPid
// Description   : 	Utility function to get the process id of the senders request
//	                via RMessage2
//-------------------------------------------------------------------------------	
TInt IpcUtils::CurrentTimeL()
	{
	TTime t;
	t.UniversalTime();
	TTimeIntervalSeconds res;
	User::LeaveIfError( t.SecondsFrom(UNIX_BASE, res) );
	return res.Int();
	}
	

//-------------------------------------------------------------------------------
// Function Name : 	IpcUtils::GetRandom()
// Description   : 	Static function to get the random sequence of number which 
//					are be used for IPC 
//-------------------------------------------------------------------------------	

TInt64 IpcUtils::iSeed = Math::Random( );

TInt IpcUtils::GetRandom()
	{
	return Math::Rand( IpcUtils::iSeed );
	}

		

