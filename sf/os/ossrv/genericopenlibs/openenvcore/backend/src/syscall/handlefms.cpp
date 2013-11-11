/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// connectors for re-entrant networking system calls

#include <sys/errno.h>
#include "sysreent.h"
#include "aeselectreent.h"
#include <unistd.h>
#include <fcntl.h>		// for open()
#include <sys/ioctl.h>
#include <stdarg.h> 
#include <utf.h>
#include <unistd.h>
#include <sys/types.h>
#include <spawn.h>
#include <charconv.h>
#include "sys/stat.h"
#include <wchar.h>
#include "systemspecialfilercg.h"
#include "link.h"
#include "errno.h"

#include "sysif.h"
#include "lposix.h"



#define	MAXPATHLEN	256	/* E32STD.H: KMaxFullName */

TInt __EPOC32_WDIR::UpdateNarrow()
	{
	return -1;		//this is not supported.
	}
	
TInt __EPOC32_DIR::UpdateNarrow()
	{
	//update the narrow one
	
	TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(iCurrentNarrowName, iCurrentName);
	if (ret >= 0)
		{
		iCurrentNarrow.d_namlen=(short)iCurrentNarrowName.Length();
		memcpy(iCurrentNarrow.d_name,iCurrentNarrowName.PtrZ(),iCurrentNarrowName.Length() + 1);
		}
	return ret;
	}

__EPOC32_WDIR::~__EPOC32_WDIR()
	{
	delete iPath;
	delete iEntries;
	}

TInt __EPOC32_WDIR::Open(const wchar_t* _path,int* aErrno)
	{
	TParse name;
	TInt err = Backend()->ResolvePath(name,_path,NULL);
	if (!err)
		err = Open(name.DriveAndPath());
	if (err)
		MapError(err,*aErrno);
	return err;
	}

TInt __EPOC32_WDIR::Open(const TDesC& aPath)
	{
	delete iPath;
	iPath=aPath.Alloc();		//allocate space for a hbufc and initialise it to the TDesC contents
	return Open();
	}

TInt __EPOC32_WDIR::Open()
	{
	delete iEntries;
	iEntries=0;
	iIndex = 0;
	iCount = 0 ;
	if (iPath==0)
		{
		return ENOMEM;
		}
		
	RFs& session = Backend()->FileSession();
	TInt err=session.GetDir(*iPath,KEntryAttMaskSupported,ESortByName+EDescending,iEntries);
		
	if (!err) 
		{
		// iCount is actually a Last Index, not the number of entries!
		iCount = iEntries->Count()-1;
		iIndex = 0;
		}
	return err;
	}

