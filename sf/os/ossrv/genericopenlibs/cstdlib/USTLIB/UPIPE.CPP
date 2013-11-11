// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of pipes
// 
//

#include "POSIXIF.H"
#include <sys/errno.h>
#include <sys/stat.h>

// Child end of the pipe

CPipeChildDesc::CPipeChildDesc(TInt anIndex, RPosixSession& aSession)
		: CFileDescBase(), iIndex(anIndex), iSession(aSession), iParamDes(0,0,0)
	{}

TInt CPipeChildDesc::LSeek (int&, int)
	{
	return ESPIPE;	// can't seek on a pipe
	}

TInt CPipeChildDesc::FStat(struct stat *st)
	{
	// I am a fifo about which little is known
	st->st_mode = S_IFIFO;
	st->st_blksize=0;
	return KErrNone;
	}

void CPipeChildDesc::Read (TDes8& aBuf, TRequestStatus& aStatus)
	{
	if (iClientClosed || !IsReadable())
		{
		Complete(aStatus,KErrEof);
		return;
		}
	TIpcArgs args(iIndex,&aBuf,aBuf.MaxLength());
	iSession.Request(PMPipeRead,args,aStatus);	// asynchronous request
	}

void CPipeChildDesc::ReadCancel()
	{
	Cancel(PMPipeRead);
	}

TInt CPipeChildDesc::ReadCompletion(TDes8& aDesc, TInt aStatus)
	{
	if (aStatus==KErrEof)
		{
		ClientClose();
		aDesc.Zero();		// set read length to zero
		return KErrNone;	// indicates graceful close at the other end
		}
	return aStatus;
	}

void CPipeChildDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	if (iClientClosed || !IsWriteable())
		{
		Complete(aStatus,KErrEof);
		return;
		}
	TIpcArgs args(iIndex,&aDesc,aDesc.Length());
	iSession.Request(PMPipeWrite,args,aStatus);	// asynchronous request
	}

void CPipeChildDesc::WriteCancel()
	{
	Cancel(PMPipeWrite);
	}

TInt CPipeChildDesc::WriteCompletion(TDes8& /*aDesc*/, TInt aStatus)
	{
	if (aStatus==KErrEof)
		ClientClose();
	return aStatus;
	}

void CPipeChildDesc::Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus)
//
// The work of the Ioctl is done in the parent, including writing back to aParam?
// Use the default completion which just returns aStatus.Int()
//
	{
	if (iClientClosed)
		{
		Complete(aStatus,KErrEof);
		return;
		}
	iParamDes.Set((TText8*)aParam,4,4);
	TIpcArgs args(iIndex,aCmd,&iParamDes);	
	if (aCmd==E32IOSELECT)
		args.Set(3, *((TInt*)aParam));
	iSession.Request(PMPipeIoctl,args,aStatus);	// asynchronous request
	}

void CPipeChildDesc::IoctlCancel()
	{
	Cancel(PMPipeIoctl);
	}

void CPipeChildDesc::Cancel(TInt aType)
	{
	if (iClientClosed)
		return;
	TIpcArgs args(iIndex,aType);
	iSession.Request(PMPipeCancel,args);
	}	

TInt CPipeChildDesc::FinalClose()
	{
	ClientClose();
	TIpcArgs args(iIndex);
	return iSession.Request(PMPipeClose,args);		// synchronous request
	}

// 
// Parent end of the pipe, where the real work is done
//

CPipeDesc::CPipeDesc(TInt anIndex) : CFileDescBase()
	{
	iIndex=anIndex;
	}

void CPipeDesc::SetClientSide(CPipeDesc*& aClientPointer)
	{
	iClientSide=&aClientPointer;
	}

_LIT(KCPipeDescPanic, "CPipeDesc");
void CPipeDesc::Panic(TInt aReason)
	{
	User::Panic(KCPipeDescPanic,aReason);
	}

void CPipeDesc::Panic(RMessage2& aMessage, TInt aReason)
	{
	aMessage.Panic(KCPipeDescPanic,aReason);
	}

TInt CPipeDesc::LSeek (int&, int)
	{
	return ESPIPE;	// can't seek on a pipe
	}

TInt CPipeDesc::FStat(struct stat *st)
	{
	// I am a fifo about which little is known
	st->st_mode = S_IFIFO;
	st->st_blksize=0;
	return KErrNone;
	}

