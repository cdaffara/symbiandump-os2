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


#include <string.h>
#include <stdio.h>
#include <fcntl.h>		// for struct stat
#include <wchar.h>
#include <sys/errno.h>		// for ENOTSOCK
#include <sys/ioctl.h>
#include <c32comm.h>
#include <sys/stat.h>
#include <limits.h> //for LLONG_MAX and INT_MAX
#ifdef SYMBIAN_OE_LIBRT
#include <sys/shm.h>
#endif// SYMBIAN_OE_LIBRT
#include "sysreent.h"
#include "systemspecialfilercg.h"
#include "fdesc.h"
#include "lposix.h"
#include "ltime.h"



/*
Temp directory, this should be checked later
*/
#ifdef WIDEP_tmpdir
#undef WIDEP_tmpdir
#define WIDEP_tmpdir   L"/System/temp/"
#endif //WIDEP_tmpdir



CFileDescBase::CFileDescBase( TUint aFcntl, TUint32 aFdAttrib ) :
	iReadTimeout(-1), iFcntlFlag(aFcntl), iFdAttrib(aFdAttrib), iPollErr(KErrNone)
	{
	}
TInt CFileDescBase::SetAtt(TUint /*aSetAttMask*/, TUint /*aClearAttMask*/) 
	{
	return 0;
	}

// A CFileDescBase factory function, for "named" file-like objects
CFileDescBase* CFileDescBase::Open(const wchar_t* name, int mode, int perms, TInt& err)
	{
    err = KErrNone;
    CFileDescBase* ret = NULL;

	if(WcsCmp(name, L"CON:") == 0)
		{
		ret = new CTtyDesc;	// NB. This won't be the default stdin/stdout/stderr console
		}
	else if(WcsCmp(name, L"NUL:") == 0)
		{
		ret = new CFileDescBase;
		}
	else if((WcsCmp(name, L"TMP:") == 0) || (mode & O_TMPFILE))
		{	
		CTempFileDesc* tmp = new CTempFileDesc;
		if(tmp != NULL)
			{
			err = tmp->Open(name, mode);
			if(err != KErrNone)
				{
				delete tmp;
				tmp = NULL;
				}
			}
		ret = tmp;
		}
#ifdef SYMBIAN_OE_LIBRT
	else if(mode & O_SHMFLG)
 		{
 		CSharedMemDesc* shm= new CSharedMemDesc;
 		if(shm != NULL)
 			{
 			err = shm->Open(name, mode, perms);
			if(err != KErrNone)
				{
				delete shm;
				shm = NULL;
				}
 			}						
 		ret = shm;
 		}
 #endif //SYMBIAN_OE_LIBRT
	else if	((L'C' == name[0]) && (L'O' == name[1]) && (L'M' == name[2]) && (L':' == name[4]) && ((name[3] >= L'1') && (name[3] <= L'9')) ||
			(L'I' == name[0]) && (L'R' == name[1]) && (L'C' == name[2]) && (L'O' == name[3]) && (L'M' == name[4]) && (L':' == name[6]) && ((name[5] >= L'1') && (name[5] <= L'9')))
		{
		CSerialDesc* com = new CSerialDesc;
		if (com != NULL)
			{
			err = com->Open(name, mode, perms);
			if(err != KErrNone)
				{
				delete com;
				com = NULL;
				}
			}
		ret = com;
		}
	else //It's a special file, directory or normal file
		{
		RFs& rfs = Backend()->FileSession();
        TUint attribval;
        TFullName fullName;
        err = GetFullFile(fullName, (const TText16*)name, rfs);
        if(err != KErrNone)
            return NULL;
        int ret1 = rfs.Att(fullName, attribval);
        if (ret1 == 0 && ((attribval & (KEntryAttHidden | KEntryAttSystem))== (KEntryAttHidden | KEntryAttSystem)))
            {
            TSpecialFileType fileType = _SystemSpecialFileBasedFilePath(name, err, rfs);
            if(fileType  == EFileTypeMkFifo) //special file, FIFO
                {
                ret = CFileTable::FifoOpen(name, mode, perms, err);
                }
            else if(fileType  == EFileTypeSymLink) //special file, symlink
                {
                ret = CFileTable::LinkOpen(name, mode, perms, err, rfs);
                }
            }
		else //normal file or directory
		    {
     	    //Try opening as a file
		    CFileDesc* file = new CFileDesc;
			if(file != NULL)
				{
				err = file->Open(rfs, fullName, mode, perms);
				if(err != KErrNone)
					{
					delete file;
					file = NULL;
					}
				}
			ret = file;
			
			//Check if it is directory, if the open as file failed
		    if(err != KErrNone && CheckIfDirectory(fullName, rfs)) 
				{
				CDirectoryDesc* dir = new CDirectoryDesc;
				if(dir != NULL)
					{
					err = dir->Open(rfs, fullName, mode, perms);
					if(err != KErrNone)
						{
						delete dir;
						dir = NULL;
						}
					}
				ret = dir;
				}
    		}
		}
	
	if(err == KErrNone && ret == NULL)
    	{	
    	err = KErrNoMemory;
    	}
	
	return ret;
	}

