
// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : tcapsignalblocks.cpp
// 
//

#include "tcapsignal.h"

// -----------------------------------------------------------------------------
// CTesttrapsignal::captestonkill1()
// Test Case ID: OPENENV-LIBC-CIT-6000, OPENENV-LIBC-CIT-6001
// API tested: kill()
// Description: Capability test for kill() with SIGKILL signal
// -----------------------------------------------------------------------------

TInt CTestcapsignal::captestonkill1()
	{
	int pid, ret, ret1 = KErrGeneral, pid1, status;
	char **argv=(char**)malloc(2*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\kill.exe");
	ret = posix_spawn(&pid, "z:\\sys\\bin\\kill.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	User::After(50000); // TODO: Remove once sending issue is fixed
	INFO_PRINTF2(_L("the value of pid returned by posix_spawn is %d"), pid);
	ret = kill(pid,SIGKILL);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in sending a signal and errno is set to %d"),errno);
		ret1 = errno;
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	ret1 = KErrNone;	
	
close:
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::captestonkill2()
// Test Case ID: OPENENV-LIBC-CIT-6002, OPENENV-LIBC-CIT-6003
// API tested: kill()
// Description: Capability test for kill() with SIGSTOP signal
// -----------------------------------------------------------------------------

TInt CTestcapsignal::captestonkill2()
	{
	int pid, ret, ret1 = KErrGeneral, pid1, status;
	char **argv=(char**)malloc(2*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\kill.exe");
	ret = posix_spawn(&pid, "z:\\sys\\bin\\kill.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	User::After(50000); // TODO: Remove once sending issue is fixed
	INFO_PRINTF2(_L("the value of pid returned by posix_spawn is %d"), pid);
	ret = kill(pid,SIGSTOP);
	if(ret == -1)
		{
		ERR_PRINTF2(_L("Error in sending a signal and errno is set to %d"),errno);
		ret1 = errno;
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	ret1 = KErrNone;
	
close:
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;
	}
	
	
//End of file
