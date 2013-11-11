// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 
//


#include <sys/errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include "sysreent.h"

// -----------------------------------------------------------------------------
// Creat  :  Function that stub which 
 
// This function takes the system call number and that transforms
// controll to the respective routines that performs the task 
// requested .                                          
// Returns: -1: if the requested task fails 
//           0: if the requested task successfull
//  0x7FFFFFFF: if invalid system call.                   
// -----------------------------------------------------------------------------
//

extern "C" {

#define	MAXPATHLEN	260	/* E32STD.H: KMaxFullName + 4 to avoid data loss */
	
EXPORT_C int creat(const char* file, mode_t mode)
	{
	wchar_t _widename[MAXPATHLEN+1];
	
	if(!file)
		{
		errno = EFAULT ;
		return -1 ;//null file pointer
		}

	if ((size_t)-1 != mbstowcs(_widename, file, MAXPATHLEN))
		{
		return _open_r(&errno , _widename , O_CREAT | O_WRONLY | O_TRUNC , mode);  
		}
	else
		{
		errno = EILSEQ;
		return -1;	// Illegal Sequence of wide characters
		}
	}


EXPORT_C int wcreat(const wchar_t* file, mode_t mode)
	{
	if (file)
		{
		return _wopen_r(&errno , file , O_CREAT | O_WRONLY | O_TRUNC , mode);  
		}
	else
		{
		errno = EFAULT;
		return -1;	//null file pointer
		}
	}
}
