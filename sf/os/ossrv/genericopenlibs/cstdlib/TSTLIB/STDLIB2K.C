/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Year 2000 compliance tests for STDLIB
* 
*
*/



#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>	/* timeval, gettimeofday */

int failures=0;
void failed(char* reason)
	{
	printf("\nFAILURE >>>%s\n", reason);
	failures++;
	}



typedef struct {
	int day;	/* 1-31 */
	int month;	/* 1-12 */
	int year;	/* 1970-2050 */
	} testdate;

char* testmonths[14] = {
	"invalid_0",
	"Jan", "Feb", "March", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec",
	"invalid_13"
	};

char* tmdaynames[8] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
	"invalid_7"
	};

/* Format a testdate to match the Y2K document
 */
void format_testdate(testdate* aDate, char* aBuffer)
	{
	char* th="th";
	if (aDate->day==1 || aDate->day==21 || aDate->day==31)
		th="st";
	if (aDate->day==2 || aDate->day==22)
		th="nd";

	sprintf(aBuffer, "%d%s %s %d", aDate->day, th, testmonths[aDate->month], aDate->year);
	}

typedef struct {
	testdate	input;		/* for conversion to seconds */
	testdate	result;		/* correct value */
	} testpair;

typedef struct {
	testdate	input;		/* for conversion to seconds */
	int		result;		/* correct value */
	} testint;

/* MAJOR TEST FUNCTIONALITY - READ THIS CAREFULLY
 *
 * STDLIB date handling is done in terms of seconds since the "epoch", which was
 * 00:00 on 1st January, 1970. The Y2K significant functionality consists solely of the
 * routines which manipulate the struct tm data structure.
 *
 *   time_t mktime (struct tm *brokentime)
 *
 *   struct tm * gmtime (const time_t *time)
 *   struct tm * localtime (const time_t *time)
 *
 * The mktime conversion is always using a GMT date, so we'll ignore the localtime function for Y2K
 * purposes. We also know that gmtime and localtime use the same underlying conversion function,
 * one applying the relevant GMT offset on the way in.
 *
 * Y2K is only concerned with dates, so we will always use 12:00:00 ("High Noon") as the time
 * of day.
 */

void tm_from_testdate(struct tm* aTm, testdate* aDate)
	{
	time_t seconds;
	struct tm setup;
	struct tm* res;

	setup.tm_hour = 12;
	setup.tm_min  = 0;
	setup.tm_sec  = 0;

	setup.tm_mday = aDate->day;
	setup.tm_mon  = aDate->month - 1;	/* struct tm uses 0-11 */
	setup.tm_year = aDate->year - 1900;	/* struct tm uses years since 1900 */

	seconds = mktime(&setup);
	if (seconds != (time_t)(-1))
		{
		res = gmtime(&seconds);
		*aTm = *res;
		return;
		}

	/* unable to convert date */
	failed("tm_from_testdate: mktime failed");
	}

void normalize_tm(struct tm* aTm)
	{
	time_t seconds = mktime(aTm);
	if (seconds == (time_t)(-1))
		{
		/* unable to convert date */
		failed("normalize_tm: mktime failed");
		}
	return;
	}

int compare_testdate_tm(testdate* aDate, struct tm* aTm)
	{
	if (aDate->year-1900 != aTm->tm_year)
		return (aDate->year-1900-aTm->tm_year);
	if (aDate->month-1 != aTm->tm_mon)
		return (aDate->month-1-aTm->tm_mon);
	return (aDate->day - aTm->tm_mday);
	}

/*
 *   4.1 Rule 1 Tests
 *
 * "No valid value for current date will cause any interruption in operation"
 *
 */