extern "C" {

/* A reentrant version of open() */
EXPORT_C int _open_r (int *aErrno, const wchar_t *name, int mode, int perms)
	{	
	return Backend()->open(name, mode, perms, *aErrno);
	}

/*A reentrant version of wopen() */
EXPORT_C int _wopen_r (int *aErrno, const wchar_t *name, int mode, int perms)
	{
	return Backend()->open(name, mode, perms, *aErrno);
	}

/* A reentrant version of read()*/
EXPORT_C int _read_r (int *aErrno, int fd, char *buf, size_t nbyte)
	{
	return Backend()->read(fd, buf, nbyte, *aErrno);
	}


/* A reentrant version of write()*/
EXPORT_C int _write_r (int *aErrno, int fd, const char *buf, size_t nbyte)
	{
	return Backend()->write(fd, buf, nbyte, *aErrno);
	}


/* A reentrant version of close()*/
EXPORT_C int _close_r (int *aErrno, int fd)
	{
	return Backend()->close(fd, *aErrno);
	}

/* A reentrant version of fsync()*/
EXPORT_C int _fsync_r (int *aErrno, int fd)
	{
	return Backend()->fsync(fd, *aErrno);
	}

/*A reentrant version of fseek()*/
EXPORT_C off_t _lseek_r (int *aErrno, int fd, off_t pos, int whence)
	{
	return Backend()->lseek(fd, pos, whence, *aErrno);
	}


/* A reentrant version of fstat()*/
EXPORT_C int _fstat_r (int *aErrno, int fd, struct stat *st) 
	{
	return Backend()->fstat(fd, st, *aErrno);
	}

/* A reentrant version of stat().
*/
EXPORT_C int _stat_r (int *aErrno, const wchar_t *name, struct stat *st) 
	{
	return Backend()->stat(name, st, *aErrno);
	}
	
/* A reentrant version of lstat().
*/
EXPORT_C int _lstat_r (int *aErrno, const wchar_t *name, struct stat *st) 
    {
    return Backend()->lstat(name, st, *aErrno);
    }

EXPORT_C int _utime_r (int *aErrno, const wchar_t *name,const  struct utimbuf *filetimes) 
	{
	return Backend()->utime(name, filetimes, *aErrno);
	}


/* A reentrant version of wstat().
*/
EXPORT_C int _wstat_r (int *aErrno, const wchar_t *name, struct stat *st) 
	{
	return Backend()->stat(name, st, *aErrno);
	}

/* A reentrant version of dup().
*/
EXPORT_C int _dup_r (int *aErrno, int aFid)
	{
	return Backend()->dup(aFid, *aErrno);
	}

/* A reentrant version of dup2().
*/
EXPORT_C int _dup2_r (int *aErrno, int aFid1, int aFid2)
	{
	return Backend()->dup2(aFid1, aFid2, *aErrno);
	}

/* A reentrant version of ioctl().
*/
EXPORT_C int _ioctl_r (int *aErrno, int aFid, int aCmd, void* aParam)
	{
	return Backend()->ioctl(aFid, aCmd, aParam, *aErrno);
	}

/* A wide-character version of reentrant of getcwd().
*/
EXPORT_C wchar_t * _wgetcwd_r (int *aErrno, wchar_t *_buf, size_t _size)
	{
	if (_buf==0)
		{
		_buf=(wchar_t *)User::Alloc(_size*sizeof(wchar_t));
		if (_buf==0)
			{
			*aErrno=ENOMEM;
			return _buf;
			}
		}
	return Backend()->getcwd(_buf, _size, *aErrno);
	}

/* A reentrant version of chdir().
*/
EXPORT_C int _chdir_r (int *aErrno, const wchar_t *_path)
	{
	return Backend()->chdir(_path, *aErrno);
	}

/* A reentrant version of wchdir().
*/
EXPORT_C int _wchdir_r (int *aErrno, const wchar_t *_path)
	{
	return Backend()->chdir(_path, *aErrno);
	}

/* A reentrant version of rmdir().
*/
EXPORT_C int _rmdir_r (int *aErrno, const wchar_t *_path)
	{
	 return Backend()->rmdir(_path, *aErrno);
	}


/* A reentrant version of wrmdir().
*/
EXPORT_C int _wrmdir_r (int *aErrno, const wchar_t *_path)
	{
	return Backend()->rmdir(_path, *aErrno);
	}


/* A reentrant version of mkdir().
*/
EXPORT_C int _mkdir_r (int *aErrno, const wchar_t *_path, mode_t _mode)
	{
	return Backend()->mkdir(_path, _mode, *aErrno);
	}

/* A reentrant version of wmkdir().
*/
EXPORT_C int _wmkdir_r (int *aErrno, const wchar_t *_path, mode_t _mode)
	{
	return Backend()->mkdir(_path, _mode, *aErrno);
	}


/* A reentrant version of chmod().
*/
EXPORT_C int _chmod_r (int *aErrno, const wchar_t *_path, mode_t _mode)
	{
	return Backend()->chmod(_path, _mode, *aErrno);
	}

/* A reentrant version of chmod().
*/
EXPORT_C int _fchmod_r (int *aErrno, int _fd , mode_t _mode)
	{
	return Backend()->fchmod(_fd, _mode, *aErrno);
	}
/* A reentrant version of wchmod().
*/
EXPORT_C int _wchmod_r (int *aErrno, const wchar_t *_path, mode_t _mode)
	{
	return Backend()->chmod(_path, _mode, *aErrno);
	}

/* A wide-character version of reentrant of unlink().
*/
EXPORT_C int _wunlink_r (int *aErrno, const wchar_t *_path)
	{
	return Backend()->unlink(_path, *aErrno);
	}


/* A reentrant version of rename().
*/
EXPORT_C int _rename_r (int *aErrno, const wchar_t *oldpath, const wchar_t *newpath)
	{
	return Backend()->rename(oldpath, newpath, *aErrno);
	}


/* A wide-character version of reentrant of rename().
*/
EXPORT_C int _wrename_r (int *aErrno, const wchar_t *oldpath, const wchar_t *newpath)
	{
	return Backend()->rename(oldpath, newpath, *aErrno);
	}

/* A wide-character version of reentrant of realpath().
*/
EXPORT_C wchar_t * _wrealpath_r (int *aErrno, const wchar_t *relpath, wchar_t *resolved)
	{
		if(relpath == NULL || resolved == NULL)   //invalid arguments
			{
				*aErrno = EFAULT;
				return (NULL);
			}
	TPtr16 name((TText16*)resolved,MAXPATHLEN);
	TParse path;
	TInt err = Backend()->ResolvePath(path, relpath, &name);
	if (!err)
		{
		err = path.SetNoWild(path.DriveAndPath(),NULL,&name);
		if (!err)
			{
			name = path.FullName();
			name.ZeroTerminate();
			return resolved;
			}
		}
	MapError(err, *aErrno);
	return 0;
	}

LOCAL_C TInt _parseCmd (const wchar_t* command, wchar_t* exepath, TDes& buf)
	{
	TLex lexer((TText*)command);
	buf = lexer.NextToken();
	WcsCpy(exepath,(wchar_t*)buf.PtrZ());

	lexer.SkipSpace();
	if (!lexer.Eos())
		{
		buf = lexer.Remainder();
		buf.TrimRight();
		}
	return KErrNone;
	}

EXPORT_C int _wpopen_r (int* aErrno, const wchar_t* command, const char* mode)
	{
	wchar_t exepath[KMaxPath+1];
	TFileName buf;

	if (_parseCmd(command, exepath, buf) == KErrNotFound)
		{
		// no such file
		*aErrno = ENOENT;
		return -1;
		}
		
	return Backend()->popen(exepath, (wchar_t*)buf.PtrZ(), mode, *aErrno);
	}

EXPORT_C int _pclose_r(int* aErrno, int aFid)
	{
	return Backend()->pclose(aFid, *aErrno);
	}

/* A wide-character version of reentrant of popen3().
*/
EXPORT_C int _wpopen3_r (int* aErrno, const wchar_t* file, const wchar_t* cmd, 
						 wchar_t** envp, int fids[3])
	{
			
	wchar_t* cmdargs = NULL;
	if (cmd)
		{
		TFileName buf = (TText*)cmd;
		buf.Trim();
		cmdargs = (wchar_t*)buf.PtrZ();
		}

	return Backend()->popen3(file, cmdargs, envp, fids, *aErrno);
	}


/* A reentrant version of waitpid().
*/
EXPORT_C int _waitpid_r (int *aErrno, int pid, int* status, int options)
	{
	return Backend()->waitpid(pid, status, options, *aErrno);
	}


/* A reentrant version of wait().
*/
EXPORT_C int _wait_r (int *aErrno, int* status)
	{
	return _waitpid_r(aErrno, -1, status, 0);
	}


/* A wide-character version of reentrant of system().
*/
EXPORT_C int _wsystem_r (int *aErrno, const wchar_t* command)
	{
	wchar_t exepath[KMaxPath+1];
	TFileName buf;		
	
	if (NULL == command)
		{
		return 1;	// special case, says that we do support wsystem().
		}		
		
	if (_parseCmd(command, (wchar_t*)exepath, buf) == KErrNotFound)
		{
		// no such file
		*aErrno = ENOENT;
		return -1;
		}
	
	return Backend()->system(exepath, (wchar_t*)buf.PtrZ(), *aErrno);
	}


/* A reentrant version of select().
*/
EXPORT_C int _select_r(int *aErrno, int maxfd, fd_set *readfds, fd_set *writefds,
					   fd_set *exceptfds, struct timeval *tvptr)
	{
	return Backend()->select(maxfd, readfds, writefds, exceptfds, tvptr, *aErrno);
	}	


EXPORT_C int _fchdir_r(int *aErrno, int filedesc)
	{
	struct stat st;
	
	if(_fstat_r(aErrno, filedesc, &st)!=0)
		{
		*aErrno  = EBADF ;//Not an open file descriptor
		return -1;
		}
	else 
		{
		if (!(S_ISDIR(st.st_mode)))
			{
			*aErrno = ENOTDIR ; //Not a directory
			return -1;
			}
		}
		
	const wchar_t* dirName(NULL) ;
	dirName = Backend()->GetDirName (filedesc);

	if( dirName )
		{
		return Backend()->chdir(dirName, *aErrno);
		}
	else
		{
		//fildesc does not refer to a valid fd.
		*aErrno = ENOTDIR;
		return -1;
		}
	}


/*
Gets the path name of the current working directory.
If a buffer is specified, the path name is placed in that buffer,
and the address of the buffer is returned. 
@return If successful returns buf, if a non-null pointer was specified, 
or the address of the allocated memory otherwise.
@param anErrno Ref to the error no. 
@param _buf Points to the buffer to copy the current working directory to, 
or NULL if getcwd() should allocate the buffer.
@param _size Is the size, in bytes, of the array of characters that buf points to.
*/
/* A reentrant version of getcwd()*/
EXPORT_C wchar_t * _getcwd_r (int *aErrno, wchar_t *_buf, size_t _size)
	{
	return Backend()->getcwd(_buf, _size, *aErrno);
	}

// -----------------------------------------------------------------------------
// Function name: link
// Description: Provides link functionality.
// Returns:  0    : On success
//          -1    : On error
// In case of error, errno value set
// Remark: This is a simulated functionality and not supported by the platform     
// -----------------------------------------------------------------------------
//

EXPORT_C int _link_r(int *aErrno, const wchar_t *_oldwidename, const wchar_t *_newwidename)
	{
   	return Backend()->link(_oldwidename, _newwidename, *aErrno);					            
	}


// -----------------------------------------------------------------------------
// Function name: unlink
// Description: Provides unlink functionality.
// Returns:  0    : On success
//          -1    : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------
//

EXPORT_C int _unlink_r(int *aErrno, const wchar_t *_pathwidename)
	{
	return Backend()->unlink(_pathwidename, *aErrno);				            
	}

   
EXPORT_C int _posix_spawn_r(pid_t* pid, const wchar_t* wpath, 
							const posix_spawn_file_actions_t* file_actions,
							const posix_spawnattr_t* attrp,
							const wchar_t* wargs, 
							wchar_t** wenvp)
	{
	return Backend()->posix_spawn(pid, wpath, file_actions, attrp, wargs, wenvp);
	}
	
// -----------------------------------------------------------------------------
// Function name: exit
// Description: Provides exit functionality.
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------
//
EXPORT_C void _exit_r(int code)
	{
	return Backend()->Exit(code);
	}

// -----------------------------------------------------------------------------
// _fcntl_r() : A reentrant version of fcntl().
// This API is used for setting a file descriptor as non-blocking
// Returns: System wide error code
// -----------------------------------------------------------------------------
//
EXPORT_C int _fcntl_r (int *aErrno, int aFid, int aCmd, long anArg)
	{
	return Backend()->fcntl(aFid, aCmd, anArg, *aErrno);
	}
	
//
EXPORT_C DIR* _opendir_r (int* aErrno, const wchar_t* _path)
	{
	 __EPOC32_DIR* dp = new __EPOC32_DIR();
	if (!dp ||(Backend()->AddToDirList((DIR*)dp) != KErrNone))
		{
		*aErrno = ENOENT;
		return 0;
		}
	TInt err = dp->Open(_path,aErrno);
	if (err)
		{
		Backend()->RemoveDirFromList((DIR*)dp);
		delete dp;
		return 0;
		}
		
	return (DIR *)dp;
	}

//
EXPORT_C int _wclosedir_r(int* aErrno, WDIR* dp)
	{
	if(Backend()->RemoveDirFromList((DIR*)dp) == KErrNotFound)
		{
		*aErrno = EBADF;	
		}
	delete dp;
	return 0;
	}

// 

EXPORT_C struct dirent* _readdir_r (int* aErrno, DIR *dp)
    {
	__EPOC32_DIR *Dp  = (__EPOC32_DIR *)dp ;
	
	if(!dp || Backend()->FindInDirList((DIR*)dp) == KErrNotFound )
		{
		*aErrno = EBADF;
		return 0;	
		}
	if (_wreaddir_r(Dp))
		{
		if (Dp->UpdateNarrow()>=0)
			{
			return &Dp->iCurrentNarrow;	
			}
		else
			{
			*aErrno = EINVAL;
			}
		}
	return 0;
	}

EXPORT_C struct wdirent* _wreaddir_r(WDIR *dp)
	{
	struct wdirent *ep=&dp->iCurrent;
	if ((dp->iIndex < 0) ||  (dp->iIndex > dp->iCount ))
		return  (wdirent *) NULL;
	
	const TEntry& entry=(*dp->iEntries)[(dp->iIndex)++];
	// in practice, these files must have been created as "X:\something", so they
	// can't really be longer than KMaxFileName-3
	dp->iCurrentName.Copy(entry.iName);
	dp->iCurrentName.ZeroTerminate();
	ep->d_namlen=(short)dp->iCurrentName.Length();
	ep->d_name=(wchar_t*)dp->iCurrentName.PtrZ();
	return ep;
	}

EXPORT_C void _wrewinddir_r(WDIR *dp)
	{
	if (dp==0)
		return;
	(void) dp->Open();	// POSIX doesn't allow for rewind failing
	}
	
// -----------------------------------------------------------------------------
//int _setecho_r(int *aErrno, int aFd, size_t aEchoVal)
//
//Sets the echo flag for this fd.
// -----------------------------------------------------------------------------

EXPORT_C int _setecho_r(int *aErrno, int aFd, uint8_t aEchoVal)
	{
	return Backend()->SetEcho(aFd, aEchoVal, *aErrno);
	}
	
} //extern "C" end

// These functions should be out of extern "C"
/* A reentrant version of aselect().
*/
EXPORT_C int _aselect_r(int *aErrno, int maxfd, fd_set *readfds, fd_set *writefds, 
						fd_set *exceptfds, struct timeval *tvptr,
						TRequestStatus* requeststatus)
	{
	return Backend()->aselect(maxfd, readfds, writefds, exceptfds, tvptr,requeststatus, *aErrno);
	}	

/* A reentrant version of cancelaselect().
*/
EXPORT_C int _cancelaselect_r(int *aErrno, TRequestStatus* requeststatus)
	{
	return Backend()->cancelaselect(requeststatus, *aErrno);
	}	

/* A reentrant version of eselect().
*/
EXPORT_C int _eselect_r(int *aErrno, int maxfd, fd_set *readfds, fd_set *writefds,
						fd_set *exceptfds, struct timeval *tvptr, int numreqs,
						TRequestStatus* waitarray)
	{
	return Backend()->eselect(maxfd, readfds, writefds, exceptfds, tvptr,numreqs, waitarray,*aErrno);
	}

