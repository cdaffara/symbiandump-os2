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
* ANSI C version of the RTest facilities, specifically
* Start(char *title)
* Next(char *title)
* Test(int line);
* As a simplification we don't do the levels of nesting.
* 
*
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "CTEST.H"

static void failed(struct __testdata* td, int line)
	{
	printf("Test %d.%d failed, line %d\n", td->iCheck, td->iSubtest, line);
	printf("Press any key to exit\n");
	fflush(stdout);
	getchar();
	exit(84);
	}

EXPORT_C void td_Title(struct __testdata* td, char *title)
	{
	
	printf("%s\n\n", title);
	td->iCheck=0;
	td->iSubtest=0;
	}

EXPORT_C void td_Next(struct __testdata* td, char *testname)
	{
	printf("%03d: %s\n", ++(td->iCheck), testname);
	td->iSubtest=0;
	}

EXPORT_C void td_Test(struct __testdata* td, int line, int mustBeTrue)
	{
	(td->iSubtest)++;
	if (mustBeTrue)
		return;
	failed(td, line);
	}

EXPORT_C void td_TestErrno(struct __testdata* td, int line, int mustBeTrue, int expectedErrno)
	{
	(td->iSubtest)++;
	td->iErrno = errno;
	if (mustBeTrue)
		{
		if (expectedErrno==td->iErrno)
			return;
		printf("Test %d.%d failed, line %d : expected errno = %d, not %d\n",
			td->iCheck, td->iSubtest, line, expectedErrno, td->iErrno);
		}
	failed(td, line);
	}

EXPORT_C void td_Close()
	{
	printf("\n\nCompleted OK\n");
	fflush(stdout);
	}