testpair data_4_1[14] = {
	{{ 31,12,1998 } , {  1, 1, 1999 } },
	{{ 27, 2,1999 } , { 28, 2, 1999 } },
	{{ 28, 2,1999 } , {  1, 3, 1999 } },
	{{ 31, 8,1999 } , {  1, 9, 1999 } },
	{{  8, 9,1999 } , {  9, 9, 1999 } },
	{{  9, 9,1999 } , { 10, 9, 1999 } },
	{{ 31,12,1999 } , {  1, 1, 2000 } },
	{{ 27, 2,2000 } , { 28, 2, 2000 } },
	{{ 28, 2,2000 } , { 29, 2, 2000 } },
	{{ 29, 2,2000 } , {  1, 3, 2000 } },
	{{ 31,12,2000 } , {  1, 1, 2001 } },
	{{ 28, 2,2001 } , {  1, 3, 2001 } },
	{{ 28, 2,2004 } , { 29, 2, 2004 } },
	{{ 29, 2,2004 } , {  1, 3, 2004 } }
	};

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1043
@SYMTestCaseDesc	    Tests for standard date boundaries
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for standard date format
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void tests_4_1()
	{
	int i;
	testdate* from_dp;
	testdate* to_dp;
	char frombuf[80];
	char tobuf[80];
	struct tm workdate;

	printf("\n4.1 Rule 1 Tests\n\n");
	printf("4.1.1.1 Standard Date Boundaries\n\n");
	for (i=0; i<14; i++)
		{
		from_dp=&data_4_1[i].input;
		to_dp=&data_4_1[i].result;

		format_testdate(from_dp, frombuf); 
		format_testdate(to_dp, tobuf); 
		printf("%-2d  From %-14s to %-14s ", i+1, frombuf, tobuf);

		tm_from_testdate(&workdate, from_dp);
		if (compare_testdate_tm(from_dp, &workdate) != 0)
			failed("test_4_1: invalid from date");

		workdate.tm_mday += 1;	/* move the time on by one day */

		normalize_tm(&workdate);
		if (compare_testdate_tm(to_dp, &workdate) != 0)
			{
			printf("*** FAILED\n");
			failures++;
			}
		else
			printf("PASSED\n");
		}

	}

/*
 *   4.2 Rule 2 Tests (incorporating Rule 4 Tests)
 *
 * "All manipulations of date or time related data will produce the required results for
 *  all valid date values prior to, during and after Year 2000"
 *
 * "Year 2000 must be recognised as a leap year"
 *
 */

testdate data_4_2_1[19] = {
	{ 31, 12, 1998 },
	{  1,  3, 1999 },
	{ 27,  2, 2000 },
	{ 31, 12, 2000 },
	{ 28,  2, 2004 },
	{  1,  1, 1999 },
	{  9,  9, 1999 },
	{ 28,  2, 2000 },
	{  1,  1, 2001 },
	{ 29,  2, 2004 },
	{ 27,  2, 1999 },
	{ 31, 12, 1999 },
	{ 29,  2, 2000 },
	{ 28,  2, 2000 },
	{  1,  3, 2004 },
	{ 28,  2, 1999 },
	{  1,  1, 2000 },
	{  1,  3, 2000 },
	{  1,  3, 2001 }
	};

void tests_4_2_1()
	{
	int i;
	testdate* from_dp;
	char frombuf[80];
	struct tm workdate;

	printf("\n4.2 Rule 2 Tests (incorporating Rule 4 Tests)\n\n");
	printf("4.2.1 Valid Dates\n\n");
	for (i=0; i<19; i++)
		{
		from_dp=&data_4_2_1[i];

		format_testdate(from_dp, frombuf); 
		printf("%-2d  Valid date %-14s  ", i+1, frombuf);

		tm_from_testdate(&workdate, from_dp);
		if (compare_testdate_tm(from_dp, &workdate) != 0)
			{
			printf("*** FAILED\n");
			failures++;
			}
		else
			printf("PASSED\n");
		}
	}

testdate data_4_2_2[5] = {
	{ 31,  4, 1998 },
	{ 30,  2, 2000 },
	{ 29,  2, 2001 },
	{ 29,  2, 1999 },
	{ 30,  2, 2004 }
	};