//Check if a path corresponds to a directory. This is a helper function
//for CFileDescBase::Open()
TBool CFileDescBase::CheckIfDirectory(const TDesC& aPath, RFs& aFs)
	{
	//Check whether its a Drive Name
	if(KMaxDriveName+1 == aPath.Length())
		{
		if(RFs::IsValidDrive(TDriveUnit(aPath)))
			{
			return ETrue;
			}
		}
	//Now check whether it is a directory
	else
		{
		TEntry entry;
		if(aFs.Entry(aPath, entry) == KErrNone)
			{
			if(entry.IsDir())
				{
				return ETrue;
				}
			}
		}

	return EFalse;
	}

// Useful default implementations for CFileDescBase virtual functions.
// Function to be re-implemented in a child class which can seek. 
TInt CFileDescBase::LSeek (off_t& offset, int)
	{
	// minimal implementation for devices which can't seek
	offset=-1;
	return ESPIPE;
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

void CFileDescBase::Sync(TRequestStatus& aStatus)
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
	int *param = reinterpret_cast<int*> (aParam);
	switch ((unsigned)aCmd)
		{
	case FIONREAD:
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
		else
			{
			err = aSs.ShareAuto();
			if (err)
				return 0;
			}
		}
	CSockDescBase *socketBase = NULL;
	if (family == AF_LOCAL || family == PF_LOCAL || family == AF_UNIX)
		{

		//coverity[alloc_fn]
		//coverity[assign]

		socketBase = new CFileSocketDesc;
		}
	else
		{
		socketBase = new CSocketDesc;
		}

	if (!socketBase)
		{
		err=KErrNoMemory;
		return NULL;
		}
	err = socketBase->Socket(aSs,family,style,protocol);

	if (err)
		{
		delete socketBase;
		if (err == KErrBadName) // Some mismatch in family-style-protocol
			{
			err = EPROTONOSUPPORT;
			}
		//coverity[memory_leak]
		return 0;
		}
	return socketBase;
	}

// minimal implementation of sockets, useful for all non-socket descriptors

