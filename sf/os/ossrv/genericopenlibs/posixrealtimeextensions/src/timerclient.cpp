/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "timerutils.h"
#include "timerclient.h"
#include "timerhandler.h"
#include <errno.h>
#include <e32atomics.h>

// -------------------------------------------------------------------------------
// RTimerSession::Connect
// -------------------------------------------------------------------------------

TInt RTimerSession::Connect(RServer2 aServer)
    {
	TInt ret = CreateSession(aServer, TVersion(0, 0, 0), 1,EIpcSession_Sharable);
	return ret;
    }

//-------------------------------------------------------------------------------
// Function Name : 	RTimerSession::OnDemandConnect()
// Description   : 	To create connection to server on demand
//					
//-------------------------------------------------------------------------------	
TInt RTimerSession::OnDemandConnect(RServer2 aServer)
	{
	TInt err = KErrNone;
	//On Demand connection
	if ( !iIsConnected )
		{
		Lock();
		if ( !iIsConnected )
			{
			err = Connect (aServer);
			if ( KErrNone == err)
				{
				iIsConnected = ETrue;
				}
			}
		UnLock();
		}
	return err;
	}
//--------------------------------------------------------------------------------
//This sends a timer delete request to the server.
//--------------------------------------------------------------------------------

TInt RTimerSession::DeleteTimer(TInt aTimerId)
	{
	TInt ret = SendReceive(EDeleteTimer,TIpcArgs(aTimerId));
	return ret;
	}

//--------------------------------------------------------------------------------
//This sends a set timer request to the timer server.
//--------------------------------------------------------------------------------

TInt RTimerSession::SetTime(TInt aTimerId)
	{
	TInt ret = SendReceive(ESetTimer,TIpcArgs(aTimerId));
	return ret;
	}

