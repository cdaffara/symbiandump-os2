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
* Description:  Implementation of link/symlink.
*
*/


// INCLUDE FILES

#include <sys/errno.h>
#include <sys/types.h>
#include <wchar.h>
#include <string.h>
#include <unistd.h>
#include "lposix.h"
#include <sys/stat.h>
#include <sys/limits.h>
#include "sysreent.h"
#include <stdlib.h>
#include "sysif.h"
#include "systemspecialfilercg.h"
#include "link.h"

// -----------------------------------------------------------------------------
// Funcation name: link
// Description: Provides link functionality.
// Returns:  0    : On success
//          -1    : On error
// In case of error, errno value set
// Remark: This is a simulated functionality and not supported by the platform     
// -----------------------------------------------------------------------------
//

extern "C" {

#define	MAXPATHLEN	260	/* E32STD.H: KMaxFullName + 4 to avoid data loss */

EXPORT_C int link(const char *oldpath, const char *newpath)
	{
    wchar_t _oldwidename[MAXPATHLEN+1];
    wchar_t _newwidename[MAXPATHLEN+1];

	if ((size_t)-1 != mbstowcs(_oldwidename, oldpath, MAXPATHLEN) && \
	    (size_t)-1 != mbstowcs(_newwidename, newpath, MAXPATHLEN)) 
		{
	    return _link_r(&errno, _oldwidename, _newwidename);		
		}
	errno = EILSEQ;
	return -1;
	}

// -----------------------------------------------------------------------------
// Funcation name: symlink
// Description: Provides symlink functionality which is exactly similar to link
// in our case.
// Returns: ssize_t : On success
//          -1      : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------
//
    
EXPORT_C int symlink(const char *oldpath, const char *newpath)
	{
    wchar_t _oldwidename[MAXPATHLEN+1];
    wchar_t _newwidename[MAXPATHLEN+1];

	if ((size_t)-1 != mbstowcs(_oldwidename, oldpath, MAXPATHLEN) && \
	    (size_t)-1 != mbstowcs(_newwidename, newpath, MAXPATHLEN)) 
		{
	    return _link_r(&errno, _oldwidename, _newwidename);		
		}            
	errno = EILSEQ;
	return -1;	
	}

// -----------------------------------------------------------------------------
// Funcation name: readlink
// Description: Reads the link file.
// Returns: ssize_t : On success
//          -1      : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------
//

EXPORT_C ssize_t readlink(const char *path, char *buf, int bufsize)
{
	TSpecialFileType fileType;
	struct SLinkInfo enBuf;
	int err = KErrNone;
	char name[MAXPATHLEN+1];
	
	wchar_t widename[MAXPATHLEN+1];

	if(path)
		{	
		if ((size_t)-1 != mbstowcs(widename, path, MAXPATHLEN)) 
			{	
			/*Special file identification and read of IPC is utilized here*/
			if( (fileType = _SystemSpecialFileBasedFilePath(widename, err, Backend()->FileSession())) == EFileTypeSymLink)
				{
				if (err)
				    {
			    	return MapError(err, errno);
					}
				err = _ReadSysSplFile(widename, (char*)&enBuf, sizeof(struct SLinkInfo), errno, Backend()->FileSession());
				if (err == KErrNone)
					{
					if((0 < bufsize && bufsize <= SSIZE_MAX) && NULL != buf )	
						{
						if ((size_t)-1 != wcstombs(name, enBuf.iParentPath, sizeof(enBuf.iParentPath)))
							{
							strncpy(buf, name, bufsize);
							return (strlen(name) >= bufsize ? bufsize : strlen(buf));
							}
						}
					else
						{
						errno = EINVAL;
						return -1;	
						}
					}
				}
			else if(err || (EFileNotFound == fileType))
				{
				return MapError(err, errno);
				}				
			errno = EINVAL;
			return -1;		
			}
		return MapError(EILSEQ, errno);	
		}
	return MapError(EINVAL, errno);	
	}

// -----------------------------------------------------------------------------
// Funcation name: unlink
// Description: Provides unlink functionality.
// Returns:  0    : On success
//          -1    : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------
//

EXPORT_C int unlink(const char *pathname)
{
	wchar_t _pathwidename[MAXPATHLEN+1];    

	if ((size_t)-1 != mbstowcs(_pathwidename, pathname, MAXPATHLEN)) 
	{
	    return _unlink_r(&errno, _pathwidename);		
	}            
	errno  = EILSEQ;
	return -1;	      
}

// -----------------------------------------------------------------------------
// Funcation name: lstat, __xstat, __lxstat
// Description: All these functions directly call stat by themself .
// Returns:  0      : On success
//          -1      : On error
// In case of error, errno value set
//      
// -----------------------------------------------------------------------------
//

EXPORT_C int lstat(const char *file_name, struct stat *buf)
	{
    if(!buf)
       {
        errno = EFAULT ;
        return -1 ; 
       }
    
    wchar_t tmpbuf[MAXPATHLEN+1];   
    if ((size_t)-1 != mbstowcs(tmpbuf, file_name, MAXPATHLEN))
        {
        return _lstat_r(&errno, tmpbuf, buf);
        }
    errno = EILSEQ;     
    return -1;    
	}
EXPORT_C int __xstat(int /*vers*/, const char *file, struct stat *buf)
	{
	return stat(file, buf);
	}

EXPORT_C int __lxstat (int /*version*/, const char *file, struct stat *buf)
	{
	return lstat(file, buf);
	}

    
} // extern "C"


//  End of File
