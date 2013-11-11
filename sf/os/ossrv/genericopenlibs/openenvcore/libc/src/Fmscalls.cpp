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
* Description: connectors for re-entrant system calls
*/


// connectors for re-entrant system calls

#include "sysif.h"
#include "lposix.h"
#include <unistd.h>
#include <fcntl.h>		// for open()
#include <sys/ioctl.h>
#include <stdarg.h> 
#include <sys/errno.h>
#include <sys/stat.h>
#include <utf.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "sysreent.h"
#include <sys/aeselect.h>
#include "aeselectreent.h"
#include "fdesc.h"
#include "stdio_r.h"		// for popen3
#include "stdlib_r.h"		// for system

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif
#define	MAXPATHLEN	260	/* E32STD.H: KMaxFullName + 4 to avoid data loss */

extern "C" {

/*
Opens the file which name is stored in the file string.
*/
EXPORT_C int open (const char *file, int flags, ...)
	{
	va_list argList; 
	register int perms;
	
	if(!file) 
		{
    	errno = EFAULT ;
    	return -1 ;	//null file pointer
		}
	
	va_start (argList, flags);
	perms = va_arg(argList,int);
	va_end (argList);

	//If mode is invalid
	if( perms  > (S_IRWXU | S_IRWXG | S_IRWXO ) )
		{
		//make it read-write for all
		perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
		}
	wchar_t _widename[MAXPATHLEN+1];

	if ((size_t)-1 != mbstowcs(_widename, file, MAXPATHLEN))
		{
		return _open_r (&errno, _widename, flags, perms);  
		}
	else
		{
		errno = EILSEQ;		
		return -1;	// Illegal Sequence of wide characeters
		}

	}

/*
A wide_character version of a open().
*/
EXPORT_C int wopen (const wchar_t *file, int flags, ...)
	{
	va_list ap;
	int ret;
	register int perms;
	
	if(!file)
		{
    	errno = EFAULT ;
    	return -1 ;	
		}
		
	va_start (ap, flags);
	perms = va_arg(ap,int);
	va_end (ap);
	
	//If mode is invalid
	if( perms  > (S_IRWXU | S_IRWXG | S_IRWXO ) )
		{
		//make it read-write for all
		perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
		}

	ret = _wopen_r (&errno, file, flags, perms);
	
	return ret;
	}


/*
Reads a block of data of the length specified by cnt.
*/
EXPORT_C int read (int fd, void *buf, size_t cnt)
	{
	if((int)cnt == 0)       
		{
			return 0 ;
		}	
	if((int)cnt < 0) 
		{
	  	errno = EINVAL ;
	  	return -1 ;
		}
	if(!buf)
	  	{
	  	errno = EFAULT ;
	  	return -1 ;
	  	}
	
	//If the fd corresponding to STDIN
	if(fd == STDIN_FILENO && (Backend()->GetDesc(STDIN_FILENO))->Attributes() == KConsoleFd) 
		{
		int len = 0;
		char ch = '\0';
		int ret = -1;
		do
			{
			ret = _read_r(&errno, STDIN_FILENO, &ch, 1);

			//Copy requested count of data, ignore the rest
			if(ret > 0 && len < cnt )
				{
				((char*)buf)[len] = ch;
				len++;
				}
			else if(ret < 0)
				{
				break;
				}
			
			}while(ch != '\n');
		
		//Handle error case.
		if(ret == -1)
			{
			return ret;
			}

		return len;
		}
	else
		{
		return _read_r(&errno, fd, (char*)buf, cnt);
		}
	}


/*
Writes a block of data of the length specified by cnt.
*/
EXPORT_C int write (int fd, const void *buf, size_t cnt)
	{
		if((int)cnt == 0) 
		{
	   	return 0;
		}
	if((int)cnt < 0) 
		{
	   	errno = EINVAL;
	   	return -1;
		}
	if(!buf) 
	   	{
	   	errno = EFAULT ;
	   	return -1 ;
	   	}
	return _write_r(&errno, fd, (char*)buf, cnt);
	}


/*
Close a file.
*/
EXPORT_C int close (int fd)
	{
	return  _close_r(&errno, fd);
	}



/*
Synchronizes a file's in-memory state with that on the physical medium.
*/
EXPORT_C int fsync (int fd)
	{
	return _fsync_r(&errno, fd);
	}

/*
Repositions the read/write file offset.
*/
EXPORT_C off_t lseek (int fd, off_t pos, int whence)
	{
	return _lseek_r(&errno, fd, pos, whence);
	}


/*
Gets information about the named file and writes it to the area that buf points to.
The system must be able to search all directories leading to the file; 
however, read, write, or execute permission of the file is not required.
*/
EXPORT_C int fstat (int fd, struct stat *st)
	{
	if(!st)
	   {
	    errno = EFAULT ;
	    return -1 ;	
	   }
	return _fstat_r(&errno, fd, st);
	}


/*
Gets the size of a file. 
*/
EXPORT_C int stat (const char *name, struct stat *st)
	{
	if(!st)
	   {
	    errno = EFAULT ;
	    return -1 ;	
	   }
    
    wchar_t tmpbuf[MAXPATHLEN+1];	
	if ((size_t)-1 != mbstowcs(tmpbuf, name, MAXPATHLEN))
		{
		return _stat_r(&errno, tmpbuf, st);
		}
	errno = EILSEQ;		
	return -1;
	}


EXPORT_C int utime (const char *name, const struct utimbuf *filetimes)
	{
	if(!name) 
		{
		errno = EFAULT ;
		return -1 ;
		}

	wchar_t tmpbuf[MAXPATHLEN+1];	
	if ((size_t)-1 != mbstowcs(tmpbuf, name, MAXPATHLEN))
		{
		return _utime_r (&errno, tmpbuf, filetimes);
		}
	
	errno = EILSEQ;		
	return -1;
	}



/*
A wide_character version of a stat().
*/
EXPORT_C int wstat (const wchar_t *name, struct stat *st)
	{
			if( !name || !st )
			{
					errno = EFAULT;
					return -1;
			}
	return _wstat_r (&errno, name, st);
	}


/*
duplicates an open file descriptor.
*/
EXPORT_C int dup (int aFid)
	{
	return _dup_r(&errno, aFid);
	}


/*
function duplicates an open file descriptor.
*/
EXPORT_C int dup2 (int aFid1, int aFid2)
	{
	return _dup2_r(&errno, aFid1, aFid2);
	}


/*
sorms a variety of device-specific control functions on device special files.
*/
EXPORT_C int ioctl (int aFid, unsigned long aCmd, ...)
	{
	void* aParam ;
	va_list  Vlist ;
	int ret;
	va_start(Vlist , aCmd ) ;
	aParam = va_arg(Vlist , void *)  ;
	if(!aParam) 
		{
		errno = EFAULT ; 
		return -1 ;
		}

	ret = _ioctl_r(&errno, aFid, aCmd, aParam);
	
	va_end(Vlist) ;
	return ret;
	}



/*
Gets the path name of the current working directory.
If a buffer is specified, the path name is placed in that buffer,
and the address of the buffer is returned.
*/

/*
A wide_character version of a getcwd().
*/
EXPORT_C wchar_t* wgetcwd (wchar_t *_buf, size_t _size)
	{
	return _wgetcwd_r(&errno, _buf, _size);
	}


/*
Changes the current working directory to be pathname. 
The current directory is the beginning point for file 
searches when path names are not absolute. 
If the chdir() function fails, the current working directory remains unchanged.
*/
EXPORT_C int chdir (const char *_path)
	{
	if(!_path) 
		{
		errno = EFAULT;
		return -1;
	  	}

	//we need to use a wide buffer and convert
	wchar_t tmpbuf[MAXPATHLEN+1];		//use the max path length possible
	if ((size_t)-1 != mbstowcs(tmpbuf, _path, MAXPATHLEN))
		{
		return _chdir_r(&errno, tmpbuf);
		}
	errno = EILSEQ;		
	return -1;
	}


/* A wide-character version of chdir().
*/
EXPORT_C int wchdir (const wchar_t *_path)
	{

	if(!_path) 
		{
		errno = EFAULT;
		return -1;
	  	}

	return _wchdir_r(&errno, _path);
	}


/*
Removes an empty directory whose name is given by pathname.
The directory must not have any entries other than dot (.) and dot-dot (..).
*/
EXPORT_C int rmdir (const char *_path)
	{
	if(!_path)
		{
		errno = EFAULT ;
		return -1 ;
		}

	if(!strcmp((_path + strlen(_path) -1 ) , ".") ) 
		{
		errno = EINVAL ;
		return -1 ;  
		}

	wchar_t tmpbuf[MAXPATHLEN+1];		//use the max path length possible
	if ((size_t)-1 != mbstowcs(tmpbuf, _path, MAXPATHLEN))
		{
		return _rmdir_r(&errno, tmpbuf);
		}
	errno = EILSEQ;		
	return -1;
	}



/* A wide-character version of rmdir().
*/
EXPORT_C int wrmdir (const wchar_t *_path)
	{
			if( !_path )
			{
				 errno = EFAULT;
				 return -1;
			}
	return _wrmdir_r(&errno, _path);
	}


/*
Creates a new directory with the specified path name. 
The file permissions of the new directory are initialized from the specified mode. 
*/
EXPORT_C int mkdir (const char *_path, mode_t _mode)
	{
	if(!_path)
		{
		errno = EFAULT ;
		return -1 ;
		}

	//we need to use a wide buffer and convert
	wchar_t tmpbuf[MAXPATHLEN+1];		//use the max path length possible
	if ((size_t)-1 != mbstowcs(tmpbuf, _path, MAXPATHLEN))
		{
		return _mkdir_r(&errno, tmpbuf, _mode);
		}
	errno = EILSEQ;		
	return -1;
	}



/* A wide-character version of mkdir().
*/
EXPORT_C int wmkdir (const wchar_t *_path, mode_t _mode)
	{

	if(!_path)
		{
		errno = EFAULT ;
		return -1 ;
		}
	
	return _wmkdir_r(&errno, _path, _mode);
	}


/*
Sets the access permissions for the file 
whose name is given by pathname to the bit pattern contained in mode. 
For this call to succeed, the effective user ID of the process must match 
the owner of the file, or the process must have appropriate privileges. 
The owner of the file pathname always has privileges to change permission modes 
and file attributes.
*/
EXPORT_C int chmod (const char *_path, mode_t _mode)
	{
	if(!_path)
		{
		errno = EFAULT ;
		return -1 ;
		}

	wchar_t tmpbuf[MAXPATHLEN+1];	
	if ((size_t)-1 != mbstowcs(tmpbuf, _path, MAXPATHLEN))
		{
		return _chmod_r(&errno, tmpbuf, _mode);
		}
	errno = EILSEQ;		
	return -1;
	}
/*
Sets the access permissions for the file specifed by file descriptor
whose name is given by pathname to the bit pattern contained in mode. 
For this call to succeed, the effective user ID of the process must match 
the owner of the file, or the process must have appropriate privileges. 
The owner of the file pathname always has privileges to change permission modes 
and file attributes.
*/
EXPORT_C int fchmod (int fd , mode_t _mode)
	{
	
		return _fchmod_r(&errno, fd, _mode);
	}



/* A wide-character version of chmod().
*/
EXPORT_C int wchmod (const wchar_t *_path, mode_t _mode)
	{
	
	if(!_path)
		{
		errno = EFAULT ;
		return -1 ;
		}
	
	return _wchmod_r(&errno, _path, _mode);
	}



/* A wide-character version of unlink().
*/
EXPORT_C int wunlink (const wchar_t *_path)
	{
			if( !_path )
			{
					errno = EFAULT;
					return -1;
			}
	return _wunlink_r(&errno, _path);
	}


/*
Renames a file.
*/
EXPORT_C int rename (const char *oldpath, const char *newpath)
	{
	if((!oldpath) ||(!newpath))
		{
		errno = EFAULT ;
		return -1 ;
		}

	wchar_t _old[MAXPATHLEN+1];		
	wchar_t _new[MAXPATHLEN+1];		
	if ((size_t)-1 != mbstowcs(_old, oldpath, MAXPATHLEN))
		{
		if ((size_t)-1 != mbstowcs(_new, newpath, MAXPATHLEN))
			{
			return _rename_r(&errno, _old, _new);
			}
		}
	errno = EILSEQ;		
	return -1;
	}


/* A wide-character version of rename().
*/
EXPORT_C int wrename (const wchar_t *oldpath, const wchar_t *newpath)
	{
	
	if((!oldpath) ||(!newpath))
		{
		errno = EFAULT ;
		return -1 ;
		}
	
	return _wrename_r(&errno, oldpath, newpath);
	}


/*
Takes a specified path name, pathname and resolves all symbolic links,
extra slashes (/), and references to /./ and /../. 
The resulting absolute path name is placed in the memory location 
pointed to by the resolved_path argument.
*/

/* A wide-character version of realpath().
*/
EXPORT_C wchar_t* wrealpath (const wchar_t* path, wchar_t* resolved)
	{
	return _wrealpath_r(&errno, path, resolved);
	}


/*
Gives access to the client's stdin
*/
EXPORT_C FILE* popen (const char* command, const char* mode)
	{
	// Check for the validity of command.
	// On Linux, the shell would return cannot find command to execute
	if (command == NULL)
		{
		errno = ENOENT;
		return NULL;
		}
		
	if(strlen(command) > KMaxPath)
		{
		errno = ENAMETOOLONG;
		return NULL;
		}
	// Check for the validity of Mode.
	if( (!mode) || mode[0] != 'r' && mode[0] != 'w' )
		{
		// Invalid mode
		errno = EINVAL;
		return NULL;
		}
		
	wchar_t wcmd[KMaxPath+1];
		
	// Widen command
	if ((size_t)-1 != mbstowcs(wcmd, command, KMaxPath))
		{
		int fd = _wpopen_r(&errno, wcmd, mode);
		if (fd > 0)
			{
			// return value is a valid fd
			return fdopen(fd, mode);
			}
		}
	else
		{
		errno = EILSEQ;
		}

	return NULL;
	}
	
	
EXPORT_C FILE* wpopen (const wchar_t* command, const wchar_t* wmode)
	{
	char mode[2];
	size_t sz;

	mode[0] = '\0';
	sz = wcstombs(mode, wmode, 2);
	//Check for the validity of Mode.
	if (sz <= 0 || (mode[0] != 'r' && mode[0] != 'w'))
		{
		//If its neither "r" nor "w", its undefined behavior
		errno = EINVAL;
		return NULL;
		}

	if(command)
		{
		if(wcslen(command) > KMaxPath)
			{
			errno = ENAMETOOLONG;
			return NULL;
			}
			
		int fd = _wpopen_r(&errno, command, mode );
		//If return Value is valid fd
		if (fd > 0)
			{
			return fdopen(fd, mode);
			}
		}
	else
		{
		errno = ENOENT;
		}
	return NULL;
	}


EXPORT_C int pclose(FILE* stream)
	{
	TInt fd;
	if (!stream || ((fd = fileno(stream)) == -1))
		{
		errno = EINVAL;
		return -1;
		}
		
	TInt err = _pclose_r(&errno, fd);
	
	if (err != 0)
		{
		if (errno == ECHILD)
			{
			fclose(stream);
			// reset errno just in case it has been modified by fclose
			errno = ECHILD;
			}
		
		return -1;
		}
		
	return fclose(stream);
	}
	
/* A wide-character version of popen3().
*/
EXPORT_C int wpopen3 (const wchar_t* file, const wchar_t* cmd, wchar_t** env, int fids[3])
	{
	if (file == NULL)
		{
		errno = ENOENT;
		return -1;
		}
		
	if(wcslen(file) > KMaxPath)
		{
		errno = ENAMETOOLONG;
		return -1;	
		}
		
	return _wpopen3_r(&errno, file, cmd, env, fids);
	}


EXPORT_C int popen3 (const char* file, const char* cmd, char** env, int fids[3])
	{
	if (file == NULL)
		{
		errno = ENOENT;
		return -1;
		}
		
	if(strlen(file) > KMaxPath)
		{
		errno = ENAMETOOLONG;
		return -1;	
		}
		
	wchar_t wfile[KMaxPath+1];
	wchar_t wcmd[KMaxPath+1];
	
	wchar_t** wenv = NULL;
	
	TInt ret = mbstowcs(wfile, file, KMaxPath);
	TInt cmdlen = mbstowcs(wcmd, cmd, KMaxPath);
	
	if (ret != (size_t)-1 && cmdlen != (size_t)-1)
		{
		//OK, we've widened the first 2 args
		//now for the environment

		//env will be an array of char pointers with a NULL as the last one
		if (env)
			{
			TInt count = 0;
			
			for (; env[count]; ++count) { }

			//coverity[alloc_fn]
			//coverity[assign]
			
			wenv = (wchar_t **)malloc((count+1) * sizeof(wchar_t*));
			if (!wenv)
				{
				errno = ENOMEM;
				return -1;
				}
			
			for (int i = 0; i < count; ++i)
				{
				int len = strlen(env[i]) + 1;
				wenv[i] = (wchar_t *)malloc(len * sizeof(wchar_t));
				if (wenv[i] == NULL)
					{
					//coverity[leave_without_push]

					errno = ENOMEM;
					goto bailout;
					}
				if (mbstowcs(wenv[i], env[i], len) == (size_t)-1)
					{
					//coverity[leave_without_push]
					errno = EILSEQ;
					wenv[i+1] = NULL;
					goto bailout;
					}
				}
				
			wenv[count] = 0;
			}

		if (cmdlen)
			{
			//coverity[leave_without_push]
			ret =  wpopen3(wfile, wcmd, wenv, fids);
			}
		else
			{
			//coverity[leave_without_push]
			ret = wpopen3(wfile, NULL, wenv, fids);
			}
			
		}
	else
		{
		errno = EILSEQ;
		return -1;
		}

bailout:
	if (wenv)
		{
		for (int i = 0; wenv[i]; ++i)
			{
			free(wenv[i]);
			}
		free(wenv);
		}

	return ret;
	}

/*
Lets the calling process obtain status information about one of its child processes.
If status information is available for two or more child processes,
the order in which their status is reported is unspecified.
*/
EXPORT_C int waitpid (int pid, int* status, int options)
	{
	return _waitpid_r(&errno, pid, status, options);
	}


/*
Calls reentrant version of waitpid().
*/
EXPORT_C int wait (int* status)
	{
	return _wait_r(&errno, status);
	}



/*
Execute command.
*/

/* A wide-character version of a system().
*/
EXPORT_C int wsystem (const wchar_t* cmd)
	{
	if (cmd==0)
		{
		return 1;	// special case, says that we do support system().	
		}

	if(wcslen(cmd) > KMaxPath)
		{
		errno = ENAMETOOLONG;
		return -1;	
		}
		
	return _wsystem_r(&errno, cmd);
	}


// -----------------------------------------------------------------------------
// Select() : Implementation of Select for I/O multiplexing
// This API is used for waiting on multiple descriptors to become ready
// Maximum timeout to wait can also be specified
// Returns: System wide error code
// -----------------------------------------------------------------------------
//
EXPORT_C int select(int maxfd, fd_set *readfds, fd_set *writefds,
					fd_set *exceptfds, struct timeval *tvptr)
	{	
	return _select_r(&errno, maxfd, readfds, writefds, exceptfds, tvptr);
	}
} // extern "C"

