/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <tools/certapp-api.h>

#define CERTAPP "certapp"
typedef const char *ArgPtr;

#ifdef __LINUX__
#include <unistd.h>
#include <sys/wait.h>
#else
#include <process.h>
#endif

#ifndef BULLSEYE_OFF
#ifdef _BullseyeCoverage
#define BULLSEYE_OFF "BullseyeCoverage save off";
#define BULLSEYE_RESTORE "BullseyeCoverage restore";
#else
#define BULLSEYE_OFF 
#define BULLSEYE_RESTORE 
#endif
#endif

static const char * const OPT_PROGRESS = "--progress=";
static const char * const OPT_ERRORS = "--errors=";

int RunCertApp(const char *aProgress, const char *aErrors,
			   int argc, char **argv)
{
	int ret = -1;
BULLSEYE_OFF
	if(argc<0) abort(); // Bad argument
BULLSEYE_RESTORE

	int newArgc = argc+4;
	const char **newArgv = (const char **)malloc(sizeof(ArgPtr)*newArgc);

	const char *progFile = (aProgress) ? (aProgress) : ("-");
	const char *errorsFile = (aErrors) ? (aErrors) : ("-");

	char *progress=(char *)malloc(strlen(OPT_PROGRESS)+strlen(progFile)+1);
	strcpy(progress, OPT_PROGRESS);
	strcat(progress, progFile);

	char *errors=(char *)malloc(strlen(OPT_ERRORS)+strlen(errorsFile)+1);
	strcpy(errors, OPT_ERRORS);
	strcat(errors, errorsFile);
	
	newArgv[0] = CERTAPP;
	newArgv[1] = progress;
	newArgv[2] = errors;
	int i=0;
	for(i=0; i<argc; ++i)
		{
		newArgv[i+3] = argv[i];
		}
	newArgv[newArgc-1] = 0; // Terminate newArgv array

#ifdef __LINUX__
	//
	// Linux version to run certapp
	//
	pid_t pid = vfork();
	if(pid == -1)
		{
		// vfork call failed
		printf("Failed to run %s\n", CERTAPP);
		return errno;
		}
	if(pid == 0)
		{
		// Child side of vfork
		// Exec certapp
		
		execvp(CERTAPP, (char * const *)newArgv);
		// Only get here if the exec call failed...
		switch(errno)
			{
			case ENOENT:
				fprintf(stderr, "*** Could not find certapp executable to launch!\n");
				break;

			default:
				fprintf(stderr, "*** Failed to launch certapp, execvp called failed with error code%d !\n", errno);
				break;
			}
		exit(errno); 
		}

	// Parent side of vfork
	
	// Wait for certapp to finish
	for(;;)
		{
		// Block until our child to exits
		int waitStatus = waitpid(pid, &ret, 0);
		if(waitStatus == pid)
			{
			// Our child process exited and ret contains its status
			break; // Done
			}
		if(waitStatus != -1)
			{
			// Should never happen
			break;
			}
		// Decode errno
		if(errno == EINTR)
			{
			// Signal handler interrupted us - re-issue waitpid call
			continue;
			}
		// Error
		ret = errno;
		break;
		};
#else
	//
	// Windows version to run certapp
	//
	ret = _spawnvp(_P_WAIT, CERTAPP, newArgv);
	
#endif

	free(progress);
	free(errors);
	free(newArgv);

	return ret;
}

// End of file

