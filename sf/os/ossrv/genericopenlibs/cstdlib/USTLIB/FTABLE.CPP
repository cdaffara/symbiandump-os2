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
// Handle a table of CFileDescBase pointers
// 
//

#include "SYSIF.H"
#include "FDESC.H"
#include "LTIME.H"
#include "LPOSIX.H"
#include <fcntl.h>
#include <sys/errno.h>

#define RESERVED(i)	((CFileDescBase*)((i)+1))

const TInt MINFILES = 8;

CFileTable::CFileTable() : iFids(MINFILES)
	{
	}

void CFileTable::InitL()
	{
	CFileDescBase* ptr=0;
	iFids.InsertL(0, ptr, 8);	// set up the first 8 entries with zero
	}

CFileTable::~CFileTable()
	{
	Close();
	}

void CFileTable::Close()
	{
	for (TInt aFid=0; aFid<iFids.Count(); aFid++)
		{
		if (iFids[aFid]!=0 && iFids[aFid]!=RESERVED(aFid))
			{
			iFids[aFid]->RecvFromCancel();
			iFids[aFid]->Close();
			}
		Release(aFid);
		}
	}

void CFileTable::Release(TInt aFid)
	{
	iFids[aFid]=0;
	if (aFid<MINFILES || aFid<iFids.Count()-1)
		return;
	// opportunity to shrink the array
	while (aFid>=MINFILES && iFids[aFid]==0)
		{
		iFids.Delete(aFid);
		aFid--;
		}
	iFids.Compress();
	}

void CFileTable::Default(CFileDescBase* aConsole)
	{
	if (iFids[0]==0)
		iFids[0]=aConsole->Dup();
	if (iFids[1]==0)
		iFids[1]=aConsole->Dup();
	if (iFids[2]==0)
		iFids[2]=aConsole->Dup();
	}

TInt CFileTable::Attach(TInt aFid, CFileDescBase* aFile)
	{
	if (aFid<0 || aFid>=iFids.Count() || (iFids[aFid] != 0 && iFids[aFid]!=RESERVED(aFid)))
		return KErrArgument;
	iFids[aFid]=aFile;
	return KErrNone;
	}

TInt CFileTable::Reserve()
	{
	TInt aFid=0;
	TInt err=KErrNone;
	for (aFid=0; aFid<iFids.Count(); aFid++)
		if (iFids[aFid]==0)
			goto success;
	TRAP(err,iFids.ExtendL());
	if (err!=KErrNone)
		return err;
	aFid=iFids.Count()-1;

success:
	iFids[aFid]=RESERVED(aFid);
	return aFid;
	}

TInt CFileTable::Reserve(int aFids[3])
	{
	TInt i=0;
	for (i=0; i<3; i++)
		{
		if (aFids[i]==-1)
			{
			TInt fd=Reserve();
			if (fd<0)
				{
				Detach(aFids);	// release the ones we did get
				return KErrInUse;
				}
			aFids[i]=fd;
			}
		}
	return KErrNone;
	}

TInt CFileTable::Detach(int aFids[3])
	{
	TInt i=0;
	for (i=0; i<3; i++)
		{
		if (aFids[i]>=0)
			Attach(aFids[i],0);
		}
	return KErrInUse;
	}

TInt CFileTable::Detach(TInt aFid, CFileDescBase*& aDetached)
	{
	if (aFid<0 || aFid>=iFids.Count())
		return KErrArgument;
	aDetached=iFids[aFid];
	Release(aFid);
	return KErrNone;
	}

TInt CFileTable::At(TInt aFid, CFileDescBase*& aFound) const
	{
	if (aFid<0 || aFid>=iFids.Count())
		return KErrArgument;
	aFound=iFids[aFid];
	if (aFound==0)
		return KErrNotFound;
	return KErrNone;
	}

TInt CFileTable::Dup(TInt& aFid)
	{
	if (aFid<0 || aFid>=iFids.Count())
		return KErrArgument;
	CFileDescBase* fdesc=iFids[aFid];
	if (fdesc==0)
		return KErrNotFound;
	TInt aFid2=0;
	for (aFid2=0; aFid2<iFids.Count(); aFid2++)
		if (iFids[aFid2]==0)
			{
			iFids[aFid2]=fdesc->Dup();
			aFid=aFid2;
			return KErrNone;
			}
	return KErrInUse;
	}

TInt CFileTable::Dup2(TInt aFid, TInt aFid2)
	{
	if (aFid<0 || aFid>=iFids.Count() || aFid2<0 || aFid2>=iFids.Count() || aFid==aFid2)
		return KErrArgument;
	CFileDescBase* fdesc=iFids[aFid];
	if (fdesc==0)
		return KErrNotFound;
	CFileDescBase* old=iFids[aFid2];
	iFids[aFid2]=fdesc->Dup();
	if (old)
		old->Close();
	return KErrNone;
	}

