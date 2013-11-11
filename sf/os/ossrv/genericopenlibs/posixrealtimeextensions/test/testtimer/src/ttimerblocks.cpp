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
// Name        : ttimerblocks.cpp
// Test cases for blocking signal api's
//



#include "ttimer.h"
#include "ttimerthread.h"
#define Maxtimerid 512
int timer_value = 0;

RSemaphore iLock;
void CreateLock()
	{
	iLock.CreateLocal(0);
	}
void ReleaseLock()
	{
	iLock.Signal();
	}
void AcquireLock()
	{
	iLock.Wait();
	}

//signal handler
void sig_handler(int sig)
	{
	timer_value = sig;
	ReleaseLock();
	}

//overrun signal handler
void overrun_handler(int sig)
	{
	timer_value = sig;
	sleep(1);
	}

//notification function
void notifyfunc(union sigval val)
	{
	timer_value = val.sival_int;
	}

void alarm_handler(int sig)
	{
	timer_value = sig;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi1
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: Test case to create a timer with NONE as sigev_notify parameter
// TIMER_ABSTIME not set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi1 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
		return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	sigev.sigev_notify = SIGEV_NONE;
	timer_t timerid;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);

	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and  errno is %d"),errno);
     	return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec.it_value.tv_sec = Valuesec;
	timerspec.it_value.tv_nsec = Valuenanosec;
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,0,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
     	ret1 = -1;
     	goto close;			
		}
	sleep(2);
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi2
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: Test case to create a timer with NONE as sigev_notify parameter
// TIMER_ABSTIME set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi2 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec;
	struct timespec curtmspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	sigev.sigev_notify = SIGEV_NONE;
	timer_t timerid;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and  errno is %d"),errno);
     	return -1;	
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	ret = clock_gettime(CLOCK_REALTIME,&curtmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}
	timerspec.it_value.tv_sec = curtmspec.tv_sec + Valuesec;
	timerspec.it_value.tv_nsec = curtmspec.tv_nsec + Valuenanosec; 
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,TIMER_ABSTIME,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"), errno);
		ret1 = -1;
     	goto close;				
		}
	sleep(2);
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;
     	}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid); 
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi3
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer with SIGEV_SIGNAL as sigev_notify parameter
// TIMER_ABSTIME not set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi3 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec, Signum, Maxsig, Sigval;
	struct sigevent sigev;
	struct itimerspec timerspec;
	timer_t timerid;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read signal number")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read maximum signal number")) ;
	 	return -1;
	  	}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		timer_value = 0;
		if(signal(Signum,sig_handler) == SIG_ERR)
			{
		 	ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"), errno) ;
		 	return ret1;		
			}
		sigev.sigev_notify = SIGEV_SIGNAL;
		sigev.sigev_signo = Signum;

		ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
	     	return -1;		
			}
		timerspec.it_value.tv_sec = Valuesec;
		timerspec.it_value.tv_nsec = Valuenanosec;
		timerspec.it_interval.tv_sec = Intervalsec;
		timerspec.it_interval.tv_nsec = Intervalnanosec;
		timer_value = 0;
		CreateLock();
		ret = timer_settime(timerid,0,&timerspec,NULL);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to set the timer for signal %d and errno is %d"),Signum,errno);
			ret1 = -1;
	     	goto close;				
			}
