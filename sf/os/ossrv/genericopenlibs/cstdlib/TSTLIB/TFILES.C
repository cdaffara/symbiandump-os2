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
* Some file manipulation via STDIO
* 
*
*/



#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>	/* for getcwd */
#include <sys/fcntl.h>	/* for O_RDONLY */
#include "CTEST.H"

test_Data;

char *poem[] = {
    "I wandered lonely as a cloud\n",
    "That floats on high o'er hill and vale\n",
    0
};

char *extraline = "Something something daffodils";

char *finalpoem = 
    "I wandered lonely as a cloud\n"
    "That floats on high o'er hill and vale\n"
    "Something something daffodils";

char* filename = "c:\\tfiles.txt";

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1055
@SYMTestCaseDesc	    Tests for file manipulation operations 
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new file and write some text into it,check for errors
                        Open a file for appending,close and open to read back the file.Check for errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void daffodils()
	{
	FILE *in, *out;
	char *cp, buf[256];
	int n, err;
	

	test_Next("Writing simple text file");

	out = fopen(filename, "w");
	test(out != 0); 

	for (n=0; poem[n]; n++)
	    {
	    err=fputs(poem[n], out);
		test(err>=0);
	    }
	fclose(out);

	/* 2. Open a file for appending */

	test_Next("Open file for appending");

	out = fopen(filename, "a");
	test(out != 0);

	n = fwrite(extraline, 1, strlen(extraline), out);
	test(n == (int)strlen(extraline));
	fclose(out);

	/* 3. Read back the file */

	test_Next("Read back the file using fread");

	in = fopen(filename, "r");
	test(in != 0);
	cp = finalpoem;

	do
	    {
	    n = fread(buf, 1, 17, in);
	    test(n >= 0);
		if (n>0)
			test(strncmp(buf,cp,n)==0);
		cp += n;
	    }
	while (!feof(in));

	fclose(in);

	/* 4. Read back the file a line at a time */

	test_Next("Read back the file using fgets");

	in = fopen(filename, "r");
	test(in != 0);

	n = 0;
	do
	    {
	    cp = fgets(buf, sizeof(buf), in);
	    test(cp != NULL);
		if (poem[n])
			{
			test(strcmp(cp,poem[n])==0);
			n++;
			}
		else
			test(strcmp(cp,extraline)==0);
	    }
	while (!feof(in));

	fclose(in);
	}

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1056
@SYMTestCaseDesc	    Tests for maximum files open and close operations
@SYMTestPriority 	    High
@SYMTestActions  	    Open a file 100 times and close them in different orders.Tests for the error code. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
#define NFILES 100
void maxfiles()
	{
	int fids[NFILES];
	int i,n;

	test_Next("Open file 100 times, close in reverse order");
	for (i=0; i<NFILES; i++)
		{
		fids[i] = open(filename, O_RDONLY, 0666);
		test(fids[i]>=0);
		}
	for (i=NFILES-1; i>=0; i--)
		{
		close(fids[i]);
		}

	test_Next("Open file 100 times, close in same order");
	for (i=0; i<NFILES; i++)
		{
		fids[i] = open(filename, O_RDONLY, 0666);
		test(fids[i]>=0);
		}
	for (i=0; i<NFILES; i++)
		{
		close(fids[i]);
		}

	test_Next("Open file 100 times, close in mod 7 order");
	for (i=0; i<NFILES; i++)
		{
		fids[i] = open(filename, O_RDONLY, 0666);
		test(fids[i]>=0);
		}
	for (i=0,n=0; i<NFILES; i++)
		{
		close(fids[n]);
		n += 7;
		if (n >= NFILES)
			n -= NFILES;
		}
	}

int close_console=0;
void allTests()
	{
	daffodils();
	maxfiles();

	if (close_console)
		{
		test_Close();
		close(0);
		close(1);
		close(2);
		}
	}

int main()
	{
	void* client;
	int err;

	test_Title("TFILES");

	allTests();

	test_Next("Do it again using the CPosixServer (for them, not me)");
	close_console=1;

	start_posix_server();	/* calls SpawnPosixServer from C++ code */


	client=create_thread(allTests, "TFILES tests");
	test(client!=0);
	start_thread(client);
	err=wait_for_thread(client);
	test(err==0);

	test_Close();

	return 0;
	}
