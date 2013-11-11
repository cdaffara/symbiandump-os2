// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// connectors for re-entrant system calls
// 
//

#include "SYSIF.H"
#include "LPOSIX.H"

#include <reent.h>
#include <unistd.h>
#include <fcntl.h>		// for open()
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <stdio_r.h>		// for popen3
#include <stdlib_r.h>		// for system
#include <utf.h>
#include <string.h>


extern "C" {

/**
Opens the file which name is stored in the file string.

@return	On Success, a non-negative integer representing the lowest numbered unused file descriptor.
		On Failure, returns -1, errno may be set.		
*/
EXPORT_C int open (const char *file, int flags, ...)
	{
	va_list ap;
	int ret;

	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).

	va_start (ap, flags);
	ret = _open_r (r, file, flags, va_arg (ap, int));
	va_end (ap);
	return ret;
	}

/**
A wide_character version of a open().
*/
EXPORT_C int wopen (const wchar_t *file, int flags, ...)
	{
	va_list ap;
	int ret;

	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).

	va_start (ap, flags);
	ret = _wopen_r (r, file, flags, va_arg (ap, int));
	va_end (ap);
	return ret;
	}

/** A reentrant version of open().
*/
EXPORT_C int _open_r (struct _reent *r, const char *name, int mode, int perms)
	{
	wchar_t _widename[KMaxFileName+1];

	if (-1 != mbstowcs(_widename, name, KMaxFileName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.open(_widename,mode,perms,r->_errno);
		}
	
	MapError(EILSEQ, r->_errno);		
	return 0;	//null file pointer

	}

/** A reentrant version of wopen().
*/
EXPORT_C int _wopen_r (struct _reent *r, const wchar_t *name, int mode, int perms)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.open(name,mode,perms,r->_errno);
	}

/**
Reads a block of data of the length specified by cnt.

@return On Success, return a non-negative integer indicating the number of bytes actually read.
		On Failure, returns -1, errno may be set.		
*/
EXPORT_C int read (int fd, char *buf, size_t cnt)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _read_r (r, fd, buf, cnt);
	}

/** A reentrant version of read().
*/
EXPORT_C int _read_r (struct _reent *r, int fd, char *buf, size_t nbyte)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.read(fd,buf,nbyte,r->_errno);
	}

/**
Writes a block of data of the length specified by cnt.

@return On Success, returns the number of bytes written to the file. The number
		shall never be greater than cnt.
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int write (int fd, const char *buf, size_t cnt)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _write_r (r, fd, buf, cnt);
	}

/** A reentrant version of write().
*/
EXPORT_C int _write_r (struct _reent *r, int fd, const char *buf, size_t nbyte)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.write(fd,buf,nbyte,r->_errno);
	}

/**
Close a file.

@return	On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int close (int fd)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _close_r (r, fd);
	}


/** A reentrant version of close().
*/
EXPORT_C int _close_r (struct _reent *r, int fd)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.close(fd,r->_errno);
	}

/**
Synchronizes a file's in-memory state with that on the physical medium.

@param fd Is the file descriptor for the file to be synchronized.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int fsync (int fd)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _fsync_r (r, fd);
	}

/** A reentrant version of fsync().
*/
EXPORT_C int _fsync_r (struct _reent *r, int fd)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.fsync(fd,r->_errno);
	}

/**
Repositions the read/write file offset.
@return a nonnegative integer that indicates the file pointer value. 
@param fd Is the file descriptor of an open file.
@param pos Specifies the number of bytes to offset the file pointer 
from a specified file origin.
@param whence Specifies the location from which to start seeking.
*/
EXPORT_C off_t lseek (int fd, off_t pos, int whence)
	{
	return _lseek_r (_REENT, fd, pos, whence);
	}

/** A reentrant version of fseek().
*/
EXPORT_C off_t _lseek_r (struct _reent *r, int fd, off_t pos, int whence)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.lseek(fd,pos,whence,r->_errno);
	}

