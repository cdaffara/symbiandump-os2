/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <fcntl.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <wchar.h>
#include <rpipe.h>
#include <sys/syslimits.h>  // for checking maximum no of FDs that can be opened
#include "sysif.h"
#include "fdesc.h"
#include "ltime.h"
#include "lposix.h"
#include "systemspecialfilercg.h"
#include "link.h"
#include "spawn_r.h"
#include "fdtransfer_srv.h"
#include "fdtransfer_cli.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <utf.h>

#define RESERVED(i)	((CFileDescBase*)((i)+1))
#define IS_SUPPORTED(filetype) ( filetype == EFileTypeGeneral || filetype == EFileTypeMkFifo )

static const TInt KMinFds = 8;
static const TInt KMinConnections = 8;
static const TInt KPopen3Fds = 3;

static const TInt KDefaultPipeSz = 512;
static const TInt KPipeReadHandle = 0;
static const TInt KPipeWriteHandle = 1;

enum TChildType
	{
	EPopenChild,
	EPopen3Child,
	ESpawnChild,
	ESystemChild
	};

enum
	{
	// for all operations
	ESlotParentId = 0x8,  //CApaCommandLine::EFirstEnvironmentSlotForPublicUse  apacmdln.h
	ESlotChildType,
	ESlotSemaphore,
	// for popen, popen3
	ESlotStdPipe1 = 0xb,
	// for posix_spawn
	ESlotActions = 0xb,
	// for posix_spawn and popen3
	ESlotEnv = 0xc,
	// for popen3
	ESlotStdPipe2 = 0xd,
	ESlotStdPipe3,
	ESlotSignalMask

	};

CFileTable::CFileTable() : iFids(KMinFds),
						   iRConnectionList(KMinConnections)
	{
	// nada
	}

TInt CFileTable::Init(RHeap* aHeap)
	{
	iPrivateHeap = aHeap;
	CFileDescBase* ptr = NULL;
	//Create FID Lock
	TInt retVal = iFidLock.CreateLocal();
	//Create iRConnectionList Lock
	retVal |= iRcLock.CreateLocal();

	if (!retVal)
		{
		// set up the first KMinFds entries with zero
		TRAP(retVal, iFids.InsertL(0, ptr, KMinFds));
		}
	return retVal;
	}

CFileTable::~CFileTable()
	{
	Close();
	DeleteRConnectionList();

	RHeap* oHeap = User::SwitchHeap(iPrivateHeap);
	iRcLock.Close();
	TInt count = iChildren.Count();
	for(TInt i = 0; i < count; i++)
		{
		iChildren[i].iProc.Close();
		}
	iFidLock.Close();
	iChildren.Close();
	iFids.Reset();
	iCleanup.SaveUserHeap(oHeap);
	}

void CFileTable::Close()
	{
	TInt count = iFids.Count();
	TInt aFid = 0;
	for (; aFid<count; aFid++)
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
	//Lock the resource before using it and unlock later
	iFidLock.Wait();

	iFids[aFid]=0;
	if (aFid<KMinFds || aFid<iFids.Count()-1)
		{
		iFidLock.Signal();
		return;
		}

	// Switch to the backend heap
	RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);

	// opportunity to shrink the array
	while (aFid>=KMinFds && iFids[aFid]==0)
		{
		iFids.Delete(aFid);
		aFid--;
		}
	iFids.Compress();

	// reset to the thread's default heap
	User::SwitchHeap(oldHeap);
	iFidLock.Signal();
	}

void CFileTable::Default(CFileDescBase* aRedirMedia, CFileDescBase* aStdErrMedia)
	{
	if (iFids[0]==0)
		iFids[0]=aRedirMedia->Dup();
	if (iFids[1]==0)
		iFids[1]=aRedirMedia->Dup();
	if (iFids[2]==0)
		iFids[2]=aStdErrMedia->Dup();
	}

TInt CFileTable::Attach(TInt aFid, CFileDescBase* aFile)
	{
	//Lock the resource before using it and unlock later
	iFidLock.Wait();

	if( aFid<0 || aFid>=iFids.Count() || (iFids[aFid] != 0 && iFids[aFid] != RESERVED(aFid)) )
		{
		iFidLock.Signal();
		return KErrArgument;
		}

	iFids[aFid] = aFile;
	iFidLock.Signal();
	return KErrNone;
	}

TInt CFileTable::Reserve()
	{
	TInt aFid = 0;
	TInt err=KErrNone;

	//Lock the resource before using it and unlock later
	iFidLock.Wait();
	TInt count = iFids.Count();

	//Check, is there any free slots in the Table
	for (; aFid<count; aFid++)
		if (iFids[aFid]==0)
			goto success;

	//Check whether we can open more FDs for this process
	if(OPEN_MAX > iFids.Count())
		{
		// Do a switch heap now to make sure that, all the heap allocation will happen
		// with in our private heap
		RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);

		TRAP(err,iFids.ExtendL());
		//Set back the threads default heap
		User::SwitchHeap(oldHeap);

		if (err!=KErrNone)
			{
			iFidLock.Signal();
			return err;
			}
		aFid=iFids.Count()-1;
		}
	else
		{
		iFidLock.Signal();
		//We have reached the maximum limit allowed
		return KErrMaxFdOpened;
		}

success:
	iFids[aFid]=RESERVED(aFid);
	iFidLock.Signal();
	return aFid;
	}

