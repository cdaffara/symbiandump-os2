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
* Time handling
* 
*
*/



#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>	/* timeval, gettimeofday */

void msdev_time_example(void);

void NEXT_TEST(const char *title)
    {
    static int first = 1;
    int c;
    if (!first)
	{
	printf("\nContinue [Y/N]? ");
	c = getchar();
	if (c == 'q' || c == 'Q' || c == 'n' || c == 'N')
	    {
	    printf("\nAbandoned.\n");
	    exit(-1);
	    }
	}
    first = 0;
    printf("\f\n%s\n\n", title);
    }

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1080
@SYMTestCaseDesc	    Time handling functions test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for time handling routines
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int main (int argc, char *argv[])
    {
    time_t now, then;
    struct tm *tmp, newtm;
    char tmpbuf[128];
    struct timeval tv, tv2;
    struct timezone tz;
    int err, duration, count;
   
    NEXT_TEST("What's the time, Mr Epoc32");

    then = 0;
    now = time(0);
    err = gettimeofday(&tv, &tz);

    printf("time_t 0     = %9ld = %s\n", then, ctime(&then));
    printf("time now     = %9ld = %s", now, ctime(&now));

    if (err < 0)
	printf("gettimeofday failed\n");
    else 
	{
	printf("gettimeofday = %9ld + %d usecs\n\n", tv.tv_sec, tv.tv_usec);
	printf("timezone     = %d mins west of GMT, DST is %s\n",
	    tz.tz_minuteswest, tz.tz_dsttime? "ON":"OFF");
	}

    
    NEXT_TEST("Cost of getttimeofday");

    count = 0;
    gettimeofday(&tv, &tz);

    /* Wait for first usec transition */
    err = gettimeofday(&tv2, &tz);
    while (err == 0 && tv2.tv_sec == tv.tv_sec && tv2.tv_usec == tv.tv_usec)
		{
		err = gettimeofday(&tv, &tz);
		}

    /* Now wait for second transition */
    err = gettimeofday(&tv2, &tz);
    while (err == 0 && tv2.tv_sec == tv.tv_sec &&tv2.tv_usec == tv.tv_usec)
	{
	err = gettimeofday(&tv2, &tz);
	count+=1;
	}

    if (err < 0)
	printf("gettimeofday failed\n");
    else
	{
	duration = 1000000 *(tv2.tv_sec - tv.tv_sec) + tv2.tv_usec - tv.tv_usec;
	if (tv2.tv_usec < tv.tv_usec)
	    duration += 1000000;
	printf("smallest usec delta = %d usecs\n", duration);
	printf("gettimeofday took %d usecs for %d calls = %d usecs\n",
	    duration, count, duration/count);
	}


    NEXT_TEST("localtime: Conversion to struct tm");
    
    tmp = localtime(&now);
    printf("time now = %9ld = %s", now, ctime(&now));
    printf("tm now   = %02d/%02d/%02d %02d:%02d.%02d, wday=%d, dayno=%d\n",
	tmp->tm_mday, tmp->tm_mon+1, tmp->tm_year,
	tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
	tmp->tm_wday, tmp->tm_yday);

    
    NEXT_TEST("mktime: Conversion from struct tm");
    
    /* Example derived from MSDEV mktime documentation */

    newtm.tm_mday = 03;
    newtm.tm_mon  = 04;
    newtm.tm_year = 94;
    newtm.tm_hour = 12;
    newtm.tm_min  = 45;
    newtm.tm_sec  = 47;
    newtm.tm_wday = 9999;
    newtm.tm_yday = 9999;

    tmp = &newtm;
    printf("tm before = %02d/%02d/%02d %02d:%02d.%02d, wday=%d, dayno=%d\n",
	tmp->tm_mday, tmp->tm_mon+1, tmp->tm_year,
	tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
	tmp->tm_wday, tmp->tm_yday);
    then = mktime(tmp);
    printf("tm after  = %02d/%02d/%02d %02d:%02d.%02d, wday=%d, dayno=%d\n",
	tmp->tm_mday, tmp->tm_mon+1, tmp->tm_year,
	tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
	tmp->tm_wday, tmp->tm_yday);
    printf("then      = %ld = %s", then, ctime(&then));

    printf("\nAdd 29 days...(Wed Jun 01)\n\n");
    newtm.tm_mday += 29;
    then = mktime(tmp);
    printf("tm after  = %02d/%02d/%04d %02d:%02d.%02d, wday=%d, dayno=%d\n",
	tmp->tm_mday, tmp->tm_mon+1, tmp->tm_year+1900,
	tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
	tmp->tm_wday, tmp->tm_yday);
    printf("then      = %ld = %s", then, ctime(&then));


    NEXT_TEST("strftime: customised string conversion");
    
    tmp = localtime( &now );
    strftime( tmpbuf, 128,
         "Today is %A, day %d of %B in the year %Y.\n", tmp );
    printf("time now = %9ld = %s", now, ctime(&now));
    printf( tmpbuf );

    
    NEXT_TEST("Example code from MSDEV documentation");

    msdev_time_example();

    return 0;
   }

/* 
Example code derived from MSDEV documentation on time() function
  
Time in seconds since UTC 1/1/70:       768027063
UNIX time and date:                     Tue May 03 21:51:03 1994
Coordinated universal time:             Wed May 04 04:51:03 1994
12-hour time:                           09:51:03 PM
Christmas                               Sat Dec 25 12:00:00 1993

Today is Tuesday, day 03 of May in the year 1994.
*/

void msdev_time_example(void)
{
    char tmpbuf[128], ampm[] = "AM";
    time_t ltime;
    struct tm *today, *gmt, xmas = { 0, 0, 12, 25, 11, 93 };

    /* Get UNIX-style time and display as number and string. 
     * time( &ltime );	Tue May 03 21:51:03 1994
     */
    struct tm example = { 3, 51, 21, 3, 4, 94 };
    ltime = mktime(&example);

    printf( "Time in seconds since UTC 1/1/70:   %ld\n", ltime );
    printf( "UNIX time and date:                 %s", ctime( &ltime ) );

    /* Display UTC. */
    gmt = gmtime( &ltime );
    printf( "Coordinated universal time:         %s", asctime( gmt ) );

    /* Convert to time structure and adjust for PM if necessary. */
    today = localtime( &ltime );
    if( today->tm_hour > 12 )
	{
	strcpy( ampm, "PM" );
	today->tm_hour -= 12;
	}
    if( today->tm_hour == 0 )  /* Adjust if midnight hour. */
	today->tm_hour = 12;

    /* Note how pointer addition is used to skip the first 11 
     * characters and printf is used to trim off terminating 
     * characters.
     */
    printf( "12-hour time:                       %.8s %s\n",
       asctime( today ) + 11, ampm );

    /* Make time for noon on Christmas, 1993. */
    if( mktime( &xmas ) != (time_t)-1 )
	printf( "Christmas                           %s\n", asctime( &xmas ) );

    /* Use time structure to build a customized time string. */
    today = localtime( &ltime );

    /* Use strftime to build a customized time string. */
    strftime( tmpbuf, 128,
	"Today is %A, day %d of %B in the year %Y.\n", today );
    printf( tmpbuf );
}

  

 