/**
Gets information about the named file and writes it to the area that buf points to.
The system must be able to search all directories leading to the file; 
however, read, write, or execute permission of the file is not required.

@param fd Is a file descriptor referring to a file for which status is returned.
@param st Points to a stat structure where status information about the file is to be placed.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int fstat (int fd, struct stat *st)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _fstat_r (r, fd, st);
	}

/** A reentrant version of fstat().
*/
EXPORT_C int _fstat_r (struct _reent *r, int fd, struct stat *st) 
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.fstat(fd,st,r->_errno);
	}

/**
Gets the size of a file. 

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int stat (const char *name, struct stat *st)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _stat_r (r, name, st);
	}

/** A reentrant version of stat().
*/
EXPORT_C int _stat_r (struct _reent *r, const char *name, struct stat *st) 
	{
	wchar_t tmpbuf[KMaxFullName+1];	
	if (-1 != mbstowcs(tmpbuf, name, KMaxFullName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.stat(tmpbuf, st, r->_errno);
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** 
A wide_character version of a stat().
*/
EXPORT_C int wstat (const wchar_t *name, struct stat *st)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wstat_r (r, name, st);
	}

/** A reentrant version of wstat().
*/
EXPORT_C int _wstat_r (struct _reent *r, const wchar_t *name, struct stat *st) 
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.stat(name,st,r->_errno);
	}

/**
Duplicates an open file descriptor. 
		
@param aFid Is the file descriptor to duplicate.

@return On Success, returns a non-negative integer, namely the duplicated file descriptor, which
		is the lowest available descriptor. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int dup (int aFid)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _dup_r(r, aFid);
	}

/** A reentrant version of dup().
*/
EXPORT_C int _dup_r (struct _reent *r, int aFid)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.dup(aFid,r->_errno);
	}

/**
Function duplicates an open file descriptor. 

@param aFid1 Is the file descriptor to duplicate.
@param aFid2 Is the file descriptor that filedes is duplicated onto.

@return On Success, returns a non-negative integer, namely the duplicated file descriptor, which
		is the lowest available descriptor. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int dup2 (int aFid1, int aFid2)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _dup2_r(r, aFid1, aFid2);
	}

/** A reentrant version of dup2().
*/
EXPORT_C int _dup2_r (struct _reent *r, int aFid1, int aFid2)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.dup2(aFid1,aFid2,r->_errno);
	}

/**
Performs a variety of device-specific control functions on device special files.

@return On Success, returns a value other than -1 that depends upon the STREAMS device control function. 
        On Failure, return -1, errno may be set.
*/
EXPORT_C int ioctl (int aFid, int aCmd, void* aParam)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _ioctl_r(r, aFid, aCmd, aParam);
	}

/** A reentrant version of ioctl().
*/
EXPORT_C int _ioctl_r (struct _reent *r, int aFid, int aCmd, void* aParam)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.ioctl(aFid,aCmd,aParam,r->_errno);
	}

/**
Gets the path name of the current working directory.
If a buffer is specified, the path name is placed in that buffer,
and the address of the buffer is returned. 
@return If successful returns buf, if a non-null pointer was specified, 
or the address of the allocated memory otherwise. 
@param _buf Points to the buffer to copy the current working directory to, 
or NULL if getcwd() should allocate the buffer.
@param _size Is the size, in bytes, of the array of characters that buf points to.
*/
EXPORT_C char* getcwd (char *_buf, size_t _size)
	{
	return _getcwd_r(_REENT,_buf,_size);
	}

/** 
A wide_character version of a getcwd().
*/
EXPORT_C wchar_t* wgetcwd (wchar_t *_buf, size_t _size)
	{
	return _wgetcwd_r(_REENT,_buf,_size);
	}

