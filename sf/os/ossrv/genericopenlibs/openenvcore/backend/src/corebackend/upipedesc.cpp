/* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 * Name        : UPIPEDESC.CPP
 * Description : Contains the source for the pipe descriptor classes 
 * 
 */



#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <rpipe.h>
#include <signal.h>
#include "fdesc.h"

TInt CPipeDescBase::Fcntl(TUint anArg, TUint aCmd)
	{
	TInt ret = KErrNone;
    TUint  accessMode = iFcntlFlag & O_ACCMODE;
    TUint  statusFlag = O_NONBLOCK | O_APPEND| O_SYNC; //Looks like we only support O_NONBLOCK

	// F_DUPFD is handled before this method is reached
	switch (aCmd)
		{
	case F_SETFL:
	    iFcntlFlag = accessMode|(anArg & statusFlag); //preserve the access mode flags
		break;
	case F_GETFL:
		ret = iFcntlFlag;
		break;
	case F_SETFD:
		if(anArg & FD_CLOEXEC)
			iFdAttrib |= KCloseonExec;
		else
			iFdAttrib &= ~KCloseonExec;
		break;
	case F_GETFD:
		if (iFdAttrib & KCloseonExec)
			{
			ret = FD_CLOEXEC;
			}
		break;
		}

	return ret;
	}


TInt CPipeDescBase::FStat(struct stat *st)
	{
	// I am a fifo about which little is known
	st->st_mode = S_IFIFO;
	st->st_blksize = 0;
	return KErrNone;
	}

void CPipeDescBase::Ioctl(int /* aCmd */, void* /* aParam */, TRequestStatus& aStatus)
	{
	// return if it is an invalid fd - Do I need this?
	if (iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}

	Complete(aStatus, KErrNotSupported);
	}

TInt CPipeDescBase::FinalClose()
	{
	iHandle.Close();
	return KErrNone;
	}


void CPipeReadDesc::Read(TDes8& aDesc, TRequestStatus& aStatus)
	{
	// return if it is an invalid fd - Do I need this?
	if (iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}

	//Acquire the Lock before read and release it later
	Complete(aStatus, DoRead(aDesc));
	}

TInt CPipeReadDesc::DoRead(TDes8& aDesc)
	{
	TInt err = KErrNone;

	if (iFcntlFlag & O_NONBLOCK)
		{
		err = iHandle.Read(aDesc, aDesc.MaxLength());
		if (err == 0 || err == KErrUnderflow || err == KErrNotReady)
			{
			err = KErrWouldBlock;
			}
		}
	else
		{
		err = iHandle.ReadBlocking(aDesc, aDesc.MaxLength());
		if (err == KErrNotReady)
			{
			err = 0;
			}
		}

	return err;
	}

void CPipeReadDesc::Write(TDes8& /*aDesc*/, TRequestStatus& aStatus)
	{
	// Write on a RD_ONLY handle
	Complete(aStatus, KErrBadHandle);
	}

TInt CPipeReadDesc::Poll(TUint aEvents)
	{
	if (aEvents & EReadyForWriting && !(aEvents & EReadyForReading) &&  !(aEvents & EAnyException))
		{
		return KErrNotSupported;
		}
	TInt sz = iHandle.Size();
	if (sz == 0)
		{
		if(aEvents & EAnyException)
      			{
      			TBuf8<1> dummy;
      			//returns the number of bytes read from the pipe if passed.
      			sz = iHandle.Read(dummy,0);
      			// if write end of pipe is closed return an exception.
      			if (sz == KErrNotReady)
        			{
        			return EAnyException;
        			}
       		return 0;
       		}
		}
	if (sz > 0)
		{
		if(aEvents & EReadyForReading)
			{
			// There is data in the pipe
			return EReadyForReading;
			}
		return 0;
		}
	return 0;
	}

