// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "srvPerf.h"

#ifdef __CENTREP_SERVER_PERFTEST__

#include "srvreqs.h"
#include "srvsubsess.h"

// SessionClose
// Decrement reference count. If no more active sessions
// then advance iLastCompleteAccess
void TCentRepPerfTest::SessionClose()
	{
	iActiveSessionCount--;
	// There are clients who never close their sessions. Hence
	// cannot use iActiveSessionCount == 0 as indication to copy
	// iCount to iLastCompleteAccess.

	if (!IsFinished())
		{
		iLastCompleteAccess = iCount;
		}
	}

// Append
// Add the integer if iEntries array is not full yet.
TInt TCentRepPerfTest::Append(TUint32 aEntry)
	{
	if (iCount < KCentRepPerfTestArraySize)
		{
		iEntries[iCount++] = aEntry;
		return KErrNone;
		}
	return KErrOverflow;
	}
	
// Function EncodeEventAndData
// Put event Id in upper 8 bit and data in lower 24 bit.
inline
TUint32 EncodeEventAndData(TUint aEventId, TUint32 aData)
	{
	return ((aEventId & KEventIdMask) << KEventIdShiftBits) |
		   (aData & KDataMask);
	}

// Save initial values of timer
void TCentRepPerfTest::DoEventStart(CServerSubSession* aSubSession, const TClientRequest& /*aMessage*/)
	{
	if(IsFinished())
		{
		return;
		}
	
	//set the uid of the repository that this operation will be 
	//performed on as the current repository
	if(aSubSession) //aSubSession would be NULL in the case of EInitialise and EClose operations
		{
		SetCurrentRepositoryUid(aSubSession->RepositoryUid().iUid);
		}
	
	iStartTick = User::FastCounter();
	}

// Save initial values of timer
void TCentRepPerfTest::DoServerStart()
	{
	iStartTick = User::FastCounter();
	}	
	
// store time of event, etc. in performance array
void TCentRepPerfTest::DoServerEnd()
	{
	TUint32 endTick = User::FastCounter();

	TServerRequest fn = EInitialiseServer;
	TUint32 entry = EncodeEventAndData(fn, endTick - iStartTick);
	Append(entry);
	}

// store time of event, etc. in performance array
void TCentRepPerfTest::DoEventEnd(CServerSubSession* /*aSubSession*/, const TClientRequest& aMessage)
	{
	TUint32 endTick = User::FastCounter();

	if (IsFinished())
		{
		return;
		}

	TServerRequest fn = static_cast<TServerRequest>(aMessage.Function());
	// Performance data has 3 parts. First: time spent to
	// service the request. 2nd if event is open/close/evict
	// time of the event. 3rd, if open/close/evict repository UID

	// First part: event ID and CPU time spent servicing the request
	TUint32 entry = EncodeEventAndData(fn, endTick - iStartTick);
	Append(entry);

	if (fn == EInitialise)
		{
		SessionOpen();
		
		//store the repository UID in the subsession so that we can refer to it later
		SetCurrentRepositoryUid(aMessage.Int0());
		}

	//get the uid of the repository that this operation is being performed on
	TUint32 currentRepositoryUid = CurrentRepositoryUid();
	
	TBool repOpenCloseEvict = fn == EInitialise || fn == EClose || fn == EEvict;

	// Third part: repository UID if event is open/close/evict.
	if (repOpenCloseEvict)
		{
		Append(iStartTick);
		Append(currentRepositoryUid);  // Append will check if array IsFull.
		}

	// One last thing to do: check if all concurrent sessions close.
	if (fn == EClose)
		{
		SessionClose();

		// This is a temporary solution to detect end-of-boot until SS code is 
		// modified to return end-of-boot state and Centrep becomes SSA 
		if (currentRepositoryUid == 0xCCCCCC00)
			{
			IncEndOfBoot();
			}
		}
	}
	

#endif //__CENTREP_SERVER_PERFTEST__

