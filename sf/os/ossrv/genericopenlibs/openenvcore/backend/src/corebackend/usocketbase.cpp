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


#include "fdesc.h"

TInt CSockDescBase::Socket(RSocketServ& /*aSs*/, int /*family*/, int /*style*/, int /*protocol*/)
	{
	return KErrNotSupported;
	}
TInt CSockDescBase::Fcntl(TUint anArg, TUint aCmd)
	{
	//fcntl supports only F_SETFL and F_GETFL for Non-Blocking I/O
	//If aCmd and anArg does not match these, return with Error
	TInt retVal = KErrNone;

	switch( aCmd )
		{
		case F_SETFL:
			{

			//Send the equivalent Flags of Symbian to RSocket
			TUint flags = iFcntlFlag;
			if( anArg & O_NONBLOCK )
				{
				ATOMICSOCKETOP(retVal = iSocket.SetOpt(KSONonBlockingIO, KSOLSocket),retVal = EBADF;)
				flags |= O_NONBLOCK;
				}
			else
				{
				ATOMICSOCKETOP(retVal = iSocket.SetOpt(KSOBlockingIO, KSOLSocket),retVal = EBADF;)				
				flags &= ~O_NONBLOCK;
				}
			if (retVal == KErrNone)
				{
				retVal = iFcntlFlag = flags; 
				}
			break;
			}
		case F_GETFL:
			{
			// Socket descriptors are always read/write
			iFcntlFlag |= O_RDWR;	
			//Return fcntl flag
			retVal = iFcntlFlag;
			break;
			}
		default:
			retVal = KErrNotSupported;
		}
	return retVal;


	}

TInt CSockDescBase::FStat(struct stat *st)
	{

	// I am a socket about which little is known
	st->st_mode = S_IFSOCK;
	st->st_blksize=0;
	return KErrNone;

	}

TInt CSockDescBase::FinalClose()
	{
	iSocket.Close();
	iReadLock.Close();
	iWriteLock.Close();
	iIoctlLock.Close();
	return KErrNone;
	}

void CSockDescBase::Read(TDes8& aBuf, TRequestStatus& aStatus)
	{
	TSockXfrLength len;
	TRequestStatus tempStatus;

	ATOMICSOCKETOP(iSocket.RecvOneOrMore(aBuf, 0, tempStatus, len),Complete(tempStatus,KErrBadHandle))
		// needs a completion which returns the length
	User::WaitForRequest(tempStatus);
	if (tempStatus.Int() != KErrNone)
		{
		Complete(aStatus, tempStatus.Int());
		}
	else
		{
		Complete(aStatus, len());
		}
	}

void CSockDescBase::Write(TDes8& aBuf, TRequestStatus& aStatus)
	{
	TRequestStatus tempStatus;
	TInt bytesWritten = 0;
	TInt bufLength = aBuf.Length();
	TSockXfrLength len = 0;
	do
		{
		ATOMICSOCKETOP(iSocket.Send(aBuf.Mid(bytesWritten), 0, tempStatus, len),Complete(tempStatus,KErrBadHandle))		
		User::WaitForRequest(tempStatus);			
		if (len() == 0 || tempStatus.Int() == KErrDisconnected )
			{
			break;
			}
		bytesWritten += len();			
		} while (bytesWritten < bufLength);
	if (tempStatus.Int() != KErrNone)
		{
		Complete(aStatus, tempStatus.Int());
		}
	else if ((len() == 0) && (bytesWritten == 0))
		{
		Complete(aStatus, KErrWouldBlock);
		}	
	else
		{
		//to do change request complete with actual length written when defect
		//is fixed
		Complete(aStatus, bytesWritten);	
		}

	}

