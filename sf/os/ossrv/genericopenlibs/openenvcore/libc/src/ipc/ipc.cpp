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
* Description:  Implementation of IPC key generation (POSIX Standard).
*
*/


// INCLUDE FILES
#include <sys/ipc.h>
#include <sys/stat.h>
#include <liblogger.h>
#include <unistd.h>
#include <e32const.h>

// -----------------------------------------------------------------------------
// Funcation name: ftok
// Description: Convert a pathname and project identifier to an IPC key.
// Returns: None zero number: On success
//          -1              : On error.
// In case of error, errno value set to
//          EACCESS:      Access denied.
//          ENOENT:       Path name does not exist or path name is a empty string.
//          ENAMETOOLONG: Path name too long.
//          ELOOP:        Too many symbolic links were encountered in resolving path.
//          ENOTDIR:      Component of the path prefix is not a directory.
// -----------------------------------------------------------------------------
//

extern "C" {

TInt GeneratePathKey(const char* aPath)
    {
    if(aPath == NULL)
        {
        return -1;
        }
    TInt key = 0;
    TInt index = 0;
    while(*aPath != '\0')
        {
        index++;
        if(index > 16)
            {
            index = 0;
            }
        key += ((*aPath << index) + *aPath);
        aPath++;
        }
    return key;
    }


EXPORT_C key_t ftok(const char *pathname, int proj_id)
    {
    // 1. check the path status.
    // 2. if path doesn't exists then return error -1 and set the errno with appropriate value.
    // 3. else generate the IPC key.

	struct stat st;
    // 1.
	if (stat(pathname, &st) < 0)
	    {
	    // 2.
		return (key_t)-1;
	    }
    // 3. File exists.

    // Is it a special file.
    char splFileName[KMaxFileName+1];
    if (readlink(pathname, splFileName, KMaxFileName) > 0)
        {
    	return (key_t) (proj_id << 24 | GeneratePathKey(splFileName));
        }
	else
	    {
    	return (key_t) (proj_id << 24 | GeneratePathKey(pathname));
        }
    }
   
} // extern "C" {

//  End of File