void CFileDescBase::RecvFrom(TDes8& /*aDesc*/, TSockAddr& /*from*/, int /*flags*/, TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::RecvFromCancel() {}

TInt CFileDescBase::CompletionStatus (TInt& /*aLength*/, TInt aStatus)
	{
	return aStatus;
	}

void CFileDescBase::SendTo(TDes8& /*aDesc*/, const struct sockaddr* /* anAddr*/, unsigned long /*aAddrLen*/, int /*flags*/, TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::SendToCancel() {}

void CFileDescBase::Shutdown(TUint /*aHow*/,TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::ShutdownCancel() {}

TInt CFileDescBase::Bind(const struct sockaddr* /*addr*/, unsigned long /*size*/)
	{
	return ENOTSOCK;
	}

TInt CFileDescBase::Listen(TUint /*qSize*/)
	{
	return ENOTSOCK;
	}

void CFileDescBase::Accept(CFileDescBase*& /*aNewSocket*/, TRequestStatus& aStatus, RSocketServ& /*aSs*/,  TSockAddr * /*aAddr*/)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	}

void CFileDescBase::AcceptCancel() {}

void CFileDescBase::Connect(const struct sockaddr* /*anAddr*/,unsigned long /*aSize*/,TRequestStatus& aStatus)
	{
	// minimal implementation
	Complete(aStatus,ENOTSOCK);
	User::WaitForRequest(aStatus);
	}

void CFileDescBase::ConnectCancel() {}

TBool CFileDescBase::GetConnectionProgress()
	{
	return EFalse;
	}

void CFileDescBase::SetConnectionProgress( TBool /* aInProgress */ ) {}

TInt CFileDescBase::SockName(int /*anEnd*/, struct sockaddr* /*anAddr*/,unsigned long* /*aSize*/)
	{
	return ENOTSOCK;
	}

TInt CFileDescBase::GetSockOpt(TInt  /*anOptionName*/,TInt /*anOptionLevel*/,TDes8& /*anOption*/)
	{
	return ENOTSOCK;
	}

TInt CFileDescBase::SetSockOpt(TInt  /*anOptionName*/,TInt /*anOptionLevel*/,TDesC8& /*anOption*/)
	{
	return ENOTSOCK;
	}

// -----------------------------------------------------------------------------
// CFileDescBase::Fcntl
// Minimal Implementation for fcntl
// -----------------------------------------------------------------------------
//
TInt CFileDescBase::Fcntl(TUint /*anArg*/, TUint aCmd)
	{
	// minimal implementation: 
	if(aCmd == F_GETFL)
		{
		return KErrNotFound;
		}
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CFileDescBase::Poll
// Checks the file for ready to write/read and exception status
// -----------------------------------------------------------------------------
//
TInt CFileDescBase::Poll(TPollMode /*aMode*/,TBool& aStatus,TInt& /*aErrno*/)
    {
    // always return True, let the derived filedescriptors override
    aStatus = ETrue;
    return 0;
    }

// -----------------------------------------------------------------------------
// CFileDescBase::Poll - Overloaded
// Synchronous non-blocking call that returns current state of the file descriptor
// -----------------------------------------------------------------------------
//
TInt CFileDescBase::Poll(TUint aEvents)
    {
    // We return that the file descriptor is select true for ready to read, 
    // ready to write and error conditions. 
    // Let derived file descriptors override as approp.
    TInt readyEvents = 0;
    
    if (aEvents & EReadyForReading)
		{
		readyEvents = EReadyForReading;
		}
	if (aEvents & EReadyForWriting)
		{
		readyEvents |= EReadyForWriting;
		}
	if (aEvents & EAnyException)
		{
		readyEvents |= EAnyException;
		}

    return readyEvents;
    }

// -----------------------------------------------------------------------------
// CFileDescBase::NotifyActivity
// Registers with the approp. underlying subsystem for read/write/except notifications
// -----------------------------------------------------------------------------
//
TInt CFileDescBase::NotifyActivity(TUint /*aEvents*/, TRequestStatus& /*aRequest*/, TTimeIntervalMicroSeconds32 /*timeout*/)
	{
    // We return KErrCompletion indicating that this activity is complete for 
    // this descriptor.
    // Let derived file descriptors override as approp.
	return KErrCompletion;
	}

// -----------------------------------------------------------------------------
// CFileDescBase::TweakWatchedEvents
// Requests for fd behaviour specific additional events
// -----------------------------------------------------------------------------
//
void CFileDescBase::TweakWatchedEvents(TUint& /*events*/)
    {
    // No implementation required
    // This version is used for both Redir and pipe desc, it is overridden
    // by socket
    }

// -----------------------------------------------------------------------------
// CFileDescBase::TweakReadyEvents
// Prepares the fd behaviours specific output events
// -----------------------------------------------------------------------------
//
TInt CFileDescBase::TweakReadyEvents(TInt /*errval*/)
    {
    return KErrNone;
    }
	
// CFileDescBase::CancelNotify
// Cancel an outstanding request for read/write/except notifications
// -----------------------------------------------------------------------------
//
void CFileDescBase::CancelNotify()
	{
	// do nothing
	return;
	}


// Generic (non-virtual) handling for Close
EXPORT_C TInt CFileDescBase::Close()
	{ 
	TInt err = KErrNone;
	//Decrement the dup count and mark this FD as invalid if dupcount is 0
	if ( --iDupCount < 0 )
		{
		iFdAttrib |= KInvalidFd;
		//delete this FD only if its not mmaped and being used
		if(!(iFdAttrib & KMMapedFd))
			{
			err=FinalClose();
			//Release the Lock now
			delete this;
			}
		}
	return err;
	}

TInt CFileDescBase::FinalClose()
	{
	return KErrNone;
	}

void CFileDescBase::SetFids(void *)
	{
	// nada (virtual)
	}

// Simple implementation of File handling
static int MapMode(int aMode, TUint& fMode)
	{
	// EPOC32 doesn't support Write-Only
	if (aMode & (O_WRONLY|O_RDWR))
		{
		fMode = EFileWrite;
		}
	else
		{
		fMode = EFileRead;
		}

	fMode |= (aMode & O_TEXT) ? EFileStreamText : EFileStream;
	fMode |= EFileShareReadersOrWriters;

	return aMode & (O_CREAT|O_TRUNC|O_APPEND|O_EXCL|O_EXLOCK);
	}

CFileDesc::CFileDesc()
	:CFileDescBase(), iSize(EBufferSize), iExt(-1)
	{}

void CFileDesc::SetState(const TDes& params)
	{
	TFileName name;
	iFile.FullName(name);
	iDrive = (TInt16)TDriveUnit(name);

	TLex lexr(params);
	lexr.Val(iPos);
	lexr.Inc();
	lexr.Val(iExt);
	lexr.Inc();
	lexr.Val(iFcntlFlag);
	lexr.Inc();
	lexr.Val(iFdAttrib, EDecimal);
	lexr.Inc();
	lexr.Val(iPos);
	lexr.Inc();
	lexr.Val(iExt);
	lexr.Inc();
	lexr.Val(iSize);
	
	}

CFileDesc::~CFileDesc()
	{
	iFile.Close();
	if(iBuffer)
		{
		RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
		delete [] iBuffer;
		User::SwitchHeap(oldHeap);
		}
	}

TInt CFileDesc::FinalClose()
	{
	TInt err = DoSync();
	iLock.Close();
	return err;
	}
	
TInt CFileDesc::Open(RFs& aSession, const TDesC& aName, int mode, int perms)
	{
	TInt err;
	TUint fMode;
	
	iDrive=(TInt16)TDriveUnit(aName);

	// Create  = make new file, can return KErrAlreadyExists
	// Open    = open an existing file, can return KErrPathNotFound or KErrNotFound
	// Replace = open a new file, zapping the existing one if necessary

	int mapped=MapMode(mode, fMode);
	// if the file is in \sys, use EFileShareReadersOnly not EFileShareReadersOrWriters
	TParsePtrC pars(aName);
	if (pars.Path().FindF(_L("\\SYS\\")) == 0)
		{
		fMode &= ~EFileShareReadersOrWriters;
		fMode |= EFileShareReadersOnly;
		}
	
	// if the file is in \resource, dont use EFileShareReadersOrWriters
	if (pars.Path().FindF(_L("\\resource\\")) == 0)
		{
		fMode &= ~EFileShareReadersOrWriters;
		fMode |= EFileShareReadersOnly;
		}

	//Set iFcntlFlag flag	
	iFcntlFlag = mode & O_ACCMODE;	
    
    if(mode & O_APPEND) 
    	{
    	iFcntlFlag |= O_APPEND;
    	}
    	
	TInt fileMode = 0;
	TBool created = EFalse;
	//store the open modes in the file descriptor object

	switch(perms & (S_IRUSR | S_IWUSR ))  
		{
		case S_IRUSR | S_IWUSR: //owner read write
		case S_IWUSR :         //owner write   
		    {
			break;
			}
			case S_IRUSR:           //owner read 
			{
			fileMode = KEntryAttReadOnly;
			break;
			}
		default:
			{
			 break;
			}
		}

	switch (mapped)
		{
		case O_CREAT|O_EXCL: //For O_EXCL if file exists then return error
			err = iFile.Create(aSession, aName, fMode);
            if(err)   
            	{
                return err;
                }  
                created = ETrue ;
			break;
		case O_CREAT|O_TRUNC:
			err = iFile.Replace(aSession, aName, fMode);
			if(!err)
				{
				created = ETrue;
				}   
			break;
		case O_TRUNC:
			err = iFile.Open(aSession, aName, fMode);
			if(!err)				
			    err = iFile.SetSize(0);
			break;
		case O_CREAT|O_EXCL|O_APPEND: //if file exists then return error
			err = iFile.Create(aSession, aName, fMode);
            if(err)   
            	{
                return err;
                }  
                created = ETrue ;
			break;
		case O_CREAT|O_EXCL|O_TRUNC: //if file exists then return error
			err = iFile.Create(aSession, aName, fMode);
            if(!err)   
            	{
                err = iFile.SetSize(0);
                created = ETrue ;                
                }  
            else
            	{
            	return err;
            	}  
			break;
		case O_EXLOCK: //emulating exclusive locks
			err = iFile.Open(aSession, aName, fMode);
            if(err == KErrNone)   
            	{
            	FSIZE fileSize = 0;
            	err = iFile.Size(fileSize);
            	if(fileSize <= 0 )
            		return KErrNotSupported;
            	
            	if(err == KErrNone)
            		{
            		err = iFile.Lock(0, fileSize);
            		}
                } 
   			
            if(err != KErrNone)   
            	{
                return err;
                } 
			break;	
		// Everything else is assumed to mean open existing file,
		// If the file isn't there, O_CREAT implies that we should make it
		default:
			err = iFile.Open(aSession, aName, fMode);
			if (err == KErrNotFound && (mapped & O_CREAT))
				{ 
			    err = iFile.Create(aSession, aName, fMode);
				if(!err)   
					{
					created = ETrue;
					}  

				} 
			if((err == KErrArgument) &&(mode == (O_CREAT | O_RDONLY)))
				{
				TUint CrMode = EFileShareReadersOrWriters | EFileWrite ;
				CrMode |= (mode & O_TEXT) ? EFileStreamText : EFileStream;
				err = iFile.Create(aSession , aName , CrMode);
			  
				if(!err)  
					{
					created = ETrue;
					iFile.Close() ;
					err = iFile.Open(aSession, aName , fMode);
					}
				}

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
	
	//If File is to be opened for O_CREAT | O_RDONLY, set the corresponding attributes
	if(created && fileMode) 
		{
		err = iFile.SetAtt(fileMode, 0) ;	
		}
		
	if (!err)
		{
		err = Alloc();
		}
	
	if(err == KErrNone)
		{
		err = CreateLock();
		}
	
	return err;
	}

TInt CFileDesc::LSeek (off_t& offset, int whence)
	{
	//return if its an invalid fd
	//This scenario comes when the file is closed, but mmap still exists
	if(iFdAttrib & KInvalidFd)
		{
		return KErrBadHandle;	
		}
	iLock.Wait();
	FSIZE pos=offset;
	FSIZE ext=Ext();
	if (ext < 0)
		{
		iLock.Signal();
		return ext;
		}
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
			{
			iLock.Signal();
			return KErrArgument;
			}
		}

	TInt ret = KErrNone;
	if (pos < 0)
		{
		iLock.Signal();
		return KErrArgument;
		}
	else if (pos >= ext)
		{
		ret = Flush();		
		if (!ret)
			{
			iState = EAlloc;
			}
		}

	switch (iState)
		{
		case EAlloc:
			iPos = pos;
			break;
		case EReading:
			{
			FSIZE lag = iPos - pos;
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
				if (ret != KErrNone)
					{
					iLock.Signal();
					return ret;
					}
					iPos = pos;
				}
			break;
		default:
			{
			//Do nothing
			}
		}
	offset = pos;
	iLock.Signal();
	return ret;
	}

void CFileDesc::MapStat(struct stat& st, const TTime& aModTime, TUint& aAttr, const mode_t aMode)
    {
    if ( aMode == S_IFREG)
        {
        st.st_mode = (aAttr&KEntryAttDir) ? S_IFDIR: S_IFREG;
        }
    else
        {
        st.st_mode = aMode;
        }
    
    if ((aAttr&KEntryAttReadOnly))
        {
		st.st_mode |= S_IRUSR;
	    }
	 else
	 	{
	 	st.st_mode = st.st_mode | S_IWUSR | S_IRUSR;
	 	}
	    
    st.st_nlink = (S_IFLNK == st.st_mode) ? 2 : 1;
	TTimeIntervalSeconds res;
	const TTime KEpocTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000));    // 00:00, Jan 1st 1970
    if(aModTime.SecondsFrom(KEpocTime, res))
		{
		st.st_mtime = -1;
		st.st_atime = -1 ;
		st.st_ctime = -1;
		}
	else
		{
		st.st_mtime = res.Int();
		st.st_ctime = st.st_atime = st.st_mtime ;   //here modification and access time are same
		}
    st.st_blksize=512;
    }

TInt CFileDesc::FStat (struct stat* st)
	{
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		return KErrBadHandle;	
		}
	TInt err;
	TUint att;
	TTime modtime;
	
	err = iFile.Att(att);
	if (!err)
	    {
	    err = iFile.Modified(modtime);
	    if (!err)
		{
		FSIZE ext=Ext();
		if (ext >= 0)
		    {
		    st->st_size = ext;
		    st->st_dev = st->st_rdev = iDrive;
		    MapStat(*st, modtime, att);
		    return 0;
		    }
		else
			err = ext;
		}
	    }
	return err;
	}

