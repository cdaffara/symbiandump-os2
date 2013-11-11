/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of named pipe (POSIX Standard).
*
*/


// INCLUDE FILES
#include <sys/errno.h>
#include "sysreent.h"
#include <string.h>
#include <stdlib.h>
#include "reent.h"
#include "lposix.h"
#include "sysif.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Funcation name: mkfifo
// Description: Provides a FIFO special file (a named pipe).
// Returns: 0     : On success
//          -1    : On error
// In case of error, errno value set to
//          EACCES: One  of  the  directories in pathname did not allow search
//                  (exe-cute) permission.
//          EEXIST: pathname already exists.
//          ENAMETOOLONG: Either the total length of pathname is greater
//                        than PATH_MAX, or an individual  file  name  component
//                        has a length greater than NAME_MAX.  In the GNU system,
//                        there is no imposed limit on over-all  file name length,
//                        but some file systems may place limits on the length of a component.
//          ENOENT: A directory component in pathname does not exist or  is  a  dan-gling
//                  symbolic link.
//          ENOSPC: The directory or filesystem has no room for the new file.
//          ENOTDIR: A  component  used as a directory in pathname is not, in fact, a
//                   directory.
//          EROFS:  pathname refers to a read-only filesystem.
// To be removed
//          EFAULT: Pathname is not valid.
//          EMFILE: Too many file descriptors are in use by the process.
//          ENFILE: System limit on the total number of open files has been
//                  reached.
// -----------------------------------------------------------------------------
//

extern "C" {

#define	MAXPATHLEN	260	/* E32STD.H: KMaxFullName + 4 to avoid data loss */
EXPORT_C int mkfifo(const char *pathname, mode_t mode)
    {
    if(!pathname || !strlen(pathname))
        {
        errno = ENOENT;
        return -1;
        }
    if(strlen(pathname) > KMaxFileName)
        {
        errno = ENAMETOOLONG;
        return -1;
        }
	wchar_t _widename[MAXPATHLEN+1];

	if ((size_t)-1 != mbstowcs(_widename, pathname, MAXPATHLEN)) 
		{
	    return _mkfifo_r(&errno, _widename, mode);
		}
	
	errno = EILSEQ;
	return -1;
    }

} // extern "C"
