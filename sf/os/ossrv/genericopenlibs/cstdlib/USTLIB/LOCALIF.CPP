// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SYSIF.H"
#include "FDESC.H"
#include "LTIME.H"
#include "LPOSIX.H"
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/serial.h>

// System Interface for thread-local form of STDLIB

/** 
Two-phase constructor method to create a CLocalSystemInterface instance.
*/
CLocalSystemInterface* CLocalSystemInterface::NewL()
	{
	CLocalSystemInterface* self = new (ELeave) CLocalSystemInterface();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor.
*/	
CLocalSystemInterface::CLocalSystemInterface()
	{
	}

/**
Second-phase constructor.
Initialises some data members.
This function leaves if any system-wide error happens.
*/	
void CLocalSystemInterface::ConstructL()		
	{
	iFids.InitL();
	
	CTtyDesc* console=new CTtyDesc;
	User::LeaveIfNull(console);
	
	iFids.Default(console);
	console->Close();

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(PosixFilesystem::SetDefaultDir(iFs));
	}

CLocalSystemInterface::~CLocalSystemInterface()
//
// Shut down all server connections in use
//
	{
	iFids.Close();
	iCs.Close();
	iSs.Close();
	iFs.Close();
	}

MSystemInterface& CLocalSystemInterface::Clone()
	{
	iUseCount++;
	return *this;
	}

void CLocalSystemInterface::Release()
	{
	if (--iUseCount<0)
		delete this;
	}

void CLocalSystemInterface::TerminateProcess(int status)
	{
	delete this;
	RProcess().Terminate(status);
	}


// Simple layer over PosixFilesystem

wchar_t * CLocalSystemInterface::getcwd (wchar_t* buf, unsigned long len, int& anErrno)
	{
	return PosixFilesystem::getcwd(iFs,buf,len,anErrno);
	}

int CLocalSystemInterface::chdir (const wchar_t* aPath, int& anErrno)
	{
	return PosixFilesystem::chdir(iFs,aPath,anErrno);
	}

int CLocalSystemInterface::rmdir (const wchar_t* aPath, int& anErrno)
	{
	return PosixFilesystem::rmdir(iFs,aPath,anErrno);
	}

int CLocalSystemInterface::mkdir (const wchar_t* aPath, int perms, int& anErrno)
	{
	return PosixFilesystem::mkdir(iFs,aPath,perms,anErrno);
	}

int CLocalSystemInterface::stat (const wchar_t* name, struct stat *st, int& anErrno)
	{
	return PosixFilesystem::stat(iFs,name,st,anErrno);
	}

int CLocalSystemInterface::chmod (const wchar_t* name, int perms, int& anErrno)
	{
	return PosixFilesystem::chmod(iFs,name,perms,anErrno);
	}

int CLocalSystemInterface::unlink (const wchar_t* name, int& anErrno)
	{
	return PosixFilesystem::unlink(iFs,name,anErrno);
	}

int CLocalSystemInterface::rename (const wchar_t* oldname, const wchar_t* newname, int& anErrno)
	{
	return PosixFilesystem::rename(iFs,oldname,newname,anErrno);
	}


TInt CLocalSystemInterface::ResolvePath (TParse& aResult, const wchar_t* path, TDes* aFilename)
	{
	return PosixFilesystem::ResolvePath(iFs, aResult, path, aFilename);
	}

// Simple layer over CFileTable synchronous routines

int CLocalSystemInterface::open (const wchar_t* name, int mode, int perms, int& anErrno)
	{
	//at this point we need to know if it is a serial port or file
	if	((L'C' == name[0]) && (L'O' == name[1]) && (L'M' == name[2]) && (L':' == name[4]) && ((name[3] >= L'1') && (name[3] <= L'9')) ||
			(L'I' == name[0]) && (L'R' == name[1]) && (L'C' == name[2]) && (L'O' == name[3]) && (L'M' == name[4]) && (L':' == name[6]) && ((name[5] >= L'1') && (name[5] <= L'9')))
		return iFids.open(name,mode,perms,anErrno, iCs);
	else
		return iFids.open(name,mode,perms,anErrno, iFs);
	}

int CLocalSystemInterface::dup (int fid, int& anErrno)
	{
	return iFids.dup(fid,anErrno);
	}

int CLocalSystemInterface::dup2 (int fid, int fid2, int& anErrno)
	{
	return iFids.dup2(fid,fid2,anErrno);
	}
	
int CLocalSystemInterface::close (int fid, int& anErrno)
	{
//	return iFids.close(fid,anErrno);
	return iFids.userclose(fid,anErrno);
	}

int CLocalSystemInterface::lseek (int fid, int offset, int whence, int& anErrno)
	{
	return iFids.lseek(fid,offset,whence,anErrno);
	}

int CLocalSystemInterface::fstat (int fid, struct stat *st, int& anErrno)
	{
	return iFids.fstat(fid,st,anErrno);
	}

int CLocalSystemInterface::socket (int family, int style, int protocol, int& anErrno)
	{
	return iFids.socket(family,style,protocol,anErrno,iSs);
	}

int CLocalSystemInterface::listen (int fid, int n, int& anErrno)
	{
	return iFids.listen(fid,n,anErrno);
	}

int CLocalSystemInterface::bind (int fid, struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	TUSockAddr address(addr,size);
	return iFids.bind(fid,address,anErrno);
	}

int CLocalSystemInterface::sockname (int fid, struct sockaddr* addr, unsigned long* size, int anEnd, int& anErrno)
	{
	TUSockAddr address(addr);
	TInt ret=iFids.sockname(fid,address,anEnd,anErrno);
	if (ret==0)
		address.Get(addr,size);
	return ret;
	}

int CLocalSystemInterface::getsockopt (int fid, int level, int opt, void* buf, unsigned long* len, int& anErrno)
	{
	return iFids.getsockopt(fid,level,opt,buf,len,anErrno);
	}

int CLocalSystemInterface::setsockopt (int fid, int level, int opt, void* buf, unsigned long len, int& anErrno)
	{
	return iFids.setsockopt(fid,level,opt,buf,len,anErrno);
	}

wchar_t* CLocalSystemInterface::getenv (const wchar_t* name)
	{
	return iEnv.getenv(name);
	}

void CLocalSystemInterface::unsetenv (const wchar_t* name)
	{
	iEnv.unsetenv(name);
	}

int CLocalSystemInterface::setenv (const wchar_t* name, const wchar_t* value, int rewrite, int& anErrno)
	{
	return iEnv.setenv(name,value,rewrite,anErrno);
	}


int CLocalSystemInterface::popen3 (const wchar_t *, const wchar_t *, const wchar_t *, wchar_t**, int [3], int& anErrno)
	{
	return MapError(KErrNotSupported,anErrno);
	}

int CLocalSystemInterface::waitpid (int, int*, int, int& anErrno)
	{
	return MapError(KErrNotSupported,anErrno);
	}

// Synchronous layer over CFileTable asynchronous routines

int CLocalSystemInterface::read (int fid, char* buf, unsigned long len, int& anErrno)
	{
	CFileDescBase* f=0;
	TBool patchErr = EFalse;

	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus readStatus;
		TRequestStatus timerStatus(KRequestPending);
		RTimer theTimer;
		TBool timerRunning = EFalse;

		TPtr8 ptr((TText8 *)buf, len);

		if (f->TimedRead())
			{
			TTimeIntervalMicroSeconds32 timeout(f->TimeoutValue()*1000);
			theTimer.CreateLocal();
			theTimer.After(timerStatus, timeout);
			timerRunning = ETrue;
			}

		f->Read(ptr, readStatus);

		User::WaitForRequest(readStatus, timerStatus);

		if (timerRunning)
			{
			if (timerStatus.Int() != KRequestPending)
				{
				//cancel the read and wait for it
				f->ReadCancel();
				patchErr = ETrue;	//report this as a timeout not a cancel!!
				User::WaitForRequest(readStatus);
				}
			else
				{
				//if the timer was in operation
				//cancel the timer
				theTimer.Cancel();
				User::WaitForRequest(timerStatus);
				}
			theTimer.Close();
			}

		err=f->ReadCompletion(ptr, readStatus.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err==0)
			return ptr.Length();
		}
	if (patchErr)
		err = ETIMEDOUT;

	return MapError(err,anErrno);
	}