/** A reentrant version of getcwd().
*/
EXPORT_C char* _getcwd_r (struct _reent *r, char *_buf, size_t _size)
	{
	char * _ourbuf = _buf;
	if (_buf==0)
		{
		_ourbuf=(char*)User::Alloc(_size);
		if (_ourbuf==0)
			{
			r->_errno=ENOMEM;
			return _buf;
			}
		}

	//we are dealing with wide characters from here so we need a temporary buffer
	wchar_t tmpbuf[KMaxFullName];

	MSystemInterface& sysIf=Interface(r);
	wchar_t * rval = sysIf.getcwd((wchar_t*)tmpbuf, _size, r->_errno);
	
	if (rval)	//we have a path
		{
		//convert it to UTF8
		size_t x = wcstombs(_ourbuf, tmpbuf, _size);	//convert the buffer
		return _ourbuf;
		}
	//deal with the fact we may have allocated our own buffer
	if (_buf != _ourbuf)  //we allocated it.
		User::Free(_ourbuf);
	return NULL;
	}

/** A wide-character version of reentrant of getcwd().
*/
EXPORT_C wchar_t * _wgetcwd_r (struct _reent *r, wchar_t *_buf, size_t _size)
	{
	if (_buf==0)
		{
		_buf=(wchar_t *)User::Alloc(_size*sizeof(wchar_t));
		if (_buf==0)
			{
			r->_errno=ENOMEM;
			return _buf;
			}
		}
	MSystemInterface& sysIf=Interface(r);
	return sysIf.getcwd(_buf,_size,r->_errno);
	}

