// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COMSDBG_INL__
#define __COMSDBG_INL__

#include <comms-infras/commsdebugutility.h>

//
// Inline functions
//

inline RFileLoggerBody::RFileLoggerBody() : RSessionBase()
	{
	iLoggingOnPckg()=KLoggingOnOffDefault;
	}

inline TInt RFileLoggerBody::DoCreateSession(const TDesC& aServer,const TVersion& aVersion,TInt aAsyncMessageSlots)
	{
	return CreateSession(aServer,aVersion,aAsyncMessageSlots);
	}

inline TInt RFileLoggerBody::DoSendReceive(TInt aFunction,const TIpcArgs& aArgs) const
	{
	return SendReceive(aFunction,aArgs);
	}

inline TInt RFileLoggerBody::DoSendReceive(TInt aFunction) const
	{
	return SendReceive(aFunction);
	}

inline void RFileLoggerBody::Close()
	{
	RSessionBase::Close();
	}
	
inline TBool RFileLogger::IsLogging()
	{
	if (iLoggerBody)
		{
		return iLoggerBody->iLoggingOnPckg();
		}
	else
		{
		return EFalse;	
		}
	}

inline void CSecondaryThread::SignalRequestSemaphore()
/**
 * Increment the semaphore count to match the number of msgs in the queue.
 *
 * We need to signal to the slave thread that we now have work for it.
 * We could have added a semaphore to do this, but there is already one
 * in the second thread that isn't being used: the request semaphore.
 * So, instead we will use that since it is also faster. But the code
 * looks a bit back-to-front since we are calling "RequestComplete" when
 * we are making the request. We also have to pass in a dummy status object
 * since in normal practice this would be part of the request completion process.
 */
	{
	TRequestStatus dummy;
	TRequestStatus* p = &dummy;
	iSecondaryThread.RequestComplete(p,KErrNone);
	}

#endif // __COMSDBG_INL__

