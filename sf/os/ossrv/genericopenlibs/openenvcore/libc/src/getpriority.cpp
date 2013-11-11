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
// Name        : getpriority.cpp
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
GET_GLOBAL_VAR_FROM_TLS(ProcPriority, int)
#define ProcPriority (*GET_WSD_VAR_NAME(ProcPriority, g)())
#else //EMULATOR
int ProcPriority = 0;
#endif //EMULATOR

/*
This function checks the range in which the priority of the process falls
and returns the same to the function getpriority(). If the value is out of
range it returns -1.
*/

int convPri( TProcessPriority pri)
	{
	int priority;
	
	if(((pri == EPriorityLow ) &&
		 ( (ProcPriority >= UPPER_LIMIT_PRIORITY_LOWEST) &&
		 	 ( ProcPriority <= LOWER_LIMIT_PRIORITY_LOWEST) ))||
		( pri == EPriorityBackground &&
		 ( (ProcPriority >= UPPER_LIMIT_PRIORITY_BELOW_NORMAL) &&
		 	 ( ProcPriority <= LOWER_LIMIT_PRIORITY_BELOW_NORMAL) )) ||
		( pri == EPriorityForeground &&
		 ( (ProcPriority >= UPPER_LIMIT_PRIORITY_NORMAL) &&
		 	 ( ProcPriority <= LOWER_LIMIT_PRIORITY_NORMAL) )) ||
		( pri == EPriorityHigh &&
		 ( (ProcPriority >= UPPER_LIMIT_PRIORITY_ABOVE_NORMAL) &&
		 	 ( ProcPriority <= LOWER_LIMIT_PRIORITY_ABOVE_NORMAL) ))||
		( pri == EPriorityRealTimeServer &&
		 ( (ProcPriority >= UPPER_LIMIT_PRIORITY_HIGHEST) &&
		 	 ( ProcPriority <= LOWER_LIMIT_PRIORITY_HIGHEST) )))
		{
		priority = ProcPriority;
		}
	else
		{
		priority = -1;
		errno = EINVAL;
		}		
	return priority;
	}
	
/*
This function checks if the which and who parameters sent are 
'the process priority' and 'the current process' respectively,
i.e. PROC_PROCESS and 0 respectively. And calls the function 
convPri() to check in which range the priority falls and returns
likewise.
It returns a value in the range -20 to 19 on success and -1 is a 
legitimate return value,so user has to check the errno to be sure 
of failure.
The function getpriority() shall ignore the values PRIO_PGRP,
PRIO_USER as the argument 'which' and any other value other than
0 for 'who' shall not be considered.
*/	

extern "C" {

EXPORT_C int getpriority(int which, int who)
	{
	int priority = -1;
	if ( which == PRIO_PROCESS && who == 0 )
		{
		RProcess pro;
		TProcessPriority pri;
		pri = pro.Priority();		
		priority = convPri( pri );
		}
	else
		{
		errno = ENOSYS;
		}
	return priority;
	}

} //extern "C"