void CPipeDesc::Read (TDes8& aBuf, TRequestStatus& aStatus)
	{
	if (!IsReadable())
		{
		Complete(aStatus, KErrEof);	// avoids treading on iStatus of pending Write
		return;
		}
	__ASSERT_DEBUG(iStatus==0,Panic(1));
	iStatus=&aStatus;
	if (iClientClosed)
		{
		User::RequestComplete(iStatus,KErrEof);
		return;
		}
	iReadBuf=&aBuf;
	if (ClientIoctlPending())
		CompleteClientIoctl();
	if (iClientLength!=0)
		TransferFromClient();
	}

void CPipeDesc::ReadCancel()
	{
	Cancel();
	}

TInt CPipeDesc::ReadCompletion(TDes8& aDesc, TInt aStatus)
	{
	if (aStatus==KErrEof)
		{
		ClientClose();
		aDesc.Zero();		// set read length to zero
		return KErrNone;	// indicates graceful close at the other end
		}
	return aStatus;
	}

void CPipeDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	if (!IsWriteable())
		{
		Complete(aStatus, KErrEof);	// avoids treading on iStatus of pending Read
		return;
		}
	__ASSERT_DEBUG(iStatus==0,Panic(2));
	iStatus=&aStatus;
	if (iClientClosed)
		{
		User::RequestComplete(iStatus,KErrEof);
		return;
		}
	iWriteBuf.Set(aDesc);
	if (ClientIoctlPending())
		CompleteClientIoctl();
	if (iClientLength!=0)
		TransferToClient();
	}

void CPipeDesc::WriteCancel()
	{
	Cancel();
	}

TInt CPipeDesc::WriteCompletion(TDes8& /*aDesc*/, TInt aStatus)
	{
	if (aStatus==KErrEof)
		ClientClose();
	return aStatus;
	}

void CPipeDesc::Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	TInt ret=KErrNone;
	iIoctlStatus=&aStatus;
	int *param=REINTERPRET_CAST(int*,aParam);
	switch (aCmd)
		{
	case E32IONREAD:
		// synchronous ioctls are handled in the completion routine.
		break;
	case E32IOSELECT:
		{
		int mask=(*param)&SelectMask();
		if (mask!=0 && iClientLength==0)
			return;	// wait for client to show up
		}
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	User::RequestComplete(iIoctlStatus,ret);
	}

