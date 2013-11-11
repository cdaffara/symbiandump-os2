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
// Name        : fdatasync.cpp
// Part of     : LIBC
// Contains the source for fchdir
// Version     : 1.0
//


 
#include <unistd.h>

/*
The fdatasync() function shall force all currently queued I/O operations associated
with the file indicated by file descriptor fildes to the synchronized I/O completion
state.The functionality shall be equivalent to fsync() with the symbol  -> 
_POSIX_SYNCHRONIZED_IO defined, with the exception that all I/O operations shall be
completed as defined for synchronized I/O data integrity completion.If successful, 
the fdatasync() function shall return the value 0; otherwise, the function shall
return the value -1 and set errno to indicate the error.
*/

extern "C" {

EXPORT_C int fdatasync(int filedesc)
	{
	int retVal = -1;
	#ifndef _POSIX_SYNCHRONIZED_IO
	#define _POSIX_SYNCHRONIZED_IO
 	#define FLAG_FOR_FSYNC 1
 	#endif
	//Calls fsync that provides equivalent functionality.	
	retVal =  fsync(filedesc);
	
	#ifdef FLAG_FOR_FSYNC
	#undef _POSIX_SYNCHRONIZED_IO;
	#undef FLAG_FOR_FSYNC
	#endif 
	
	return retVal;
	}

} //extern "C"

