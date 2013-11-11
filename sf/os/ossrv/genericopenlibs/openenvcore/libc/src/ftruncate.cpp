// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ftruncate.cpp
// Part of     : LIBC
// Contains the source for fchdir
// Version     : 1.0
//


 

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h> 
#include <fcntl.h> 
#include "reent.h"
#include "sysif.h"

extern "C" {

/*
The reentrant version for ftruncate function.
*/
int _truncate_r(struct _reent *r, int fd, off_t len)
	{
	struct stat st;
	
	if(fstat(fd,&st) != 0)
		{
		r->_errno  = EINVAL; 	//KErrArgument
		return -1;
		}
	else 
		{
		if (!(S_ISREG(st.st_mode)))
			{
			r->_errno = EBADF;	//KErrBadHandle
			return -1;
			}
		}
	
  	return Backend()->Truncate( fd, len, r->_errno);
	}

/*
Truncates the given file (using file descriptor) to the length specified.
Calls the reentrant version.
The ftruncate() function shall cause the size of the file to be truncated
to length arument. If the size of the file previously exceeded length, the
extra data shall no longer be available to reads on the file. If the file
previously was smaller than this size, ftruncate() shall either increase 
the size of the file or fail. Upon successful completion, ftruncate() shall
return 0; otherwise, -1 shall be returned and errno set to indicate the error.
*/
EXPORT_C int ftruncate(int filedesc, off_t length)
	{
	return _truncate_r(_REENT, filedesc, length);
	}

/*
Truncates the given file (using file name) to the length specified.
Calls the reentrant version.
The truncate() function shall cause the size of the file to be truncated
to length arument. If the size of the file previously exceeded length, the
extra data shall no longer be available to reads on the file. If the file
previously was smaller than this size, truncate() shall either increase 
the size of the file or fail. Upon successful completion, truncate() shall
return 0; otherwise, -1 shall be returned and errno set to indicate the error.
*/
EXPORT_C int truncate(const char *file , off_t length) 
	{
	int fd = 0;
	int retval = 0;
	int errVal = 0;
	 
	  if((fd = open(file , O_WRONLY)) < 0) 
	  	{
	  	 return -1;
	  	}
	  retval =  ftruncate(fd , length);
	  if(retval < 0) 
	  	{
	  	errVal = errno; //Store the error value 
	  	close(fd) ;
	  	errno = errVal;
	  	return retval;
	  	}
	  	
	 close(fd);
	 return retval;
	}

} //extern "C"
