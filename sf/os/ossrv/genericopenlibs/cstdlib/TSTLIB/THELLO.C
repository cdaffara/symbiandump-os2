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
* The classic "Hello World" program
* 
*
*/



#include <stdio.h>

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1057
@SYMTestCaseDesc	    Tests for printing to standard output stream
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for writing a string to standard output stream
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
int main (int argc, char *argv[])
	{
	printf("Hello World\n");
	return 0;
	}