void tests_4_2_2()
	{
	int i;
	testdate* from_dp;
	char frombuf[80];
	struct tm workdate;

	printf("\n4.2.2 Invalid Dates\n\n");
	for (i=0; i<5; i++)
		{
		from_dp=&data_4_2_2[i];

		format_testdate(from_dp, frombuf); 
		printf("%-2d  Invalid date %-14s  ", i+1, frombuf);

		tm_from_testdate(&workdate, from_dp);
		if (compare_testdate_tm(from_dp, &workdate) == 0)
			{
			printf("*** FAILED\n");
			failures++;
			}
		else
			printf("PASSED\n");
		}
	}

testint data_4_2_4[3] = {
	{ { 31, 12, 2000 }, 366 },
	{ { 31, 12, 1999 }, 365 },
	{ { 31, 12, 2004 }, 366 }
	};

void tests_4_2_4()
	{
	int i;
	testdate* from_dp;
	char frombuf[80];
	struct tm workdate;

	printf("\n4.2.4 Year Lengths\n\n");
	for (i=0; i<3; i++)
		{
		from_dp=&data_4_2_4[i].input;

		format_testdate(from_dp, frombuf); 
		printf("%-2d  Yearday of %-14s == %d ", i+1, frombuf, data_4_2_4[i].result);

		tm_from_testdate(&workdate, from_dp);
		if (workdate.tm_yday+1 != data_4_2_4[i].result)
			{
			printf("*** FAILED\n");
			failures++;
			}
		else
			printf("PASSED\n");
		}
	}

testint data_4_2_7[14] = {
	{ {  1,  1, 1900 }, 1 },	// not supported
	{ { 28,  2, 1900 }, 3 },	// not supported
	{ {  1,  3, 1900 }, 4 },	// not supported
	{ { 28,  2, 1999 }, 0 },
	{ {  1,  3, 1999 }, 1 },
	{ { 31, 12, 1999 }, 5 },
	{ {  1,  1, 2000 }, 6 },
	{ { 28,  2, 2000 }, 1 },
	{ { 29,  2, 2000 }, 2 },
	{ {  1,  3, 2000 }, 3 },
	{ {  1,  1, 2001 }, 1 },
	{ { 28,  2, 2004 }, 6 },
	{ { 29,  2, 2004 }, 0 },
	{ {  1,  3, 2004 }, 1 }
	};

void tests_4_2_7()
	{
	int i;
	testdate* from_dp;
	char frombuf[80];
	struct tm workdate;

	printf("\n4.2.7 Calculation of days of the week from dates\n\n");
	for (i=3; i<14; i++)
		{
		from_dp=&data_4_2_7[i].input;

		format_testdate(from_dp, frombuf); 
		printf("%-2d  %-14s == %-10s ", i+1, frombuf, tmdaynames[data_4_2_7[i].result]);

		tm_from_testdate(&workdate, from_dp);
		if (compare_testdate_tm(from_dp, &workdate) != 0)
			failed("test_4_2_7: invalid from date");

		if (workdate.tm_wday != data_4_2_7[i].result)
			{
			printf("*** FAILED  (%d %s)\n", workdate.tm_wday, tmdaynames[workdate.tm_wday]);
			failures++;
			}
		else
			printf("PASSED\n");
		}
	}

void do_all_tests()
	{
	time_t now = time(0);
	failures=0;

	tests_4_1();
	tests_4_2_1();
	tests_4_2_2();
	tests_4_2_4();
	tests_4_2_7();

	if (failures != 0)
		printf("\n\n NOT YEAR 2000 COMPLIANT\n\n");
	else
		printf("\n\n ALL TESTS PASSED - YEAR 2000 COMPLIANT\n\n");

	printf("Test run dated: %s", ctime(&now));
	}

int main()
	{
	int fd;

	do_all_tests();	

	/* Now repeat the tests into a log file */

	fd=open("c:/stdlib_y2k.txt", O_WRONLY+O_CREAT+O_TRUNC, 0);
	if (fd<0)
		{
		printf("unable to create test result file\n");
		return -1;
		}

	setbuf(stderr, NULL);
	dup2(fd, fileno(stderr));	/* redirect stderr */
	setbuf(stdout, NULL);
	dup2(fd, fileno(stdout));	/* redirect stdout */
	close(fd);
	fclose(stdin);

	do_all_tests();

	fflush(stdout);
	return 0;
	}