void CSockDescBase::RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus)
	{

	//Map the flags to the RSocket flags
	TUint rSockFlags = 0;
	if( flags & MSG_PEEK ) //Peek at incoming data
		rSockFlags |= KSockReadPeek;
	/* No equivalent flags avaialble in RSocket for
	   MSG_OOB and MSG_WAITALL */

	TSockXfrLength len;
	TRequestStatus tempStatus;

	switch (iStyle)
		{
		case SOCK_STREAM:
			// recvfrom on a stream ignores the from address - get the peername
			if (from.Length())
				SockName(1,from);
			
			ATOMICSOCKETOP(iSocket.RecvOneOrMore(aDesc,rSockFlags,tempStatus,len),Complete(tempStatus,KErrBadHandle))			            
			break;

		case SOCK_SEQPACKET:
			// get the peername (as above)
			if (from.Length())
				SockName(1,from);
			ATOMICSOCKETOP(iSocket.Recv(aDesc, rSockFlags, tempStatus),Complete(tempStatus,KErrBadHandle))
			
			break;

		default: // including SOCK_RAW, SOCK_DGRAM
			// assume datagram, as per behavior of original stdlib code:
		    ATOMICSOCKETOP(iSocket.RecvFrom(aDesc,from,rSockFlags,tempStatus,len),Complete(tempStatus,KErrBadHandle))
		}

	User::WaitForRequest(tempStatus);
	len = aDesc.Length();
	if (tempStatus.Int() != KErrNone)
		{
		if (tempStatus.Int() == KErrEof || tempStatus.Int() == KErrDisconnected)
			Complete(aStatus, len());
		else
			Complete(aStatus, tempStatus.Int());
		}
	else
		{
		Complete(aStatus, len());
		}

	}

void CSockDescBase::SendTo(TDes8& aDesc, TSockAddr& to, int flags, TRequestStatus& aStatus)
	{

	TRequestStatus tempStatus;
	TSockXfrLength len;
	TBool sendflg = EFalse;

	if (to.Length()==0)
		{
        ATOMICSOCKETOP(iSocket.Send(aDesc,flags,tempStatus,len),Complete(tempStatus,KErrBadHandle))
        sendflg = ETrue;
		}	
	else
		{
		if (isStream())
			Complete(aStatus,KErrNotSupported);	// can't sendto a stream
		else 
			{
			ATOMICSOCKETOP(iSocket.SendTo(aDesc,to,flags,tempStatus,len),Complete(tempStatus,KErrBadHandle))			
			sendflg = ETrue;
			}
		}

	if(sendflg)
		{
		User::WaitForRequest(tempStatus);
		if (tempStatus.Int() != KErrNone)
			{
			if (tempStatus.Int() == KErrEof )
				Complete(aStatus, len());
			else
				Complete(aStatus, tempStatus.Int());
			}
		else
			{
			Complete(aStatus, len());
			}	
		}


	}

TInt CSockDescBase::CompletionStatus(TInt& aLength, TInt aStatus)
	{
	aLength = aStatus;
	if (aStatus >= 0)
		{
		return 0;
		}
	return aStatus;
	}

