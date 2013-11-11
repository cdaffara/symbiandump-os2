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
// Name        : ttrapsignalblocks.cpp
// Test for signal trapping function with different signal disposition.
// 
//

#include "ttrapsignal.h"

char bufW[60];
char bufR[60];
int sigvalue = 0;
int sigactionvalue = -1;
int maskedsignal;

void sighandler(int sig)
	{
	int fd;
	fd = open("c:\\signalinfo.txt",O_WRONLY|O_CREAT);
	if (fd < 0)
		{
		sprintf(bufW, "Error in creating the file and the signal no. is  %d", sig);
		}
	else
		{
		sprintf(bufW, "In signal handler with the signal number equal to %d", sig);
		}
	write(fd, bufW, strlen(bufW));
	close(fd);
	}

//sigaction handler
void sigactionhandler(int /*sig*/)
	{
	sigset_t set;
	sigprocmask(SIG_SETMASK, NULL, &set);
	sigactionvalue = sigismember(&set,maskedsignal);
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

static int sethandlertodefault(int sig)
	{
	int ret, ret1 = KErrGeneral;
	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	ret = sigemptyset(&sa.sa_mask);
	if (ret != 0)
		{
		goto close;
		}
	sa.sa_flags = 0;
	if(sigaction(sig,&sa,NULL) == -1)
		{
		goto close;
		}
	ret1 = KErrNone;
	
close:
	return ret1;
	}

static int FileRead()
	{
	int fd, ret1 = KErrGeneral, ret;
	fd = open("c:\\signalinfo.txt",O_RDONLY);
	if (fd < 0)
		{
		goto close;
		}
	ret = read(fd,bufR,strlen(bufW));
	if (strncmp(bufR,bufW,ret) != 0)
		{
		goto close;
		}
	ret1 = KErrNone;
	
close:
	close(fd);
	unlink("c:\\signalinfo.txt");	
	return ret1;
	}

void ignorehandler(int sig)
	{
	sigvalue = sig;
	}

//set process's signal mask to 0 and signal handler to default
static int setsigsetnhandler(int sig)
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set;
	ret = sigemptyset(&set);
	if(ret != 0)
		{
		goto close;
		}
	ret = sigprocmask(SIG_SETMASK, &set, NULL);
	if(ret != 0)
		{
		goto close;
		}
	if (signal(sig,SIG_DFL) == SIG_ERR)
		{
		goto close;
		}
	ret1 = KErrNone;
		
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi
// Test Case ID: OPENENV-LIBC-CIT-5607
// API tested: sigaction()
// Description: To get the default signal disposition of a signal when the process starts using sigaction()  
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi (  )
	{
	int ret1=KErrGeneral, ret, Signum, Maxsig, Sigval;
	struct sigaction osa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGKILL) || (Signum == SIGSTOP))
			{
			continue;
			}
		if (sigaction(Signum,NULL,&osa) == -1)
			{
			ERR_PRINTF3(_L("Error in signal trapping function sigaction() for signal %d and errno is set to %d"),Signum,errno);
			goto close;			
			}
		if (osa.sa_handler != SIG_DFL)
			{
			ERR_PRINTF2(_L("Error in returning default handler for signal %d"),Signum);
			goto close;		
			}
		}
	INFO_PRINTF1(_L("Has successfully returned default handler for all signals"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}
	
// -----------------------------------------------------------------------------
// CTesttrapsignal::Testcustomsignalapi
// Test Case ID: OPENENV-LIBC-CIT-5608
// API tested: signal()
// Description: Test to change signal disposition to a custom handler << except SIGKILL and SIGSTOP >>
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testcustomsignalapi (  )
	{
	int ret1=KErrGeneral, ret, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		if (signal(Signum,sighandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		ret = FileRead();
		if(ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Error in validating signal trapping function signal() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Has successfully entered the custom handler for all signals using signal() api"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegcustomsignalapi
// Test Case ID: OPENENV-LIBC-CIT-5609,OPENENV-LIBC-CIT-5610
// API tested: signal()
// Description: Negative test: Trying to change the signal disposition to custom handler for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegcustomsignalapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on signal() api"));
	if(signal(Signum,sighandler) != SIG_ERR)
		{
		ERR_PRINTF1(_L("signal() failed on negative test"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
		
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testcustomsigsetapi1
// Test Case ID: OPENENV-LIBC-CIT-5611
// API tested: sigset()
// Description: Test case added to change signal disposition to a custom handler for an unblocked signal using sigset() << except SIGKILL and SIGSTOP >>
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testcustomsigsetapi1 (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		if (sigset(Signum,sighandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		ret = FileRead();
		if(ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Error in validating signal trapping function signal() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Has successfully entered the custom handler for all signals using sigset()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testcustomsigsetapi2
// Test Case ID: OPENENV-LIBC-CIT-5612
// API tested: sigset()
// Description: Test case added to change signal disposition to a custom handler for a blocked signal using sigset() << except SIGKILL and SIGSTOP >>
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testcustomsigsetapi2 (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		ret = sighold(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Unable to block the signal %d"),Signum);
			goto close;
			}
		if (sigset(Signum,sighandler) != SIG_HOLD)
			{
			ERR_PRINTF2(_L("Error in returning SIG_HOLD for signal %d"),Signum);
			goto close;
			}
		ret = sigrelse(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Unable to unblock the signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("signal trapping function sigset() has successfully returned SIG_HOLD"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegcustomsigsetapi
// Test Case ID: OPENENV-LIBC-CIT-5613,OPENENV-LIBC-CIT-5614
// API tested: sigset()
// Description: Negative test: Trying to change the signal disposition to custom handler for an invalid signal and the signal which cannot be ingored.
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegcustomsigsetapi (  )
	{   
	int ret1 = KErrGeneral, ret,Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigset() api"));
	if(sigset(Signum,sighandler) != SIG_ERR)
		{
		ERR_PRINTF1(_L("sigset() failed on negative test"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testcustombsd_signalapi
// Test Case ID: OPENENV-LIBC-CIT-5615
// API tested: bsd_signal()
// Description: Test case added to change signal disposition to a custom handler using bsd_signal() << except SIGKILL and SIGSTOP >>
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testcustombsd_signalapi (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
		if (ret == 0)
			{
			ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		if (bsd_signal(Signum,sighandler) == SIG_ERR)
			{
			ERR_PRINTF2(_L("Error in signal trapping function and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in raising the signal and errno is set to %d"),errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		ret = FileRead();
		if(ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Failed in validating the signal trapping function bsd_signal() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Has successfully entered the custom handler for all signals using bsd_signal()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegcustombsd_signalapi
// Test Case ID: OPENENV-LIBC-CIT-5616,OPENENV-LIBC-CIT-5617
// API tested: bsd_signal()
// Description: Negative test: Trying to change the signal disposition to custom handler for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegcustombsd_signalapi (  )
	{   
	int ret1 = KErrGeneral, ret,Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on bsd_signal() api"));
	if(bsd_signal(Signum,sighandler) != SIG_ERR)
		{
		ERR_PRINTF1(_L("bsd_signal() failed on negative test"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testcustomsigactionapi
// Test Case ID: OPENENV-LIBC-CIT-5618
// API tested: sigaction()
// Description: Test case added to change signal disposition to a custom handler using sigaction() << except SIGKILL and SIGSTOP >>
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testcustomsigactionapi (  )
{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
		if (ret == 0)
			{
			ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sa.sa_handler = sighandler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		if (sigaction(Signum,&sa,NULL) == -1)
			{
			ERR_PRINTF2(_L("Error in signal trapping function and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in raising the signal and errno is set to %d"),errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		ret = FileRead();
		if(ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Failed in validating the signal trapping function sigaction() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Has successfully entered the custom handler for all signals using sigaction()"));
	ret1 = KErrNone;
	
close:
	return ret1;
}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegcustomsigactionapi
// Test Case ID: OPENENV-LIBC-CIT-5619,OPENENV-LIBC-CIT-5620
// API tested: sigaction()
// Description: Negative test: Trying to change the signal disposition to custom handler for an invalid signal and the signal which cannot be ingored.
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegcustomsigactionapi (  )
{   
	int ret1 = KErrGeneral,ret,Signum;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigaction() api"));
	sa.sa_handler = sighandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(Signum,&sa,NULL) != -1)
		{
		ERR_PRINTF1(_L("sigaction() failed on negative test"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testignoresignalapi
// Test Case ID: OPENENV-LIBC-CIT-5621
// API tested: signal()
// Description: Test case added to change signal disposition to ignore using signal() << except SIGKILL and SIGSTOP >> 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testignoresignalapi (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sigvalue = 0;
		if(signal(Signum,ignorehandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping funtion for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		if(signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF2(_L("Error in signal trapping funtion and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		if(sigvalue != 0)
			{
			ERR_PRINTF2(_L("Unable to ignore signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals was successfully ignored using signal()"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegignoresignalapi
// Test Case ID: OPENENV-LIBC-CIT-5622,OPENENV-LIBC-CIT-5623
// API tested: signal()
// Description: Negative test: Trying to change the signal disposition to ignore for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegignoresignalapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on signal() api"));
	if(signal(Signum,SIG_IGN) != SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testignoresigsetapi1
// Test Case ID: OPENENV-LIBC-CIT-5624
// API tested: sigset()
// Description: Test case added to change signal disposition to ignore using sigset() for an unblocked signal << except SIGKILL and SIGSTOP >> 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testignoresigsetapi1 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sigvalue = 0;
		if(sigset(Signum,ignorehandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping funtion for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		if(sigset(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF2(_L("Error in signal trapping funtion and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: To be removed once sending to self is synchronous
		if(sigvalue != 0)
			{
			ERR_PRINTF2(_L("Unable to ignore signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals was successfully ignored using sigset()"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testignoresigsetapi2
// Test Case ID: OPENENV-LIBC-CIT-5625
// API tested: sigset()
// Description: Test case added to change signal disposition to ignore using sigset() for a blocked signal<< except SIGKILL and SIGSTOP >> 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testignoresigsetapi2 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		ret = sighold(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Unable to block the signal %d"),Signum);
			goto close;
			}
		if(sigset(Signum,SIG_IGN) != SIG_HOLD)
			{
			ERR_PRINTF2(_L("Error in signal trapping funtion and errno is set to %d"),errno);
			goto close;
			}
		ret = sigrelse(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Unable to unblock the signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals was successfully ignored using sigset()"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegignoresigsetapi
// Test Case ID: OPENENV-LIBC-CIT-5626,OPENENV-LIBC-CIT-5627
// API tested: sigset()
// Description: Negative test: Trying to change the signal disposition to ignore for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegignoresigsetapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigset() api"));
	if(sigset(Signum,SIG_IGN) != SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testignorebsd_signalapi
// Test Case ID: OPENENV-LIBC-CIT-5628
// API tested: bsd_signal()
// Description: Test case added to change signal disposition to ignore using bsd_signal() << except SIGKILL and SIGSTOP >> 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testignorebsd_signalapi (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sigvalue = 0;
		if(bsd_signal(Signum,ignorehandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping funtion for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		if(bsd_signal(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF2(_L("Error in signal trapping funtion and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		if(sigvalue != 0)
			{
			ERR_PRINTF2(_L("Unable to ignore signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals was successfully ignored using bsd_signal()"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}
// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegignorebsd_signalapi
// Test Case ID: OPENENV-LIBC-CIT-5629,OPENENV-LIBC-CIT-5630
// API tested: bsd_signal()
// Description: Negative test: Trying to change the signal disposition to ignore for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegignorebsd_signalapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on bsd_signal() api"));
	if(bsd_signal(Signum,SIG_IGN) != SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testignoresigactionapi
// Test Case ID: OPENENV-LIBC-CIT-5631
// API tested: sigaction()
// Description: Test case added to change signal disposition to ignore using sigaction() << except SIGKILL and SIGSTOP >> 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testignoresigactionapi (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sigvalue = 0;
		sa.sa_handler = ignorehandler;
		sa.sa_flags = 0;
		if(sigaction(Signum,&sa,NULL) != 0)
			{
			ERR_PRINTF3(_L("Error in signal trapping funtion for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		sa.sa_handler = SIG_IGN;
		if(sigaction(Signum,&sa,NULL) != 0)
			{
			ERR_PRINTF2(_L("Error in signal trapping funtion and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		if(sigvalue != 0)
			{
			ERR_PRINTF2(_L("Unable to ignore signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals was successfully ignored using sigaction()"));
	ret1 = KErrNone;
	
close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegignoresigactionapi
// Test Case ID: OPENENV-LIBC-CIT-5632,OPENENV-LIBC-CIT-5633
// API tested: sigaction()
// Description: Negative test: Trying to change the signal disposition to ignore for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegignoresigactionapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigaction() api"));
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(Signum,&sa,NULL) != -1)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigignoreapi
// Test Case ID: OPENENV-LIBC-CIT-5634
// API tested: sigignore()
// Description: Test case added to change signal disposition to ignore using sigignore() << except SIGKILL and SIGSTOP >> 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigignoreapi (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sigvalue = 0;
		if(signal(Signum,ignorehandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping funtion for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		if(sigignore(Signum) != 0)
			{
			ERR_PRINTF2(_L("Error in signal trapping funtion and errno is set to %d"),errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		if(sigvalue != 0)
			{
			ERR_PRINTF2(_L("Unable to ignore signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("All signals was successfully ignored using sigignore()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegsigignoreapi
// Test Case ID: OPENENV-LIBC-CIT-5635,OPENENV-LIBC-CIT-5636
// API tested: sigignore()
// Description: Negative test: Trying to change the signal disposition to ignore for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------
//
TInt CTestTrapsignal::Testnegsigignoreapi (  )
{
	int ret, ret1 = KErrGeneral,Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigignore() api"));
	if(sigignore(Signum) != -1)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;	
}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegdefaultsignalapi
// Test Case ID: OPENENV-LIBC-CIT-5637,OPENENV-LIBC-CIT-5638
// API tested: signal()
// Description: Negative test: Trying to change the signal disposition to default handler for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegdefaultsignalapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on signal() api"));
	if(signal(Signum,SIG_DFL) != SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegdefaultsigsetapi
// Test Case ID: OPENENV-LIBC-CIT-5639,OPENENV-LIBC-CIT-5640
// API tested: sigset()
// Description: Negative test: Trying to change the signal disposition to default handler for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegdefaultsigsetapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigset() api"));
	if(sigset(Signum,SIG_DFL) != SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegdefaultbsd_signalapi
// Test Case ID: OPENENV-LIBC-CIT-5641,OPENENV-LIBC-CIT-5642
// API tested: bsd_signal()
// Description: Negative test: Trying to change the signal disposition to default handler for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegdefaultbsd_signalapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on bsd_signal() api"));
	if(bsd_signal(Signum,SIG_DFL) != SIG_ERR)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testnegdefaultsigactionapi
// Test Case ID: OPENENV-LIBC-CIT-5643,OPENENV-LIBC-CIT-5644
// API tested: sigaction()
// Description: Negative test: Trying to change the signal disposition to default handler for an invalid signal and the signal which cannot be ingored. 
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testnegdefaultsigactionapi (  )
	{   
	int ret1 = KErrGeneral, ret, Signum;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(),_L("Signum"),Signum);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test on sigaction() api"));
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(Signum,&sa,NULL) != -1)
		{
		ERR_PRINTF1(_L("Error in signal trapping function"));
		goto close;
		}
	if (errno != EINVAL)
		{
		ERR_PRINTF1(_L("The expected value and the errno returned are not same"));
		goto close;
		}
	INFO_PRINTF1(_L("The expected value and the errno returned are same"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigsethold1
// Test Case ID: OPENENV-LIBC-CIT-5645
// API tested: sigset()
// Description: sigset() with the signal disposition as SIG_HOLD << except SIGKILL and SIGSTOP >>
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigsethold1 (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	sigset_t set;
	struct sigaction osa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		if (sigset(Signum,SIG_IGN) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		if (sigset(Signum,SIG_HOLD) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		User::After(50000); // TODO: Remove once raise() is synchronous
		if (sigaction(Signum,NULL,&osa) == -1)
			{
			ERR_PRINTF3(_L("Error in signal trapping function sigaction() for signal %d and errno is set to %d"),Signum,errno);
			goto close;			
			}
		if (osa.sa_handler != SIG_IGN)
			{
			ERR_PRINTF2(_L("Error in returning default handler for signal %d"),Signum);
			goto close;		
			}
		sigprocmask(SIG_SETMASK, NULL, &set);
		ret = sigismember(&set,Signum);
		if(ret != 1)
			{
			ERR_PRINTF2(_L("sigset() failed in adding signal to the mask for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Has successfully validated for all signals using sigset()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	
	
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi1
// Test Case ID: OPENENV-LIBC-CIT-5646
// API tested: sigaction()
// Description: To get the previous signal disposition of a signal using sigaction()  
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi1 (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	struct sigaction sa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		ret = sethandlertodefault(Signum);
		if (ret == KErrGeneral)
			{
			ERR_PRINTF2(_L("Error in settting handler to default for signal %d\n"),Signum);
			goto close;		
			}
		if (signal(Signum,sighandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		if (sigaction(Signum,NULL,&sa) == -1)
			{
			ERR_PRINTF3(_L("Error in signal trapping function sigaction() for signal %d and errno is set to %d"),Signum,errno);
			goto close;			
			}
		if (sa.sa_handler != sighandler)
			{
			ERR_PRINTF2(_L("Error in retrieving the previous signal action for signal %d"),Signum);
			goto close;					
			}
		}
	INFO_PRINTF1(_L("Successfully able to retrieve the previous signal action using sigaction()"));
	ret1 = KErrNone;
		
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi2
// Test Case ID: OPENENV-LIBC-CIT-5647
// API tested: sigaction()
// Description: To get the previous signal disposition of a signal using sigaction()  
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi2 (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	struct sigaction sa,osa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		if (signal(Signum,sighandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		sa.sa_handler = SIG_IGN;
		sa.sa_flags = 0;
		ret = sigemptyset(&(sa.sa_mask));
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Error in emptying the signal set"));
			goto close;			
			}
		if (sigaction(Signum,&sa,&osa) == -1)
			{
			ERR_PRINTF3(_L("Error in signal trapping function sigaction() for signal %d and errno is set to %d"),Signum,errno);
			goto close;			
			}
		if (osa.sa_handler != sighandler)
			{
			ERR_PRINTF2(_L("Error in retrieving the previous signal action for signal %d"),Signum);
			goto close;					
			}
		sa.sa_handler = sighandler;
		if (sigaction(Signum,&sa,&osa) == -1)
			{
			ERR_PRINTF3(_L("Error in signal trapping function sigaction() for signal %d and errno is set to %d"),Signum,errno);
			goto close;			
			}
		if (osa.sa_handler != SIG_IGN)
			{
			ERR_PRINTF2(_L("Error in retrieving the previous signal action for signal %d"),Signum);
			goto close;					
			}
		}
	INFO_PRINTF1(_L("Successfully able to retrieve the previous signal action for signal using sigaction()"));  
	ret1 = KErrNone;
		
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi3
// Test Case ID: OPENENV-LIBC-CIT-5648
// API tested: sigaction()
// Description: Test case to validate sa_mask of sigaction  
// Limitation: No signal will be added to the process's signal mask during the execution of signal handler, so sa_mask will not be supported as such
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi3 (  )
	{
	int ret1=KErrGeneral, ret, Sigval, Maxsig, Signum1, Signum2;
	sigset_t set1;
	struct sigaction sa,osa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	for (Signum1 = Sigval; Signum1 <= Maxsig; Signum1++)
		{
		if((Signum1 == SIGKILL) || (Signum1 == SIGSTOP))
			{
			continue;
			}
		ret = setsigenv();
		if( ret == KErrGeneral)
			{
			ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
			goto close;
			}
		for (Signum2 = Sigval; Signum2 <= Maxsig; Signum2++)
			{
			if((Signum2 == SIGKILL) || (Signum2 == SIGSTOP))
				{
				continue;
				}
			ret = setsigenv();
			if( ret == KErrGeneral)
				{
				ERR_PRINTF1(_L("Failed in setting the default enviroment for signal")) ;
				goto close;
				}
			sigactionvalue = -1;
			maskedsignal = Signum2;
			ret = setsigsetnhandler(Signum1);
			if (ret == KErrGeneral)
				{
				ERR_PRINTF2(_L("Failed to set default handler and signal set for signal %d"),Signum1);
				goto close;
				}
			sa.sa_handler = sigactionhandler;
			sa.sa_flags = SA_NODEFER;
			ret = sigemptyset(&(sa.sa_mask));
			if (ret != 0)
				{
				ERR_PRINTF1(_L("Unable to empty the set"));
				goto close;	
				}
			ret = sigaddset(&(sa.sa_mask),Signum2);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Unable to add a signal %d to the set"),Signum2);
				goto close;	
				}
			if (sigaction(Signum1,&sa,&osa) == -1)
				{
				ERR_PRINTF3(_L("Error in signal trapping function sigaction() for signal %d and errno is set to %d"),Signum1,errno);
				goto close;	
				}
			sigprocmask(SIG_SETMASK, NULL, &set1);
			ret = sigismember(&set1,Signum2);
			if(ret != 0)
				{
				ERR_PRINTF1(_L("Is part of a signal set"));
				goto close;	
				}
			ret = raise(Signum1);
			if (ret != 0)
				{
				ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum1,errno);
				goto close;		
				}
			User::After(50000); // TODO: Remove once raise is synchronous
			if (sigactionvalue != 0)
				{
				ERR_PRINTF2(_L("Error in masking the signal during execution of signal handler for signal %d"),Signum2);
				goto close;			
				}
			ret = sigismember(&set1,Signum2);
			if(ret != 0)
				{
				ERR_PRINTF1(_L("Is part of a signal set"));
				goto close;	
				}
			}
		}
	INFO_PRINTF1(_L("sigaction() api was successfully able to validate on sa_mask member"));
	ret1 = KErrNone;
		
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi4
// Test Case ID: OPENENV-LIBC-CIT-5649
// API tested: sigaction()
// Description: Test case to validate sa_flags of sigaction() which are not supported  
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi4 (  )
	{	
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Flagvalue, Sigval;
	struct sigaction sa,osa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Flagvalue"),Flagvalue);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the Flag value")) ;
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
		sa.sa_handler = SIG_IGN;
		sa.sa_flags = Flagvalue;
		ret = sigemptyset(&(sa.sa_mask));
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Error in emptying the signal set"));
			goto close;			
			}
		if (sigaction(Signum,&sa,&osa) != -1)
			{
			ERR_PRINTF2(_L("Sigaction() failed on negative test for signal %d"),Signum);
			goto close;			
			}
		if(errno != ENOTSUP)
			{
			ERR_PRINTF2(_L("Sigaction() failed with an errno %d"),errno);
			goto close;			
			}	
		}
	INFO_PRINTF2(_L("Sigaction() successfully returned ENOTSUP for all signals with a flag value as %d"),Flagvalue);
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi5
// Test Case ID: OPENENV-LIBC-CIT-5650
// API tested: sigaction()
// Description: sigaction() test on SA_RESETHAND flag.  
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi5 (  )
	{	
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	struct sigaction sa,osa;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		sa.sa_handler = ignorehandler;
		sa.sa_flags = SA_RESETHAND;
		ret = sigemptyset(&(sa.sa_mask));
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Error in emptying the signal set"));
			goto close;			
			}
		if (sigaction(Signum,&sa,&osa) == -1)
			{
			ERR_PRINTF3(_L("Sigaction() failed on testing SA_RESETHAND flag for signal %d and errno is %d"),Signum,errno);
			goto close;			
			}	
		if (osa.sa_handler != SIG_IGN)
			{
			ERR_PRINTF2(_L("Sigaction() failed to return SIG_IGN for signal %d"),Signum);
			goto close;				
			}
		sigvalue = 0;
		ret = raise(Signum);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Error in raising the signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}	
		User::After(50000); // TODO: Remove once raise is synchronous
		if(sigvalue != Signum)
			{
			ERR_PRINTF2(_L("Failed to enter the signal handler for signal %d"),Signum);
			goto close;		
			}
		if (sigaction(Signum,NULL,&osa) == -1)
			{
			ERR_PRINTF2(_L("Sigaction() failed to retrieve earlier signal information for signal %d"),Signum);
			goto close;			
			}	
		if (osa.sa_handler != SIG_DFL)
			{
			ERR_PRINTF2(_L("Sigaction() failed to reset the  for signal %d"),Signum);
			goto close;				
			}
		}
	INFO_PRINTF1(_L("Sigaction() successfully validated on SA_RESETHAND flag for all signals "));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testsigactionapi6
// Test Case ID: OPENENV-LIBC-CIT-5651
// API tested: sigaction()
// Description: sigaction() test on SA_SIGINFO flag.  
// -----------------------------------------------------------------------------

TInt CTestTrapsignal::Testsigactionapi6 (  )
	{
	int ret1 = KErrGeneral, ret, Signum, Maxsig, Sigval;
	struct sigaction sa,osa;
	union sigval sival;
	ret = GetIntFromConfig(ConfigSection(),_L("Sigval"),Sigval);
	if(ret == 0)
		{
		INFO_PRINTF1(_L("Error in reading the signal number"));
		goto close;
		}
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
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
		sa.sa_sigaction = siginfoflaghandler;
		sa.sa_flags = SA_SIGINFO;
		ret = sigemptyset(&(sa.sa_mask));
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Error in emptying the signal set"));
			goto close;			
			}
		sigvalue = 0;
		if (sigaction(Signum,&sa,&osa) == -1)
			{
			ERR_PRINTF3(_L("Sigaction() failed on testing SA_SIGINFO flag for signal %d and errno is %d"),Signum,errno);
			goto close;			
			}	
		sival.sival_int = Signum;
		ret = sigqueue(getpid(),Signum,sival);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in sigqueue() for signal %d"),Signum);
			goto close;			
			}
		User::After(50000); // TODO: remove once sending to self is synchronous
		if(sigvalue != 1)
			{
			ERR_PRINTF2(_L("sigqueue() failed for union sigval functionality for signal %d"),Signum);
			goto close;				
			}
		}
	INFO_PRINTF1(_L("Sigaction() successfully validated on SA_SIGINFO flag for all signals "));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

//End of file
