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
// Name        : nice.cpp
// Part of     : LIBC
// Contains the source for fchdir
// Version     : 1.0
//



#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <e32std.h>
#include <e32const.h>
#include <unistd.h>

extern "C" {

/*
The function nice() increases or decreases the priority value by
adding the value 'incr' to the current priority.
It internally uses getpriority and setpriority to set the priority 
to new value.
It returns the new value of the priority, which shall be in the range 
of -15 to 19. -1 is a legitimate return value, so the user has to check 
the errno to be sure of failure.
*/
EXPORT_C int nice(int incr)
	{
	
	int retGetPri = -1;
	int retSetPri = -1;
	int retVal 	  = -1;
	
	if(((retGetPri = getpriority(PRIO_PROCESS, 0)) == -1) && (errno))
		{
		errno = ENOSYS;
		}
	else
		{
		retSetPri = setpriority(PRIO_PROCESS, 0, retGetPri + incr);
		if(!retSetPri)
			{
			retVal = 0;
			}
		}
	return retVal;
	}

} //extern "C"
