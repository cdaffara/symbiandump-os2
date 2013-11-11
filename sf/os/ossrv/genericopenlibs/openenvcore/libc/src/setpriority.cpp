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
// Name        : setpriority.cpp
// Part of     : LIBC
// Contains the source for fchdir
// Version     : 1.0
//




#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <e32std.h>
#include <e32const.h>

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR
int *GET_WSD_VAR_NAME(ProcPriority, g)();
#define ProcPriority (*GET_WSD_VAR_NAME(ProcPriority, g)())
#else
extern int ProcPriority;
#endif //EMULATOR

/*
This function checks where in the priority range the parameter falls
and sets the priority accordingly.
It returns -1 if the paramter is out of range.
*/

TProcessPriority convPri( int pri)
	{
	TProcessPriority priority = TProcessPriority(-1);

	if((pri >= UPPER_LIMIT_PRIORITY_LOWEST) &&
		 ( pri <= LOWER_LIMIT_PRIORITY_LOWEST))
		{
		priority = EPriorityLow;
		}
	else if	((pri >= UPPER_LIMIT_PRIORITY_BELOW_NORMAL) &&
			   ( pri <= LOWER_LIMIT_PRIORITY_BELOW_NORMAL))
		{
		priority = EPriorityBackground;
		}
	else if((pri >= UPPER_LIMIT_PRIORITY_NORMAL) && 
	  		  ( pri <= LOWER_LIMIT_PRIORITY_NORMAL))
		{
		priority = EPriorityForeground;
		}
	else if((pri >= UPPER_LIMIT_PRIORITY_ABOVE_NORMAL) &&
			  ( pri <= LOWER_LIMIT_PRIORITY_ABOVE_NORMAL))
		{
		priority = EPriorityHigh;
		}
	/* If priority is anything other than this,
	   Symbian wont change it for user process/thread*/
	return priority;
	}	

/*
This function setpriority() checks if the which and who parameters
sent are 'the process priority' and 'the current process' respectively,
i.e. PROC_PROCESS and 0 respectively. And calls the function convPri()
to check in which range the priority falls and returns likewise.
It returns a value in the range 0 on success and -1 on failure.

The function setpriority() shall ignore the values PRIO_PGRP,
PRIO_USER as the argument 'which' and any other value other than
0 for 'who' shall not be considered. For effective change in priority,
the  'value' should be in the range of -15 to 19. If the 'value' is 
greater than the greatest or less than the least, it will set the 'value'
to the maximum or minimum value respectively.
*/	
	
extern "C" {

EXPORT_C int setpriority( int which, int who, int value )
	{
	int retval = -1;

	if((who == 0) && (which == PRIO_PROCESS ))
		{
		if(value > LOWER_LIMIT_PRIORITY_LOWEST)
			{
			value = LOWER_LIMIT_PRIORITY_LOWEST;
			}
		if(value < UPPER_LIMIT_PRIORITY_HIGHEST)
			{
			value = UPPER_LIMIT_PRIORITY_ABOVE_NORMAL;
			}

		int priority = convPri( value );
		if( priority != -1 )
			{
			RProcess pro;
			pro.SetPriority( convPri( value ) );
			ProcPriority = value;
			retval = 0;
			}
		}
	//handling error case and not supported case
	if( retval == -1 )
		{
		errno = ENOSYS;
		}
	return retval;
	}
} //extern "C"
