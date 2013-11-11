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
//

#include "FDESC.H"
#include "LPOSIX.H"
#include "LTIME.H"
#include <string.h>
#include <stdio_r.h>
#include <fcntl.h>		// for struct stat
#include <sys/errno.h>		// for ENOTSOCK
#include <sys/ioctl.h>
#include <c32comm.h>

#include "POSIXIF.H"	// for details of CPosixRequest::iLink

// Cleanup support

void CFileDescBase::Cleanup(TAny *aPtr)
	{
	((CFileDescBase*)aPtr)->Close();
	}

void CFileDescBase::PushLC()
	{
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	}

// Private constructor

inline TPosixRequestQueue::TPosixRequestQueue() 
: TSglQue<CPosixRequest>(_FOFF(CPosixRequest,iLink)) 
	{}

CFileDescBase::CFileDescBase() : iReadTimeout(-1)
	{
	}

// A CFileDescBase factory function, for "named" file-like objects

//CFileDescBase* CFileDescBase::Open(RFs& aSession, const char* name, int mode, int perms, TInt& err)
//CFileDescBase* CFileDescBase::Open(RFs& aSession, const wchar_t* name, int mode, int perms, TInt& err)
CFileDescBase* CFileDescBase::Open(RSessionBase& aSession, const wchar_t* name, int mode, int perms, TInt& err)
	{
	CFileDescBase* ret=0;

	if (wcscmp(name,L"CON:")==0)
		ret= new CTtyDesc;	// NB. This won't be the default stdin/stdout/stderr console
	else
	if (wcscmp(name,L"NUL:")==0)
		ret= new CFileDescBase;
	else
	if (wcscmp(name,L"TMP:")==0)
		{
		RFs& rfs = static_cast<RFs&>(aSession);
		TParse path;
		err=GetFullPath(path,(const TText16*)WIDEP_tmpdir, rfs,NULL);
		if (err)
			return 0;
		CTempFileDesc* tmp= new CTempFileDesc;
		if (tmp)
			{
			err=tmp->Open(rfs,path.DriveAndPath());
			if (err)
				{
				delete tmp;
				return 0;
				}
			}
		ret=tmp;
		}
	else if	((L'C' == name[0]) && (L'O' == name[1]) && (L'M' == name[2]) && (L':' == name[4]) && ((name[3] >= L'1') && (name[3] <= L'9')) ||
			(L'I' == name[0]) && (L'R' == name[1]) && (L'C' == name[2]) && (L'O' == name[3]) && (L'M' == name[4]) && (L':' == name[6]) && ((name[5] >= L'1') && (name[5] <= L'9')))
		{

		RCommServ& rcs = static_cast<RCommServ&>(aSession);
		if (!rcs.Handle())	
			{
			err=rcs.Connect();	//connect to the server
			if (err)
				return 0;
			}
		CSerialDesc * tmp = new CSerialDesc;
		if (tmp)
			{
			RCommServ& rcs = static_cast<RCommServ&>(aSession);
			CleanupStack::PushL(tmp);
			err = tmp->Open(rcs, name, mode, perms);
			CleanupStack::Pop(tmp);
			if (err)
				{
				delete tmp;
				return 0;
				}
			}
		ret = tmp;
		}
	else
		{
		TFullName fullName;
		RFs& rfs = static_cast<RFs&>(aSession);
		err=GetFullFile(fullName,(const TText16*)name,rfs);
		if (err)
			return 0;
		CFileDesc* f= new CFileDesc;
		if (f)
			{
			err=f->Open(rfs,fullName,mode,perms);
			if (err)
				{
				delete f;
				return 0;
				}
			}
		ret=f;
		}
	err=(ret==0)? KErrNoMemory:KErrNone;
	return ret;
	}

// Useful default implementations for CFileDescBase virtual functions.

TInt CFileDescBase::LSeek (int& offset, int)
	{
	// minimal implementation for devices which can't seek
	offset=0;
	return KErrNone;
	}

