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
#include "reent.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sysif.h"
#include "sysreent.h"


/*
The fchdir() function shall be equivalent to chdir() except that
the directory that is to be the new current working directory is
specified by the file descriptor fildes.A conforming application
can obtain a file descriptor for a file of type directory using 
open(), provided that the file status flags and access modes do 
not contain O_WRONLY or O_RDWR.
    
Upon successful completion, fchdir() shall return 0. Otherwise,
it shall return -1 and set errno to indicate the error. 

Calls the reentrant version of the function.
*/	
extern "C" {
EXPORT_C int fchdir(int filedesc)
	{
	return _fchdir_r(&errno, filedesc);
	}

}