// -----------------------------------------------------------------------------
// aselect() : Implementation of Select for asynchronous I/O multiplexing
// This API is used for waiting on multiple descriptors to become ready
// Maximum timeout to wait can also be specified
// Returns: System wide error code
// This api does not provide C Linkage
// -----------------------------------------------------------------------------
//
EXPORT_C int aselect(int maxfd, fd_set *readfds, fd_set *writefds,
					fd_set *exceptfds, struct timeval *tvptr,
					TRequestStatus* requeststatus)
	{	
	return _aselect_r(&errno, maxfd, readfds, writefds, exceptfds, tvptr, requeststatus);
	}

// -----------------------------------------------------------------------------
// cancelaselect() : Implementation of Select for asynchronous I/O multiplexing
// This API is used for cancelling the aselect issued on requeststatus
// Returns: System wide error code
// This api does not provide C Linkage
// -----------------------------------------------------------------------------
//
EXPORT_C int cancelaselect(TRequestStatus* requeststatus)
	{	
	return _cancelaselect_r(&errno, requeststatus);
	}

// -----------------------------------------------------------------------------
// eselect() : Implementation of Select for I/O multiplexing
// This API is used for waiting on multiple descriptors to become ready,
// or for any of the TRequestStatus object in the TRequestStatus array passed
// to be signalled
// Maximum timeout to wait can also be specified
// Returns: System wide error code
// This api does not provide C Linkage
// -----------------------------------------------------------------------------
//
EXPORT_C int eselect(int maxfd, fd_set *readfds, fd_set *writefds,
					fd_set *exceptfds, struct timeval *tvptr, int numreqs,
					TRequestStatus* waitarray)
	{	
	return _eselect_r(&errno, maxfd, readfds, writefds, exceptfds, tvptr, numreqs, waitarray);
	}

// -----------------------------------------------------------------------------
// fcntl() : Implementation of fcntl for supporting Non-Blocking I/O 
// This API is used for setting a file descriptor as non-blocking
// Returns: System wide error code
// -----------------------------------------------------------------------------
//
EXPORT_C int fcntl (int aFid, int aCmd, ...)
	{
	va_list ap;
	va_start(ap, aCmd);

	int ret;
	ret = _fcntl_r(&errno, aFid,aCmd, va_arg(ap, long)); 
	
 	va_end(ap);
	return ret ;
	}

// -----------------------------------------------------------------------------
//int setecho(int fd, unsigned int echoval)
//
//Sets the echo flag for this fd.
// -----------------------------------------------------------------------------

extern "C" {
EXPORT_C int setecho(int fd, uint8_t echoval)
	{
	return _setecho_r(&errno, fd, echoval);
	}
} //extern "C"