TInt CFileDesc::SetAtt(TUint aSetAttMask, TUint aClearAttMask) 
	{
	return iFile.SetAtt(aSetAttMask , aClearAttMask) ;
	}

TInt CFileDesc::Alloc()
	{
	if (iSize)
		{
		//Delete if iBuffer is holding some memory already
		if (iBuffer)
			{
			delete [] iBuffer;
			}
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
	if (iExt >= 0) 
   		{ 
        if (iPos > iExt) 
    	    { 
			iFile.SetSize(iPos); 
			} 
		}
	
	// Check if the specified file is opened in O_APPEND 
	// mode and set right the file write position
	if(iFcntlFlag & O_APPEND)
		{
		FSIZE bytes=0;
		TInt ret = iFile.Size(bytes);
		if (ret < 0)
			return ret;
		iPos = Max(bytes, Pos());
		}
		
	TInt r = iFile.Write(iPos,ptr);
	if (r == KErrNone)
		{
		r = ptr.Length();
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
		if (iState != EAlloc)
			{
			TInt ret = Flush();
			if (ret != KErrNone)
				{
				return ret;
				}
			}
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
	return aDesc.Length();
	}

void CFileDesc::Read (TDes8& aDesc, TRequestStatus& aStatus)
	{
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}
	//Acquire the Lock before read and release it later
	iLock.Wait();
	Complete(aStatus,DoRead(aDesc));
	iLock.Signal();
	}

TInt CFileDesc::DoWrite (TDes8& aDesc)
	{
	if (iState != EWriting)
		{
		if (iState != EAlloc)
			{
			iPos -= iEnd - iPtr;
			}

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
		return len;
	TInt r=Flush();
	if (r < 0)
		return r;
	if (max >= iSize)
		return  len + FileWrite(p, max);
	iPtr = Mem::Copy(iPtr, p, max);
	return len + max;
	}

void CFileDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}
	//Acquire the Lock before write and release it later
	iLock.Wait();
	Complete(aStatus,DoWrite(aDesc));
	iLock.Signal();
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
	iLock.Wait();
	Complete(aStatus,DoSync());
	iLock.Signal();
	}