TInt CFileTable::Reserve(int aFids[KPopen3Fds])
	{
	TInt i=0;
	for (i=0; i<KPopen3Fds; i++)
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

TInt CFileTable::Detach(int aFids[KPopen3Fds])
	{
	TInt i=0;
	for (i=0; i<KPopen3Fds; i++)
		{
		if (aFids[i]>0)
			Attach(aFids[i],0);
		}
	return KErrInUse;
	}

TInt CFileTable::Detach(TInt aFid, CFileDescBase*& aDetached)
	{
	if (aFid<0 || aFid>=iFids.Count())
		return KErrBadHandle;
	aDetached=iFids[aFid];
	Release(aFid);
	return KErrNone;
	}

TInt CFileTable::At(TInt aFid, CFileDescBase*& aFound) const
	{
	if (aFid<0 || aFid>=iFids.Count())
		return KErrBadHandle;
	aFound=iFids[aFid];
	if (aFound==0 || aFound == RESERVED(aFid))
		return KErrBadHandle;
	return KErrNone;
	}

int CFileTable::dup(int fid, int& anErrno)
	{
	TInt newfd = Dup(fid);
	if (newfd < KErrNone)
		{
		return MapError(newfd, anErrno);
		}
	return newfd;
	}

// -----------------------------------------------------------------------------
// CFileTable::Dup
// Invoke DupFd with the aFid and 0
// -----------------------------------------------------------------------------
TInt CFileTable::Dup(TInt aFid)
	{
	if (aFid<0 || aFid>=iFids.Count())
		return KErrBadHandle;

	CFileDescBase* fdesc;
	// Validate aFid
	if (At(aFid, fdesc) != KErrNone)
		{
		return KErrBadHandle;
		}

	return DupFd(aFid, 0);
	}

int CFileTable::dup2(int fid1, int fid2, int& anErrno)
	{
	// Invoke Dup2. If there is a valid descriptor at fid2, ensure it is closed
	TInt newfd = Dup2(fid1, fid2, ETrue);
	if (newfd < KErrNone)
		{
		return MapError(newfd, anErrno);
		}
	return newfd;
	}

// -----------------------------------------------------------------------------
// CFileTable::Dup2
// Invoked by both dup2 and fcntl with F_DUPFD
// When invoked from fcntl, aCloseFid2 is FALSE and Dup2 does not close the
// descriptor at aFid2.
// When invoked from dup2, aCloseFid2 is TRUE and Dup2 closes the descriptor
// at aFid2 before invoking DupFd
// -----------------------------------------------------------------------------
TInt CFileTable::Dup2(TInt aFid1, TInt aFid2, TBool aCloseFid2)
	{
	if (aFid1<0 || aFid1>=iFids.Count() || aFid2<0 || aFid2 > OPEN_MAX)
		return KErrBadHandle;

	CFileDescBase* fdesc;
	// Validate aFid
	if (At(aFid1, fdesc) != KErrNone)
		{
		return KErrBadHandle;
		}

	if (aCloseFid2)
		{
		// Invoked from dup2
		if (aFid1 == aFid2)
			{
			return aFid2;
			}

		// Validate aFid2 and close descriptor, if found.
		if (At(aFid2, fdesc) == KErrNone)
			{
			fdesc->Close();
			iFids[aFid2] = 0;
			}
		}

	return DupFd(aFid1, aFid2);
	}

// -----------------------------------------------------------------------------
// CFileTable::DupFd
// Invoked by CFileTable::Dup and CFileTable::Dup2
// Return a new fd at slot >= aFid2 refering to the same CFileDescBase object as
// the one at aFid1.
// -----------------------------------------------------------------------------
TInt CFileTable::DupFd(TInt aFid1, TInt aFid2)
	{
	// Arguments have been validated in the calling methods
	TInt err = KErrNone;
	CFileDescBase* fdesc = iFids[aFid1];

	iFidLock.Wait();
	TInt count = iFids.Count();
	for (; aFid2<count; aFid2++)
		if (iFids[aFid2]==0)
			{
			iFids[aFid2]=fdesc->Dup();
			iFidLock.Signal();
			return aFid2;
			}
	iFidLock.Signal();
	//Expand the FTable
	TRAP(err,ExpandFTableL(aFid2+1));
	//Handle error case
	if (KErrNone == err)
		{
		iFidLock.Wait();
		iFids[aFid2] = fdesc->Dup();
		iFidLock.Signal();
		return aFid2;
		}
	return err;
	}

int CFileTable::open (const wchar_t* name, int mode, int perms, int& anErrno)
	{
	int fd=Reserve();
	TInt err=fd;
	if (fd>=0)
		{
		RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
		CFileDescBase *f=CFileDescBase::Open(name, mode, perms, err);
		User::SwitchHeap(oldHeap);

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

	// KErrNotReady, here typically means filesystem not mounted or the mentioned
	// drive doesnt exist. As KErrNotReady cannot be directly mapped to the existing
	// posix error codes, ENOENT would be the appropriate error to return in this scenario.
	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}

	if (err == KErrPathNotFound)
		{
		RFs& rfs = Backend()->FileSession();
		err = Find_Error(name, rfs);
		}

	return MapError(err,anErrno);
	}

int CFileTable::socket (int family, int style, int protocol, int& anErrno, RSocketServ& aSs)
	{
	int fd=Reserve();
	TInt err=fd;
	if (fd>=0)
		{
		CFileDescBase* f;
		RHeap* oldHeap;
			oldHeap = User::SwitchHeap(iPrivateHeap);
			f = CFileDescBase::Socket(aSs, family, style, protocol, err);

		User::SwitchHeap(oldHeap);

		if (err == KErrNone)
			{
			err=Attach(fd,f);
			if (err == KErrNone)
				{
				f->SetFids((void*)this);
				return fd;
				}
			delete f;
			}
		Attach(fd,0);	// cancel the reservation
		}
	return MapError(err,anErrno);
	}

int CFileTable::UserClose(int fid, int& anErrno)
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
			err=EBADF;
		else
			f->RecvFromCancel();
			err=f->Close();
		}
	return MapError(err,anErrno);
	}

off_t CFileTable::lseek (int fid, off_t offset, int whence, int& anErrno)
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
		{
		memset(st,0,sizeof(struct stat));
		err=f->FStat(st);
		}
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

int CFileTable::bind (int fid, const struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		{
		err=f->Bind(addr,size);
		}

	if (err == KErrArgument)
		{
		anErrno = EADDRNOTAVAIL;
		return -1;
		}
	if (err == KErrNotFound)
		{
		anErrno = EADDRNOTAVAIL;
		return -1;
		}
	if (err == KErrAlreadyExists || err == KErrInUse)
		{
		anErrno = EADDRINUSE;
		return -1;
		}
	return MapError(err,anErrno);
	}

int CFileTable::sockname (int fid, struct sockaddr* address, unsigned long* aAddrLen, int anEnd, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (!err)
		err=f->SockName(anEnd, address,aAddrLen);
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
		// SOS returns KErrNotSupported or KErrAccessDenied for wrong options, Linux requires ENOPROTOOPT always
		if (err == KErrNotSupported || err == KErrAccessDenied)
			{
			anErrno = ENOPROTOOPT;
			return -1;
			}
		}
	return MapError(err,anErrno);
	}