/**
Changes the current working directory to be pathname. The current directory is the
beginning point for file searches when path names are not absolute. 
If the chdir() function fails, the current working directory remains unchanged.

@param _path Is the path name of a directory.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int chdir (const char *_path)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _chdir_r(r, _path);
	}

/** A reentrant version of chdir().
*/
EXPORT_C int _chdir_r (struct _reent *r, const char *_path)
	{
	//we need to use a wide buffer and convert
	wchar_t tmpbuf[KMaxFullName+1];		//use the max path length possible
	if (-1 != mbstowcs(tmpbuf, _path, KMaxFullName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.chdir(tmpbuf, r->_errno);
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** A wide-character version of chdir().
*/
EXPORT_C int wchdir (const wchar_t *_path)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wchdir_r(r, _path);
	}

/** A reentrant version of wchdir().
*/
EXPORT_C int _wchdir_r (struct _reent *r, const wchar_t *_path)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.chdir(_path,r->_errno);
	}

/**
Removes an empty directory whose name is given by pathname.
The directory must not have any entries other than dot (.) and dot-dot (..).

@param _path Points to the directory that the rmdir() function removes.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int rmdir (const char *_path)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _rmdir_r(r, _path);
	}

/** A reentrant version of rmdir().
*/
EXPORT_C int _rmdir_r (struct _reent *r, const char *_path)
	{
	wchar_t tmpbuf[KMaxFullName+1];		//use the max path length possible
	if (-1 != mbstowcs(tmpbuf, _path, KMaxFullName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.rmdir(tmpbuf, r->_errno);
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** A wide-character version of rmdir().
*/
EXPORT_C int wrmdir (const wchar_t *_path)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wrmdir_r(r,_path);
	}

/** A reentrant version of wrmdir().
*/
EXPORT_C int _wrmdir_r (struct _reent *r, const wchar_t *_path)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.rmdir(_path,r->_errno);
	}

/**
Creates a new directory with the specified path name. 
The file permissions of the new directory are initialized from the specified mode. 

@param _path Specifies the name of the new directory. The path name can be absolute or relative. 
	   If the specified path name is relative, the directory is created based upon your current
	   working directory.
@param _mode Is a bitwise-OR field that specifies what permissions the directory has when it is created.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set. Does not create a directory.
*/
EXPORT_C int mkdir (const char *_path, mode_t _mode)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _mkdir_r(r,_path,_mode);
	}

/** A reentrant version of mkdir().
*/
EXPORT_C int _mkdir_r (struct _reent *r, const char *_path, mode_t _mode)
	{
	//we need to use a wide buffer and convert
	wchar_t tmpbuf[KMaxFullName+1];		//use the max path length possible
	if (-1 != mbstowcs(tmpbuf, _path, KMaxFullName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.mkdir(tmpbuf, _mode, r->_errno);
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** A wide-character version of mkdir().
*/
EXPORT_C int wmkdir (const wchar_t *_path, mode_t _mode)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wmkdir_r(r, _path, _mode);
	}

/** A reentrant version of wmkdir().
*/
EXPORT_C int _wmkdir_r (struct _reent *r, const wchar_t *_path, mode_t _mode)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.mkdir(_path,_mode,r->_errno);
	}

/**
Sets the access permissions for the file whose name is given by pathname to the bit
pattern contained in mode. For this call to succeed, the effective user ID of the
process must match the owner of the file, or the process must have appropriate privileges. 
The owner of the file pathname always has privileges to change permission modes and file attributes.

@param _path Points to the name of the file.
@param _mode Is a bitwise-or field that specifies the new permission modes for path name.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int chmod (const char *_path, mode_t _mode)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _chmod_r(r, _path, _mode);
	}

/** A reentrant version of chmod().
*/
EXPORT_C int _chmod_r (struct _reent *r, const char *_path, mode_t _mode)
	{
	wchar_t tmpbuf[KMaxFullName+1];	
	if (-1 != mbstowcs(tmpbuf, _path, KMaxFullName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.chmod(tmpbuf,_mode,r->_errno);
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** A wide-character version of chmod().
*/
EXPORT_C int wchmod (const wchar_t *_path, mode_t _mode)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wchmod_r(r, _path, _mode);
	}

/** A reentrant version of wchmod().
*/
EXPORT_C int _wchmod_r (struct _reent *r, const wchar_t *_path, mode_t _mode)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.chmod(_path,_mode,r->_errno);
	}

/**
Removes a link to a file, and decrements the link count of the referenced file by one. When
the file's link count becomes 0 and no process has the file open, the space occupied by the
file is freed, and the file is no longer accessible. If one or more processes have the file
open when the last link is removed, the link is removed before unlink() returns, but the
removal of the file contents is postponed until all references to the file are closed.

@param _path Points to the path name that names the file to be unlinked.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set.
*/
EXPORT_C int unlink (const char *_path)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _unlink_r(r, _path);
	}

/** A reentrant version of unlink().
*/
EXPORT_C int _unlink_r (struct _reent *r, const char *_path)
	{
	wchar_t tmpbuf[KMaxFullName+1];		
	if (-1 != mbstowcs(tmpbuf, _path, KMaxFullName))
		{
		MSystemInterface& sysIf=Interface(r);
		return sysIf.unlink(tmpbuf, r->_errno);
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** A wide-character version of unlink().
*/
EXPORT_C int wunlink (const wchar_t *_path)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wunlink_r(r, _path);
	}

/** A wide-character version of reentrant of unlink().
*/
EXPORT_C int _wunlink_r (struct _reent *r, const wchar_t *_path)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.unlink(_path,r->_errno);
	}

/**
Renames a file. 

@param oldpath Points to the path name of the file to be renamed. The path name can be
	   absolute or relative. If a relative path name is given, the file is searched from
	   the current working directory.
@param newpath Points to the path name of the file. The path name can be absolute or relative. 
	   If a relative path name is given, the file is searched from the current working directory.

@return On Success, returns 0. 
		On Failure, returns -1, errno may be set. Does not change either the file named
		by old or the file named by new (if either exists).
*/
EXPORT_C int rename (const char *oldpath, const char *newpath)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _rename_r (r, oldpath, newpath);
	}

/** A reentrant version of rename().
*/
EXPORT_C int _rename_r (struct _reent *r, const char *oldpath, const char *newpath)
	{
	wchar_t _old[KMaxFullName+1];		
	wchar_t _new[KMaxFullName+1];		
	if (-1 != mbstowcs(_old, oldpath, KMaxFullName))
		{
		if (-1 != mbstowcs(_new, newpath, KMaxFullName))
			{
			MSystemInterface& sysIf=Interface(r);
			return sysIf.rename(_old, _new, r->_errno);
			}
		}
	MapError(EILSEQ, r->_errno);		
	return -1;
	}

/** A wide-character version of rename().
*/
EXPORT_C int wrename (const wchar_t *oldpath, const wchar_t *newpath)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wrename_r (r, oldpath, newpath);
	}

