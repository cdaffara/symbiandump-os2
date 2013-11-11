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
* Description:  Contains the source for fchdir
 *
*/


#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
/*
 * pathconf --
 * gets configurable system variables.
 * 'path' is the path of a file or directory
 * 'name' is the name of the configurable system variable 
 * queried relative to that file or directory.
 */
  

EXPORT_C long pathconf(const char *path, int name)
	{
	int fd = 0;
	int ret = -1;
	if( path )
	    {
    	ret = fpathconf(fd, name);
	    }
	else
	    {
	    errno = ENOENT;
	    }
	return ret;
	}