void CFileDescBase::Read (TDes8& aBuf, TRequestStatus& aStatus)
	{
	// minimal implementation for /dev/null
	aBuf.Zero();	// set length to zero
	TRequestStatus* sp=&aStatus;
	User::RequestComplete(sp,KErrNone);
	}

void CFileDescBase::ReadCancel() {}

TInt CFileDescBase::ReadCompletion (TDes8& /*aBuf*/, TInt aStatus)
	{
	return aStatus;
	}


TInt CFileDescBase::FStat (struct stat *st)
	{
	// minimal implementation: 
	// I am a character device about which little is known
	st->st_mode = S_IFCHR;
	st->st_blksize=0;
	return KErrNone;
	}

void CFileDescBase::Complete (TRequestStatus& aStatus, TInt aResult)
	{
	TRequestStatus* sp=&aStatus;
	User::RequestComplete(sp,aResult);
	}

void CFileDescBase::Write (TDes8& /*aBuf*/, TRequestStatus& aStatus)
	{
	// minimal implementation for /dev/null
	// we will claim to have written all of the data
	Complete(aStatus,KErrNone);
	}

void CFileDescBase::WriteCancel() {}

TInt CFileDescBase::WriteCompletion (TDes8& /*aBuf*/, TInt aStatus)
	{
	return aStatus;
	}

void CFileDescBase::Sync (TRequestStatus& aStatus)
	{
	// minimal implementation for /dev/null
	Complete(aStatus,KErrNone);
	}

void CFileDescBase::SyncCancel() {}

void CFileDescBase::Ioctl(int /*aCmd*/, void* /*aParam*/, TRequestStatus& aStatus)
	{
	// minimal implementation for /dev/null and other synchronous devices
	Complete(aStatus,KErrNone);
	}

void CFileDescBase::IoctlCancel() 
	{
	return;	// suitable for all synchronous ioctls
	}