/** A wide-character version of reentrant of rename().
*/
EXPORT_C int _wrename_r (struct _reent *r, const wchar_t *oldpath, const wchar_t *newpath)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.rename(oldpath,newpath,r->_errno);
	}

/**
Takes a specified path name, pathname and resolves all symbolic links,
extra slashes (/), and references to /./ and /../. 
The resulting absolute path name is placed in the memory location 
pointed to by the resolved_path argument.
@return resolved_path. 
When an error occurs,returns a null pointer, setsresolved_path 
to the path name that caused the error.
@param path Points to the path name that you want resolved to an absolute form. 
This may be either a relative or absolute path name. 
All but the final component of this path name must exist when you call realpath().
@param resolved Points to the location where the canonical version
of pathname is to be placed. 
*/
EXPORT_C char* realpath (const char* path, char* resolved)
	{
	return _realpath_r(_REENT, path, resolved);
	}

/** A wide-character version of realpath().
*/
EXPORT_C wchar_t* wrealpath (const wchar_t* path, wchar_t* resolved)
	{
	return _wrealpath_r(_REENT, path, resolved);
	}

/** A wide-character version of reentrant of realpath().
*/
EXPORT_C wchar_t * _wrealpath_r (struct _reent *r, const wchar_t *relpath, wchar_t *resolved)
	{

	TPtr16 name((TText16*)resolved,MAXPATHLEN);
	TParse path;
	MSystemInterface& sysIf=Interface(r);
	TInt err = sysIf.ResolvePath(path, relpath, &name);
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
	MapError(err,r->_errno);
	return 0;
	}

/** A reentrant version of realpath().
*/
EXPORT_C char* _realpath_r (struct _reent *r, const char *relpath, char *resolved)
	{

	TFileName name;
	TInt err;
	
	TParse path;
	MSystemInterface& sysIf=Interface(r);

	wchar_t _wrelpath[KMaxFileName];

	if (-1 != mbstowcs(_wrelpath, relpath , KMaxFileName))
		{
		err = sysIf.ResolvePath(path, _wrelpath, &name);
		if (!err)
			{
			err = path.SetNoWild(path.DriveAndPath(),NULL,&name);
			if (!err)
				{
				name = path.FullName();

				if (-1 != wcstombs(resolved, (wchar_t*)name.PtrZ(), KMaxFileName))
					return resolved;
				else
					{
					err = EILSEQ;
					}
				}
			}
		}
		else
		{
		err = EILSEQ;
		}

	MapError(err,r->_errno);
	return 0;
	}

/**
Gives access to the client's stdin.

@return On Success, returns a pointer to an open stream, used to read or write to the pipe.
		On Failure, return a null pointer.
*/
EXPORT_C int popen3 (const char* cmd, const char* mode, char** env, int fids[3])
	{
    struct _reent *r = _REENT2;
	if (!r)
		return NULL; // Memory for library globals is not allocated (errno not set).
	return _popen3_r (r,cmd,mode,env,fids);
	}

