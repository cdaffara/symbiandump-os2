/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "sntpclientengine.h"
#include "util.h"

// 40 second timeout on operations
#define SNTP_ENGINE_TIMEOUT 40000000 

// NTP port

#define SNTP_REMOTE_PORT 123

_LIT(KNTPEpochDate,"19000000:");

/* The simplest possible NTP request */

static const TUint8 sntpRequest[48] = {
	0x23, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00 };
	
/* The main engine of the SNTP client */

CSNTPClient* CSNTPClient::NewL(TCommandLineArgs& aArgs)
	{
	CSNTPClient* self = CSNTPClient::NewLC(aArgs);
	CleanupStack::Pop(self);
	return self;
	}
	
CSNTPClient* CSNTPClient::NewLC(TCommandLineArgs& aArgs)
	{
	CSNTPClient* self = new (ELeave) CSNTPClient(aArgs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
TSNTPClientState CSNTPClient::State()
	{
	return iState;
	}
	
void CSNTPClient::Start()
	{
	
	iState = EStateResolve;
	iResolver.GetByName(*(iArgs.iServers[iServerIndex]), iNameEntry, iStatus);
	SetActive();
	iTimer->After(SNTP_ENGINE_TIMEOUT);
	
	}
	
CSNTPClient::~CSNTPClient()
	{
	Cancel();
	iResolver.Close();
	iSock.Close();
	iSockServ.Close();
	
	delete iTimer;
	}
	
CSNTPClient::CSNTPClient(TCommandLineArgs& aArgs)
	: CActive(EPriorityStandard), iArgs(aArgs)
	{
	}
	
void CSNTPClient::ConstructL()
	{
	User::LeaveIfError(iSockServ.Connect());
	User::LeaveIfError(iSock.Open(iSockServ, KAfInet, KSockDatagram, KProtocolInetUdp));
	User::LeaveIfError(iResolver.Open(iSockServ, KAfInet, KProtocolInetUdp));
	
	iTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	CActiveScheduler::Add(this);
	}
	
void CSNTPClient::RunL()
	{
	
	if (iStatus.Int() < 0)
		{
		User::Leave(iStatus.Int());
		}

	switch (iState)
		{
	
	case EStateResolve:
		iTimer->Cancel();
		iBuffer.Zero();
		iBuffer.Append(sntpRequest, 48);
	
		// set the port on the address
		iNameEntry().iAddr.SetPort(SNTP_REMOTE_PORT);
	
		iState = EStateWrite;
		iSock.SendTo(iBuffer, iNameEntry().iAddr, 0, iStatus);
		SetActive();
		iTimer->After(SNTP_ENGINE_TIMEOUT);
		break;
	
	case EStateWrite:
		iTimer->Cancel();
		iState = EStateRead;
		iBuffer.Zero();
		iSock.RecvFrom(iBuffer, iNameEntry().iAddr, 0, iStatus);
		SetActive();
		iTimer->After(SNTP_ENGINE_TIMEOUT);
		break;
		
	case EStateRead:
		{
		iTimer->Cancel();
		SetTimeL();
		iStatus = KRequestPending;
		iState = EStateComplete;
		TRequestStatus* status = &iStatus;
		SetActive();
		User::RequestComplete(status, KErrNone);
		break;
		}
		
	case EStateComplete:
		CActiveScheduler::Stop();
		// done
		break;
		
	default:
		// wuh oh. BC break!
		User::Leave(KErrArgument);
		break;
		
		}
	
	}
	
void CSNTPClient::DoCancel()
	{
	
	iTimer->Cancel();

	switch (iState)
		{
	case EStateResolve:
		iResolver.Cancel();
		break;
	case EStateWrite:
		iSock.CancelSend();
		break;
	case EStateRead:
		iSock.CancelRecv();
		break;
		}
		
	}
	
	
TInt CSNTPClient::RunError(TInt /* aError */)
	{
	// The current server didn't work, lets try the next if available.
	iTimer->Cancel();
	
	if (++iServerIndex < iArgs.iServers.Count())
		{
		Start();
		}
	else
		{
		iState = EStateFailed;
		CActiveScheduler::Stop();
		}
	return KErrNone;
	}
	
void CSNTPClient::TimerExpired()
	{
	Cancel();
	
	// The current server didn't work, lets try the next if available.
	
	if (++iServerIndex < iArgs.iServers.Count())
		{
		Start();
		}
	else
		{
		iState = EStateAborted;
		CActiveScheduler::Stop();
		}
	}
	
void CSNTPClient::SetTimeL()
	{
	
	TUint32 timestamp(0);
	
	/* Use the seconds from the transmit time field 
	   
	 */
	
	for (TInt i = 40; i < 44; ++i)
		{
		timestamp = (timestamp << 8) + iBuffer[i];
		}
	
	// Obtain the time, including the specified timezone offset
	
	TTimeIntervalMinutes mins(timestamp / 60);
	TTimeIntervalSeconds secs(timestamp % 60);
	
	TTime ntpTime;
	User::LeaveIfError(ntpTime.Set(KNTPEpochDate));
	ntpTime += mins;
	ntpTime += secs;
	
	// Apply offset and (possibly) daylight savings time
	
	TTimeIntervalHours hours;
	
	if (iArgs.iApplyDaylightSavings && Util::DaylightSavingsAppliesL(ntpTime))
		{
		hours = iArgs.iOffset + 1;
		}
	else
		{
		hours = iArgs.iOffset;
		}
		
	ntpTime += hours;
	
	User::LeaveIfError(User::SetHomeTime(ntpTime));
	
	}
	
	
/* Timeout handler for read/write operations */

CTimeOutTimer::CTimeOutTimer(const TInt aPriority)
    : CTimer(aPriority)
    {
    }

CTimeOutTimer::~CTimeOutTimer()
    {
	Cancel();
    }

CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
    {
    CTimeOutTimer *p = new (ELeave) CTimeOutTimer(aPriority);
    CleanupStack::PushL(p);
	p->ConstructL(aTimeOutNotify);
	CleanupStack::Pop();
    return p;
    }

void CTimeOutTimer::ConstructL(MTimeOutNotify &aTimeOutNotify)
    {
	iNotify=&aTimeOutNotify;
	CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

void CTimeOutTimer::RunL()
// Timer request has completed, so notify the timer's owner
    {
	iNotify->TimerExpired();
	}