FSIZE CFileDesc::Pos()
	{
	FSIZE pos = iPos;
	if (iState == EReading)
		pos -= (iEnd - iPtr);
	else if (iState == EWriting)
		pos += (iPtr - iBuffer);
	return pos;
	}

FSIZE CFileDesc::Ext()
	{
	TInt r = iFile.Size(iExt);
	if (r < 0)
		return r;
	return iExt;
	}

TInt CFileDesc::IoctlCompletion(int aCmd, void* aParam, TInt aStatus)
	{
	TInt ret=aStatus;
	if (ret!=KErrNone)
		return ret;
	// some useful sums about the current state of the file
	FSIZE curoff = Pos();
	FSIZE size = Ext();
	if (size < 0)
		ret = size;
	int *param=reinterpret_cast<int*> (aParam);
	switch ((unsigned)aCmd)
		{
	case FIONREAD:
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


#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
// this needs to be corrected once the api which gives the large file size is available.
//#define MAX_SIZE	LLONG_MAX  /* 2^63-1  - MAX file size */
#define MAX_SIZE    INT_MAX    /* 2^31-1  - MAX file size */
#else
#define MAX_SIZE 	INT_MAX    /* 2^31-1  - MAX file size */
#endif //SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS

TInt CFileDesc::ProcessLockParams(FSIZE& pos, FSIZE& lock_len, TInt& lock_type, struct flock* lock)
    {
    TInt retVal=0;
    lock_type = lock->l_type;
	if(!lock->l_len) 		//if len = 0 then lock must be extended to the system limit for max file size
		{
		lock_len=MAX_SIZE;
		}
	else 		//len argument is non-zero
		{
		lock_len = lock->l_len;
		}
	pos =lock->l_start;	//offset		
	switch( lock->l_whence )
		{
		case SEEK_SET:
		
			if(pos<0)
				{
				retVal = KErrArgument; 	
				return retVal;	
				}
			else
				break;
		case SEEK_CUR:
			pos += iPos;	//current position + offset
			break;
		case SEEK_END: 
			retVal = iFile.Size(pos); 								
			switch( retVal)
				{
				case KErrNone: 
					pos += lock->l_start;	//file size + ofset
					break;
				default: 
					return retVal; 								
				}
			
			if(pos<0)
				{
				retVal = KErrArgument; 	
				return retVal;	
				}
			break;
		default: 
			retVal = KErrArgument;	//invalid argument
			return retVal;																	
		 }		
			
	if( lock_len < 0)
		{
		pos += lock_len;
		lock_len = -lock_len;
		}
    return retVal;
     }
// -----------------------------------------------------------------------------
// CFileDesc::Fcntl
// Symbian File Specific Implementation for fcntl
// There is no mapping from Symbian RFile for File fcntl.
// This implementation supports fcntl with F_SETFL and F_GETFL as cmd
// We can set the Flag as Blocking or Non-Blocking.
// -----------------------------------------------------------------------------
//
TInt CFileDesc::Fcntl(TUint anArg, TUint aCmd)
	{
	struct flock *lock = NULL;
	FSIZE pos = 0,lock_len = -1;
	TInt lock_type = -1;
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		return KErrBadHandle;
		}
	TInt retVal = KErrNone;
	//fcntl supports only F_SETFL and F_GETFL for Non-Blocking I/O
	//If aCmd and anArg does not match these, return with Error
	switch( aCmd )
		{
		case F_SETFL:
			{
			//Set the fcntl Flag
			if((anArg == O_APPEND) || (anArg == O_NONBLOCK)) 
				{
				iFcntlFlag |=  anArg ;	
				}
		retVal = 0 ;
		break;
			}
		case F_GETFL:
			{
			//Return fcntl flag
			retVal = iFcntlFlag;
			break;
			}
		case F_GETFD:
			{
			if(iFdAttrib & KCloseonExec)
				{
				retVal = 1;
				}
			break;
			}
		case  F_SETFD:
			{
			if(anArg == 1) 
				{
				iFdAttrib |= SET_CLOSE_ON_EXEC_FLG; // (1 << 2) ;
				retVal = anArg;
				}
			else if(anArg == 0 ) 
				{
				iFdAttrib &= ~SET_CLOSE_ON_EXEC_FLG; //(1 << 2) ;
				retVal = anArg;
				}
			else  
				{
				retVal = KErrArgument;    //return invalid argument 
				}
			break;
			}
		case F_GETLK:
			{
			lock = (struct flock*)anArg;				
		    retVal=ProcessLockParams(pos, lock_len, lock_type, lock);
		    if(retVal)
		    return retVal;
		    switch( lock_type)
				{
				case F_WRLCK:	
					retVal =  iFile.Lock(pos ,lock_len); 	//try to lock the region.
					if (retVal  == KErrNone)
						{						
						retVal =  iFile.UnLock(pos ,lock_len);   //if previous lock() is successful then unlock the region
						lock->l_type = F_UNLCK;				     //indication that region can be locked
						}
					else
						{
						retVal = KErrNone;		//region cant be lock. return success, but don't change lock->l_type
						}
					break;
				default:
					retVal = KErrArgument;								
				}
			break;
			}	 
		case F_SETLK:
			{
			lock = (struct flock*)anArg;
			retVal=ProcessLockParams(pos, lock_len, lock_type, lock);
			if(retVal)
			return retVal;
		    switch( lock_type)
				{
				case F_WRLCK:
					retVal =  iFile.Lock(pos ,lock_len);	//try to lock without waiting
					break;
				case F_UNLCK:
					retVal =  iFile.UnLock(pos ,lock_len);	//try to unlock without waiting
					break;
				case F_RDLCK:
					retVal = KErrNotSupported;

				default:
				retVal = KErrArgument;    //return invalid argument 
				}
			break;
			}
		default:
			retVal = KErrNotSupported;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CFileDesc::Truncate
// Symbian File Specific Implementation for Truncate
// This will truncate the file which is opened in write mode
// -----------------------------------------------------------------------------
//
TInt CFileDesc::Truncate(off_t anOffset)
	{
	TInt retVal;
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		return KErrBadHandle;
		}
	iExt = anOffset;
	retVal = iFile.SetSize(anOffset);
	if (retVal == KErrNotSupported)
		{
		return KErrArgument;//to work around RFile64::SetSize() return value
		}
	return retVal;
	}

// Extra support for temporary files
TInt CTempFileDesc::Open(const wchar_t* aName, TInt mode)
	{
	iSession = Backend()->FileSession();
	
	TFileName filePath;
	if (mode & O_TMPFILE) //from tmpfile()
		{				
		//obtain original mode for tmpfile()
		mode &= (~O_TMPFILE);
		filePath.Copy((const TText*)aName);
		}
	else //for "TMP:"
		{
		TParse path;
		TInt err = GetFullPath(path, (const TText16*)WIDEP_tmpdir, NULL);
		if (err != KErrNone)
			{
			return err;	
			}				
		filePath.Copy(path.DriveAndPath());
		}

	if((mode & O_BUFFERED) == 0)
		{
		setSize(0);
		}
	iFcntlFlag = mode & O_ACCMODE;//setting the fcntl flag
	TInt ret = Alloc();
	if (ret != KErrNone)
		{
		return ret;
		}
	TFileName aPathName;
	aPathName.Append(iSession.GetSystemDriveChar());
	aPathName.Append(TChar(KDriveDelimiter));
	aPathName.Append(_L("\\System\\temp"));	
	if (filePath.Find(aPathName) == 0)
		{			
		ret = iSession.MkDir(filePath);
		if (ret != KErrNone && ret != KErrAlreadyExists)
			{
			return ret;	
			}		
		else
			{
			ret = iFile.Temp(iSession, filePath, iName, EFileShareReadersOrWriters);
			}		
		}			
	else		
		{
		ret = iFile.Create(iSession, filePath, EFileShareReadersOrWriters);			
		}	
	return ret;		
	}

TInt CTempFileDesc::FinalClose()
	{	
  	TInt ret = iFile.FullName(iName);
  	if (ret != KErrNone)
  		{
  		return ret;
  		}
  	iFile.Close();
	return iSession.Delete(iName);
	}

#ifdef SYMBIAN_OE_LIBRT
// -----------------------------------------------------------------------------
// CSharedMemDesc::CSharedMemDesc : Constructor
// Constructs sharedmemory Descriptor
// -----------------------------------------------------------------------------
CSharedMemDesc::CSharedMemDesc()
	{
	iLock.CreateLocal();
	}

// -----------------------------------------------------------------------------
// CSharedMemDesc::CSharedMemDesc : Destructor
// Destructor for sharedmemory Descriptor
// -----------------------------------------------------------------------------
CSharedMemDesc::~CSharedMemDesc()
	{
	iLock.Close();	
	}

// -----------------------------------------------------------------------------
// CSharedMemDesc::Open
// Gives sharedmemory Descriptor
// -----------------------------------------------------------------------------
TInt CSharedMemDesc::Open(const wchar_t* aName, int mode, int perms)
	{
	int aerr = 0, err =0;
	void* ptr = NULL;
	TInt shmkey = 0;
	TInt shmid = 0;
	struct shmid_ds *buf = NULL;
	
	mode &= (~O_SHMFLG);
	if(mode == O_WRONLY)
		{
		return KErrNone;
		}		
		
	TFileName shrdmemname;
	shrdmemname.Copy((const TText*)aName);					 			
	
	iPerms = perms;
	iFcntlFlag = mode & O_ACCMODE;
	shmkey = GeneratePathKey(shrdmemname);
	switch (mode)
		{		
		case O_CREAT:
		case O_CREAT|O_RDWR:
			shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, IPC_CREAT|perms, aerr);
			if (shmid == -1)
				{
				err = aerr;
				return err;
				}
		break;
		case O_TRUNC:
		case O_TRUNC|O_RDWR:
		case O_CREAT|O_TRUNC:
			shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, IPC_CREAT|perms, aerr);
			if (shmid == -1)
				{
				err = aerr;
				return err;
				}
			if (Backend()->iIpcS.shmctl(shmid, IPC_RMID, buf, aerr) == -1)
				{
				err = aerr;
				return err;
				}
			shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, IPC_CREAT|perms, aerr);
			if (shmid == -1)
				{
				err = aerr;
				return err;
				}
		break;
		case O_CREAT|O_EXCL:
		case O_CREAT|O_EXCL|O_RDWR:
			shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, IPC_CREAT|IPC_EXCL|perms, aerr);
			if (shmid == -1)
				{
				err = aerr;
				return err;
				}
		break;
		case O_CREAT|O_EXCL|O_TRUNC:
		case O_CREAT|O_EXCL|O_TRUNC|O_RDWR:
			shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, IPC_CREAT|IPC_EXCL|perms, aerr);
			if (shmid == -1)
				{
				err = aerr;
				return err;
				}
			if (Backend()->iIpcS.shmctl(shmid, IPC_RMID, buf, aerr) == -1)
				{
				err = aerr;
				return err;
				}
			shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, IPC_CREAT|IPC_EXCL|perms, aerr);
			if (shmid == -1)
				{
				err = aerr;
				return err;
				}
		break;
		default:
		shmid = Backend()->iIpcS.shmget(shmkey, SHM_CHUNKSIZE, perms, aerr);
		if (shmid == -1)
			{
			err = aerr;
			return err;
			}
		break;		
		}
	ptr = Backend()->iIpcS.shmat(shmid, (void *)NULL, 0, aerr);
	if (ptr == (void*)-1)
		{
		err = aerr;
		return err;
		}
	iPtr = ptr;
	iKey = shmkey;
	return err;
	}

