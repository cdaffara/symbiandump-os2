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
// Name        : tsendsignalblocks.cpp
// Test cases to validate sending a signal to self and other process's
//



#include "tsendsignal.h"


//to clear signal mask and set the handler to default
static int setsigenv()
	{
	int Signum, ret, ret1 = KErrGeneral ;
	for(Signum=1; Signum <= 64; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = sigignore(Signum);
		if (ret != 0)
			{
			goto close;
			}
		ret = sigrelse(Signum);
		if (ret != 0)
			{
			goto close;
			}
		}
	User::After(50000);
	for(Signum=1; Signum <= 64; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}		
		if (signal(Signum,SIG_DFL) == SIG_ERR)
			{
			goto close;
			}	
		}
	ret1 = KErrNone;
	
	close:
	alarm(0);
	return ret1;
	}
	

// -----------------------------------------------------------------------------
// CTestsendsignal::TestRaise
// Test Case ID: OPENENV-LIBC-CIT-5986
// API tested: raise()
// Description: Positive test to raise a signal using raise() api
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestRaise (  )
	{   
	int ret, status, Signum, ret1 = KErrGeneral;
	pid_t pid, pid1;
	char **argv=(char**)malloc(3*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	strcpy(argv[0], "z:\\sys\\bin\\raisesignal.exe");
	sprintf(argv[1], "%d", Signum);	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\raisesignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	if(WEXITSTATUS(status) != 0)
		{
		ERR_PRINTF1(_L("raise() failed in the child process"));
		goto close;		
		}
	INFO_PRINTF2(_L("Successfully able to raise a signal %d"),Signum);
	ret1 = KErrNone;

close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestRaise1
// 
// API tested: raise()
// Description: Positive test to raise a signal using raise() api
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestRaise1 (  )
	{ 
	int ret, status, ret1 = KErrGeneral;
	pid_t pid, pid1;
	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\raisesignal1.exe", NULL, NULL, NULL, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		return ret1;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		return ret1;
		}
	if(WEXITSTATUS(status) != 0)
		{
		ERR_PRINTF1(_L("raise() failed in the child process"));
		return ret1;		
		}
	INFO_PRINTF1(_L("Successfully able to raise a signal "));
	ret1 = KErrNone;
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeRaise ( )
// Test Case ID: OPENENV-LIBC-CIT-5987
// API tested: raise()
// Description: To send an invalid signal to self using raise()
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeRaise (  )
	{   
	int ret1=KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on raise"));
	ret = raise(Signum);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("raise failed to check an invalid signal"));
		goto close;
		}
	INFO_PRINTF2(_L("The errno returned from raising an invalid signal is %d\n"),errno);
	ret1 = KErrNone;
		
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestKill1
// Test Case ID: OPENENV-LIBC-CIT-5988
// API tested: kill()
// Description: To send a signal to other process
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestKill1 (  )
	{   
	int ret, ret1 = KErrGeneral, status, Signum;
	pid_t pid, pid1;
	char **argv=(char**)malloc(3*sizeof(char*));
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\sendsignal.exe");
	sprintf(argv[1], "%d", Signum);	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	User::After(50000); // TODO: Remove once sending issue is fixed
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	ret = kill(pid,Signum);
	if(ret != 0)
		{
		ERR_PRINTF2( _L("Error in sending the signal to the spawned process and errno is set %d"),errno);
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	if(WEXITSTATUS(status) != 0)
		{
		ERR_PRINTF2(_L("sigqueue() failed in the child process for signal %d"),Signum);
		goto close;		
		}
	INFO_PRINTF2(_L("Successfully able to send a signal %d"),Signum);
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestKill2
// Test Case ID: OPENENV-LIBC-CIT-5989
// API tested: kill()
// Description: Positive test to send a signal to self using kill()
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestKill2 (  )
	{    
	int ret, ret1 = KErrGeneral, status, Signum;
	pid_t pid, pid1;
	char **argv=(char**)malloc(3*sizeof(char*));
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\sendsignaltoself.exe");
	sprintf(argv[1], "%d", Signum);	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsignaltoself.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF3(_L("waitpid failed for signal %d and errno is %d"),Signum,errno);	
		goto close;
		}
	if(WEXITSTATUS(status) != 0)
		{
		ERR_PRINTF1(_L("raise() failed in the child process"));
		goto close;		
		}
	INFO_PRINTF2(_L("Successfully able to send a signal %d"),Signum);
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeKill1 ( )
// Test Case ID: OPENENV-LIBC-CIT-5990
// API tested: kill()
// Description: Test case to send a signal with s =0 to a valid process using kill().
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeKill1 (  )
	{  
	int ret1 = KErrGeneral, pid, pid1, ret, status;
	char **argv=(char**)malloc(3*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\receivesignal.exe");
	sprintf(argv[1], "%d", 0);	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\receivesignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	ret = kill(pid,0);
	INFO_PRINTF1(_L("Negative test on Kill()"));
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Failed to set the return value"));	
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}	
	INFO_PRINTF1(_L("Kill() returned 0 as the process exists"));
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeKill2 ( )
// Test Case ID: OPENENV-LIBC-CIT-5991
// API tested: kill()
// Description: Negative test: To send an invalid signal to other process using kill()
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeKill2 (  )
	{  
	int ret1 = KErrGeneral, pid, pid1, Signum, ret, status;
	char **argv=(char**)malloc(3*sizeof(char*));
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close; 
		}
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\receivesignal.exe");
	ret = posix_spawn(&pid, "z:\\sys\\bin\\receivesignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	ret = kill(pid,Signum);
	INFO_PRINTF1(_L("Negative test on Kill()"));
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to set the return value"));	
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}	
	INFO_PRINTF1(_L("Both the return and expected value of kill() are same"));
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeKill3 ( )
// Test Case ID: OPENENV-LIBC-CIT-5992, OPENENV-LIBC-CIT-5993, OPENENV-LIBC-CIT-5994
// API tested: kill()
// Description: Negative test on sending a signal with an unsupported pid value using kill()
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeKill3 (  )
	{  
	int Maxsig, Pid, Signum, ret, ret1 = KErrGeneral, Sigval;
	ret = GetIntFromConfig(ConfigSection(), _L("Pid"), Pid);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the Process id number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"), Maxsig);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the Maximum sig value")) ;
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on Kill()"));
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		ret = kill(Pid,Signum);
		if((ret != -1) || (errno != ENOTSUP))
			{
			ERR_PRINTF2(_L("Failed to set the return value for signal %d"), Signum);	
			goto close;
			}
		}
	INFO_PRINTF1(_L("Negative test: Both the errno and expected value of kill() are same validated for all signals"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeKill4 ( )
// Test Case ID: OPENENV-LIBC-CIT-5995,OPENENV-LIBC-CIT-5996,OPENENV-LIBC-CIT-5997
// API tested: kill()
// Description: Negative test on kill() for errno ESRCH
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeKill4 (  )
	{  
	int Signum, ret, ret1 = KErrGeneral;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = kill(1000,Signum);
	INFO_PRINTF1(_L("Negative test on Kill()"));
	if((ret != -1) || (errno != ESRCH))
		{
		ERR_PRINTF1(_L("Kill() failed to return ESRCH on negative test"));	
		goto close;
		}
	INFO_PRINTF1(_L("Kill() successfully returned ESRCH on negative test"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestSigqueue
// Test Case ID: OPENENV-LIBC-CIT-5998
// API tested: sigqueue()
// Description: Positive test to raise a signal using sigqueue() api
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestSigqueue (  )
	{   
	int ret, ret1 = KErrGeneral, status, Signum;
	pid_t pid, pid1;
	union sigval sival;
	char **argv=(char**)malloc(3*sizeof(char*));
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\sendsignal.exe");
	sprintf(argv[1], "%d", Signum);	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	User::After(50000); // TODO: Remove once the sending issue is fixed
	sival.sival_int = 0;
	ret = sigqueue(pid,Signum,sival);
	if(ret != 0)
		{
		ERR_PRINTF2( _L("Error in sending the signal to the spawned process and errno is set %d"),errno);
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	if(WEXITSTATUS(status) != 0)
		{
		ERR_PRINTF2(_L("sigqueue() failed in the child process for signal %d"),Signum);
		goto close;		
		}
	INFO_PRINTF2(_L("Successfully able to send a signal %d"),Signum);
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeSigqueue1 ( )
 // Test Case ID: OPENENV-LIBC-CIT-5999
// API tested: sigqueue()
// Description: Test case to send a signal with s =0 to a valid process using sigqueue().
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeSigqueue1 (  )
	{   
	int ret1 = KErrGeneral, pid, pid1, ret, status;
	union sigval sival;
	char **argv=(char**)malloc(3*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\receivesignal.exe");
	sprintf(argv[1], "%d", 0);	
	ret = posix_spawn(&pid, "z:\\sys\\bin\\receivesignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	sival.sival_int = 0;
	ret = sigqueue(pid,0,sival);
	INFO_PRINTF1(_L("Negative test on sigqueue()"));
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Failed to set the return value"));	
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}	
	INFO_PRINTF1(_L("Kill() returned 0 as the process exists"));
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeSigqueue2 ( )
// Test Case ID: OPENENV-LIBC-CIT-6000
// API tested: sigqueue()
// Description: Negative test: To send an invalid signal to other process using sigqueue()
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeSigqueue2 (  )
	{  
	int ret1 = KErrGeneral, pid, pid1, Signum, ret, status;
	union sigval sival;
	char **argv=(char**)malloc(3*sizeof(char*));
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close; 
		}
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\receivesignal.exe");
	ret = posix_spawn(&pid, "z:\\sys\\bin\\receivesignal.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2( _L("the value of pid returned by posix_spawn is %d"),  pid);
	sival.sival_int = 0;
	ret = sigqueue(pid,Signum,sival);
	INFO_PRINTF1(_L("Negative test on sigqueue()"));
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to set the return value"));	
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}	
	INFO_PRINTF1(_L("Both the return and expected value of kill() are same"));
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeSigqueue3 ( )
// Test Case ID: OPENENV-LIBC-CIT-6001,OPENENV-LIBC-CIT-6002,OPENENV-LIBC-CIT-6003
// API tested: sigqueue()
// Description: Negative test on sending a signal with an unsupported pid value using sigqueue()
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeSigqueue3 (  )
	{  
	int Maxsig, Pid, Signum, ret, ret1 = KErrGeneral, Sigval;
	union sigval sival;
	ret = GetIntFromConfig(ConfigSection(), _L("Pid"), Pid);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the Process id number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"), Maxsig);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the Maximum sig value")) ;
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigqueue()"));
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		sival.sival_int = 0;
		ret = sigqueue(Pid,Signum,sival);
		if((ret != -1) || (errno != ESRCH))
			{
			ERR_PRINTF2(_L("Failed to set the return value for signal %d"), Signum);	
			goto close;
			}
		}
	INFO_PRINTF1(_L("Negative test: Both the errno and expected value of kill() are same validated for all signals"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestNegativeSigqueue4 ( )
// Test Case ID: OPENENV-LIBC-CIT-6004,OPENENV-LIBC-CIT-6005,OPENENV-LIBC-CIT-6006
// API tested: sigqueue()
// Description: Negative test on sigqueue() for errno ESRCH
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestNegativeSigqueue4 (  )
	{  
	int Signum, ret, ret1 = KErrGeneral;
	union sigval sival;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	sival.sival_int = 0;
	ret = sigqueue(1000,Signum,sival);
	INFO_PRINTF1(_L("Negative test on sigqueue()"));
	if((ret != -1) || (errno != ESRCH))
		{
		ERR_PRINTF1(_L("Kill() failed to return ESRCH on negative test"));	
		goto close;
		}
	INFO_PRINTF1(_L("Kill() successfully returned ESRCH on negative test"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestsendsignal::TestSigKillwithTmpfile( )
//
// API tested: kill()
// Description: test for kill(). tmpfile() is called so that in exit() tmpfile()
//  			cleanup happens and opendir() is called.
// -----------------------------------------------------------------------------

TInt CTestSendsignal::TestSigKillwithTmpfile()
	{
	int ret = KErrGeneral, pid, pid1, Signum, status;
    ret = GetIntFromConfig(ConfigSection(),_L("Signum"), Signum);
    FILE* ff;
    if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number\n")) ;
		goto close;
		}
	ff = tmpfile();
	if(ff == 0)
		{
		ERR_PRINTF2(_L("Unable to create temporary file, errno=%d\n"), errno);
		}
	else
		{
		fclose(ff);
		}
	//Run a second instance of the current test process
	ret = posix_spawn(&pid, "z:\\sys\\bin\\tsendsignal.exe", NULL, NULL, NULL, NULL);  
	if(ret == -1)
		{
		ERR_PRINTF2(_L("posix_spawn() has failed with error=%d\n"), errno);	
		goto close;
		}
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed...\n"));	
		goto close;
		}
	else
		{
		//Send a SIGKILL signall
		ret = kill(pid, Signum);
		if(ret < 0)
			{
			ERR_PRINTF2(_L("kill() has failed with error=%d\n"), errno);
			goto close;
			}
		INFO_PRINTF1(_L("Kill() on self successfull\n"));
		}
	INFO_PRINTF1(_L("Test Case successfull\n"));
	close:
	return ret;
	}

//End of file