//		sleep(2);
		
		AcquireLock();
		if(timer_value != Signum)
			{
			ERR_PRINTF2(_L("The expected and timer_value are not same for signal %d"),Signum);
			goto close;
			}
		ret = timer_delete(timerid);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to delete the timer for signal %d and errno is %d"),Signum, errno);
	     	return -1;				
			}
		}
	INFO_PRINTF1(_L("Relative time"));
		INFO_PRINTF1(_L("Successfully able to create a timer with SIGEV_SIGNAL as sigev_notify parameter validated for all signals"));
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi4
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer with SIGEV_SIGNAL as sigev_notify parameter
// TIMER_ABSTIME set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi4 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec, Signum, Maxsig, Sigval;
	struct sigevent sigev;
	struct itimerspec timerspec;
	struct timespec curtmspec;
	timer_t timerid;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read signal number")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read maximum signal number")) ;
	 	return ret1;
	  	}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		timer_value = 0;
		if(signal(Signum,sig_handler) == SIG_ERR)
			{
		 	ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"), errno) ;
		 	return ret1;		
			}
		sigev.sigev_notify = SIGEV_SIGNAL;
		sigev.sigev_signo = Signum;

		ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to create a timer for signal %d and errno is %d"),Signum,errno);
	     	return -1;	
			}
		ret = clock_gettime(CLOCK_REALTIME,&curtmspec);
		if (ret != 0)
			{
		 	ERR_PRINTF3(_L("Failed to get the time of specified clock id for signal %d and errno is %d"),Signum,errno);
		 	goto close;	
			}
		timerspec.it_value.tv_sec = curtmspec.tv_sec + Valuesec;
		timerspec.it_value.tv_nsec = curtmspec.tv_nsec + Valuenanosec;
		timerspec.it_interval.tv_sec = Intervalsec;
		timerspec.it_interval.tv_nsec = Intervalnanosec;
		timer_value = 0;
		ret = timer_settime(timerid,TIMER_ABSTIME,&timerspec,NULL);
		//sleep(2);
		AcquireLock();
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to set the timer for signal %d and errno is %d"),Signum,errno);
			ret1 = -1;
	     	goto close;				
			}
		if(timer_value != Signum)
			{
			ERR_PRINTF2(_L("The expected and timer_value are not same for signal %d"),Signum);
	     	goto close;				
			}
		ret = timer_delete(timerid);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to delete the timer for signal %d and errno is %d"),Signum, errno);
	     	return -1;				
			}
		}
	INFO_PRINTF1(_L("Absolute time"));
	INFO_PRINTF1(_L("Successfully able to create a timer with SIGEV_SIGNAL as sigev_notify parameter validated for all signals"));
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}


// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi5
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer with SIGEV_THREAD as sigev_notify parameter
// TIMER_ABSTIME not set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi5 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct sched_param parm;	 
	pthread_attr_t attr;
	struct itimerspec timerspec;
	timer_t timerid;
	pthread_attr_init( &attr );
	parm.sched_priority = 255; //raise the priority..
	pthread_attr_setschedparam(&attr, &parm);
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	timer_value = 0;
	sigev.sigev_notify = SIGEV_THREAD;
	sigev.sigev_value.sival_int = 100;
	sigev.sigev_notify_function = notifyfunc ;
	sigev.sigev_notify_attributes = &attr;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec.it_value.tv_sec = Valuesec;
	timerspec.it_value.tv_nsec = Valuenanosec;
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,0,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
		ret1 = -1;
		goto close;				
		}
	sleep(2);
	if(timer_value != 100)
		{
		ERR_PRINTF1(_L("The expected and timer_value are not same"));
		goto close;				
		}
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
		return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi6
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer with SIGEV_THREAD as sigev_notify parameter
// TIMER_ABSTIME set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi6 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec;
	struct timespec curtmspec;
	timer_t timerid;
	struct sched_param parm;	
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	parm.sched_priority = 255;
	pthread_attr_setschedparam(&attr, &parm);
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	timer_value = 0;
	sigev.sigev_notify = SIGEV_THREAD;
	sigev.sigev_value.sival_int = 100;
	sigev.sigev_notify_function = notifyfunc ;
	sigev.sigev_notify_attributes = &attr;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	ret = clock_gettime(CLOCK_REALTIME,&curtmspec);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
		goto close;	
		}
	timerspec.it_value.tv_sec = curtmspec.tv_sec + Valuesec;
	timerspec.it_value.tv_nsec = curtmspec.tv_nsec + Valuenanosec; 
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,TIMER_ABSTIME,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
		ret1 = -1;
		goto close;				
		}
	sleep(2); 
	if(timer_value != 100)
		{
		ERR_PRINTF1(_L("The expected and timer_value are not same"));
		goto close;				
		}
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
		return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi7
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer for CLOCK_REALTIME id using timer_create() with sigevent argument as NULL
// TIMER_ABSTIME not set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi7 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	timer_t timerid;
	struct itimerspec timerspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return -1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return -1;
	  	}
	if(signal(SIGALRM, alarm_handler) == SIG_ERR)
		{
	 	ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"), errno) ;
	 	return ret1;				
		}
	ret = timer_create(CLOCK_REALTIME,NULL,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec.it_value.tv_sec = Valuesec;
	timerspec.it_value.tv_nsec = Valuenanosec;
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,0,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
		ret1 = -1;
		goto close;				
		}
	sleep(2);
	if(timer_value != SIGALRM)
		{
		ERR_PRINTF1(_L("Error in raising the signal after timer expire"));
		goto close;	
		}
	INFO_PRINTF1(_L("Successfully able to set the timer with the default signal as SIGALRM") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi8
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer for CLOCK_REALTIME id using timer_create() with sigevent argument as NULL
// TIMER_ABSTIME set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi8 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	timer_t timerid;
	struct itimerspec timerspec;
	struct timespec curtmspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	if(signal(SIGALRM, alarm_handler) == SIG_ERR)
		{
	 	ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"),errno) ;
	 	return ret1;				
		}
	ret = timer_create(CLOCK_REALTIME,NULL,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	ret = clock_gettime(CLOCK_REALTIME,&curtmspec);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
		goto close;	
		}
	timerspec.it_value.tv_sec = curtmspec.tv_sec + Valuesec;
	timerspec.it_value.tv_nsec = curtmspec.tv_nsec + Valuenanosec; 
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,TIMER_ABSTIME,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
		ret1 = -1;
		goto close;				
		}
	sleep(2);
	if(timer_value != SIGALRM)
		{
		ERR_PRINTF1(_L("Error in raising the signal after timer expire"));
		goto close;	
		}
	INFO_PRINTF1(_L("Successfully able to set the timer with the default signal as SIGALRM") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi9
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create()
// Description: Negative Test-Trying to create the timer for an invalid clock id using timer_create()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi9 (  )
	{
	int ret, ret1 = KErrGeneral, Invalidclockid;
	timer_t timerid;
	ret = GetIntFromConfig(ConfigSection(), _L("Invalidclockid"), Invalidclockid);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = timer_create(Invalidclockid,NULL,&timerid);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_create() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;		
		}
	INFO_PRINTF1(_L("timer_create() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi10
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create()
// Description: Timer id as NULL in timer_create()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi10 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = timer_create(CLOCK_REALTIME,NULL,NULL);
	if((ret != -1) || (errno != EFAULT))
		{
		ERR_PRINTF2(_L("timer_create() failed to return EFAULT on negative test and errno is %d"),errno);
     	goto close;				
		}
	INFO_PRINTF1(_L("timer_create() successfully returned EFAULT on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi11
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create()
// Description: Timer id as NULL & an invalid clockid in timer_create()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi11 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = timer_create(CLOCK_MONOTONIC,NULL,NULL);
	if((ret != -1) || (errno != EFAULT))
		{
		ERR_PRINTF2(_L("timer_create() failed to return EFAULT on negative test and errno is %d"),errno);
     	goto close;				
		}
	INFO_PRINTF1(_L("timer_create() successfully returned EFAULT on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi12
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create()
// Description: Trying to create more than maximum no. of timer's possible per process using timer_create()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi12 (  )
	{
	int ret, ret1 = KErrGeneral, i, j;
	timer_t timerid[Maxtimerid + 1];
	for (i = 0; i <= Maxtimerid ; i++)
		{
		ret = timer_create(CLOCK_REALTIME,NULL,&timerid[i]);
		if((ret == -1) && (errno == EAGAIN) && (i == Maxtimerid) )
			{
			INFO_PRINTF3(_L("timer_create() has successfully returned EAGAIN on negative test %d %d"),errno,i);
			ret1 = KErrNone;
			break;		
			}
		else if(ret < 0)
			{
			INFO_PRINTF3(_L("timer_create() has failed with error %d %d"),errno,i);
			}
		}
	for(j = 0; j <= Maxtimerid-1; j++)
		{
		ret = timer_delete(timerid[j]);
		if(ret != 0)
			{			
			ERR_PRINTF3(_L("Failed to delete the timer and errno is %d %d"),errno, j);
			ret1 = KErrGeneral;		
			break;
			}
		}
	INFO_PRINTF1(_L("timer_delete() has successfully deleted all the timers negative test"));
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi13
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_delete()
// Description: To delete the timer with an invalid timer id using timer_delete()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi13 (  )
	{
	timer_t timerid;
	int ret, ret1 = KErrGeneral;
	ret = timer_create(CLOCK_REALTIME,NULL,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("Able to create the timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	/*Deleting a timer id again expected result = -1 and errno = EINVAL  */
	ret = timer_delete(timerid);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_delete() failed to return EINVAL on negative test and errno is %d"),errno);
     	goto close;				
		}
	INFO_PRINTF1(_L("timer_delete() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi14
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_delete()
// Description: To delete the timer with NULL using timer_delete()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi14 (  )
	{
	int ret, ret1 = KErrGeneral;
	ret = timer_delete(NULL);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_delete() failed to return EFAULT on negative test and errno is %d"),errno);
     	goto close;				
		}
	INFO_PRINTF1(_L("timer_delete() successfully returned EFAULT on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi15
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime()
// Description: Trying to set the timer for an invalid timer id using timer_settime()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi15 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec, Timerflag;
	timer_t timerid;
	struct itimerspec timerspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Timerflag"), Timerflag);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read the timer flag")) ;
	 	return ret1;
	  	}
	if(signal(SIGALRM, alarm_handler) == SIG_ERR)
		{
	 	ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"), errno) ;
	 	return ret1;				
		}
	ret = timer_create(CLOCK_REALTIME,NULL,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	timerspec.it_value.tv_sec = Valuesec;
	timerspec.it_value.tv_nsec = Valuenanosec;
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,Timerflag,&timerspec,NULL);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_settime() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;				
		}
	INFO_PRINTF1(_L("timer_settime() successfully able to return EINVAL on negative test") );
	ret1 = KErrNone;
	
	close:
	RThread athr;
	if(athr.Open(_L("LibrtTimerServ")) == KErrNone)
		{
		athr.Close();
		}
	
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi16
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime()
// Description: Trying to set the timer with an invalid tv_nsec parameter using timer_settime()
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi16 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec, Signum, Maxsig, Timerflag, Sigval;
	struct sigevent sigev;
	struct itimerspec timerspec;
	timer_t timerid;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value"));
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read signal number")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read maximum signal number")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Timerflag"), Timerflag);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read the timer flag")) ;
	 	return ret1;
	  	}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		timer_value = 0;
		if(signal(Signum,sig_handler) == SIG_ERR)
			{
		 	ERR_PRINTF3(_L("Error in signal trapping function for signal is %d and errno is %d"),Signum, errno) ;
		 	return ret1;		
			}
		sigev.sigev_notify = SIGEV_SIGNAL;
		sigev.sigev_signo = Signum;
		ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to create a timer %d and errno is %d"),Signum, errno);
			return -1;
			}
		timerspec.it_value.tv_sec = Valuesec;
		timerspec.it_value.tv_nsec = Valuenanosec;
		timerspec.it_interval.tv_sec = Intervalsec; 
		timerspec.it_interval.tv_nsec = Intervalnanosec; 
		ret = timer_settime(timerid,Timerflag,&timerspec,NULL);
		if((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF2(_L("timer_settime() failed to return EINVAL on negative test and errno is %d"),errno);
			goto close;
			}
		ret = timer_delete(timerid);
		if(ret != 0)
			{
			ERR_PRINTF3(_L("Failed to delete the timer %d and errno is %d"),Signum,errno);
			return -1;
			}
		}
	INFO_PRINTF1(_L("timer_settime() successfully able to return EINVAL on negative test") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi17
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer with SIGEV_SIGNAL as sigev_notify parameter
// TIMER_ABSTIME not set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi17 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	timer_value = 0;
	if(signal(SIGUSR1,overrun_handler) == SIG_ERR)
		{
		ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"), errno) ;
		return ret1;
		}
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	timer_t timerid;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;
		}
	timerspec.it_value.tv_sec = Valuesec;
	timerspec.it_value.tv_nsec = Valuenanosec;
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	timer_value = 0;
	ret = timer_settime(timerid,0,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the time of specified clock id and errno is %d "),errno);
		ret1 = -1;
		goto close;				
		}
	sleep(2);
	if(timer_value != SIGUSR1)
		{
		ERR_PRINTF1(_L("The expected and timer_value are not same for signal"));
		ret1 = -1;
		goto close;
		}
	ret = timer_getoverrun(timerid);
	if (ret != 0)
		{
		ERR_PRINTF2(_L("Error in timer_getoverrun() and errno is %d"),errno);
		ret1 = -1;
		goto close; 			
		}
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
		return -1;				
		}
	INFO_PRINTF1(_L("Relative Timer") );
	INFO_PRINTF1(_L("Successfully able to create a timer with SIGEV_SIGNAL as sigev_notify parameter") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi18
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create(), timer_delete()
// Description: To create a timer with SIGEV_SIGNAL as sigev_notify parameter
// TIMER_ABSTIME set
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi18 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec, oldtimerspec;
	struct timespec curtmspec;
	timer_t timerid;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
		timer_value = 0;
		if(signal(SIGUSR1,overrun_handler) == SIG_ERR)
			{
		 	ERR_PRINTF2(_L("Error in signal trapping function and errno is %d"), errno) ;
		 	return ret1;
			}
		sigev.sigev_notify = SIGEV_SIGNAL;
		sigev.sigev_signo = SIGUSR1;
		ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
			return -1;
			}
		ret = clock_gettime(CLOCK_REALTIME,&curtmspec);
		if (ret != 0)
			{
		 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
		 	ret1 = -1;
		 	goto close;	
			}
		timerspec.it_value.tv_sec = curtmspec.tv_sec + Valuesec;
		timerspec.it_value.tv_nsec = curtmspec.tv_nsec + Valuenanosec;
		timerspec.it_interval.tv_sec = Intervalsec;
		timerspec.it_interval.tv_nsec = Intervalnanosec;
		timer_value = 0;
		ret = timer_settime(timerid,TIMER_ABSTIME,&timerspec,&oldtimerspec);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to set the timer for signal and errno is %d"),errno);
			ret1 = -1;
	     	goto close;				
			}
		sleep(2);
		if(timer_value != SIGUSR1)
			{
			ERR_PRINTF2(_L("The expected and timer_value are not same for signal %d"),errno);
			goto close;
			}
		ret = timer_getoverrun(timerid);
		if (ret != 0)
			{
			ERR_PRINTF2(_L("Error in timer_getoverrun() and errno is %d"),errno);
			ret1 = -1;
	     	goto close;				
			}
		ret = timer_delete(timerid);
		if(ret != 0)
			{
			ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
			ret1 = -1;			
			}
	INFO_PRINTF1(_L("Absolute Timer") );
	INFO_PRINTF1(_L("Successfully able to create a timer with SIGEV_SIGNAL as sigev_notify parameter") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi19
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_overrun(), timer_gettime()
// Description: Negative test: timer_overrun(), timer_gettime() with an invalid timerid
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi19 (  )
	{
	timer_t timerid;
	int ret, ret1 = KErrGeneral;
	struct itimerspec timerspec;
	ret = timer_create(CLOCK_REALTIME,NULL,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno);
		return -1;		
		}
	INFO_PRINTF1(_L("successfully able to create the timer") );
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret = timer_gettime(timerid,&timerspec);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_gettime() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("timer_gettime() successfully returned EINVAL on negative test") );
	ret = timer_getoverrun(timerid);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_getoverrun() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;
		}
	INFO_PRINTF1(_L("timer_getoverrun() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi20
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create()
// Description: Negative test: timer_create() with an invalid signal
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi20 (  )
	{
	timer_t timerid;
	int ret, ret1 = KErrGeneral, Sigval;
	struct sigevent sigev;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read signal number")) ;
	 	return -1;
	  	}
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = Sigval;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);	
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_create() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;				
		}
	INFO_PRINTF1(_L("timer_create() with an invalid sigev_signo member") );
	INFO_PRINTF1(_L("timer_create() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi21
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_create()
// Description: Negative test: timer_create() with an invalid sigev_notify member
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi21 (  )
	{
	timer_t timerid;
	int ret, ret1 = KErrGeneral, Sigval, Signum, Maxsig, Notify;
	struct sigevent sigev;
	ret = GetIntFromConfig(ConfigSection(), _L("Sigval"), Sigval);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read signal number")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Maxsig"), Maxsig);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read maximum signal number")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Notify"), Notify);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read an invalid notify value")) ;
	 	return ret1;
	  	}
	for(Signum=Sigval; Signum <= Maxsig; Signum++)
		{
		if((Signum == SIGSTOP) || (Signum == SIGKILL))
			{
			continue;
			}
		sigev.sigev_notify = Notify;
		sigev.sigev_signo = Signum;
		ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);	
		if((ret != -1) || (errno != EINVAL))
			{
			ERR_PRINTF2(_L("timer_create() failed to return EINVAL on negative test and errno is %d"),errno);
			goto close;				
			}
		}
	INFO_PRINTF1(_L("timer_create() with an invalid sigev_notify member") );
	INFO_PRINTF1(_L("timer_create() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	
	close:
	return ret1;		
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi22
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime()
// Description: Negative test: timer_settime() with an input itimerspec value as NULL
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi22 (  )
	{
	int ret, ret1 = KErrGeneral;
	struct sigevent sigev;
	struct itimerspec timerspec;
	timer_t timerid;
	sigev.sigev_notify = SIGEV_NONE;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"), errno );
     	return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	ret = timer_settime(timerid,0,NULL,&timerspec);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_settime() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;				
		}
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	sleep(2);
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("timer_settime() with itimerspec input as NULL") );
	INFO_PRINTF1(_L("timer_settime() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi23
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime()
// Description: Negative test: timer_settime() with an invalid flag member
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi23 (  )
	{
	int ret, ret1 = KErrGeneral;
	struct sigevent sigev;
	struct itimerspec timerspec;
	timer_t timerid;
	sigev.sigev_notify = SIGEV_NONE;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"),errno );
     	return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec.it_value.tv_sec = 1;
	timerspec.it_value.tv_nsec = 0;
	timerspec.it_interval.tv_sec = 0;
	timerspec.it_interval.tv_nsec = 0;
	ret = timer_settime(timerid,-1,NULL,&timerspec);
	if((ret != -1) || (errno != EINVAL))
		{
		ERR_PRINTF2(_L("timer_settime() failed to return EINVAL on negative test and errno is %d"),errno);
		goto close;				
		}
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	sleep(3);
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("timer_settime() with an invalid flag value") );
	INFO_PRINTF1(_L("timer_settime() successfully returned EINVAL on negative test") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi24
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_gettime()
// Description: Negative test: timer_gettime() with an input itimerspec value as NULL
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi24 (  )
	{
	int ret, ret1 = KErrGeneral;
	struct sigevent sigev;
	struct itimerspec timerspec;
	timer_t timerid;
	timer_value = 0;
	sigev.sigev_notify = SIGEV_NONE;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF1(_L("Failed to create a timer") );
     	return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec.it_value.tv_sec = 1;
	timerspec.it_value.tv_nsec = 0;
	timerspec.it_interval.tv_sec = 1;
	timerspec.it_interval.tv_nsec = 0;
	ret = timer_settime(timerid,0,&timerspec,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
     	ret1 = -1;
     	goto close;				
		}
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_gettime(timerid,NULL);
	if((ret != -1) || (errno != EFAULT))
		{
		ERR_PRINTF2(_L("timer_gettime() failed to return EFAULT on negative test and errno is %d"),errno);
		goto close;				
		}	
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	ret1 = -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	INFO_PRINTF1(_L("timer_gettime() with an input itimerspec value as NULL") );
	INFO_PRINTF1(_L("timer_gettime() successfully returned EFAULT on negative test") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi25
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime(),timer_gettime()
// Description: To retreive the amount of time until the timer expires
// Relative timer
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi25 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec, gettime, oldtimerspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	sigev.sigev_notify = SIGEV_NONE;
	timer_t timerid;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"), errno );
     	return -1;	
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec.it_value.tv_sec = Valuesec;
	timerspec.it_value.tv_nsec = Valuenanosec;
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,0,&timerspec,&oldtimerspec);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"), errno );
     	ret1 = -1;
     	goto close;				
		}
	sleep(1);
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_gettime(timerid,&gettime);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to get the timer and the errno is %d"),errno );
     	ret1 = -1;
     	goto close;				
		}
	if((gettime.it_value.tv_sec > (timerspec.it_value.tv_sec-1)) || (gettime.it_value.tv_nsec > (timerspec.it_value.tv_nsec))|| (gettime.it_interval.tv_sec != timerspec.it_interval.tv_sec) || (gettime.it_interval.tv_nsec != timerspec.it_interval.tv_nsec))
		{
		ERR_PRINTF1(_L("Failed to get the timer value") );
		ret1 = -1;
     	goto close;
		}
	INFO_PRINTF1(_L("Successfully able to get the timer") );	
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi26
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime(),timer_gettime()
// Description: To retreive the amount of time until the timer expires
// Absolute timer
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi26 (  )
	{
	int ret, ret1 = KErrGeneral, Intervalsec, Intervalnanosec, Valuesec, Valuenanosec;
	struct sigevent sigev;
	struct itimerspec timerspec, gettime, oldtimerspec;
	struct timespec curtmspec;
	timer_value = 0;
	ret = GetIntFromConfig(ConfigSection(), _L("Valuesec"), Valuesec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Valuenanosec"), Valuenanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_value")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalsec"), Intervalsec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read seconds value of it_interval")) ;
	 	return ret1;
	  	}
	ret = GetIntFromConfig(ConfigSection(), _L("Intervalnanosec"), Intervalnanosec);
	if(ret == 0)
		{
	 	ERR_PRINTF1(_L("Unable to read nano seconds value of it_interval")) ;
	 	return ret1;
	  	}
	sigev.sigev_notify = SIGEV_NONE;
	timer_t timerid;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"), errno );
     	return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	ret = clock_gettime(CLOCK_REALTIME,&curtmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	ret1 = -1;
	 	goto close;	
		}
	timerspec.it_value.tv_sec = curtmspec.tv_sec + Valuesec;
	timerspec.it_value.tv_nsec = curtmspec.tv_nsec + Valuenanosec; 
	timerspec.it_interval.tv_sec = Intervalsec;
	timerspec.it_interval.tv_nsec = Intervalnanosec;
	ret = timer_settime(timerid,TIMER_ABSTIME,&timerspec,&oldtimerspec);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"), errno );
		ret1 = -1;
     	goto close;				
		}
	sleep(1);
	INFO_PRINTF1(_L("Successfully able to set the timer") );
	ret = timer_gettime(timerid,&gettime);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to get the timer and errno is %d"), errno );
     	ret1 = -1;
     	goto close;				
		}
	if((gettime.it_value.tv_sec > (Valuesec-1)) || (gettime.it_value.tv_nsec > (Valuenanosec))|| (gettime.it_interval.tv_sec != Intervalsec) || (gettime.it_interval.tv_nsec != Intervalnanosec))
		{
		ERR_PRINTF1(_L("Failed to get the timer value") );
		ret1 = -1;
     	goto close;
		}
	INFO_PRINTF1(_L("Successfully able to get the timer") );	
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;				
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;	
	}

