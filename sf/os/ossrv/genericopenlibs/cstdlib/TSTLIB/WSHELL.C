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
* William's Shell!
* Simple Directory Lister, from the GNU C help file 
* 
*
*/



#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int
dir (void)
{
  DIR *dp;
  struct dirent *ep;

  dp = opendir ("./");
  if (dp != NULL)
    {
      while (ep = readdir (dp))
        puts (ep->d_name);
      (void) closedir (dp);
    }
  else
    puts ("Couldn't open the directory.");

  return 0;
}
/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1114
@SYMTestCaseDesc	    Tests for command shell behaviour
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for command shell behaviour
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
/* A silly shell-like thing */

#include <stdlib.h>	/* definition of exit() */
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>	/* for getcwd */
#include <sys/stat.h>	/* for mkdir */

int main(int argc, char*argv[])
	{
	char cmd[80];
	char path[MAXPATHLEN+1];
	int x;

	for(;;) 
		{
		printf("%s> ", getcwd(path,sizeof(path)));
		x = scanf("%80s%s", cmd, path);

		if (x!=2)
			{
			printf("\nerror\n");
			continue;
			}

		if (strcmp(cmd,"exit")==0) 
			break;
		else
		if (strcmp(cmd,"ver")==0) 
			{
			printf("DOS version 3.30\n");
			continue;
			}
		else
		if (strcmp(cmd,"dir")==0) 
			{
			dir();
			continue;
			}
		else
		if (strcmp(cmd,"date")==0) 
			{
			time_t now=time(0);
			printf("%s",ctime(&now));
			continue;
			}
		else
		if (strcmp(cmd,"cd")==0) 
			x = chdir(path);
		else
		if (strcmp(cmd,"mkdir")==0) 
			x = mkdir(path, 0x777);
		else
		if (strcmp(cmd,"rmdir")==0) 
			x = rmdir(path);
		else
			{
			printf("Unrecognised command >%s<\n", cmd);
			continue;
			}
		if (x!=0)
			perror(path);
		}
	}