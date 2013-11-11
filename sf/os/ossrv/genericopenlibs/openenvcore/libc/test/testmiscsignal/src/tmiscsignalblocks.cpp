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
// Name        : tmiscsignalblocks.cpp
// Test cases for signal api's
//




#include "tmiscsignal.h"

int value=0;
TTime time1,time2;
TTimeIntervalSeconds interval;

//To redirect the stderr output of psignal() to a file.
static int dupfd(int &fd,int &newfd)
	{
	int ret1 = KErrGeneral;
	unlink("c:\\psignal.txt");
	fd = open("c:\\psignal.txt" , O_CREAT | O_WRONLY , 0666);
	if(fd <= 0 )
		{
		printf("Failed to create and open file in C: drive \n");
		goto close;
		}
	newfd = dup2(fd,2);
	if(newfd != 2)
		{
		printf("Error in duping the file descriptor\n");
		goto close;
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

//Custom defined handler
void handler(int /*sig*/)
	{
	value++;
	}

//Custom defined handler for SIGALRM signal.
void alarmhandler(int /*sig*/)
	{
	time2.HomeTime();
	time2.SecondsFrom(time1,interval);
	value = interval.Int();
	}

//to empty the process's signal mask
static int emptysigset()
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set;
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		goto close;
		}
	ret = sigprocmask(SIG_SETMASK,&set,NULL);
	if (ret != 0)
		{
		goto close;
		}
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

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
// CTestmiscsignal::Testpsignal1
// Test Case ID: OPENENV-LIBC-CIT-5984
// API tested: psignal()
// Description: Positive test case to print the signal message of a valid signal
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testpsignal1 (  )
	{
	int ret, Signum, fd1, fd2=0, newfd, ret1 = KErrGeneral;
	TPtrC String,Expected;
	char buf[50] = {0};
	char* input;
	char* output;
	TBuf8<256> String1,Expected1;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the signal number from ini")) ;
		goto close;
		}
	ret = GetStringFromConfig(ConfigSection(), _L("String"), String);
	if(ret == 0)  
		{
	    ERR_PRINTF1(_L("Failed to read the second parameter of psignal() from ini")) ;
	    goto close;
		}
	String1.Copy(String);
	input = (char*) String1.Ptr();
	input[String1.Length()]='\0';
	ret = GetStringFromConfig(ConfigSection(), _L("Expected"), Expected);
	if(!ret)  
		{
		ERR_PRINTF1(_L("Failed to read the expected result from ini")) ;
		goto close;
		}
	Expected1.Copy(Expected);
	output = (char*) Expected1.Ptr();
	output[Expected1.Length()]='\0';
	ret = dupfd(fd1,newfd);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to dup the file"));
		goto close;
		}
	INFO_PRINTF1(_L("success in duping the file"));
	psignal(Signum,input);
	fd2 = open("c:\\psignal.txt" , O_RDONLY , 0666);
	if(fd2 <= 0)
		{
		ERR_PRINTF1(_L("Uable to open the file for reading"));
		goto close;
		}
	ret = read(fd2,buf,50);
	if (ret <=  0)
		{
		ERR_PRINTF1(_L("Unable to read the contents of file"));
		goto close;		
		}
	ret = strncmp(buf,output,Expected1.Length());
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Failure in validating psignal()"));
		goto close;
		}
	INFO_PRINTF1(_L("success in validating psignal()"));
	ret1 = KErrNone;

	close:
	close(fd1);
	close(fd2);
	close(newfd);
	unlink("c:\\psignal.txt");
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testpsignal2
// Test Case ID: OPENENV-LIBC-CIT-5985 
// API tested: psignal()
// Description: Positive test case to print the signal message with the second parameter as NULL
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testpsignal2 (  )
	{
	int ret, Signum, fd1, fd2=0, newfd, ret1 = KErrGeneral;
	TPtrC String,Expected;
	char buf[50] = {0};
	char* output;
	TBuf8<256> String1,Expected1;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the signal number from ini")) ;
		goto close;
		}
	ret = GetStringFromConfig(ConfigSection(), _L("Expected"), Expected);
	if(!ret)  
		{
		ERR_PRINTF1(_L("Failed to read the expected result from ini")) ;
		goto close;
		}
	Expected1.Copy(Expected);
	output = (char*) Expected1.Ptr();
	output[Expected1.Length()]='\0';
	ret = dupfd(fd1,newfd);
	if(ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to dup the file"));
		goto close;
		}
	INFO_PRINTF1(_L("success in duping the file"));
	psignal(Signum,NULL);
	fd2 = open("c:\\psignal.txt" , O_RDONLY , 0666);
	if(fd2 <= 0)
		{
		ERR_PRINTF1(_L("Uable to open the file for reading"));
		goto close;
		}
	ret = read(fd2,buf,50);
	if (ret <=  0)
		{
		ERR_PRINTF1(_L("Unable to read the contents of file"));
		goto close;		
		}
	ret = strncmp(buf,output,Expected1.Length());
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Failure in validating psignal()"));
		goto close;
		}
	INFO_PRINTF1(_L("success in validating psignal()"));
	ret1 = KErrNone;

	close:
	close(fd1);
	close(fd2);
	close(newfd);
	unlink("c:\\psignal.txt");
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testcombinesignal1
// Test Case ID: OPENENV-LIBC-CIT-5986,OPENENV-LIBC-CIT-5987
// API tested: raise()
// Description: To combine multiple pending signals (both RT and NRT signals) for delivery using raise() api
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testcombinesignal1 (  )
	{
	int ret, ret1 = KErrGeneral, Maxsig, Signum, Expected, Sigval;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Maximum signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Expected"), Expected);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Expected value from ini")) ;
		goto close;
		}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Failed in setting the default enviroment for signal %d"),Signum) ;
			goto close;
			}	
		ret = emptysigset();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Unable to empty the process's signal set for signal %d"),Signum) ;
			goto close;
			}		
 		value = 0;
		if (signal(Signum,handler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = sighold(Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Unable to block a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = raise(Signum);
 		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the first time and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = raise(Signum);
 		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the second time  and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = raise(Signum);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the third time and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = sigrelse(Signum);
		User::After(50000);
		if ((ret != 0) || (value != Expected))
			{
			ERR_PRINTF3(_L("Unable to unblock a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}		
		}	
	INFO_PRINTF1(_L("Each pending signal was called only once"));
	ret1 = KErrNone;	

	close:	
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testcombinesignal2
// Test Case ID: OPENENV-LIBC-CIT-5988,OPENENV-LIBC-CIT-5989
// API tested: kill()
// Description: To combine multiple pending signals (both RT and NRT signals) for delivery using kill() api
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testcombinesignal2 (  )
	{
	int ret, ret1 = KErrGeneral, Maxsig, Signum, Expected, Sigval;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Maximum signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Expected"), Expected);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Expected value from ini")) ;
		goto close;
		}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Failed in setting the default enviroment for signal %d"),Signum) ;
			goto close;
			}			
		ret = emptysigset();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Unable to empty the process's signal set for signal %d"),Signum) ;
			goto close;
			}		
 		value = 0;
		if (signal(Signum,handler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = sighold(Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Unable to block a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = kill(getpid(),Signum);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the first time and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = kill(getpid(),Signum);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the second time  and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = kill(getpid(),Signum);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the third time and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = sigrelse(Signum);
		User::After(50000);
		if ((ret != 0) || (value != Expected))
			{
			ERR_PRINTF3(_L("Unable to unblock a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}		
		}	
	INFO_PRINTF1(_L("Each pending signal was called only once"));
	ret1 = KErrNone;	

	close:	
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testcombinesignal3
// Test Case ID: OPENENV-LIBC-CIT-5990,OPENENV-LIBC-CIT-5991
// API tested: sigqueue()
// Description: To combine multiple pending signals (both RT and NRT signals) for delivery using sigqueue() api
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testcombinesignal3 (  )
	{
	int ret, ret1 = KErrGeneral, Maxsig, Signum, Expected, Sigval;
	union sigval sival;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Maximum signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Expected"), Expected);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Expected value from ini")) ;
		goto close;
		}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Failed in setting the default enviroment for signal %d"),Signum) ;
			goto close;
			}			
		ret = emptysigset();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Unable to empty the process's signal set for signal %d"),Signum) ;
			goto close;
			}		
 		value = 0;
		if (signal(Signum,handler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = sighold(Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Unable to block a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		sival.sival_int = 0;
		ret = sigqueue(getpid(),Signum,sival);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the first time and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = sigqueue(getpid(),Signum,sival);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the second time  and errno is %d"),Signum,errno) ;
			goto close;
			}	
		ret = sigqueue(getpid(),Signum,sival);
		User::After(50000);
 		if ((ret != 0) || (value != 0))
			{
			ERR_PRINTF3(_L("Unable to raise a signal %d for the third time and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = sigrelse(Signum);
		User::After(50000);
		if ((ret != 0) || (value != Expected))
			{
			ERR_PRINTF3(_L("Unable to unblock a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}		
		}	
	INFO_PRINTF1(_L("Each pending signal was called as the expected number of times using sigqueue()"));
	ret1 = KErrNone;	

	close:	
	return ret1;
	
	
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigchildsignal
// Test Case ID: OPENENV-LIBC-CIT-5992
// API tested: raise() in the child process
// Description: To generate SIGCHLD signal while raising SIGKILL to self in the child process
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigchildsignal (  )
	{
	int ret, ret1 = KErrGeneral, pid, pid1, status;
	char **argv=(char**)malloc(2*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]= 0;
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal ")) ;
		goto close;
		}	
	ret = emptysigset();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}
	if (signal(SIGCHLD,handler) == SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function")) ;
		goto close;
		}
	strcpy(argv[0], "z:\\sys\\bin\\killself.exe");
	value = 0;
	ret = posix_spawn(&pid, "z:\\sys\\bin\\killself.exe", NULL, NULL, argv, (char**)NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF2(_L("the value of pid returned by posix_spawn is %d"), pid);
	User::After(40000);
	pid1 = waitpid(pid, &status, WUNTRACED);
	if (pid1 != pid)
		{
		ERR_PRINTF1(_L("waitpid failed..."));	
		goto close;
		}
	User::After(20000);
	if(value != 1)
		{
		ERR_PRINTF1(_L("the child has failed to return SIGCHLD signal"));
		goto close;
		}
	INFO_PRINTF1(_L("Successfully the child has returned SIGCHLD signal"));
	ret1 = KErrNone;	

	close:	
	free((void*)argv[0]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigpipesignal
// Test Case ID: OPENENV-LIBC-CIT-5993
// API tested: pipe() to generate SIGPIPE signal
// Description: Test case to generate SIGPIPE signal in the child process.
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigpipesignal (  )
	{
	int ret, ret1 = KErrGeneral, fds[2];
	value = 0;	
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	if (signal(SIGPIPE,handler) == SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping")) ;
		goto close;
		}
	ret = pipe(fds);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a pipe and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Pipe is created successfully"));
	close(fds[0]);  // read end of a pipe is closed
	ret = write(fds[1],"sigpipe",10);
	if((ret != -1) && (errno != EPIPE))
		{
		ERR_PRINTF1(_L("Failed to generate the SIGPIPE signal on pipe"));
		goto close;
		}
	User::After(50000); // TODO: Remove once sending to self is synchronous
	INFO_PRINTF1(_L("writing to a pipe with read end closed has successfully returned EPIPE"));
	if(value != 1)
		{
		ERR_PRINTF1(_L("Failed to generate the SIGPIPE signal on pipe"));
		goto close;
		}
	INFO_PRINTF1(_L("SIGPIPE signal is generated while writing into a pipe with read end closed"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testinheritsignal
// Test Case ID: OPENENV-LIBC-CIT-5994
// API tested: sighold()
// Description: Child process inheriting the parents signal mask 
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testinheritsignal (  )
	{
	int ret, ret1 = KErrGeneral, pid, pid1, status, Maxsig, Signum, sig, Sigval;
	sigset_t set;
	char **argv=(char**)malloc(3*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(3*sizeof(char));
	argv[2]= 0;
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret = emptysigset();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the signal number from ini")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)  
		{
		ERR_PRINTF1(_L("Failed to read the Maximum signal number from ini")) ;
		goto close;
		}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = sighold(Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Unable to block a signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}	
		// to check the parent process's signal mask
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the process's signal mask for signal %d"),Signum) ;
			goto close;
			}
		ret = sigprocmask(SIG_SETMASK,NULL,&set);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask for signal %d"),Signum) ;
			goto close;
			}
		for(sig=1; sig <= Signum; sig++)
			{
			if((sig == SIGKILL) || (sig == SIGSTOP))
				{
				continue;
				}
			ret = sigismember(&set,sig);
			if (ret != 1)
				{
				ERR_PRINTF2(_L("Signal %d is not a member of the parent process signal mask"),Signum) ;
				goto close;
				}
			}
		// to check the child process's signal mask
		strcpy(argv[0], "z:\\sys\\bin\\inheritsignal.exe");
		sprintf(argv[1], "%d", Signum);	
		ret = posix_spawn(&pid, "z:\\sys\\bin\\inheritsignal.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		pid1 = waitpid(pid, &status, WUNTRACED);
		if (pid1!=pid)
			{
			ERR_PRINTF1(_L("waitpid failed..."));	
			goto close;
			}
		if (WEXITSTATUS(status) != 1)
			{
			ERR_PRINTF2(_L("The process's signal mask does not match for both the child and parent process for signal %d added"),Signum);	
			goto close;
			}
		}
	INFO_PRINTF1(_L("Both the child and parent process has same signal mask validated for each and every signal"));
	ret1 = KErrNone;
	
	close:
	emptysigset();
	free((void *)argv[0]);
	free((void *)argv[1]);
	free((void *)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigalrmsignal1
// Test Case ID: OPENENV-LIBC-CIT-5995
// API tested: alarm()
// Description: Test case to generate the SIGALRM signal  
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigalrmsignal1 (  )
	{
	int ret, ret1= KErrGeneral, Timeout,Expected;
	value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Timeout"), Timeout);
	if(ret == 0)  
	     {
	     ERR_PRINTF1(_L("Failed to read the timeout value from ini")) ;
	     goto close;
	     }
	ret = GetIntFromConfig(ConfigSection(), _L("Expected"), Expected);
	if(ret == 0)  
	     {
	     ERR_PRINTF1(_L("Failed to read the expected result from ini")) ;
	     goto close;
	     }	
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret = emptysigset();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}		
	if (signal(SIGALRM,alarmhandler) == SIG_ERR)
		{
		ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"),errno) ;
		goto close;
		}
	time1.HomeTime();
	ret = alarm(Timeout);
	sleep(5);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("alarm() has failed to returne 0")) ;
		goto close;		
		}
	INFO_PRINTF1(_L("alarm() has successfully returned 0")) ;
	if(value != Expected)
		{
		ERR_PRINTF1(_L("alarm() has failed to generate SIGALRM signal successfully")) ;
		goto close;
		}
	INFO_PRINTF1(_L("sigalrm signal has generated after Timeout seconds")) ;
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigalrmsignal2
// Test Case ID: OPENENV-LIBC-CIT-5996
// API tested: alarm()
// Description: To validate the return value's of alarm() api when called multiple times.  
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigalrmsignal2 (  )
	{
	int ret, ret1 = KErrGeneral, Timeout1,Timeout2,ReturnValue1,LapsedTime1,ReturnValue2,LapsedTime2;
	value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Timeout1"), Timeout1);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Failed to read the timeout value for the first alarm() called"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("ReturnValue1"), ReturnValue1);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the ReturnValue value for the first alarm() called"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("LapsedTime1"), LapsedTime1);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Failed to read the LapsedTime value for the first alarm() called"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Timeout2"), Timeout2);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the timeout value for the second alarm() called"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("ReturnValue2"), ReturnValue2);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the ReturnValue value for the second alarm() called"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("LapsedTime2"), LapsedTime2);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Failed to read the LapsedTime value for the second alarm() called"));
		goto close;
		}	
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret = emptysigset();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}	
	if (signal(SIGALRM,alarmhandler) == SIG_ERR)
		{
		ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"),errno) ;
		goto close;
		}
	value = 0;
	time1.HomeTime();
	ret = alarm(Timeout1);
	sleep(5);
	if(ret != ReturnValue1)
		{
		ERR_PRINTF3(_L("alarm() failed to return %d when called for the first time and errno is set %d"),ReturnValue1,errno) ;
		goto close;
		}
	if (value != LapsedTime1)
		{
		ERR_PRINTF2(_L("alarm() failed to return the lapsedtime as %d when called for the first time"),LapsedTime1) ;
		goto close;
		}
	value = 0;
	time1.HomeTime();
	ret = alarm(Timeout2);
	sleep(5);
	if(ret != ReturnValue2)
		{
		ERR_PRINTF3(_L("alarm() failed to return %d when called for the second time and errno is set to %d"),ReturnValue2,errno) ;
		goto close;
		}
	if(value != LapsedTime2)
		{
		ERR_PRINTF2(_L("alarm() failed to return the lapsedtime as %d when called for the second time"),LapsedTime2) ;
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to validate alarm() api")) ;
	ret1 = KErrNone;

	close:
	alarm(0);  //To clear any pending SIGALRM signal
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigalrmsignal3
// Test Case ID: OPENENV-LIBC-CIT-5997
// API tested: alarm()
// Description:  With sleep() in between the alarm() apis
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigalrmsignal3 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret = emptysigset();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}
	if (signal(SIGALRM,alarmhandler) == SIG_ERR)
		{
		ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"),errno) ;
		goto close;
		}
	value = 0;
	time1.HomeTime();
	ret = alarm(4);
	sleep(3);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("alarm() failed to return 0 when called for the first time")) ;
		goto close;
		}
	if (value != 0)
		{
		ERR_PRINTF2(_L("The expected value is not same when alarm() called for first time and value set is %d"),value) ;
		goto close;		
		}
	value = 0;
	time1.HomeTime();
	ret = alarm(5);
	sleep(3);
	if(ret != 1)
		{
		INFO_PRINTF1(_L("alarm() failed to return 1 when called for the second time")) ;
		goto close;		
		}
	if(value != 0)
		{
		ERR_PRINTF2(_L("The expected value is not same when alarm() called for second time and value set is %d"),value) ;
		goto close;			
		}
	value = 0;
	time1.HomeTime();
	ret = alarm(3);
	sleep(4);
	if(ret != 2)
		{
		ERR_PRINTF1(_L("alarm() failed to return 2 when called for the third time")) ;
		goto close;	
		}
	if(value != 3)
		{
		ERR_PRINTF2(_L("The expected value is not same when alarm() called for third time and value set is %d"),value) ;
		goto close;	
		}
	INFO_PRINTF1(_L("Successfully able to validate alarm() api")) ;
	ret1 = KErrNone;

	close:
	alarm(0);  //To clear any pending SIGALRM signal
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigalrmsignal4
// Test Case ID: OPENENV-LIBC-CIT-5998
// API tested: alarm()
// Description: A test case to validate that only one SIGALRM generation could be scheduled as alarm requests cannot be stacked.  
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigalrmsignal4 (  )
	{
	int ret = KErrGeneral, ret1, ret2, ret3, Timeout1, Timeout2, Timeout3, LapsedTime;
	ret1 = GetIntFromConfig(ConfigSection(), _L("Timeout1"), Timeout1);
	if(ret1 == 0)
		{
	 	ERR_PRINTF1(_L("Failed to read the timeout value for the first alarm() called"));
		goto close;	
		}
	ret1 = GetIntFromConfig(ConfigSection(), _L("Timeout2"), Timeout2);
	if(ret1 == 0)
		{
		ERR_PRINTF1(_L("Failed to read the timeout value for second alarm() called"));
		goto close;	
		}
	ret1 = GetIntFromConfig(ConfigSection(), _L("Timeout3"), Timeout3);
	if(ret1 == 0)
		{
		ERR_PRINTF1(_L("Failed to read the timeout value for third alarm() called"));
		goto close;	
		}
	ret1 = GetIntFromConfig(ConfigSection(), _L("LapsedTime"), LapsedTime);
	if(ret1 == 0)
		{
	 	ERR_PRINTF1(_L("Failed to read the LapsedTime value for the first alarm() called"));
		goto close;	
		}
	ret1 = setsigenv();
	if( ret1 == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret1 = emptysigset();
	if( ret1 == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}
	if (signal(SIGALRM,alarmhandler) == SIG_ERR)
		{
		ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"),errno) ;
		goto close;
		}
	value = 0;
	ret1 = alarm(Timeout1);
	User::After(50000);
	ret2 = alarm(Timeout2);
	User::After(50000);
	time1.HomeTime();
	ret3 = alarm(Timeout3);
	User::After(50000);
	sleep(5);
	if((ret1 != 0) || (ret2 != Timeout1) || (ret3 != Timeout2) || (value != LapsedTime))
		{
		ERR_PRINTF1(_L("alarm() has failed to generate SIGALRM signal successfully")) ;
		goto close;
		}
	INFO_PRINTF1(_L("Validated that only one SIGALRM generation could be scheduled as alarm requests cannot be stacked."));
	INFO_PRINTF1(_L("Successfully generated sigalarm signal"));
	ret = KErrNone;

	close:
	return ret;
	}

// -----------------------------------------------------------------------------
// CTestmiscsignal::Testsigalrmsignal5
// Test Case ID: OPENENV-LIBC-CIT-5999
// API tested: alarm()
// Description: Negative tes- alarm() with an invalid time.  
// -----------------------------------------------------------------------------

TInt CTestMiscsignal::Testsigalrmsignal5 (  )
	{
	int ret , ret1=KErrGeneral;
	ret1 = setsigenv();
	if( ret1 == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret = emptysigset();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal set")) ;
		goto close;
		}
	ret = alarm(unsigned(-3));
	if(ret != 0)
		{
		ERR_PRINTF1(_L("alarm() unable to return 0 on negative test")) ;
		goto close;
		}	
	INFO_PRINTF1(_L("alarm() successfully able to return 0 on negative test"));
	ret1 = KErrNone;

	close:
	return ret1;	
	}

//End of file
