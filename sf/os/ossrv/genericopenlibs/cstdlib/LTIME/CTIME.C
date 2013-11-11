/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* FUNCTION
* <<ctime>>---convert time to local and format as string
* INDEX
* ctime
* ANSI_SYNOPSIS
* #include <time.h>
* char *ctime(time_t <[clock]>);
* char *ctime_r(time_t <[clock]>, char *<[buf]>);
* TRAD_SYNOPSIS
* #include <time.h>
* char *ctime(<[clock]>)
* time_t <[clock]>;
* char *ctime_r(<[clock]>, <[buf]>)
* time_t <[clock]>;
* char *<[buf]>;
* Convert the time value at <[clock]> to local time (like <<localtime>>)
* and format it into a string of the form
* . Wed Jun 15 11:38:07 1988\n\0
* (like <<asctime>>).
* RETURNS
* A pointer to the string containing a formatted timestamp.
* PORTABILITY
* ANSI C requires <<ctime>>.
* <<ctime>> requires no supporting OS subroutines.
* 
*
*/



#include <time.h>
#include <sys/reent.h>

#ifndef _REENT_ONLY

/**
Convert time_t value to string.
Converts tim_p to a string containing time 
and date adjusted to local time zone in readable format.
@return A pointer to the string containing the date 
and time information in readable format.
The string pointed is statically allocated 
and shared by ctime and asctime functions. 
Each time one of these functions is called 
the content of the string is overwritten.
@param tim_p pointer to a time_t value, 
usually returned by time function.
*/
EXPORT_C char *
ctime (const time_t * tim_p)
{
  return ctime_r (tim_p, _REENT->_asctime);
}

#endif