// Simple synchronous services

int CFileTable::dup (int fid, int& anErrno)
	{
	TInt ret=Dup(fid);
	if (ret==KErrNone)
		return fid;
	return MapError(ret,anErrno);
	}

int CFileTable::dup2 (int fid, int fid2, int& anErrno)
	{
	TInt err=Dup2(fid,fid2);
	return MapError(err,anErrno);
	}
	
//int CFileTable::open (const char* name, int mode, int perms, int& anErrno, RFs& aFs)
//int CFileTable::open (const wchar_t* name, int mode, int perms, int& anErrno, RFs& aFs)
int CFileTable::open (const wchar_t* name, int mode, int perms, int& anErrno, RSessionBase& aFs)
	{
	int fd=Reserve();
	TInt err=fd;
	if (fd>=0)
		{
		//coverity[alloc_fn]
		//coverity[var_assign]
		CFileDescBase *f=CFileDescBase::Open(aFs,name,mode,perms,err);
		if (!err)
			{
			err=Attach(fd,f);
			if (!err)
				return fd;
			delete f;
			}
		Attach(fd,0);	// cancel the reservation
		//coverity[leaked_storage]
		}
	return MapError(err,anErrno);
	}

int CFileTable::socket (int family, int style, int protocol, int& anErrno, RSocketServ& aSs)
	{
	int fd=Reserve();
	TInt err=fd;
	if (fd>=0)
		{
		CFileDescBase *f=CFileDescBase::Socket(aSs,family,style,protocol,err);
		if (!err)
			{
			CleanupStack::PushL(f);
			err=Attach(fd,f);
			CleanupStack::PopAndDestroy(f);
			
			if (!err)
				{
				return fd;	
				}
			
			}
		
		Attach(fd,0);	// cancel the reservation
		}

	return MapError(err,anErrno);
	}

int CFileTable::userclose(int fid, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		{
		f->UserClose();
		close(fid, anErrno);
		}
	else
		{
		if (KErrNotFound == err)
			err = EBADF;
		}
	return MapError(err,anErrno);
	}

int CFileTable::close (int fid, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=Detach(fid,f);
	if (!err)
	   {
		if (f==0)
		  {
			err=EBADF;
		  }
		else
		  {
		    f->RecvFromCancel();
			err=f->Close();
		  }
	   }
	return MapError(err,anErrno);
	}



int CFileTable::lseek (int fid, int offset, int whence, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		{
		err=f->LSeek(offset, whence);
		if (err==KErrNone)
			return offset;
		}
	return MapError(err,anErrno);
	}

int CFileTable::fstat (int fid, struct stat *st, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		err=f->FStat(st);
	return MapError(err,anErrno);
	}

int CFileTable::listen (int fid, int n, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		err=f->Listen(n);
	return MapError(err,anErrno);
	}

int CFileTable::bind (int fid, TSockAddr& address, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		err=f->Bind(address);
	return MapError(err,anErrno);
	}

int CFileTable::sockname (int fid, TSockAddr& address, int anEnd, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		err=f->SockName(anEnd, address);
	return MapError(err,anErrno);
	}

int CFileTable::getsockopt (int fid, int level, int opt, void* buf, unsigned long* len, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		{
		TPtr8 ptr((TText8 *)buf, *len, *len);
		err=f->GetSockOpt(opt,level,ptr);
		if (err==0)
			*len=ptr.Length();
		}
	return MapError(err,anErrno);
	}

int CFileTable::setsockopt (int fid, int level, int opt, void* buf, unsigned long len, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (err==KErrNone)
		{
		//Negative option is not supported and have not been implemented yet
		//anErrno is then mapped to ENOSYS(88) (function not implemented yet)
		if (opt<=0)
			return MapError(KErrNotSupported,anErrno);
		TPtrC8 ptr((TText8 *)buf, len);
		err=f->SetSockOpt(opt,level,ptr);
		}
	return MapError(err,anErrno);
	}

int CFileTable::ioctlcomplete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		err=f->IoctlCompletion(cmd,param, aStatus.Int());
	return MapError(err,anErrno);
	}

int CFileTable::ioctlcancel (int fid, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		f->IoctlCancel();
	return MapError(err,anErrno);
	}

// Preparation for an Asynchronous service
//
// The CFileDescBase* should be Closed after completion: the use of Dup prevents other
// clients from invalidating the pointer during an asynchronous operation.

TInt CFileTable::Asynch (int fid, CFileDescBase*& aFile)
	{
	TInt err=At(fid,aFile);
	if (!err)
		aFile->Dup();
	return err;
	}