TInt CSharedMemDesc::ShmRead(TUint8* aPtr,TInt aLength)
	{
	TInt err = KErrNone;
	iSize = Backend()->iIpcS.GetShmSize(iKey, err);
	TInt diff = iSize - iPos;
	if (aLength >= diff)
		aLength = diff;
	TPtr8 ptr(aPtr, aLength);
	ptr.Copy((TUint8*)(iPtr)+iPos, aLength);
	TInt r = ptr.Length();
	iPos += r;
	return r;
	}

TInt CSharedMemDesc::DoShmRead (TDes8& aDesc)
	{
	TUint8* p = (TUint8*) aDesc.Ptr();
	TInt max = aDesc.MaxLength();
	TInt ret = ShmRead(p, max);
	p += ret;
	aDesc.SetLength(p-aDesc.Ptr());
	return aDesc.Length();
	}


// -----------------------------------------------------------------------------
// CSharedMemDesc::Read
// Reading from a sharedmemory
// -----------------------------------------------------------------------------
void CSharedMemDesc::Read(TDes8& aDesc, TRequestStatus& aStatus)
	{
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}
	//Acquire the Lock before read and release it later
	iLock.Wait();
	Complete(aStatus,DoShmRead(aDesc));
	iLock.Signal();
	}

TInt CSharedMemDesc::ShmWrite(TUint8* aPtr,TInt aLength)
	{
	TInt err = KErrNone;
	TInt size = Backend()->iIpcS.GetShmSize(iKey, err);
	TPtrC8 ptr(aPtr,aLength);
	TUint8* bufPtr = const_cast<TUint8*>(ptr.Ptr());
	TInt len = ptr.Length();
	TInt r = Min(aLength, len);	
	Mem::Copy((TUint8*)(iPtr)+iPos, bufPtr, r);
	iPos += r;
	if (iPos > iExt && iExt >= 0)
		iExt = iPos;
	if (iExt <= size)
		iSize = iExt = size;
	else
		iSize = iExt;
	Backend()->iIpcS.SetShmSize(iKey, iSize, err);
	return r;
	}