// -----------------------------------------------------------------------------
// CTesttimer::Testtimerapi27
// Test Case ID: OPENENV-LIBC-CIT-5946
// API tested: timer_settime(),timer_gettime()
// Description: To retreive the amount of time until the timer expires using timer_settime()
// Absolute timer
// -----------------------------------------------------------------------------

TInt CTesttimer::Testtimerapi27 (  )
	{
	int ret, ret1 = KErrGeneral;
	struct sigevent sigev;
	struct itimerspec timerspec1, gettime, oldtimerspec;
	timer_value = 0;
	sigev.sigev_notify = SIGEV_NONE;
	timer_t timerid;
	ret = timer_create(CLOCK_REALTIME,&sigev,&timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to create a timer and errno is %d"), errno );
     	return -1;		
		}
	INFO_PRINTF1(_L("Successfully able to create a timer") );
	timerspec1.it_value.tv_sec = 5;
	timerspec1.it_value.tv_nsec = 0;
	timerspec1.it_interval.tv_sec = 0;
	timerspec1.it_interval.tv_nsec = 0;
	ret = timer_settime(timerid,0,&timerspec1,NULL);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"), errno );
     	ret1 = -1;				
     	goto close;
		}
	INFO_PRINTF1(_L("Successfully able to set the timer for the first time") );
	sleep(1);
	timerspec1.it_value.tv_sec = 3;
	timerspec1.it_value.tv_nsec = 0;
	timerspec1.it_interval.tv_sec = 0;
	timerspec1.it_interval.tv_nsec = 0;
	ret = timer_settime(timerid,0,&timerspec1,&oldtimerspec);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"), errno );
     	ret1 = -1;				
     	goto close;				
		}
	INFO_PRINTF1(_L("Successfully able to set the timer for the second time") );
	if(oldtimerspec.it_value.tv_sec > 4)
		{
		ERR_PRINTF1(_L("Failed to set the timer value") );
		goto close;
		}
	sleep(1);
	timerspec1.it_value.tv_sec = 0;
	timerspec1.it_value.tv_nsec = 0;
	timerspec1.it_interval.tv_sec = 0;
	timerspec1.it_interval.tv_nsec = 0;
	ret = timer_settime(timerid,0,&timerspec1,&oldtimerspec);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"),errno);
     	goto close;				
		}
	if(oldtimerspec.it_value.tv_sec > 2)
		{
		ERR_PRINTF1(_L("Failed to set the timer value") );
		goto close;
		}
	timerspec1.it_value.tv_sec = 2;
	timerspec1.it_value.tv_nsec = 0;
	timerspec1.it_interval.tv_sec = 5;
	timerspec1.it_interval.tv_nsec = 0;
	ret = timer_settime(timerid,0,&timerspec1,&oldtimerspec);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to set the timer and errno is %d"), errno);
		ret1 = -1;
     	goto close;				
		}
	if(oldtimerspec.it_value.tv_sec != 0)
		{
		ERR_PRINTF1(_L("Failed to set the timer value") );
		goto close;
		}
	ret = timer_gettime(timerid,&gettime);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to get the timer and errno is %d"),errno);
		ret1 = -1;
     	goto close;				
		}
	
	if((gettime.it_value.tv_sec > 2) || (gettime.it_interval.tv_sec != timerspec1.it_interval.tv_sec) || (gettime.it_interval.tv_nsec != timerspec1.it_interval.tv_nsec))
		{
		ERR_PRINTF1(_L("Failed to get the timer value") );
		goto close;
		}
	INFO_PRINTF1(_L("Successfully able to get the timer") );	
	ret = timer_delete(timerid);
	if(ret != 0)
		{
		ERR_PRINTF2(_L("Failed to delete the timer and errno is %d"),errno);
     	return -1;			
		}
	INFO_PRINTF1(_L("Successfully able to delete the timer") );
	ret1 = KErrNone;
	return ret1;
	close:
	timer_delete(timerid);
	return ret1;	
	}