TInt CSockDescBase::Poll(TUint aEvents)
	{
	TInt status = 0;
	TInt readyEvents = 0;
	TInt err = KErrNone;
	ATOMICSOCKETOP(err = iSocket.GetOpt(KSOSelectPoll, KSOLSocket, status),err = KErrBadHandle)
	
	if (err != KErrNone)
		{
		// Poll should return any of the requested events.
		// In case of any error, the error will be set, and can be later checked by the descriptor.


		// For non-blocking socket, ensure to reset "iConnectInProgress" flag for a non-connected 
		// socket on which a connection is pending.
		if(GetConnectionProgress())
			{
            iPollErr = err;

            SetConnectionProgress(EFalse);
			}
			
		// set all the events that has been requested for
		// This handles a scenario where connect fails( in loopback )
		// here poll should return all the events requested as ready
		// not KErrNone or failure value as returning KErrNone will indicate
		// no events are ready and notifyactivity will be used though the event has
		// completed
		if( aEvents & EReadyForReading )
		    {
		    readyEvents |= EReadyForReading;
		    }
		if( aEvents & EReadyForWriting )
		    {
		    readyEvents |= EReadyForWriting;
		    }
        if( aEvents & EAnyException )
            {
            readyEvents |= EAnyException;
            }
        return readyEvents;		
		}

	if ((status & KSockSelectRead) && (aEvents & EReadyForReading))
		{
		readyEvents |= EReadyForReading;
		}
		
	if ((status & KSockSelectWrite) && (aEvents & EReadyForWriting))
		{
		readyEvents |= EReadyForWriting;
		}
		
	if (status & KSockSelectExcept)
		{

		if (GetConnectionProgress())
			{		
			TInt val = -1;
			TInt ret = KErrNone;
			ATOMICSOCKETOP(ret = iSocket.GetOpt(KSOSelectLastError, KSOLSocket, val),ret = KErrBadHandle)

			(iPollErr = ret) || (iPollErr = val);
			TBool setExceptFd = ETrue;
			// Some special checks for non-blocking sockets.
			if(aEvents & EReadyForWriting)
				{
				readyEvents |= EReadyForWriting;
				setExceptFd = EFalse;
				}
			
			if(aEvents & EReadyForReading)
				{
				readyEvents |= EReadyForReading;
				setExceptFd = EFalse;
				}
	
			if(setExceptFd && (aEvents & EAnyException))
				{
				readyEvents |= EAnyException;
				}
			}
		else
			{
			if(aEvents & EAnyException)
				{
				readyEvents |= EAnyException;
				}
			}
		}	

	if(GetConnectionProgress() && readyEvents)
		{
		SetConnectionProgress(EFalse);		
		}
	
	return readyEvents;
	}

