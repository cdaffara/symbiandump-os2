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
// Name        : tblocksignalblocks.cpp
// Test cases for blocking signal api's
// 
//


#include "tblocksignal.h"

int sigvalue = 0;

//empty the process's signal mask
static int emptysignalmask(sigset_t &set)
	{
	int ret, ret1 = 0;
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		goto close;
		}
	ret = sigprocmask(SIG_SETMASK,&set,NULL); //delete all the signals from the process's signal mask
	if (ret != 0)
		{
		goto close;
		}
	ret1 = 1;
	
	close:
	return ret1;
	}	

//to add signal to the process's signal mask
static int setsignaltomask(sigset_t &set,int sig)
	{
	int ret, ret1 = 0;
	ret = sigemptyset(&set);
	if(ret != 0)
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

// to empty the signal set.
static int emptyset(sigset_t &set1, sigset_t &set2)
	{
	int ret, ret1 = 0;
	ret = sigemptyset(&set1);
	if (ret != 0)
		{
		goto close;
		}
	ret = sigemptyset(&set2);
	if (ret != 0)	
		{
		goto close;
		}
	ret1 = 1;
	
	close:
	return ret1;
	}

// to add signals s1,s2 to set
static int addsignaltoset(sigset_t &set, int s1, int s2)
	{
	int ret, ret1 = 0;
	ret = sigaddset(&set,s1);
	if (ret != 0)
		{
		goto close;
		}
	ret = sigaddset(&set,s2);
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

void sigpausehandler(int sig)
	{
	sigvalue =  sig;
	}

//to clear signal mask and set the handler to default
static int setsigenv()
	{
	int Signum, ret, ret1 = KErrGeneral;
	
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
// CTestblocksignal::Testempty1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API te sted: sigemptyset(), sigisemptyset()
// Description: Initialize the signal set to the empty set and check if the set is empty.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testempty1 (  )
	{
	sigset_t set;
	int ret, ret1 = KErrGeneral;
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to empty the set"));
	ret = sigisemptyset(&set);
	if (ret != 1)
		{
		ERR_PRINTF1(_L("sigisemptyset() has failed to check whether the set is empty or not"));
		goto close;
		}	
	INFO_PRINTF1(_L("The signal set is empty validated using sigisemptyset()"));
	ret1 = KErrNone;	
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testempty2
// Test Case ID: OPENENV-LIBC-CIT-5947, OPENENV-LIBC-CIT-5948
// API tested: sigemptyset(), sigisemptyset()
// Description: Negative test attempting to empty the invalid pointer set and to check if the invalid pointer set is empty
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testempty2 (  )
	{
	int ret, ret1 = KErrGeneral;
	INFO_PRINTF1(_L("Negative test on sigemptyset()"));
	ret = sigemptyset(NULL);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigemptyset() failed to return EINVAL on negative test"));
		ERR_PRINTF2(_L("errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigemptyset() is success in validating the NULL input"));
	INFO_PRINTF1(_L("Negative test on sigisemptyset()"));
	ret = sigisemptyset(NULL);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigisemptyset() failed to return EINVAL on negative test"));
		ERR_PRINTF2(_L("errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigisemptyset() is success in validating the NULL input"));	
	ret1 = KErrNone;
	
close:
	return ret1;
}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddset1
// Test Case ID: OPENENV-LIBC-CIT-5949
// API tested: sigaddset(), sigisemptyset()
// Description: Test case to check  the signal set if the set is not empty
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddset1 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum;
	sigset_t set;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum = Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Unable to empty the set for signal %d and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigisemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("sigisemptyset() has failed to check whether the set is empty or not for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Successfully checked the set for all signals"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddset2
// Test Case ID: OPENENV-LIBC-CIT-5950
// API tested: sigaddset(),sigemtpyset(),sigismember()
// Description: Negative test to check whether the signal is a member of set.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddset2 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Uable to empty the set for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Uable to empty the set after adding a signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		ret = sigismember(&set,Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("sigismember() has failed to validate for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigismember() has successfully returned 0 validated for all signals on negative test"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddset3
// Test Case ID: OPENENV-LIBC-CIT-5951
// API tested: sigismember(),sigaddset()
// Description: Negative test to check whether the invalid signal of non-empty set is a member of set or not.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddset3 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Uable to empty the set for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigismember(&set,Signum);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("sigismember() failed to validate for signal %d"),Signum);
			goto close;
			}
		ret = sigismember(&set,-1);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to return EINVAL on negative test"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("Negative test: sigismember() has successfully returned -1 checked for all signals"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddset4
// Test Case ID: OPENENV-LIBC-CIT-5952
// API tested: sigismember()
// Description: Negative test to check whether the invalid signal of an empty set is a member or not.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddset4 (  )
	{
	int ret, ret1 = KErrGeneral, Invalsig;
	sigset_t set;
	ret = GetIntFromConfig(ConfigSection(),_L("Invalsig"),Invalsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read an invalid signal number")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Uable to empty the set"));
		goto close;
		}
	ret = sigismember(&set,Invalsig);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to return EINVAL on negative test"));
		goto close;
		}
	INFO_PRINTF1(_L("Negative test: sigismember() has successfully returned -1 checked for all signals"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddset5
// Test Case ID: OPENENV-LIBC-CIT-5953
// API tested: sigismember()
// Description: Negative test to check valid signal in a NULL pointer set using sigismember()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddset5 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigismember(NULL,Signum);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF2(_L("Failed to return EINVAL on negative test for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Negative test: sigismember() has successfully returned -1 checked for all signals"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddndelset1
// Test Case ID: OPENENV-LIBC-CIT-5954
// API tested: sigaddset(),sigdelset()
// Description: Positive test for sigaddset() and sigdelset()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddndelset1 (  )
	{
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Uable to empty the set and errno is set to %d"),errno);
			goto close;
			}
		ret = sigismember(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to check the signal %d in the set before adding to it"),Signum);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigismember(&set,Signum);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("Failed to check the signal %d in the set after adding to it"),Signum);
			goto close;
			}
		ret = sigdelset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Unable to delete a signal %d from the set and errno is %d"),Signum,errno);
			goto close;
			}
		ret = sigismember(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to check the signal %d in the set after deleting it"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Each signal is successfully added and deleted from the set"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddndelset2
// Test Case ID: OPENENV-LIBC-CIT-5955
// API tested: sigaddset(),sigdelset()
// Description: Negative test for sigaddset() and sigdelset() to check an invalid signal with the set been empty
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddndelset2 (  )
	{
	int ret, ret1 = KErrGeneral, Invalsig;
	sigset_t set;	
	ret = GetIntFromConfig(ConfigSection(),_L("Invalsig"),Invalsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read an invalid signal number")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Uable to empty the set and errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Success in emptying the set"));
	INFO_PRINTF1(_L("Negative test on sigaddset()"));
	ret = sigaddset(&set,Invalsig);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to return -1 when trying to add an invalid signal to the set"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigaddset() successfully set the errno to EINVAL"));
	INFO_PRINTF1(_L("Negative test on sigdelset()"));
	ret = sigdelset(&set,Invalsig);
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Failed to return -1 when trying to delete an invalid signal to the set"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigdelset() successfully set the errno to EINVAL"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddndelset3
// Test Case ID: OPENENV-LIBC-CIT-5956
// API tested: sigaddset(),sigdelset()
// Description: Negative test for sigaddset() and sigdelset() with the set been non empty
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddndelset3 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Uable to empty the set and errno is set to %d"),errno);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigaddset(&set,-1);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to return -1 when trying to add an invalid signal to the set"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigdelset(&set,-1);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to return -1 when trying to delete an invalid signal to the set"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigaddset() and sigdelset() successfully set the errno to EINVAL validated for each signal"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testaddndelset4
// Test Case ID: OPENENV-LIBC-CIT-5957
// API tested: sigaddset(),sigdelset()
// Description: Negative test for sigaddset() and sigdelset() with the set been NULL
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testaddndelset4 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum;
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
	INFO_PRINTF1(_L("Negative test on sigaddset() and sigdelset()"));
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigaddset(NULL,Signum);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to return -1 when trying to add an invalid signal to the set"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigdelset(NULL,Signum);
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF1(_L("Failed to return -1 when trying to delete an invalid signal to the set"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigdelset() successfully set the errno to EINVAL"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask1
// Test Case ID: OPENENV-LIBC-CIT-5958
// API tested: sigprocmask()
// Description: To examine the signal mask of the calling process
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask1 (  )
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set, oset;
	ret = emptysignalmask(set);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
		goto close;	
		}
	ret = sigprocmask(SIG_SETMASK,NULL,&oset);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("sigprocmask failed to return 0"));
		ERR_PRINTF2(_L("errno is set to %d"),errno);
		goto close;
		}
	ret = sigisemptyset(&oset);
	if (ret != 1)
		{
		ERR_PRINTF1(_L("Failed to validate sigprocmask()"));
		goto close;
		}
	INFO_PRINTF1(_L("The process's signal mask is set to 0"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask2
// Test Case ID: OPENENV-LIBC-CIT-5959
// API tested: sigprocmask()
// Description: To block signals using sigprocmask()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask2 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set, oset;
	INFO_PRINTF1(_L("Test on sigprocmask() with SIG_BLOCK parameter"));
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = emptysignalmask(set);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
			goto close;	
			}
		ret = setsignaltomask(set,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigprocmask(SIG_BLOCK,NULL,&oset); //to get the process's signal mask stored at oset
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask set with signal %d"),Signum);
			ERR_PRINTF2(_L("Errno is set to %d"),errno);
			goto close;
			}
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigprocmask() is been validated for each and every signal"));
	ret1 = KErrNone;	

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask3
// Test Case ID: OPENENV-LIBC-CIT-5960
// API tested: sigprocmask()
// Description: To unblock signals using sigprocmask()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask3 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set, oset;
	INFO_PRINTF1(_L("Test on sigprocmask() with SIG_UNBLOCK parameter"));
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = emptysignalmask(set);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
			goto close;	
			}
		ret = setsignaltomask(set,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigprocmask(SIG_UNBLOCK,&set,NULL); //Remove the signal from process's signal mask
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to remove a signal %d from the process's signal mask"),Signum);
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigprocmask(SIG_UNBLOCK,NULL,&oset); //to get the process's signal mask stored at oset
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask for signal %d"),Signum);
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if (ret != 1)
			{
			ERR_PRINTF2(_L("Failed to remove a signal %d from process's signal mask using SIG_UNBLOCK"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("Sigprocmask() validated for each signal using SIG_UNBLOCK"));
	ret1 = KErrNone;	
	
close:	
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask4
// Test Case ID: OPENENV-LIBC-CIT-5961
// API tested: sigprocmask()
// Description: To completely set the signal mask using sigprocmask()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask4 ( )
	{
	int ret, ret1 = KErrGeneral, Signum1, Signum2, Maxsig, Sigval;
	sigset_t set1, set2, oset;
	INFO_PRINTF1(_L("Test on sigprocmask() with SIG_SETMASK parameter"));
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number"));
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(),_L("Maxsig"),Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum1=Sigval; Signum1 <= Maxsig; Signum1++)
		{
		if((Signum1 == SIGSTOP) || (Signum1 == SIGKILL))
			{
			continue;
			}
		for (Signum2=Sigval; Signum2<=Maxsig ; Signum2++)
			{
			if((Signum2 == SIGSTOP) || (Signum2 == SIGKILL))
				{
				continue;
				}
			ret = emptysignalmask(set1);
			if(ret == 0)
				{
				ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
				goto close;	
				}
			ret = setsignaltomask(set1,Signum1);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum1);
				goto close;
				}
			ret = sigemptyset(&set2);
			if (ret != 0)
				{
				ERR_PRINTF1(_L("Unable to empty the set2"));
				goto close;
				}
			ret = sigaddset(&set2,Signum2);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the set2"),Signum2);
				goto close;
				}
			ret = sigprocmask(SIG_SETMASK,&set2,NULL); 
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask with SIG_SETMASK set"),Signum2);
				goto close;
				}
			ret = sigprocmask(SIG_BLOCK,NULL,&oset); //to get the process's signal mask stored at oset
			if(ret != 0)
				{
				ERR_PRINTF1(_L("Unable to retrieve the process's signal mask"));
				ERR_PRINTF2(_L("the errno is set to %d"),errno);
				goto close;
				}	 
			ret = checksignalinset(oset,Signum2);
			if (ret == 0)
				{
				ERR_PRINTF2(_L("Failed to add signal %d to the process's signal mask using SIG_SETMASK"),Signum2);
				goto close;
				}
			}
		}
	INFO_PRINTF1(_L("Successfully validated sigprocmask() for all signals using SIG_SETMASK"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask5
// Test Case ID: OPENENV-LIBC-CIT-5962
// API tested: sigprocmask()
// Description: Negative test on sigprocmask()with a non empty set
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask5 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set1, set2, oset;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = emptysignalmask(set1);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
			goto close;	
			}
		ret = setsignaltomask(set1,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigemptyset(&set2);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigaddset(&set2,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigprocmask(5,&set2,NULL); //with an invalid how argument
		if ((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF2(_L("sigprocmask() has not failed with an invalid how argument for signal %d"),Signum);
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigprocmask(SIG_BLOCK,NULL,&oset); //to get the process's signal mask stored at oset
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to set the process's signal mask for signal %d"),Signum);
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = checksignalinset(oset,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("failed to validate the set as it contains more than one signal"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("Process's signal mask is same before and after sigprocmask() failure"));
	INFO_PRINTF1(_L("sigprocmask() returns EINVAL for an invalid how argument for all signals"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask6
// Test Case ID: OPENENV-LIBC-CIT-5963
// API tested: sigprocmask()
// Description: Negative test on sigprocmask() with the set been empty
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask6 (  )
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set;
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	ret = sigprocmask(5,&set,NULL); //with an invalid how argument
	if ((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigprocmask() has not failed with an invalid how argument for an empty signal set"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigprocmask() returns EINVAL for an invalid how argument"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask7
// Test Case ID: OPENENV-LIBC-CIT-5964, OPENENV-LIBC-CIT-5965
// API tested: sigprocmask()
// Description: To unblock the signals which is not blocked and validated using sigisemptyset()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask7 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set, oset;
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
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = emptysignalmask(set);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
			goto close;	
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigprocmask(SIG_UNBLOCK,&set,NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Unable to unblock the signal %d from the process's signal mask which is not present"),Signum);
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}	 
		ret = sigprocmask(SIG_UNBLOCK,NULL,&oset); //to get the process's signal mask
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Unable to retrieve the process's signal mask"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}	 
		ret = sigisemptyset(&oset);
		if(ret != 1)
			{
			ERR_PRINTF2(_L("Failed to validate sigprocmask() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("The process's signal mask for all signals remains same validated using sigismember()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigprocmask8
// Test Case ID: OPENENV-LIBC-CIT-5966
// API tested: sigprocmask()
// Description: Attempting to block the signals which cannot be blocked using sigprocmask() 
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigprocmask8 (  )
	{
	int ret, ret1 = KErrGeneral, Signum;
	sigset_t set;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = emptysignalmask(set);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
		goto close;	
		}
	ret = setsignaltomask(set,Signum);
	if(ret == 0)
		{
		ERR_PRINTF2(_L("Unable to add a signal %d to the process's signal mask"),Signum) ;
		goto close;
		}
	INFO_PRINTF2(_L("Blocked a signal %d in the process's signal mask using sigprocmask()"),Signum);
	ret = sigprocmask(SIG_SETMASK,NULL,&set);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Failed to get the process's signal mask set"));
		ERR_PRINTF2(_L("errno is set to %d"),errno);
		goto close;
		}
	ret =  sigisemptyset(&set);
	if(ret != 1)
		{
		ERR_PRINTF1(_L("failed to validate sigprocmask() on negative test"));
		goto close;
		}
	INFO_PRINTF2(_L("Blocked a signal %d in the process's signal mask using sigprocmask()"),Signum);
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigfillset1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigfillset()
// Description: To initialize signal set to contain all signals
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigfillset1 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	ret = sigfillset(&set);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Sigfillset() failed to return 0"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		ret = sigismember(&set,Signum);
		if(ret != 1)
			{
			ERR_PRINTF1(_L("Signum is not a member of set"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("Completely initialized the signal set using sigfillset()"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigfillset2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigfillset()
// Description: Negative test attempting to set all the signals on set pointed to a NULL pointer.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigfillset2 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = sigfillset(NULL);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigfillset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		} 	 
	INFO_PRINTF1(_L("sigfillset() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;

close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigandset1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigandset()
// Description: To evaluate the intersection of two sets
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigandset1 (  )
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set, set1, set2;
	ret = sigemptyset(&set1);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	ret = sigfillset(&set2);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Failed to initialize the set"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	ret = sigandset(&set,&set1,&set2);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("sigandset() failed to perform intersection of two sets"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	ret = sigisemptyset(&set);
	if(ret != 1)
		{
		ERR_PRINTF1(_L("Failed to validate sigandset()"));
		goto close;	
		}
	INFO_PRINTF1(_L("Successfully able to validate sigandset()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigandset2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigandset()
// Description: To evaluate the intersection of two sets containing more than one signal.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigandset2 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum1, Signum2, Signum3;
	sigset_t set, set1, set2;
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
	for (Signum1=Sigval; Signum1 <= Maxsig; Signum1++)
		{
		for (Signum2=Sigval; Signum2 <= Maxsig; Signum2++)
			{
			for (Signum3=Sigval; Signum3 <= Maxsig; Signum3++)
				{
				if(Signum1 == Signum3)
					{
					continue;
					}
				ret = emptyset(set1,set2);
				if(ret == 0)
					{
					ERR_PRINTF1(_L("failed to empty the sets"));
					goto close;
					}
				ret = addsignaltoset(set1,Signum1,Signum2);
				if(ret == 0)
					{
					ERR_PRINTF3(_L("failed to add signals %d and %d to the set1"),Signum1,Signum2);
					goto close;
					}
				ret = addsignaltoset(set2,Signum2,Signum3);
				if(ret == 0)
					{
					ERR_PRINTF3(_L("failed to add signals %d and %d to the set2"),Signum2,Signum2);
					goto close;
					}
				ret = sigandset(&set,&set1,&set2);
				if(ret != 0)
					{
					ERR_PRINTF1(_L("failed to perform intersection of two sets"));
					goto close;
					}
				ret = checksignalinset(set,Signum2);
				if(ret == 0)
					{
					ERR_PRINTF1(_L("failed to validate the resultant set as it contains more than one signal"));
					goto close;
					}	
				}
			}
		}
	INFO_PRINTF1(_L("resultant set of sigandset() successfully contains only one signal"));
	INFO_PRINTF1(_L("Intersection of two sets is performed successfully"));	
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigandset3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigandset()
// Description: To evaluate the intersection of two sets with the resultant set been non empty 
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigandset3 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set, set1, set2;
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
		ret = sigemptyset(&set1);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigfillset(&set2);
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Failed to initialize the set"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the resultant set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigaddset(&set,Signum);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Faile to add a signal %d to the resultant set"),Signum);
			goto close;
			}
		ret = sigandset(&set,&set1,&set2);
		if(ret != 0)
			{
			ERR_PRINTF1(_L("sigandset() failed to perform intersection of two sets"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = sigisemptyset(&set);
		if(ret != 1)
			{
			ERR_PRINTF2(_L("Failed to validate sigandset() for signal %d"),Signum);
			goto close;	
			}
		}
	INFO_PRINTF1(_L("Intersection of two sets is performed successfully"));
	INFO_PRINTF1(_L("Successfully able to validate sigandset() with all signals added to resultant set"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigorset1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigorset()
// Description: To evaluate the union of two sets
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigorset1 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, Sigval;
	sigset_t set, set2, set1;
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
		ret = sigemptyset(&set2);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set2 and errno returned is %d"),errno);
			goto close;
			}
		ret = sigaddset(&set2,Signum);
		if (ret != 0)
			{
			ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
			goto close;
			}
		ret = sigemptyset(&set1);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set1 and errno returned is %d"),errno);
			goto close;
			}
		ret = sigorset(&set,&set1,&set2);
		if(ret != 0)
			{
			ERR_PRINTF1(_L("sigorset() failed to perform union of two sets"));
			ERR_PRINTF2(_L("the errno is set to %d"),errno);
			goto close;
			}
		ret = checksignalinset(set,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("failed to validate the resultant set as it contains more than one signal"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("Union of two sets is performed successfully using sigorset()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigorset2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigorset()
// Description: To evaluate the union of two sets  containing more than one signal.
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigorset2 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum1, Signum2, Signum3;
	sigset_t set, set1, set2;
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
	for (Signum1=Sigval; Signum1 <= Maxsig; Signum1++)
		{
		for (Signum2=Sigval; Signum2 <= Maxsig; Signum2++)
			{
			for (Signum3=Sigval; Signum3 <= Maxsig; Signum3++)
				{
				if(Signum1 == Signum3)
					{
					continue;
					}
				ret = emptyset(set1,set2);
				if(ret == 0)
					{
					ERR_PRINTF1(_L("failed to empty the sets"));
					goto close;
					}
				ret = addsignaltoset(set1,Signum1,Signum2);
				if(ret == 0)
					{
					ERR_PRINTF3(_L("failed to add signals %d and %d to the set1"),Signum1,Signum2);
					goto close;
					}
				ret = addsignaltoset(set2,Signum2,Signum3);
				if(ret == 0)
					{
					ERR_PRINTF3(_L("failed to add signals %d and %d to the set2"),Signum2,Signum3);
					goto close;
					}
				ret = sigorset(&set,&set1,&set2);
				if(ret != 0)
					{
					ERR_PRINTF1(_L("failed to perform intersection of two sets"));
					goto close;
					}
				if(!(sigismember(&set,Signum1)) || !(sigismember(&set,Signum2)) || !(sigismember(&set,Signum3)))
					{
					ERR_PRINTF1(_L("failed to validate the resultant set as it contains more than one signal"));
					goto close;
					}
				}
			}
		}
	INFO_PRINTF1(_L("Intersection of two sets is performed successfully"));	
	ret1 = KErrNone;

close:
	return ret1;
	}
// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigorset3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigorset()
// Description: To evaluate the union of two sets when the resultant set is non empty
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigorset3 (  )
	{
	int ret, ret1 = KErrGeneral, Signum, Maxsig, i, Sigval;
	sigset_t set, set2, set1;
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
		for( i=1; i<Maxsig; i++)
			{
			ret = sigemptyset(&set2);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Unable to empty the set2 and errno returned is %d"),errno);
				goto close;
				}
			ret = sigaddset(&set2,Signum);
			if (ret != 0)
				{
				ERR_PRINTF3(_L("Failed to add a signal %d to the set and errno returned is %d"),Signum,errno);
				goto close;
				}
			ret = sigemptyset(&set1);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Unable to empty the set1 and errno returned is %d"),errno);
				goto close;
				}
			ret = sigemptyset(&set);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Unable to empty the resultant set and errno returned is %d"),errno);
				goto close;
				}
			ret = sigaddset(&set,i);
			if (ret != 0)
				{
				ERR_PRINTF2(_L("Faile to add a signal %d to the resultant set"),Signum);
				goto close;
				}		
			ret = sigorset(&set,&set1,&set2);
			if(ret != 0)
				{
				ERR_PRINTF1(_L("sigorset() failed to perform union of two sets"));
				ERR_PRINTF2(_L("the errno is set to %d"),errno);
				goto close;
				}
			ret = checksignalinset(set,Signum);
			if(ret == 0)
				{
				ERR_PRINTF1(_L("failed to validate the resultant set as it contains more than one signal"));
				goto close;
				}
			}
		}
	INFO_PRINTF1(_L("Union of two sets is performed successfully using sigorset()"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testandorset1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigandset(), sigorset()
// Description: Negative tests on sigandset() and sigorset() with an invalid 3 parameter
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testandorset1 (  )
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set, set1;
	ret = sigemptyset(&set1);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to empty the set"));
	INFO_PRINTF1(_L("Negative tests with an invalid third parameter"));
	ret = sigandset(&set,&set1,NULL);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigandset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigandset() returned EINVAL on negative test"));
	ret = sigorset(&set,&set1,NULL);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigorset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigorset() returned EINVAL on negative test"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testandorset2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigandset(), sigorset()
// Description: Negative tests on sigandset() and sigorset() with an invalid second parameter
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testandorset2 (  )
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set, set1;
	ret = sigemptyset(&set1);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to empty the set"));
	INFO_PRINTF1(_L("Negative tests with an invalid second parameter"));
	ret = sigandset(&set,NULL,&set1);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigandset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigandset() returned EINVAL on negative test"));
	ret = sigorset(&set,NULL,&set1);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigorset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigorset() returned EINVAL on negative test"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testandorset3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigandset(), sigorset()
// Description: Negative tests on sigandset() and sigorset() with an invalid resultant parameter
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testandorset3 (  )
	{
	int ret, ret1 = KErrGeneral;
	sigset_t set, set1;
	ret = sigemptyset(&set1);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to empty the set"));
	INFO_PRINTF1(_L("Negative tests with an invalid resultant parameter"));
	ret = sigandset(NULL,&set,&set1);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigandset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigandset() returned EINVAL on negative test"));
	ret = sigorset(NULL,&set,&set1);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("sigorset() failed on negative test"));
		ERR_PRINTF2(_L("the errno is set to %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("sigorset() returned EINVAL on negative test"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsighold1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sighold()
// Description: Test case to add a signal to the calling process's signal mask using sighold() 
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsighold1 (  )
	{
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	sigset_t oset, set;
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
	INFO_PRINTF1(_L("Test on sighold()"));
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigprocmask(SIG_SETMASK,&set,NULL); //delete all the signals from the process's signal mask
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Uable to delete all the signals from the process's signal mask"));
			ERR_PRINTF2(_L("errno is set to %d"),errno);
			goto close;
			}
		ret = sighold(Signum);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigprocmask(SIG_SETMASK,NULL,&oset);  //to get process's signal mask
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask set for signal %d"),Signum);
			ERR_PRINTF2(_L("Errno is set to %d"),errno);
			goto close;
			}
		ret = checksignalinset(oset,Signum);
		if(ret == 0)
			{
			ERR_PRINTF1(_L("failed to validate the resultant set as it contains more than one signal"));
			goto close;
			}
		}
	INFO_PRINTF1(_L("Successfully able to add every signal to the process's signal mask"));
	ret1 = KErrNone;
	
close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsighold2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sighold()
// Description: Attempting to block the signals which cannot be blocked using sighold() 
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsighold2 (  )
	{
	int ret, ret1 = KErrGeneral, Signum;
	sigset_t set, oset;
	ret = GetIntFromConfig(ConfigSection(), _L("Signum"), Signum);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}
	ret = sigemptyset(&set);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
		goto close;
		}
	ret = sigprocmask(SIG_SETMASK,&set,NULL); //delete all the signals from the process's signal mask
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Uable to delete all the signals from the process's signal mask"));
		ERR_PRINTF2(_L("errno is set to %d"),errno);
		goto close;
		}
	ret = sighold(Signum);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("sighold() failed on negative test"));
		goto close;
		}
	ret = sigprocmask(SIG_SETMASK,NULL,&oset);
	if (ret != 0)
		{
		ERR_PRINTF1(_L("Failed to get the process's signal mask set"));
		ERR_PRINTF2(_L("errno is set to %d"),errno);
		goto close;
		}
	ret =  sigisemptyset(&oset);
	if(ret != 1)
		{
		ERR_PRINTF1(_L("failed to validate sighold()"));
		goto close;
		}
	INFO_PRINTF2(_L("Blocked a signal %d in the process's signal mask using sighold()"),Signum);
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigrelse1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigrelse()
// Description: To remove valid signal from the calling process's signal mask using sigrelse().
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigrelse1 (  )
	{
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	sigset_t oset,set;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
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
	INFO_PRINTF1(_L("Test on sigrelse()"));
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = setsignaltomask(set,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		ret = sigrelse(Signum);
		if(ret != 0)
			{
			ERR_PRINTF1(_L("Failed to remove a signal from process's signal mask"));
			goto close;
			} 
		ret = sigprocmask(SIG_SETMASK,NULL,&oset);  //to get process's signal mask
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask set for signal %d"),Signum);
			ERR_PRINTF2(_L("Errno is set to %d"),errno);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if(ret != 1)
			{
			ERR_PRINTF2(_L("Failed to validate sigrelse() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigrelse() successfully able to remove all signals from process's signal mask"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigrelse2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigrelse()
// Description: Negative Test: To remove valid signal from the calling process's signal mask using sigrelse() which was not blocked. 
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigrelse2 (  )
	{
	int ret, Signum, ret1 = KErrGeneral, Maxsig, Sigval;
	sigset_t oset,set;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
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
	INFO_PRINTF1(_L("Test on sigrelse()"));
	for (Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		ret = sigemptyset(&set); 
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Unable to empty the set and errno returned is %d"),errno);
			goto close;
			}
		ret = sigprocmask(SIG_SETMASK,&set,NULL);
		if (ret != 0)
			{
			ERR_PRINTF1(_L("Unable to empty the process's signal mask set"));
			ERR_PRINTF2(_L("errno is set to %d"),errno);
			goto close;
			}
		ret = sigrelse(Signum);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to remove a signal %d from process's signal mask"),Signum);
			goto close;
			} 
		ret = sigprocmask(SIG_SETMASK,NULL,&oset);  //to get process's signal mask
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask set for signal %d"),Signum);
			ERR_PRINTF2(_L("Errno is set to %d"),errno);
			goto close;
			}
		ret = sigisemptyset(&oset);
		if(ret != 1)
			{
			ERR_PRINTF2(_L("Failed to validate sigrelse() for signal %d"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigrelse() is success on negative test for all signals"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testholdnrelse1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sighold(),sigrelse()
// Description: Negative test on sighold() and sigrelse() 
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testholdnrelse1 (  )
	{
	int ret, ret1 = KErrGeneral, Invalsig;
	ret = GetIntFromConfig(ConfigSection(),_L("Invalsig"),Invalsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read an invalid signal number")) ;
		goto close;
		}
	ret = sighold(Invalsig);
	if((ret != -1) ||  (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Negative test failed on sighold()"));
		goto close;
		}
	INFO_PRINTF1(_L("sighold() successfully returned EINVAL on negative test"));
	ret = sigrelse(Invalsig);
	if((ret != -1) ||  (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Negative test failed on sigrelse()"));
		goto close;
		}	
	INFO_PRINTF1(_L("sigrelse() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testsigpause1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigpause()
// Description: To unblock the masked signal using sigpause()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testsigpause1 (  )
	{
	int ret, ret1 = KErrGeneral, Sigval, Maxsig, Signum, pid, pid1, status;
	sigset_t set, oset;
	TTime time1, time2;
	TTimeIntervalSeconds aInterval;
	char **argv=(char**)malloc(4*sizeof(char*));
	argv[0]=(char*)malloc(34*sizeof(char));	//child process’s name
	argv[1]=(char*)malloc(6*sizeof(char));	//calling process’s pid
	argv[2]=(char*)malloc(3*sizeof(char));	//signal to be raised
	argv[3]= 0;
	strcpy(argv[0], "z:\\sys\\bin\\sigpause.exe");
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the signal number")) ;
		goto close;
		}	
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read the maximum signal number")) ;
		goto close;
		}
	for (Signum = Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
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
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		if (signal(Signum,sigpausehandler) == SIG_ERR)
			{
			ERR_PRINTF3(_L("Error in signal trapping function for signal %d and errno is set to %d"),Signum,errno);
			goto close;
			}
		sprintf(argv[1], "%d", getpid());
		sprintf(argv[2], "%d", Signum);
		ret = posix_spawn(&pid, "z:\\sys\\bin\\sigpause.exe", NULL, NULL, argv, (char**)NULL);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Error in posix spawn and errno is set to %d"),errno);
			goto close;
			}
		sigvalue = 0;
		time1.HomeTime();
		ret = sigpause(Signum);
		time2.HomeTime();
		time2.SecondsFrom(time1,aInterval);
		if((ret != -1) || (errno != EINTR)) //Time check removed because timing of posix-spawn can't be guaranted 
			{
			ERR_PRINTF4(_L("sigpause() return value is %d and errno is %d and time is %d"),ret,errno,aInterval.Int());
			ERR_PRINTF2(_L("sigpause() failed to suspend for 2 secs for signal %d"),Signum);
			goto close;
			} 
		pid1 = waitpid(pid, &status, WUNTRACED);
		if (pid1 != pid)
			{
			ERR_PRINTF2(_L("waitpid failed... for signal %d"),Signum);	
			goto close;
			}
		if(sigvalue != Signum)
			{
			ERR_PRINTF2(_L("Failed to enter the signal handler for signal %d"),Signum);
			goto close;			
			}
		ret = sigprocmask(SIG_SETMASK,NULL,&oset);  //to get process's signal mask
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Failed to retrieve the process's signal mask set for signal %d"),Signum);
			ERR_PRINTF2(_L("Errno is set to %d"),errno);
			goto close;
			}
		ret = checksignalinset(oset,Signum);
		if (ret == 0)
			{
			ERR_PRINTF2(_L("Failed to add a signal %d to the process's signal mask"),Signum);
			goto close;
			}
		}
	INFO_PRINTF1(_L("sigpause() successfully able to validate for all signals"));
	ret1 = KErrNone;

	close:
	free((void*)argv[0]);
	free((void*)argv[1]);
	free((void*)argv[2]);
	free((void*)argv);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTestblocksignal::Testnegsigpause1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: sigpause()
// Description: Trying to unblock an invalid signal using sigpause()
// -----------------------------------------------------------------------------

TInt CTestBlocksignal::Testnegsigpause1 (  )
	{
	int ret, ret1 = KErrGeneral, Invalsig;
	ret = GetIntFromConfig(ConfigSection(),_L("Invalsig"),Invalsig);
	if (ret == 0)
		{
		ERR_PRINTF1(_L("Failed to read an invalid signal number")) ;
		goto close;
		}
	ret = sigpause(Invalsig);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF1(_L("Negative test failed on sigpause()"));
		goto close;
		}
	INFO_PRINTF1(_L("sigpause() successfully returned EINVAL on negative test"));
	ret1 = KErrNone;
	
	close:
	return ret1;
	}

//End of file
