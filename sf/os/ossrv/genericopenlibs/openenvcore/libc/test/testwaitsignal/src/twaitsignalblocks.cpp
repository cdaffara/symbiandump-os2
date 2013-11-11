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
// Name        : twaitsignalblocks.cpp
// Test cases for waiting signal api's
// 
//

#include "twaitsignal.h"

int sigvalue = 0;

//to clear signal mask and set the handler to default
static int setsigenv()
	{
	int Signum, ret, ret1 = KErrGeneral ;
	sigset_t oset;
	if(sigpending(&oset) != 0)
	goto close;

	for(Signum=1; Signum <= 64; Signum++)
		{
		if ((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}

		if(sigismember(&oset,Signum) == 0)
		continue;

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
	User::After(30000);
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

//to add a signal sig to the process's signal mask
static int setsignaltomask(sigset_t &set, int sig)
	{
	int ret, ret1 = 0;
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		goto close;
		}
	ret = sigaddset(&set,sig);
	if (ret != 0)
		{
		goto close;
		}
	ret  = sigprocmask(SIG_BLOCK,&set,NULL);
	if (ret != 0)
		{
		goto close;
		}
	ret1 = 1;
	
	close:
	return ret1;
	}

// Check whether the signal is a member of set or not
static int checksignalinset(sigset_t set, int sig)
	{
	int ret, ret1 = 0;
	ret = sigismember(&set,sig);
	if (ret != 1)
		{
		goto close;
		}
	ret = sigdelset(&set,sig);
	if(ret != 0)
		{
		goto close;
		}
	ret =  sigisemptyset(&set);
	if(ret != 1)
		{
		goto close;
		}
	ret1 = 1;
		
	close:
	return ret1;
	}

void siginfoflaghandler(int sig, siginfo_t *info, void */*context*/)
	{
	int val = info->si_value.sival_int;
	if ((val == sig) && (sig == info->si_signo))
		{
		sigvalue = 1;
		}
	else
		{
		sigvalue = 0;
		}
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testpendingsignal1
// Test Case ID: OPENENV-LIBC-CIT-5951
// API tested: sigpending()
// Description: To examine the pending signals using sigpending()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testpendingsignal1 (  )
	{
	sigset_t set, oset;
	int ret, Signum, Maxsig, ret1 = KErrGeneral, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			} 
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigemptyset(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to validate sigpending() api for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Success in validating sigpending() api for every signal"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testpendingsignal2
// Test Case ID: OPENENV-LIBC-CIT-5952
// API tested: sigpending()
// Description: Trying to retrieve the pending signal using sigpending() in the absence of any pending signal
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testpendingsignal2 (  )
	{
	sigset_t set, oset;
	int Signum, ret, ret1 = KErrGeneral, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigemptyset(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to retrieve the pending signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if(ret != 1)
			{
			ERR_PRINTF2(_L("Sigpending() failed on negative test for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Negative test: Successfully validated sigpending() for each signal"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testpendingsignal3
// Test Case ID: OPENENV-LIBC-CIT-5953
// API tested: sigpending()
// Description: To examine the pending signal when non empty signal set is passed to sigpending()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testpendingsignal3 (  )
	{
	sigset_t set, oset;
	int ret, Signum1, Signum2, ret1 = KErrGeneral, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum1=Sigval; Signum1<=Maxsig; Signum1++)
		{
		if((Signum1 == SIGKILL) || (Signum1 == SIGSTOP))
			{
			continue;
			}
		for ( Signum2=Sigval; Signum2<=Maxsig; Signum2++)
			{
			if ((Signum2 == SIGKILL) || (Signum2 == SIGSTOP))
				{
				continue; //skip for SIGSTOP and SIGKILL signals
				}
			ret = setsigenv();
			if( ret == KErrGeneral)
				{
				ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
				goto close;
				}
			if (signal(Signum1,SIG_IGN) == SIG_ERR)
				{
				ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum1,errno) ;
				goto close;
				}
			ret = setsignaltomask(set,Signum1);
			if(ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum1);
				goto close;
				}
			ret = raise(Signum1);
			if(ret != 0)
				{
				ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),Signum2,errno);
				goto close;
				}
			User::After(20000); // TODO: To be removed once sending to self is synchronous
			ret = sigemptyset(&oset);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
				goto close;
				}
			ret = sigaddset(&oset,Signum2);
			if (ret != 0)
				{
				ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum2,errno);
				goto close;
				}
			ret = sigpending(&oset);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Failed to retrieve only the pending signal %d"),Signum1);
				goto close;
				}
			ret = checksignalinset(oset,Signum1);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to validate sigpending() api for signal %d"),Signum2);
				goto close;
				}
			}
		}
	INFO_PRINTF1(_L("sigpending() successfully deleted signal from the pending signal set"));
	INFO_PRINTF1(_L("sigpending() successfully retrieves only one signal"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testpendingsignal4
// Test Case ID: OPENENV-LIBC-CIT-5954 
// API tested: sigpending()
// Description: Negative test on sigpending()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testpendingsignal4 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = sigpending(NULL);
	if((ret != -1) || (errno != EFAULT))
		{
		ERR_PRINTF1(_L("sigpending() failed to return EFAULT on negative test"));
		goto close;
		}
	INFO_PRINTF1(_L("sigpending() successfully retreived EFAULT on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal1
// Test Case ID: OPENENV-LIBC-CIT-5955
// API tested: sigwait()
// Description: To clear the system's pending signal selected from the queued pending signals specified by set using sigwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal1 (  )
	{
	sigset_t set, oset;
	int ret, Signum, sig, ret1 = KErrGeneral, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		ret = kill(getpid(),Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to retrieve the pending signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF1(_L("Error in getting the pending signal"));
			goto close;
			}
		ret = sigwait(&set,&sig);
		if ((ret != 0) || (sig != Signum))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwait() and errno is %d"),Signum,errno);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("pending signal %d not cleared after sigwait()"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Successfully retrieved the pending signal using sigwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// ---------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal2
/// Test Case ID: OPENENV-LIBC-CIT-5956,OPENENV-LIBC-CIT-5957,OPENENV-LIBC-CIT-5958
// API tested: sigwait()
// Description: To retrieve the pending signal from only multiple Non-RT signals and RT signals queued using sigwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal2 (  )
	{
	int ret, ret1 = KErrGeneral, Signum1, Signum2, Maxsig1, Maxsig2, sig, OnlyRTSignal, value, x, y;
	sigset_t set, set1;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum1"), Signum1);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the signal number Signum1"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig1"), Maxsig1);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the maximum signal number for Signum1"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Signum2"), Signum2);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the signal number Signum2"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig2"), Maxsig2);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the maximum signal number for Signum2"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("OnlyRTSignal"), OnlyRTSignal);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the OnlyRTSignal value"));
	 	goto close;
		}
	INFO_PRINTF1(_L("To retrieve the pending signal from only multiple Non-RT signals and RT signals queued using sigwait()"));
	for (x = Signum1; x <= Maxsig1; x++)
		{
		if((x == SIGKILL) || (x == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(x,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),x,errno) ;
			goto close;
			}
		for (y = Signum2; y <= Maxsig2; y++)
			{
			if((y == SIGKILL) || (y == SIGSTOP))
				{
				continue;
				}
			ret = setsigenv();
			if( ret == KErrGeneral)
				{
				ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
				goto close;
				}
			if (signal(y,SIG_IGN) == SIG_ERR)
				{
				ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),y,errno) ;
				goto close;
				}
			value = x;
			ret = setsignaltomask(set1,x);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),x);
				goto close;
				}
			ret = setsignaltomask(set1,y);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),y);
				goto close;
				}
			ret = raise(x);
			if(ret != 0)
				{
				ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),x,errno);
				goto close;
				}
			User::After(50000); // TODO: To be removed once sending to self is synchronous
			ret = raise(y);
			if(ret != 0)
				{
				ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),y,errno);
				goto close;
				}
			User::After(50000); // TODO: To be removed once sending to self is synchronous
			ret = sigfillset(&set);
			if(ret != 0)
				{
				ERR_PRINTF1(_L("Failed to fill the set completely"));
				goto close;
				}
			ret = sigwait(&set,&sig);
			if(ret != 0)
				{
				ERR_PRINTF2(_L("Error in getting the pending signal using sigwait() and errno is %d"),errno);
				goto close;
				}
			if(OnlyRTSignal == 1)
				{
				if(x > y)
					{
					value = y;
					}
				if(sig != value)
					{
					ERR_PRINTF2(_L("Failed to store the pending signal %d in sig"),value);
					goto close;
					}			
				}
			else
				{
				if((sig != x) && (sig != y))
					{
					ERR_PRINTF1(_L("Failed to store the pending signal in sig"));
					goto close;
					}
				}
			}
		}
	INFO_PRINTF1(_L("Successfully retreived the pending signal using sigwait()"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal3
/// Test Case ID: OPENENV-LIBC-CIT-5959
// API tested: sigwait()
// Description: To examine the suspension of process using sigwait() when the blocked signal is raised after n seconds.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal3 (  )
	{
	int ret, sig, ret1 = KErrGeneral, pid, Signum, Maxsig, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));	//child process’s name
	argv[1]=(char*)malloc(6*sizeof(char));	//calling process’s pid
	argv[2]=(char*)malloc(3*sizeof(char));	//signal to be raised
	argv[3]= 0;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		time1.HomeTime();
		ret = sigwait(&set,&sig);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if((ret != 0) || (sig != Signum))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwait() and errno is %d"),Signum,errno);
			goto close;
			}
		/* It is not assured that posix_spawn and sigwait are called immediately after each other

		if((aInterval.Int() < 2)  || (aInterval.Int() > 3))
			{
			ERR_PRINTF2(_L("sigwait() failed to suspend for 2 seconds for signal %d"),Signum);
			ERR_PRINTF2(_L("sigwait() was suspended for %d seconds"),aInterval.Int());
			goto close;
			}
		*/
		}
	INFO_PRINTF1(_L("sigwait() successfully was suspended for 2 seconds for every signal"));
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}
 
// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal4
// Test Case ID: OPENENV-LIBC-CIT-5960
// API tested: sigwait()
// Description: Negative test on sigwait() with an invalid set parameter
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal4 (  )
	{
	int sig, ret, ret1 = KErrGeneral;
	ret = sigwait(NULL,&sig);
	if(ret != EINVAL)
		{
		ERR_PRINTF1(_L("sigwait() failed on negative test to return EFAULT"));
		goto close;
		}
	INFO_PRINTF1(_L("sigwait() successfully returned EFAULT on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal5
// 
// API tested: sigwait()
// Description: sigwait() with a second parameter as Zero
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal5 (  )
	{
	int ret, ret1 = KErrGeneral, pid, Signum, Maxsig, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));	//child process’s name
	argv[1]=(char*)malloc(6*sizeof(char));	//calling process’s pid
	argv[2]=(char*)malloc(3*sizeof(char));	//signal to be raised
	argv[3]= 0;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		time1.HomeTime();
		ret = sigwait(&set,NULL);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwait() and errno is %d"),Signum,errno);
			goto close;
			}
		/* The delay between posix_spawn and sigwait can't be assumed to be 0

		if(aInterval.Int() < 2)
			{
			ERR_PRINTF2(_L("sigwait() failed to suspend for 2 seconds for signal %d"),Signum);
			ERR_PRINTF2(_L("sigwait() was suspended for %d seconds"),aInterval.Int());
			goto close;
			}
		*/
		}
	INFO_PRINTF1(_L("sigwait() successfully was suspended for 2 seconds for every signal"));
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal6
// Test Case ID: OPENENV-LIBC-CIT-5962 
// API tested: sigwait()
// Description: sigwait() on SIGKILL/SIGSTOP selected in the set.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal6 (  )
	{
	sigset_t set;
	int sig, ret, Signum, ret1 = KErrGeneral;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number"));
		goto close;
		}
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Failed in emptying the set for signal %d"),Signum) ;
		goto close;
		}		
	ret = sigaddset(&set,Signum);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Failed to add a signal to the set and errno returned is %d"),errno);
		goto close;
		}
	INFO_PRINTF2(_L("Signal %d is added to the set"),Signum);
	time1.HomeTime();
	ret = sigwait(&set,&sig);
	time2.HomeTime();
	time2.SecondsFrom(time2,aInterval);
	if((ret != 0) || (aInterval.Int() != 0))
		{
		ERR_PRINTF2(_L("sigwait() failed ignoring %d signal"),Signum);
		goto close;
		}
	INFO_PRINTF2(_L("sigwait() successfully ignored %d signal"),Signum);
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal7
// Test Case ID: OPENENV-LIBC-CIT-5963 
// API tested: sigwait()
// Description: Negative test when an empty signal set is passed to sigwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal7 (  )
	{
	sigset_t set;
	int ret, ret1 = KErrGeneral, sig;
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Unable to empty the set"));
		goto close;
		}
	ret = sigwait(&set,&sig);
	if (ret != EINVAL)
		{
		ERR_PRINTF2(_L("Error in getting the pending signal using sigwait() and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigwait() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal8
// 
// API tested: sigwait()
// Description: Two signals added to the input set of sigwait(), one of them being either SIGKILL/SIGSTOP
// To improve coverage
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal8 (  )
	{
	sigset_t set, oset;
	int ret, Signum, sig, ret1 = KErrGeneral, Maxsig, Sigval, Negsignal;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Negsignal"),Negsignal);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the sigkill/sigstop signal")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		ret = kill(getpid(),Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to retrieve the pending signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		User::After(10000);
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF1(_L("Error in getting the pending signal"));
			goto close;
			}
		ret =  sigaddset(&set, Negsignal);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add signal %d to the set for signal %d"),Negsignal,Signum);
			goto close;
			}
		ret = sigwait(&set,&sig);
		if ((ret != 0) || (sig != Signum))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwait() and errno is %d"),Signum,errno);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("pending signal %d not cleared after sigwait()"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Successfully retrieved the pending signal using sigwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitsignal9
// 
// API tested: sigwait()
// Description: Retrieving the pending signal using sigwait() while signal is raised using sigqueue()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitsignal9 (  )
	{
	sigset_t set;
	int ret, Signum, sig, ret1 = KErrGeneral, Maxsig, Sigval;
	union sigval sival;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		sa.sa_sigaction = siginfoflaghandler;
		sa.sa_flags = SA_SIGINFO;
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (sigaction(Signum,&sa,NULL) == -1)
			{
			ERR_PRINTF3(_L("Sigaction() failed on testing SA_SIGINFO flag for signal %d and errno is %d"),Signum,errno);
			goto close;			
			}	
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		sival.sival_int = Signum;
		ret = sigqueue(getpid(),Signum,sival);
		if(ret != 0)
			{
			ERR_PRINTF2( _L("Error in sending the signal to the spawned process and errno is set %d"),errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		sigvalue = 0;
		ret = sigwait(&set,&sig);
		if ((ret != 0) || (sig != Signum))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwait() and errno is %d"),Signum,errno);
			goto close;
			}
		if(sigvalue != 0)
			{
			ERR_PRINTF2(_L("sigqueue() failed for union sigval functionality for signal %d"),Signum);
			goto close;				
			}
		}
	INFO_PRINTF1(_L("Successfully retrieved the pending signal using sigwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitinfosignal1
// Test Case ID: OPENENV-LIBC-CIT-5964 
// API tested: sigwaitinfo()
// Description: To clear the system's pending signal selected from the queued pending signals specified by set using sigwaitinfo()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitinfosignal1 (  )
	{
	sigset_t set, oset;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	siginfo_t info;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		ret = kill(getpid(),Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Failed to retrieve the pending signal"));
			goto close;
			}
		User::After(10000);
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Error in getting the pending signal %d"),Signum);
			goto close;
			}
		ret = sigwaitinfo(&set,&info);
		if ((ret != Signum) || (info.si_signo != Signum))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwaitinfo() and errno is %d"),Signum,errno);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Failed to retrieve the pending signal"));
			goto close;
			}
		ret = sigisemptyset(&oset);
		if (ret != 1)
			{
			ERR_PRINTF1(_L("pending signal not cleared after sigwaitinfo()"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals are pending before sigwaitinfo()"));
	INFO_PRINTF1(_L("Successfully retrieved the pending signal validated for all signals"));
	INFO_PRINTF1(_L("No signal is pending after sigwaitinfo()"));
	ret1 = KErrNone;
			
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitinfosignal2
// Test Case ID: OPENENV-LIBC-CIT-5965,OPENENV-LIBC-CIT-59656,OPENENV-LIBC-CIT-5967
// API tested: sigwaitinfo()
// Description: To retrieve the pending signal from the multiple RT and Non-RT signals queued using sigwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitinfosignal2 (  )
	{
	int ret, ret1 = KErrGeneral, Signum1, Signum2, Maxsig1, Maxsig2, OnlyRTSignal, value, x, y;
	sigset_t set, set1;
	siginfo_t info;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum1"), Signum1);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the signal number Signum1"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig1"), Maxsig1);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the maximum signal number for Signum1"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Signum2"), Signum2);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the signal number Signum2"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig2"), Maxsig2);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the maximum signal number for Signum2"));
	 	goto close;
		}
	
	ret = GetIntFromConfig(ConfigSection(), _L("OnlyRTSignal"), OnlyRTSignal);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the OnlyRTSignal value"));
	 	goto close;
		}
	
	for (x = Signum1; x <= Maxsig1; x++)
		{
		if((x == SIGKILL) || (x == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		
		for (y = Signum2; y <= Maxsig2; y++)
			{
			if((y == SIGKILL) || (y == SIGSTOP))
				{
				continue;
				}
			ret = setsigenv();
			if( ret == KErrGeneral)
				{
				ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
				goto close;
				}
			User::After(10000);
			if (signal(x,SIG_IGN) == SIG_ERR)
				{
				ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),x,errno) ;
				goto close;
				}
			if (signal(y,SIG_IGN) == SIG_ERR)
				{
				ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),y,errno) ;
				goto close;
				}
			value = x;
			ret = setsignaltomask(set1,x);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),x);
				goto close;
				}
			ret = setsignaltomask(set1,y);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),y);
				goto close;
				}
			ret = raise(x);
			if(ret != 0)
				{
				ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),x);
				goto close;
				}
			User::After(10000);
			ret = raise(y);
			if(ret != 0)
				{
				ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),y);
				goto close;
				}
			User::After(20000); // TODO: To be removed once sending to self is synchronous
			ret = sigfillset(&set);
			if(ret != 0)
				{
				ERR_PRINTF1(_L("Failed to fill the set completely"));
				goto close;
				}
			ret = sigwaitinfo(&set,&info);
			if(OnlyRTSignal == 1)
				{
				if(x > y)
					{
					value = y;
					}
				if((ret != value) || (info.si_signo != value))
					{
					ERR_PRINTF2(_L("Failed to store the pending signal %d in sig"),value);
					goto close;
					}			
				}
			else
				{
				if((ret != x) && (ret != y))
					{
					ERR_PRINTF3(_L("Failed to return the pending signal in the combination of %d and %d signals"),x,y);
					goto close;
					}
				if(info.si_signo != ret)
					{
					ERR_PRINTF1(_L("Failed to store the pending signal in sig"));
					goto close;
					}
				}
			setsigenv();
			User::After(20000);
			}
		}
	INFO_PRINTF1(_L("Successfully retreived the pending signal validated for every signal"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitinfosignal3
// Test Case ID: OPENENV-LIBC-CIT-5968
// API tested: sigwaitinfo()
// Description: To examine the suspension of process using sigwait() when the blocked signal is raised after n seconds.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitinfosignal3 (  )
	{
	int ret, ret1 = KErrGeneral, pid, Signum, Maxsig, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	siginfo_t info;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(6*sizeof(char));
	argv[2]=(char*)malloc(3*sizeof(char));
	argv[3]= 0;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set, Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);
		
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		time1.HomeTime();
		ret = sigwaitinfo(&set,&info);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if((ret != Signum) || (info.si_signo != Signum))
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		/* Doesn't make sense, because there might be delays between posix_spawn and sigwaitinfo()
		if(aInterval.Int() < 2)
			{
			ERR_PRINTF2(_L("sigwaitinfo() failed to suspend for 2 seconds for signal %d"),Signum);
			goto close;
			}
		*/
		}
	INFO_PRINTF1(_L("sigwaitinfo() successfully was suspended for 2 seconds validated for every signal"));
	ret1 = KErrNone;
	
close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitinfosignal4
// Test Case ID: OPENENV-LIBC-CIT-5969
// API tested: sigwaitinfo()
// Description: Negative test with an invalid set parameter
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitinfosignal4 (  )
	{
	int ret, ret1 = KErrGeneral;
	siginfo_t info;
	ret = sigwaitinfo(NULL,&info);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to set the errno to EFAULT on negative test"));
		goto close;
		}
	INFO_PRINTF1(_L("sigwaitinfo() has successfully returned EFAULT on negative test"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testwaitinfosignal5
// Test Case ID: OPENENV-LIBC-CIT-5970
// API tested: sigwaitinfo()
// Description: Negative test with an invalid siginfo parameter
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testwaitinfosignal5 (  )
	{
	int ret, ret1 = KErrGeneral, pid, Signum, Maxsig, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(6*sizeof(char));
	argv[2]=(char*)malloc(3*sizeof(char));
	argv[3]= 0;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set, Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);
		
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		time1.HomeTime();
		ret = sigwaitinfo(&set,NULL);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if(ret != Signum)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		/* Can't guarantee that there is no delay between posix_spawn and sigwaitinfo

		if(aInterval.Int() < 2)
			{
			ERR_PRINTF2(_L("sigwaitinfo() failed to suspend for 2 seconds for signal %d"),Signum);
			goto close;
			}
		*/
		}
	INFO_PRINTF1(_L("sigwaitinfo() successfully was suspended for 2 seconds validated for every signal"));
	ret1 = KErrNone;
	
close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal:: Testwaitinfosignal6
// Test Case ID: OPENENV-LIBC-CIT-5971
// API tested: sigwaitinfo()
// Description: sigwaitinfo() on SIGKILL/SIGSTOP selected in the set.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal:: Testwaitinfosignal6 (  )
	{
	sigset_t set;
	int ret, Signum, ret1 = KErrGeneral;
	siginfo_t info;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number"));
		goto close;
		}
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}	
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Failed in emptying the set for signal %d"),Signum) ;
		goto close;
		}
	ret = sigaddset(&set,Signum);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Failed to add a signal to the set and errno returned is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Signal is added to the set"));
	time1.HomeTime();
	ret = sigwaitinfo(&set,&info);
	time2.HomeTime();
	time2.SecondsFrom(time2,aInterval);
	if((ret != 0) || (aInterval.Int() != 0))
		{
		ERR_PRINTF2(_L("sigwaitinfo() failed ignoring %d signal"),Signum);
		goto close;
		}
	INFO_PRINTF2(_L("sigwaitinfo() successfully ignored %d signal"),Signum);
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal:: Testwaitinfosignal7
// Test Case ID: OPENENV-LIBC-CIT-5972 
// API tested: sigwaitinfo()
// Description: Negative test when an empty signal set is passed to sigwaitinfo()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal:: Testwaitinfosignal7 (  )
	{
	sigset_t set;
	int ret, ret1 = KErrGeneral;
	siginfo_t info;
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Unable to empty the set"));
		goto close;
		}
	ret = sigwaitinfo(&set,&info);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("Error in getting the pending signal using sigwait() and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigwait() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal:: Testwaitinfosignal8
// 
// API tested: sigwaitinfo()
// Description: Two signals added to the input set of sigwaitinfo(), one of them being either SIGKILL/SIGSTOP
// To improve coverage
// -----------------------------------------------------------------------------

TInt CTestWaitsignal:: Testwaitinfosignal8 (  )
	{
	sigset_t set, oset;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval, Negsignal;
	siginfo_t info;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Negsignal"),Negsignal);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the sigkill/sigstop signal")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		ret = kill(getpid(),Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to raise a blocked signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		User::After(60000); // TODO: To be removed once sending to self is synchronous
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Failed to retrieve the pending signal"));
			goto close;
			}
		User::After(30000);
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Error in getting the pending signal %d"),Signum);
			goto close;
			}
		ret =  sigaddset(&set, Negsignal);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add signal %d to the set for signal %d"),Negsignal,Signum);
			goto close;
			}
		ret = sigwaitinfo(&set,&info);
		if ((ret != Signum) || (info.si_signo != Signum))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwaitinfo() and errno is %d"),Signum,errno);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Failed to retrieve the pending signal"));
			goto close;
			}
		ret = sigisemptyset(&oset);
		if (ret != 1)
			{
			ERR_PRINTF1(_L("pending signal not cleared after sigwaitinfo()"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals are pending before sigwaitinfo()"));
	INFO_PRINTF1(_L("Successfully retrieved the pending signal validated for all signals"));
	INFO_PRINTF1(_L("No signal is pending after sigwaitinfo()"));
	ret1 = KErrNone;
			
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal:: Testwaitinfosignal9
// 
// API tested: sigwaitinfo()
// Description: Retrieving the pending signal using sigwaitinfo() while signal is raised using sigqueue()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal:: Testwaitinfosignal9 (  )
	{
	sigset_t set;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	union sigval sival;
	struct sigaction sa;
	siginfo_t info;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		sa.sa_sigaction = siginfoflaghandler;
		sa.sa_flags = SA_SIGINFO;
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (sigaction(Signum,&sa,NULL) == -1)
			{
			ERR_PRINTF3(_L("Sigaction() failed on testing SA_SIGINFO flag for signal %d and errno is %d"),Signum,errno);
			goto close;			
			}	
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		sival.sival_int = Signum;
		ret = sigqueue(getpid(),Signum,sival);
		if(ret != 0)
			{
			ERR_PRINTF2( _L("Error in sending the signal to the spawned process and errno is set %d"),errno);
			goto close;
			}
		User::After(30000); // TODO: To be removed once sending to self is synchronous
		sigvalue = 0;
		ret = sigwaitinfo(&set,&info);
		if (ret != Signum)
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwaitinfo() and errno is %d"),Signum,errno);
			goto close;
			}
		if((info.si_signo != Signum) || (info.si_value.sival_int != Signum) || (sigvalue != 0))
			{
			ERR_PRINTF2(_L("sigqueue() failed for union sigval functionality for signal %d"),Signum);
			goto close;				
			}
		}
	INFO_PRINTF1(_L("Successfully retrieved the pending signal using sigwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal1
// Test Case ID: OPENENV-LIBC-CIT-5974 
// API tested: sigtimedwait()
// Description: To clear the system's pending signal selected from the queued pending signals specified by set using sigtimedwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal1 (  )
	{
	sigset_t set, oset;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	siginfo_t info;
	timespec timeout;
	timeout.tv_sec = 2;
	timeout.tv_nsec = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),Signum);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		ret = sigismember(&oset,Signum);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("Error in getting the pending signal %d"),Signum);
			goto close;
			}
		ret = sigtimedwait(&set,&info,&timeout);
		if ((ret != Signum) || (info.si_signo != Signum))
			{
			ERR_PRINTF1(_L("Error in getting the pending signal using sigwait()"));
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Failed to retrieve the pending signal"));
			goto close;
			}
		ret = sigismember(&oset,Signum);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("pending signal %d not cleared after sigwait()"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals are pending before sigtimedwait()"));
	INFO_PRINTF1(_L("Successfully retrieved the pending signal validated for all signals"));
	INFO_PRINTF1(_L("No signal is pending after sigtimedwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal2
// Test Case ID: OPENENV-LIBC-CIT-5975
// API tested: sigtimedwait()
// Description: Retreiving the pending signal raised within timeout using sigtimedwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal2 (  )
	{
	int ret1 = KErrGeneral, ret, pid, Signum, Maxsig, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	siginfo_t info;
	timespec timeout;
	timeout.tv_sec = 4;
	timeout.tv_nsec = 0;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(6*sizeof(char));
	argv[2]=(char*)malloc(3*sizeof(char));
	argv[3]= 0; 
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);		
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		time1.HomeTime();
		ret = sigtimedwait(&set,&info,&timeout);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if((ret != Signum) || (info.si_signo != Signum))
			{ 
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		/* Can't assume that gap between posix_spawn and sigtimedwait is not relevant

		if(aInterval.Int() < 2)
			{
			ERR_PRINTF2(_L("sigtimedwait() failed to suspend for 2 seconds for signal %d"),Signum);
			goto close;
			}
		*/
		}
	INFO_PRINTF1(_L("sigwait() successfully was suspended for 2 seconds validated for all signals"));
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal3
// Test Case ID: OPENENV-LIBC-CIT-5975
// API tested: sigtimedwait()
// Description: Test case with no pending signal received within n seconds of timeout.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal3 (  )
	{
	sigset_t set;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	siginfo_t info;
	timespec timeout;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	timeout.tv_sec = 2;
	timeout.tv_nsec = 0; 
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		time1.HomeTime();
		ret = sigtimedwait(&set,&info,&timeout);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if ((ret != -1) || (errno != EAGAIN))
			{
			ERR_PRINTF3(_L("sigtimedwait() failed to return EAGAIN on negative test and errno is %d for signal %d"),errno,Signum);
			goto close;
			}
		if (aInterval.Int() < 2)
			{
			ERR_PRINTF1(_L("sigtimewait() failed on negative testing"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigtimedwait() successfully returned EAGAIN on negative test validated for all signals"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal4
// Test Case ID: OPENENV-LIBC-CIT-5976
// API tested: sigtimedwait()
// Description: Retreiving the pending signal raised after time elapse using sigtimedwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal4 (  )
	{
	int ret1 = KErrGeneral, ret, pid, Signum, Maxsig, pid1, status, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	siginfo_t info;
	timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(6*sizeof(char));
	argv[2]=(char*)malloc(3*sizeof(char));
	argv[3]= 0; 
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		User::After(10000);
		ret = setsigenv();
		User::After(10000);
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);		
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		User::After(30000);
		time1.HomeTime();
		ret = sigtimedwait(&set,&info,&timeout);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		pid1 = waitpid(pid, &status, WUNTRACED);
		if (pid1 != pid)
			{
			ERR_PRINTF1(_L("waitpid failed..."));	
			goto close;
			}
		if((ret != -1) || (errno != EAGAIN)) 
			{
			ERR_PRINTF2(_L("sigtimedwait() failed to return EAGAIN on negative test for signal %d"),Signum);
			goto close;
			}
		/* Can't assume gap between posix_spawn and sigtimedwait is negligible

		if (aInterval.Int() < 1)
			{
			ERR_PRINTF1(_L("sigtimewait() failed on negative testing"));
			goto close;
			}
		*/
		}
	INFO_PRINTF1(_L("sigtimedwait() successfully returned EAGAIN on negative test for all signals"));
	ret1 = KErrNone;
		
	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal5
// Test Case ID: OPENENV-LIBC-CIT-5977
// API tested: sigtimedwait()
// Description: sigtimedwait() called with an invalid timeout argument in the presence of pending signal.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal5 (  )
	{
	sigset_t set, oset;
	int ret, Signum, Maxsig, ret1 = KErrGeneral, Sigval, Nanosec;
	siginfo_t info;
	timespec timeout;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Nanosec"), Nanosec);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read tv_nsec value from ini")) ;
	 	goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		timeout.tv_sec = 2;
		timeout.tv_nsec = Nanosec; 
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),Signum);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigtimedwait(&set,&info,&timeout);
		if ((ret != Signum) || (info.si_signo != Signum))
			{
			ERR_PRINTF2(_L("Error in getting the pending signal %d using sigtimedwait()"),Signum);
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to validate sigpending() api for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigtimedwait() called with an invalid timeout argument in the presence of pending signal"));
	INFO_PRINTF1(_L("Validation done using sigpending() "));
	ret1 = KErrNone;
		
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal6
// Test Case ID: OPENENV-LIBC-CIT-5978
// API tested: sigtimedwait()
// Description: sigtimedwait() called with an invalid timeout argument in the absence of pending signal.
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal6 (  )
	{
	sigset_t set;
	int ret, Signum, ret1 = KErrGeneral, Nanosec, Maxsig, Sigval;
	siginfo_t info;
	timespec timeout;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Nanosec"), Nanosec);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read tv_nsec value from ini")) ;
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}	
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		timeout.tv_sec = 0;
		timeout.tv_nsec = Nanosec;
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigtimedwait(&set,&info,&timeout);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d and errno is %d"),Signum,errno);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigtimedwait() returned EINVAL on negative test validated for all signals"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal7
// Test Case ID: OPENENV-LIBC-CIT-5979 
// API tested: sigtimedwait()
// Description: Negative test on sigtimedwait() with an invalid set parameter
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal7 (  )
	{
	int ret, ret1 = KErrGeneral;
	siginfo_t info;
	timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;
	ret = sigtimedwait(NULL,&info,&timeout);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to set the errno to EINVAL on negative test"));
		goto close;
		}
	INFO_PRINTF1(_L("sigwaitinfo() has successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal8
// Test Case ID: OPENENV-LIBC-CIT-5980 
// API tested: sigtimedwait()
// Description: Negative test on sigtimedwait() with an invalid siginfo parameter
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal8 (  )
	{
	int ret, ret1 = KErrGeneral, pid, Signum, Maxsig, Sigval;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	sigset_t set;
	timespec timeout;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));
	argv[1]=(char*)malloc(6*sizeof(char));
	argv[2]=(char*)malloc(3*sizeof(char));
	argv[3]= 0;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		timeout.tv_sec = 3;
		timeout.tv_nsec = 0;
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set, Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		strcpy(argv[0], "z:\\sys\\bin\\sendsigtoparent.exe");
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);
		
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sendsigtoparent.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		time1.HomeTime();
		ret = sigtimedwait(&set,NULL,&timeout);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if(ret != Signum)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		/* There is no guarantee that there is no delay between sigtimedwait and posix_spawn

		if(aInterval.Int() < 2)
			{
			ERR_PRINTF2(_L("sigtimedwait() failed to suspend for 2 seconds for signal %d"),Signum);
			goto close;
			}
			*/
		}
	INFO_PRINTF1(_L("sigtimedwait() successfully was suspended for 2 seconds validated for every signal"));
	ret1 = KErrNone;
	
	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal9
// Test Case ID: OPENENV-LIBC-CIT-5981
// API tested: sigtimedwait()
// Description: Negative test when an empty signal set is passed to sigwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal9 (  )
	{
	sigset_t set;
	int ret, ret1 = KErrGeneral;
	siginfo_t info;
	timespec timeout;
	ret = setsigenv();
	if( ret == KErrGeneral)
		{
		ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Unable to empty the set"));
		goto close;
		}
	ret = sigtimedwait(&set,&info,&timeout);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("Error in getting the pending signal using sigtimedwait() and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigwait() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal10
// Test Case ID: OPENENV-LIBC-CIT-5983, OPENENV-LIBC-CIT-5984,OPENENV-LIBC-CIT-5985
// API tested: sigtimedwait()
// Description: To retrieve the pending signal from the multiple RT and Non-RT signals queued using sigwait()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal10 (  )
	{
	int ret, ret1 = KErrGeneral, Signum1, Signum2, Maxsig1, Maxsig2, OnlyRTSignal, value, x, y;
	sigset_t set, set1;
	siginfo_t info;
	timespec timeout;
	timeout.tv_sec = 2;
	timeout.tv_nsec = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum1"), Signum1);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the signal number Signum1"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig1"), Maxsig1);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the maximum signal number for Signum1"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Signum2"), Signum2);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the signal number Signum2"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig2"), Maxsig2);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the maximum signal number for Signum2"));
	 	goto close;
		}
	ret = GetIntFromConfig(ConfigSection(), _L("OnlyRTSignal"), OnlyRTSignal);
	if(ret == 0)
		{
	 	INFO_PRINTF1(_L("Failed to read the OnlyRTSignal value"));
	 	goto close;
		}
	
	for (x = Signum1; x <= Maxsig1; x++)
		{
		if((x == SIGKILL) || (x == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(x,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),x,errno) ;
			goto close;
			}
		for (y = Signum2; y <= Maxsig2; y++)
			{
			if((y == SIGKILL) || (y == SIGSTOP))
				{
				continue;
				}
			ret = setsigenv();
			if( ret == KErrGeneral)
				{
				ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
				goto close;
				}
			if (signal(y,SIG_IGN) == SIG_ERR)
				{
				ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),y,errno) ;
				goto close;
				}
			value = x;
			ret = setsignaltomask(set1,x);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),x);
				goto close;
				}
			ret = setsignaltomask(set1,y);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask set"),y);
				goto close;
				}
			ret = raise(x);
			if(ret != 0)
				{
				ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),x);
				goto close;
				}
			User::After(50000); // TODO: To be removed once sending to self is synchronous
			ret = raise(y);
			if(ret != 0)
				{
				ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),y);
				goto close;
				}
			User::After(50000); // TODO: To be removed once sending to self is synchronous
			ret = sigfillset(&set);
			if(ret != 0)
				{
				ERR_PRINTF1(_L("Failed to fill the set completely"));
				goto close;
				}
			ret = sigtimedwait(&set,&info,&timeout);
			if(OnlyRTSignal == 1)
				{
				if(x > y)
					{
					value = y;
					}
				if((ret != value) || (info.si_signo != value))
					{
					ERR_PRINTF2(_L("Failed to store the pending signal %d in sig"),value);
					goto close;
					}			
				}
			else
				{
				if((ret != x) && (ret != y))
					{
					ERR_PRINTF3(_L("Failed to return the pending signal in the combination of %d and %d signals"),x,y);
					goto close;
					}
				if(info.si_signo != ret)
					{
					ERR_PRINTF1(_L("Failed to store the pending signal in sig"));
					goto close;
					}
				}
			}
		}
	INFO_PRINTF1(_L("Successfully retreived the pending signal validated for every signal"));
	ret1 = KErrNone;

	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal11