TInt CFileDescBase::IoctlCompletion(int aCmd, void* aParam, TInt aStatus)
	{
	TInt ret=aStatus;
	if (ret!=KErrNone)
		return ret;
	int *param=REINTERPRET_CAST(int*,aParam);
	switch (aCmd)
		{
	case E32IONREAD:
		*param=0;	// claim that no data is available
		break;
	case E32IOSELECT:
		*param=(*param)&(E32SELECT_READ|E32SELECT_WRITE);	// but don't block
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	return ret;
	}

// A CFileDescBase factory function, for socket objects

CFileDescBase* CFileDescBase::Socket(RSocketServ& aSs, int family, int style, int protocol, TInt& err)
	{
	// connect to the Socket Server if necessary
	if (aSs.Handle()==0)
		{
		err=aSs.Connect(TUint(-1));	// allow arbitrary number of requests
		if (err)
			return 0;
		}
	CSocketDesc* s= new CSocketDesc;
	if (s==0)
		{
		err=KErrNoMemory;
		return 0;
		}
	err=s->Socket(aSs,family,style,protocol);
	if (err)
		{
		delete s;
		return 0;
		}
	return s;
	}

// minimal implementation of sockets, useful for all non-socket descriptors

void CFileDescBase::RecvFrom(TDes8& /*aDesc*/, TSockAddr& /*from*/, int /*flags*/, TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::RecvFromCancel () {}

TInt CFileDescBase::RecvFromCompletion(TInt& /*aLength*/, TInt aStatus)
	{
	return aStatus;
	}

void CFileDescBase::SendTo(TDes8& /*aDesc*/, TSockAddr& /*to*/, int /*flags*/, TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::SendToCancel () {}

TInt CFileDescBase::SendToCompletion(TDes8& /*aDesc*/, TInt aStatus)
	{
	return aStatus;
	}

void CFileDescBase::Shutdown(TUint /*aHow*/,TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::ShutdownCancel () {}

TInt CFileDescBase::Bind(TSockAddr& /*anAddr*/)
	{
	return ENOTSOCK;
	}

TInt CFileDescBase::Listen(TUint /*qSize*/)
	{
	return ENOTSOCK;
	}

void CFileDescBase::Accept(CSocketDesc*& /*aNewSocket*/, TRequestStatus& aStatus, RSocketServ& /*aSs*/)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::AcceptCancel () {}

void CFileDescBase::Connect(TSockAddr& /*anAddr*/,TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::ConnectCancel () {}

TInt CFileDescBase::SockName(int /*anEnd*/, TSockAddr& /*anAddr*/)
	{
	return ENOTSOCK;
	}

TInt CFileDescBase::GetSockOpt(TUint /*anOptionName*/,TUint /*anOptionLevel*/,TDes8& /*anOption*/)
	{
	return ENOTSOCK;
	}

TInt CFileDescBase::SetSockOpt(TUint /*anOptionName*/,TUint /*anOptionLevel*/,TDesC8& /*anOption*/)
	{
	return ENOTSOCK;
	}

// Queue handling

void CFileDescBase::AddLast(CPosixRequest& aRequest, IOQueues aQueue)
	{
	TPosixRequestQueue& queue = iQueues[aQueue];
	queue.AddLast(aRequest);
	if (queue.IsFirst(&aRequest))
		aRequest.StartAsynch();	// queue was empty, so start straight away
	}

void CFileDescBase::Remove(CPosixRequest& aRequest, IOQueues aQueue)
	{
	TPosixRequestQueue& queue = iQueues[aQueue];
	TBool wasFirst = queue.IsFirst(&aRequest);
	queue.Remove(aRequest);
	if (wasFirst)
		{
		if (!queue.IsEmpty())
			queue.First()->StartAsynch();	// start the next outstanding request
		}
	}


// Generic (non-virtual) handling for Close

TInt CFileDescBase::Close()
	{ 
	TInt err=KErrNone;
	if (--iDupCount < 0)
		{
		err=FinalClose();
		delete this;
		}
	return err;
	}

TInt CFileDescBase::FinalClose()
	{
	return KErrNone;
	}


// Simple implementation of File handling

static int MapMode(int aMode, TUint& fMode)
	{
	// EPOC32 doesn't support Write-Only
	
	if (aMode & (O_WRONLY|O_RDWR))
		{
		fMode = EFileWrite;
		fMode |= (aMode & O_EXCL) ? EFileShareExclusive : EFileShareAny;	
		}
	else
		{
		fMode = EFileRead;
		fMode |= (aMode & O_EXCL) ? EFileShareExclusive : EFileShareReadersOnly;	
		}

	fMode |= (aMode & O_TEXT) ? EFileStreamText : EFileStream;

	return aMode & (O_CREAT|O_TRUNC|O_APPEND|O_EXCL);
	}

CFileDesc::CFileDesc()
	:CFileDescBase(), iSize(EBufferSize), iExt(-1)
	{}

CFileDesc::~CFileDesc()
	{
	iFile.Close();
	delete [] iBuffer;
	}

TInt CFileDesc::FinalClose()
	{
	return DoSync();
	}
	
TInt CFileDesc::Open(RFs& aSession, const TDesC& aName, int mode, int /*perms*/)
	{
	TInt err;
	TUint fMode;

	iDrive=(TInt16)TDriveUnit(aName);

	// Create  = make new file, can return KErrAlreadyExists
	// Open    = open an existing file, can return KErrPathNotFound or KErrNotFound
	// Replace = open a new file, zapping the existing one if necessary

	int mapped=MapMode(mode, fMode);
	switch (mapped)
		{
		case O_CREAT|O_EXCL:
			err = iFile.Create(aSession, aName, fMode);
			break;
		case O_CREAT|O_TRUNC:
			err = iFile.Replace(aSession, aName, fMode);
			break;
		case O_TRUNC:
			err = iFile.Open(aSession, aName, fMode);
			if (err == KErrPathNotFound)
			    {
			    // missing directories etc, so fail directly
			    }
			else
			    {
			    iFile.Close();
			    err = iFile.Replace(aSession, aName, fMode);
			    }
			break;

		// Everything else is assumed to mean open existing file,
		// If the file isn't there, O_CREAT implies that we should make it
		default:
			err = iFile.Open(aSession, aName, fMode);
			if (err == KErrNotFound && (mapped & O_CREAT))
			    err = iFile.Create(aSession, aName, fMode);
			if (err == KErrNone && (mapped & O_APPEND))
			    {
				iPos = Ext();
				if (iPos < 0)
					err = iPos;
			    }
			break;
		}
	if ((mode & O_BUFFERED) == 0)
		iSize = 0;
	return err;
	}

TInt CFileDesc::LSeek (int& offset, int whence)
	{

	TInt pos=offset;
	TInt ext=Ext();
	if (ext < 0)
		return ext;

	switch (whence)
		{
	case SEEK_SET:
		break;
	case SEEK_CUR:
		pos += Pos();
		break;
	case SEEK_END:
		pos += ext;
		break;
	default:
		return KErrArgument;
		}
	TInt ret = KErrNone;
	if (pos < 0)
		{
		pos = 0;
		ret = KErrEof;
		}
	else if (pos > ext)
		{
		pos = ext;
		ret = KErrEof;
		}

	switch (iState)
		{
	case EAlloc:
		iPos = pos;
		break;
	case EReading:
		{
		TInt lag = iPos - pos;
		if (lag >= 0 && lag <= (iEnd - iBuffer))
			iPtr = iEnd - lag;
		else
			{
			iPtr = iEnd;
			iPos = pos;
			}
		}
		break;
	case EWriting:
		if (pos != Pos())
			{
			ret = Flush();
			if (ret == KErrNone)
				iPos = pos;
			}
		break;
		}
	offset = pos;
	return ret;
	}

void CFileDesc::MapStat(struct stat& st, const TTime& aModTime, TUint& aAttr)
    {
    st.st_mode = (aAttr&KEntryAttDir) ? S_IFDIR:S_IFREG;
    if ((aAttr&KEntryAttReadOnly)==0)
	st.st_mode |= S_IWUSR;
    st.st_nlink = 1;
    st.st_mtime = as_time_t(aModTime);
    st.st_blksize=512;
    }

TInt CFileDesc::FStat (struct stat* st)
	{
	TInt err;
	TUint att;
	TTime modtime;
	
	err = iFile.Att(att);
	if (!err)
	    {
	    err = iFile.Modified(modtime);
	    if (!err)
		{
		err=Ext();
		if (err >= 0)
		    {
		    st->st_size = err;
		    st->st_dev = st->st_rdev = iDrive;
		    MapStat(*st, modtime, att);
		    return 0;
		    }
		}
	    }
	return err;
	}

TInt CFileDesc::Alloc()
	{
	if (iSize)
		{
		iBuffer = new TUint8[iSize];
		if (iBuffer == 0)
			return KErrNoMemory;
		}
	return KErrNone;
	}

TInt CFileDesc::FileRead(TUint8* aPtr,TInt aLength)
	{
	TPtr8 ptr(aPtr,aLength);
	TInt r=iFile.Read(iPos,ptr);
	if (r == KErrNone)
		{
		r = ptr.Length();
		iPos += r;
		if (r < aLength)
			iExt = iPos;
		}
	return r;
	}

TInt CFileDesc::FileWrite(TUint8* aPtr,TInt aLength)
	{
	TPtrC8 ptr(aPtr,aLength);
	TInt r = iFile.Write(iPos,ptr);
	if (r == KErrNone)
		{
		iPos += aLength;
		if (iPos > iExt && iExt >= 0)
			iExt = iPos;
		}
	return r;
	}

TInt CFileDesc::Flush()
	{
	if (iPtr > iBuffer)
		{
		TInt r = FileWrite(iBuffer, iPtr-iBuffer);
		if (r < 0)
			return r;
		iPtr = iBuffer;
		}
	return KErrNone;
	}

TInt CFileDesc::DoRead (TDes8& aDesc)
	{
	if (iState != EReading)
		{
		TInt ret = (iState == EAlloc) ? Alloc() : Flush();
		if (ret != KErrNone)
			return ret;
		iState = EReading;
		iPtr = iEnd = iBuffer;
		}

	TUint8* p = (TUint8*) aDesc.Ptr();
	TInt max = aDesc.MaxLength();
	TInt avail = iEnd - iPtr;
	TInt len = Min(max, avail);
	if (len > 0)
		{
		p = Mem::Copy(p, iPtr, len);
		iPtr += len;
		max -= len;
		}
	if (max >= iSize)
		{
		TInt ret = FileRead(p, max);
		if (ret < 0)
			return ret;
		p += ret;
		}
	else if (max > 0)
		{
		TInt ret = FileRead(iBuffer, Min(max + EReadAhead, iSize));
		if (ret < 0)
			return ret;
		len = Min(max, ret);
		p = Mem::Copy(p, iBuffer, len);
		iPtr = iBuffer + len;
		iEnd = iBuffer + ret;
		}
	aDesc.SetLength(p-aDesc.Ptr());
	return KErrNone;
	}

void CFileDesc::Read (TDes8& aDesc, TRequestStatus& aStatus)
	{
	Complete(aStatus,DoRead(aDesc));
	}

TInt CFileDesc::DoWrite (TDes8& aDesc)
	{
	if (iState != EWriting)
		{
		if (iState == EAlloc)
			{
			TInt ret = Alloc();
			if (ret != KErrNone)
				return ret;
			}
		else
			iPos -= iEnd - iPtr;

		iState = EWriting;
		iPtr = iBuffer;
		iEnd = iBuffer + iSize;
		}

	TUint8* p = (TUint8*) aDesc.Ptr();
	TInt max = aDesc.Length();
	TInt avail = iEnd - iPtr;
	TInt len = Min(max, avail);
	if (len > 0)
		{
		iPtr = Mem::Copy(iPtr, p, len);
		p += len;
		max -= len;
		}
	if (max == 0)
		return KErrNone;
	TInt r=Flush();
	if (r < 0)
		return r;
	if (max >= iSize)
		return FileWrite(p, max);
	iPtr = Mem::Copy(iPtr, p, max);
	return KErrNone;
	}

void CFileDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	Complete(aStatus,DoWrite(aDesc));
	}

TInt CFileDesc::DoSync()
	{
	if (iState == EWriting)
		{
		TInt ret = Flush();
		if (ret < 0)
			return ret;
		}
	return iFile.Flush();
	}

void CFileDesc::Sync(TRequestStatus& aStatus)
	{
	Complete(aStatus,DoSync());
	}

TInt CFileDesc::Pos()
	{
	TInt pos = iPos;
	if (iState == EReading)
		pos -= (iEnd - iPtr);
	else if (iState == EWriting)
		pos += (iPtr - iBuffer);
	return pos;
	}

TInt CFileDesc::Ext()
	{
	if (iExt < 0)
		{
		TInt r = iFile.Size(iExt);
		if (r < 0)
			return r;
		}
	return Max(iExt, Pos());
	}

TInt CFileDesc::IoctlCompletion(int aCmd, void* aParam, TInt aStatus)
	{
	TInt ret=aStatus;
	if (ret!=KErrNone)
		return ret;
	// some useful sums about the current state of the file
	TInt curoff = Pos();
	TInt size = Ext();
	if (size < 0)
		ret = size;
	int *param=REINTERPRET_CAST(int*,aParam);
	switch (aCmd)
		{
	case E32IONREAD:
		if (ret==KErrNone)
			*param=(size-curoff);
		break;
	case E32IOSELECT:
		{
		int mask=E32SELECT_WRITE;
		if ((size-curoff)>0)
			mask |= E32SELECT_READ;
		*param=(*param)&mask;	// but don't block
		}
		break;
	default:
		ret=KErrNotSupported;
		break;
		}
	return ret;
	}

// Extra support for temporary files

TInt CTempFileDesc::Open(RFs& aSession, const TDesC& aPath)
	{
	iSession=aSession;
	iDrive=(TInt16)TDriveUnit(aPath);
	TInt err=iFile.Temp(aSession, aPath, iName, EFileShareAny);
	return err;
	}

TInt CTempFileDesc::FinalClose()
	{
	iFile.Close();
	TInt err=iSession.Delete(iName);
	return err;
	}
