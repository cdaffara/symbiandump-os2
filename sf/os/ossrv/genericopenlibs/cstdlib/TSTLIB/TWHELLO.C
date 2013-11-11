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
* The classic "Hello World" program made wide
* 
*
*/



#include <stdio.h>
#include <stdlib.h>

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1095
@SYMTestCaseDesc	    Tests for printing to standard output stream
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing a string to standard output stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int wmain (int argc, wchar_t *argv[])
	{
	int x;
	char buffer[200];
	printf("Hello World\n");
	for (x=1; x < argc; x++)
		{
		wcstombs(buffer, argv[x], sizeof(buffer));
		printf("arg %d is %s\n", x, buffer);
		}
	return 0;
	}