/** A wide-character version of popen3().
*/
EXPORT_C int wpopen3 (const wchar_t* cmd, const wchar_t* mode, wchar_t** env, int fids[3])
	{
    struct _reent *r = _REENT2;
	if (!r)
		return NULL; // Memory for library globals is not allocated (errno not set).
	return _wpopen3_r (r,cmd,mode,env,fids);
	}

/** A reentrant version of a popen3().
*/
EXPORT_C int _popen3_r (struct _reent *r, const char* cmd, const char* mode, char** env, int fids[3])
	{

	wchar_t wcmd[MAXPATHLEN+1];
	wchar_t wmode[MAXPATHLEN+1];

	wchar_t ** wenv = NULL;
	wchar_t * buf = NULL;
	
	TInt ret = 0;

	if ((-1 != mbstowcs(wcmd, cmd, MAXPATHLEN)) && 
		(-1 != mbstowcs(wmode, mode, MAXPATHLEN)))
		{
		//OK, we've widened the first 2 args
		//now for the environment

		//env is basically an array of char pointers with a NULL as the last one
		if (env)
			{
			//OK we have a ptr to something
			//count the number of entries and get their lengths so we can work out how much space
			//is needed for the new one

			TInt count = 0;
			TInt total = 0;
			while (env[count] != NULL)
				{
				total+= strlen(env[count])+1;
				count++;
				}
			//total has number of bytes in the strings
			//max number of unicode chars is with a 1 to 1 mapping.
			wenv = (wchar_t**)malloc(1 + count*sizeof(wchar_t*));
			buf = (wchar_t*)malloc(2*total);
			
			if (!(wenv && buf))		//we've had a malloc failure
				{
				r->_errno = ENOMEM;
				goto bailout;
				}

			wchar_t* p = buf;

			TInt ret;
			for (TInt x = 0; x < count; x++)
				{
				wenv[count] = p;
				ret = mbstowcs(p, env[count], MAXPATHLEN);
				if (ret >= 0)
					{
						p += ret;	//step to next bit of space
					}
				else
					{
					r->_errno = EILSEQ;
					goto bailout;
					}

				}
			}


		ret =  _wpopen3_r(r, wcmd, wmode, wenv, fids);
		}
	else
		{
		r->_errno = EILSEQ;
		}

	//don't lose the memory
bailout:
	free(wenv);
	free(buf);

	return ret;
	}

/** A wide-character version of reentrant of popen3().
*/
EXPORT_C int _wpopen3_r (struct _reent *r, const wchar_t* cmd, const wchar_t* mode, wchar_t** env, int fids[3])
	{
	// Find the full path of the thing we are executing...
	const wchar_t* cp=cmd;
	while (*cp==L' ')
		++cp;	// skip leading spaces
	wchar_t file[MAXPATHLEN+1];
	TInt i=0;
	wchar_t c=0;
	for (i=0; i<MAXPATHLEN; i++, cp++)
		{
		c=*cp;
		file[i]=c;
		if (c==L' ' || c==L'\t' || c==L'\0')	// stop at first space, tab or \0  
			break;
		}
	file[i]=L'\0';
	wchar_t resolved[MAXPATHLEN+1];
	if(_wrealpath_r(r, file, resolved)==0)
		return -1;	// no such file

	// Strip leading whitespace from the rest of the commandline
	for (; i<MAXPATHLEN;i++,cp++)
		{
		c=*cp;
		if (c=='\0')
			break;
		if ((c!=' ') && (c!='\t'))
			break;
		}

	fids[0]=-2;
	fids[1]=-2;
	fids[2]=-2;
	const wchar_t* mp=mode;
	while (*mp)
		{
		wchar_t c=*mp++;
		if (c==L'r')
			fids[0]=-1;
		else if (c==L'w')
			fids[1]=-1;
		else if (c==L'e')
			fids[2]=-1;
		}

	MSystemInterface& sysIf=Interface(r);
	return sysIf.popen3(resolved,cp,mode,env,fids,r->_errno);
	}