TInt CSharedMemDesc::DoShmWrite (TDes8& aDesc)
	{
	TUint8* p = (TUint8*) aDesc.Ptr();
	TInt max = aDesc.Length();
	return  ShmWrite(p, max);
	}

// -----------------------------------------------------------------------------
// CSharedMemDesc::Write
// Writing to sharedmemory
// -----------------------------------------------------------------------------
void CSharedMemDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		Complete(aStatus, KErrBadHandle);
		return;
		}
	//Acquire the Lock before write and release it later
	iLock.Wait();
	Complete(aStatus,DoShmWrite(aDesc));
	iLock.Signal();
	}

// -----------------------------------------------------------------------------
// CSharedMemDesc::Fcntl
// fcntl Implementation for sharedmemory 
// -----------------------------------------------------------------------------
//
TInt CSharedMemDesc::Fcntl(TUint anArg, TUint aCmd)
	{
	//return if its an invalid fd
	if(iFdAttrib & KInvalidFd)
		{
		return KErrBadHandle;
		}
	TInt retVal = KErrNotSupported;
	//fcntl supports only F_SETFL and F_GETFL for Non-Blocking I/O
	//If aCmd and anArg does not match these, return with Error
	switch(aCmd)
		{
		case F_SETFL:
			{
			//Set the fcntl Flag
			if(anArg == O_NONBLOCK)
				{
				iFcntlFlag |=  anArg ;	
				}
			retVal = 0 ;
			break;
			}
		case F_GETFL:
			{
			//Return fcntl flag
			retVal = iFcntlFlag;
			break;
			}
		case F_GETFD:
			{
			if(iFdAttrib & KCloseonExec)
				{
				retVal = 1;
				}
			break;
			}
		case  F_SETFD:
			{
			if(anArg == 1) 
				{
				iFdAttrib |= SET_CLOSE_ON_EXEC_FLG; // (1 << 2) ;
				retVal = anArg;
				}
			else if(anArg == 0 ) 
				{
				iFdAttrib &= ~SET_CLOSE_ON_EXEC_FLG; //(1 << 2) ;
				retVal = anArg;
				}
			else  
				{
				retVal = KErrArgument;    //return invalid argument 
				}
			break;
			}
		}
	return retVal;	
	}