int CLocalSystemInterface::write (int fid, const char* buf, unsigned long len, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TPtr8 ptr((TText8 *)buf, len, len);
		f->Write(ptr,status);
		User::WaitForRequest(status);
		err=f->WriteCompletion(ptr, status.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err==0)
			return ptr.Length();
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::recvfrom (int fid, char* buf, unsigned long len, int flags, struct sockaddr* from, unsigned long* fromsize, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TPtr8 ptr((TText8 *)buf, len);
		TUSockAddr addr(from);
		f->RecvFrom(ptr,addr,flags,status);
		User::WaitForRequest(status);
		TInt ret=0;
		err=f->RecvFromCompletion(ret, status.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err==0)
			{
			addr.Get(from,fromsize);
			return ptr.Length();
			}
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::sendto (int fid, const char* buf, unsigned long len, int flags, struct sockaddr* to, unsigned long tosize, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TPtr8 ptr((TText8 *)buf, len, len);
		TUSockAddr addr(to,tosize);
		f->SendTo(ptr,addr,flags,status);
		User::WaitForRequest(status);
		err=f->SendToCompletion(ptr, status.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err==0)
			return ptr.Length();
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::fsync (int fid, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		f->Sync(status);
		User::WaitForRequest(status);
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		err=status.Int();
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::shutdown (int fid, int how, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		f->Shutdown(how,status);
		User::WaitForRequest(status);
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		err=status.Int();
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::connect (int fid, struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TUSockAddr address(addr,size);
		f->Connect(address,status);
		User::WaitForRequest(status);
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		err=status.Int();
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::accept (int fid, int& anErrno)
//
// The CSocketDesc performing the Accept is responsible for creating the new CSocketDesc
//
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		CSocketDesc* newf=0;
		int fd=iFids.Reserve();
		err=fd;
		if (fd>=0)
			{
			TRequestStatus status;
			f->Accept(newf,status,iSs);
			CleanupStack::PushL(newf);
			User::WaitForRequest(status);
			f->Close();	// balances the Dup() in CFileTable::Asynch()
			err=status.Int();
			if (!err)
				{
				err=iFids.Attach(fd,newf);
				if (!err)
					{	
					CleanupStack::PopAndDestroy(newf);
					return fd;
					}
				}
			iFids.Attach(fd,0);	// cancel the reservation
			CleanupStack::PopAndDestroy(newf);
			}
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::ioctl (int fid, int cmd, void* param, int& anErrno)
	{
	TRequestStatus ioctlStatus;
	TInt err=ioctl(fid,cmd,param,ioctlStatus,anErrno);
	if (err==KErrNone)
		{
		User::WaitForRequest(ioctlStatus);
		err=ioctl_complete(fid,cmd,param,ioctlStatus,anErrno);
		}
	return err;
	}

// C++ version of asynchronous ioctl

int CLocalSystemInterface::ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		f->Ioctl(cmd,param,aStatus);
		f->Close();	// balances the Dup() in CFileTable::Asynch() - live dangerously!
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	return iFids.ioctlcomplete(fid,cmd,param,aStatus,anErrno);
	}

int CLocalSystemInterface::ioctl_cancel (int fid, int& anErrno)
	{
	return iFids.ioctlcancel(fid,anErrno);
	}
