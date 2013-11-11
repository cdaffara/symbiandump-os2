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
* <<difftime>>---subtract two times
* INDEX
* difftime
* ANSI_SYNOPSIS
* #include <time.h>
* double difftime(time_t <[tim1]>, time_t <[tim2]>);
* TRAD_SYNOPSIS
* #include <time.h>
* double difftime(<[tim1]>, <[tim2]>)
* time_t <[tim1]>;
* time_t <[tim2]>;
* Subtracts the two times in the arguments: `<<<[tim1]> - <[tim2]>>>'
* that are in seconds.
* RETURNS
* The difference (in seconds) between <[tim2]> and <[tim1]>, as a <<double>>.
* PORTABILITY
* ANSI C requires <<difftime>>, and defines its result to be in seconds
* in all implementations.
* <<difftime>> requires no supporting OS subroutines.
* 
*
*/



#include <time.h>

/**
Return difference between two times.
Calculates the time difference between tim1 and tim2 in seconds.
@return The difference in seconds between the two times specified.
@param tim2 Latter time 
@param tim1 Former time
*/
EXPORT_C double
difftime (time_t tim1, time_t tim2) __SOFTFP
{
  return ((double) (tim1 - tim2));
}