// -----------------------------------------------------------------------------
// CSharedMemDesc::FStat
// stat Implementation for sharedmemory 
// -----------------------------------------------------------------------------
//
TInt CSharedMemDesc::FStat(struct stat *st)
	{
	// set mode as directory
	TInt err = 0, aerr =0;
	TInt shmid = 0;
	struct shmid_ds shmDesc;
	st->st_mode = S_IFREG|iPerms;
	shmid = Backend()->iIpcS.shmget(iKey, SHM_CHUNKSIZE, iPerms, aerr);
	Backend()->iIpcS.shmctl(shmid, IPC_STAT, &shmDesc, aerr);
	st->st_uid = shmDesc.shm_perm.cuid;
	st->st_gid = 0;
	st->st_size = Backend()->iIpcS.GetShmSize(iKey, err);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSharedMemDesc::FinalClose
// stat Implementation for sharedmemory 
// -----------------------------------------------------------------------------
//
TInt CSharedMemDesc::FinalClose()
	{
	TInt err = 0;
	TInt ret = KErrNone;
	ret = Backend()->iIpcS.shmdt(iPtr, err);
	iLock.Close();
	return ret;
	}
	
TInt CSharedMemDesc::Pos()
	{
	return iPos;
	}

TInt CSharedMemDesc::Ext()
	{
	return iExt;
	}
	
//-------------------------------------------------------------------------------
// Function Name : CSharedMemDesc::LSeek()
// Description   : Shall seek to specific position of shared memory
//-------------------------------------------------------------------------------	
TInt CSharedMemDesc::LSeek (off_t& offset, int whence)
	{
	//return if its an invalid fd
	//This scenario comes when the file is closed, but mmap still exists
	if(iFdAttrib & KInvalidFd)
		{
		return KErrBadHandle;	
		}
	TInt pos=offset;
	TInt ext=Ext();
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
	if (pos < 0)
		{
		return KErrArgument;
		}
	iPos = pos;
	offset = pos;
	return KErrNone;
	}
	
#endif //SYMBIAN_OE_LIBRT