int CFileTable::setsockopt (int fid, int level, int opt, void* buf, unsigned long len, int& anErrno)
	{
	CFileDescBase *f=0;
	TInt err=At(fid,f);
	if (err==KErrNone)
		{
		TPtrC8 ptr((TText8 *)buf, len);
		err=f->SetSockOpt(opt,level,ptr);
		// SOS returns KErrNotSupported or KErrAccessDenied for wrong options, Linux requires ENOPROTOOPT always
		if (err == KErrNotSupported || err == KErrAccessDenied)
			{
			anErrno = ENOPROTOOPT;
			return -1;
			}
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

// -----------------------------------------------------------------------------
// CFileTable::fcntl
// Implementation for fcntl
// This API will route the fcntl to corresponding Descriptor (pipe, file, socket)
// -----------------------------------------------------------------------------
//
int CFileTable::fcntl (int aFid, int aCmd, long anArg, int& anErrno)
	{
	CFileDescBase* f = NULL;
	TInt err = At(aFid, f);
	if (!err)
		{
		//fcntl supports only F_SETFL and F_GETFL for Non-Blocking I/O
		//If aCmd and anArg does not match these, return with Error
		switch( aCmd )
			{
			case F_SETFL:
				{
				err = f->Fcntl(anArg, aCmd);
				break;
				}
			case F_GETFL:
				{
				err = f->Fcntl(anArg, aCmd);
				//Check for return value.
				if (err == KErrNotFound)
					{
					err = MapError(KErrNotSupported, anErrno);
					}
				break;
				}
			case F_GETFD:
				{
				err = f->Fcntl(anArg, aCmd);
				break;
				}
			case F_SETFD:
				{
				err = f->Fcntl(anArg, aCmd);
				break;
				}
			case F_DUPFD:
				{
				// Invoke Dup2. Don't close descriptor at anArg, if any.
				err = Dup2(aFid, anArg, EFalse);
				break;
				}
			case F_GETLK:
				{
				if( anArg == NULL)
				err = KErrArgument;
				else
				err = f->Fcntl(anArg, aCmd);
				break;
				}
			case F_SETLK:
				{
				if( anArg == NULL)
				err = KErrArgument;
				else
				err = f->Fcntl(anArg, aCmd);
				break;
				}
			default:
				err = KErrArgument;
			}
		}
	//Handle Error case here
	if (err <  0)
		{
		err = MapError(err, anErrno);
		}

	return err;
	}

// Preparation for an Asynchronous service
//
// The CFileDescBase* should be Closed after completion: the use of Dup prevents other
// clients from invalidating the pointer during an asynchronous operation.

TInt CFileTable::Asynch(int fid, CFileDescBase*& aFile)
	{
	TInt err=At(fid,aFile);
	if (!err)
		aFile->Dup();
	return err;
	}

// -----------------------------------------------------------------------------
// CFileTable::PassToChild
// -----------------------------------------------------------------------------
inline void CFileTable::TransferToChild(RProcess& aChild, TInt aFds[], TInt aWhich, TInt aSlot)
	{
	CFileDescBase *pdesc = NULL;
	TInt err = 0;

	// Obtain file descriptor at aFds[aWhich]
	if(At(aFds[aWhich], pdesc) == KErrNone)
		{
		// Pass in the pipe handle to child
		aChild.SetParameter(aSlot, ((CPipeDescBase*)pdesc)->Handle());
		}
		
	// Close this handle in parent. Ignore error?
	UserClose(aFds[aWhich], err);

	// Ensure the other end of the pipe is not inherited to child
	if(At(aFds[!aWhich], pdesc) == KErrNone)
		{
		pdesc->SetAttributes(KCloseonExec, ETrue);
		}
	}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
void CFileTable::TransferSigMaskToChild(RProcess& childproc,TDes& sigmask)
	{
 	sigmask.AppendNum(Backend()->GetSigMask(),EHex);
	childproc.SetParameter(ESlotSignalMask,sigmask);
	}
#endif

// -----------------------------------------------------------------------------
// CFileTable::methods to add to and remove from the children array
// -----------------------------------------------------------------------------
inline void CFileTable::AddChild(const TChild& aChild)
	{
	RHeap* oldheap = User::SwitchHeap(iPrivateHeap);
	iChildren.Append(aChild);
	iChildren[iChildren.Count()-1].iProc.Duplicate(RThread());
	User::SwitchHeap(oldheap);
	}

inline void CFileTable::RemoveChild(TInt aIdx)
	{
	RHeap* oldheap = User::SwitchHeap(iPrivateHeap);
	iChildren[aIdx].iProc.Close();
	iChildren.Remove(aIdx);
	User::SwitchHeap(oldheap);
	}

// -----------------------------------------------------------------------------
// CFileTable::popen
// -----------------------------------------------------------------------------
int CFileTable::popen(const wchar_t* aFile, const wchar_t* aCmd,
		const char* aMode, int& anErrno)
	{
	TInt err = KErrNone;

	int fds[2];
	err = pipe(fds, anErrno);

	if (err != KErrNone)
		{
		return -1;
		}

	RProcess childproc;
	if (aCmd)
		{
		err = childproc.Create(TPtrC((TText*)aFile), TPtrC((TText*)aCmd));
		}
	else
		{
		err = childproc.Create(TPtrC((TText*)aFile), KNullDesC);
		}

	if (err != KErrNone)
		{
		MapError(err, anErrno);

		// close pipe handles and return
		ClosePipe(fds, err);
		return -1;
		}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	// Request for notification when child exits
	Backend()->RegisterExitNotification(childproc.Id());
#endif

   	// Pass process id of parent process to the child process
   	childproc.SetParameter(ESlotParentId, RProcess().Id());
   	// Pass in the child type
   	childproc.SetParameter(ESlotChildType, EPopenChild);

	// Pass in the appropriate pipe handle to the child.
	if (aMode[0] == 'r')
		{
		TransferToChild(childproc, fds, KPipeWriteHandle, ESlotStdPipe1);
		}
	else
		{
		TransferToChild(childproc, fds, KPipeReadHandle, ESlotStdPipe1);
		}

	// begin critical section
	iFidLock.Wait();
	RSemaphore sem;
	err = sem.CreateGlobal(KNullDesC, 0);
	if (err != KErrNone)
	{
	// Unable to create semaphore. Abandon it all!
	   		anErrno = ECHILD;
			childproc.Terminate(0);
	   		childproc.Close();

	   		// close pipe handles
	   		ClosePipe(fds, err);
	   		// give up lock
	   		iFidLock.Signal();
	   		return -1;
	}
	// Pass in the semaphore
	childproc.SetParameter(ESlotSemaphore, sem);  //Pass the created one

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	TBuf<16> sigmask; //Will be alive till copied by child thanks to fd_transfer server
	TransferSigMaskToChild(childproc,sigmask);
#endif

   	childproc.Resume();
   	TBuf<20> name(KFDTransferSvr);
   	name.AppendNum(childproc.Id(), EHex);
	DoTransferAll(sem,name);


	TChild tchild(childproc);
	// Store the in-parent fd
	tchild.iFid = (aMode[0] == 'r')? fds[KPipeReadHandle] : fds[KPipeWriteHandle];

	// Add to iChildren array and close handle

	AddChild(tchild);

	// end critical section
	iFidLock.Signal();
	sem.Close();
	childproc.Close();
	// The TProcessId will be converted to a TUint
	return tchild.iFid;
	}

// -----------------------------------------------------------------------------
// CFileTable::pclose
// -----------------------------------------------------------------------------
int CFileTable::pclose(int aFid, int& anErrno)
	{
	RProcess child;

	TInt idx = FindChild(aFid);
	if (idx == KErrNotFound)
		{
		// aFid not found among popen-ed children.
		anErrno = EINVAL;
		return -1;
		}

	if (child.Open((GetChildren())[idx].iPid) != KErrNone)
		{
		// the child is already dead
		RemoveChild(idx);

		// Unable to obtain status
		anErrno = ECHILD;
		return -1;
		}

	TRequestStatus status;
	child.Logon(status);

	// Wait for child to die
	User::WaitForRequest(status);
	child.Close();
	RemoveChild(idx);

	if (status.Int() == KErrNoMemory)
		{
		// Unable to obtain status
		anErrno = ECHILD;
		return -1;
		}

	return status.Int();
	}

// -----------------------------------------------------------------------------
// CFileTable::popen3
// -----------------------------------------------------------------------------
int CFileTable::popen3 (const wchar_t *aFile, const wchar_t *aCmd,
						wchar_t** aEnvp, int aFids[3], int& anErrno)
	{
	TInt err = KErrNone;


	int infds[2];
	err = pipe(infds, anErrno);
	if (err != KErrNone)
		{
		return -1;
		}

	int outfds[2];
	err = pipe(outfds, anErrno);
	if (err != KErrNone)
		{
		ClosePipe(infds, err);
		return -1;
		}

	int errfds[2];
	err = pipe(errfds, anErrno);
	if (err != KErrNone)
		{
		ClosePipe(infds, err);
		ClosePipe(outfds, err);
		return -1;
		}

	RProcess childproc;
	if (aCmd)
		{
		err = childproc.Create(TPtrC((TText*)aFile), TPtrC((TText*)aCmd));
		}
	else
		{
		err = childproc.Create(TPtrC((TText*)aFile), KNullDesC);
		}

	if (err != KErrNone)
		{
		ClosePipe(infds, err);
		ClosePipe(outfds, err);
		ClosePipe(errfds, err);
		return -1;
		}
		
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	// Request for notification when child exits
	Backend()->RegisterExitNotification(childproc.Id());
#endif

	// Pass process id of parent process to the child process
	childproc.SetParameter(ESlotParentId, RProcess().Id());
   	// Pass in the child type
   	childproc.SetParameter(ESlotChildType, EPopen3Child);

	// Pass in the handle that will be Duped to the child's STDIN
	TransferToChild(childproc, infds, KPipeReadHandle, ESlotStdPipe1);

	// Pass in the handle that will be Duped to the child's STDOUT
	TransferToChild(childproc, outfds, KPipeWriteHandle, ESlotStdPipe2);

	// Pass in the handle that will be Duped to the child's STDERR
	TransferToChild(childproc, errfds, KPipeWriteHandle, ESlotStdPipe3);

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	TBuf<16> sigmask; //Will be alive till copied by child thanks to fd_transfer server
	TransferSigMaskToChild(childproc,sigmask);
#endif

	if (aEnvp && aEnvp[0])
		{
		PassEnvToChild(childproc, aEnvp);
		}

	// begin critical section
	iFidLock.Wait();

    // Resume the child process
	RSemaphore sem;
	err = sem.CreateGlobal(KNullDesC, 0);
	if (err != KErrNone)
	{
	// Unable to create semaphore. Abandon it all!
	   		anErrno = ECHILD;
			childproc.Terminate(0);
	   		childproc.Close();

	   		// close pipe handles
	   		ClosePipe(infds, err);
	   		ClosePipe(outfds, err);
	   		ClosePipe(errfds, err);
	   		// give up lock
	   		iFidLock.Signal();
	   		return -1;
	}
	// Pass in the semaphore
	childproc.SetParameter(ESlotSemaphore, sem);
	childproc.Resume();
	TBuf<20> name(KFDTransferSvr);
	name.AppendNum(childproc.Id(), EHex);
	DoTransferAll(sem,name);

	// Add to iChildren array and close handle
	TChild tchild(childproc);
	AddChild(tchild);

	// end critical section
	iFidLock.Signal();

	aFids[0] = infds[KPipeWriteHandle];
	aFids[1] = outfds[KPipeReadHandle];
	aFids[2] = errfds[KPipeReadHandle];
    sem.Close();
	childproc.Close();
	// The TProcessId will be converted to a TUint
	return tchild.iPid;
	}

// -----------------------------------------------------------------------------
// CFileTable::PassEnvToChild
// -----------------------------------------------------------------------------
void CFileTable::PassEnvToChild(RProcess& child, wchar_t** aEnvp)
	{
	TUint16 count = 0;
	TInt len = 0;

	// first determine the size of descriptor required to hold envp strings
	for (TInt i = 0; aEnvp[i] != NULL; ++i)
		{
		wchar_t *p = aEnvp[i];
		while (*p)
			++p;

		len += (p - aEnvp[i]) + 1;
		++count;
		}

	// account for the terminal 0 as well
	++count;
	++len;

	HBufC* pBuf = HBufC::New(len + sizeof(count));
	if (pBuf)
		{
		TPtr ptr(pBuf->Des());
		// first insert count of number of env strings
		ptr.AppendNumFixedWidthUC(count, EDecimal, sizeof(count));
		for (TInt i = 0; aEnvp[i] != NULL; ++i)
			{
			ptr.Append(TPtrC((TText *)aEnvp[i]));
			ptr.Append(TChar(L'\0'));
			}
		// the terminal 0
		ptr.Append(TChar(L'\0'));

		// pass in the envp
		child.SetParameter(ESlotEnv, *pBuf);

		delete pBuf;
		}
	}

// -----------------------------------------------------------------------------
// CFileTable::PassFileActionsToChild
// -----------------------------------------------------------------------------
void CFileTable::PassFileActionsToChild(RProcess& child,
										const posix_spawn_file_actions_t* aFileActions)
	{
	CFileDescBase* fdesc;
	TInt totlen = 0;

	TFileAction* fa = (aFileActions->_fa->iIter)++;
	while (fa)
		{
		switch (fa->iOp)
			{
		case EOpen:
			totlen += fa->iPath->Length() + 16;
			break;
		case EClose:
			if (At(fa->iFid1, fdesc) == KErrNone)
				{
				fdesc->SetAttributes(KSpawnCloseInChild, ETrue);
				}
			break;
		case EDup:
			totlen += 16;
		default:
			break;
			}

		fa = (aFileActions->_fa->iIter)++;
		}

	// reset iterator
	(aFileActions->_fa->iIter).SetToFirst();

	if (totlen)
		{
		HBufC* pBuf = HBufC::New(totlen);
		if (pBuf)
			{
			TPtr ptr(pBuf->Des());
			fa = (aFileActions->_fa->iIter)++;
			while (fa)
				{
				switch (fa->iOp)
					{
				case EOpen:
					ptr.AppendNum(fa->iOp);
					ptr.Append(' ');
					ptr.AppendNum(fa->iFid1);
					ptr.Append(' ');
					ptr.AppendNum(fa->iOFlag);
					ptr.Append(' ');
					ptr.AppendNum(fa->iMode);
					ptr.Append(' ');
					ptr.Append(*(fa->iPath));
					ptr.Append(' ');
					break;
				case EDup:
					ptr.AppendNum(fa->iOp);
					ptr.Append(' ');
					ptr.AppendNum(fa->iFid1);
					ptr.Append(' ');
					ptr.AppendNum(fa->iFid2);
					ptr.Append(' ');
					break;
				default:
					break;
					}

				fa = (aFileActions->_fa->iIter)++;
				}

			// reset iterator
			(aFileActions->_fa->iIter).SetToFirst();

			// pass in the actions
			child.SetParameter(ESlotActions, *pBuf);
			delete pBuf;
			}
		}
	}

// -----------------------------------------------------------------------------
// CFileTable::system
// -----------------------------------------------------------------------------
int CFileTable::system(const wchar_t* aCmd, const wchar_t* aCmdArg, int& anErrno)
	{
	TInt err = KErrNone;

	// Create a process with the command
	RProcess childproc;
	if (aCmdArg)
		{
		err = childproc.Create(TPtrC((TText*)aCmd), TPtrC((TText*)aCmdArg));
		}
	else
		{
		err = childproc.Create(TPtrC((TText*)aCmd), KNullDesC);
		}

	if (KErrNone == err)
		{
		// Pass process id of parent process to the child process
		childproc.SetParameter(ESlotParentId, RProcess().Id());
   		// Pass in the child type
   		childproc.SetParameter(ESlotChildType, ESystemChild);


		// begin critical section
		iFidLock.Wait();

		RSemaphore sem;
		TInt err = sem.CreateGlobal(KNullDesC, 0);
		if (err != KErrNone)
		{
		// Unable to create semaphore. Abandon it all!
		   		anErrno = ECHILD;
				childproc.Terminate(0);
		   		childproc.Close();

		   		// give up lock
		   		iFidLock.Signal();
		   		return -1;
		}
		// Pass in the semaphore
		childproc.SetParameter(ESlotSemaphore, sem);  //Pass the created one
#ifdef SYMBIAN_OE_POSIX_SIGNALS		
		TBuf<16> sigmask; //Will be alive till copied by child thanks to fd_transfer server
		TransferSigMaskToChild(childproc,sigmask);
#endif
		// Resume the child process
		childproc.Resume();

		TBuf<20> name(KFDTransferSvr);
		name.AppendNum(childproc.Id(), EHex);
		DoTransferAll(sem,name);

		// end critical section
		iFidLock.Signal();

		// Logon onto the process
		TRequestStatus status;
		childproc.Logon(status);
		// Wait for child exit
		User::WaitForRequest(status);
		sem.Close();
		childproc.Close();

		return status.Int();
		}
	return MapError(err, anErrno);
	}

// -----------------------------------------------------------------------------
// CFileTable::posix_spawn
// Posix Spawn Attributes (aAttr) is ignored, for now.
// -----------------------------------------------------------------------------
int CFileTable::posix_spawn(int* aPid, const wchar_t* aFile,
					 		const posix_spawn_file_actions_t* aFileActions,
					 		const posix_spawnattr_t* /*aAttr*/,
					 		const wchar_t* aArgs,
					 		wchar_t** aEnvp)
	{
	TInt err = KErrNone;
	RProcess childproc;

	if (aArgs)
		{
		err = childproc.Create(TPtrC((TText*)aFile), TPtrC((TText*)aArgs));
		}
	else
		{
		err = childproc.Create(TPtrC((TText*)aFile), KNullDesC);
		}

	if (err != KErrNone)
		{
		return ECHILD;
		}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	// Request for notification when child exits
	Backend()->RegisterExitNotification(childproc.Id());
#endif

	TChild tchild(childproc);


	// Pass process id of parent process to the child process
	childproc.SetParameter(ESlotParentId, RProcess().Id());
   	// Pass in the child type
   	childproc.SetParameter(ESlotChildType, ESpawnChild);

	if (aEnvp && aEnvp[0])
		{
		PassEnvToChild(childproc, aEnvp);
		}

	if (aFileActions && !(aFileActions->_fa->iActions.IsEmpty()))
		{
		PassFileActionsToChild(childproc, aFileActions);
		}

	// begin critical section
	iFidLock.Wait();

	RSemaphore sem;
	err = sem.CreateGlobal(KNullDesC, 0);
	if (err != KErrNone)
	{
		// Unable to create semaphore. Abandon it all!
	 		childproc.Terminate(0);
	   		childproc.Close();

	   		// give up lock
	   		iFidLock.Signal();
	   		return ECHILD;

	}
	// Pass in the semaphore
	childproc.SetParameter(ESlotSemaphore, sem);  //Pass the created one
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	TBuf<16> sigmask; //Will be alive till copied by child thanks to fd_transfer server
	TransferSigMaskToChild(childproc,sigmask);
#endif
	// Resume the child process

	childproc.Resume();

	TBuf<20> name(KFDTransferSvr);
	name.AppendNum(childproc.Id(), EHex);
	DoTransferAll(sem,name);

	// Add to iChildren array and close handle
	AddChild(tchild);

	// end critical section
	iFidLock.Signal();
	sem.Close();
	childproc.Close();

	if (aPid)
		{
		*aPid = tchild.iPid;
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CFileTable::waitpid
// -----------------------------------------------------------------------------
int CFileTable::waitpid(int aPid, int* aStatus, int aOptions, int& anErrno)
	{
	TInt err = KErrNone;
	RProcess child;

	if (aPid == -1)
		{
		// wait on any child
		// array of child processes that are still alive
		const RArray<TChild>& children = GetChildren();
		TInt cnt = children.Count();
		if (cnt == 0)
			{
			anErrno = ECHILD;
			return -1;
			}

		RArray<TUint> theliving(4);
		TInt i;
		for (i = 0; i < cnt; ++i)
			{
			TInt pid = children[i].iPid;
			err = child.Open(pid);
			if (err != KErrNone)
				{
				// unable to open child...possibly dead and gone
				RemoveChild(i);
				--cnt;
				continue;
				}

			if (child.ExitType() == EExitPending)
				{
				theliving.Append(i);
				child.Close();
				// continue to next child to check if there is a dead child in list
				}
			else
				{
				if (aStatus)
					{
					*aStatus = child.ExitReason();
					*aStatus = *aStatus << 8;
					//coverity[unchecked_value]
					*aStatus |= child.ExitType();
					}
				child.Close();
				RemoveChild(i);
				theliving.Close();

				return pid;
				}
			}

		// we are here - implies no child was dead
		cnt = theliving.Count();
		TRequestStatus* reqarray = new TRequestStatus[cnt];
		TInt reqcnt = 0;
		for (i = 0; i < cnt; ++i)
			{
			err = child.Open(children[theliving[i]].iPid);
			if (err != KErrNone)
				{
				RemoveChild(theliving[i]);
				theliving.Remove(i);
				--cnt;
				continue;
				}
			child.Logon(reqarray[reqcnt++]);
			child.Close();
			}

		// wait for any of the children to die
		CLocalSystemInterface::WaitForNRequest(reqarray, reqcnt);

		for (i = 0; i < reqcnt; ++i)
			{
			if (reqarray[i].Int() != KRequestPending)
				{
				TInt pid = -1;
				if (reqarray[i].Int() < KErrNone)
					{
					// error
					anErrno = ECHILD;
					}
				else
					{
					// req.Int(), if valid, returns the ExitReason of the child
					if (aStatus)
						{
						*aStatus = reqarray[i].Int();
						}
					pid = children[theliving[i]].iPid;
					}

				// time to return. Cleanup outstanding requests
				// reuse i - we don't care anymore
				for (i = 0; i < reqcnt; ++i)
					{
					if (reqarray[i].Int() == KRequestPending)
						{
						// outstanding logon request
						err = child.Open(children[theliving[i]].iPid);
						if (err == KErrNone)
							{
							child.LogonCancel(reqarray[i]);
							child.Close();
							}
						}
					}
				theliving.Close();
				delete[] reqarray;
				return pid;
				}
			}
		//coverity[leaked_storage]
		}
	else
		{
		// wait for a specific child
		// first ensure that the child is one of mine
		TInt idx = FindChild(TProcessId(aPid));
		if (idx == KErrNotFound)
			{
			// not my child.
			anErrno = ECHILD;
			return -1;
			}

		err = child.Open(TProcessId(aPid));
		if (err != KErrNone)
			{
			// unable to open child...possibly dead and gone
			RemoveChild(idx);
			anErrno = ECHILD;
			return -1;
			}

		if (child.ExitType() != EExitPending)
			{
			// Already dead
			if (aStatus)
				{
				*aStatus = child.ExitReason();
				*aStatus = *aStatus << 8;
				//coverity[unchecked_value]
				*aStatus |= child.ExitType();

				}
			}
		else
			{
			// still alive
			if (aOptions & WNOHANG)
				{
				child.Close();
				return 0;
				}

			TRequestStatus req;
			child.Logon(req);
			User::WaitForRequest(req);
			if (req.Int() < KErrNone)
				{
				child.Close();
				RemoveChild(idx);
				anErrno = ECHILD;
				return -1;
				}
			// req.Int(), if valid, returns the ExitReason of the child
			if (aStatus)
				{
				*aStatus = req.Int();
				*aStatus = *aStatus << 8;
				//coverity[unchecked_value]
				*aStatus |= child.ExitType();

				}
			}
		child.Close();
		RemoveChild(idx);
		return aPid;
		}
	return -1;
	}

TInt CFileTable::InheritFiles(CFileDesTransferServer* server)
{
StartFDTransferServer(server);
CloseFDTransferServer(server);
return 0;
}
// -----------------------------------------------------------------------------
// CFileTable::CheckOrigins
// -----------------------------------------------------------------------------
#ifdef SYMBIAN_OE_POSIX_SIGNALS
void CFileTable::CheckOrigins(wchar_t**& aEnvp, int& aCount,TUint64 & aSigMask)
#else
void CFileTable::CheckOrigins(wchar_t**& aEnvp, int& aCount)
#endif
	{
	TInt err = KErrNone;

	TInt parentId;

	err = User::GetTIntParameter(ESlotParentId, parentId);
	if (err != KErrNone)
		{
		/*
		Unable to read either the parent Id from environment slot or unable to
		open a handle to the parent. This can either indicate a problem or that
		this process was not born of a popen/popen3/system/posix_spawn invocation.
		Return.
		*/
		return;
		}

	// Find out how I was conceived
	TInt how;
	err = User::GetTIntParameter(ESlotChildType, how);
	if (err != KErrNone || how < 0 || how > 3)
		{
		// Some error. I will never know how I was conceived.
		return;
		}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	TBuf<16> sigmask;
	err = User::GetDesParameter(ESlotSignalMask,sigmask);
	if(err == KErrNone)
		{
		TLex lexmask(sigmask);
		err=lexmask.Val((TInt64 &)aSigMask,EHex);
			if (err!=KErrNone)
			{
			 aSigMask = 0x0000000000000000;
			}
		}
	else
		{
		aSigMask = 0x0000000000000000;
		}
#endif

	// Second, inherit any regular files the parent may have open
	RSemaphore sem;
	err = sem.Open(ESlotSemaphore);

	if (err != KErrNone)
		{
		// Unable to open a semaphore that should have been there. Sigh.
		return;
		}

	CFileDesTransferServer* pServer = NULL;
	TRAP(err, pServer = NewFDTransferServerL(sem));  //how to handle this
	InheritFiles(pServer);   //check errors here
	delete pServer;

	// Finally, do launch-method-specific stuff
	switch (how)
		{
		case EPopenChild:
			{
			RPipe tpipe;
			err = tpipe.Open(ESlotStdPipe1);
			if (err == KErrNone)
				{
				int fd = Reserve();
				err = CreatePipeDesc(fd, tpipe);
				if (err == KErrNone)
					{
					if (tpipe.HandleType() == RPipe::EReadChannel)
						{
						// close and remap stdin
						Dup2(fd, 0, ETrue);
						}
					else
						{
						// close and remap stdout
						Dup2(fd, 1, ETrue);
						}
					}
				else
					{
					// Unable to create a pipe fd. Cannot remap stdio.
					tpipe.Close();
					}
				}
			break;
			}
		case EPopen3Child:
			/*
			If there is an error opening any of the pipes or creating pipe file
			descriptors, popen3 will not map any of STDIN, STDOUT or STDERR.
			The child will execute without any channel to or from its creator.

			TODO: Should we communicate the error to the parent in some manner??
			*/
			{
			RPipe inpipe, outpipe, errpipe;

			err = inpipe.Open(ESlotStdPipe1);
			if (err != KErrNone)
				{
				break;
				}

			err = outpipe.Open(ESlotStdPipe2);
			if (err != KErrNone)
				{
				inpipe.Close();
				break;
				}

			err = errpipe.Open(ESlotStdPipe3);
			if (err != KErrNone)
				{
				inpipe.Close();
				outpipe.Close();
				break;
				}

			int infd, outfd, errfd;

			infd = Reserve();
			err = CreatePipeDesc(infd, inpipe);
			if (err != KErrNone)
				{
				inpipe.Close();
				outpipe.Close();
				errpipe.Close();
				break;
				}

			outfd = Reserve();
			err = CreatePipeDesc(outfd, outpipe);
			if (err != KErrNone)
				{
				UserClose(infd, err);
				outpipe.Close();
				errpipe.Close();
				break;
				}

			errfd = Reserve();
			err = CreatePipeDesc(errfd, errpipe);
			if (err != KErrNone)
				{
				UserClose(infd, err);
				UserClose(outfd, err);
				errpipe.Close();
				break;
				}

			// Now close and remap stdin, stdout and stderr
			Dup2(infd, 0, ETrue);
			Dup2(outfd, 1, ETrue);
			Dup2(errfd, 2, ETrue);

			// Inherit the environment passed in by the parent
			TInt aLen = User::ParameterLength(ESlotEnv);
			if (aLen > 0)
				{
				HBufC* envbuf = HBufC::New(aLen/2);
				if (envbuf)
					{
					TPtr ptr = envbuf->Des();
					err = User::GetDesParameter(ESlotEnv, ptr);
					if (err == KErrNone)
						{
						TUint16 count;
						TLex lexr(*envbuf);
						err = lexr.Val(count, EDecimal);
						if (err == KErrNone)
							{
							aEnvp = (wchar_t**)User::Alloc(count * sizeof(wchar_t*));
							if (aEnvp)
								{
								const wchar_t* wp = (const wchar_t*) \
												((envbuf->Des()).Ptr() + sizeof(count));
								count = 0;
								while (*wp != L'\0')
									{
									TPtrC ptr((TText*)wp);
									TInt len = ptr.Length() + 1;
									aEnvp[count] = (wchar_t*)User::Alloc(len * sizeof(wchar_t));
									if (!aEnvp)
										{
										// Unable to alloc memory. Stop this madness.
										break;
										}
									memcpy(aEnvp[count], ptr.Ptr(), len * sizeof(wchar_t));
									wp += len;
									++count;
									}
								aEnvp[count] = 0;
								aCount = count;
								// aEnvp will be deleted by the caller
								}
							}
						}
					delete envbuf;
					}
				}
			break;
			}
		case ESpawnChild:
			{
			TInt len = User::ParameterLength(ESlotActions);
			if (len > 0)
				{
				HBufC* pBuf = HBufC::New(len);
				if (pBuf)
					{
					TPtr ptr(pBuf->Des());
					err = User::GetDesParameter(ESlotActions, ptr);
					if (err == KErrNone)
						{
						TLex lexr(ptr);
						TInt op;
						err = lexr.Val(op);

						while (err == KErrNone)
							{
							switch(op)
								{
							case EOpen:
								TInt fd;
								lexr.Inc();
								err = lexr.Val(fd);
								if (err == KErrNone)
									{
									TInt oflag;
									lexr.Inc();
									err = lexr.Val(oflag);
									if (err == KErrNone)
										{
										TInt mode;
										lexr.Inc();
										err = lexr.Val(mode);
										lexr.Inc();
										TFileName fname(lexr.NextToken());
										TInt fid = open((wchar_t*)fname.PtrZ(), oflag, mode, err);
										if (fid > 0)
											{
											dup2(fid, fd, err);
											}
										}
									}
								break;
							case EDup:
								TInt fd1;
								lexr.Inc();
								err = lexr.Val(fd1);
								if (err == KErrNone)
									{
									TInt fd2;
									lexr.Inc();
									err = lexr.Val(fd2);
									if (err == KErrNone)
										{
										dup2(fd1, fd2, err);
										}
									}
								break;
								}
							lexr.Inc();
							err = lexr.Val(op);
							}
						}
					delete pBuf;
					}
				}
			break;
			}
		default:
			break;
		}
	}
void CFileTable::DoTransferAll(RSemaphore &sem,TDesC& name)
{
   //coverity[unchecked_value]
   sem.Wait(100000);  //Is this the optimum time to wait

   RFileDesTransferSession session;
   TInt err = session.Connect(name);
   if (err == KErrNone)
   		{
   		    session.TransferFilesToServer();  //handle error here , should i
   		    session.TransferPipesToServer();  //handle error here , should i

   		}
   session.Close();
}

// -----------------------------------------------------------------------------
// CFileTable::NewFDTransferServerL
// -----------------------------------------------------------------------------
CFileDesTransferServer* CFileTable::NewFDTransferServerL(RSemaphore &sem)
	{
	CActiveScheduler* pScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(pScheduler);

	CFileDesTransferServer* pServer = CFileDesTransferServer::NewLC(sem);

	// Replace the current active scheduler with this one
	pServer->iOldScheduler = CActiveScheduler::Replace(pScheduler);

    TBuf<20> name(KFDTransferSvr);
    name.AppendNum(RProcess().Id(), EHex);
    TInt err = pServer->AddToScheduler(name);
    if (err != KErrNone)
    	{
    	// Something went wrong. Reinstall the old active scheduler, if any, and leave
		// Remove active objects from scheduler before reverting to the old one
    	pServer->RemoveFromScheduler();
    	if (pServer->iOldScheduler)
    		{
    		CActiveScheduler::Replace(pServer->iOldScheduler);
    		}
    	User::Leave(err);
    	}

	// pop both pScheduler and pServer from the cleanup stack
	CleanupStack::Pop(2);
	return pServer;
	}

// -----------------------------------------------------------------------------
// CFileTable::StartFDTransferServer
// -----------------------------------------------------------------------------
inline void CFileTable::StartFDTransferServer(CFileDesTransferServer* pServer)
	{
	pServer->StartServer();
	}

// -----------------------------------------------------------------------------
// CFileTable::CloseFDTransferServer
// -----------------------------------------------------------------------------
void CFileTable::CloseFDTransferServer(CFileDesTransferServer* pServer)
	{
	CActiveScheduler* pScheduler = CActiveScheduler::Current();
	// Remove active objects from scheduler before reverting to the old one
	pServer->RemoveFromScheduler();
	if (pServer->iOldScheduler)
		{
		CActiveScheduler::Replace(pServer->iOldScheduler);
		}
	else
		{
		// No previous AS, uninstall this one
		CActiveScheduler::Install(NULL);
		}
	// cleanup
	delete pScheduler;
	}

// -----------------------------------------------------------------------------
// CFileTable::pipe
// -----------------------------------------------------------------------------
int CFileTable::pipe(int fildes[2], int& anErrno)
    {
   	int readFd = Reserve();
	if (readFd < 0)
		{
		return MapError(readFd, anErrno);
		}

	int writeFd = Reserve();
	if (writeFd < 0)
		{
		Attach(readFd, 0);	// cancel the reservation
		return MapError(writeFd, anErrno);
		}

	// initialize the RPipe subsystem
	TInt err = RPipe::Init();
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		Attach(readFd, 0);
		Attach(writeFd, 0);
		return MapError(err, anErrno);
		}

	// Allocate the memory for the read fd
	CPipeReadDesc* pRdDesc = (CPipeReadDesc*) iPrivateHeap->AllocZ(sizeof(CPipeReadDesc));

	if (pRdDesc)
		{
		// Do a placement new to construct the CPipeReadDesc object
		pRdDesc = new (pRdDesc) CPipeReadDesc;

		// Allocate the memory for the write fd
		CPipeWriteDesc* pWrDesc = (CPipeWriteDesc*) iPrivateHeap->AllocZ(sizeof(CPipeWriteDesc));

		if (pWrDesc)
			{
			// Do a placement new to construct the CPipeWriteDesc object
			pWrDesc = new (pWrDesc) CPipeWriteDesc;

			// Now create the pipe
			err = RPipe::Create(KDefaultPipeSz, pRdDesc->Handle(), pWrDesc->Handle());

			if (err == KErrNone)
				{
				Attach(readFd, pRdDesc);
				Attach(writeFd, pWrDesc);

				fildes[0] = readFd;
				fildes[1] = writeFd;
				return 0;
				}
			delete pWrDesc;
			}
		else
			{
			err = KErrNoMemory;
			}
		delete pRdDesc;
		}
	else
		{
		err = KErrNoMemory;
		}

	Attach(readFd, 0);
	Attach(writeFd, 0);
	return MapError(err,anErrno);
    }

// -----------------------------------------------------------------------------
// CFileTable::CreatePipeDesc
// -----------------------------------------------------------------------------
int CFileTable::CreatePipeDesc(int aFd, RPipe& aHandle, TUint16 aMode)
	{
	TInt err = KErrNone;

	if (aHandle.HandleType() == RPipe::EReadChannel)
		{
		// Allocate the memory for the read fd
		CPipeReadDesc* pRdDesc = (CPipeReadDesc*) iPrivateHeap->AllocZ(sizeof(CPipeReadDesc));
		if (pRdDesc)
			{
			// Do a placement new to construct the CPipeReadDesc object
			pRdDesc = new (pRdDesc) CPipeReadDesc(aMode);
			pRdDesc->Handle() = aHandle;
			Attach(aFd, pRdDesc);
			}
		else
			{
			err = KErrNoMemory;
			}
		}
	else
		{
		// Allocate the memory for the write fd
		CPipeWriteDesc* pWrDesc = (CPipeWriteDesc*) iPrivateHeap->AllocZ(sizeof(CPipeWriteDesc));
		if (pWrDesc)
			{
			// Do a placement new to construct the CPipeWriteDesc object
			pWrDesc = new (pWrDesc) CPipeWriteDesc(aMode);
			pWrDesc->Handle() = aHandle;
			Attach(aFd, pWrDesc);
			}
		else
			{
			err = KErrNoMemory;
			}
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CFileTable::ClosePipe
// Convinience function for closure of open pipe file descriptors
// -----------------------------------------------------------------------------
//
inline void CFileTable::ClosePipe(int fds[2], int& err)
	{
	UserClose(fds[0], err);
	UserClose(fds[1], err);
	}

// -----------------------------------------------------------------------------
// CFileTable::ExpandFTableL
// This function expands the ftbale to acount and fills the newlly created
// enties with 0. acount should be always greater than the current size of the
// of the table
//-----------------------------------------------------------------------------

void CFileTable::ExpandFTableL(TInt count)
{
	TInt oldCount = iFids.Count();
	RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
	iFids.ResizeL(count);
	User::SwitchHeap(oldHeap);
	for(int i = oldCount; i<iFids.Count(); i++)
		iFids[i] = 0;

}

// CFileTable::mkfifo
// -----------------------------------------------------------------------------
int CFileTable::mkfifo(const wchar_t *pathName, mode_t /*mode*/, int& anErrno, RFs& aFs)
    {
    struct SSpecialFileMagicHeader enBuf;
    int err = _EncodeSystemSplFileMagicHeader(&enBuf, EFileTypeMkFifo);
    if (err != 0)
        {
        return MapError(err, anErrno);
        }
    return _CreateSysSplFile(pathName, (char*)&enBuf, sizeof(struct SSpecialFileMagicHeader), anErrno, aFs);
    }

// -----------------------------------------------------------------------------
// CFileTable::FifoOpen
// -----------------------------------------------------------------------------
CFileDescBase* CFileTable::FifoOpen(const wchar_t* pathName, mode_t mode, int /*perms*/, int& anErrno)
    {
	// initialize the RPipe subsystem
    TInt err = RPipe::Init();
    if (err != KErrNone && err != KErrAlreadyExists)
    	{
    	return NULL;
    	}

    TParsePtrC fName(TPtrC((TText*)pathName));
	err = RPipe::Define(fName.NameAndExt(), KDefaultPipeSz);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		return NULL;
		}

	if ((mode & O_WRONLY) == O_WRONLY)
		{
		CPipeWriteDesc* pWrDesc = \
					(CPipeWriteDesc*) Backend()->Alloc(sizeof(CPipeWriteDesc));
		if (pWrDesc)
			{
			pWrDesc = new (pWrDesc) CPipeWriteDesc(mode);
			err = (pWrDesc->Handle()).Open(fName.NameAndExt(), RPipe::EOpenToWriteNamedPipeButFailOnNoReaders);
			if (err == KErrNotReady)
				{
				if (pWrDesc->FcntlFlag() & O_NONBLOCK)
					{
					anErrno = ENXIO;
					return NULL;
					}
				else
					{
					TRequestStatus status;
					(pWrDesc->Handle()).Wait(fName.NameAndExt(), status);
					User::WaitForRequest(status);
					err = status.Int();
					}
				}

			if (err == KErrNone)
				{
				pWrDesc->SetAttributes(KFifoFd, ETrue);
				return pWrDesc;
				}

			MapError(err, anErrno);
			delete pWrDesc;
			}
		else
			{
			anErrno = ENOMEM;
			}
		}
	else if ((mode & O_RDONLY) == O_RDONLY)
		{
		CPipeReadDesc* pRdDesc = \
					(CPipeReadDesc*) Backend()->Alloc(sizeof(CPipeReadDesc));
		if (pRdDesc)
			{
			pRdDesc = new (pRdDesc) CPipeReadDesc(mode);
			err = (pRdDesc->Handle()).Open(fName.NameAndExt(), RPipe::EOpenToRead);
			if (err == KErrNone)
				{
				if (!(pRdDesc->FcntlFlag() & O_NONBLOCK))
					{
					TRequestStatus status;
					(pRdDesc->Handle()).WaitForWriter(fName.NameAndExt(), status);
					User::WaitForRequest(status);
					err = status.Int();
					}

				if (err == KErrNone)
					{
					pRdDesc->SetAttributes(KFifoFd, ETrue);
					return pRdDesc;
					}
				}

			MapError(err, anErrno);
			delete pRdDesc;
			}
		else
			{
			anErrno = ENOMEM;
			}
		}
	else
		{
		// any other mode
		anErrno = EINVAL;
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CFileTable::link
// -----------------------------------------------------------------------------
int CFileTable::link(const wchar_t *oldpathName, const wchar_t *newpathName, int& anErrno, RFs& aFs)
    {
    struct SLinkInfo enBuf;
    int err = KErrNone;

	TSpecialFileType fileType = _SystemSpecialFileBasedFilePath(oldpathName, err, aFs);
	if (IS_SUPPORTED(fileType))
		{
		if (err)
		    {
			if (err == KErrNotReady)
				{
				anErrno = ENOENT;
				return -1;
				}

			return MapError(err, anErrno);
			}
		// Embedded the link type tag header in to the encode buffer
		err = _EncodeSystemSplFileMagicHeader(&(enBuf.iMagicHeader), EFileTypeSymLink);
	    if (err != 0)
	        {
	        return MapError(err, anErrno);
	        }
	    WcsCpy(enBuf.iParentPath, oldpathName);
	    // Create a special file with the encoded buffer embedded with in it
	    return _CreateSysSplFile(newpathName, (char*)&enBuf, sizeof(struct SLinkInfo), anErrno, aFs);
    	}
	else
		{
		if (fileType == EFileNotFound || err == KErrPermissionDenied)
			{
			return MapError(err, anErrno);
			}
		else
			{
			/* We won't support a request where the oldpathName itself is a special file type */
			if (!err && (EFileTypeSymLink == fileType))
				{
				anErrno = ELOOP;
				return -1;
				}

			if (err == KErrNotReady)
				{
				anErrno = ENOENT;
				return -1;
				}
			}
		}
	anErrno = EINVAL;
	return -1;
    }

// -----------------------------------------------------------------------------
// CFileTable::LinkOpen
// -----------------------------------------------------------------------------
CFileDescBase* CFileTable::LinkOpen(const wchar_t* pathName, mode_t mode, int perms, int& anErrno, RFs& aFs)
    {
	struct SLinkInfo enBuf;
	CFileDesc *f = NULL;
	int err = KErrNone;

	err = _ReadSysSplFile(pathName, (char*)&enBuf, sizeof(struct SLinkInfo), anErrno, aFs);
	if (err == KErrNone)
		{
		TFullName fullName;
		err = GetFullFile(fullName,(const TText16*)enBuf.iParentPath,aFs);
		f = (CFileDesc*) Backend()->Alloc(sizeof(CFileDesc));
		if (f)
			{
			f = new (f) CFileDesc;
			err = f->Open(aFs, fullName, mode, perms);
			if (err != KErrNone)
				{
				if (err == KErrPathNotFound || err == KErrNotFound)
					{
					// the parent file does not exist. remove the link.
					err = _DeleteSystemSpecialFileBasedFilePath(pathName, aFs);
					if (err != KErrNone)
						{
						MapError(err, anErrno);
						}
					}
				delete f;
				f = NULL;
				}
			}
		}
	return f;
    }

// -----------------------------------------------------------------------------
// CFileTable::unlink
// -----------------------------------------------------------------------------
int CFileTable::unlink(RFs& aFs, const wchar_t *pathName, int& anErrno)
	{
	TInt ret = 0;
	TInt err = KErrNone;
	TFileName fullName;
#ifdef SYMBIAN_OE_LIBRT	
	struct shmid_ds *buf = NULL;
#endif //SYMBIAN_OE_LIBRT
	TSpecialFileType fileType = _SystemSpecialFileBasedFilePath(pathName, err, aFs);

	if (err == KErrPermissionDenied)
		{
		return MapError(err, anErrno);
		}

	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}

	if (fileType == EFileGeneralError)
		{
		anErrno = EINVAL;
		return -1;
		}

	if (err || fileType == EFileNotFound)
		{
#ifdef SYMBIAN_OE_LIBRT
		TFileName filename;
		filename.Copy((const TText*)pathName);
		TInt shmkey = GeneratePathKey(filename);
		TInt shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, 0666, err);
		if (shmid > 0)
			{
			ret = Backend()->iIpcS.shmctl(shmid, IPC_RMID, buf, err);
			if (ret == 0)
				{
				return MapError(KErrNone, anErrno);
				}
			}
#endif //SYMBIAN_OE_LIBRT		
		return MapError(err, anErrno);
		}

	GetFullFile(fullName,(const TText16*)pathName,aFs);
	TParsePtrC fName(fullName);

	switch (fileType)
		{
	case EFileTypeSocket:
		{
		TBuf8<KMaxFileName> socketPath;
		err = CnvUtfConverter::ConvertFromUnicodeToUtf8(socketPath,fullName);
		if(err != KErrNone)
			{
			anErrno = EINVAL;
			return -1;
			}
		err = Backend()->iIpcS.RemLocalSockAddr(socketPath);
		if(err == KErrNone || err == KErrNotFound)
			{
			err = PosixFilesystem::reg_unlink(aFs, pathName, anErrno);
			}
		return MapError(err,anErrno);
		}
	case EFileTypeMkFifo:
		err = RPipe::Destroy(fName.NameAndExt());
		if (err != KErrNone && err != KErrNotFound)
			{
			ret = MapError(err, anErrno);
			}
		/* fall thro */
	case EFileTypeSymLink:
		err = _DeleteSystemSpecialFileBasedFilePath(pathName, aFs);
		if (err != KErrNone)
			{
			if (err == KErrNotReady)
				{
				anErrno = ENOENT;
				return -1;
				}

			ret = MapError(err, anErrno);
			}
		break;
	default:
		// not a special file
		ret = PosixFilesystem::reg_unlink(aFs, pathName, anErrno);
		}

	return ret;
	}

TInt CFileTable::RConnectionCount()
	{
	return iRConnectionList.Count();
	}

TInt CFileTable::RConnectionAt(TInt aRcIndex, RConnection *&aRc)
	{
	iRcLock.Wait();
	TInt count = iRConnectionList.Count();
	if (aRcIndex < 0 || aRcIndex >= count)
		{
		iRcLock.Signal();
		return KErrArgument;
		}

	aRc = iRConnectionList[aRcIndex];
	iRcLock.Signal();
	if (aRc == 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

TInt CFileTable::AddRConnectionPtr(RConnection *aRc, TInt &aRcIndex)
	{
	TInt pos;
	TInt err = KErrNone;
	TInt rconnCount = 0;

	aRcIndex = -1;
	iRcLock.Wait();
	rconnCount = iRConnectionList.Count();
	for (pos = 0; pos < rconnCount; pos++)
		{
		if (iRConnectionList[pos] == NULL)
			{
			break;
			}
		}

	// No more space in list
	if (pos == rconnCount )
		{
		RHeap* oheap = User::SwitchHeap(iPrivateHeap);
		TRAP(err,iRConnectionList.ExtendL());
		User::SwitchHeap(oheap);
		}

	if (err != KErrNone)
		{
		iRcLock.Signal();
		return err;
		}
	iRConnectionList[pos] = aRc;
	iRcLock.Signal();
	aRcIndex = pos;
	return err;
	}

void CFileTable::RemoveRConnectionPtrAt(TInt aRcIndex)
	{
	iRcLock.Wait();
	if (aRcIndex < 0 || aRcIndex >= iRConnectionList.Count())
		{
		iRcLock.Signal();
		return;
		}

	iRConnectionList[aRcIndex] = 0;
	iRcLock.Signal();
	return;
	}

void CFileTable::DeleteRConnectionList()
	{
	iRcLock.Wait();
	iRConnectionList.Reset();
	iRcLock.Signal();
	return;
	}


//-----------------------------------------------------------------------------
//int CFileTable::SetEcho(int aFd, TBool aEcho, int& aErrno)
//
//Set the echo flag of the suppported console abstractions matching
//the given fd.
//-----------------------------------------------------------------------------

int CFileTable::SetEcho(int aFd, TUint8 aEcho, int& aErrno)
	{
	CFileDescBase* fdesc =0;
	int err = KErrNone;
	aErrno = 0;

	if (aFd<0 || aFd>=iFids.Count())
		{
		return MapError(KErrBadHandle, aErrno);
		}

	if (At(aFd, fdesc) != KErrNone)
		{
		return MapError(KErrBadHandle, aErrno);
		}

	switch (fdesc->Type())
		{
		case CFileDescBase::ETtyDesc:
			{
			err = static_cast<CTtyDesc*> (fdesc)->SetEcho(aEcho);
			if(err)
				{
				return MapError(err, aErrno);
				}
			}
			break;

		case CFileDescBase::ERedirDesc:
			{
			err = static_cast<CRedirDesc*> (fdesc)->SetEcho(aEcho);
			if(err)
				{
				return MapError(err, aErrno);
				}
			}
			break;

		default:
			return MapError(KErrNotSupported, aErrno);
		}

	return 0;
	}