/**
Lets the calling process obtain status information about one of its child processes.
If status information is available for two or more child processes, the order in
which their status is reported is unspecified.

@param pid Specifies a set of child processes for which the status is requested
@param status Specifies the location to which the child process' exit status is stored.
@param options Is the bitwise inclusive-OR of zero or more of the following flags.

@return On Success, returns a value equal to the process ID of the child process.
		On Failure, returns -1 and errno may be set OR returns 0 if the status is not available
		for the specified process and it's set not to hang in the options.
*/
EXPORT_C int waitpid (int pid, int* status, int options)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _waitpid_r (r, pid, status, options);
	}

/** A reentrant version of waitpid().
*/
EXPORT_C int _waitpid_r (struct _reent *r, int pid, int* status, int options)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.waitpid(pid,status,options,r->_errno);
	}

/**
Calls reentrant version of waitpid().
*/
EXPORT_C int wait (int* status)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _waitpid_r (r, -1, status, 0);
	}

/** A reentrant version of wait().
*/
EXPORT_C int _wait_r (struct _reent *r, int* status)
	{
	return _waitpid_r (r,-1,status,0);
	}

/**
Execute command.

@param cmd Null-terminated string containing the system command to be executed.

@return On Success, the command interpreter returns an adequate value; generally 0
        indicates that the action performed by the command interpreter terminated
        with no errors. 
        On Failure, return -1.
*/
EXPORT_C int system (const char* cmd)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _system_r (r, cmd);
	}

/** A reentrant version of system().
*/
EXPORT_C int _system_r (struct _reent *r, const char* cmd)
	{
	if (cmd==0)
		return 1;	// special case, says that we do support system().
	int fids[3];
	int pid=_popen3_r(r, cmd, "", 0, fids);
	if (pid<0)
		return -1;
	int status=0;
	pid=_waitpid_r (r,pid,&status,0);
	if (pid<0)
		return -1;
	return status;
	}

/** A wide-character version of a system().
*/
EXPORT_C int wsystem (const wchar_t* cmd)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wsystem_r (r, cmd);
	}

/** A wide-character version of reentrant of system().
*/
EXPORT_C int _wsystem_r (struct _reent *r, const wchar_t* cmd)
	{
	if (cmd==0)
		return 1;	// special case, says that we do support system().
	int fids[3];
	int pid=_wpopen3_r(r, cmd, (wchar_t*)L"", 0, fids);
	if (pid<0)
		return -1;
	int status=0;
	pid=_waitpid_r (r,pid,&status,0);
	if (pid<0)
		return -1;
	return status;
	}

} // extern "C"

#include <estlib.h>

/** Dubious asynchronous interface to ioctl, must be called from C++

@return On Success, returns a value other than -1.
		On Failure, returns -1 and errno may be set.
*/
EXPORT_C int ioctl (int aFid, int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _ioctl_r(r, aFid, aCmd, aParam, aStatus);
	}

/** A reentrant version of a ioctl().
*/
EXPORT_C int _ioctl_r (struct _reent *r, int aFid, int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.ioctl(aFid,aCmd,aParam,aStatus,r->_errno);
	}

EXPORT_C int ioctl_complete (int aFid, int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _ioctl_complete_r(r, aFid, aCmd, aParam, aStatus);
	}

/** A reentrant version of a ioctl_complete().
*/
EXPORT_C int _ioctl_complete_r (struct _reent *r, int aFid, int aCmd, void* aParam, TRequestStatus& aStatus)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.ioctl_complete(aFid,aCmd,aParam,aStatus,r->_errno);
	}

EXPORT_C int ioctl_cancel (int aFid)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _ioctl_cancel_r(r, aFid);
	}

/** A reentrant version of a ioctl_cancel().
*/
EXPORT_C int _ioctl_cancel_r (struct _reent *r, int aFid)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.ioctl_cancel(aFid,r->_errno);
	}