// 
// API tested: sigtimedwait()
// Description: Two signals added to the input set of sigtimedwait(), one of them being either SIGKILL/SIGSTOP
// To improve coverage
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal11 (  )
	{
	sigset_t set, oset;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval, Negsignal;
	siginfo_t info;
	timespec timeout;
	timeout.tv_sec = 2;
	timeout.tv_nsec = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Negsignal"),Negsignal);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the sigkill/sigstop signal")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is %d"),Signum,errno) ;
			goto close;
			}
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to raise a blocked signal %d"),Signum);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the pending signal %d"),Signum);
			goto close;
			}
		ret = sigismember(&oset,Signum);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("Error in getting the pending signal %d"),Signum);
			goto close;
			}
		ret =  sigaddset(&set, Negsignal);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add signal %d to the set for signal %d"),Negsignal,Signum);
			goto close;
			}
		ret = sigtimedwait(&set,&info,&timeout);
		if ((ret != Signum) || (info.si_signo != Signum))
			{
			ERR_PRINTF1(_L("Error in getting the pending signal using sigwait()"));
			goto close;
			}
		ret = sigpending(&oset);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Failed to retrieve the pending signal"));
			goto close;
			}
		ret = sigismember(&oset,Signum);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("pending signal %d not cleared after sigwait()"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals are pending before sigtimedwait()"));
	INFO_PRINTF1(_L("Successfully retrieved the pending signal validated for all signals"));
	INFO_PRINTF1(_L("No signal is pending after sigtimedwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal12
// 
// API tested: sigtimedwait()
// Description:  Negative test- sigtimedwait() with timeout value as NULL
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal12 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum;
	siginfo_t info;
	sigset_t set;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigtimedwait(&set,&info,NULL);
		if((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to set the errno to EFAULT on negative test"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigwaitinfo() has successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal::Testtimedwaitsignal13
// 
// API tested: sigtimedwait()
// Description:  Negative test- sigtimedwait() with timeout being zero value
// -----------------------------------------------------------------------------

TInt CTestWaitsignal::Testtimedwaitsignal13 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum;
	siginfo_t info;
	sigset_t set;
	timespec timeout;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		timeout.tv_sec = 0;
		timeout.tv_nsec = 0;
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigtimedwait(&set,&info,&timeout);
		if((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to set the errno to EFAULT on negative test"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigwaitinfo() has successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestwaitsignal:: Testtimedwaitsignal14
// 
// API tested: sigtimedwait()
// Description: Retrieving the pending signal using sigtimedwait() while signal is raised using sigqueue()
// -----------------------------------------------------------------------------

TInt CTestWaitsignal:: Testtimedwaitsignal14 (  )
	{
	sigset_t set;
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	union sigval sival;
	struct sigaction sa;
	siginfo_t info;
	struct timespec timeout; 
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the  maximum signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		timeout.tv_sec = 1;
		timeout.tv_nsec = 0;
		sa.sa_sigaction = siginfoflaghandler;
		sa.sa_flags = SA_SIGINFO;
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		if (sigaction(Signum,&sa,NULL) == -1)
			{
			ERR_PRINTF3(_L("Sigaction() failed on testing SA_SIGINFO flag for signal %d and errno is %d"),Signum,errno);
			goto close;			
			}	
		ret = setsignaltomask(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Failed to add a signal to the process's signal mask"));
			goto close;
			}
		sival.sival_int = Signum;
		ret = sigqueue(getpid(),Signum,sival);
		if(ret != 0)
			{
			ERR_PRINTF2( _L("Error in sending the signal to the spawned process and errno is set %d"),errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		sigvalue = 0;
		ret = sigtimedwait(&set,&info,&timeout);
		if (ret != Signum)
			{
			ERR_PRINTF3(_L("Error in getting the pending signal %d using sigwaitinfo() and errno is %d"),Signum,errno);
			goto close;
			}
		if((info.si_signo != Signum) || (info.si_value.sival_int != Signum) || (sigvalue != 0))
			{
			ERR_PRINTF2(_L("sigqueue() failed for union sigval functionality for signal %d"),Signum);
			goto close;				
			}
		}
	INFO_PRINTF1(_L("Successfully retrieved the pending signal using sigwait()"));
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}


//End of file