/* Register for notification of activity */
TInt CSockDescBase::NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 timeout)
	{
	if (timeout.Int())
		{
		TInt ret = iIoctlLock.Wait(timeout.Int());
		if (ret != KErrNone)
			{
			return KErrCompletion;
			}
		}
	else
		{
		iIoctlLock.Wait();
		}

	iSelectEvents() = 0;
	
	if (aEvents & EReadyForReading)
		{
		iSelectEvents() = KSockSelectRead;
		}
	if (aEvents & EReadyForWriting)
		{
		iSelectEvents() |= KSockSelectWrite;
		}
	if (aEvents & EAnyException)
		{
		iSelectEvents() |= KSockSelectExcept;
		}
	ATOMICSOCKETOP(iSocket.Ioctl(KIOctlSelect, aRequest, &iSelectEvents, KSOLSocket),Complete(aRequest,KErrBadHandle))	
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CSockDescBase::TweakWatchedEvents
// Requests for socket behaviour specific additional events
// -----------------------------------------------------------------------------
//
void CSockDescBase::TweakWatchedEvents(TUint& events)
    {
    if((events & EReadyForReading) || (events & EReadyForWriting))
        {
        // Handles scenario of select called with writefds, after a non blocking connect on a socket
        // as per connect specification, select with the writefds after connect should
        // set writefds, irrespective of if the connection is successful or failure
        // We request for an exception event, as in Symbian connect failure is
        // an exception
        if(GetConnectionProgress())
            {
            events |= EAnyException;
            }
        }           
    }
// -----------------------------------------------------------------------------
// CSockDescBase::TweakReadyEvents
// Prepares the socket behaviours specific output events
// -----------------------------------------------------------------------------
//
TInt CSockDescBase::TweakReadyEvents(TInt errval)
    {

    TInt returnEvents = 0;
    if( errval >= KErrNone )
        {
        // This file descriptor is socket-like
        // SelectEvents will also signal ioctlLock.
        // This hack allows other threads to issue an ioctl on this socket now rather than later.
        const TUint events = GetSelectEvents();
        if (events & KSockSelectRead) 
            {
            returnEvents |= EReadyForReading;
            } 
        if (events & KSockSelectWrite) 
            {
            returnEvents |= EReadyForWriting;
            } 
        if (events & KSockSelectExcept) 
            { 
            // if it is a case of non blocking connect check for the flag 
            // though the flag indicates connection is in progress, we come out of
            // waitforNrequest only after a event
            if(GetConnectionProgress())
                {

                TInt val = -1;
                TInt ret = KErrNone;
                ATOMICSOCKETOP(ret = iSocket.GetOpt(KSOSelectLastError, KSOLSocket, val),ret = KErrBadHandle)
                (iPollErr = ret) || (iPollErr = val);

            
                returnEvents |= EReadyForReading;
                returnEvents |= EReadyForWriting;
                returnEvents |= EAnyException;
                }
            else
                {
                returnEvents |= EAnyException;
                }
            }
        }
    else
        {
        if( GetConnectionProgress() )
            {
            // Dummy call to retrieve select events also unlocks the socket
            const TUint events = GetSelectEvents();
            
            TInt val = -1;
            TInt ret = KErrNone;
            ATOMICSOCKETOP(ret = iSocket.GetOpt(KSOSelectLastError, KSOLSocket, val),ret = KErrBadHandle)
            (iPollErr = ret) || (iPollErr = val);
            // set all the events that has been requested for
            // This handles a scenario where connect fails( in loopback )
            // here all the events requested should be ready ready
            // Since PrepareOutputEvents is not aware of the events requested, all are
            // set as ready and it will be filteret in select
            returnEvents |= EReadyForWriting;
            returnEvents |= EReadyForReading;
            returnEvents |= EAnyException;
            }
        else
            {
            returnEvents |= EAnyException;
            }
        }
    return returnEvents;
    }

void CSockDescBase::CancelNotify()
	{
	ATOMICSOCKETOP(iSocket.CancelIoctl(),NOP)	
	iIoctlLock.Signal();
	}

TInt CSockDescBase::Listen(TUint qSize)
	{	
	TInt ret = KErrNone;
	ATOMICSOCKETOP(ret = iSocket.Listen(qSize), return KErrBadHandle)
	return ret;
	}

void CSockDescBase::ReadCancel()
	{
	ATOMICSOCKETOP(iSocket.CancelRecv(),NOP)	
	}

TInt CSockDescBase::ReadCompletion(TDes8& /*aBuf*/, TInt aStatus)
	{
	if(KErrNone == aStatus)
		{
		return iLength();
		}
	return aStatus;
	}

void CSockDescBase::RecvFromCancel()
	{
	ATOMICSOCKETOP(iSocket.CancelRecv(),NOP)	
	}

void CSockDescBase::SendToCancel()
	{
	ATOMICSOCKETOP(iSocket.CancelSend(),NOP)
	}

void CSockDescBase::WriteCancel()
	{
	ATOMICSOCKETOP(iSocket.CancelWrite(),NOP)	
	}

TInt CSockDescBase::SockName(int anEnd, TSockAddr& anAddr)
	{
	const TUint KBadFamily = 0xFF000000;

	TUSockAddr *addr;

	addr = (TUSockAddr*)&anAddr;
	if (addr->iError != 0)
		{
		return addr->iError;
		}

	anAddr.SetFamily(KBadFamily);
	if (anEnd==0)
	    ATOMICSOCKETOP(iSocket.LocalName(anAddr),NOP)		
	else
	    ATOMICSOCKETOP(iSocket.RemoteName(anAddr),NOP)		
	if (anAddr.Family()==KBadFamily)
		return ENOTCONN; // assume that the call failed, but there is no way to find out why
	return KErrNone;
	}

void CSockDescBase::Shutdown(TUint aHow,TRequestStatus& aStatus)
	{
	RSocket::TShutdown how;

	switch (aHow)
		{
		case SHUT_RD:
			how = RSocket::EStopInput;
			break;				
		case SHUT_WR:
			how = RSocket::EStopOutput;
			break;		
		case SHUT_RDWR:
			how = RSocket::EImmediate;
			break;		
		default:
			Complete(aStatus,KErrArgument); // Invalid argument
			return;
		}
	ATOMICSOCKETOP(iSocket.Shutdown(how,aStatus),Complete(aStatus,KErrBadHandle))	
	return;
	}
