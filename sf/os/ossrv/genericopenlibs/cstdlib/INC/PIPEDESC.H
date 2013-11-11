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
// Support for pipes between processes
// CPipeDesc is in the parent process, CPipeChildDesc is in the child
// 
//

#include "FDESC.H"
#include <sys/ioctl.h>

NONSHARABLE_CLASS(CPipeDesc) : public CFileDescBase
/**
@internalComponent
*/
	{
public:
	CPipeDesc(TInt anIndex);

	void SetClientSide(CPipeDesc*& aClientPointer);
	TInt LSeek(int& offset, int whence);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	TInt ReadCompletion(TDes8& aDesc, TInt aStatus);
	void ReadCancel();
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt WriteCompletion(TDes8& aDesc, TInt aStatus);
	void WriteCancel();
	TInt FStat(struct stat *st);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	void IoctlCancel();

	void ClientRead(const RMessage2& aMessage);
	void ClientWrite(const RMessage2& aMessage);
	void ClientIoctl(const RMessage2& aMessage);
	void ClientCancel(const RMessage2& aMessage);
	void ClientClose();
protected:
	TInt FinalClose();
private:
	void Cancel();
	TInt IsWriteable() { return !iIndex; }	// 0 => child STDIN
	TInt IsReadable() { return iIndex; }
	TInt SelectMask()       { return (iIndex)? E32SELECT_READ :E32SELECT_WRITE; }
	TInt ClientSelectMask() { return (iIndex)? E32SELECT_WRITE:E32SELECT_READ;  }
	TInt ClientIoctlPending() { return iClientIoctlPending; }

	void TransferFromClient();
	void TransferToClient();
	void CompleteClientIoctl(TInt ret);
	void CompleteClientIoctl();
	void Panic(TInt aReason);
	void Panic(RMessage2& aMessage, TInt aReason);
private:
	TInt iIndex;	// index into per-process table of pipes, also implies direction.
	CPipeDesc** iClientSide;
	TInt iClientClosed;
	// Pending info for parent operation
	TRequestStatus* iStatus;	// null implies "no pending operation"
	TRequestStatus* iIoctlStatus;
	TPtrC8 iWriteBuf;
	TDes8* iReadBuf;
	// Pending info for child operation
	TInt iClientLength;		// 0 implies "no pending operation"
	TInt iClientOffset;
	RMessage2 iMessage;
	TInt iClientIoctlPending;
	RMessage2 iIoctlMessage;
	};

class RPosixSession;
NONSHARABLE_CLASS(CPipeChildDesc) : public CFileDescBase
/**
CPipeChildDesc
Basically just forwards requests to the parent CPosixServer, where the related CPipeDesc
does the actual work.
@internalComponent
*/
	{
public:
	CPipeChildDesc(TInt anIndex, RPosixSession& aSession); 

	TInt LSeek(int& offset, int whence);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	TInt ReadCompletion(TDes8& aDesc, TInt aStatus);
	void ReadCancel();
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt WriteCompletion(TDes8& aDesc, TInt aStatus);
	void WriteCancel();
	TInt FStat(struct stat *st);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	void IoctlCancel();

	void ClientClose() { iClientClosed=1; }

protected:
	TInt FinalClose();
private:
	void Cancel(TInt aType);
	TInt IsWriteable() { return iIndex; }	// 0 => child STDIN
	TInt IsReadable() { return !iIndex; }
	TInt SelectMask() { return (!iIndex)? E32SELECT_READ:E32SELECT_WRITE; }

	TInt iIndex;	// index into per-process table of pipes, also implies direction.
	RPosixSession& iSession;
	TInt iClientClosed;
	TPtr8 iParamDes;
	};