TInt CPipeReadDesc::NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 /*timeout = 0*/)
	{
	if (aEvents & EReadyForWriting && !(aEvents & EReadyForReading) &&  !(aEvents & EAnyException))
		{
		return KErrNotSupported;
		}

	// Even if aEvents includes EAnyException, I will register for DataAvailable.
	// If aRequest.Int() is not KErrNone, I will mark this fd as set in exceptfds.
	// Else ignore.
	iHandle.NotifyDataAvailable(aRequest);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CPipeDescBase::TweakReadyEvents
// Prepares the pipe specific output events
// -----------------------------------------------------------------------------
//
TInt CPipeReadDesc::TweakReadyEvents(TInt errval)
    {
    TInt returnEvents = 0;
    // This file descriptor is pipe-like 
    if( errval >= KErrNone )
        {
        returnEvents |= EReadyForReading;
        }
    else
        {
        returnEvents |= EAnyException;
        }
    return returnEvents;
    }
	
void CPipeReadDesc::CancelNotify()
	{
	iHandle.CancelDataAvailable();
	}

void CPipeWriteDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	// return if it is an invalid fd - Do I need this?
	if (iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}

	// Acquire the Lock before write and release it later
	Complete(aStatus, DoWrite(aDesc));
	}

TInt CPipeWriteDesc::DoWrite(TDes8& aDesc)
	{
	TInt err = KErrNone;
	TInt sz = aDesc.Length();
	TInt size = 0;
	if (iFcntlFlag & O_NONBLOCK)
		{
		if (aDesc.Length() > iHandle.MaxSize())
			{
			sz = iHandle.MaxSize() - iHandle.Size();
			if (sz == 0)
				{
				// no space in pipe. Attempt to write 1 byte
				sz = 1;
				}
			}

		err = iHandle.Write(aDesc, sz);
		if (err == KErrOverflow)
			{
			err = KErrWouldBlock;
			}
		size = err;
		}
	else
		{
		TPtrC8 ptr(aDesc);
		TInt len;

		do
			{
			len = sz;
			if (len > iHandle.MaxSize())
				{
				len = iHandle.MaxSize();
				}

			err = iHandle.Write(ptr, len);
			if (err == KErrOverflow)
				{
				TRequestStatus req = KRequestPending;
				iHandle.NotifySpaceAvailable(len, req);
				User::WaitForRequest(req);
				err = iHandle.Write(ptr, len);
				}

			if (err < 0)
				{
				size = err;
				break;
				}

			/* err is the number of bytes actually written into the pipe */
			sz -= err;
			if (sz)
				{
				ptr.Set( ptr.Mid(err));
				}
			size +=err;
			} while (sz);
		}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	if(err == KErrNotReady)
		{
		TInt lErrno;
		Backend()->raise(SIGPIPE,lErrno);
		}
#endif
	return size;
	}


void CPipeWriteDesc::Read(TDes8& /*aDesc*/, TRequestStatus& aStatus)
	{
	// Read on a WR_ONLY handle
	Complete(aStatus, KErrBadHandle);
	}

TInt CPipeWriteDesc::Poll(TUint aEvents)
	{
	if (aEvents & EReadyForReading && !(aEvents & EReadyForWriting) &&  !(aEvents & EAnyException))
		{
		return KErrNotSupported;
		}

	if (iHandle.MaxSize() > iHandle.Size())
		{
		if(aEvents & EAnyException)
      			{
		      TBuf8<1> dummy;
		      //to check if read end of a pipe is closed
		      TInt sz = iHandle.Write(dummy,0);
		      if (sz == KErrNotReady)
		     		{
		        	return EAnyException;
		        	}
      			return 0;
			}
		// There is space to write into the pipe
		if(aEvents & EReadyForWriting)
			{
			return EReadyForWriting;
			}
		return 0;
		}
	return 0;
	}

TInt CPipeWriteDesc::NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 /*timeout = 0*/)
	{
	if (aEvents & EReadyForReading && !(aEvents & EReadyForWriting) &&  !(aEvents & EAnyException))
		{
		return KErrNotSupported;
		}

	// Even if aEvents is EAnyException, I will register for SpaceAvailable.
	// If aRequest.Int() is not KErrNone, I will mark this fd as set in exceptfds.
	// Else ignore.

	/* I wait for the pipe to be empty. Am I bad? */
	iHandle.NotifySpaceAvailable(iHandle.MaxSize(), aRequest);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CPipeWriteDesc::TweakReadyEvents
// Prepares the pipe specific output events
// -----------------------------------------------------------------------------
//
TInt CPipeWriteDesc::TweakReadyEvents(TInt errval)
    {
    TInt returnEvents = 0;
    // This file descriptor is pipe-like 
    if( errval >= KErrNone )
        {        
        returnEvents |= EReadyForWriting;
        }
    else
        {
        returnEvents |= EAnyException;
        }
    return returnEvents;
    }
	
void CPipeWriteDesc::CancelNotify()
	{
	iHandle.CancelSpaceAvailable();
	}