TInt CPipeDesc::IoctlCompletion(int aCmd, void* aParam, TInt aStatus)
	{
	TInt ret=aStatus;
	if (ret!=KErrNone)
		return ret;
	int *param=REINTERPRET_CAST(int*,aParam);
	switch (aCmd)
		{
	case E32IONREAD:
		if (IsReadable())
			*param=iClientLength;	// 0 if no outstanding client data
		else
			*param=0;	// claim that no data is available
		break;
	case E32IOSELECT:
		{
		int mask=0;
		if (iClientLength!=0)
			mask = SelectMask();
		*param=(*param)&mask;
		}
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	return ret;
	}

void CPipeDesc::IoctlCancel()
	{
	User::RequestComplete(iIoctlStatus,KErrCancel);
	}

void CPipeDesc::Cancel()
	{
	// Pipes are unidirectional, so don't need to distinguish between
	// ReadCancel and WriteCancel
	User::RequestComplete(iStatus,KErrCancel);
	}

// Client-side interface

void CPipeDesc::ClientWrite(const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(iClientLength==0,Panic(3));
	if (iClientClosed)
		{
		aMessage.Complete(KErrEof);
		return;
		}
	iClientLength=aMessage.Int2();
	iClientOffset=0;
	iMessage=aMessage;
	if (iIoctlStatus!=0)
		User::RequestComplete(iIoctlStatus,KErrNone);
	if (iStatus!=0)
		TransferFromClient();
	}

void CPipeDesc::ClientRead(const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(iClientLength==0,Panic(4));
	if (iClientClosed)
		{
		aMessage.Complete(KErrEof);
		return;
		}
	iClientLength=aMessage.Int2();
	iMessage=aMessage;
	if (iIoctlStatus!=0)
		User::RequestComplete(iIoctlStatus,KErrNone);
	if (iStatus!=0)
		TransferToClient();
	}

void CPipeDesc::ClientIoctl(const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(!ClientIoctlPending(),Panic(7));
	if (iClientClosed)
		{
		aMessage.Complete(KErrEof);
		return;
		}
	iClientIoctlPending=1;
	iIoctlMessage=aMessage;
	TInt ret=KErrNone;
	switch (aMessage.Int1())
		{
	case E32IONREAD:
		// synchronous ioctls are handled in the completion routine.
		break;
	case E32IOSELECT:
		{
		int mask=aMessage.Int3();
		mask&=ClientSelectMask();
		if (mask!=0 && iStatus==0)
			return;	// wait for parent activity
		}
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	CompleteClientIoctl(ret);
	}

void CPipeDesc::ClientCancel(const RMessage2& aMessage)
	{
	if (aMessage.Int1()==PMPipeIoctl)
		{
		if (ClientIoctlPending())
			CompleteClientIoctl(KErrCancel);
		return;
		}
	// Pipes are unidirectional, so Read and Write are cancelled by
	// cancelling the current client operation.
	//
	if (iClientLength!=0)
		{
		iMessage.Complete(KErrCancel);
		iClientLength=0;
		}
	}

void CPipeDesc::ClientClose()
	{
	iClientClosed=1;
	// terminate any pending requests
	if (iStatus!=0)
		User::RequestComplete(iStatus,KErrEof);
	if (ClientIoctlPending())
		CompleteClientIoctl(KErrEof);
	if (iClientLength!=0)
		{
		iMessage.Complete(KErrEof);
		iClientLength=0;
		}
	}

TInt CPipeDesc::FinalClose()
	{
	ClientClose();
	if (iClientSide)
		{
		*iClientSide=0;
		iClientSide=0;
		}
	return KErrNone;
	}

void CPipeDesc::TransferFromClient()
//
// Handle transfer of data from client to parent.
// Always complete the parent read, but only complete the child write when
// all of the data has been consumed.
//
	{
	TRAPD(err,iMessage.ReadL(1,*iReadBuf,iClientOffset));
	if (err)
		{
		Panic(iMessage,5);
		iClientLength=0;
		ClientClose();	// will complete the parent read
		return;
		}
	TInt length=iReadBuf->Length();	// record the amount of data transferred
	User::RequestComplete(iStatus,KErrNone);
	iClientOffset+=length;
	iClientLength-=length;
	if (iClientLength==0)
		iMessage.Complete(KErrNone);
	}

void CPipeDesc::TransferToClient()
//
// Handle transfer from parent to client
// Always complete the client read, but only complete the parent write when
// all of the data has been consumed.
//
	{
	TInt err=KErrNone;
	TInt length=iWriteBuf.Length();
	TInt written=length;
	if (iClientLength >= length)
		{
		TRAP(err,iMessage.WriteL(1,iWriteBuf,0));
		}
	else
		{
		written=iClientLength;
		TRAP(err,iMessage.WriteL(1,iWriteBuf.Left(written),0));
		}
	iClientLength=0;
	if (err)
		{
		Panic(iMessage,6);
		ClientClose();	// will complete the parent write
		return;
		}
	iMessage.Complete(KErrNone);
	length-=written;
	if (length==0)
		User::RequestComplete(iStatus,KErrNone);
	else
		iWriteBuf.Set(iWriteBuf.Right(length));
	}

void CPipeDesc::CompleteClientIoctl(TInt ret)
	{
	if (ret!=KErrNone)
		{
		iIoctlMessage.Complete(ret);
		iClientIoctlPending=0;
		return;
		}
	CompleteClientIoctl();
	}

void CPipeDesc::CompleteClientIoctl()
//
// Complete outstanding PMPipeIoctl message
//
	{
	TInt ret=KErrNone;
	int param=0;
	switch (iIoctlMessage.Int1())
		{
	case E32IONREAD:
		if (IsWriteable() && iStatus!=0)
			param=iWriteBuf.Length();
		else
			param=0;	// claim that no data is available
		break;
	case E32IOSELECT:
		{
		int mask=0;
		if (iStatus!=0)
			mask=ClientSelectMask();
		param=(iIoctlMessage.Int3())&mask;
		}
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	if (ret==KErrNone)
		{
		TPtrC8 paramReturn((const TText8*)&param,4);
		TRAP(ret,iIoctlMessage.WriteL(2,paramReturn,0));
		}
	iIoctlMessage.Complete(ret);
	iClientIoctlPending=0;
	}