TInt CTesttimer::Testtimerapi28( )
    {
    TInt err;

    TRequestStatus timerThreadStatus1;
    RThread timerThread1;
    TTimerTestThreadParams paramsTimerThread1(*this);

    TRequestStatus timerThreadStatus2;
    RThread timerThread2;
    TTimerTestThreadParams paramsTimerThread2(*this);

    Logger().ShareAuto();
    //Create the Thread to create shared mem and write to it.
    err = timerThread1.Create(_L("TimerThread1"), (TThreadFunction)CTimerTestThread::OEEntry, KDefaultStackSize, KMinHeapSize, 1024*1024,&paramsTimerThread1);
    if(err != KErrNone)
    {
        ERR_PRINTF2(_L("Timer Thread1 Not created - %d"), err);
        SetTestStepResult(EFail);
        return EFail;

    }
    INFO_PRINTF1(_L("Timer Thread1 created "));
    //Create the thread to open shared mem and read from it.
    err = timerThread2.Create(_L("TimerThread2"), (TThreadFunction)CTimerTestThread::OEEntry, KDefaultStackSize, KMinHeapSize, 1024*1024,&paramsTimerThread2);
    if(err != KErrNone)
    {
        ERR_PRINTF2(_L("Timer Thread 2 Not created - %d"), err);
        SetTestStepResult(EFail);
        //Close the Write Thread created previously
        timerThread1.Close();
        return EFail;

    }
    INFO_PRINTF1(_L("Timer Thread created "));

    timerThread1.SetPriority(EPriorityNormal);
    timerThread2.SetPriority(EPriorityNormal);

    timerThread1.Logon(timerThreadStatus1);
    timerThread2.Logon(timerThreadStatus2);

    timerThread1.Resume();
    timerThread2.Resume();

    User::WaitForRequest(timerThreadStatus1);
   User::WaitForRequest(timerThreadStatus2);

    timerThread1.Close();
    timerThread2.Close();

    //Check the status of the threads..
   if(!paramsTimerThread1.iTestResult )
        {
         ERR_PRINTF1(_L("Timer Thread 1 Not successful"));
         SetTestStepResult(EFail);
        }
   if(!paramsTimerThread2.iTestResult )
        {
          ERR_PRINTF1(_L("Timer Thread 2 Not successful"));
          SetTestStepResult(EFail);
        }

    return TestStepResult();


    }
//End of a file




