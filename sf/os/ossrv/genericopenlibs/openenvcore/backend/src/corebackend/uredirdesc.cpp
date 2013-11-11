// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <errno.h>
#include "fdesc.h"

CRedirDesc::CRedirDesc() : CFileDescBase(O_RDWR)
	{
	iReadNone = EFalse;
	iWriteNone = EFalse;
	iStatus = ENotConnected;
	iLock.CreateLocal();
	}
// -----------------------------------------------------------------------------
// CRedirDesc::Configure
// Implementation for Configure the Media i.e. connect to stdio server
// -----------------------------------------------------------------------------
//
TInt CRedirDesc::Configure()
	{
	TInt ret;
	TBuf8<1> aDes;
	iStatus = ENoServer;
	ret = iSession.Connect(); 
	if( KErrNone  == ret)
		{
		ret = iSession.CheckMedia(aDes);
		if( KErrNone == ret)
			{
			iStatus = EConnected;
			//In case both the media is NONE the server would fail the connection and 
			//this block of the code in not executed. If any of media either read or 
			//write in NONE specified in config.ini file then the iReadNone and iWriteNone
			//flag is set to ETrue which is check in read and write functions.
			if( !aDes.Compare(_L8("R")) )
				{
				iReadNone = ETrue;
				}
			else
				{
				if( !aDes.Compare(_L8("W")) )
					{
					iWriteNone = ETrue;	
					}
				}
			}
		}
	return ret;
	}
// -----------------------------------------------------------------------------
// CRedirDesc::Read
// Implementation for Read
// -----------------------------------------------------------------------------
//
void CRedirDesc::Read(TDes8& aDes, TRequestStatus& aStatus)
	{
	if( ENotConnected == iStatus )
		{
		iLock.Wait();
		if( ENotConnected == iStatus )
			{
			Configure();	
			}
		iLock.Signal();
		}
	if( EConnected == iStatus && !iReadNone )
		{
		iSession.Read(aStatus, aDes, aDes.MaxLength());
		}
	else 
		{
		// If no session is there i.e. no server in production code or in the
		// config.ini file for the stdioserver the both media type is NONE then
		// just complete the request with EOF so that libc can handle it.
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrEof);
		}
	
	}


TInt CRedirDesc::WriteCompletion(TDes8& aDesc, TInt /*aStatus*/)
	{
	return aDesc.Length();
	}
	
TInt CRedirDesc::ReadCompletion(TDes8& aDesc, TInt /*aStatus*/)
	{
	return aDesc.Length();
	}

// -----------------------------------------------------------------------------
// CRedirDesc::Write
// Implementation for Write
// -----------------------------------------------------------------------------
//
void CRedirDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{	
	if( ENotConnected == iStatus )
		{
		iLock.Wait();
		if( ENotConnected == iStatus )
			{	
			Configure();	
			}
		iLock.Signal();
		}
	if(EConnected  == iStatus && !iWriteNone)
		{
		iSession.Write(aStatus, aDesc, aDesc.Length());		
		}
	else
		{
		// If no session is there i.e. no server in production code or the user
		// has specified the media type in config.ini file as NONE put the write
		// data in blackhole and just complete the request as done in Read call.
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

// -----------------------------------------------------------------------------
// CRedirDesc::Fcntl
// Implementation for fcntl
// -----------------------------------------------------------------------------
//
TInt CRedirDesc::Fcntl(TUint /*anArg*/, TUint aCmd)
	{
	if (aCmd == F_GETFL)
		{
		return iFcntlFlag;
		}
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CRedirDesc::FinalClose
// Cleanup function used by FID
// -----------------------------------------------------------------------------
//
TInt CRedirDesc::FinalClose()
	{
    iLock.Close();
	iSession.Close();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CRedirDesc::Poll
// checks if that aEvents is ready or not.
// -----------------------------------------------------------------------------
//
TInt CRedirDesc::Poll(TUint aEvents)
	{
	TInt readyEvents = 0;

	if ((aEvents & EReadyForWriting ) && !iWriteNone)
		{
		readyEvents |= EReadyForWriting;
		}
	
	return readyEvents;
	}
	
// -----------------------------------------------------------------------------
// CRedirDesc::NotifyActivity
// registers a notification for read event i.e. when data is ready for read.
// this function is only for reading because poll itself would return for writing
// if that media for writing is available.
// -----------------------------------------------------------------------------
//
TInt CRedirDesc::NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 /* timeout=0 */)
	{
	if( ENotConnected == iStatus )
		{
		iLock.Wait();
		if( ENotConnected == iStatus )
			{
			Configure();	
			}
		iLock.Signal();
		}

	if(EConnected  != iStatus || aEvents & EReadyForWriting || iReadNone )
		{
		//complete aStatus with KErrNone
		TRequestStatus* status = &aRequest;
		User::RequestComplete(status, KErrNone);
		}
	
	if ((aEvents & EReadyForReading) && !iReadNone)
		{		
		TInt events = 0;
		events = EReadyForReading;
		iSession.NotifyActivity((TInt)events, aRequest);
		}
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CRedirDesc::TweakReadyEvents
// Prepares the redir desc specific output events
// -----------------------------------------------------------------------------
//
TInt CRedirDesc::TweakReadyEvents(TInt errval)
    {
    TInt returnEvents = 0;
    // This file descriptor is pipe-like 
    if( errval >= KErrNone )
        {
        returnEvents |= EReadyForReading;
        returnEvents |= EReadyForWriting;
        }
    else
        {
        returnEvents |= EAnyException;
        }
    return returnEvents;
    }

// -----------------------------------------------------------------------------
// CRedirDesc::CancelNotify
// To cancel notification.
// -----------------------------------------------------------------------------
//
void CRedirDesc::CancelNotify()
	{
	if ( iSession.Handle() )
		{
		iSession.CancelNotify();
		}
	}

//-----------------------------------------------------------------------------
//int CRedirDesc::SetEcho(TBool aEcho)
//
//Sets the echo flag to the given value.
//-----------------------------------------------------------------------------

int CRedirDesc::SetEcho(TUint8 aEcho)
	{
	int err = KErrNone;
	//start the redirection server if not already started...
	if( ENotConnected == iStatus )
		{
		iLock.Wait();
		Configure();	
		iLock.Signal();
		}	
	//set the echo flag if the connection with the server is successfull...
	if(EConnected  == iStatus)
		{
		err = iSession.SetEcho(aEcho);
		}
	else
		{
		// If no session is there i.e. no server in production code or in the
		// config.ini file for the stdioserver the both media type is NONE then
		// just return EOF so that libc can handle it.
		err = KErrEof;
		}
	return err;
	}
	

// method definitions for CStdErrDesc
CStdErrDesc::CStdErrDesc() : CFileDescBase(O_RDWR)
	{
	// nada
	}

// -----------------------------------------------------------------------------
// CStdErrDesc::Write
// Implementation for Write
// -----------------------------------------------------------------------------
//
void CStdErrDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	HBufC16* des16 = NULL;
	TRAPD(err, des16 = HBufC16::NewL(aDesc.Length()));
	if (err != KErrNone)
		{
		//complete aStatus with that error code
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		return;
		}
	TPtr16 ptr(des16->Des());
	ptr.Copy(aDesc);
	#ifdef _DEBUG
	RDebug::RawPrint(*des16);
	#endif //_DEBUG
	delete des16;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

// -----------------------------------------------------------------------------
// CStdErrDesc::Fcntl
// Implementation for fcntl
// -----------------------------------------------------------------------------
//
TInt CStdErrDesc::Fcntl(TUint /*anArg*/, TUint aCmd)
	{
	if (aCmd == F_GETFL)
		{
		return iFcntlFlag;
		}
	return KErrNotSupported;
	}

TInt CStdErrDesc::WriteCompletion(TDes8& aDesc, TInt /*aStatus*/)
	{
	return aDesc.Length();
	}
